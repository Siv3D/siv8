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

namespace
{
	struct RestoreSceneConfig
	{
		const SceneConfig config = Scene::GetConfig();
		const ColorF background = Scene::GetBackground();
		const ColorF letterbox = Scene::GetLetterbox();

		~RestoreSceneConfig()
		{
			Scene::Configure(config);
			Scene::SetBackground(background);
			Scene::SetLetterbox(letterbox);
		}
	};

	Image CaptureScene()
	{
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}
}

TEST_CASE("SceneConfig.factories_and_six_uses")
{
	static_assert(SceneConfig{} == SceneConfig::Window());
	static_assert(SceneConfig::Fixed({ 800, 600 }).sizeMode == SceneSizeMode::Fixed);
	static_assert(SceneConfig::PixelArt({ 200, 150 }).scalingFilter == TextureFilter::Nearest);
	static_assert(SceneConfig::WindowPixels().fullscreen == FullscreenSceneMode::FollowWindow);

	struct Case { SceneConfig config; Size scene; Size render; Size fullscreenScene; Size fullscreenRender; };
	for (const auto& c : {
		Case{ SceneConfig::Fixed({ 800, 600 }), { 800, 600 }, { 800, 600 }, { 800, 600 }, { 800, 600 } },
		Case{ SceneConfig::Fixed({ 800, 600 }, SceneResolution::Display), { 800, 600 }, { 1600, 1200 }, { 800, 600 }, { 1440, 1080 } },
		Case{ SceneConfig::Window(), { 800, 600 }, { 800, 600 }, { 800, 600 }, { 800, 600 } },
		Case{ SceneConfig::Window(SceneResolution::Display), { 800, 600 }, { 1600, 1200 }, { 800, 600 }, { 1440, 1080 } },
		Case{ SceneConfig::PixelArt({ 200, 150 }), { 200, 150 }, { 200, 150 }, { 200, 150 }, { 200, 150 } },
		Case{ SceneConfig::WindowPixels(), { 1600, 1200 }, { 1600, 1200 }, { 1920, 1080 }, { 1920, 1080 } } })
	{
		CAPTURE(c.config.sizeMode, c.config.resolution, c.config.scalingFilter);
		WindowState window;
		window.virtualSize = { 800, 600 };
		window.frameBufferSize = { 1600, 1200 };
		SceneMisc::RenderState state;
		state.configure(c.config, window);
		CHECK(state.size == c.scene);
		CHECK(state.renderSize() == c.render);
		window.fullscreen = true;
		window.virtualSize = window.frameBufferSize = { 1920, 1080 };
		state.updateWindow(window);
		CHECK(state.size == c.fullscreenScene);
		CHECK(state.renderSize() == c.fullscreenRender);
		CHECK(state.config == c.config);
		window.fullscreen = false;
		window.virtualSize = { 800, 600 };
		window.frameBufferSize = { 1600, 1200 };
		state.updateWindow(window);
		CHECK(state.size == c.scene);
		CHECK(state.renderSize() == c.render);
		CHECK(state.config == c.config);
	}
	CHECK(Format(SceneSizeMode::WindowPixels) == U"WindowPixels");
	CHECK(Format(SceneResolution::Display) == U"Display");
	CHECK(Format(FullscreenSceneMode::KeepSize) == U"KeepSize");
}

