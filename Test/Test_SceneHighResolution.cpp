//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Scene/SceneUtility.hpp"
# include <cmath>

namespace
{
	struct RestoreScene
	{
		const SceneConfig config = Scene::GetConfig();
		const ColorF background = Scene::GetBackground();

		~RestoreScene()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
		}
	};

	Image CaptureScene()
	{
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}

	Color AtLogical(const Image& image, const Size logicalSize, const Vec2 pos)
	{
		return image[static_cast<int32>(pos.y * image.height() / logicalSize.y)]
			[static_cast<int32>(pos.x * image.width() / logicalSize.x)];
	}
}

TEST_CASE("SceneHighResolution.size_and_presentation")
{
	struct Case { Size logical; Size framebuffer; Size render; };
	for (const auto& c : {
		Case{ { 800, 600 }, { 800, 600 }, { 800, 600 } },
		Case{ { 800, 600 }, { 1600, 1200 }, { 1600, 1200 } },
		Case{ { 800, 600 }, { 3200, 2400 }, { 3200, 2400 } },
		Case{ { 800, 600 }, { 1600, 900 }, { 1200, 900 } },
		Case{ { 800, 600 }, { 600, 1600 }, { 800, 600 } },
		Case{ { 800, 600 }, { 400, 300 }, { 800, 600 } },
		Case{ { 800, 600 }, { 1000, 750 }, { 1000, 750 } },
		Case{ { 800, 600 }, { 1200, 900 }, { 1200, 900 } },
		Case{ { 333, 200 }, { 1001, 600 }, { 999, 600 } },
		Case{ { 3, 2 }, { 8, 8 }, { 8, 5 } },
		Case{ { 1, 1 }, { 1, 1 }, { 1, 1 } } })
	{
		INFO(c.logical);
		INFO(c.framebuffer);
		SceneMisc::RenderState state;
		state.size = c.logical;
		state.frameBufferSize = c.framebuffer;
		const auto [scale, rect] = state.composition();
		CHECK(state.renderSize() == c.logical);
		state.config.resolution = SceneResolution::Display;
		CHECK(state.renderSize() == c.render);
		CHECK(state.composition() == std::pair{ scale, rect });
		CHECK(rect.center().epsilonEquals(c.framebuffer * 0.5, 1e-9));
		CHECK((rect.center() - rect.pos).epsilonEquals(c.logical * (scale * 0.5), 1e-9));
		CHECK((rect.size / scale).epsilonEquals(Vec2{ c.logical }, 1e-9));
	}
}

