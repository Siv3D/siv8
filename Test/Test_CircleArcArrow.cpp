//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/CircleArcArrow.hpp"
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# include <time.h>
# endif

namespace
{
	using namespace s3d::detail::CircleArcArrow;
	using Index = Vertex2D::IndexType;
	constexpr Index Offset = 37;
	const Float4 Ink{ 0.3f, 0.7f, 1.0f, 0.5f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
		bool omitted = false;
	};

	Mesh CaptureMesh(const Placement& placement, const float thickness, const double extraWidth, const LineCap cap, const float scale)
	{
		Mesh mesh;
		Vertex2D sentinel;
		sentinel.set(-1000, -2000, -3, -4, Float4{ -1, -2, -3, -4 });
		uint32 allocations = 0;
		const auto allocate = [&](const uint16 vc, const uint32 ic)
		{
			++allocations;
			mesh.vertices.assign(vc + size_t{ 2 }, sentinel);
			mesh.indices.assign(ic + size_t{ 2 }, Index{ 65535 });
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, Offset };
		};
		Array<Vec2> points;
		WritePoints(points, placement, thickness, scale);
		const auto result = Vertex2DBuilder::BuildLineStringArrow(allocate, cap, points, placement.tip, thickness,
			(thickness + static_cast<float>(extraWidth * placement.sizeScale)), Ink, scale);
		mesh.omitted = not result;
		uint32 count = result.value_or(0);
		if (mesh.omitted)
		{
			WritePoints(points, placement, thickness, scale, true);
			count = Vertex2DBuilder::BuildLineString(allocate, cap, LineCap::Flat, points, none, thickness, false, CloseRing::No, Ink, scale);
		}
		REQUIRE(allocations == 1);
		REQUIRE(mesh.indices.size() == count + size_t{ 2 });
		CHECK(mesh.indices.front() == 65535);
		CHECK(mesh.indices.back() == 65535);
		for (const auto* vertex : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(vertex->pos == sentinel.pos);
			CHECK(vertex->color == sentinel.color);
		}
		for (const auto& vertex : mesh.vertices) CHECK(vertex.tex == sentinel.tex);
		mesh.vertices.pop_back(); mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back(); mesh.indices.erase(mesh.indices.begin());
		for (const auto index : mesh.indices) CHECK((Offset <= index && index < Offset + mesh.vertices.size()));
		for (const auto& vertex : mesh.vertices)
		{
			CHECK(std::isfinite(vertex.pos.x)); CHECK(std::isfinite(vertex.pos.y));
			CHECK(vertex.color == Ink);
		}
		return mesh;
	}

	struct SceneScope
	{
		const SceneConfig config = Scene::GetConfig();
		const ColorF background = Scene::GetBackground();
		const bool vsync = Graphics::IsVSyncEnabled();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedRenderStates2D blend{ BlendState::Default2D };
		ScopedColorMul2D mul{ Palette::White };
		ScopedColorAdd2D add{ 0.0 };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		SceneScope()
		{
			Scene::Configure(SceneConfig::Fixed({ 480, 360 }));
			Scene::SetBackground(Palette::Black);
		}
		~SceneScope()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
			Graphics::SetVSyncEnabled(vsync);
		}
	};

	template <class Draw>
	Image Capture(Draw&& draw)
	{
		REQUIRE(System::Update()); draw(); ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update()); REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("CircleArcArrow.placement_chords_and_last_root")
{
	for (const double radius : { -100.0, 100.0 })
	for (const double start : { -1.7, 0.0, 0.4, 17.0 })
	for (const double angle : { -Math::TwoPi, -4.7, -Math::Pi, -0.1, 0.000001, 0.1, Math::Pi, 4.7, Math::TwoPi })
	for (const double length : { 0.00001, 32.0, 140.0, 200.0, 400.0 })
	{
		CAPTURE(radius, start, angle, length);
		const Circle circle{ 35, -17, radius };
		const auto p = Place(circle, start, angle, length);
		REQUIRE(p);
		CHECK(p->start.distanceFrom(circle.center) == Catch::Approx(100).margin(1e-10));
		CHECK(p->base.distanceFrom(circle.center) == Catch::Approx(100).margin(1e-10));
		CHECK(p->base.distanceFrom(p->tip) == Catch::Approx(length * p->sizeScale).margin(1e-9));
		CHECK(Abs(p->shaftSweep) <= Abs(angle));
		CHECK(Abs(p->sweep - p->shaftSweep) <= Math::Pi + 1e-12);
		CHECK((0 < p->sizeScale && p->sizeScale <= 1));
		if (Abs(angle) == Math::TwoPi) CHECK(p->start == p->tip);
		// Every later point is closer to the tip: the chosen root is the last.
		for (int32 i = 1; i <= 8; ++i)
		{
			const double a = (start + p->shaftSweep + (angle - p->shaftSweep) * (i / 8.0));
			const Vec2 point = circle.center + Vec2{ std::sin(a) * 100, -std::cos(a) * 100 };
			CHECK(point.distanceFrom(p->tip) <= length * p->sizeScale + 1e-9);
		}
	}
}