TEST_CASE("SceneConfig.fullscreen_policy_changes_and_same_geometry_exit")
{
	WindowState window;
	window.virtualSize = { 800, 600 };
	window.frameBufferSize = { 1600, 1200 };
	SceneMisc::RenderState state;
	state.configure(SceneConfig::Window(SceneResolution::Display), window);
	window.fullscreen = true;
	window.virtualSize = window.frameBufferSize = { 1920, 1080 };
	state.updateWindow(window);
	CHECK(state.size == Size{ 800, 600 });
	CHECK(state.renderSize() == Size{ 1440, 1080 });
	CHECK(state.composition().second.pos.epsilonEquals(Vec2{ 240, 0 }, 1e-9));
	CHECK(state.composition().second.size.epsilonEquals(Vec2{ 1440, 1080 }, 1e-9));

	// Reconfiguration must use the preserved size, not the fullscreen geometry.
	auto config = state.config;
	config.resolution = SceneResolution::SceneSize;
	state.configure(config, window);
	CHECK(state.size == Size{ 800, 600 });
	CHECK(state.renderSize() == state.size);
	config.fullscreen = FullscreenSceneMode::FollowWindow;
	state.configure(config, window);
	CHECK(state.size == Size{ 1920, 1080 });
	config.fullscreen = FullscreenSceneMode::KeepSize;
	state.configure(config, window);
	window.virtualSize = window.frameBufferSize = { 1280, 720 };
	state.updateWindow(window);
	CHECK(state.size == Size{ 1920, 1080 });

	// Exit without changing either cached dimension: tracking fullscreen matters.
	window.fullscreen = false;
	state.updateWindow(window);
	CHECK(state.size == Size{ 1280, 720 });
	CHECK(state.config == config);

	// A fixed configuration selected while fullscreen also survives the exit.
	window.fullscreen = true;
	state.updateWindow(window);
	state.configure(SceneConfig::PixelArt({ 200, 150 }), window);
	CHECK(state.size == Size{ 200, 150 });
	window.fullscreen = false;
	state.updateWindow(window);
	CHECK(state.size == Size{ 200, 150 });
	CHECK(state.config == SceneConfig::PixelArt({ 200, 150 }));
}

TEST_CASE("SceneConfig.dpi_minimized_and_configuration_values")
{
	WindowState window;
	window.virtualSize = { 800, 600 };
	window.frameBufferSize = { 1600, 1200 };
	SceneMisc::RenderState state;
	auto config = SceneConfig::Window(SceneResolution::Display);
	config.fixedSceneSize = { 0, -1 }; // Unused values remain part of the configuration.
	state.configure(config, window);
	CHECK(state.config == config);
	CHECK(state.size == Size{ 800, 600 });
	window.frameBufferSize = { 800, 600 };
	state.updateWindow(window);
	CHECK(state.size == Size{ 800, 600 });
	CHECK(state.renderSize() == Size{ 800, 600 });
	window.virtualSize = window.frameBufferSize = { 320, 240 };
	state.updateWindow(window);
	CHECK(state.size == Size{ 320, 240 });

	const auto beforeMinimize = state;
	window.minimized = true;
	window.virtualSize = window.frameBufferSize = { 0, 0 };
	state.updateWindow(window);
	CHECK(state == beforeMinimize);
	state.configure(SceneConfig::Fixed({ 640, 480 }, SceneResolution::Display), window);
	CHECK(state.size == Size{ 640, 480 });
	CHECK(state.renderSize() == Size{ 640, 480 });
	window.minimized = false;
	state.updateWindow(window);
	CHECK(state.size == Size{ 640, 480 });
	window.virtualSize = { 800, 600 };
	window.frameBufferSize = { 1600, 1200 };
	state.updateWindow(window);
	CHECK(state.renderSize() == Size{ 1600, 1200 });

	// Startup in fullscreen uses the default scene size for the keep policy.
	SceneMisc::RenderState startup;
	window.fullscreen = true;
	window.virtualSize = window.frameBufferSize = { 1920, 1080 };
	startup.configure(SceneConfig{}, window);
	CHECK(startup.size == Size{ 800, 600 });
	CHECK(startup.renderSize() == Size{ 800, 600 });
}