TEST_CASE("SceneHighResolution.window_modes_and_transitions")
{
	for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
	{
		for (const bool enabled : { false, true })
		{
			SceneMisc::RenderState state;
			state.config.sizeMode = mode;
			state.config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
			WindowState window;
			window.virtualSize = { 1000, 750 };
			window.frameBufferSize = { 1500, 1125 };
			state.updateWindow(window);
			const Size expected = (mode == SceneSizeMode::Fixed) ? Size{ 800, 600 }
				: ((mode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
			CHECK(state.size == expected);
			CHECK(state.renderSize() == (enabled ? window.frameBufferSize : expected));
			// A DPI-only change keeps virtual dimensions and the fixed scene intact.
			window.frameBufferSize = { 2000, 1500 };
			state.updateWindow(window);
			CHECK(state.size == ((mode == SceneSizeMode::WindowPixels) ? window.frameBufferSize : expected));
			const auto valid = state;
			window.minimized = true;
			window.frameBufferSize = { 0, 0 };
			state.updateWindow(window);
			CHECK(state == valid);
			window.minimized = false;
			state.updateWindow(window);
			CHECK(state == valid);
			window.frameBufferSize = { 1000, 750 };
			state.updateWindow(window);
			CHECK(state.frameBufferSize == window.frameBufferSize);
		}
	}
	SceneMisc::RenderState state;
	WindowState window;
	state.configure(SceneConfig::Fixed({ 320, 240 }), window);
	state.updateWindow(window);
	CHECK(state.size == Size{ 320, 240 });
	state.configure(SceneConfig::Window(), window);
	CHECK(state.size == window.virtualSize);
}

TEST_CASE("SceneHighResolution.validation_and_api_round_trip")
{
	const RestoreScene restore;
	REQUIRE(System::Update());
	Scene::Configure(SceneConfig::Fixed(Size{ 200, 100 }, SceneResolution::SceneSize));
	CHECK(Scene::RenderResolution() == Scene::Size());
	const Vec2 cursor = Cursor::PosF();
	const Vec2 previous = Cursor::PreviousPosF();
	const auto base = Cursor::GetBaseWindowTransform();
	{
		auto config = Scene::GetConfig();
		config.resolution = SceneResolution::Display;
		Scene::Configure(config);
	}
	CHECK((Scene::GetConfig().resolution == SceneResolution::Display));
	CHECK(Scene::Size() == Size{ 200, 100 });
	CHECK(Scene::Width() == 200);
	CHECK(Scene::Height() == 100);
	CHECK(Scene::Center() == Point{ 100, 50 });
	CHECK(Scene::CenterF() == Vec2{ 100, 50 });
	CHECK(Scene::Rect() == Rect{ 200, 100 });
	CHECK(Graphics2D::GetDrawingSize() == Scene::Size());
	CHECK(Graphics2D::GetRenderTargetSize() == Scene::RenderResolution());
	CHECK(Cursor::PosF() == cursor);
	CHECK(Cursor::PreviousPosF() == previous);
	CHECK(Cursor::GetBaseWindowTransform() == base);
	CHECK(Scene::ClientToScene(Cursor::PosRaw()).epsilonEquals(base.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
	const Size render = Scene::RenderResolution();
	for (const Size invalid : { Size{ 0, 1 }, Size{ 1, 0 }, Size{ -1, 100 }, Size{ 16385, 1 }, Size{ 1, 16385 } })
	{
		CHECK_THROWS_AS(Scene::Configure(SceneConfig::Fixed(invalid)), Error);
		CHECK(Scene::Size() == Size{ 200, 100 });
		CHECK(Scene::RenderResolution() == render);
		CHECK((Scene::GetConfig().resolution == SceneResolution::Display));
	}
	CHECK_NOTHROW(SceneMisc::ValidateSceneSize({ 1, 1 }));
	CHECK_NOTHROW(SceneMisc::ValidateSceneSize({ 16384, 16384 }));
	{
		auto config = Scene::GetConfig();
		config.resolution = SceneResolution::SceneSize;
		Scene::Configure(config);
	}
	CHECK_FALSE((Scene::GetConfig().resolution == SceneResolution::Display));
	CHECK(Scene::Size() == Size{ 200, 100 });
	CHECK(Scene::RenderResolution() == Scene::Size());
}

TEST_CASE("SceneHighResolution.draw_camera_flush_and_state_restore")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), TransformCursor::Yes, Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	Scene::Configure(SceneConfig::Fixed(Size{ 200, 100 }, SceneResolution::Display));
	const Size renderSize = Scene::RenderResolution();
	REQUIRE(renderSize.x > Scene::Width());
	const Camera2D sceneCamera;
	CHECK(sceneCamera.getCenter() == Scene::CenterF());
	CHECK(sceneCamera.getRegion() == RectF{ Scene::Size() });
	CHECK(sceneCamera.getMat3x2() == Mat3x2::Identity());
	Scene::Rect().draw(Palette::Green);
	Rect{ 0, 0, 20, 20 }.draw(Palette::Red);
	Graphics2D::Flush();
	{
		const auto cameraScope = sceneCamera.createTransformer();
		const Transformer2D move{ Mat3x2::Translate(80, 30), TransformCursor::Yes };
		Rect{ 0, 0, 40, 40 }.draw(Palette::Blue);
		CHECK(Graphics2D::GetRMSScaling() == 1.0f);
		CHECK(Graphics2D::GetLocalTransform() == Mat3x2::Translate(80, 30));
		{
			const Transformer2D replace{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
			Rect{ 180, 80, 20, 20 }.draw(Palette::Yellow);
		}
	}
	const Image image = CaptureScene();
	CHECK(image.size() == renderSize);
	CHECK(AtLogical(image, { 200, 100 }, { 10, 10 }) == Palette::Red);
	CHECK(AtLogical(image, { 200, 100 }, { 100, 50 }) == Palette::Blue);
	CHECK(AtLogical(image, { 200, 100 }, { 190, 90 }) == Palette::Yellow);
	CHECK(AtLogical(image, { 200, 100 }, { 60, 80 }) == Palette::Green);
	CHECK(Graphics2D::GetRMSScaling() == 1.0f);
	// Reapplying the same configuration must not clear an already executed pass.
	Scene::Rect().draw(Palette::Red);
	Graphics2D::Flush();
	{
		auto config = Scene::GetConfig();
		config.resolution = SceneResolution::Display;
		config.fixedSceneSize = Size{ 200, 100 };
		Scene::Configure(config);
	}
	CHECK(AtLogical(CaptureScene(), { 200, 100 }, { 100, 50 }) == Palette::Red);
}

TEST_CASE("SceneHighResolution.resize_keeps_pending_draws")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	{
		auto config = Scene::GetConfig();
		config.sizeMode = SceneSizeMode::Fixed;
		Scene::Configure(config);
	}
	for (const bool enabled : { false, true })
	{
		INFO(enabled);
		{
			auto config = Scene::GetConfig();
			config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
			config.fixedSceneSize = Size{ 200, 100 };
			Scene::Configure(config);
		}
		{
			const Transformer2D move{ Mat3x2::Translate(20, 20) };
			Rect{ 20, 20 }.draw(Palette::Red);
		}
		// The old coordinates remain recorded, but the pending draw uses the new target.
		const Size oldRender = Scene::RenderResolution();
		{
			auto config = Scene::GetConfig();
			config.fixedSceneSize = Size{ 400, 200 };
			Scene::Configure(config);
		}
		if (enabled) { CHECK(Scene::RenderResolution() == oldRender); }
		Rect{ 180, 80, 40, 40 }.draw(Palette::Blue);
		const Image image = CaptureScene();
		CHECK(image.size() == Scene::RenderResolution());
		CHECK(AtLogical(image, { 400, 200 }, { 30, 30 }) == Palette::Red);
		CHECK(AtLogical(image, { 400, 200 }, { 200, 100 }) == Palette::Blue);
		CHECK(AtLogical(image, { 400, 200 }, { 350, 180 }) == Palette::Black);
	}
}

TEST_CASE("SceneHighResolution.toggle_keeps_pending_draws")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	{
		auto config = Scene::GetConfig();
		config.sizeMode = SceneSizeMode::Fixed;
		config.fixedSceneSize = Size{ 200, 100 };
		Scene::Configure(config);
	}
	for (const bool enabled : { false, true })
	{
		INFO(enabled);
		{
			auto config = Scene::GetConfig();
			config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
			Scene::Configure(config);
		}
		Scene::Rect().draw(Palette::Red);
		{
			auto config = Scene::GetConfig();
			config.resolution = (not enabled ? SceneResolution::Display : SceneResolution::SceneSize);
			Scene::Configure(config);
		}
		Rect{ 80, 30, 40, 40 }.draw(Palette::Blue);
		const Image image = CaptureScene();
		CHECK(image.size() == Scene::RenderResolution());
		CHECK(AtLogical(image, { 200, 100 }, { 10, 10 }) == Palette::Red);
		CHECK(AtLogical(image, { 200, 100 }, { 190, 90 }) == Palette::Red);
		CHECK(AtLogical(image, { 200, 100 }, { 100, 50 }) == Palette::Blue);
	}
}

TEST_CASE("SceneHighResolution.resize_discards_only_executed_drawing")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	{
		auto config = Scene::GetConfig();
		config.sizeMode = SceneSizeMode::Fixed;
		Scene::Configure(config);
	}
	for (const bool enabled : { false, true })
	{
		INFO(enabled);
		{
			auto config = Scene::GetConfig();
			config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
			config.fixedSceneSize = Size{ 200, 100 };
			Scene::Configure(config);
		}
		Scene::Rect().draw(Palette::Red);
		Graphics2D::Flush();
		// Already executed pixels are cleared; pending commands and their vertex ranges survive.
		Rect{ 20, 20, 20, 20 }.draw(Palette::Green);
		const Size oldRender = Scene::RenderResolution();
		{
			auto config = Scene::GetConfig();
			config.fixedSceneSize = Size{ 400, 200 };
			Scene::Configure(config);
		}
		if (enabled) { CHECK(Scene::RenderResolution() == oldRender); }
		Rect{ 160, 60, 80, 80 }.draw(Palette::Yellow);
		const Image image = CaptureScene();
		CHECK(AtLogical(image, { 400, 200 }, { 10, 10 }) == Palette::Black);
		CHECK(AtLogical(image, { 400, 200 }, { 30, 30 }) == Palette::Green);
		CHECK(AtLogical(image, { 400, 200 }, { 200, 100 }) == Palette::Yellow);
	}
}

