//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/EngineShader/IEngineShader.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/IRenderer2D.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Renderer2DDrawSubmission.hpp"
# include <iostream>

# if SIV3D_PLATFORM(WINDOWS)
	# include <Siv3D/Windows/Windows.hpp>
# else
	# include <time.h>
# endif

# if SIV3D_PLATFORM(MACOS)
	# include <pthread.h>
	# include <pthread/qos.h>
# endif

namespace
{
	int64 ThreadTimeNanoseconds()
	{
		# if SIV3D_PLATFORM(WINDOWS)
			FILETIME creation{}, exit{}, kernel{}, user{};
			REQUIRE(GetThreadTimes(GetCurrentThread(), &creation, &exit, &kernel, &user));
			const uint64 ticks = ((uint64(kernel.dwHighDateTime) << 32) | kernel.dwLowDateTime)
				+ ((uint64(user.dwHighDateTime) << 32) | user.dwLowDateTime);
			return static_cast<int64>(ticks * 100);
		# else
			timespec time{};
			REQUIRE(clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time) == 0);
			return (int64(time.tv_sec) * 1'000'000'000 + time.tv_nsec);
		# endif
	}

	template <class Draw>
	void BenchmarkSubmission(const char* name, const int32 trianglesPerDraw, const EngineVS engineVS, const EnginePS enginePS, Draw&& draw)
	{
		constexpr int32 DrawCount = 4096;
		constexpr int32 WarmupFrames = 24;
		constexpr int32 SampleFrames = 96;
		auto* renderer = SIV3D_ENGINE(Renderer2D);
		const auto oldVS = renderer->getCustomVS();
		const auto oldPS = renderer->getCustomPS();
		const ScopeExit restoreShaders{ [&] { renderer->setCustomVS(oldVS); renderer->setCustomPS(oldPS); } };
		const auto& vs = SIV3D_ENGINE(EngineShader)->getVS(engineVS);
		const auto& ps = SIV3D_ENGINE(EngineShader)->getPS(enginePS);
		const char* modes[] = { "steady", "changing", "custom-vs", "custom-ps", "custom-both" };
		for (int32 mode = 0; mode < 5; ++mode)
		{
			renderer->setCustomVS((mode == 2 || mode == 4) ? Optional<VertexShader>{ vs } : none);
			renderer->setCustomPS((mode == 3 || mode == 4) ? Optional<PixelShader>{ ps } : none);
			Array<int64> elapsed, cpu;
			elapsed.reserve(SampleFrames);
			cpu.reserve(SampleFrames);
			REQUIRE(System::Update());
			for (int32 frame = 0; frame < WarmupFrames + SampleFrames; ++frame)
			{
				// Let the CPU leave its idle state before timing a short submission loop.
				const int64 warmupEnd = (Time::GetNanosec() + 5'000'000);
				while (Time::GetNanosec() < warmupEnd) {}
				const int64 cpuStart = ThreadTimeNanoseconds();
				const int64 start = Time::GetNanosec();
				if (mode == 1)
				{
					for (int32 i = 0; i < DrawCount; ++i)
					{
						const ScopedColorMul2D color{ (i % 2) ? Palette::Red : Palette::Blue };
						const ScopedRenderStates2D blend{ (i % 2) ? BlendState::Additive : BlendState::Default2D };
						draw();
					}
				}
				else
				{
					for (int32 i = 0; i < DrawCount; ++i) draw();
				}
				const int64 duration = (Time::GetNanosec() - start);
				const int64 cpuDuration = (ThreadTimeNanoseconds() - cpuStart);
				REQUIRE(System::Update());
				const auto metrics = Profiler::GetFrameMetrics();
				REQUIRE(metrics.triangleCount == (DrawCount * trianglesPerDraw));
				REQUIRE(metrics.drawCalls == ((mode == 1) ? DrawCount : 1));
				if (WarmupFrames <= frame)
				{
					elapsed.push_back(duration);
					cpu.push_back(cpuDuration);
				}
			}
			elapsed.sort();
			cpu.sort();
			std::cout << "submission shape=" << name << " mode=" << modes[mode]
				<< " ns=" << (double(elapsed[SampleFrames / 2]) / DrawCount)
				<< " p10=" << (double(elapsed[SampleFrames / 10]) / DrawCount)
				<< " p90=" << (double(elapsed[SampleFrames * 9 / 10]) / DrawCount)
				<< " cpu_ns=" << (double(cpu[SampleFrames / 2]) / DrawCount) << '\n';
		}
	}
}

