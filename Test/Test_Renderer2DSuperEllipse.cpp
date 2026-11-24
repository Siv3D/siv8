//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/SuperEllipseTessellation.hpp"
# include <ThirdParty/nanobench/nanobench.h>

namespace
{
	struct FrameSceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		FrameSceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 240, 200 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}

		~FrameSceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	struct FrameCapture
	{
		Image image;
		int64 triangles;
	};

	template <class Draw>
	FrameCapture Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return{ ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}

	double MaxContourDeviation(const detail::SuperEllipseTessellation::Quarter& quarter, const Vec2 axes,
		const double n, const int32 samples = 512)
	{
		// Independent angular reference, including samples very close to the axes
		// where large exponents concentrate curvature. Compare against every chord.
		double maxError = 0;
		const auto sample = [&](const Vec2 normalized)
		{
			const Vec2 point = (normalized * axes);
			double distanceSq = Math::Inf;
			for (size_t i = 1; i < quarter.count; ++i)
			{
				const Vec2 a = quarter.points[i - 1], d = (quarter.points[i] - a);
				const double t = Clamp((point - a).dot(d) / d.lengthSq(), 0.0, 1.0);
				distanceSq = Min(distanceSq, point.distanceFromSq(a + d * t));
			}
			maxError = Max(maxError, std::sqrt(distanceSq));
		};
		for (int32 i = 1; i < samples; ++i)
		{
			const double angle = (Math::HalfPi * i / samples);
			sample({ std::pow(std::cos(angle), 2 / n), std::pow(std::sin(angle), 2 / n) });
		}
		for (int32 i = 2; i <= 48; ++i)
		{
			const double angle = std::ldexp(1.0, -i);
			const Vec2 p{ std::pow(std::cos(angle), 2 / n), std::pow(std::sin(angle), 2 / n) };
			sample(p);
			sample({ p.y, p.x });
		}
		return maxError;
	}

	void DrawShape(const SuperEllipse& shape, const double thickness, const int32 mode)
	{
		const ColorF red{ 1, 0, 0, 0.5 }, blue{ 0, 0, 1, 0.5 };
		const Pattern::PolkaDot white{ .primary = Palette::White, .background = Palette::White };
		switch (mode)
		{
		case 0: CHECK(&shape.drawFrame(thickness, Palette::White) == &shape); break;
		case 1: CHECK(&shape.drawFrame(thickness, white) == &shape); break;
		case 2: CHECK(&shape.draw(Palette::White) == &shape); break;
		case 3: CHECK(&shape.draw(white) == &shape); break;
		case 4: CHECK(&shape.draw(red, blue) == &shape); break;
		case 5: CHECK(&shape.draw(Arg::top(red), Arg::bottom(blue)) == &shape); break;
		case 6: CHECK(&shape.draw(Arg::left(red), Arg::right(blue)) == &shape); break;
		}
	}
}

TEST_CASE("Renderer2D.superellipse_uniform_scaling")
{
	const FrameSceneScope scene;
	for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 })
	for (const double exponent : { 0.5, 1.0, 4.0, 32.0 })
	for (const double scale : { 0.5, 1.0, 4.0 })
	{
		CAPTURE(mode, exponent, scale);
		const auto actual = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(scale).translated(120, 100) };
			DrawShape(SuperEllipse{ 0, 0, 24, 16, exponent }, 4, mode);
		});
		// Scaling the geometry and stroke explicitly must select the same detail.
		const auto expected = Capture([&]
		{
			DrawShape(SuperEllipse{ 120, 100, 24 * scale, 16 * scale, exponent }, 4 * scale, mode);
		});
		CHECK(actual.triangles == expected.triangles);
		CHECK(actual.image == expected.image);
		CHECK(actual.triangles > 0);
	}
}