TEST_CASE("CircleArcArrow.shrinking_half_turn_and_empty")
{
	const Circle circle{ 0, 0, 100 };
	for (const double sign : { -1.0, 1.0 })
	{
		const auto shortArc = Place(circle, 0, sign * Math::HalfPi, 400);
		REQUIRE(shortArc);
		CHECK(shortArc->base == shortArc->start);
		CHECK(shortArc->shaftSweep == 0);
		CHECK(shortArc->sizeScale == Catch::Approx(std::sqrt(2.0) / 4));
		for (const double angle : { Math::Pi - 1e-6, Math::Pi, Math::Pi + 1e-6, 1.5 * Math::Pi, Math::TwoPi })
		{
			const auto p = Place(circle, 0, sign * angle, 400);
			REQUIRE(p);
			CHECK(Abs(p->shaftSweep) == Catch::Approx(Max(angle - Math::Pi, 0.0)).margin(1e-12));
			if (Math::Pi <= angle) CHECK(p->sizeScale == 0.5);
		}
		const auto one = Place(circle, 0.3, sign * Math::TwoPi, 30);
		const auto many = Place(circle, 0.3, sign * 20 * Math::Pi, 30);
		REQUIRE(one); REQUIRE(many);
		CHECK(one->tip == many->tip); CHECK(one->base == many->base);
		CHECK(one->shaftSweep == many->shaftSweep);
	}
	CHECK_FALSE(Place(Circle{ 0, 0, 0 }, 1, 2, 10));
	CHECK_FALSE(Place(circle, 1, 0, 10));
}

TEST_CASE("CircleArcArrow.point_budget_reuse_and_exact_endpoints")
{
	Array<Vec2> points;
	points.reserve(253);
	const Vec2* const storage = points.data();
	for (const double radius : { 1.0, 100.0, 1000.0 })
	for (const float scale : { 0.25f, 1.0f, 16.0f })
	for (const double sweep : { -Math::TwoPi, -0.01, 0.01, Math::TwoPi })
	for (const bool full : { false, true })
	{
		const auto p = Place(Circle{ 40, 50, radius }, 0.7, sweep, radius * 0.3);
		REQUIRE(p);
		WritePoints(points, *p, 8, scale, full);
		CHECK(points.data() == storage);
		CHECK((1 <= points.size() && points.size() <= 253));
		CHECK(points.front() == p->start);
		CHECK(points.back() == (full ? p->tip : p->base));
		for (const auto point : points) CHECK(point.distanceFrom(p->center) == Catch::Approx(radius).epsilon(1e-6));
	}
}

