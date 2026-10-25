//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <cmath>

namespace
{
	const ColorF First{ 0.9, 0.3, 0.1, 0.4 }, Last{ 0.1, 0.4, 0.9, 0.8 };
	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope() { Scene::Configure(config); Scene::SetBackground(background); }
	};
	struct Frame { Image image; int64 triangles; };
	template <class Draw> Frame Capture(Draw&& draw)
	{
		REQUIRE(System::Update()); draw(); ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update()); REQUIRE(ScreenCapture::HasNewFrame());
		return { ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}
	void CheckImage(const Image& actual, const Image& expected)
	{
		REQUIRE(actual.size() == expected.size());
		// Geometry evaluated before or inside the VS can differ at an AA edge.
		size_t different = 0;
		uint64 total = 0;
		for (size_t i = 0; i < actual.pixelCount(); ++i)
		{
			const auto a = actual.data()[i], b = expected.data()[i];
			const int32 delta = Abs(int32(a.r) - b.r) + Abs(int32(a.g) - b.g) + Abs(int32(a.b) - b.b) + Abs(int32(a.a) - b.a);
			different += (delta > 4); total += delta;
		}
		CHECK(different < 32);
		CHECK(total < 2048);
	}
	struct Mesh { Array<Vertex2D> vertices; Array<Vertex2D::IndexType> indices; };
	template <class Build> Mesh BuildMesh(Build&& build)
	{
		Mesh mesh;
		Vertex2D guard; guard.set(-123, -456, -7, -8, { -1, -2, -3, -4 });
		uint32 requests = 0;
		const auto allocate = [&](uint16 vc, uint32 ic)
		{
			++requests;
			mesh.vertices.assign(vc + size_t{ 2 }, guard);
			mesh.indices.assign(ic + size_t{ 2 }, Vertex2D::IndexType{ 65535 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, 37 };
		};
		const auto count = build(allocate);
		if (requests == 0) { CHECK(count == 0); return mesh; }
		CHECK(requests == 1);
		CHECK(mesh.indices.size() == count + size_t{ 2 });
		for (const auto* vertex : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(vertex->pos == guard.pos); CHECK(vertex->tex == guard.tex); CHECK(vertex->color == guard.color);
		}
		CHECK(mesh.indices.front() == 65535); CHECK(mesh.indices.back() == 65535);
		mesh.vertices.pop_back(); mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back(); mesh.indices.erase(mesh.indices.begin());
		for (const auto index : mesh.indices) REQUIRE((37 <= index && index < 37 + mesh.vertices.size()));
		for (const auto& vertex : mesh.vertices) CHECK(vertex.tex == Float2{ -7, -8 });
		return mesh;
	}
	void CheckPosition(const Float2 p, const Vec2 expected)
	{
		CHECK(p.x == Catch::Approx(expected.x).margin(0.0002));
		CHECK(p.y == Catch::Approx(expected.y).margin(0.0002));
	}
	void DrawRotated(const Ellipse& e, double rotation, int32 paint)
	{
		const Pattern::PolkaDot pattern{ .primary = First, .background = Last };
		switch (paint)
		{
		case 0: CHECK(&e.drawRotated(rotation, First) == &e); break;
		case 1: e.drawRotated(rotation, First, Last); break;
		case 2: e.drawRotated(rotation, Arg::top = First, Arg::bottom = Last); break;
		case 3: e.drawRotated(rotation, Arg::left = First, Arg::right = Last); break;
		case 4: e.drawRotated(rotation, pattern); break;
		case 5: CHECK(&e.drawFrameRotated(rotation, 8, First) == &e); break;
		case 6: e.drawFrameRotated(rotation, 8, First, Last); break;
		case 7: e.drawFrameRotated(rotation, 3, 5, First); break;
		case 8: e.drawFrameRotated(rotation, 3, 5, First, Last); break;
		case 9: e.drawFrameRotated(rotation, 8, pattern); break;
		case 10: e.drawFrameRotated(rotation, 3, 5, pattern); break;
		}
	}
	void DrawOriginal(const Ellipse& e, int32 paint)
	{
		const Pattern::PolkaDot pattern{ .primary = First, .background = Last };
		switch (paint)
		{
		case 0: e.draw(First); break; case 1: e.draw(First, Last); break;
		case 2: e.draw(Arg::top = First, Arg::bottom = Last); break;
		case 3: e.draw(Arg::left = First, Arg::right = Last); break;
		case 4: e.draw(pattern); break; case 5: e.drawFrame(8, First); break;
		case 6: e.drawFrame(8, First, Last); break; case 7: e.drawFrame(3, 5, First); break;
		case 8: e.drawFrame(3, 5, First, Last); break; case 9: e.drawFrame(8, pattern); break;
		case 10: e.drawFrame(3, 5, pattern); break;
		}
	}
}

