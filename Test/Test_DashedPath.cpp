//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Geometry2D/DashedPath.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/IRenderer2D.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include <chrono>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# include <time.h>
# endif

namespace
{
	using detail::DashedPath::Workspace;
	using detail::DashedPath::Split;

	LineString RunPoints(const Workspace& out, const size_t index)
	{
		const auto points = out.view(out.runs[index]);
		return LineString{ points.begin(), points.end() };
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
			Scene::Configure(config); Scene::SetBackground(background); Graphics::SetVSyncEnabled(vsync);
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

TEST_CASE("DashedPath.fixed_pattern_shader_oracle")
{
	Workspace out;
	const LineString path{ { 0, 0 }, { 7, 0 }, { 37, 0 }, { 100, 0 } };
	const auto triangle = [](double x) { return std::abs(2 * (x - std::floor(x)) - 1); };
	for (const auto preset : { LineStyle::Dotted, LineStyle::Dashed, LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot })
	for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const double width : { 0.75, 4.0, 12.3, 240.0 })
	for (const double phase : { -23.125, -0.01, 0.0, 0.5, 39.25 })
	{
		LineStyle style{ preset(phase) }; style.cap = cap;
		REQUIRE(Split(out, path, false, width, style));
		const double extension = (cap == LineCap::Flat ? 0 : width * 0.5);
		const double unit = (preset.type == LinePattern::RoundDot && extension
			? (100 + width) / (std::ceil(100 / width * 0.5) * 2 + 1) : width);
		for (int32 i = 0; i < 420; ++i)
		{
			const double d = (i + 0.371) * (100 + 2 * width) / 420 - width;
			const double x = (d + extension) / unit - phase;
			double coverage = 0;
			switch (preset.type)
			{
			case LinePattern::Dotted: coverage = triangle((x - 0.5) / 2) - 0.5; break;
			case LinePattern::Dashed: coverage = triangle((x - 1) / 4) - 0.4; break;
			case LinePattern::LongDash: coverage = triangle((x - 1) / 10) - 0.3; break;
			case LinePattern::DashDot: coverage = std::max(triangle((x - 1) / 10) - 0.4, triangle((x - 1) / 10 + 0.5) - 0.9); break;
			case LinePattern::RoundDot: coverage = 0.5 - std::abs(std::remainder(x - 0.5, 2.0)); break;
			default: break;
			}
			if (std::abs(coverage) < 1e-5) continue;
			bool covered = false;
			for (const auto& run : out.runs)
			{
				const auto points = out.view(run);
				covered |= (points.front().x <= d && d < points.back().x);
			}
			for (const auto& dot : out.dots) covered |= (dot.start.x <= d && d < dot.end.x);
			CHECK(covered == (coverage > 0 && -extension <= d && d < 100 + extension));
		}
	}
}

TEST_CASE("DashedPath.corners_seams_and_storage")
{
	Workspace out;
	const LineString path{ { 0, 0 }, { 10, 0 }, { 10, 10 } };
	REQUIRE(Split(out, path, false, 5, LineStyle::DashedFlat));
	REQUIRE(out.runs.size() == 2);
	CHECK(RunPoints(out, 0) == LineString{ { 0, 0 }, { 10, 0 }, { 10, 1 } });
	CHECK(RunPoints(out, 1) == LineString{ { 10, 9 }, { 10, 10 } });
	REQUIRE(Split(out, path, false, 5, LineStyle::DashedFlat(-0.198)));
	CHECK(RunPoints(out, 0)[1] == Vec2{ 10, 0 });
	CHECK(RunPoints(out, 0).back().y == Catch::Approx(0.01));
	const LineString ring{ { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } };
	for (const LineString& source : { ring, LineString{ { 0, 0 }, { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 }, { 0, 0 }, { 0, 0 } } })
	{
		REQUIRE(Split(out, source, true, 5, LineStyle::Dashed));
		REQUIRE(out.runs.size() == 2);
		const auto first = RunPoints(out, 0);
		const LineString expected{ { 0, 1 }, { 0, 0 }, { 10, 0 }, { 10, 1 } };
		REQUIRE(first.size() == expected.size());
		for (size_t i = 0; i < first.size(); ++i)
		{
			CHECK(first[i].x == Catch::Approx(expected[i].x));
			CHECK(first[i].y == Catch::Approx(expected[i].y));
		}
		CHECK(RunPoints(out, 1) == LineString{ { 10, 9 }, { 10, 10 }, { 0, 10 }, { 0, 9 } });
	}
	const auto pc = out.path.capacity(), vc = out.points.capacity(), rc = out.runs.capacity();
	REQUIRE(Split(out, ring, true, 5, LineStyle::DashedFlat));
	CHECK(out.path.capacity() == pc); CHECK(out.points.capacity() == vc); CHECK(out.runs.capacity() == rc);
	REQUIRE(Split(out, ring, true, 100, LineStyle::LongDash));
	REQUIRE(out.runs.size() == 1); CHECK(out.runs.front().closed); CHECK(RunPoints(out, 0) == ring);
	REQUIRE(Split(out, ring, true, 5, LineStyle::Dotted));
	REQUIRE(out.runs.size() == 4);
	CHECK(RunPoints(out, 0) == LineString{ { 0, 0 }, { 5, 0 } });
	// A center exactly at the seam belongs to one dot, not two clipped halves.
	REQUIRE(Split(out, ring, true, 5, LineStyle::RoundDot(-0.5)));
	REQUIRE(out.dots.size() == 4); CHECK(out.runs.empty());
	CHECK(out.dots.front().start == Float2{ -2.5f, 0 });
	CHECK(out.dots.front().end == Float2{ 2.5f, 0 });
	const auto dots = out.dots;
	REQUIRE(Split(out, ring, true, 5, LineStyle::RoundDotFlat(1.5)));
	REQUIRE(out.dots.size() == dots.size());
	for (size_t i = 0; i < dots.size(); ++i)
	{
		CHECK(out.dots[i].start == dots[i].start); CHECK(out.dots[i].end == dots[i].end);
		CHECK(out.dots[i].uRange == Float2{ 0, 1 });
	}
}

TEST_CASE("DashedPath.empty_width_and_budget")
{
	Workspace out;
	for (const LineString& path : { LineString{}, LineString{ { 3, 4 } }, LineString{ { 3, 4 }, { 3, 4 } } })
	for (const bool closed : { false, true })
	{
		REQUIRE(Split(out, path, closed, 1, LineStyle::Dashed)); CHECK(out.runs.empty()); CHECK(out.dots.empty());
	}
	const LineString path{ { 0, 0 }, { 8192, 0 } };
	for (const auto preset : { LineStyle::DottedFlat, LineStyle::RoundDotFlat })
	{
		REQUIRE(Split(out, path, false, 1, preset)); CHECK(out.runs.size() + out.dots.size() == 4096);
		CHECK_FALSE(Split(out, LineString{ { 0, 0 }, { 8192.01, 0 } }, false, 1, preset));
		CHECK(out.runs.empty()); CHECK(out.points.empty()); CHECK(out.dots.empty());
		REQUIRE(Split(out, path, false, 0, preset)); CHECK(out.runs.empty()); CHECK(out.dots.empty());
	}
	CHECK_FALSE(Split(out, path, false, 1e-12, LineStyle::DashDot));
	CHECK(out.runs.empty()); CHECK(out.dots.empty());
	REQUIRE(Split(out, LineString{ { 0, 0 }, { 0.001, 0 } }, false, 1, LineStyle::DottedFlat(1)));
	CHECK(out.runs.empty());
	REQUIRE(Split(out, LineString{ { 0, 0 }, { 0.001, 0 } }, false, 1, LineStyle::DottedFlat));
	REQUIRE(out.runs.size() == 1); CHECK(RunPoints(out, 0).back() == Vec2{ 0.001, 0 });
	REQUIRE(Split(out, LineString{ { 0, 0 }, { 9, 0 } }, false, 4, LineStyle::RoundDotFlat(-0.5)));
	REQUIRE(out.dots.size() == 2);
	CHECK(out.dots[0].uRange == Float2{ 0.5f, 1 }); CHECK(out.dots[1].uRange == Float2{ 0, 0.75f });
}

TEST_CASE("DashedPath.round_dot_upload")
{
	std::array<Vertex2D, 6> vertices;
	for (auto& v : vertices) v.set(-99, -99, -99, -99, Float4{ -1, -1, -1, -1 });
	std::array<Vertex2D::IndexType, 8> indices; indices.fill(999);
	const auto allocate = [&](auto v, auto i)
	{
		REQUIRE(v == 4); REQUIRE(i == 6);
		return Vertex2DBufferPointer{ vertices.data() + 1, indices.data() + 1, 17 };
	};
	const Float4 color{ 0.1f, 0.2f, 0.3f, 0.5f };
	CHECK(Vertex2DBuilder::BuildRoundDot(allocate, { 10, 20 }, { 16, 20 }, 8, { 0.25f, 1 }, color) == 6);
	CHECK(vertices[1].pos == Float2{ 10, 24 }); CHECK(vertices[2].pos == Float2{ 10, 16 });
	CHECK(vertices[3].pos == Float2{ 16, 24 }); CHECK(vertices[4].pos == Float2{ 16, 16 });
	CHECK(vertices[1].tex == Float2{ 0.25f, 1 }); CHECK(vertices[4].tex == Float2{ 1, 0 });
	for (size_t i = 1; i <= 4; ++i) CHECK(vertices[i].color == color);
	CHECK(vertices.front().pos == Float2{ -99, -99 }); CHECK(vertices.back().pos == Float2{ -99, -99 });
	CHECK(indices.front() == 999); CHECK(indices.back() == 999);
	for (size_t i = 1; i <= 6; ++i) { CHECK(indices[i] >= 17); CHECK(indices[i] < 21); }
	CHECK(Vertex2DBuilder::BuildRoundDot([](auto, auto) { return Vertex2DBufferPointer{}; }, { 0, 0 }, { 1, 0 }, 8, { 0, 1 }, color) == 0);
	// A clipped interval can collapse when converted to float.
	CHECK(Vertex2DBuilder::BuildRoundDot(allocate, { 10, 20 }, { 10, 20 }, 8, { 0.5f, 0.5001f }, color) == 6);
	for (size_t i = 1; i <= 4; ++i) { CHECK(std::isfinite(vertices[i].pos.x)); CHECK(std::isfinite(vertices[i].pos.y)); }
}

TEST_CASE("DashedPath.public_rendering_patterns_and_transforms")
{
	const SceneScope scope;
	const LineString path{ { 60, 100 }, { 160, 100 }, { 160, 200 } };
	const ColorF ink{ 1.0, 0.5 };
	for (const Mat3x2 matrix : { Mat3x2::Identity(), Mat3x2{ -0.8f, 0.1f, 0.15f, 0.9f, 400, 10 } })
	{
		const auto actual = Capture([&] { const Transformer2D t{ matrix }; CHECK(&path.draw(LineStyle::LongDashFlat, 24, ink) == &path); });
		const auto expected = Capture([&]
		{
			const Transformer2D t{ matrix };
			LineString{ { 60, 100 }, { 160, 100 }, { 160, 108 } }.draw(LineCap::Flat, 24, ink);
			LineString{ { 160, 180 }, { 160, 200 } }.draw(LineCap::Flat, 24, ink);
		});
		CHECK(actual == expected);
		CHECK(actual == Capture([&] { const Transformer2D t{ matrix }; path.draw(LineStyle::LongDashFlat, -24, ink); }));
	}
	for (const auto preset : { LineStyle::Dotted, LineStyle::Dashed, LineStyle::LongDash, LineStyle::DashDot, LineStyle::RoundDot })
	for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		LineStyle style{ preset(-0.375) }; style.cap = cap;
		const auto actual = Capture([&] { LineString{ { 60, 100 }, { 180, 100 }, { 355, 100 } }.draw(style, 16); });
		const auto expected = Capture([&] { Line{ 60, 100, 355, 100 }.draw(style, 16); });
		// Compare interiors away from shader AA transitions. The geometry cutter
		// intentionally does not reproduce Line's longitudinal shader fringe.
		for (int32 x = 40; x < 375; ++x)
		{
			const int32 r = expected[100][x].r;
			if (r < 8 || r > 247) CHECK(std::abs(int32(actual[100][x].r) - r) < 12);
		}
	}
	for (const auto style : { LineStyle::Flat, LineStyle::Square, LineStyle::Round })
		CHECK(Capture([&] { path.draw(style, 16, ink); }) == Capture([&] { path.draw(style.cap, 16, ink); }));
	const LineString ring{ { 60, 60 }, { 160, 60 }, { 160, 160 }, { 60, 160 } };
	CHECK(Capture([&] { CHECK(&ring.drawClosed(LineStyle::Dashed, 50, ink) == &ring); }) == Capture([&]
	{
		LineString{ { 60, 70 }, { 60, 60 }, { 160, 60 }, { 160, 70 } }.draw(LineCap::Flat, 50, ink);
		LineString{ { 160, 150 }, { 160, 160 }, { 60, 160 }, { 60, 150 } }.draw(LineCap::Flat, 50, ink);
	}));
	CHECK(Capture([&] { ring.drawClosed(LineStyle::Round, 16, ink); }) == Capture([&] { ring.drawClosed(16, ink); }));
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		path.draw(LineStyle::Dashed, 0); path.draw(LineStyle::Dotted, 1e-12);
		LineString{ { 60, 60 }, { 60, 60 } }.draw(LineStyle::RoundDot, 12);
	}));
}