TEST_CASE("Renderer2D.superellipse_affine_rms")
{
	const FrameSceneScope scene;
	for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 })
	{
		CAPTURE(mode);
		const auto count = [&](const Mat3x2& local, const Mat3x2& camera)
		{
			return Capture([&]
			{
				const Transformer2D localScope{ local, Transformer2D::Target::SetLocal };
				const Transformer2D cameraScope{ camera.translated(120, 100), Transformer2D::Target::SetCamera };
				DrawShape(SuperEllipse{ 0, 0, 24, 16, 4 }, 4, mode);
			}).triangles;
		};
		const auto base = count(Mat3x2::Identity(), Mat3x2::Identity());
		const auto stretched = count(Mat3x2::Identity(), Mat3x2::Scale(4.0f, 1.0f));
		CHECK(stretched > base);
		CHECK(count(Mat3x2::Rotate(45_deg), Mat3x2::Scale(4.0f, 1.0f)) == stretched);
		CHECK(count(Mat3x2::Scale(4.0f, 1.0f), Mat3x2::Scale(0.25f, 1.0f)) == base);
	}
}

TEST_CASE("Renderer2D.superellipse_scene_resolution")
{
	const FrameSceneScope scene;
	const Size logical{ 240, 200 };
	for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 })
	{
		CAPTURE(mode);
		const auto draw = [&] { DrawShape(SuperEllipse{ 120, 100, 24, 16, 4 }, 4, mode); };
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::SceneSize));
		const auto base = Capture(draw);
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > logical.x);
		const auto highResolution = Capture(draw);
		CHECK(highResolution.triangles > base.triangles);
		CHECK(Graphics2D::GetRMSScaling() == 1.0f);
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const auto expected = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(Float2{ float(pixels.x) / logical.x, float(pixels.y) / logical.y }) };
			draw();
		});
		CHECK(highResolution.triangles == expected.triangles);
		CHECK(highResolution.image == expected.image);
	}
}

TEST_CASE("Renderer2D.superellipse_vertices_benchmark", "[.benchmark]")
{
	std::array<Vertex2D, detail::SuperEllipseTessellation::MaxPoints + 1> vertices;
	std::array<Vertex2D::IndexType, detail::SuperEllipseTessellation::MaxPoints * 3> indices;
	const auto allocate = [&](uint16, uint32) { return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 }; };
	ankerl::nanobench::Bench bench;
	bench.title("SuperEllipse fill vertices").unit("shape").epochs(11).warmup(10000).minEpochIterations(10000);
	for (const float radius : { 20.0f, 80.0f, 320.0f, 4000.0f, 16000.0f })
	for (const float n : { 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 32.0f })
	{
		const std::string name = ("a=" + std::to_string(static_cast<int32>(radius)) + ", n=" + std::to_string(n));
		bench.run(name, [&]
		{
			ankerl::nanobench::doNotOptimizeAway(Vertex2DBuilder::BuildSuperEllipse(allocate,
				Float2{ 0, 0 }, radius, radius * 0.75f, n, ColorFillDirection::InOut,
				Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1.0f));
		});
	}
}

TEST_CASE("Renderer2D.superellipse_tessellation_error")
{
	namespace Tess = detail::SuperEllipseTessellation;
	for (const double n : { 0.125, 0.25, 0.5, 0.9, 0.99, 1.0, 1.01, 1.1, 2.0, 4.0, 8.0, 32.0, 128.0 })
	for (const Vec2 axes : { Vec2{ 80, 80 }, Vec2{ 160, 20 }, Vec2{ 20, 160 }, Vec2{ 0.5, 0.25 } })
	for (const double scale : { 0.0, 0.5, 1.0, 4.0, 16.0 })
	{
		CAPTURE(n, axes, scale);
		Tess::Quarter quarter;
		Tess::Tessellate(quarter, axes.x, axes.y, n, scale);
		REQUIRE_FALSE(quarter.limited);
		REQUIRE(quarter.count >= 2);
		REQUIRE(quarter.count <= Tess::MaxOctantSegments * 2 + 1);
		CHECK(quarter.points[0] == Vec2{ axes.x, 0 });
		CHECK(quarter.points[quarter.count - 1] == Vec2{ 0, axes.y });
		if (n == 1.0) CHECK(quarter.count == 2);
		for (size_t i = 1; i < quarter.count; ++i)
		{
			CHECK(quarter.points[i].x <= quarter.points[i - 1].x);
			CHECK(quarter.points[i].y >= quarter.points[i - 1].y);
			CHECK(quarter.points[i] != quarter.points[i - 1]);
			const Vec2 p = (quarter.points[i] / axes);
			CHECK((std::pow(p.x, n) + std::pow(p.y, n)) == Catch::Approx(1.0).margin(1e-10));
		}

		const double maxError = (MaxContourDeviation(quarter, axes, n) * scale);
		CHECK(maxError <= Tess::TargetError + 1e-8);
	}
}