TEST_CASE("Renderer2D.ellipse_rotated_vertices")
{
	const Float2 center{ 17, -23 };
	for (const float rotation : { 0.0f, 0.3f, -1.2f, Math::HalfPiF })
	for (const float scale : { 0.0f, 1.0f, 64.0f })
	for (const auto direction : { ColorFillDirection::InOut, ColorFillDirection::TopBottom, ColorFillDirection::LeftRight })
	for (const bool frame : { false, true })
	{
		CAPTURE(rotation, scale, static_cast<int32>(direction), frame);
		const auto build = [&](bool rotated)
		{
			return BuildMesh([&](const BufferCreatorFunc& allocate)
			{
				using namespace Vertex2DBuilder;
				if (frame)
				{
					if (rotated) return BuildEllipseFrameRotated(allocate, center, 60, 20, rotation, 3, 5, First.toFloat4(), Last.toFloat4(), scale);
					return BuildEllipseFrame(allocate, center, 60, 20, 3, 5, First.toFloat4(), Last.toFloat4(), scale);
				}
				if (rotated) return BuildEllipseRotated(allocate, center, 60, 20, rotation, direction, First.toFloat4(), Last.toFloat4(), scale);
				return BuildEllipse(allocate, center, 60, 20, direction, First.toFloat4(), Last.toFloat4(), scale);
			});
		};
		const auto original = build(false), rotated = build(true);
		REQUIRE(rotated.vertices.size() == original.vertices.size()); CHECK(rotated.indices == original.indices);
		for (size_t i = 0; i < original.vertices.size(); ++i)
		{
			const Vec2 p = (Vec2{ original.vertices[i].pos } - center);
			CheckPosition(rotated.vertices[i].pos, Vec2{ center } + Vec2{ p.x * std::cos(rotation) - p.y * std::sin(rotation), p.x * std::sin(rotation) + p.y * std::cos(rotation) });
			CHECK(rotated.vertices[i].color == original.vertices[i].color);
		}
	}
	const auto fail = [](uint16, uint32) { return Vertex2DBufferPointer{ nullptr, nullptr, 0 }; };
	CHECK(Vertex2DBuilder::BuildEllipseRotated(fail, center, 60, 20, 0.3f, ColorFillDirection::InOut, First.toFloat4(), Last.toFloat4(), 1) == 0);
	CHECK(Vertex2DBuilder::BuildEllipseFrameRotated(fail, center, 60, 20, 0.3f, 3, 5, First.toFloat4(), Last.toFloat4(), 1) == 0);
}

TEST_CASE("Renderer2D.ellipse_rotated_public_and_resolution")
{
	const SceneScope scene;
	for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
	{
		Scene::Configure(SceneConfig::Fixed({ 480, 280 }, resolution));
		const Transformer2D camera{ Mat3x2::Scale(1.5f, 0.75f), Transformer2D::Target::SetCamera };
		for (int32 paint = 0; paint < 11; ++paint)
		{
			CAPTURE(static_cast<int32>(resolution), paint);
			const Ellipse e{ 140, 150, -60, 20 };
			const auto zero = Capture([&] { DrawRotated(e, 0, paint); });
			const auto original = Capture([&] { DrawOriginal(e, paint); });
			CHECK(zero.triangles == original.triangles); CheckImage(zero.image, original.image);
			if (paint == 4 || paint >= 9) continue; // Pattern coordinates remain in the existing pattern space.
			const auto rotated = Capture([&] { DrawRotated(e, 0.6, paint); });
			const auto transformed = Capture([&]
			{
				const Transformer2D local{ Mat3x2::Rotate(0.6, e.center), Transformer2D::Target::SetLocal };
				DrawOriginal(e, paint);
			});
			CHECK(rotated.triangles == transformed.triangles); CheckImage(rotated.image, transformed.image);
		}
	}
	const auto empty = Capture([&]
	{
		Ellipse{ 100, 100, 0, 20 }.drawRotated(0.3);
		Ellipse{ 100, 100, 60, 0 }.drawFrameRotated(0.3, 8);
		Ellipse{ 100, 100, 60, 20 }.drawFrameRotated(0.3, 0);
	});
	CHECK(empty.triangles == 0);
}

