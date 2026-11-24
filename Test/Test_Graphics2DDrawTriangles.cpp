//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Renderer2D/Renderer2DDrawSubmission.hpp>
# include <limits>
# include <cstring>
# include <iostream>

# if SIV3D_PLATFORM(WINDOWS)
# include "../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Renderer2D/D3D11/D3D11Renderer2DCommandManager.hpp"
# else
# include "../Siv3D/src/Siv3D-Platform/macOS/Siv3D/Renderer2D/Metal/MetalRenderer2DCommandManager.hpp"
# endif

namespace
{
# if SIV3D_PLATFORM(WINDOWS)
	using CommandManager = D3D11Renderer2DCommandManager;
	using CommandType = D3D11Renderer2DCommandType;
# else
	using CommandManager = MetalRenderer2DCommandManager;
	using CommandType = MetalRenderer2DCommandType;
# endif

	struct Parameters
	{
		Float4 originSize{ 20, 20, 24, 24 };
		Float4 stepRotation{ 32, 0, 1, 0 };
		Float4 color{ 1, 0.25f, 0.5f, 0.5f };
	};

	VertexShader LoadVS()
	{
		return HLSL{ U"example/shader/hlsl/procedural_triangles.hlsl", U"VS_Procedural" }
			| MSL{ U"example/shader/msl/procedural_triangles.metal", U"VS_Procedural" };
	}

	PixelShader LoadPS()
	{
		return HLSL{ U"example/shader/hlsl/procedural_triangles.hlsl", U"PS_Procedural" }
			| MSL{ U"example/shader/msl/procedural_triangles.metal", U"PS_Procedural" };
	}

	struct SceneScope
	{
		SceneConfig config = Scene::GetConfig();
		MSAASampleCount samples = Scene::GetMSAASampleCount();
		ColorF background = Scene::GetBackground();
		Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		ScopedViewport2D viewport{ none };
		ScopedScissorRect2D scissor{ none };
		ScopedColorMul2D mul{ Palette::White, ScopedColorMul2D::Target::Set };
		ScopedColorAdd2D add{ 0.0, ScopedColorAdd2D::Target::Set };
		ScopedRenderStates2D states{ BlendState::Default2D, RasterizerState::Default2D };

		SceneScope()
		{
			REQUIRE(System::Update());
			Scene::Configure(SceneConfig::Fixed({ 320, 240 }, SceneResolution::SceneSize));
			Scene::SetMSAASampleCount(MSAASampleCount::NoMSAA);
			Scene::SetBackground(Palette::Black);
			// D3D11 clears at frame start; apply the new background before capturing.
			REQUIRE(System::Update());
		}

		~SceneScope()
		{
			Scene::SetBackground(background);
			Scene::SetMSAASampleCount(samples);
			Scene::Configure(config);
		}
	};

	struct Capture
	{
		Image image;
		FrameMetrics metrics;
	};

	template <class Draw>
	Capture CaptureDraw(Draw&& draw)
	{
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return { ScreenCapture::GetFrame(), Profiler::GetFrameMetrics() };
	}

	void CheckImage(const Image& actual, const Image& expected)
	{
		REQUIRE(actual.size() == expected.size());
		int32 maximumDifference = 0;
		size_t maximumDifferenceIndex = 0;
		for (size_t i = 0; i < (actual.size_bytes() / sizeof(Color)); ++i)
		{
			const auto a = actual.data()[i], b = expected.data()[i];
			const int32 difference = Max(Max(Abs(int32(a.r) - b.r), Abs(int32(a.g) - b.g), Abs(int32(a.b) - b.b)), Abs(int32(a.a) - b.a));
			if (maximumDifference < difference)
			{
				maximumDifference = difference;
				maximumDifferenceIndex = i;
			}
		}
		CAPTURE(maximumDifferenceIndex, actual.data()[maximumDifferenceIndex], expected.data()[maximumDifferenceIndex]);
		CHECK(maximumDifference <= 1);
	}

