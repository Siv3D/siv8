//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/PathArrowGeometry.hpp"
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# include <time.h>
# endif

namespace
{
# if SIV3D_PLATFORM(MACOS)
	int64 ThreadTime()
	{
		timespec time{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time) == 0);
		return (int64(time.tv_sec) * 1'000'000'000 + time.tv_nsec);
	}
# endif

	Bezier3 Straight(const Vec2 a, const Vec2 b)
	{
		return { a, a.lerp(b, 1.0 / 3), a.lerp(b, 2.0 / 3), b };
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
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
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

TEST_CASE("PathArrow.polyline_last_root_shrink_duplicates_and_reuse")
{
	const std::array paths{
		LineString{ { 0, 0 }, { 100, 0 }, { 100, 100 } },
		LineString{ { 0, 0 }, { 0, 0 }, { 100, 0 }, { 100, 100 }, { 100, 100 } },
		LineString{ { 100, 0 }, { 0, 0 }, { -100, 0 }, { 0, 0 } },
		LineString{ { 0, 0 }, { 100, 0 }, { 100, 100 }, { 0, 100 }, { 0, 0 } },
		LineString{ { 1e6, -1e6 }, { 1e6 + 10, -1e6 }, { 1e6 + 20, -1e6 } } };
	Array<Vec2> shaft;
	shaft.reserve(100);
	const Vec2* storage = shaft.data();
	for (const auto& points : paths)
	for (const double length : { 0.001, 20.0, 100.0, 120.0, 1000.0 })
	{
		CAPTURE(points, length);
		const auto head = detail::FindLineStringArrowHead(points, length);
		REQUIRE(head);
		CHECK(head->tip == points.back());
		CHECK(head->base.distanceFrom(head->tip) == Catch::Approx(length * head->sizeScale).margin(1e-8));
		CHECK((0 <= head->parameter && head->parameter <= 1));
		CHECK(head->base.distanceFrom(points[head->segment].lerp(points[head->segment + 1], head->parameter)) < 1e-8);
		for (size_t i = head->segment + 1; i < points.size(); ++i)
			CHECK(points[i].distanceFrom(head->tip) <= length * head->sizeScale + 1e-8);
		if (head->sizeScale < 1)
			for (const Vec2 point : points) CHECK(point.distanceFrom(head->tip) <= length * head->sizeScale + 1e-8);
		detail::WriteLineStringArrowShaft(shaft, points, *head);
		CHECK(shaft.data() == storage);
		CHECK(shaft.front() == points.front()); CHECK(shaft.back() == head->base);
	}
	const auto lastMaximum = detail::FindLineStringArrowHead(paths[2], 200);
	REQUIRE(lastMaximum); CHECK(lastMaximum->segment == 2); CHECK(lastMaximum->sizeScale == 0.5);
	const auto vertex = detail::FindLineStringArrowHead(paths[0], 100);
	REQUIRE(vertex); CHECK(vertex->segment == 1); CHECK(vertex->parameter == 0);
	for (const auto& points : { LineString{}, LineString{ { 1, 2 } }, LineString{ { 1, 2 }, { 1, 2 }, { 1, 2 } } })
		CHECK_FALSE(detail::FindLineStringArrowHead(points, 20));
}

TEST_CASE("PathArrow.spline_cross_segment_roots_and_interior_maxima")
{
	const std::array curves{ Straight({ 0, 0 }, { 100, 0 }), Straight({ 100, 0 }, { 100, 50 }),
		Straight({ 100, 50 }, { 110, 50 }) };
	const auto head = detail::FindSplineArrowHead(curves, 80);
	REQUIRE(head); CHECK(head->segment == 0); CHECK(head->sizeScale == 1);
	CHECK(head->base.x == Catch::Approx(110 - std::sqrt(80 * 80 - 50 * 50)).margin(1e-7));
	CHECK(head->base.y == 0);
	const auto boundary = detail::FindSplineArrowHead(curves, 10);
	REQUIRE(boundary); CHECK(boundary->segment == 2); CHECK(boundary->parameter == 0);

	const std::array loop{ Bezier3{ { 0, 0 }, { 300, 100 }, { -200, 100 }, { 0, 0 } },
		Bezier3{ { 0, 0 }, { 20, -10 }, { 20, 10 }, { 0, 0 } } };
	for (const double length : { 2.0, 30.0, 70.0, 500.0 })
	{
		const auto h = detail::FindSplineArrowHead(loop, length);
		REQUIRE(h);
		CHECK(h->base == loop[h->segment].pointAt(h->parameter));
		CHECK(h->base.distanceFrom(h->tip) == Catch::Approx(length * h->sizeScale).margin(1e-7));
		for (size_t i = 0; i < loop.size(); ++i)
		for (int32 j = 0; j <= 1000; ++j)
		{
			const double t = j / 1000.0;
			if (h->sizeScale < 1 || i > h->segment || (i == h->segment && h->parameter <= t))
				CHECK(loop[i].pointAt(t).distanceFrom(h->tip) <= length * h->sizeScale + 1e-7);
		}
		if (length == 500) { CHECK(h->segment == 0); CHECK((0 < h->parameter && h->parameter < 1)); }
	}
	const std::array duplicated{ loop[0], loop[0] };
	const auto last = detail::FindSplineArrowHead(duplicated, 500);
	REQUIRE(last); CHECK(last->segment == 1);
	const std::array constant{ Straight({ 2, 3 }, { 2, 3 }), Straight({ 2, 3 }, { 2, 3 }) };
	CHECK_FALSE(detail::FindSplineArrowHead(constant, 20));
	CHECK_FALSE(detail::FindSplineArrowHead({}, 20));
}

TEST_CASE("PathArrow.public_straight_and_single_bezier_equivalence")
{
	const SceneScope scope;
	const LineString points{ { 60, 100 }, { 180, 100 }, { 300, 100 }, { 300, 100 } };
	const Bezier3 curve{ { 40, 250 }, { 140, 50 }, { 240, 300 }, { 400, 180 } };
	const auto spline = Spline2D::FromBezierSegments(std::array{ curve });
	const auto padded = Spline2D::FromBezierSegments(std::array{
		Straight(curve.p0, curve.p0), curve, Straight(curve.p3, curve.p3) });
	for (const auto cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const double length : { 30.0, 350.0 })
	{
		const ColorF ink{ 1.0, 0.5 };
		const SizeF size{ 80, length };
		const auto line = Capture([&] { Line{ points.front(), points.back() }.drawArrow(cap, 8, size, ink); });
		const auto polyline = Capture([&] { CHECK(&points.drawArrow(cap, 8, size, ink) == &points); });
		CHECK(line == polyline);
		for (const int32 budget : { 1, 256 })
		{
			const auto bezier = Capture([&] { curve.drawArrow(cap, 8, size, ink, 0.5, budget); });
			const auto path = Capture([&] { CHECK(&spline.drawArrow(cap, 8, size, ink, 0.5, budget) == &spline); });
			CHECK(bezier == path);
			CHECK(bezier == Capture([&] { padded.drawArrow(cap, 8, size, ink, 0.5, budget); }));
		}
	}
}

TEST_CASE("PathArrow.public_closed_fallback_alpha_transform_and_empty")
{
	const SceneScope scope;
	const LineString ring{ { 100, 100 }, { 300, 100 }, { 300, 250 }, { 100, 250 }, { 100, 100 } };
	Array<Bezier3> curves;
	for (size_t i = 1; i < ring.size(); ++i) curves.push_back(Straight(ring[i - 1], ring[i]));
	const auto closed = Spline2D::FromBezierSegments(curves, CloseRing::Yes);
	const auto open = Spline2D::FromBezierSegments(curves);
	const ColorF ink{ 1.0, 0.5 };
	const SizeF size{ 40, 45 };
	const auto reference = Capture([&] { ring.drawArrow(8, size, ink); });
	CHECK(reference == Capture([&] { closed.drawArrow(8, size, ink); }));
	CHECK(reference == Capture([&] { open.drawArrow(LineCap::Flat, 8, size, ink); }));
	const auto reversedRing = ring.reversed();
	const auto reversedSpline = closed.reversed();
	CHECK(Capture([&] { reversedRing.drawArrow(8, size, ink); })
		== Capture([&] { reversedSpline.drawArrow(8, size, ink); }));
	const auto head = detail::FindSplineArrowHead(curves, size.y);
	REQUIRE(head); CHECK(head->tip == ring.front());
	CHECK(reference[145][100].r == Catch::Approx(128).margin(1));
	const Mat3x2 affine{ 0.7f, 0.1f, 0.2f, 0.8f, 10, 15 };
	const auto transformed = Capture([&] { const Transformer2D transform{ affine }; closed.drawArrow(8, size); });
	const Vec2 inside = affine.transformPoint(head->base * 0.6 + head->tip * 0.4);
	CHECK(transformed[int32(inside.y)][int32(inside.x)].r > 200);

	// The oblique terminal cut cannot fit a head only marginally wider than the shaft.
	const LineString elbow{ { 80, 80 }, { 240, 80 }, { 240, 110 } };
	const std::array elbowCurves{ Straight(elbow[0], elbow[1]), Straight(elbow[1], elbow[2]) };
	const auto elbowSpline = Spline2D::FromBezierSegments(elbowCurves);
	const auto full = Capture([&] { elbow.draw(LineCap::Round, LineCap::Flat, 12, ink); });
	CHECK(full == Capture([&] { elbow.drawArrow(LineCap::Round, 12, SizeF{ 0.001, 80 }, ink); }));
	CHECK(full == Capture([&] { elbowSpline.drawArrow(LineCap::Round, 12, SizeF{ 0.001, 80 }, ink); }));
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		for (const double bad : { 0.0, -1.0 })
		{
			ring.drawArrow(bad, size); closed.drawArrow(bad, size);
			ring.drawArrow(8, SizeF{ bad, 30 }); closed.drawArrow(8, SizeF{ bad, 30 });
			ring.drawArrow(8, SizeF{ 30, bad }); closed.drawArrow(8, SizeF{ 30, bad });
		}
		LineString{}.drawArrow(8, size); LineString{ { 10, 10 }, { 10, 10 } }.drawArrow(LineCap::Round, 8, size);
		Spline2D{}.drawArrow(8, size);
		Spline2D::FromBezierSegments(std::array{ Straight({ 10, 10 }, { 10, 10 }) }).drawArrow(LineCap::Round, 8, size);
	}));
	CHECK_THROWS_AS(closed.drawArrow(8, size, Palette::White, 0, 256), std::invalid_argument);
	CHECK_THROWS_AS(closed.drawArrow(8, size, Palette::White, 0.5, 0), std::invalid_argument);
	CHECK_THROWS_AS(closed.drawArrow(0, size, Palette::White, 0.5, 4097), std::invalid_argument);
}