TEST_CASE("Renderer2D.ellipse_arc_geometry_and_budget")
{
	for (const Float2 axes : { Float2{ 60, 20 }, Float2{ 100, 2 }, Float2{ 2, 100 } })
	for (const float sweep : { -Math::TwoPiF, -0.01f, 0.01f, Math::PiF, Math::TwoPiF })
	for (const float scale : { 1.0f, 64.0f })
	{
		CAPTURE(axes, sweep, scale);
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildEllipseArc(allocate, LineCap::Flat, LineCap::Flat, { 0, 0 }, axes.x, axes.y, 1.56f, sweep,
				0.01f, 4, ColorFillDirection::InOut, First.toFloat4(), Last.toFloat4(), scale);
		});
		REQUIRE(mesh.vertices.size() >= 4); CHECK(mesh.vertices.size() <= 506);
		CHECK(mesh.indices.size() == (mesh.vertices.size() / 2 - 1) * 6);
		for (size_t i = 0; i < mesh.vertices.size(); i += 2)
		{
			const Vec2 inner = mesh.vertices[i].pos, outer = mesh.vertices[i + 1].pos;
			const Vec2 normal = ((outer - inner) / 4.01);
			const Vec2 base = (inner + normal * 0.01);
			CHECK(normal.length() == Catch::Approx(1.0).margin(0.00002));
			CHECK((base / axes).lengthSq() == Catch::Approx(1.0).margin(0.00002));
			const Vec2 analytic = Vec2{ base.x / (axes.x * axes.x), base.y / (axes.y * axes.y) }.normalized();
			CHECK(normal.distanceFrom(analytic) < 0.00003);
			CHECK(mesh.vertices[i].color == First.toFloat4()); CHECK(mesh.vertices[i + 1].color == Last.toFloat4());
		}
		const auto endpoint = [&](double t) { return Vec2{ axes.x * std::sin(t), -axes.y * std::cos(t) }; };
		for (const auto [i, t] : { std::pair{ size_t{ 0 }, double(1.56f + Min(sweep, 0.0f)) }, std::pair{ mesh.vertices.size() - 2, double(1.56f + Max(sweep, 0.0f)) } })
		{
			const Vec2 inner = mesh.vertices[i].pos, outer = mesh.vertices[i + 1].pos;
			CHECK((inner + (outer - inner) * (0.01 / 4.01)).distanceFrom(endpoint(t)) < 0.0002);
		}
	}
}

TEST_CASE("Renderer2D.ellipse_arc_caps_and_failure")
{
	for (const auto start : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const auto end : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const float sweep : { -1.2f, 0.0f, 1.2f, Math::TwoPiF })
	{
		CAPTURE(static_cast<int32>(start), static_cast<int32>(end), sweep);
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildEllipseArc(allocate, start, end, { 0, 0 }, 60, 20, 0.3f, sweep, 3, 5,
				ColorFillDirection::LeftRight, First.toFloat4(), Last.toFloat4(), 2);
		});
		if (sweep == 0 && start == LineCap::Flat && end == LineCap::Flat) CHECK(mesh.indices.empty());
		for (const auto& v : mesh.vertices)
		{
			CHECK(std::isfinite(v.pos.x)); CHECK(std::isfinite(v.pos.y));
			CHECK(v.color.w >= 0.39999f); CHECK(v.color.w <= 0.80001f);
		}
		const auto fail = [](uint16, uint32) { return Vertex2DBufferPointer{ nullptr, nullptr, 0 }; };
		CHECK(Vertex2DBuilder::BuildEllipseArc(fail, start, end, { 0, 0 }, 60, 20, 0.3f, sweep, 3, 5,
			ColorFillDirection::InOut, First.toFloat4(), Last.toFloat4(), 2) == 0);
	}
}