	void DrawReference(const Parameters& p, const uint32 count)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			const Float2 origin = Float2{ p.originSize.x, p.originSize.y } + Float2{ p.stepRotation.x, p.stepRotation.y } * float(i);
			const Float2 x{ p.originSize.z * p.stepRotation.z, p.originSize.z * p.stepRotation.w };
			const Float2 y{ -p.originSize.w * p.stepRotation.w, p.originSize.w * p.stepRotation.z };
			Triangle{ Vec2{ origin }, Vec2{ origin + x }, Vec2{ origin + y } }.draw(ColorF{ p.color });
		}
	}
}

TEST_CASE("Graphics2DDrawTriangles.command_order_and_counts")
{
	CommandManager manager;
	manager.flush();
	const size_t first = manager.getCommands().size();
	manager.pushDraw(3);
	manager.pushColorMul(Float4{ 0, 1, 0, 1 });
	manager.pushColorMul(Float4{ 1, 1, 1, 1 }); // Cancel the state change, still preserve draw order.
	manager.pushProceduralDraw(3);
	manager.pushProceduralDraw(0x03000000u); // Exceeds both the 16-bit and the 24-bit ranges.
	manager.pushDraw(6);
	manager.pushProceduralDraw(std::numeric_limits<uint32>::max());
	manager.pushDraw(9);
	manager.flush();
	const auto& commands = manager.getCommands();
	REQUIRE(commands.size() == first + 6);
	const std::array types{ CommandType::Draw, CommandType::DrawProcedural, CommandType::DrawProcedural,
		CommandType::Draw, CommandType::DrawProcedural, CommandType::Draw };
	const std::array counts{ 3u, 3u, 0x03000000u, 6u, std::numeric_limits<uint32>::max(), 9u };
	for (size_t i = 0; i < types.size(); ++i)
	{
		REQUIRE(commands[first + i].type == types[i]);
		const uint32 index = commands[first + i].index;
		CHECK((types[i] == CommandType::Draw ? manager.getDraw(index).indexCount : manager.getProceduralDraw(index)) == counts[i]);
	}
# if SIV3D_PLATFORM(WINDOWS)
	CHECK(manager.getDraw(commands[first + 3].index).startIndex == 3);
	CHECK(manager.getDraw(commands[first + 5].index).startIndex == 9);
# else
	CHECK(manager.hasDraws());
# endif
	manager.reset();
# if SIV3D_PLATFORM(MACOS)
	CHECK_FALSE(manager.hasDraws());
	manager.pushProceduralDraw(3);
	CHECK(manager.hasDraws());
	manager.reset();
	CHECK_FALSE(manager.hasDraws());
# endif
	manager.pushProceduralDraw(6);
	CHECK(manager.getProceduralDraw(manager.getCommands().back().index) == 6);
	CHECK(manager.getCommands().back().index == 0);
}

TEST_CASE("Graphics2DDrawTriangles.batch_and_constant_boundaries")
{
	CommandManager manager;
	manager.flush();
	manager.pushDraw(3);
# if SIV3D_PLATFORM(WINDOWS)
	manager.deferUpdateBuffers(2);
# else
	manager.pushBaseVertex(65535);
# endif
	const Float4 outer{ 1, 2, 3, 4 }, inner{ 5, 6, 7, 8 };
	const uint32 saved = manager.beginConstantBufferScope(ShaderStage::Vertex, 2, &outer, sizeof(outer));
	manager.pushProceduralDraw(3);
	const uint32 nested = manager.beginConstantBufferScope(ShaderStage::Vertex, 2, &inner, sizeof(inner));
	manager.pushProceduralDraw(6);
	manager.endConstantBufferScope(ShaderStage::Vertex, 2, nested);
	manager.pushProceduralDraw(9);
	manager.endConstantBufferScope(ShaderStage::Vertex, 2, saved);
	manager.pushDraw(6);
	manager.flush();
	Array<uint32> draws;
	Array<Float4> snapshots;
	for (const auto& command : manager.getCommands())
	{
		if (command.type == CommandType::DrawProcedural)
		{
			draws.push_back(manager.getProceduralDraw(command.index));
		}
		if (command.type == CommandType::SetConstantBuffer)
		{
			const auto& store = manager.getConstantBuffers();
			const auto& cb = store.get(command.index);
			if (cb.size)
			{
				Float4 value;
				std::memcpy(&value, store.data(cb), sizeof(value));
				snapshots.push_back(value);
			}
		}
	}
	CHECK(draws == Array<uint32>{ 3, 6, 9 });
	CHECK(snapshots == Array<Float4>{ outer, inner, outer });
	const auto& commands = manager.getCommands();
	REQUIRE(commands.back().type == CommandType::Draw);
# if SIV3D_PLATFORM(WINDOWS)
	REQUIRE(commands[commands.size() - 2].type == CommandType::UpdateBuffers);
	CHECK(commands[commands.size() - 2].index == 2);
	CHECK(manager.getDraw(commands.back().index).startIndex == 0);
# else
	CHECK(manager.getDraw(commands.back().index).baseVertex == 65535);
# endif
}