TEST_CASE("DashedPath.curve_and_spline_public_apis")
{
	const SceneScope scope;
	const Bezier2 quadratic{ { 35, 190 }, { 180, -110 }, { 325, 190 } };
	const Bezier3 cubic{ { 30, 175 }, { 80, -30 }, { 270, 270 }, { 330, 60 } };
	const ColorF ink{ 1.0, 0.5 };
	for (const auto preset : { LineStyle::DashDot, LineStyle::RoundDotFlat, LineStyle::Round })
	{
		const LineStyle style{ preset(-0.25) };
		CHECK(Capture([&] { CHECK(&quadratic.draw(style, 8, ink) == &quadratic); })
			== Capture([&] { quadratic.toLineStringAdaptive().draw(style, 8, ink); }));
		CHECK(Capture([&] { CHECK(&cubic.draw(style, 8, ink) == &cubic); })
			== Capture([&] { cubic.toLineStringAdaptive().draw(style, 8, ink); }));
		const LineString knots{ { 40, 170 }, { 100, 80 }, { 200, 220 }, { 300, 60 } };
		for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
		{
			const Spline2D spline = Spline2D::FromCatmullRom(knots, closed);
			CHECK(Capture([&] { CHECK(&spline.draw(style, 8, ink) == &spline); }) == Capture([&]
			{
				const auto line = spline.toLineStringAdaptive();
				if (closed) line.drawClosed(style, 8, ink); else line.draw(style, 8, ink);
			}));
		}
	}
	const auto blank = Capture([] {});
	CHECK(blank == Capture([&]
	{
		const Vec2 p{ 100, 100 };
		Bezier2{ p, p, p }.draw(LineStyle::Dashed, 10); Bezier3{ p, p, p, p }.draw(LineStyle::RoundDot, 10);
		Spline2D{}.draw(LineStyle::DashDot, 10);
	}));
	CHECK_THROWS_AS(Spline2D{}.draw(LineStyle::Dashed, 10, ink, 0), std::invalid_argument);
	CHECK_THROWS_AS(Spline2D{}.draw(LineStyle::Dashed, 10, ink, 0.5, 4097), std::invalid_argument);
}