TEST_CASE("SceneConfig.validation_boundaries")
{
	SceneMisc::RenderState state;
	WindowState window;
	for (const Size size : { Size{ 1, 1 }, Size{ 16384, 16384 } })
	{
		CHECK_NOTHROW(state.configure(SceneConfig::Fixed(size), window));
		CHECK(state.size == size);
		CHECK(state.renderSize() == size);
	}
	const auto before = state;
	for (const Size size : { Size{ 0, 1 }, Size{ 1, 0 }, Size{ -1, 100 }, Size{ 16385, 1 }, Size{ 1, 16385 } })
	{
		CHECK_THROWS_AS(state.configure(SceneConfig::Fixed(size), window), Error);
		CHECK(state == before);
	}
	window.virtualSize = window.frameBufferSize = { 32768, 24576 };
	state.configure(SceneConfig::Fixed({ 800, 600 }, SceneResolution::Display), window);
	CHECK(state.renderSize() == Size{ 32768, 24576 });
	CHECK_THROWS_AS(SceneMisc::ValidateSceneSize(state.renderSize()), Error);
}

TEST_CASE("SceneConfig.api_atomic_configuration_and_failure")
{
	const RestoreSceneConfig restore;
	REQUIRE(System::Update());
	const WindowState window = Window::GetState();
	Scene::SetBackground(Palette::Black);
	Scene::SetLetterbox(Palette::Blue);
	Scene::Configure(SceneConfig::PixelArt({ 320, 240 }));
	CHECK(Scene::GetConfig() == SceneConfig::PixelArt({ 320, 240 }));
	CHECK(Scene::RenderResolution() == Size{ 320, 240 });

	const auto fixed = SceneConfig::Fixed({ 640, 480 }, SceneResolution::Display);
	Scene::Configure(fixed);
	CHECK(Scene::GetConfig() == fixed);
	CHECK(Scene::Size() == Size{ 640, 480 });
	CHECK(Scene::GetConfig().scalingFilter == TextureFilter::Linear);
	CHECK(Scene::GetBackground() == ColorF{ Palette::Black });
	CHECK(Scene::GetLetterbox() == ColorF{ Palette::Blue });
	CHECK(Window::GetState().virtualSize == window.virtualSize);
	CHECK(Window::GetState().bounds == window.bounds);
	CHECK(Window::GetState().fullscreen == window.fullscreen);
	CHECK(Window::GetState().style == window.style);

	const Size render = Scene::RenderResolution();
	const Mat3x2 cursor = Cursor::GetBaseWindowTransform();
	for (int32 i = 0; i < 5; ++i)
	{
		auto invalid = SceneConfig::PixelArt({ 320, 240 });
		if (i == 0) { invalid.fixedSceneSize.x = 0; }
		if (i == 1) { invalid.sizeMode = static_cast<SceneSizeMode>(255); }
		if (i == 2) { invalid.resolution = static_cast<SceneResolution>(255); }
		if (i == 3) { invalid.fullscreen = static_cast<FullscreenSceneMode>(255); }
		if (i == 4) { invalid.scalingFilter = static_cast<TextureFilter>(255); }
		CHECK_THROWS_AS(Scene::Configure(invalid), Error);
		CHECK(Scene::GetConfig() == fixed);
		CHECK(Scene::Size() == Size{ 640, 480 });
		CHECK(Scene::RenderResolution() == render);
		CHECK(Cursor::GetBaseWindowTransform() == cursor);
	}

	// Editing a returned copy cannot mutate the selected policy.
	auto changed = Scene::GetConfig();
	changed.resolution = SceneResolution::SceneSize;
	CHECK(Scene::GetConfig() == fixed);
	Scene::Configure(changed);
	CHECK(Scene::RenderResolution() == Size{ 640, 480 });
	CHECK(Cursor::GetBaseWindowTransform() == cursor);
	Scene::Configure(SceneConfig::Window());
	CHECK(Scene::GetConfig() == SceneConfig{});
	CHECK(Scene::Size() == Window::GetState().virtualSize);
}