TEST_CASE("Graphics2DDrawTriangles.shader_selection")
{
	struct Commands
	{
		Array<uint32> events;
		void pushEnginePS(uint32 value) { events.push_back(value); }
		void pushProceduralDraw(uint32 value) { events.push_back(value); }
	};
	struct Shaders { uint32 psShape = 42; };
	struct Custom { bool vs = true, ps = false; };
	for (const bool customPS : { false, true })
	{
		Commands commands;
		Renderer2DDrawSubmission::SubmitProcedural(commands, Shaders{}, Custom{ true, customPS }, 3);
		CHECK(commands.events == (customPS ? Array<uint32>{ 3 } : Array<uint32>{ 42, 3 }));
	}
}

TEST_CASE("Graphics2DDrawTriangles.zero_preserves_batch")
{
	const SceneScope scope;
	const auto actual = CaptureDraw([]
	{
		Graphics2D::DrawTriangles(0); // No custom VS required.
		Rect{ 20, 20, 20, 20 }.draw(Palette::Red);
		Graphics2D::DrawTriangles(0);
		Rect{ 40, 20, 20, 20 }.draw(Palette::Red);
	});
	CHECK(actual.metrics.drawCalls == 1);
	CHECK(actual.metrics.triangleCount == 4);
	CHECK(actual.image[25][25] == Palette::Red);
	CHECK(actual.image[25][45] == Palette::Red);
}

TEST_CASE("Graphics2DDrawTriangles.images_and_vertex_id_reset")
{
	const SceneScope scope;
	const VertexShader vs = LoadVS();
	const PixelShader ps = LoadPS();
	REQUIRE(vs);
	REQUIRE(ps);
	const Parameters p;
	const auto reference = CaptureDraw([&]
	{
		DrawReference(p, 1);
		DrawReference(p, 1);
		DrawReference(p, 3);
	});
	for (const bool customPS : { false, true })
	{
		const ScopedCustomShader2D shader{ vs };
		const ScopedVSConstantBuffer2D constants{ 2, ConstantBuffer<Parameters>{ p } };
		for (int32 frame = 0; frame < 4; ++frame)
		{
			const auto draw = [&]
			{
				Graphics2D::DrawTriangles(1);
				Graphics2D::DrawTriangles(1);
				Graphics2D::DrawTriangles(3);
			};
			const auto actual = CaptureDraw([&]
			{
				if (customPS) { const ScopedCustomShader2D pixel{ ps }; draw(); }
				else draw();
			});
			CheckImage(actual.image, reference.image);
			CHECK(actual.metrics.drawCalls == 3);
			CHECK(actual.metrics.triangleCount == 5);
		}
	}
}

