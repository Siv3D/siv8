//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/DoubleHeadedArrowGeometry.hpp"
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
	Bezier3 Straight(const Vec2 a, const Vec2 b)
	{
		return { a, a.lerp(b, 1.0 / 3), a.lerp(b, 2.0 / 3), b };
	}

	template <class Head>
	void CheckHeads(const detail::DoubleArrowHeads<Head>& heads, const Vec2 start, const Vec2 end, const double length)
	{
		CHECK(heads.start.tip == start);
		CHECK(heads.end.tip == end);
		for (const auto* head : { &heads.start, &heads.end })
		{
			CHECK((0 < head->sizeScale && head->sizeScale <= 1.0 + 1e-10));
			CHECK(head->base.distanceFrom(head->tip) == Catch::Approx(length * head->sizeScale).margin(1e-8));
		}
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
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("DoubleHeadedArrow.bezier_roots_reverse_and_short_paths")
{
	const auto check = [](const auto& curve)
	{
		for (const double length : { 0.001, 30.0, 100.0, 1000.0 })
		{
			const auto heads = detail::FindBezierDoubleArrowHeads(curve, length);
			const auto reversed = detail::FindBezierDoubleArrowHeads(curve.reversed(), length);
			REQUIRE(heads); REQUIRE(reversed);
			CheckHeads(*heads, curve.pointAt(0), curve.pointAt(1), length);
			CHECK(heads->start.parameter <= heads->end.parameter);
			CHECK(heads->start.base.distanceFrom(curve.pointAt(heads->start.parameter)) < 1e-8);
			CHECK(heads->end.base.distanceFrom(curve.pointAt(heads->end.parameter)) < 1e-8);
			CHECK(heads->start.base.distanceFrom(reversed->end.base) < 1e-7);
			CHECK(heads->end.base.distanceFrom(reversed->start.base) < 1e-7);
		}
	};
	check(Bezier2{ { 0, 0 }, { 90, -150 }, { 200, 20 } });
	check(Bezier3{ { 0, 0 }, { 80, -150 }, { 120, 180 }, { 200, 20 } });
	check(Bezier3{ { 0, 0 }, { 300, 150 }, { -200, 100 }, { 0, 0 } });
	const auto shortPath = detail::FindBezierDoubleArrowHeads(Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 100, 0 } }, 200);
	REQUIRE(shortPath);
	CHECK(shortPath->start.base == shortPath->end.base);
	CHECK(shortPath->start.base.x == Catch::Approx(50).margin(1e-8));
	CHECK(shortPath->start.sizeScale == Catch::Approx(0.25));
	CHECK(shortPath->end.sizeScale == Catch::Approx(0.25));
	CHECK_FALSE(detail::FindBezierDoubleArrowHeads(Straight({ 1, 2 }, { 1, 2 }), 20));
}