TEST_CASE("Renderer2D.ellipse_arc_zero_cap_geometry_and_colors")
{
	for (const float a : { 20.0f, 60.0f }) // Includes the circular fast path.
	for (const auto start : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const auto end : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const auto coloring : { ColorFillDirection::InOut, ColorFillDirection::LeftRight })
	{
		CAPTURE(a, static_cast<int32>(start), static_cast<int32>(end), static_cast<int32>(coloring));
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildEllipseArc(allocate, start, end, { 0, 0 }, a, 20, 0.3f, 0, 3, 5,
				coloring, First.toFloat4(), Last.toFloat4(), 64);
		});
		const Vec2 base{ a * std::sin(double(0.3f)), -20 * std::cos(double(0.3f)) };
		const Vec2 normal = Vec2{ base.x / (a * a), base.y / 400 }.normalized();
		const Vec2 tangent{ -normal.y, normal.x };
		const Vec2 center = (base + normal);
		const double firstLength = (start == LineCap::Flat ? 0 : 4);
		const double lastLength = (end == LineCap::Flat ? 0 : 4);
		for (const auto& vertex : mesh.vertices)
		{
			const Vec2 delta = (Vec2{ vertex.pos } - center);
			const double u = delta.dot(tangent), v = delta.dot(normal);
			CHECK(u >= -firstLength - 0.0001); CHECK(u <= lastLength + 0.0001);
			CHECK(Abs(v) <= 4.0001);
			if ((u < -0.0001 && start == LineCap::Round) || (u > 0.0001 && end == LineCap::Round))
			{
				CHECK(delta.length() == Catch::Approx(4).margin(0.0001));
			}
			const double t = (coloring == ColorFillDirection::InOut ? (v + 4) / 8 : (u + firstLength) / (firstLength + lastLength));
			const Float4 expected = First.lerp(Last, t).toFloat4();
			CHECK((vertex.color - expected).length() < 0.00005f);
		}
		double area = 0;
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const Vec2 p = mesh.vertices[mesh.indices[i] - 37].pos;
			const Vec2 q = mesh.vertices[mesh.indices[i + 1] - 37].pos;
			const Vec2 r = mesh.vertices[mesh.indices[i + 2] - 37].pos;
			const double twiceArea = (q - p).cross(r - p);
			CHECK(twiceArea >= -0.0001);
			area += twiceArea * 0.5;
		}
		const auto capArea = [](LineCap cap) { return cap == LineCap::Flat ? 0.0 : cap == LineCap::Square ? 32.0 : 8 * Math::Pi; };
		CHECK(area == Catch::Approx(capArea(start) + capArea(end)).margin(0.1));
	}
}

TEST_CASE("Renderer2D.ellipse_arc_gradient_length")
{
	for (const float sweep : { -4.5f, 4.5f, Math::TwoPiF })
	{
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildEllipseArc(allocate, LineCap::Flat, LineCap::Flat, { 0, 0 }, 60, 20, 0.3f, sweep,
				3, 5, ColorFillDirection::LeftRight, First.toFloat4(), Last.toFloat4(), 2);
		});
		Array<double> lengths{ 0.0 };
		Vec2 previous = (Vec2{ mesh.vertices[0].pos } + mesh.vertices[1].pos) * 0.5;
		for (size_t i = 2; i < mesh.vertices.size(); i += 2)
		{
			const Vec2 current = (Vec2{ mesh.vertices[i].pos } + mesh.vertices[i + 1].pos) * 0.5;
			lengths.push_back(lengths.back() + current.distanceFrom(previous));
			previous = current;
		}
		for (size_t i = 0; i < lengths.size(); ++i)
		{
			const double t = lengths[i] / lengths.back();
			const Float4 expected = First.lerp(Last, sweep < 0 ? 1 - t : t).toFloat4();
			CHECK((mesh.vertices[i * 2].color - expected).length() < 0.00001f);
			CHECK(mesh.vertices[i * 2].color == mesh.vertices[i * 2 + 1].color);
		}
	}
}

TEST_CASE("Renderer2D.ellipse_arc_flat_tip_quality")
{
	for (const Float2 axes : { Float2{ 100, 2 }, Float2{ 2, 100 } })
	for (const float scale : { 1.0f, 64.0f })
	{
		const auto mesh = BuildMesh([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildEllipseArc(allocate, LineCap::Flat, LineCap::Flat, { 0, 0 }, axes.x, axes.y, 0, Math::TwoPiF,
				0, 8, ColorFillDirection::InOut, First.toFloat4(), Last.toFloat4(), scale);
		});
		double maximumError = 0;
		for (int32 sample = 0; sample <= 4096; ++sample)
		{
			const double t = sample * Math::TwoPi / 4096;
			const Vec2 base{ axes.x * std::sin(t), -axes.y * std::cos(t) };
			const Vec2 normal = Vec2{ base.x / (axes.x * axes.x), base.y / (axes.y * axes.y) }.normalized();
			const Vec2 expected = (base + normal * 8);
			double distanceSq = Math::Inf;
			for (size_t i = 1; i + 2 < mesh.vertices.size(); i += 2)
			{
				const Vec2 p = mesh.vertices[i].pos, delta = (Vec2{ mesh.vertices[i + 2].pos } - p);
				const double f = (delta.lengthSq() > 0 ? Clamp((expected - p).dot(delta) / delta.lengthSq(), 0.0, 1.0) : 0.0);
				distanceSq = Min(distanceSq, expected.distanceFromSq(p + delta * f));
			}
			maximumError = Max(maximumError, std::sqrt(distanceSq) * scale);
		}
		CAPTURE(axes, scale, maximumError);
		CHECK(maximumError < 0.5);
		CHECK(mesh.vertices.size() <= 506);
	}
}