TEST_CASE("Graphics2DDrawTriangles.mixed_draws_snapshots_and_flush")
{
	const SceneScope scope;
	const VertexShader vs = LoadVS();
	REQUIRE(vs);
	const Texture texture{ Image{ 2, 2, Palette::Blue } };
	REQUIRE(texture);
	Parameters a, b;
	b.originSize = Float4{ 40, 30, 40, 40 };
	b.color = Float4{ 0, 1, 0, 0.5f };
	const auto reference = CaptureDraw([&]
	{
		texture.resized(32, 32).draw(16, 16);
		DrawReference(a, 2);
		DrawReference(b, 1);
		DrawReference(a, 1);
		Rect{ 60, 30, 12, 12 }.draw(Palette::Yellow);
	});
	for (const bool flush : { false, true })
	{
		const auto actual = CaptureDraw([&]
		{
			texture.resized(32, 32).draw(16, 16);
			{
				// Both the shader and the source buffers can die before execution.
				const ScopedCustomShader2D shader{ LoadVS() };
				ConstantBuffer<Parameters> parameters{ a };
				const ScopedVSConstantBuffer2D outer{ 2, parameters };
				if (flush) Graphics2D::Flush();
				Graphics2D::DrawTriangles(2);
				parameters = b;
				{
					const ScopedVSConstantBuffer2D inner{ 2, parameters };
					parameters->color = Float4{ 0, 0, 1, 1 };
					if (flush) { Graphics2D::Flush(); Graphics2D::Flush(); }
					Graphics2D::DrawTriangles(1);
				}
				if (flush) Graphics2D::Flush();
				Graphics2D::DrawTriangles(1);
			}
			Rect{ 60, 30, 12, 12 }.draw(Palette::Yellow);
		});
		CheckImage(actual.image, reference.image);
		CHECK(actual.metrics.drawCalls == 5);
		CHECK(actual.metrics.triangleCount == 8);
	}
}

TEST_CASE("Graphics2DDrawTriangles.states_and_batch_ranges")
{
	const SceneScope scope;
	const VertexShader vs = LoadVS();
	REQUIRE(vs);
	const Parameters p;
	const auto draw = [&](const bool procedural)
	{
		const Transformer2D camera{ Mat3x2::Translate(16, 8) };
		const Transformer2D local{ Mat3x2::Scale(2).translated(20, 10) };
		const ScopedQuadWarp2D warp{ Mat3x3{ 1, 0, 0.001f, 0, 1, 0.002f, 0, 0, 1 } };
		const ScopedColorMul2D mul{ ColorF{ 0.5, 1, 0.75, 0.5 } };
		const ScopedColorAdd2D add{ 0.1, 0.2, 0.05 };
		const ScopedViewport2D viewport{ Rect{ 10, 10, 280, 200 } };
		const ScopedScissorRect2D scissor{ Rect{ 40, 30, 120, 100 } };
		const auto triangles = [&]
		{
			if (procedural) { const ScopedCustomShader2D shader{ vs }; Graphics2D::DrawTriangles(4); }
			else DrawReference(p, 4);
		};
		triangles();
		{ const ScopedRenderStates2D blend{ BlendState::Additive }; triangles(); }
		for (const auto raster : { RasterizerState::WireframeCullNone, RasterizerState::SolidCullBack, RasterizerState::SolidCullFront })
		{
			const ScopedRenderStates2D state{ raster };
			triangles();
		}
	};
	const ScopedVSConstantBuffer2D constants{ 2, ConstantBuffer<Parameters>{ p } };
	const auto reference = CaptureDraw([&] { draw(false); });
	const auto actual = CaptureDraw([&] { draw(true); });
	CheckImage(actual.image, reference.image);
	CHECK(actual.metrics.drawCalls == reference.metrics.drawCalls);
	CHECK(actual.metrics.triangleCount == reference.metrics.triangleCount);

	const auto mixed = [&]
	{
		Rect{ 10, 10, 12, 12 }.draw(Palette::Lime);
		{ const ScopedCustomShader2D shader{ vs }; Graphics2D::DrawTriangles(3); }
		Rect{ 30, 30, 12, 12 }.draw(Palette::Blue);
	};
	const auto smallBatch = CaptureDraw(mixed);
	const auto large = CaptureDraw([&]
	{
		for (int32 i = 0; i < 16383; ++i) Rect{ -10, -10, 1, 1 }.draw();
		mixed();
		Graphics2D::Flush();
	});
	CheckImage(large.image, smallBatch.image);
	CHECK(large.metrics.triangleCount == smallBatch.metrics.triangleCount + 32766);
}