TEST_CASE("DoubleHeadedArrow.path_roots_cross_segments_and_reuse")
{
	Array<Vec2> shaft;
	shaft.reserve(50);
	const Vec2* storage = shaft.data();
	for (const auto& points : { LineString{ { 0, 0 }, { 0, 0 }, { 10, 0 }, { 100, 0 }, { 100, 100 }, { 100, 100 } },
		LineString{ { 0, 0 }, { 100, 0 }, { 100, 100 }, { 0, 100 }, { 0, 0 } },
		LineString{ { 0, 0 }, { 1, 0 }, { 100, 0 } } })
	{
		Array<Bezier3> curves;
		for (size_t i = 1; i < points.size(); ++i) curves.push_back(Straight(points[i - 1], points[i]));
		for (const double length : { 10.0, 80.0, 1000.0 })
		{
			CAPTURE(points, length);
			const auto heads = detail::FindLineStringDoubleArrowHeads(points, length);
			const auto spline = detail::FindSplineDoubleArrowHeads(curves, length);
			REQUIRE(heads); REQUIRE(spline);
			CheckHeads(*heads, points.front(), points.back(), length);
			CheckHeads(*spline, points.front(), points.back(), length);
			CHECK(heads->start.base.distanceFrom(spline->start.base) < 1e-7);
			CHECK(heads->end.base.distanceFrom(spline->end.base) < 1e-7);
			CHECK(heads->start.segment + heads->start.parameter <= heads->end.segment + heads->end.parameter);
			for (const auto* h : { &heads->start, &heads->end })
				CHECK(h->base.distanceFrom(points[h->segment].lerp(points[h->segment + 1], h->parameter)) < 1e-8);
			detail::WriteLineStringDoubleArrowShaft(shaft, points, *heads);
			CHECK(shaft.data() == storage);
			CHECK(shaft.front() == heads->start.base);
			CHECK(shaft.back() == heads->end.base);
			const auto reversed = detail::FindLineStringDoubleArrowHeads(points.reversed(), length);
			REQUIRE(reversed);
			CHECK(heads->start.base.distanceFrom(reversed->end.base) < 1e-7);
		}
	}
	const auto shortPath = detail::FindLineStringDoubleArrowHeads(LineString{ { 0, 0 }, { 1, 0 }, { 100, 0 } }, 200);
	REQUIRE(shortPath);
	CHECK(shortPath->start.base.x == Catch::Approx(50).margin(1e-8));
	CHECK_FALSE(detail::FindLineStringDoubleArrowHeads({}, 20));
	CHECK_FALSE(detail::FindLineStringDoubleArrowHeads(LineString{ { 2, 3 }, { 2, 3 } }, 20));
	CHECK_FALSE(detail::FindSplineDoubleArrowHeads({}, 20));
}

TEST_CASE("DoubleHeadedArrow.arc_chords_short_full_turn_and_budget")
{
	using namespace detail::CircleArcArrow;
	Array<Vec2> points;
	points.reserve(253);
	const Vec2* storage = points.data();
	for (const double radius : { -100.0, 100.0 })
	for (const double angle : { -20.0, -Math::TwoPi, -0.01, 0.01, 1.5, Math::TwoPi, 20.0 })
	for (const double length : { 1.0, 32.0, 400.0 })
	{
		const auto p = PlaceDouble(Circle{ 200, 160, radius }, 0.3, angle, length);
		REQUIRE(p);
		CHECK(Abs(p->headSweep) * 2 <= Abs(p->arc.sweep) + 1e-12);
		CHECK(p->startBase.distanceFrom(p->arc.start) == Catch::Approx(length * p->sizeScale).margin(1e-9));
		CHECK(p->endBase.distanceFrom(p->arc.tip) == Catch::Approx(length * p->sizeScale).margin(1e-9));
		CHECK(p->startBase.distanceFrom(p->arc.center) == Catch::Approx(100));
		if (Math::TwoPi <= Abs(angle)) CHECK(p->arc.start == p->arc.tip);
		for (const float scale : { 1.0f, 1000.0f })
		{
			WriteDoublePoints(points, *p, 8, scale);
			CHECK(points.data() == storage);
			CHECK((1 <= points.size() && points.size() <= 253));
			CHECK(points.front() == p->startBase);
			CHECK(points.back() == p->endBase);
		}
	}
	for (const double sweep : { -Math::TwoPi, Math::TwoPi })
	{
		const auto p = PlaceDouble(Circle{ 200, 160, 100 }, 0.3, sweep, 32);
		REQUIRE(p);
		WriteDoublePoints(points, *p, 8, 1);
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		const auto allocate = [&](uint16 vc, uint32 ic)
		{
			vertices.resize(vc); indices.resize(ic);
			return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 };
		};
		REQUIRE(Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(allocate, points, p->arc.start, p->arc.tip,
			8, 48, 48, Float4{ 1, 1, 1, 1 }, 1));
		CHECK(std::count_if(vertices.begin(), vertices.end(), [&](const auto& v) { return v.pos == Float2{ p->arc.start }; }) == 2);
	}
	CHECK_FALSE(PlaceDouble(Circle{ 0, 0, 0 }, 0, 1, 20));
	CHECK_FALSE(PlaceDouble(Circle{ 0, 0, 100 }, 0, 0, 20));
}

