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
# include <ThirdParty/nanobench/nanobench.h>

namespace
{
	struct DashRange
	{
		size_t first;
		size_t points;
	};

	struct DashMesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		Array<DashRange> dashes;
	};

	DashMesh BuildMesh(const Float2 axes, const Float2 widths, const float offset,
		const float ratio, const uint32 count, const float scale = 1.0f)
	{
		DashMesh mesh;
		constexpr uint16 IndexOffset = 17;
		uint32 allocations = 0;
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++allocations;
			mesh.vertices.resize(vertices);
			mesh.indices.resize(indices);
			return Vertex2DBufferPointer{ mesh.vertices.data(), mesh.indices.data(), IndexOffset };
		};
		const auto indices = Vertex2DBuilder::BuildEllipseDashedFrame(allocate, { 0, 0 }, axes.x, axes.y,
			widths.x, widths.y, offset, ratio, count, Float4{ 1, 0, 0, 1 }, Float4{ 0, 0, 1, 1 }, scale);
		REQUIRE(indices == mesh.indices.size());
		CHECK(allocations == (indices ? 1 : 0));
		for (const auto index : mesh.indices)
		{
			REQUIRE(index >= IndexOffset);
			REQUIRE((index - IndexOffset) < mesh.vertices.size());
		}
		if ((0 < ratio) && (ratio < 1))
		{
			for (size_t i = 0; i < mesh.indices.size(); i += 6)
			{
				const size_t first = (mesh.indices[i] - IndexOffset);
				if ((i == 0) || (mesh.indices[i] != (mesh.indices[i - 6] + 2)))
				{
					mesh.dashes.push_back({ first, 2 });
				}
				else
				{
					++mesh.dashes.back().points;
				}
			}
		}
		return mesh;
	}

	Vec2 BoundaryPoint(const Vec2 axes, const double angle, const double width)
	{
		const Vec2 unit{ std::sin(angle), -std::cos(angle) };
		return (unit * axes + (unit / axes).normalized() * width);
	}

	double Parameter(const DashMesh& mesh, const size_t vertex, const Vec2 axes, const Vec2 widths)
	{
		const Vec2 centerline = ((Vec2{ mesh.vertices[vertex].pos } * widths.x
			+ Vec2{ mesh.vertices[vertex + 1].pos } * widths.y) / (widths.x + widths.y));
		return std::atan2(centerline.x / axes.x, -centerline.y / axes.y);
	}

	double BoundaryError(const DashMesh& mesh, const DashRange dash, const Vec2 axes,
		const Vec2 widths, const bool outer)
	{
		const double start = Parameter(mesh, dash.first, axes, widths);
		double end = Parameter(mesh, (dash.first + 2 * (dash.points - 1)), axes, widths);
		if (end < start)
		{
			end += Math::TwoPi;
		}
		double error = 0;
		for (int32 i = 0; i <= 128; ++i)
		{
			const Vec2 p = BoundaryPoint(axes, (start + (end - start) * i / 128), (outer ? widths.y : -widths.x));
			double distance = Math::Inf;
			for (size_t j = 1; j < dash.points; ++j)
			{
				const size_t index = (dash.first + (j - 1) * 2 + (outer ? 0 : 1));
				const Vec2 a = mesh.vertices[index].pos, b = mesh.vertices[index + 2].pos;
				distance = Min(distance, Line{ a, b }.closestPointTo(p).distanceFrom(p));
			}
			error = Max(error, distance);
		}
		return error;
	}

	struct ArcLengthReference
	{
		static constexpr size_t Steps = 65536;
		static constexpr double Step = (Math::TwoPi / Steps);
		Vec2 axes;
		Array<double> lengths;

		explicit ArcLengthReference(const Vec2 axes) : axes{ axes }, lengths(Steps + 1, 0.0)
		{
			// Integrate analytic speed over the full ellipse in double precision;
			// independent of the rendering table's chord sums and reflections.
			double previous = speed(0);
			for (size_t i = 1; i <= Steps; ++i)
			{
				const double next = speed(i * Step);
				lengths[i] = (lengths[i - 1] + Step / 6 * (previous + 4 * speed((i - 0.5) * Step) + next));
				previous = next;
			}
		}

		double speed(const double angle) const
		{
			return std::hypot(axes.x * std::cos(angle), axes.y * std::sin(angle));
		}

		double lengthAt(double angle) const
		{
			if (angle < 0) angle += Math::TwoPi;
			const size_t i = Min(static_cast<size_t>(angle / Step), (Steps - 1));
			const double begin = (i * Step), delta = (angle - begin);
			return (lengths[i] + delta / 6 * (speed(begin) + 4 * speed(begin + delta * 0.5) + speed(angle)));
		}
	};

	void CheckSameMesh(const DashMesh& actual, const DashMesh& expected)
	{
		REQUIRE(actual.vertices.size() == expected.vertices.size());
		CHECK(actual.indices == expected.indices);
		for (size_t i = 0; i < actual.vertices.size(); ++i)
		{
			CHECK(actual.vertices[i].pos == expected.vertices[i].pos);
			CHECK(actual.vertices[i].color == expected.vertices[i].color);
		}
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D colorMul{ Palette::White };
		ScopedColorAdd2D colorAdd{ 0.0 };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };

		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}

		~SceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	struct CaptureResult
	{
		Image image;
		int64 triangles;
	};

	template <class Draw>
	CaptureResult Capture(Draw&& draw)
	{
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return{ ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}

	void DrawFrame(const Ellipse& ellipse, const double width, const RectangularDashStyle& style, const int32 mode)
	{
		switch (mode)
		{
		case 0: CHECK(&ellipse.drawDashedFrame(width * 2, style) == &ellipse); break;
		case 1: CHECK(&ellipse.drawDashedFrame(width * 2, style, Palette::Red, Palette::Blue) == &ellipse); break;
		case 2: CHECK(&ellipse.drawDashedFrame(width, width, style) == &ellipse); break;
		case 3: CHECK(&ellipse.drawDashedFrame(width, width, style, Palette::Red, Palette::Blue) == &ellipse); break;
		}
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_short_counts")
{
	for (const uint32 count : { 32u, 64u, 256u })
	for (const float offset : { 0.0f, -1.3f, 503.0f })
	{
		CAPTURE(count, offset);
		const auto mesh = BuildMesh({ 80, 80 }, { 2, 2 }, offset, 0.5f, count);
		CHECK(mesh.vertices.size() == (count * 4));
		CHECK(mesh.indices.size() == (count * 6));
		REQUIRE(mesh.dashes.size() == count);
		for (size_t i = 0; i < mesh.dashes.size(); ++i)
		{
			const auto dash = mesh.dashes[i];
			CHECK(dash.points == 2);
			CHECK(BoundaryError(mesh, dash, { 80, 80 }, { 2, 2 }, true) < 0.11);
			for (size_t end = 0; end < 2; ++end)
			{
				const double angle = (offset / 80.0 + Math::TwoPi * (i + end * 0.5) / count);
				const auto& outer = mesh.vertices[dash.first + end * 2];
				const auto& inner = mesh.vertices[dash.first + end * 2 + 1];
				CHECK(Vec2{ outer.pos }.distanceFrom(BoundaryPoint({ 80, 80 }, angle, 2)) < 0.01);
				CHECK(Vec2{ inner.pos }.distanceFrom(BoundaryPoint({ 80, 80 }, angle, -2)) < 0.01);
				CHECK(outer.color == Float4{ 0, 0, 1, 1 });
				CHECK(inner.color == Float4{ 1, 0, 0, 1 });
			}
		}
	}
	// Longer dashes retain the existing sampling, and the full-circle detail remains bounded.
	CHECK(BuildMesh({ 80, 80 }, { 2, 2 }, 0, 0.5f, 12).indices.size() == (12 * 24));
	CHECK(BuildMesh({ 80, 80 }, { 2, 2 }, 0, 0.5f, 256, 1000).indices.size() == (256 * 6));
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_flattened_tips")
{
	for (const Float2 axes : { Float2{ 160, 8 }, Float2{ 8, 160 } })
	for (const Float2 widths : { Float2{ 1, 2 }, Float2{ 0, 12 }, Float2{ 6, 2 } })
	for (const float offset : { -2.5f, 0.0f, 2.5f })
	for (const float scale : { 0.5f, 1.0f, 4.0f })
	{
		CAPTURE(axes, widths, offset, scale);
		const auto mesh = BuildMesh(axes, widths, offset, 0.5f, 256, scale);
		REQUIRE(mesh.dashes.size() == 256);
		CHECK(mesh.indices.size() < (256 * 24));
		size_t refined = 0;
		for (const auto dash : mesh.dashes)
		{
			if (dash.points == 5)
			{
				++refined;
			}
			else
			{
				REQUIRE(dash.points == 2);
				// Independent analytic offset curves, including inward offsets beyond
				// the tip's radius of curvature. A blanket two-point rule fails here.
				CHECK(BoundaryError(mesh, dash, axes, widths, true) < 0.12);
				CHECK(BoundaryError(mesh, dash, axes, widths, false) < 0.12);
			}
		}
		CHECK(refined > 0);
		CHECK(refined < 64);
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_limits")
{
	for (const uint32 count : { 0u, 10922u, 10923u, 0xFFFFFFFFu })
	{
		CAPTURE(count);
		const auto mesh = BuildMesh({ 80, 80 }, { 2, 2 }, 0, 0.5f, count);
		CHECK(mesh.indices.size() == ((count == 10922) ? (10922 * 6) : 0));
	}
	// Mixed short/refined dashes must also respect the total index limit.
	CHECK(BuildMesh({ 160, 8 }, { 2, 2 }, 0, 0.5f, 10922).indices.isEmpty());
	CHECK(BuildMesh({ 80, 40 }, { 2, 2 }, 0, 0, 12).indices.isEmpty());
	CHECK(BuildMesh({ 80, 40 }, { 2, 2 }, 0, -1, 12).indices.isEmpty());
	CHECK(BuildMesh({ 80, 40 }, { 2, 2 }, 0, 1, 0xFFFFFFFFu).indices.size() == (56 * 6));
	CHECK(BuildMesh({ 80, 0 }, { 2, 2 }, 0, 0.5f, 12).indices.isEmpty());
	CHECK(BuildMesh({ 80, 40 }, { 0, 0 }, 0, 0.5f, 12).indices.isEmpty());
	const auto fail = [](uint16, uint32) { return Vertex2DBufferPointer{ nullptr, nullptr, 0 }; };
	CHECK(Vertex2DBuilder::BuildEllipseDashedFrame(fail, { 0, 0 }, 80, 40, 2, 2,
		0, 0.5f, 64, Float4{ 1, 1, 1, 1 }, Float4{ 1, 1, 1, 1 }, 1) == 0);
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_uniform_scaling")
{
	const SceneScope scene;
	for (const int32 mode : { 0, 1, 2, 3 })
	for (const Vec2 axes : { Vec2{ 32, 32 }, Vec2{ 48, 6 } })
	for (const double scale : { 0.5, 1.0, 4.0 })
	{
		CAPTURE(mode, axes, scale);
		const auto actual = Capture([&]
		{
			const Transformer2D transform{ Mat3x2::Scale(scale).translated(240, 140) };
			DrawFrame(Ellipse{ Vec2{ 0, 0 }, axes }, 2, { .dashCount = 32, .offset = -1.5 }, mode);
		});
		const auto expected = Capture([&]
		{
			DrawFrame(Ellipse{ Vec2{ 240, 140 }, axes * scale }, (2 * scale),
				{ .dashCount = 32, .offset = (-1.5 * scale) }, mode);
		});
		CHECK(actual.triangles > 0);
		CHECK(actual.triangles == expected.triangles);
		REQUIRE(actual.image.size() == expected.image.size());
		// Scaling local vertices and adding the center in the builder use different
		// float operation orders; gradient interpolation can round by one channel unit.
		int32 maximumDifference = 0;
		size_t differingPixels = 0;
		for (size_t i = 0; i < (static_cast<size_t>(actual.image.width()) * actual.image.height()); ++i)
		{
			const Color a = actual.image.data()[i], b = expected.image.data()[i];
			maximumDifference = Max(maximumDifference, Max(Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g), Abs(int32(a.b) - b.b)));
			maximumDifference = Max(maximumDifference, Abs(int32(a.a) - b.a));
			differingPixels += (a != b);
		}
		CAPTURE(maximumDifference, differingPixels);
		CHECK(maximumDifference <= 1);
		CHECK(actual.image[140][240] == Palette::Black);
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_affine_rms")
{
	const SceneScope scene;
	for (const int32 mode : { 0, 1, 2, 3 })
	{
		CAPTURE(mode);
		const auto capture = [&](const Mat3x2& local, const Mat3x2& camera)
		{
			return Capture([&]
			{
				const Transformer2D l{ local, Transformer2D::Target::SetLocal };
				const Transformer2D c{ camera.translated(240, 140), Transformer2D::Target::SetCamera };
				DrawFrame(Ellipse{ 0, 0, 32, 16 }, 2, { .dashCount = 16, .offset = 1.25 }, mode);
			});
		};
		const auto base = capture(Mat3x2::Identity(), Mat3x2::Identity());
		const auto stretched = capture(Mat3x2::Identity(), Mat3x2::Scale(4.0f, 1.0f));
		CHECK(stretched.triangles > base.triangles);
		CHECK(capture(Mat3x2::Rotate(45_deg), Mat3x2::Scale(4.0f, 1.0f)).triangles == stretched.triangles);
		const auto cancelled = capture(Mat3x2::Scale(4.0f, 1.0f), Mat3x2::Scale(0.25f, 1.0f));
		CHECK(cancelled.triangles == base.triangles);
		CHECK(cancelled.image == base.image);
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_scene_resolution")
{
	const SceneScope scene;
	const Size logical{ 480, 280 };
	for (const int32 mode : { 0, 1, 2, 3 })
	for (const Vec2 axes : { Vec2{ 32, 32 }, Vec2{ 64, 8 } })
	{
		CAPTURE(mode, axes);
		const auto draw = [&]
		{
			const Transformer2D local{ Mat3x2::Scale(1.5f).translated(240, 140), Transformer2D::Target::SetLocal };
			DrawFrame(Ellipse{ Vec2{ 0, 0 }, axes }, 3, { .dashCount = 32, .offset = -1.5 }, mode);
		};
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::SceneSize));
		const auto base = Capture(draw);
		Scene::Configure(SceneConfig::Fixed(logical, SceneResolution::Display));
		const Size pixels = Scene::RenderResolution();
		REQUIRE(pixels.x > logical.x);
		const auto highResolution = Capture(draw);
		CHECK(highResolution.triangles > base.triangles);
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const auto expected = Capture([&]
		{
			const Transformer2D camera{ Mat3x2::Scale(Float2{ float(pixels.x) / logical.x, float(pixels.y) / logical.y }),
				Transformer2D::Target::SetCamera };
			draw();
		});
		CHECK(highResolution.triangles == expected.triangles);
		CHECK(highResolution.image == expected.image);
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_arc_length_accuracy")
{
	for (const Float2 axes : { Float2{ 0.5f, 0.125f }, Float2{ 3, 3 }, Float2{ 80, 80 }, Float2{ 160, 20 },
		Float2{ 20, 160 }, Float2{ 500, 0.5f }, Float2{ 0.5f, 500 }, Float2{ 10000, 10 } })
	{
		const ArcLengthReference reference{ axes };
		const double perimeter = reference.lengths.back();
		for (const float scale : { 0.25f, 1.0f, 4.0f })
		for (const float offset : { 0.0f, -3.25f, static_cast<float>(perimeter - 0.125), static_cast<float>(-perimeter - 0.125) })
		for (const uint32 count : { 12u, 64u })
		{
			CAPTURE(axes, scale, offset, count);
			const auto mesh = BuildMesh(axes, { 0.25f, 0.25f }, offset, 0.5f, count, scale);
			REQUIRE(mesh.dashes.size() == count);
			double maxError = 0;
			for (size_t dashIndex = 0; dashIndex < count; ++dashIndex)
			{
				const auto dash = mesh.dashes[dashIndex];
				for (size_t i = 0; i < dash.points; ++i)
				{
					const double angle = Parameter(mesh, (dash.first + i * 2), axes, { 0.25, 0.25 });
					const double actual = reference.lengthAt(angle);
					double expected = std::fmod((offset + perimeter / count * (dashIndex + 0.5 * i / (dash.points - 1))), perimeter);
					if (expected < 0) expected += perimeter;
					const double difference = Abs(actual - expected);
					maxError = Max(maxError, (Min(difference, (perimeter - difference)) * scale));
				}
			}
			// At normal sizes, phase and equal-distance samples stay within 0.01
			// render pixels. Very large capped shapes also include float vertex error.
			CHECK(maxError < Max(0.01, (Max(axes.x, axes.y) * scale * 0.000005)));
		}
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_arc_length_reuse")
{
	struct Spec { Float2 axes; Float2 widths; float scale; float offset; uint32 count; };
	const std::array specs{
		Spec{ {80,20}, {1,2}, 0.25f, 0, 12 },
		Spec{ {80,20}, {1,2}, 4, 0, 12 }, // quality changes without an axis change
		Spec{ {80,20}, {1,80}, 1, -3.25f, 64 }, // width also affects table density
		Spec{ {80,20}, {1,2}, 1, 17, 64 },
		Spec{ {20,80}, {1,2}, 1, 17, 64 }, // swap axes
		Spec{ {80,30}, {1,2}, 1, 17, 64 },
		Spec{ {100,20}, {1,2}, 1, 17, 64 },
		Spec{ {100,20}, {1,2}, 1, -17, 12 }, // phase/count do not describe the table
	};
	const auto build = [](const Spec& s) { return BuildMesh(s.axes, s.widths, s.offset, 0.5f, s.count, s.scale); };
	Array<DashMesh> expected;
	for (const auto& spec : specs)
	{
		// Separate snapshots by enough unrelated shapes to replace all cache entries.
		for (int32 i = 0; i < 12; ++i) BuildMesh({ float(17 + i), float(5 + i) }, { 1, 1 }, 0, 0.5f, 3);
		expected << build(spec);
	}
	for (int32 round = 0; round < 3; ++round)
	for (size_t i = 0; i < specs.size(); ++i)
	{
		const size_t index = ((round % 2) ? (specs.size() - 1 - i) : i);
		CAPTURE(round, index);
		CheckSameMesh(build(specs[index]), expected[index]);
		CheckSameMesh(build(specs[index]), expected[index]); // immediate reuse
	}
}

TEST_CASE("Renderer2D.ellipse_dashed_frame_vertices_benchmark", "[.benchmark]")
{
	std::array<Vertex2D, 4096> vertices;
	std::array<Vertex2D::IndexType, 8192> indices;
	const auto allocate = [&](uint16, uint32) { return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 }; };
	const Float4 white{ 1, 1, 1, 1 };
	ankerl::nanobench::Bench bench;
	bench.title("Ellipse dashed frame vertices").unit("shape").epochs(7).warmup(2000).minEpochIterations(2000);
	for (const Float2 axes : { Float2{ 80, 80 }, Float2{ 160, 20 }, Float2{ 500, 2 } })
	for (const uint32 count : { 12u, 64u, 256u })
	for (const uint32 variants : { 1u, 4u, 64u })
	{
		// 1: same ellipse with moving phase; 4: alternating shapes; 64: cache misses.
		const std::string name = (std::to_string(static_cast<int32>(axes.x)) + "x" + std::to_string(static_cast<int32>(axes.y))
			+ "/dashes=" + std::to_string(count) + "/variants=" + std::to_string(variants));
		uint32 iteration = 0;
		bench.run(name, [&]
		{
			const float delta = ((iteration % variants) * 0.125f);
			const float offset = ((iteration++ % 32) * 0.125f);
			ankerl::nanobench::doNotOptimizeAway(Vertex2DBuilder::BuildEllipseDashedFrame(allocate, { 0, 0 },
				(axes.x + delta), axes.y, 2, 2, offset, 0.5f, count, white, white, 1));
			ankerl::nanobench::doNotOptimizeAway(vertices.data());
			ankerl::nanobench::doNotOptimizeAway(indices.data());
		});
	}
}
