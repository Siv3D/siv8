//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS) || SIV3D_PLATFORM(MACOS)

# if SIV3D_PLATFORM(WINDOWS)
# include "WindowFullscreenTestHelper_Windows.hpp"
# else
# include "WindowFullscreenTestHelper_macOS.hpp"
# endif

namespace s3d
{
	using namespace Test::Fullscreen;

	TEST_CASE("WindowFullscreen.change_monitor_preserves_window_and_config", "[.fullscreen]")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const auto originalSamples = Scene::GetMSAASampleCount();
		const ScopeExit restoreSamples{ [=] { Scene::SetMSAASampleCount(originalSamples); } };
		auto expectedSamples = originalSamples;
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		if (monitors.size() < 2)
		{
			SKIP("Fullscreen monitor changes require two attached displays.");
		}
		size_t first = 0, second = 1;
		// Prefer a mixed-DPI pair; cover both directions without a monitor Cartesian product.
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			Catch::cout() << "transfer monitor[" << i << "] " << monitors[i].format().toUTF8() << '\n';
			for (size_t j = i + 1; j < monitors.size(); ++j)
			{
				if (monitors[i].scaling && monitors[j].scaling && (monitors[i].scaling != monitors[j].scaling))
				{
					first = i;
					second = j;
					break;
				}
			}
		}
		Catch::cout() << "transfer pair=" << first << " <-> " << second << '\n';
# if SIV3D_PLATFORM(MACOS)
		const DesktopSnapshot desktop;