TEST_CASE("DoubleHeadedArrow.mesh_allocation_winding_and_failure")
{
	using Index = Vertex2D::IndexType;
	constexpr Index Offset = 37;
	const Float4 ink{ 0.3f, 0.7f, 1, 0.5f };
	Vertex2D sentinel;
	sentinel.set(-1000, -2000, -3, -4, Float4{ -1, -2, -3, -4 });
	for (const auto& points : { LineString{ { 100, 100 } }, LineString{ { 100, 100 }, { 200, 100 } },
		LineString{ { 100, 100 }, { 110, 90 }, { 120, 85 }, { 200, 100 } },
		LineString{ { 100, 100 }, { 100.1, 100 } } })
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
		uint32 allocations = 0;
		const auto allocate = [&](uint16 vc, uint32 ic)
		{
			++allocations;
			vertices.assign(vc + size_t{ 2 }, sentinel);
			indices.assign(ic + size_t{ 2 }, Index{ 65535 });
			return Vertex2DBufferPointer{ vertices.data() + 1, indices.data() + 1, Offset };
		};
		const Float2 start{ 60, 110 }, end{ static_cast<float>(points.back().x + 40), 110 };
		const auto result = Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(allocate, points, start, end, 12, 60, 60, ink, 1);
		REQUIRE(result); REQUIRE(allocations == 1); REQUIRE(*result + size_t{ 2 } == indices.size());
		CHECK(indices.front() == 65535); CHECK(indices.back() == 65535);
		CHECK(vertices.front().pos == sentinel.pos); CHECK(vertices.back().pos == sentinel.pos);
		for (const auto& v : vertices) CHECK(v.tex == sentinel.tex);
		for (size_t i = 1; i + 1 < vertices.size(); ++i)
		{
			CHECK(std::isfinite(vertices[i].pos.x)); CHECK(std::isfinite(vertices[i].pos.y));
			CHECK(vertices[i].color == ink);
		}
		for (size_t i = 1; i + 1 < indices.size(); ++i)
			REQUIRE((Offset <= indices[i] && indices[i] < Offset + vertices.size() - 2));
		for (size_t i = 1; i + 2 < indices.size() - 1; i += 3)
		{
			const Float2 a = vertices[indices[i] - Offset + 1].pos;
			const Float2 b = vertices[indices[i + 1] - Offset + 1].pos;
			const Float2 c = vertices[indices[i + 2] - Offset + 1].pos;
			CHECK((b - a).cross(c - a) >= -1e-5f);
		}
		uint32 failures = 0;
		const auto failed = [&](uint16, uint32) { ++failures; return Vertex2DBufferPointer{}; };
		CHECK(Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(failed, points, start, end, 12, 60, 60, ink, 1) == uint32{ 0 });
		CHECK(failures == 1);
	}
	uint32 calls = 0;
	const auto unused = [&](uint16, uint32) { ++calls; return Vertex2DBufferPointer{}; };
	const LineString shaft{ { 100, 100 }, { 200, 100 } };
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(unused, {}, { 0, 0 }, { 1, 0 }, 12, 60, 60, ink, 1));
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(unused, shaft, { 100, 100 }, { 240, 100 }, 12, 60, 60, ink, 1));
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(unused, shaft, { 110, 100 }, { 240, 100 }, 12, 60, 60, ink, 1));
	CHECK_FALSE(Vertex2DBuilder::BuildLineStringDoubleHeadedArrow(unused, shaft, { 60, 140 }, { 240, 100 }, 12, 12, 60, ink, 1));
	CHECK(calls == 0);
}