TEST_CASE("Renderer2D.superellipse_tessellation_contour_and_budget")
{
	namespace Tess = detail::SuperEllipseTessellation;
	Array<Vec2> contour;
	for (const double n : { 0.25, 0.5, 1.0, 2.0, 4.0, 32.0 })
	{
		CAPTURE(n);
		Tess::Quarter base, enlarged, wide, limited;
		Tess::Tessellate(base, 80, 60, n, 1);
		Tess::Tessellate(enlarged, 80, 60, n, 4);
		Tess::Tessellate(wide, 80, 60, n, 1, 160);
		Tess::Tessellate(limited, 80, 60, n, 1e8);
		CHECK(enlarged.count >= base.count);
		CHECK(wide.count >= base.count);
		CHECK(limited.count <= Tess::MaxOctantSegments * 2 + 1);
		CHECK(limited.limited == (n != 1));
		Tess::WriteContour(contour, base, { 120, 100 });
		const size_t count = contour.size();
		REQUIRE(count == (base.count - 1) * 4);
		CHECK(contour[0] == Vec2{ 200, 100 });
		CHECK(contour[count / 4] == Vec2{ 120, 160 });
		CHECK(contour[count / 2] == Vec2{ 40, 100 });
		CHECK(contour[count * 3 / 4] == Vec2{ 120, 40 });
		for (size_t i = 0; i < count / 2; ++i)
		{
			CHECK((contour[i] + contour[i + count / 2]).distanceFrom(Vec2{ 240, 200 }) < 1e-10);
			CHECK(contour[i] != contour[(i + 1) % count]);
		}
	}
}

TEST_CASE("Renderer2D.superellipse_mesh_colors_and_failure")
{
	for (const float n : { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 32.0f })
	for (const float scale : { 4.0f, 10000.0f })
	for (const auto coloring : { ColorFillDirection::InOut, ColorFillDirection::TopBottom, ColorFillDirection::LeftRight })
	{
		CAPTURE(n, scale, static_cast<int32>(coloring));
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		const auto allocate = [&](uint16 v, uint32 i)
		{
			vertices.resize(v);
			indices.resize(i);
			return Vertex2DBufferPointer{ vertices.data(), indices.data(), 37 };
		};
		const auto build = [&](const BufferCreatorFunc& creator)
		{
			return Vertex2DBuilder::BuildSuperEllipse(creator, Float2{ 120, 100 }, 80, 60, n,
				coloring, Float4{ 1, 0, 0, 0.5f }, Float4{ 0, 0, 1, 0.5f }, scale);
		};
		const auto indexCount = build(allocate);
		CHECK(indexCount == indices.size());
		REQUIRE(vertices.size() >= 5);
		CHECK(vertices.size() <= 257);
		CHECK(indices.size() == (vertices.size() - 1) * 3);
		if (n == 1.0f) CHECK(vertices.size() == 5);
		CHECK(vertices[0].pos == Float2{ 120, 100 });
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			const auto& v = vertices[i];
			double blue = (i == 0 ? 0.0 : 1.0);
			if (coloring == ColorFillDirection::TopBottom) blue = ((v.pos.y - 40) / 120);
			if (coloring == ColorFillDirection::LeftRight) blue = ((v.pos.x - 40) / 160);
			CHECK(v.color.x == Catch::Approx(1 - blue).margin(1e-6));
			CHECK(v.color.z == Catch::Approx(blue).margin(1e-6));
			CHECK(v.color.y == 0);
			CHECK(v.color.w == 0.5f);
		}
		for (const auto index : indices) REQUIRE((37 <= index && index < 37 + vertices.size()));
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			const Vec2 a = vertices[indices[i] - 37].pos;
			const Vec2 b = vertices[indices[i + 1] - 37].pos;
			const Vec2 c = vertices[indices[i + 2] - 37].pos;
			CHECK((b - a).cross(c - a) > 0);
		}
		CHECK(build([](uint16, uint32) { return Vertex2DBufferPointer{ nullptr, nullptr, 0 }; }) == 0);
	}
}