TEST_CASE("Renderer2D.draw_submission_cpu_benchmark", "[.benchmark]")
{
	# if SIV3D_PLATFORM(MACOS)
		const auto qos = pthread_override_qos_class_start_np(pthread_self(), QOS_CLASS_USER_INTERACTIVE, 0);
		REQUIRE(qos);
		const ScopeExit restoreQoS{ [=] { pthread_override_qos_class_end_np(qos); } };
	# endif
	const bool vsync = Graphics::IsVSyncEnabled();
	const ScopeExit restoreVSync{ [=] { Graphics::SetVSyncEnabled(vsync); } };
	Graphics::SetVSyncEnabled(false);
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	const ScopedColorMul2D colorMul{ Palette::White, ScopedColorMul2D::Target::Set };
	const ScopedColorAdd2D colorAdd{ 0.0, ScopedColorAdd2D::Target::Set };
	const ScopedRenderStates2D blend{ BlendState::Default2D };
	const SceneConfig config = Scene::GetConfig();
	const ScopeExit restoreScene{ [&] { Scene::Configure(config); } };
	Scene::Configure(SceneConfig::Fixed({ 480, 280 }, SceneResolution::SceneSize));
	const Texture texture{ Image{ 2, 2, Palette::White } };
	REQUIRE(texture);
	const RectF rect{ -8, -8, 2, 2 };
	const Circle circle{ -8, -8, 1 };
	const PatternParameters pattern = Pattern::PolkaDot{};
	BenchmarkSubmission("rect", 2, EngineVS::Shape2D, EnginePS::Shape2D, [&] { rect.draw(); });
	BenchmarkSubmission("circle", 8, EngineVS::Shape2D, EnginePS::Shape2D, [&] { circle.draw(); });
	BenchmarkSubmission("pattern-rect", 2, EngineVS::Pattern2D, EnginePS::PatternPolkaDot, [&] { rect.draw(pattern); });
	BenchmarkSubmission("pattern-circle", 8, EngineVS::Pattern2D, EnginePS::PatternPolkaDot, [&] { circle.draw(pattern); });
	BenchmarkSubmission("textured-quad", 2, EngineVS::Shape2D, EnginePS::Texture2D, [&] { texture.draw(-8, -8); });
}

namespace
{
	struct SubmissionShaders
	{
		uint32 vsShape = 1, vsPattern = 2;
		uint32 psShape = 3, psTexture = 4;
		uint32 psLineDot = 5, psLineDash = 6, psLineLongDash = 7, psLineDashDot = 8, psLineRoundDot = 9;
		mutable uint32 patternLookups = 0;
		mutable PatternType requestedPattern = PatternType::PolkaDot;

		uint32 getPatternShader(const PatternType type) const
		{
			++patternLookups;
			requestedPattern = type;
			return 10;
		}
	};

	struct SubmissionCustomShaders
	{
		bool vs = false, ps = false;
	};

	struct SubmissionCommands
	{
		using Event = std::pair<char, uint32>;
		Array<Event> events;
		std::array<Float4, 4> parameters{};
		const Texture* texture = nullptr;

		void pushEngineVS(const uint32 id) { events.emplace_back('V', id); }
		void pushEnginePS(const uint32 id) { events.emplace_back('P', id); }
		void pushDraw(const uint32 count) { events.emplace_back('D', count); }
		void pushPatternParameter(const std::array<Float4, 4>& values)
		{
			parameters = values;
			events.emplace_back('C', 0);
		}
		void pushPSTexture(const uint32 slot, const Texture& value)
		{
			texture = &value;
			events.emplace_back('T', slot);
		}
	};