TEST_CASE("CircleArcArrow.full_turn_flat_mesh_contact")
{
	for (const double start : { 0.0, 0.3, 1.2, -2.1 })
	for (const double sweep : { -20 * Math::Pi, -Math::TwoPi, Math::TwoPi, 20 * Math::Pi })
	for (const double length : { 20.0, 70.0, 130.0 })
	for (const float scale : { 0.25f, 1.0f, 8.0f })
	{
		const auto p = Place(Circle{ 240, 180, 100 }, start, sweep, length);
		REQUIRE(p);
		const auto mesh = CaptureMesh(*p, 12, 100, LineCap::Flat, scale);
		REQUIRE_FALSE(mesh.omitted);
		const Float2 tip = p->tip;
		const auto found = std::find_if(mesh.vertices.begin(), mesh.vertices.end(), [&](const auto& v) { return v.pos == tip; });
		REQUIRE(found != mesh.vertices.end());
		REQUIRE(found + 2 < mesh.vertices.end());
		CHECK(tip == Float2{ p->start }); // The uploaded tip uses the exact start coordinate.
		const Vec2 startMidpoint = (Vec2{ mesh.vertices[0].pos } + Vec2{ mesh.vertices[1].pos }) * 0.5;
		CHECK(startMidpoint.distanceFrom(tip) < 0.00004); // Float2 endpoint rounding only.
		const Vec2 baseMidpoint = (Vec2{ (found + 1)->pos } + Vec2{ (found + 2)->pos }) * 0.5;
		CHECK(baseMidpoint.distanceFrom(p->base) < 0.00004);
		CHECK((found + 1)->pos.distanceFrom((found + 2)->pos) == Catch::Approx(112).margin(0.0001));
	}
}

TEST_CASE("CircleArcArrow.caps_fallback_and_allocation_failure")
{
	for (const double angle : { -4.7, -0.1, 0.1, 4.7 })
	for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		const auto p = Place(Circle{ 200, 160, 100 }, 0, angle, 70);
		REQUIRE(p);
		(void)CaptureMesh(*p, 8, 60, cap, 1);
		(void)CaptureMesh(*p, 220, 80, cap, 1); // Width exceeds the diameter; overlaps are permitted.
	}
	const auto p = Place(Circle{ 200, 160, 100 }, 0, 4.7, 100);
	REQUIRE(p);
	CHECK(CaptureMesh(*p, 18, 0.001, LineCap::Round, 1).omitted);
	const auto reversal = Place(Circle{ 200, 160, 100 }, 0, 4.7, 400);
	REQUIRE(reversal);
	CHECK(CaptureMesh(*reversal, 8, 60, LineCap::Flat, 1).omitted);
	Array<Vec2> points;
	WritePoints(points, *p, 8, 1);
	uint32 attempts = 0;
	const auto fail = [&](uint16, uint32) { ++attempts; return Vertex2DBufferPointer{}; };
	const auto failed = Vertex2DBuilder::BuildLineStringArrow(fail, LineCap::Round, points, p->tip, 8, 100, Ink, 1);
	REQUIRE(failed); CHECK(*failed == 0); CHECK(attempts == 1);
}

TEST_CASE("CircleArcArrow.public_caps_clamping_and_empty")
{
	const SceneScope scene;
	const Circle circle{ 240, 180, 80 };
	const ColorF color{ 1.0, 0.5 };
	for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		// An oversized head on a quarter circle leaves a straight arrow with no shaft.
		const auto p = Place(circle, 0, Math::HalfPi, 400);
		REQUIRE(p);
		const auto expected = Capture([&] { Line{ p->start, p->tip }.drawArrow(cap, 8, SizeF{ 60 * p->sizeScale, 400 * p->sizeScale }, color); });
		const auto actual = Capture([&] { CHECK(&circle.drawArcArrow(cap, 0, Math::HalfPi, 8, SizeF{ 60, 400 }, color) == &circle); });
		CHECK(actual == expected);
	}
	for (const double sign : { -1.0, 1.0 })
	{
		const auto full = Capture([&] { circle.drawArcArrow(0.3, sign * Math::TwoPi, 8, SizeF{ 40, 50 }, color); });
		const auto clamped = Capture([&] { circle.drawArcArrow(LineCap::Flat, 0.3, sign * 20 * Math::Pi, 8, SizeF{ 40, 50 }, color); });
		const auto negativeRadius = Capture([&] { Circle{ circle.center, -80 }.drawArcArrow(0.3, sign * Math::TwoPi, 8, SizeF{ 40, 50 }, color); });
		CHECK(full == clamped); CHECK(full == negativeRadius);
	}
	const auto blank = Capture([] {});
	const auto empty = Capture([&]
	{
		for (const double bad : { 0.0, -1.0 })
		{
			circle.drawArcArrow(0, 2, bad, SizeF{ 20, 30 });
			circle.drawArcArrow(0, 2, 8, SizeF{ bad, 30 });
			circle.drawArcArrow(0, 2, 8, SizeF{ 20, bad });
		}
		circle.drawArcArrow(LineCap::Round, 0, 0, 8, SizeF{ 20, 30 });
		Circle{ circle.center, 0 }.drawArcArrow(LineCap::Round, 0, 2, 8, SizeF{ 20, 30 });
	});
	CHECK(empty == blank);
}