TEST_CASE("Renderer2D.superellipse_generated_geometry_stays_fixed")
{
	const FrameSceneScope scene;
	for (const double n : { 0.5, 1.0, 4.0, 32.0 })
	{
		CAPTURE(n);
		const SuperEllipse shape{ 120, 100, 80, 60, n };
		const auto outer = shape.outer(PointsPerCircle{ 32 });
		const auto defaultOuter = shape.outer();
		const auto polygon = shape.asPolygon(PointsPerCircle{ 32 });
		const auto defaultPolygon = shape.asPolygon();
		CHECK(outer.size() == (n == 1 ? 4 : n < 1 ? 64 : 32));
		CHECK(defaultOuter.size() == (n == 1 ? 4 : n < 1 ? 112 : 56));
		REQUIRE_FALSE(polygon.isEmpty());
		REQUIRE_FALSE(defaultPolygon.isEmpty());
		if (n == 1)
		{
			CHECK(outer == Array<Vec2>{ { 120, 40 }, { 200, 100 }, { 120, 160 }, { 40, 100 } });
		}
		else
		{
			const double diagonal = std::pow(0.5, 1 / n);
			CHECK(outer[0].distanceFrom(Vec2{ 120, 40 }) < 1e-8);
			CHECK(outer[outer.size() / 8].distanceFrom(Vec2{ 120 + 80 * diagonal, 100 - 60 * diagonal }) < 1e-4);
		}
		Scene::Configure(SceneConfig::Fixed({ 240, 200 }, SceneResolution::Display));
		const Transformer2D local{ Mat3x2::Scale(3000, 2000).rotated(0.3f), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(0.5f), Transformer2D::Target::SetCamera };
		for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 }) DrawShape(shape, 4, mode);
		CHECK(shape.outer(PointsPerCircle{ 32 }) == outer);
		CHECK(shape.outer() == defaultOuter);
		const auto after = shape.asPolygon(PointsPerCircle{ 32 });
		const auto defaultAfter = shape.asPolygon();
		const auto checkPolygon = [](const Polygon& actual, const Polygon& expected)
		{
			CHECK(actual.outer() == expected.outer());
			CHECK(std::equal(actual.indices().begin(), actual.indices().end(),
				expected.indices().begin(), expected.indices().end(), [](const TriangleIndex a, const TriangleIndex b)
				{
					return ((a.i0 == b.i0) && (a.i1 == b.i1) && (a.i2 == b.i2));
				}));
		};
		checkPolygon(after, polygon);
		checkPolygon(defaultAfter, defaultPolygon);
	}
}

TEST_CASE("Renderer2D.superellipse_pixels_and_degenerate_shapes")
{
	const FrameSceneScope scene;
	for (const double n : { 0.5, 1.0, 2.0, 4.0, 32.0 })
	{
		CAPTURE(n);
		const SuperEllipse shape{ 120, 100, 80, 60, n };
		const auto solid = Capture([&] { shape.draw(ColorF{ 1.0, 0.5 }); });
		const Color interior = solid.image[100][120];
		CHECK(interior.r > 0);
		CHECK(interior.r < 255);
		CHECK(solid.image[10][10] == Palette::Black);
		// Conservative 3-pixel boxes keep samples away from the analytic edge.
		// Interior alpha must stay uniform even for concave quadrants (n < 1).
		for (int32 y = 30; y < 171; y += 3)
		for (int32 x = 30; x < 211; x += 3)
		{
			const double u = Abs(x + 0.5 - 120), v = Abs(y + 0.5 - 100);
			if ((std::pow((u + 3) / 80, n) + std::pow((v + 3) / 60, n)) < 1)
			{
				CHECK(solid.image[y][x] == interior);
			}
			else if ((std::pow(Max(0.0, u - 3) / 80, n) + std::pow(Max(0.0, v - 3) / 60, n)) > 1)
			{
				CHECK(solid.image[y][x] == Palette::Black);
			}
		}
		for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 })
		{
			CAPTURE(mode);
			const auto positive = Capture([&] { DrawShape(shape, 4, mode); });
			const auto negative = Capture([&] { DrawShape(SuperEllipse{ 120, 100, -80, -60, n }, 4, mode); });
			CHECK(positive.triangles == negative.triangles);
			CHECK(positive.image == negative.image);
		}
	}
	const auto empty = Capture([&]
	{
		for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 })
		{
			DrawShape(SuperEllipse{ 120, 100, 0, 60, 4 }, 4, mode);
			DrawShape(SuperEllipse{ 120, 100, 80, 0, 4 }, 4, mode);
			DrawShape(SuperEllipse{ 120, 100, 80, 60, 0 }, 4, mode);
			DrawShape(SuperEllipse{ 120, 100, 80, 60, -1 }, 4, mode);
		}
		SuperEllipse{ 120, 100, 80, 60, 4 }.drawFrame(0).drawFrame(-1);
	});
	CHECK(empty.triangles == 0);
	CHECK(empty.image[100][120] == Palette::Black);
}