	Array<SubmissionCommands::Event> ExpectedSubmission(const SubmissionCustomShaders custom, const uint32 vs, const uint32 ps)
	{
		if (custom.vs && custom.ps) return {};
		if (custom.vs) return { { 'P', ps } };
		if (custom.ps) return { { 'V', vs } };
		return { { 'V', vs }, { 'P', ps } };
	}
}

TEST_CASE("Renderer2D.draw_submission_commands")
{
	// Independently suppress the engine VS / PS, preserving material bindings and draw order.
	for (const SubmissionCustomShaders custom : { SubmissionCustomShaders{}, { true, false }, { false, true }, { true, true } })
	{
		CAPTURE(custom.vs, custom.ps);
		const SubmissionShaders shaders;
		// Index counts are not restricted to the 16-bit vertex index range.
		for (const uint32 indexCount : { 6u, 98'304u })
		{
			SubmissionCommands commands;
			auto expected = ExpectedSubmission(custom, 1, 3);
			expected.emplace_back('D', indexCount);
			Renderer2DDrawSubmission::SubmitShape(commands, shaders, custom, indexCount);
			CHECK(commands.events == expected);
		}

		{
			INFO("Pattern parameters remain bound with a custom pixel shader");
			SubmissionCommands commands;
			PatternParameters pattern = Pattern::Checker{};
			pattern.backgroundColor = { 0.1f, 0.2f, 0.3f, 0.4f };
			pattern.uvTransform = Mat3x2::Scale(2).translated(3, 4);
			pattern.extraParams = { 5, 6, 7, 8 };
			auto expected = ExpectedSubmission(custom, 2, 10);
			expected.emplace_back('C', 0);
			expected.emplace_back('D', 12);
			Renderer2DDrawSubmission::SubmitPattern(commands, shaders, custom, 12, pattern);
			CHECK(commands.events == expected);
			CHECK(commands.parameters == pattern.toFloat4Array());
			CHECK(shaders.patternLookups == (custom.ps ? 0u : 1u));
			if (not custom.ps) CHECK(shaders.requestedPattern == PatternType::Checker);
		}

		{
			INFO("Texture binding remains active with a custom pixel shader");
			SubmissionCommands commands;
			const Texture texture;
			auto expected = ExpectedSubmission(custom, 1, 4);
			expected.emplace_back('T', 0);
			expected.emplace_back('D', 6);
			Renderer2DDrawSubmission::SubmitTexture(commands, shaders, custom, 6, texture);
			CHECK(commands.events == expected);
			CHECK(commands.texture == &texture);

			commands = {};
			int32 lookups = 0;
			Renderer2DDrawSubmission::SubmitTextureFrom(commands, shaders, custom, 6, [&]() -> const Texture&
			{
				CHECK(commands.events == ExpectedSubmission(custom, 1, 4));
				++lookups;
				return texture;
			});
			CHECK(lookups == 1);
			CHECK(commands.events == expected);
			CHECK(commands.texture == &texture);
		}

		{
			INFO("Each line pattern selects its pixel shader");
			const std::pair<LinePattern, uint32> patterns[] = {
				{ LinePattern::Solid, 3 }, { LinePattern::Dotted, 5 }, { LinePattern::Dashed, 6 },
				{ LinePattern::LongDash, 7 }, { LinePattern::DashDot, 8 }, { LinePattern::RoundDot, 9 }
			};
			for (const auto [pattern, ps] : patterns)
			{
				CAPTURE(pattern);
				SubmissionCommands commands;
				LineStyle style;
				style.type = pattern;
				auto expected = ExpectedSubmission(custom, 1, ps);
				expected.emplace_back('D', 12);
				Renderer2DDrawSubmission::SubmitStyledLine(commands, shaders, custom, 12, style);
				CHECK(commands.events == expected);
			}
		}
	}
}