TEST_CASE("SceneConfig.unchanged_and_filter_only_preserve_executed_pixels")
{
	const RestoreSceneConfig restore;
	const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
	const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
	REQUIRE(System::Update());
	Scene::SetBackground(Palette::Black);
	Scene::Configure(SceneConfig::Fixed({ 64, 48 }));
	for (const bool changeFilter : { false, true })
	{
		Scene::Rect().draw(Palette::Red);
		Graphics2D::Flush();
		auto config = Scene::GetConfig();
		if (changeFilter) { config.scalingFilter = TextureFilter::Nearest; }
		Scene::Configure(config);
		const Image image = CaptureScene();
		REQUIRE(image.size() == Size{ 64, 48 });
		CHECK(image[24][32] == Palette::Red);
		CHECK(Scene::GetConfig() == config);
	}
}

# if SIV3D_PLATFORM(WINDOWS)

TEST_CASE("SceneConfig.Windows.maximize_minimize_restore")
{
	REQUIRE_FALSE(Window::GetState().fullscreen);
	const RestoreSceneConfig restoreScene;
	const WindowState previous = Window::GetState();
	const ScopeExit restoreWindow{ [&]
	{
		Window::Restore();
		Window::SetStyle(previous.style);
		Window::SetPos(previous.bounds.pos);
		Window::Resize(previous.virtualSize, CenterWindow::No);
		if (previous.maximized) { Window::Maximize(); }
		if (previous.minimized) { Window::Minimize(); }
		System::Sleep(500);
	} };
	const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
	const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
	Window::Restore();
	Window::SetStyle(WindowStyle::Sizable);
	REQUIRE(Window::Resize(600, 400, CenterWindow::No));
	REQUIRE(System::Update());
	for (const auto& config : { SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display),
		SceneConfig::Window(), SceneConfig::Window(SceneResolution::Display), SceneConfig::WindowPixels() })
	{
		CAPTURE(config.sizeMode, config.resolution);
		Scene::Configure(config);
		const auto windowed = Window::GetState();
		const Size initialRender = Scene::RenderResolution();
		const auto check = [&](const char* phase)
		{
			INFO(phase);
			const auto& window = Window::GetState();
			const Size expected = (config.sizeMode == SceneSizeMode::Fixed) ? config.fixedSceneSize
				: ((config.sizeMode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == expected);
			SceneMisc::RenderState reference;
			reference.config = config;
			reference.size = expected;
			reference.frameBufferSize = window.frameBufferSize;
			CHECK(Scene::RenderResolution() == reference.renderSize());
			const auto [scale, area] = reference.composition();
			CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
			const auto all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
			CHECK(Cursor::PosF().epsilonEquals(all.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
			Catch::cout() << phase << " mode=" << Format(config.sizeMode).toUTF8()
				<< " resolution=" << Format(config.resolution).toUTF8()
				<< " virtual=" << window.virtualSize << " framebuffer=" << window.frameBufferSize
				<< " scene=" << Scene::Size() << " render=" << Scene::RenderResolution() << '\n';
		};
		Window::Maximize();
		CHECK(Window::GetState().maximized);
		check("maximize immediate");
		CHECK(Scene::RenderResolution() != initialRender);
		System::Sleep(500);
		REQUIRE(System::Update());
		check("maximize next frame");
		Window::Restore();
		CHECK_FALSE(Window::GetState().maximized);
		check("restore immediate");
		CHECK(Window::GetState().virtualSize == windowed.virtualSize);
		CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
		System::Sleep(500);
		REQUIRE(System::Update());
		check("restore next frame");
		CHECK(Scene::RenderResolution() == initialRender);

		const Size sceneBeforeMinimize = Scene::Size();
		Window::Minimize();
		System::Sleep(500);
		CHECK(Window::GetState().minimized);
		REQUIRE(System::Update());
		CHECK(Scene::GetConfig() == config);
		CHECK(Scene::Size() == sceneBeforeMinimize);
		CHECK(Scene::RenderResolution() == initialRender);
		Window::Restore();
		CHECK_FALSE(Window::GetState().minimized);
		check("unminimize immediate");
		System::Sleep(500);
		REQUIRE(System::Update());
		check("unminimize next frame");
	}
}

# endif
