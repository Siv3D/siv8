//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# include "WindowFullscreenTestHelper_Windows.hpp"

namespace s3d
{
	using namespace Test::Fullscreen;

	TEST_CASE("WindowFullscreen.api_and_shortcut_preserve_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
		{
			for (const bool enabled : { false, true })
			{
				INFO(static_cast<int32>(mode));
				INFO(enabled);
				{
					auto config = Scene::GetConfig();
					config.sizeMode = mode;
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					config.fixedSceneSize = Size{ 400, 300 };
					config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
					config.scalingFilter = TextureFilter::Nearest;
					Scene::Configure(config);
				}

				AltEnter();
				REQUIRE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				// Reapplying the current window mode must leave the scene settings intact.
				Test::SetFullscreenWithPause(true);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				Test::SetFullscreenWithPause(false);
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				Test::UpdateWithPause();
				CheckPresentation(mode, enabled, TextureFilter::Nearest);

				// Also enter via the API and leave via the shortcut.
				Test::SetFullscreenWithPause(true);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				AltEnter();
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);

				// Cover both same-method round trips for every presentation policy, too.
				for (const bool shortcut : { false, true })
				{
					INFO(shortcut);
					if (shortcut) { AltEnter(); }
					else { Test::SetFullscreenWithPause(true); }
					REQUIRE(Window::GetState().fullscreen);
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					if (shortcut) { AltEnter(); }
					else { Test::SetFullscreenWithPause(false); }
					REQUIRE_FALSE(Window::GetState().fullscreen);
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.changed_policy_is_not_restored_on_exit", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		for (const bool exitByShortcut : { false, true })
		{
			INFO(exitByShortcut);
			{
				auto config = Scene::GetConfig();
				config.sizeMode = SceneSizeMode::Window;
				config.fullscreen = FullscreenSceneMode::FollowWindow;
				config.resolution = SceneResolution::SceneSize;
				config.scalingFilter = TextureFilter::Linear;
				Scene::Configure(config);
			}
			AltEnter();
			REQUIRE(Window::GetState().fullscreen);

			{
				auto config = Scene::GetConfig();
				config.sizeMode = SceneSizeMode::Fixed;
				config.fullscreen = FullscreenSceneMode::FollowWindow;
				config.fixedSceneSize = Size{ 320, 240 };
				config.resolution = SceneResolution::Display;
				config.scalingFilter = TextureFilter::Nearest;
				Scene::Configure(config);
			}
			if (exitByShortcut) { AltEnter(); }
			else { Test::SetFullscreenWithPause(false); }
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
			Test::UpdateWithPause();
			CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
		}

		// A disabled shortcut does not change the window, or leave a deferred toggle behind.
		Window::SetToggleFullscreenEnabled(false);
		AltEnter();
		CHECK_FALSE(Window::GetState().fullscreen);
		Test::SetFullscreenWithPause(true);
		REQUIRE(Window::GetState().fullscreen);
		CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
		Test::SetFullscreenWithPause(false);
		REQUIRE_FALSE(Window::GetState().fullscreen);
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });

		// Re-enabling before the next frame must not revive an ignored message.
		Window::SetToggleFullscreenEnabled(false);
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		Test::SetFullscreenWithPause(false);

		// Disabling also cancels an accepted request that has not executed yet.
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Window::SetToggleFullscreenEnabled(false);
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		Test::SetFullscreenWithPause(false);

		// Reapplying enabled without disabling must preserve a valid request.
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		CHECK(Window::GetState().fullscreen);
		Test::SetFullscreenWithPause(false);
	}