TEST_CASE("DashedPath.submission_benchmark", "[.benchmark]")
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
# endif
	struct Case
	{
		const char* name;
		LineString path;
		bool closed;
		double width;
	};
	Array<Case> selected{
		{ "right-angle", { { 40, 180 }, { 220, 180 }, { 220, 35 } }, false, 24 },
		{ "cubic-s", Bezier3{ { 30, 175 }, { 80, -30 }, { 270, 270 }, { 330, 60 } }.toLineStringAdaptive(), false, 18 },
		{ "closed-seam", { { 65, 185 }, { 285, 185 }, { 285, 55 }, { 65, 55 } }, true, 24 },
		{ "straight", { { 0, 0 }, { 500, 0 } }, false, 12 }
	};
	LineString longPath;
	for (int32 i = 0; i <= 512; ++i) longPath.push_back({ i * 4.0, 40 * std::sin(i * 0.025) });
	selected.push_back({ "dense-513", longPath, false, 2 });
	const char* methods[]{ "public", "cached_runs", "solid" };
	constexpr int32 Batch = 128;
	for (const auto& c : selected)
	for (const auto preset : { LineStyle::Dashed, LineStyle::DashDot, LineStyle::RoundDot })
	for (const bool changing : { false, true })
	{
		struct SavedRun { LineString points; bool closed; };
		std::array<Array<SavedRun>, 32> saved;
		std::array<Array<detail::DashedPath::Dot>, 32> dots;
		Workspace workspace;
		// Prepare the production cutter's output outside timing. The baseline
		// measures only submission of stored runs, with the same geometry.
		for (int32 i = 0; i < 32; ++i)
		{
			REQUIRE(Split(workspace, c.path, c.closed, c.width, preset(i * 0.125)));
			dots[i] = workspace.dots;
			for (size_t j = 0; j < workspace.runs.size(); ++j)
			{
				saved[i].push_back({ RunPoints(workspace, j), workspace.runs[j].closed });
			}
		}
		std::array<Array<double>, 3> wall;
# if SIV3D_PLATFORM(MACOS)
		std::array<Array<double>, 3> cpu;
# endif
		std::array<int64, 3> triangles{}, draws{};
		for (int32 round = -8; round < 21; ++round)
		for (int32 order = 0; order < 3; ++order)
		{
			const int32 method = ((round + order + 9) % 3);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
				// Match the submission benchmark's per-sample CPU warmup: update
				// and presentation otherwise leave these short draws at idle clocks.
				const int64 warmupEnd = (Time::GetNanosec() + 5'000'000);
				while (Time::GetNanosec() < warmupEnd) {}
# if SIV3D_PLATFORM(MACOS)
				const int64 cpuBegin = cpuTime();
# endif
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < Batch; ++i)
				{
					const int32 index = (changing ? (i % 32) : 0);
					const LineStyle style{ preset(index * 0.125) };
					if (method == 0)
					{
						if (c.closed) c.path.drawClosed(style, c.width);
						else c.path.draw(style, c.width);
					}
					else if (method == 1)
					{
						for (const auto& dot : dots[index]) SIV3D_ENGINE(Renderer2D)->addRoundDot(dot.start, dot.end, static_cast<float>(c.width), dot.uRange, Float4{ 1, 1, 1, 1 });
						for (const auto& run : saved[index])
						{
							if (run.closed) run.points.drawClosed(c.width);
							else run.points.draw(LineCap::Flat, c.width);
						}
					}
					else if (c.closed) c.path.drawClosed(c.width);
					else c.path.draw(LineCap::Square, c.width);
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Batch;
# if SIV3D_PLATFORM(MACOS)
				const double cpuNs = double(cpuTime() - cpuBegin) / Batch;
				if (round >= 0) cpu[method].push_back(cpuNs);
# endif
				if (round >= 0) wall[method].push_back(ns);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) { triangles[method] = metrics.triangleCount; draws[method] = metrics.drawCalls; }
			if (round >= 0) { CHECK(metrics.triangleCount == triangles[method]); CHECK(metrics.drawCalls == draws[method]); }
		}
		for (int32 method = 0; method < 3; ++method)
		{
			if (method < 2) { CHECK(triangles[method] == triangles[0]); CHECK(draws[method] == draws[0]); }
			wall[method].sort();
			std::cout << "DashedPath " << c.name << " pattern=" << int(preset.type) << (changing ? " changing " : " fixed ")
				<< methods[method] << " ns/call=" << wall[method][10] << " p10=" << wall[method][2] << " p90=" << wall[method][18]
				<< " triangles/call=" << double(triangles[method]) / Batch << " draws/batch=" << draws[method];
# if SIV3D_PLATFORM(MACOS)
			cpu[method].sort();
			std::cout << " cpu_ns/call=" << cpu[method][10] << " cpu_p10=" << cpu[method][2] << " cpu_p90=" << cpu[method][18];
# endif
			std::cout << '\n';
		}
	}
	// Public curve costs include adaptive sampling. The sampled-line row isolates
	// its contribution; the solid row has different output and is only context.
	const Bezier2 q{ { 35, 190 }, { 180, -110 }, { 325, 190 } };
	const Bezier3 b{ { 30, 175 }, { 80, -30 }, { 270, 270 }, { 330, 60 } };
	const Spline2D spline = Spline2D::FromCatmullRom(LineString{ { 30, 170 }, { 90, 40 }, { 180, 180 }, { 300, 60 } });
	for (int32 kind = 0; kind < 3; ++kind)
	{
		const auto line = (kind == 0 ? q.toLineStringAdaptive() : kind == 1 ? b.toLineStringAdaptive() : spline.toLineStringAdaptive());
		std::array<Array<double>, 3> wall;
		std::array<int64, 3> triangles{}, draws{};
		for (int32 round = -8; round < 21; ++round)
		for (int32 order = 0; order < 3; ++order)
		{
			const int32 method = ((round + order + 9) % 3);
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
				// Match the submission benchmark's per-sample CPU warmup: update
				// and presentation otherwise leave these short draws at idle clocks.
				const int64 warmupEnd = (Time::GetNanosec() + 5'000'000);
				while (Time::GetNanosec() < warmupEnd) {}
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < Batch; ++i)
				{
					const LineStyle style{ LineStyle::Dashed((i % 32) * 0.125) };
					if (method == 0)
					{
						if (kind == 0) q.draw(style, 8);
						else if (kind == 1) b.draw(style, 8);
						else spline.draw(style, 8);
					}
					else if (method == 1) line.draw(style, 8);
					else if (kind == 0) q.draw(LineCap::Round, 8);
					else if (kind == 1) b.draw(LineCap::Round, 8);
					else spline.draw(8);
				}
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Batch;
				if (round >= 0) wall[method].push_back(ns);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) { triangles[method] = metrics.triangleCount; draws[method] = metrics.drawCalls; }
			if (round >= 0) { CHECK(metrics.triangleCount == triangles[method]); CHECK(metrics.drawCalls == draws[method]); }
		}
		CHECK(triangles[0] == triangles[1]); CHECK(draws[0] == draws[1]);
		for (int32 method = 0; method < 3; ++method)
		{
			wall[method].sort();
			std::cout << "DashedCurve " << kind << ' ' << (method == 0 ? "public" : method == 1 ? "sampled_line" : "solid")
				<< " ns/call=" << wall[method][10] << " p10=" << wall[method][2] << " p90=" << wall[method][18]
				<< " triangles/call=" << double(triangles[method]) / Batch << " draws/batch=" << draws[method] << '\n';
		}
	}
}