TEST_CASE("SceneHighResolution.pending_batches_and_clipping_after_shrink")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedViewport2D viewport{ none };
	const ScopedScissorRect2D scissor{ none };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	Scene::Configure(SceneConfig::Fixed(Size{ 400, 300 }, SceneResolution::SceneSize));
	{
		// The command stream, not the scene buffer, keeps this texture alive.
		const Texture texture{ Image{ 4, 4, Palette::Green } };
		REQUIRE(texture);
		texture.resized(400, 300).draw();
	}
	{
		const ScopedViewport2D oldViewport{ Rect{ 400, 300 } };
		const ScopedScissorRect2D oldScissor{ Rect{ 0, 0, 100, 300 } };
		Rect{ 400, 300 }.draw(Palette::Red);
	}
	{
		const ScopedScissorRect2D outside{ Rect{ 300, 200, 100, 100 } };
		Rect{ 400, 300 }.draw(Palette::White);
	}
	// Cross a 16-bit vertex batch boundary while targets are repeatedly replaced.
	for (int32 i = 0; i < 17000; ++i)
	{
		Rect{ -10, -10, 1, 1 }.draw();
		if (i == 8000)
		{
			auto config = Scene::GetConfig();
			config.fixedSceneSize = Size{ 600, 450 };
			Scene::Configure(config);
		}
		if (i == 16000)
		{
			auto config = Scene::GetConfig();
			config.fixedSceneSize = Size{ 300, 225 };
			Scene::Configure(config);
		}
	}
	{
		auto config = Scene::GetConfig();
		config.fixedSceneSize = Size{ 200, 150 };
		Scene::Configure(config);
	}
	Rect{ 140, 50, 40, 40 }.draw(Palette::Blue);
	const Image image = CaptureScene();
	REQUIRE(image.size() == Size{ 200, 150 });
	CHECK(image[75][50] == Palette::Red);
	CHECK(image[120][150] == Palette::Green);
	CHECK(image[75][160] == Palette::Blue);
	CHECK(Profiler::GetFrameMetrics().triangleCount == 34008);
	// A following frame must start with intact state and new vertex ranges.
	Scene::Rect().draw(Palette::Yellow);
	const Image nextFrame = CaptureScene();
	CHECK(nextFrame[75][100] == Palette::Yellow);
}