TEST_CASE("PathArrow.submission_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	Graphics::SetVSyncEnabled(false);
# if SIV3D_PLATFORM(MACOS)
	struct QoS
	{
		pthread_override_t value = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		~QoS() { if (value) pthread_override_qos_class_end_np(value); }
	} qos;
# endif
	for (const int32 count : { 4, 32 })
	{
		Array<Bezier3> curves;
		LineString points;
		for (int32 i = 0; i < count; ++i)
			curves.push_back(Bezier3{ { i * 100.0, 100 }, { i * 100.0 + 30, 30 }, { i * 100.0 + 70, 170 }, { i * 100.0 + 100, 100 } });
		const auto spline = Spline2D::FromBezierSegments(curves);
		spline.toLineString(points, 8);
		for (const bool useSpline : { false, true })
		for (const bool shrink : { false, true })
		{
			const SizeF head{ 60, shrink ? (count * 200.0) : 45.0 };
			std::array<Array<double>, 2> times;
# if SIV3D_PLATFORM(MACOS)
			std::array<Array<double>, 2> cpu;
# endif
			std::array<int64, 2> triangles{}, batches{};
			for (int32 round = -8; round < 21; ++round)
			for (int32 offset = 0; offset < 2; ++offset)
			{
				const int32 method = ((round + offset + 8) % 2);
				REQUIRE(System::Update());
				{
					const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
# if SIV3D_PLATFORM(MACOS)
					const int64 cpuBegin = ThreadTime();
# endif
					const auto begin = std::chrono::steady_clock::now();
					for (int32 i = 0; i < 128; ++i)
					{
						if (useSpline)
						{
							if (method) spline.drawArrow(8, head); else spline.draw(8);
						}
						else if (method) points.drawArrow(8, head);
						else points.draw(LineCap::Flat, LineCap::Flat, 8);
					}
					const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / 128;
# if SIV3D_PLATFORM(MACOS)
					const double cpuNs = double(ThreadTime() - cpuBegin) / 128;
					if (round >= 0) cpu[method].push_back(cpuNs);
# endif
					if (round >= 0) times[method].push_back(ns);
				}
				REQUIRE(System::Update());
				const auto metrics = Profiler::GetFrameMetrics();
				if (round == 0) { triangles[method] = metrics.triangleCount; batches[method] = metrics.drawCalls; }
				if (round >= 0) { CHECK(metrics.triangleCount == triangles[method]); CHECK(metrics.drawCalls == batches[method]); }
			}
			for (size_t method = 0; method < 2; ++method)
			{
				times[method].sort();
				std::cout << "PathArrow " << (useSpline ? "spline" : "polyline") << " segments=" << count
					<< (shrink ? " shrink" : " ordinary") << (method ? " arrow" : " draw") << " ns/call=" << times[method][10]
					<< " p10=" << times[method][2] << " p90=" << times[method][18]
					<< " triangles/call=" << double(triangles[method]) / 128 << " draws/batch=" << batches[method];
# if SIV3D_PLATFORM(MACOS)
				cpu[method].sort();
				std::cout << " cpu_ns/call=" << cpu[method][10] << " cpu_p10=" << cpu[method][2] << " cpu_p90=" << cpu[method][18];
# endif
				std::cout << '\n';
			}
		}
	}
}