TEST_CASE("Renderer2D.superellipse_budget_is_distributed")
{
	namespace Tess = detail::SuperEllipseTessellation;
	REQUIRE(Tess::MaxPoints == 256);
	for (const double n : { 0.125, 0.5, 0.99, 1.0, 1.01, 2.0, 4.0, 32.0, 128.0 })
	for (const Vec2 axes : { Vec2{ 80, 80 }, Vec2{ 160, 20 }, Vec2{ 20, 160 } })
	for (const double margin : { 0.0, 20.0 })
	{
		CAPTURE(n, axes, margin);
		Tess::Quarter saturated, enlarged;
		Tess::Tessellate(saturated, axes.x, axes.y, n, 1e6, margin);
		Tess::Tessellate(enlarged, axes.x, axes.y, n, 2e6, margin);
		REQUIRE(saturated.count == (n == 1 ? 2 : 65));
		CHECK(saturated.limited == (n != 1));
		REQUIRE(enlarged.count == saturated.count);
		for (size_t i = 0; i < saturated.count; ++i)
		{
			CHECK(enlarged.points[i] == saturated.points[i]);
		}

		// A coarse tail from depth-first exhaustion violates this whole-contour
		// bound. The reference is independent of the subdivision priority formula.
		const double circleDeviation = (Max(axes.x, axes.y) * (1 - std::cos(Math::Pi / 256)));
		CHECK(MaxContourDeviation(saturated, axes, n, 4096) <= (2 * circleDeviation));
	}

	Tess::Quarter circle;
	Tess::Tessellate(circle, 1, 1, 2, 1e6);
	REQUIRE(circle.count == 65);
	for (size_t i = 0; i < circle.count; ++i)
	{
		// Constant curvature must receive equal angles throughout the quadrant.
		const double angle = std::atan2(circle.points[i].y, circle.points[i].x);
		CHECK(angle == Catch::Approx(i * Math::HalfPi / 64).margin(1e-12));
	}
}

TEST_CASE("Renderer2D.superellipse_budget_rendering")
{
	const FrameSceneScope scene;
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	{
		Scene::Configure(SceneConfig::Fixed({ 240, 200 }, resolution));
		for (const int32 mode : { 0, 1, 2, 3, 4, 5, 6 })
		for (const double scale : { 512.0, 1024.0 })
		{
			CAPTURE(mode, scale, Scene::RenderResolution());
			const auto actual = Capture([&]
			{
				const Transformer2D transform{ Mat3x2::Scale(scale).translated(120, 100) };
				// Keep the right axis tip visible even when most of the shape is offscreen.
				DrawShape(SuperEllipse{ -80, 0, 80, 60, 4 }, 4, mode);
			});
			const auto expected = Capture([&]
			{
				DrawShape(SuperEllipse{ 120 - 80 * scale, 100, 80 * scale, 60 * scale, 4 }, 4 * scale, mode);
			});
			CHECK(actual.triangles == (mode < 2 ? 512 : 256));
			CHECK(actual.triangles == expected.triangles);
			CHECK(actual.image == expected.image);
		}
	}
}