TEST_CASE("SceneHighResolution.pixel_viewport_and_scissor")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	Scene::Configure(SceneConfig::Fixed(Size{ 200, 100 }, SceneResolution::Display));
	const Size pixels = Scene::RenderResolution();
	const Rect viewport{ pixels.x / 4, pixels.y / 4, pixels.x / 2, pixels.y / 2 };
	{
		const ScopedViewport2D view{ viewport };
		const ScopedScissorRect2D scissor{ Rect{ viewport.pos, viewport.w / 2, viewport.h } };
		Rect{ 100, 50 }.draw(Palette::White);
	}
	const Image image = CaptureScene();
	CHECK(image[viewport.y + viewport.h / 2][viewport.x + viewport.w / 4] == Palette::White);
	CHECK(image[viewport.y + viewport.h / 2][viewport.x + viewport.w * 3 / 4] == Palette::Black);
	CHECK(image[viewport.y - 2][viewport.x + 2] == Palette::Black);
}


TEST_CASE("SceneHighResolution.standard_drawing_matches_explicit_scale")
{
	const RestoreScene restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	const ScopedViewport2D viewport{ none };
	const ScopedScissorRect2D scissor{ none };
	const Font msdf{ FontMethod::MSDF, 24 };
	const Font bitmap{ 24 };
	const VertexShader vs = HLSL{ U"example/shader/hlsl/constant_buffer.hlsl", U"VS_Offset" }
		| MSL{ U"example/shader/msl/constant_buffer.metal", U"VS_Offset" };
	REQUIRE(msdf);
	REQUIRE(bitmap);
	REQUIRE(vs);
	const ConstantBuffer<Float4> offset{ Float4{ 5, 3, 0, 0 } };
	const ScopedVSConstantBuffer2D constants{ 2, offset };
	REQUIRE(System::Update());
	{
		auto config = Scene::GetConfig();
		config.sizeMode = SceneSizeMode::Fixed;
		Scene::Configure(config);
	}
	Scene::SetBackground(ColorF{ 0.1, 0.15, 0.2 });
	const Size logical{ 400, 300 };
	{
		auto config = Scene::GetConfig();
		config.fixedSceneSize = logical;
		config.resolution = SceneResolution::Display;
		Scene::Configure(config);
	}
	const Size pixels = Scene::RenderResolution();
	REQUIRE(pixels.x > logical.x);
	const Mat3x2 scale = Mat3x2::Scale(Float2{ float(pixels.x) / logical.x, float(pixels.y) / logical.y });
	const auto draw = [&]
	{
		Circle{ 50, 50, 35 }.draw(ColorF{ 1, 0.3, 0.1, 0.7 });
		RoundRect{ 110, 20, 100, 60, 12 }.draw(Pattern::Checker{});
		Bezier3{ Vec2{ 20, 110 }, Vec2{ 120, 70 }, Vec2{ 180, 150 }, Vec2{ 220, 110 } }.draw(3, Palette::White);
		msdf(U"MSDF").draw(TextStyle::OutlineShadow(0.15, Palette::Green, Vec2{ 2, 3 }, Palette::Black), 20, 160);
		bitmap(U"Bitmap").draw(20, 220);
		const ScopedCustomShader2D shader{ vs };
		Rect{ 250, 30, 100, 100 }.draw(Palette::Orange);
	};
	draw();
	const Image highResolution = CaptureScene();
	const auto highMetrics = Profiler::GetFrameMetrics();
	CHECK(Graphics2D::GetRMSScaling() == 1.0f);
	{
		auto config = Scene::GetConfig();
		config.resolution = SceneResolution::SceneSize;
		config.fixedSceneSize = pixels;
		Scene::Configure(config);
	}
	{
		const Transformer2D explicitScale{ scale };
		draw();
	}
	const Image reference = CaptureScene();
	CHECK(highResolution == reference);
	const auto referenceMetrics = Profiler::GetFrameMetrics();
	CHECK(highMetrics.triangleCount == referenceMetrics.triangleCount);
	CHECK(highMetrics.drawCalls == referenceMetrics.drawCalls);

	// Projective transforms precede the engine scale, including with a custom VS.
	const auto h = Mat3x3::TryHomography(RectF{ 0, 0, 100, 100 }, Quad{ 50, 20, 220, 40, 190, 190, 80, 160 });
	REQUIRE(h);
	const auto drawWarp = [&]
	{
		Rect{ 100, 100 }.draw(Pattern::PolkaDot{});
		const ScopedCustomShader2D shader{ vs };
		Rect{ 20, 30, 30, 40 }.draw(ColorF{ 1, 0.4, 0.2, 0.6 });
	};
	{
		auto config = Scene::GetConfig();
		config.fixedSceneSize = logical;
		config.resolution = SceneResolution::Display;
		Scene::Configure(config);
	}
	{
		const ScopedQuadWarp2D projective{ *h };
		drawWarp();
	}
	const Image highWarp = CaptureScene();
	{
		auto config = Scene::GetConfig();
		config.resolution = SceneResolution::SceneSize;
		config.fixedSceneSize = pixels;
		Scene::Configure(config);
	}
	{
		const ScopedQuadWarp2D projective{ *h * Mat3x3{ scale } };
		drawWarp();
	}
	CHECK(highWarp == CaptureScene());
}