TEST_CASE("Renderer2D.ellipse_arc_public_angles_and_overloads")
{
	const SceneScope scene;
	const Ellipse ellipse{ 240, 140, 80, 40 };
	const Pattern::PolkaDot pattern{ .primary = First, .background = Last };
	for (int32 paint = 0; paint < 4; ++paint)
	for (int32 variant = 0; variant < 3; ++variant)
	{
		const auto draw = [&](int32 v, double sweep)
		{
			if (v == 0)
			{
				if (paint == 0) CHECK(&ellipse.drawArc(0, sweep, 3, 5, First) == &ellipse);
				else if (paint == 1) ellipse.drawArc(0, sweep, 3, 5, First, Last);
				else if (paint == 2) ellipse.drawArc(0, sweep, 3, 5, Arg::start = First, Arg::end = Last);
				else ellipse.drawArc(0, sweep, 3, 5, pattern);
			}
			else if (v == 1)
			{
				if (paint == 0) ellipse.drawArc(LineCap::Flat, 0, sweep, 3, 5, First);
				else if (paint == 1) ellipse.drawArc(LineCap::Flat, 0, sweep, 3, 5, First, Last);
				else if (paint == 2) ellipse.drawArc(LineCap::Flat, 0, sweep, 3, 5, Arg::start = First, Arg::end = Last);
				else ellipse.drawArc(LineCap::Flat, 0, sweep, 3, 5, pattern);
			}
			else
			{
				if (paint == 0) ellipse.drawArc(LineCap::Flat, LineCap::Flat, 0, sweep, 3, 5, First);
				else if (paint == 1) ellipse.drawArc(LineCap::Flat, LineCap::Flat, 0, sweep, 3, 5, First, Last);
				else if (paint == 2) ellipse.drawArc(LineCap::Flat, LineCap::Flat, 0, sweep, 3, 5, Arg::start = First, Arg::end = Last);
				else ellipse.drawArc(LineCap::Flat, LineCap::Flat, 0, sweep, 3, 5, pattern);
			}
		};
		const auto actual = Capture([&] { draw(variant, Math::HalfPi); });
		const auto expected = Capture([&] { draw(0, Math::HalfPi); });
		CHECK(actual.image == expected.image); CHECK(actual.triangles == expected.triangles);
		CHECK(actual.image[140][160] == Palette::Black);
		CHECK(actual.image[140][240] == Palette::Black);
		if (paint != 2)
		{
			const auto full = Capture([&] { draw(variant, -10); });
			const auto frame = Capture([&]
			{
				if (paint == 0) ellipse.drawFrame(3, 5, First);
				else if (paint == 1) ellipse.drawFrame(3, 5, First, Last);
				else ellipse.drawFrame(3, 5, pattern);
			});
			CHECK(full.image == frame.image); CHECK(full.triangles == frame.triangles);
		}
	}
	// The angle is the ray direction, not the ellipse parameter.
	const auto ray = Capture([&] { ellipse.drawArc(0, 45_deg, 3, 5, Palette::White); });
	CHECK(ray.image[103][268] != Palette::Black);
	CHECK(ray.image[112][297] == Palette::Black);
}

TEST_CASE("Renderer2D.ellipse_arc_transform_resolution")
{
	const SceneScope scene;
	for (const double sweep : { -1.2, 0.0, 1.2 })
	{
		const auto draw = [&]
		{
			const Transformer2D local{ Mat3x2::Scale(1.5f, 0.75f).rotated(0.3).translated(240, 140), Transformer2D::Target::SetLocal };
			Ellipse{ 0, 0, 60, 20 }.drawArc(LineCap::Round, LineCap::Square, 0.3, sweep, 3, 5, Arg::start = First, Arg::end = Last);
			Circle{ 0, 0, 40 }.drawArc(LineCap::Square, LineCap::Round, 0.3, sweep, 3, 5, First);
		};
		Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::Display));
		const auto high = Capture(draw);
		const Size pixels = Scene::RenderResolution();
		Scene::Configure(SceneConfig::Fixed(pixels, SceneResolution::SceneSize));
		const auto expected = Capture([&]
		{
			const Transformer2D camera{ Mat3x2::Scale(Float2{ float(pixels.x) / 480, float(pixels.y) / 280 }), Transformer2D::Target::SetCamera };
			draw();
		});
		CHECK(high.triangles == expected.triangles); CHECK(high.image == expected.image);
	}
}