	TEST_CASE("WindowFullscreen.resize_restores_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Test::UpdateWithPause();
		const auto monitors = System::EnumerateMonitors();
		const auto routes = MixedDPIRoutes(monitors);
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		for (const auto& [source, target] : routes)
		{
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			REQUIRE(Window::CenterToScreen(source));
			Test::UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool enabled : { false, true })
				{
					// Resize delegates to ResizeVirtual: alternate those aliases instead of
					// repeating their entire matrix. Exercise actual sizes for every policy.
					for (const int32 api : { (enabled ? 1 : 0), 2 })
					{
						CAPTURE(source, target, mode, enabled, ResizeNames[api]);
						Catch::cout() << "route=" << source << "->" << target << " api=" << ResizeNames[api] << '\n';
						REQUIRE(Window::Resize(600, 400, CenterWindow::No));
						const auto windowed = Window::GetState();
						RECT nativeWindowed{};
						REQUIRE(::GetWindowRect(MainWindow(), &nativeWindowed));
						{
							auto config = Scene::GetConfig();
							config.sizeMode = (mode == SceneSizeMode::Window) ? SceneSizeMode::WindowPixels : SceneSizeMode::Window;
							config.fullscreen = FullscreenSceneMode::FollowWindow;
							config.resolution = (not enabled ? SceneResolution::Display : SceneResolution::SceneSize);
							config.scalingFilter = TextureFilter::Linear;
							Scene::Configure(config);
						}
						ReportWindow("windowed");
						Test::SetFullscreenWithPause(true, target);
						REQUIRE(Window::GetState().fullscreen);
						// All three settings changed in fullscreen must survive the resize exit.
						{
							auto config = Scene::GetConfig();
							config.sizeMode = mode;
							config.fullscreen = FullscreenSceneMode::FollowWindow;
							config.fixedSceneSize = Size{ 400, 300 };
							config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
							config.scalingFilter = TextureFilter::Nearest;
							Scene::Configure(config);
						}
						ReportWindow("fullscreen");
						const auto full = Window::GetState();
						CHECK(System::GetCurrentMonitorIndex() == target);
						CHECK(full.frameBufferSize == monitors[target].displayRect.size);
						CheckPresentation(mode, enabled, TextureFilter::Nearest);
						for (int32 sameAPI = 0; sameAPI < 3; ++sameAPI)
						{
							CAPTURE(ResizeNames[sameAPI]);
							CHECK(ResizeFunctions[sameAPI]((sameAPI == 2) ? full.frameBufferSize : full.virtualSize, CenterWindow::No));
							CHECK(Window::GetState().fullscreen);
							CHECK(Window::GetState().bounds == full.bounds);
							CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
							CheckPresentation(mode, enabled, TextureFilter::Nearest);
						}
						for (const Size invalid : { Size{ 0, 600 }, Size{ 800, -1 }, Size{ 8193, 600 }, Size{ 800, 8193 } })
						{
							CHECK_THROWS_AS(ResizeFunctions[api](invalid, CenterWindow::No), Error);
						}
						Test::UpdateWithPause();
						CHECK(Window::GetState().fullscreen);
						CHECK(Window::GetState().virtualSize == full.virtualSize);
						CHECK(Window::GetState().frameBufferSize == full.frameBufferSize);
						CHECK(Window::GetState().bounds == full.bounds);
						CheckPresentation(mode, enabled, TextureFilter::Nearest);

						const Size requested{ 800, 600 };
						const bool resized = ResizeFunctions[api](requested, CenterWindow::No);
						const auto check = [&](const char* phase)
						{
							INFO(phase);
							ReportWindow(phase);
							const auto& window = Window::GetState();
							CHECK_FALSE(window.fullscreen);
							CHECK(System::GetCurrentMonitorIndex() == source);
							CHECK(window.scaling == windowed.scaling);
							CHECK(::GetDpiForWindow(MainWindow()) == Math::Round(windowed.scaling * 96));
							CHECK(window.bounds.pos == windowed.bounds.pos);
							RECT bounds{}, client{};
							REQUIRE(::GetWindowRect(MainWindow(), &bounds));
							REQUIRE(::GetClientRect(MainWindow(), &client));
							CHECK(bounds.left == nativeWindowed.left);
							CHECK(bounds.top == nativeWindowed.top);
							const Size pixels = (api == 2) ? requested : Math::Round(requested * windowed.scaling).asPoint();
							const Size points = (api == 2) ? Math::Round(requested / windowed.scaling).asPoint() : requested;
							CHECK(window.virtualSize == points);
							CHECK(window.frameBufferSize == pixels);
							CHECK(Size{ client.right, client.bottom } == pixels);
							CheckPresentation(mode, enabled, TextureFilter::Nearest);
							CHECK(Scene::ClientToScene(pixels * 0.5).epsilonEquals(Scene::Size() * 0.5, 1e-6));
							const auto all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
							CHECK(Cursor::PosF().epsilonEquals(all.inverse().transformPoint(Cursor::PosRaw()), 1e-3));
						};
						check("resize immediate");
						Test::PauseForWindowTransition();
						REQUIRE(resized);
						Test::UpdateWithPause();
						check("resize next frame");
					}
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.keep_size_resize_restores_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Test::UpdateWithPause();
		const auto routes = MixedDPIRoutes(System::EnumerateMonitors());
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		for (const auto& [source, target] : routes)
		{
			REQUIRE(Window::Resize(600, 400, CenterWindow::No));
			REQUIRE(Window::CenterToScreen(source));
			Test::UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (int32 api = 0; api < 3; ++api)
			{
				CAPTURE(source, target, ResizeNames[api]);
				Catch::cout() << "KeepSize route=" << source << "->" << target << " api=" << ResizeNames[api] << '\n';
				REQUIRE(Window::Resize(600, 400, CenterWindow::No));
				Scene::Configure(SceneConfig{});
				const auto windowed = Window::GetState();
				RECT nativeWindowed{};
				REQUIRE(::GetWindowRect(MainWindow(), &nativeWindowed));
				Test::SetFullscreenWithPause(true, target);
				REQUIRE(Window::GetState().fullscreen);
				CHECK(System::GetCurrentMonitorIndex() == target);
				CheckSelectedConfig(SceneConfig{}, windowed.virtualSize);
				Test::UpdateWithPause();
				CheckSelectedConfig(SceneConfig{}, windowed.virtualSize);
				auto config = Scene::GetConfig();
				Size heldSize = Scene::Size();
				if (api != 0)
				{
					config.resolution = SceneResolution::Display;
					config.scalingFilter = TextureFilter::Nearest;
					Scene::Configure(config);
					CheckSelectedConfig(config, heldSize);
				}
				if (api == 2)
				{
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					Scene::Configure(config);
					heldSize = Window::GetState().virtualSize;
					CheckSelectedConfig(config, heldSize);
					config.fullscreen = FullscreenSceneMode::KeepSize;
					Scene::Configure(config);
					CheckSelectedConfig(config, heldSize);
				}
				Scene::Configure(Scene::GetConfig());
				CheckSelectedConfig(config, heldSize);
				const auto full = Window::GetState();
				CHECK(ResizeFunctions[api]((api == 2) ? full.frameBufferSize : full.virtualSize, CenterWindow::No));
				CHECK_THROWS_AS(ResizeFunctions[api]({ 0, 600 }, CenterWindow::No), Error);
				CHECK(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds == full.bounds);
				CheckSelectedConfig(config, heldSize);
				Test::UpdateWithPause();
				CHECK(Window::GetState().fullscreen);
				CheckSelectedConfig(config, heldSize);
				ReportWindow("KeepSize fullscreen");

				const Size requested{ 800, 600 };
				const bool resized = ResizeFunctions[api](requested, CenterWindow::No);
				const auto check = [&](const char* phase)
				{
					INFO(phase);
					ReportWindow(phase);
					const auto& window = Window::GetState();
					CHECK_FALSE(window.fullscreen);
					CHECK(System::GetCurrentMonitorIndex() == source);
					CHECK(window.scaling == windowed.scaling);
					CHECK(::GetDpiForWindow(MainWindow()) == Math::Round(windowed.scaling * 96));
					CHECK(window.bounds.pos == windowed.bounds.pos);
					RECT bounds{}, client{};
					REQUIRE(::GetWindowRect(MainWindow(), &bounds));
					REQUIRE(::GetClientRect(MainWindow(), &client));
					CHECK(bounds.left == nativeWindowed.left);
					CHECK(bounds.top == nativeWindowed.top);
					const Size pixels = (api == 2) ? requested : Math::Round(requested * windowed.scaling).asPoint();
					const Size points = (api == 2) ? Math::Round(requested / windowed.scaling).asPoint() : requested;
					CHECK(window.virtualSize == points);
					CHECK(window.frameBufferSize == pixels);
					CHECK(Size{ client.right, client.bottom } == pixels);
					CheckSelectedConfig(config, points);
				};
				check("KeepSize resize immediate");
				Test::PauseForWindowTransition();
				REQUIRE(resized);
				Test::UpdateWithPause();
				check("KeepSize resize next frame");
			}
		}
	}

	TEST_CASE("WindowFullscreen.below_minimum_resize_restores_window", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreMinimum{ [&] { Window::SetMinimumFrameBufferSize(restore.window.minFrameBufferSize); } };
		Test::UpdateWithPause();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		Window::SetMinimumFrameBufferSize({ 200, 200 });
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::Window;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.resolution = SceneResolution::Display;
			config.scalingFilter = TextureFilter::Linear;
			Scene::Configure(config);
		}
		const auto windowed = Window::GetState();
		for (int32 api = 0; api < 3; ++api)
		{
			CAPTURE(ResizeNames[api]);
			auto config = Scene::GetConfig();
			config.fullscreen = (api == 0) ? FullscreenSceneMode::FollowWindow : FullscreenSceneMode::KeepSize;
			Scene::Configure(config);
			Test::SetFullscreenWithPause(true);
			REQUIRE(Window::GetState().fullscreen);
			// In-range requests below the minimum return false after restoring the window.
			CHECK_FALSE(ResizeFunctions[api]({ 1, 1 }, CenterWindow::No));
			const auto check = [&]
			{
				CHECK_FALSE(Window::GetState().fullscreen);
				CHECK(Window::GetState().bounds == windowed.bounds);
				CHECK(Window::GetState().virtualSize == windowed.virtualSize);
				CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				CHECK(Scene::GetConfig() == config);
				CheckPresentation(SceneSizeMode::Window, true, TextureFilter::Linear);
			};
			check();
			Test::PauseForWindowTransition();
			Test::UpdateWithPause();
			check();
		}
	}