TEST_CASE("DoubleHeadedArrow.public_straight_shrink_alpha_and_curve_equivalence")
{
	const SceneScope scope;
	const ColorF ink{ 1.0, 0.5 };
	const LineString straight{ { 60, 100 }, { 65, 100 }, { 300, 100 }, { 300, 100 } };
	const Bezier2 quadratic{ { 60, 100 }, { 60, 100 }, { 300, 100 } };
	const Bezier3 cubic = Straight(straight.front(), straight.back());
	for (const double length : { 30.0, 350.0 })
	{
		const SizeF size{ 80, length };
		const auto reference = Capture([&] { Line{ straight.front(), straight.back() }.drawDoubleHeadedArrow(8, size, ink); });
		CHECK(reference == Capture([&] { CHECK(&straight.drawDoubleHeadedArrow(8, size, ink) == &straight); }));
		CHECK(reference == Capture([&] { CHECK(&quadratic.drawDoubleHeadedArrow(8, size, ink) == &quadratic); }));
		CHECK(reference == Capture([&] { CHECK(&cubic.drawDoubleHeadedArrow(8, size, ink) == &cubic); }));
		CHECK(reference[100][180].r == Catch::Approx(128).margin(1));
	}
	const Bezier3 curve{ { 50, 250 }, { 140, 50 }, { 240, 300 }, { 400, 180 } };
	const auto spline = Spline2D::FromBezierSegments(std::array{ curve });
	for (const int32 budget : { 1, 256 })
	{
		const auto reference = Capture([&] { curve.drawDoubleHeadedArrow(8, SizeF{ 70, 45 }, ink, 0.5, budget); });
		CHECK(reference == Capture([&] { CHECK(&spline.drawDoubleHeadedArrow(8, SizeF{ 70, 45 }, ink, 0.5, budget) == &spline); }));
		if (budget == 256)
		{
			uint8 maximum = 0;
			for (const auto pixel : reference) maximum = Max(maximum, pixel.r);
			CHECK(maximum == 128);
			const auto heads = detail::FindBezierDoubleArrowHeads(curve, 45);
			REQUIRE(heads);
			for (const auto& h : { heads->start, heads->end })
			{
				const Vec2 center = h.base.lerp(h.tip, 0.3);
				CHECK(reference[int32(center.y)][int32(center.x)].r == Catch::Approx(128).margin(1));
			}
		}

	}
}

TEST_CASE("DoubleHeadedArrow.public_closed_fallback_transform_and_empty")
{
	const SceneScope scope;
	const ColorF ink{ 1.0, 0.5 };
	const LineString ring{ { 100, 100 }, { 300, 100 }, { 300, 250 }, { 100, 250 }, { 100, 100 } };
	Array<Bezier3> curves;
	for (size_t i = 1; i < ring.size(); ++i) curves.push_back(Straight(ring[i - 1], ring[i]));
	const auto spline = Spline2D::FromBezierSegments(curves, CloseRing::Yes);
	const auto reference = Capture([&] { ring.drawDoubleHeadedArrow(8, SizeF{ 40, 45 }, ink); });
	CHECK(reference == Capture([&] { spline.drawDoubleHeadedArrow(8, SizeF{ 40, 45 }, ink); }));
	const LineString elbow{ { 80, 80 }, { 240, 80 }, { 240, 110 } };
	CHECK(Capture([&] { elbow.draw(LineCap::Flat, LineCap::Flat, 12, ink); })
		== Capture([&] { elbow.drawDoubleHeadedArrow(12, SizeF{ 0.001, 80 }, ink); }));
	const Mat3x2 affine{ -0.7f, 0.1f, 0.2f, 0.8f, 400, 15 };
	CHECK(Capture([&] { const Transformer2D t{ affine }; ring.drawDoubleHeadedArrow(8, SizeF{ 40, 45 }); })
		== Capture([&] { const Transformer2D t{ affine }; spline.drawDoubleHeadedArrow(8, SizeF{ 40, 45 }); }));
	const Circle circle{ 240, 180, 100 };
	CHECK(Capture([&] { CHECK(&circle.drawArcDoubleHeadedArrow(0.3, Math::TwoPi, 8, SizeF{ 40, 35 }, ink) == &circle); })
		== Capture([&] { circle.drawArcDoubleHeadedArrow(0.3, 20 * Math::Pi, 8, SizeF{ 40, 35 }, ink); }));
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		for (const double bad : { 0.0, -1.0 })
		{
			ring.drawDoubleHeadedArrow(bad, SizeF{ 40, 35 });
			spline.drawDoubleHeadedArrow(8, SizeF{ bad, 35 });
			circle.drawArcDoubleHeadedArrow(0, 1, 8, SizeF{ 40, bad });
		}
		LineString{}.drawDoubleHeadedArrow(8, SizeF{ 40, 35 });
		Spline2D{}.drawDoubleHeadedArrow(8, SizeF{ 40, 35 });
		Straight({ 20, 20 }, { 20, 20 }).drawDoubleHeadedArrow(8, SizeF{ 40, 35 });
		Bezier2{ { 20, 20 }, { 20, 20 }, { 20, 20 } }.drawDoubleHeadedArrow(8, SizeF{ 40, 35 });
		circle.drawArcDoubleHeadedArrow(0, 0, 8, SizeF{ 40, 35 });
	}));
	CHECK_THROWS_AS(spline.drawDoubleHeadedArrow(8, SizeF{ 40, 35 }, ink, 0, 256), std::invalid_argument);
	CHECK_THROWS_AS(spline.drawDoubleHeadedArrow(8, SizeF{ 40, 35 }, ink, 0.5, 0), std::invalid_argument);
	CHECK_THROWS_AS(spline.drawDoubleHeadedArrow(0, SizeF{ 40, 35 }, ink, 0.5, 4097), std::invalid_argument);
}