TEST_CASE("SceneHighResolution.window_resize_immediate_and_next_frame")
{
	const RestoreScene restore;
	const Size oldWindow = Window::GetState().virtualSize;
	const ScopeExit restoreWindow{ [&] { Window::Resize(oldWindow, CenterWindow::No); } };
	REQUIRE(System::Update());
	for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
	{
		for (const bool enabled : { false, true })
		{
			{
				auto config = Scene::GetConfig();
				config.sizeMode = mode;
				config.fixedSceneSize = Size{ 400, 300 };
				config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
				Scene::Configure(config);
			}
			for (const bool actual : { false, true })
			{
				INFO(static_cast<int32>(mode));
				INFO(enabled);
				INFO(actual);
				REQUIRE((actual ? Window::ResizeActual(720, 540, CenterWindow::No)
					: Window::Resize(640, 480, CenterWindow::No)));
				const auto check = [&]
				{
					const auto& window = Window::GetState();
					if (not actual) { CHECK(window.virtualSize == Size{ 640, 480 }); }
					const Size expected = (mode == SceneSizeMode::Fixed) ? Size{ 400, 300 }
						: ((mode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
					CHECK(Scene::Size() == expected);
					SceneMisc::RenderState reference;
					reference.size = expected;
					reference.frameBufferSize = window.frameBufferSize;
					reference.config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
					CHECK(Scene::RenderResolution() == reference.renderSize());
					CHECK(Scene::ClientToScene(window.frameBufferSize * 0.5).epsilonEquals(expected * 0.5, 1e-6));
					const auto [scale, area] = reference.composition();
					const Mat3x2 base = Mat3x2::Scale(scale).translated(area.pos);
					CHECK(Cursor::GetBaseWindowTransform() == base);
					CHECK(Cursor::PosF().epsilonEquals(base.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
				};
				check();
				REQUIRE(System::Update());
				check();
			}
		}
	}
}