# endif
		Window::SetToggleFullscreenEnabled(true);
		Window::Restore();
		const auto checkTarget = [&](const size_t target)
		{
			const auto& state = Window::GetState();
			REQUIRE(state.fullscreen);
			CHECK_FALSE(state.minimized);
			CHECK(System::GetCurrentMonitorIndex() == target);
			CHECK(state.bounds == monitors[target].displayRect);
			CHECK(state.scaling == monitors[target].scaling.value_or(1.0));
# if SIV3D_PLATFORM(WINDOWS)
			RECT native{};
			REQUIRE(::GetWindowRect(MainWindow(), &native));
			CHECK(Rect{ native.left, native.top, native.right - native.left, native.bottom - native.top } == monitors[target].displayRect);
			RECT client{};
			REQUIRE(::GetClientRect(MainWindow(), &client));
			CHECK(Size{ client.right - client.left, client.bottom - client.top } == state.frameBufferSize);
			CHECK(state.frameBufferSize == monitors[target].displayRect.size);
			CHECK(state.scaling == (::GetDpiForWindow(MainWindow()) / 96.0));
			CHECK(state.virtualSize == Math::Round(state.frameBufferSize / state.scaling).asPoint());
# else
			CHECK(state.virtualSize == monitors[target].displayRect.size);
			CHECK(state.frameBufferSize == monitors[target].fullscreenResolution);
			desktop.check();
# endif
		};
		auto following = SceneConfig::Window(SceneResolution::Display);
		following.fullscreen = FullscreenSceneMode::FollowWindow;
		size_t step = 0;
		for (auto config : { SceneConfig::Fixed({ 320, 240 }), SceneConfig::Fixed({ 320, 240 }, SceneResolution::Display),
			SceneConfig::PixelArt({ 200, 150 }), SceneConfig::Window(), SceneConfig::Window(SceneResolution::Display),
			SceneConfig::WindowPixels(), following })
		{
			const size_t source = ((step % 2) ? second : first), target = ((step % 2) ? first : second);
			size_t entry = source;
			if ((step == 0) && (2 < monitors.size()))
			{
				// Reuse this round trip for window A -> fullscreen B -> fullscreen C -> A.
				for (size_t i = 0; i < monitors.size(); ++i)
				{
					if ((i != source) && (i != target)) { entry = i; break; }
				}
			}
			CAPTURE(step, source, target, config.sizeMode, config.resolution, config.fullscreen);
			Catch::cout() << "transfer step=" << step << " window=" << source
				<< " fullscreen=" << entry << " -> " << target << '\n';
			REQUIRE(Window::CenterToScreen(source));
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			const auto windowed = Window::GetState();
			Scene::Configure(config);
			Size expected = Scene::Size();
			const auto check = [&]
			{
				if ((config.sizeMode != SceneSizeMode::Fixed) &&
					(not Window::GetState().fullscreen || config.fullscreen == FullscreenSceneMode::FollowWindow))
				{
					expected = ((config.sizeMode == SceneSizeMode::Window) ? Window::GetState().virtualSize : Window::GetState().frameBufferSize);
				}
				CheckSelectedConfig(config, expected);
				CHECK(Scene::GetMSAASampleCount() == expectedSamples);
			};
			Test::SetFullscreenWithPause(true, entry);
			checkTarget(entry);
			check();
			REQUIRE(System::Update());
			check();
			Test::SetFullscreenWithPause(true, target);
			checkTarget(target);
			check();
			REQUIRE(System::Update());
			checkTarget(target);
			check();
			const auto full = Window::GetState();
# if SIV3D_PLATFORM(WINDOWS)
			const auto style = ::GetWindowLongPtrW(MainWindow(), GWL_STYLE);
			const auto extendedStyle = ::GetWindowLongPtrW(MainWindow(), GWL_EXSTYLE);
# endif
			const auto checkUnmoved = [&]
			{
				CHECK(Window::GetState().bounds == full.bounds);
				CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
				CHECK(Window::GetState().virtualSize == full.virtualSize);
				CHECK(Window::GetState().scaling == full.scaling);
				CHECK(Window::GetState().fullscreen);
# if SIV3D_PLATFORM(WINDOWS)
				CHECK(::GetWindowLongPtrW(MainWindow(), GWL_STYLE) == style);
				CHECK(::GetWindowLongPtrW(MainWindow(), GWL_EXSTYLE) == extendedStyle);
# endif
				check();
			};
			Window::SetFullscreen(true, target);
			checkUnmoved();
			Window::SetFullscreen(true);
			checkUnmoved();
			Window::SetFullscreen(true, std::numeric_limits<size_t>::max());
			checkUnmoved();
# if SIV3D_PLATFORM(WINDOWS)
			if (step == 0)
			{
				auto& window = *static_cast<CWindow*>(SIV3D_ENGINE(Window));
				WindowsMonitor::Functions functions;
				functions.enumerateMonitors = [] { return Array<MonitorInfo>{}; };
				window.setFullscreen(true, source, functions);
				checkUnmoved();
				functions.enumerateMonitors = []
				{
					auto result = System::EnumerateMonitors();
					for (auto& monitor : result) { monitor.displayDeviceName = U"not-an-attached-display"; }
					return result;
				};
				window.setFullscreen(true, std::numeric_limits<size_t>::max(), functions);
				checkUnmoved();
			}
# endif
			if (step == 6)
			{
				// KeepSize selected in fullscreen preserves the current scene through another move.
				config.fullscreen = FullscreenSceneMode::KeepSize;
				config.scalingFilter = TextureFilter::Nearest;
				Scene::Configure(config);
				expectedSamples = MSAASampleCount::NoMSAA;
				Scene::SetMSAASampleCount(expectedSamples);
				Test::SetFullscreenWithPause(true, source);
				checkTarget(source);
				check();
				REQUIRE(System::Update());
				check();
			}
			if (step == 1)
			{
# if SIV3D_PLATFORM(WINDOWS)
				AltEnter();
# else
				RequestFullscreen(FullscreenRoute::Shortcut);
# endif
			}
			else if ((2 <= step) && (step <= 4))
			{
				const Size requested{ 620, 420 };
				if (step == 2) { REQUIRE(Window::Resize(requested, CenterWindow::No)); }
				else if (step == 3) { REQUIRE(Window::ResizeVirtual(requested, CenterWindow::No)); }
				else { REQUIRE(Window::ResizeActual(requested, CenterWindow::No)); }
				Test::PauseForWindowTransition();
				if (step != 4) { CHECK(Window::GetState().virtualSize == requested); }
# if SIV3D_PLATFORM(WINDOWS)
				else { CHECK(Window::GetState().frameBufferSize == requested); }
# else
				else { CHECK(Window::GetState().virtualSize == Math::Round(requested / windowed.scaling).asPoint()); }
# endif
			}
			else { Test::SetFullscreenWithPause(false, target); }
			const auto checkRestored = [&]
			{
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds.pos == windowed.bounds.pos);
				CHECK(System::GetCurrentMonitorIndex() == source);
				CHECK(Window::GetState().scaling == windowed.scaling);
				if ((step < 2) || (4 < step))
				{
					CHECK(Window::GetState().bounds == windowed.bounds);
					CHECK(Window::GetState().virtualSize == windowed.virtualSize);
					CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				}
				else
				{
					const Size requested{ 620, 420 };
					const Size points = ((step == 4) ? Math::Round(requested / windowed.scaling).asPoint() : requested);
					CHECK(Window::GetState().virtualSize == points);
# if SIV3D_PLATFORM(WINDOWS)
					const Size pixels = ((step == 4) ? requested : Math::Round(points * windowed.scaling).asPoint());
# else
					const Size pixels = Math::Round(points * windowed.scaling).asPoint();
# endif
					CHECK(Window::GetState().frameBufferSize == pixels);
				}
				check();
			};
			checkRestored();
			REQUIRE(System::Update());
			checkRestored();
			++step;
		}
	}

	TEST_CASE("WindowFullscreen.scene_config_default_and_reconfigure", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		const auto monitors = System::EnumerateMonitors();
		REQUIRE(not monitors.isEmpty());
		for (const auto& monitor : monitors)
		{
			Catch::cout() << "SceneConfig monitor: " << monitor.name.toUTF8()
				<< " desktop=" << monitor.displayRect << " framebuffer=" << monitor.fullscreenResolution
				<< " scale=" << monitor.scaling.value_or(1.0) << '\n';
		}
		const auto shortcut = []
		{
# if SIV3D_PLATFORM(WINDOWS)
			AltEnter();
# else
			RequestFullscreen(FullscreenRoute::Shortcut);
# endif
		};
		const auto check = [](const SceneConfig& config, const Size expected)
		{
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == expected);
			const auto& window = Window::GetState();
			SceneMisc::RenderState reference;
			reference.config = config;
			reference.sceneSize = expected;
			reference.windowFrameBufferSize = window.frameBufferSize;
			CHECK(Scene::RenderResolution() == reference.renderResolution());
			const auto [scale, area] = reference.composition();
			CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
			CHECK(Scene::ClientToScene(window.frameBufferSize * 0.5).epsilonEquals(expected * 0.5, 1e-6));
		};
		// Two mixed-route round trips cover both entry paths without multiplying all presets.
		for (const bool enterByShortcut : { false, true })
		{
			Scene::Configure(SceneConfig{});
			const Size windowScene = Scene::Size();
			if (enterByShortcut) { shortcut(); }
			else { Test::SetFullscreenWithPause(true, (System::GetCurrentMonitorIndex() + 1) % monitors.size()); }
			REQUIRE(Window::GetState().fullscreen);
			check(SceneConfig{}, windowScene);
			Test::UpdateWithPause();
			check(SceneConfig{}, windowScene);
			auto config = SceneConfig::Window(SceneResolution::Display);
			config.scalingFilter = TextureFilter::Nearest;
			Scene::Configure(config);
			check(config, windowScene);
			Scene::Configure(Scene::GetConfig());
			check(config, windowScene);
			const auto full = Window::GetState();
			const auto checkUnmoved = [&]
			{
				CHECK(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds == full.bounds);
				CHECK(Window::GetState().virtualSize == full.virtualSize);
				CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
				CHECK(Scene::GetConfig() == config);
			};
			CHECK_FALSE(Window::CenterToScreen());
			checkUnmoved();
			CHECK_FALSE(Window::CenterToScreen((System::GetCurrentMonitorIndex() + 1) % monitors.size()));
			checkUnmoved();
			CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
			checkUnmoved();
			// Default CenterWindow::Yes must not turn a same-size resize into an exit.
			CHECK(Window::Resize(full.virtualSize));
			CHECK(Window::ResizeVirtual(full.virtualSize));
			CHECK(Window::ResizeActual(full.frameBufferSize));
			checkUnmoved();
			check(config, windowScene);
			if (enterByShortcut) { Test::SetFullscreenWithPause(false); }
			else { shortcut(); }
			REQUIRE_FALSE(Window::GetState().fullscreen);
			check(config, Window::GetState().virtualSize);
			Test::UpdateWithPause();
			check(config, Window::GetState().virtualSize);
			REQUIRE(Window::CenterToScreen());
			REQUIRE(Window::CenterToScreen(System::GetCurrentMonitorIndex()));
			CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
			CHECK_FALSE(Window::GetState().fullscreen);
			CHECK(Scene::GetConfig() == config);
		}
	}
}

# endif