TEST_CASE("DoubleHeadedArrow.submission_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
	const auto cpuTime = []
	{
		timespec t{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
		return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
	};
# else
	const auto cpuTime = [] { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); };
# endif
	const Bezier2 b2{ { 40, 200 }, { 200, 40 }, { 400, 180 } };
	const Bezier3 b3{ { 40, 200 }, { 140, 40 }, { 260, 300 }, { 400, 180 } };
	const Circle circle{ 240, 180, 100 };
	const LineString line{ { 40, 200 }, { 120, 120 }, { 260, 160 }, { 400, 180 } };
	const auto spline = Spline2D::FromBezierSegments(std::array{ b3, b3.movedBy(360, -20) });
	const char* names[]{ "Bezier2", "Bezier3", "CircleArc", "LineString", "Spline2D" };
	for (int32 shape = 0; shape < 5; ++shape)
	for (const bool shortPath : { false, true })
	{
		const SizeF head{ 60, shortPath ? 1000.0 : 40.0 };
		std::array<Array<double>, 2> wall, cpu;
		std::array<int64, 2> triangles{};
		for (int32 round = -8; round < 21; ++round)
		for (int32 offset = 0; offset < 2; ++offset)
		{
			const int32 method = ((round + offset + 8) % 2);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
				const auto cpuBegin = cpuTime();
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < 128; ++i)
				{
					switch (shape)
					{
					case 0: if (method) b2.drawDoubleHeadedArrow(8, head); else b2.drawArrow(8, head); break;
					case 1: if (method) b3.drawDoubleHeadedArrow(8, head); else b3.drawArrow(8, head); break;
					case 2: if (method) circle.drawArcDoubleHeadedArrow(0.3, 4.7, 8, head); else circle.drawArcArrow(0.3, 4.7, 8, head); break;
					case 3: if (method) line.drawDoubleHeadedArrow(8, head); else line.drawArrow(8, head); break;
					case 4: if (method) spline.drawDoubleHeadedArrow(8, head); else spline.drawArrow(8, head); break;
					}
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / 128;
				const double cpuNs = double(cpuTime() - cpuBegin) / 128;
				if (round >= 0) { wall[method].push_back(ns); cpu[method].push_back(cpuNs); }
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) triangles[method] = metrics.triangleCount;
			if (round >= 0) CHECK(metrics.triangleCount == triangles[method]);
		}
		for (int32 method = 0; method < 2; ++method)
		{
			wall[method].sort(); cpu[method].sort();
			std::cout << "DoubleHeadedArrow " << names[shape] << (shortPath ? " shrink" : " ordinary")
				<< (method ? " double" : " single") << " ns/call=" << wall[method][10]
				<< " p10=" << wall[method][2] << " p90=" << wall[method][18]
				<< " triangles/call=" << double(triangles[method]) / 128;
# if SIV3D_PLATFORM(MACOS)
			std::cout << " cpu_ns/call=" << cpu[method][10];
# endif
			std::cout << '\n';
		}
	}
}