TEST_CASE("CircleArcArrow.public_fallback_alpha_and_transform")
{
	const SceneScope scene;
	const Circle circle{ 240, 180, 100 };
	const ColorF color{ 1.0, 0.5 };
	for (const double sweep : { -4.7, 4.7, Math::TwoPi })
	{
		const auto p = Place(circle, 0.2, sweep, 100);
		REQUIRE(p);
		Array<Vec2> points;
		WritePoints(points, *p, 18, 1, true);
		const LineString expectedCurve{ points.begin(), points.end() };
		const auto expected = Capture([&] { expectedCurve.draw(LineCap::Round, LineCap::Flat, 18, color); });
		const auto fallback = Capture([&] { circle.drawArcArrow(LineCap::Round, 0.2, sweep, 18, SizeF{ 0.001, 100 }, color); });
		CHECK(fallback == expected);
	}
	for (const double sweep : { -4.7, 4.7 })
	{
		const auto p = Place(circle, 0.2, sweep, 50);
		REQUIRE(p);
		const auto actual = Capture([&] { circle.drawArcArrow(0.2, sweep, 10, SizeF{ 60, 50 }, color); });
		const Vec2 direction = (p->tip - p->base).normalized();
		for (int32 i = -2; i <= 2; ++i)
		{
			const Vec2 point = p->base + direction * i;
			CHECK(actual[int32(point.y)][int32(point.x)].r == Catch::Approx(128).margin(1));
		}
		const Mat3x2 affine{ 0.7f, 0.1f, 0.2f, 0.8f, 10, 15 };
		const Vec2 inside = affine.transformPoint(p->base * 0.6 + p->tip * 0.4);
		const auto transformed = Capture([&]
		{
			const Transformer2D transform{ affine };
			circle.drawArcArrow(0.2, sweep, 10, SizeF{ 60, 50 });
		});
		CHECK(transformed[int32(inside.y)][int32(inside.x)].r > 200);
	}
}

TEST_CASE("CircleArcArrow.submission_benchmark", "[.benchmark]")
{
	const SceneScope scene;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
# endif
	struct Case { const char* name; double radius, sweep; SizeF head; };
	const std::array cases{
		Case{ "ordinary", 100, 4.7, { 40, 32 } },
		Case{ "short", 100, 0.2, { 40, 80 } },
		Case{ "full", 100, Math::TwoPi, { 40, 40 } },
		Case{ "omitted", 100, 4.7, { 0.001, 100 } },
		Case{ "maximum", 1000, 4.7, { 60, 80 } } };
	for (const auto& c : cases)
	{
		const Circle circle{ 200, 160, c.radius };
		std::array<Array<double>, 2> times;
		std::array<int64, 2> triangles{}, calls{};
		for (int32 round = -8; round < 21; ++round)
		for (int32 offset = 0; offset < 2; ++offset)
		{
			const int32 method = ((round + offset + 8) % 2);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < 128; ++i)
				{
					if (method) circle.drawArcArrow(0.3, c.sweep, 8, c.head);
					else circle.drawArc(0.3, c.sweep, 4, 4);
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / 128;
				if (round >= 0) times[method].push_back(ns);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) { triangles[method] = metrics.triangleCount; calls[method] = metrics.drawCalls; }
			if (round >= 0) { CHECK(metrics.triangleCount == triangles[method]); CHECK(metrics.drawCalls == calls[method]); }
		}
		for (size_t method = 0; method < 2; ++method)
		{
			times[method].sort();
			std::cout << "CircleArcArrow " << c.name << (method ? " arrow" : " arc") << " ns/call=" << times[method][10]
				<< " p10=" << times[method][2] << " p90=" << times[method][18]
				<< " triangles/call=" << double(triangles[method]) / 128 << " draws/batch=" << calls[method] << '\n';
		}
	}
}
