//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <chrono>
# include <cmath>
# include <iostream>
# if SIV3D_PLATFORM(MACOS)
# include <pthread.h>
# include <pthread/qos.h>
# include <time.h>
# endif

namespace
{
	struct BenchmarkScene
	{
		const SceneConfig config = Scene::GetConfig();
		const bool vsync = Graphics::IsVSyncEnabled();
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
# if SIV3D_PLATFORM(MACOS)
		pthread_override_t qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
# endif
		BenchmarkScene()
		{
			Scene::Configure(SceneConfig::Fixed({ 320, 240 }));
			Graphics::SetVSyncEnabled(false);
		}
		~BenchmarkScene()
		{
			Scene::Configure(config);
			Graphics::SetVSyncEnabled(vsync);
# if SIV3D_PLATFORM(MACOS)
			if (qos) pthread_override_qos_class_end_np(qos);
# endif
		}
	};

# if SIV3D_PLATFORM(MACOS)
	int64 ThreadTime()
	{
		timespec t{};
		REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t) == 0);
		return (int64(t.tv_sec) * 1'000'000'000 + t.tv_nsec);
	}
# endif

	template <class Draw>
	void MeasureSubmission(const char* name, const size_t parameter, Draw&& draw)
	{
		constexpr int32 Calls = 128;
		Array<double> wall;
# if SIV3D_PLATFORM(MACOS)
		Array<double> cpu;
# endif
		int64 triangles = 0, drawCalls = 0;
		for (int32 round = -8; round < 21; ++round)
		{
			REQUIRE(System::Update());
			{
				const Transformer2D offscreen{ Mat3x2::Translate(10000, 10000) };
# if SIV3D_PLATFORM(MACOS)
				const auto cpuBegin = ThreadTime();
# endif
				const auto begin = std::chrono::steady_clock::now();
				for (int32 i = 0; i < Calls; ++i) draw();
				const double ns = std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count() / Calls;
# if SIV3D_PLATFORM(MACOS)
				const double cpuNs = double(ThreadTime() - cpuBegin) / Calls;
				if (round >= 0) cpu.push_back(cpuNs);
# endif
				if (round >= 0) wall.push_back(ns);
			}
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			if (round == 0) { triangles = metrics.triangleCount; drawCalls = metrics.drawCalls; }
			if (round >= 0)
			{
				CHECK(metrics.triangleCount == triangles);
				CHECK(metrics.drawCalls == drawCalls);
			}
		}
		wall.sort();
		std::cout << "Preparation " << name << " parameter=" << parameter << " wall_ns=" << wall[10]
			<< " p10=" << wall[2] << " p90=" << wall[18];
# if SIV3D_PLATFORM(MACOS)
		cpu.sort();
		std::cout << " cpu_ns=" << cpu[10];
# endif
		std::cout << " triangles/call=" << double(triangles) / Calls << " drawCalls/batch=" << drawCalls << '\n';
	}
}

TEST_CASE("Renderer2DPreparation.closed_frames_benchmark", "[.benchmark]")
{
	const BenchmarkScene scene;
	for (const double radius : { 1.0, 40.0, 1000.0 })
	{
		const Circle circle{ 160, 120, radius };
		const Ellipse ellipse{ 160, 120, radius, radius * 0.5 };
		MeasureSubmission("CircleFrame", size_t(radius), [&] { circle.drawFrame(radius * 0.2); });
		MeasureSubmission("EllipseFrame", size_t(radius), [&] { ellipse.drawFrame(radius * 0.2); });
		MeasureSubmission("ShadowRing", size_t(radius), [&] { circle.drawShadow(Vec2{ 0, 0 }, radius * 0.25, 0, ColorF{ 0, 0.5 }, false); });
		MeasureSubmission("ShadowFill", size_t(radius), [&] { circle.drawShadow(Vec2{ 0, 0 }, radius * 0.25); });
	}
}

TEST_CASE("Renderer2DPreparation.gradient_lines_benchmark", "[.benchmark]")
{
	const BenchmarkScene scene;
	for (const size_t count : { 3u, 40u, 512u })
	for (const bool sharp : { false, true })
	{
		LineString points;
		for (size_t i = 0; i < count; ++i)
		{
			points << Vec2{ double(i * 8), sharp ? double((i % 2) * 32) : 20 * std::sin(i * 0.1) };
		}
		MeasureSubmission(sharp ? "SharpSolid" : "SmoothSolid", count,
			[&] { points.draw(LineCap::Flat, 4, Palette::Red); });
		MeasureSubmission(sharp ? "SharpGradient" : "SmoothGradient", count,
			[&] { points.draw(LineCap::Flat, 4, Palette::Red, Palette::Blue); });
	}
	const Bezier2 quadratic{ { 0, 0 }, { 100, 300 }, { 300, 0 } };
	const Bezier3 cubic{ { 0, 0 }, { 100, 300 }, { 200, -200 }, { 300, 0 } };
	MeasureSubmission("Bezier2Solid", 0, [&] { quadratic.draw(4, Palette::Red); });
	MeasureSubmission("Bezier2Gradient", 0, [&] { quadratic.draw(4, Arg::start = ColorF{ Palette::Red }, Arg::end = ColorF{ Palette::Blue }); });
	MeasureSubmission("Bezier3Solid", 0, [&] { cubic.draw(4, Palette::Red); });
	MeasureSubmission("Bezier3Gradient", 0, [&] { cubic.draw(4, Arg::start = ColorF{ Palette::Red }, Arg::end = ColorF{ Palette::Blue }); });
}