TEST_CASE("Graphics2DDrawTriangles.msaa_and_resolution")
{
	const SceneScope scope;
	const VertexShader vs = LoadVS();
	REQUIRE(vs);
	Parameters p;
	p.originSize = Float4{ 20.25f, 20.5f, 43.5f, 31.25f };
	const ScopedVSConstantBuffer2D constants{ 2, ConstantBuffer<Parameters>{ p } };
	for (const auto samples : { MSAASampleCount::NoMSAA, MSAASampleCount::X4MSAA })
	{
		Scene::SetMSAASampleCount(samples);
		for (const auto resolution : { SceneResolution::SceneSize, SceneResolution::Display })
		{
			Scene::Configure(SceneConfig::Fixed({ 320, 240 }, resolution));
			const auto reference = CaptureDraw([&] { DrawReference(p, 2); DrawReference(p, 2); });
			for (int32 frame = 0; frame < 4; ++frame)
			{
				const auto actual = CaptureDraw([&]
				{
					const ScopedCustomShader2D shader{ vs };
					Graphics2D::Flush(); // First pass can be empty; subsequent passes must load it.
					Graphics2D::DrawTriangles(2);
					Graphics2D::Flush();
					Graphics2D::DrawTriangles(2);
				});
				CheckImage(actual.image, reference.image);
				CHECK(actual.metrics.drawCalls == 2);
				CHECK(actual.metrics.triangleCount == 4);
			}
		}
	}
}

TEST_CASE("Graphics2DDrawTriangles.cpu_benchmark", "[.benchmark]")
{
	const SceneScope scope;
	const bool vsync = Graphics::IsVSyncEnabled();
	const ScopeExit restoreVSync{ [=] { Graphics::SetVSyncEnabled(vsync); } };
	Graphics::SetVSyncEnabled(false);
	const VertexShader vs = LoadVS();
	REQUIRE(vs);
	Parameters p;
	p.originSize = Float4{ -16, -16, 1, 1 };
	p.stepRotation = Float4{ 0, 0, 1, 0 };
	const ScopedVSConstantBuffer2D constants{ 2, ConstantBuffer<Parameters>{ p } };
	constexpr uint32 Count = 1024;
	constexpr int32 Warmup = 12, Samples = 41;
	const std::array names{ "indexed", "procedural-single", "procedural-bulk", "mixed" };
	for (int32 mode = 0; mode < 4; ++mode)
	{
		Array<int64> submission, replay;
		for (int32 sample = 0; sample < (Warmup + Samples); ++sample)
		{
			const int64 start = Time::GetNanosec();
			if (mode == 0) DrawReference(p, Count);
			else if (mode < 3)
			{
				const ScopedCustomShader2D shader{ vs };
				if (mode == 1) for (uint32 i = 0; i < Count; ++i) Graphics2D::DrawTriangles(1);
				else Graphics2D::DrawTriangles(Count);
			}
			else
			{
				for (uint32 i = 0; i < (Count / 2); ++i)
				{
					DrawReference(p, 1);
					const ScopedCustomShader2D shader{ vs };
					Graphics2D::DrawTriangles(1);
				}
			}
			const int64 submitted = Time::GetNanosec();
			Graphics2D::Flush();
			const int64 flushed = Time::GetNanosec();
			REQUIRE(System::Update());
			const auto metrics = Profiler::GetFrameMetrics();
			REQUIRE(metrics.triangleCount == Count);
			REQUIRE(metrics.drawCalls == ((mode == 0 || mode == 2) ? 1 : Count));
			if (Warmup <= sample)
			{
				submission.push_back(submitted - start);
				replay.push_back(flushed - submitted);
			}
		}
		submission.sort();
		replay.sort();
		std::cout << "draw-triangles mode=" << names[mode] << " triangles=" << Count
			<< " submission_ns_per_triangle=" << double(submission[Samples / 2]) / Count
			<< " p10=" << double(submission[Samples / 10]) / Count
			<< " p90=" << double(submission[Samples * 9 / 10]) / Count
			<< " flush_us=" << double(replay[Samples / 2]) / 1000 << '\n';
	}
}