	TEST_CASE("WindowFullscreen.bounds_match_selected_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Test::UpdateWithPause();
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::WindowPixels;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.resolution = SceneResolution::SceneSize;
			config.scalingFilter = TextureFilter::Linear;
			Scene::Configure(config);
		}
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		auto& window = *static_cast<CWindow*>(SIV3D_ENGINE(Window));
		WindowsMonitor::Functions functions;
		functions.enumerateMonitors = [] { return Array<MonitorInfo>{}; };
		const size_t invalid = std::numeric_limits<size_t>::max();
		window.setFullscreen(true, invalid, functions);
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const size_t firstMonitor = System::GetCurrentMonitorIndex();
		REQUIRE(firstMonitor < monitors.size());
		for (size_t step = 0; step < monitors.size(); ++step)
		{
			const size_t i = ((firstMonitor + step) % monitors.size());
			RECT windowedBounds{};
			REQUIRE(::GetWindowRect(MainWindow(), &windowedBounds));
			const auto windowed = Window::GetState();
			const Rect expected = monitors[i].displayRect;
			INFO(i);
			INFO(expected);
			if (step == 0)
			{
				// Recover from failure, falling back by identity in a differently ordered list.
				functions.enumerateMonitors = []
				{
					auto snapshot = System::EnumerateMonitors();
					if (not snapshot.isEmpty()) { std::rotate(snapshot.begin(), snapshot.begin() + 1, snapshot.end()); }
					return snapshot;
				};
				window.setFullscreen(true, invalid, functions);
				Test::PauseForWindowTransition();
			}
			else { Test::SetFullscreenWithPause(true, i); }
			Test::UpdateWithPause();
			REQUIRE(Window::GetState().fullscreen);
			RECT bounds{};
			REQUIRE(::GetWindowRect(MainWindow(), &bounds));
			CHECK(Rect{ bounds.left, bounds.top, (bounds.right - bounds.left), (bounds.bottom - bounds.top) } == expected);
			CHECK(Window::GetState().frameBufferSize == expected.size);
			CheckPresentation(SceneSizeMode::WindowPixels, false, TextureFilter::Linear);
			Test::SetFullscreenWithPause(false);
			Test::UpdateWithPause();
			REQUIRE(::GetWindowRect(MainWindow(), &bounds));
			CHECK(bounds.left == windowedBounds.left);
			CHECK(bounds.top == windowedBounds.top);
			CHECK(bounds.right == windowedBounds.right);
			CHECK(bounds.bottom == windowedBounds.bottom);
			CHECK(Window::GetState().virtualSize == windowed.virtualSize);
			CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
		}
	}
}

# endif
