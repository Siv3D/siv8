//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include "WindowFullscreenTestHelper_macOS.hpp"

namespace s3d
{
	using namespace Test::Fullscreen;

	TEST_CASE("WindowFullscreen.macOS.ui_and_api_preserve_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { Test::SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		Window::SetToggleFullscreenEnabled(true);
		Test::UpdateWithPause();
		const DesktopSnapshot desktop;
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		NSMenuItem* menu = FullscreenMenuItem(NSApp.mainMenu);
		REQUIRE(menu != nil);
		CHECK(std::string{ menu.keyEquivalent.UTF8String } == "f");
		CHECK(menu.keyEquivalentModifierMask == (NSEventModifierFlagControl | NSEventModifierFlagCommand));
		for (size_t monitor = 0; monitor < desktop.monitors.size(); ++monitor)
		{
			REQUIRE(Window::CenterToScreen(monitor));
			Test::UpdateWithPause();
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool highResolution : { false, true })
				{
					CAPTURE(monitor, mode, highResolution);
					Window::SetStyle(highResolution ? WindowStyle::Sizable : WindowStyle::Fixed);
					{
						auto config = Scene::GetConfig();
						config.sizeMode = mode;
						config.fullscreen = FullscreenSceneMode::FollowWindow;
						config.fixedSceneSize = Size{ 400, 300 };
						config.resolution = (highResolution ? SceneResolution::Display : SceneResolution::SceneSize);
						config.scalingFilter = TextureFilter::Nearest;
						Scene::Configure(config);
					}
					const auto windowed = Window::GetState();
					for (const auto route : { FullscreenRoute::Shortcut, FullscreenRoute::Menu,
						FullscreenRoute::API })
					{
						CAPTURE(route);
						RequestFullscreen(route);
						REQUIRE(Window::GetState().fullscreen);
						CHECK(Window::GetState().bounds == desktop.monitors[monitor].displayRect);
						CHECK((native.styleMask & NSWindowStyleMaskFullScreen) == 0);
						CHECK([native validateMenuItem:menu]);
						CHECK([menu.title isEqualToString:@"Exit Full Screen"]);
						CheckPresentation(mode, highResolution, TextureFilter::Nearest);
						desktop.check();
						const auto exit = (route == FullscreenRoute::Shortcut) ? FullscreenRoute::API
							: ((route == FullscreenRoute::Menu) ? FullscreenRoute::Menu : FullscreenRoute::Shortcut);
						RequestFullscreen(exit);
						REQUIRE_FALSE(Window::GetState().fullscreen);
						CHECK(Window::GetState().bounds == windowed.bounds);
						CHECK(Window::GetState().virtualSize == windowed.virtualSize);
						CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
						CHECK([native validateMenuItem:menu]);
						CHECK([menu.title isEqualToString:@"Enter Full Screen"]);
						CheckPresentation(mode, highResolution, TextureFilter::Nearest);
						desktop.check();
					}
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.disabled_pending_repeat_and_styles", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { Test::SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		NSMenuItem* menu = FullscreenMenuItem(NSApp.mainMenu);
		REQUIRE(menu != nil);
		const DesktopSnapshot desktop;
		for (const WindowStyle style : { WindowStyle::Fixed, WindowStyle::Sizable, WindowStyle::Frameless })
		{
			CAPTURE(style);
			Window::SetStyle(style);
			Window::SetToggleFullscreenEnabled(false);
			CHECK_FALSE(Window::IsToggleFullscreenEnabled());
			CHECK_FALSE([native validateMenuItem:menu]);
			CHECK((native.collectionBehavior & NSWindowCollectionBehaviorFullScreenPrimary) == 0);
			NSButton* green = [native standardWindowButton:NSWindowZoomButton];
			if (style != WindowStyle::Frameless)
			{
				REQUIRE(green != nil);
				CHECK(green.enabled == (style == WindowStyle::Sizable));
				CHECK_FALSE(green.hidden);
				CHECK(green.action != NSSelectorFromString(@"toggleFullscreen_Siv3D:"));
			}
			[native toggleFullScreen:menu];
			FullscreenShortcut();
			Window::SetToggleFullscreenEnabled(true);
			Test::UpdateWithPause();
			CHECK_FALSE(Window::GetState().fullscreen);
			FullscreenShortcut();
			Window::SetToggleFullscreenEnabled(false);
			Window::SetToggleFullscreenEnabled(true);
			Test::UpdateWithPause();
			CHECK_FALSE(Window::GetState().fullscreen);

			FullscreenShortcut();
			FullscreenShortcut(true);
			[native toggleFullScreen:menu]; // Two requests before a frame coalesce.
			Window::SetToggleFullscreenEnabled(true);
			Test::UpdateWithPause();
			REQUIRE(Window::GetState().fullscreen);
			FullscreenShortcut(true);
			Test::UpdateWithPause();
			CHECK(Window::GetState().fullscreen);
			// Settings changed while fullscreen survive an exit through the shortcut.
			{
				auto config = Scene::GetConfig();
				config.sizeMode = SceneSizeMode::Fixed;
				config.fullscreen = FullscreenSceneMode::FollowWindow;
				config.fixedSceneSize = Size{ 320, 240 };
				config.resolution = SceneResolution::Display;
				config.scalingFilter = TextureFilter::Nearest;
				Scene::Configure(config);
			}
			RequestFullscreen(FullscreenRoute::Shortcut);
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
			Window::SetToggleFullscreenEnabled(false);
			Test::SetFullscreenWithPause(true);
			REQUIRE(Window::GetState().fullscreen);
			FullscreenShortcut();
			Test::UpdateWithPause();
			CHECK(Window::GetState().fullscreen);
			Test::SetFullscreenWithPause(false);
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CHECK(Window::GetStyle() == style);
			desktop.check();
		}

		// Exercise NSApplication's event queue and main-menu key-equivalent path,
		// including currentEvent-based repeat suppression in toggleFullScreen:.
		Window::SetStyle(WindowStyle::Sizable);
		Window::SetToggleFullscreenEnabled(true);
		[NSApp postEvent:FullscreenKeyEvent(false) atStart:YES];
		Test::UpdateWithPause();
		REQUIRE(Window::GetState().fullscreen);
		[NSApp postEvent:FullscreenKeyEvent(true) atStart:YES];
		Test::UpdateWithPause();
		CHECK(Window::GetState().fullscreen);
		[NSApp postEvent:FullscreenKeyEvent(false) atStart:YES];
		Test::UpdateWithPause();
		REQUIRE_FALSE(Window::GetState().fullscreen);

		// Fullscreen enablement must not affect native maximization controls.
		for (const bool enabled : { false, true })
		{
			Window::SetToggleFullscreenEnabled(enabled);
			NSButton* green = [native standardWindowButton:NSWindowZoomButton];
			REQUIRE(green != nil);
			REQUIRE(green.enabled);
			CHECK_FALSE(green.hidden);
			[green performClick:nil];
			Test::UpdateWithPause();
			CHECK_FALSE(Window::GetState().fullscreen);
			CHECK(Window::GetState().maximized);
			Window::Restore();
			Test::UpdateWithPause();
			CHECK_FALSE(Window::GetState().maximized);
		}
		NSMenuItem* zoom = [[NSMenuItem alloc] initWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];
		CHECK([native validateMenuItem:zoom]);
		[native performZoom:zoom];
		Test::UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		CHECK(Window::GetState().maximized);
		Window::Restore();
		Test::UpdateWithPause();
		CHECK_FALSE(Window::GetState().maximized);
		desktop.check();
	}

	TEST_CASE("WindowFullscreen.macOS.api_preserves_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Test::UpdateWithPause();
		const DesktopSnapshot desktop;
		const Point previousCursor = Cursor::ScreenPos();
		const ScopeExit restoreCursor{ [=] { ::CGWarpMouseCursorPosition(CGPointMake(previousCursor.x, previousCursor.y)); } };
		for (size_t monitor = 0; monitor < desktop.monitors.size(); ++monitor)
		{
			CAPTURE(monitor);
			REQUIRE(Window::CenterToScreen(monitor));
			Test::UpdateWithPause();
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool enabled : { false, true })
				{
					CAPTURE(mode, enabled);
					{
						auto config = Scene::GetConfig();
						config.sizeMode = mode;
						config.fullscreen = FullscreenSceneMode::FollowWindow;
						config.fixedSceneSize = Size{ 400, 300 };
						config.resolution = (enabled ? SceneResolution::Display : SceneResolution::SceneSize);
						config.scalingFilter = TextureFilter::Nearest;
						Scene::Configure(config);
					}
					Test::SetFullscreenWithPause(true);
					REQUIRE(Window::GetState().fullscreen);
					desktop.check();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					Test::UpdateWithPause();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					{
						const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
						const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
						const Point logical{ 100, 80 };
						const Mat3x2 all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
						Cursor::SetPos(logical);
						Circle{ logical, 12 }.draw(Palette::Red);
						ScreenCapture::RequestCurrentFrame();
						Test::UpdateWithPause();
						CHECK(Cursor::PosF().epsilonEquals(logical, 1.0));
						CHECK(Vec2{ Cursor::PosRaw() }.epsilonEquals(all.transformPoint(logical), 1.0));
						CHECK(Circle{ logical, 3 }.mouseOver());
						CHECK_FALSE(Circle{ logical + Point{ 30, 0 }, 3 }.mouseOver());
						REQUIRE(ScreenCapture::HasNewFrame());
						CHECK(ScreenCapture::GetFrame().size() == Scene::RenderResolution());
					}
					// Reapplying fullscreen must not change the selected policy.
					Test::SetFullscreenWithPause(true);
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					Test::SetFullscreenWithPause(false);
					REQUIRE_FALSE(Window::GetState().fullscreen);
					desktop.check();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					Test::UpdateWithPause();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.monitor_round_trip_and_changed_policy", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Test::UpdateWithPause();
		const DesktopSnapshot desktop;
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (size_t source = 0; source < monitors.size(); ++source)
		{
			CAPTURE(source);
			REQUIRE(Window::CenterToScreen(source));
			Test::UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (size_t i = 0; i < monitors.size(); ++i)
			{
				CAPTURE(i, monitors[i].name, monitors[i].displayRect, monitors[i].scaling.value_or(1.0));
				const auto windowed = Window::GetState();
				CAPTURE(windowed.bounds.pos, windowed.virtualSize, windowed.frameBufferSize, windowed.scaling);
				{
					auto config = Scene::GetConfig();
					config.sizeMode = SceneSizeMode::Window;
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					config.resolution = SceneResolution::SceneSize;
					config.scalingFilter = TextureFilter::Linear;
					Scene::Configure(config);
				}
				Test::SetFullscreenWithPause(true, i);
				REQUIRE(Window::GetState().fullscreen);
				CheckPresentation(SceneSizeMode::Window, false, TextureFilter::Linear);
				Test::UpdateWithPause();
				desktop.check();
				// Compare to the desktop before entering fullscreen, not the new mode.
				const auto& fullscreenMonitor = monitors[i];
				CAPTURE(fullscreenMonitor.displayRect, fullscreenMonitor.scaling.value_or(1.0));
				CHECK(System::GetCurrentMonitorIndex() == i);
				CHECK(Window::GetState().bounds.pos == fullscreenMonitor.displayRect.pos);
				CHECK(Window::GetState().virtualSize == fullscreenMonitor.displayRect.size);
				CHECK(Window::GetState().frameBufferSize == fullscreenMonitor.fullscreenResolution);
				CHECK(Window::GetState().scaling == fullscreenMonitor.scaling.value_or(1.0));
				CheckPresentation(SceneSizeMode::Window, false, TextureFilter::Linear);
				{
					auto config = Scene::GetConfig();
					config.sizeMode = SceneSizeMode::Fixed;
					config.fullscreen = FullscreenSceneMode::FollowWindow;
					config.fixedSceneSize = Size{ 320, 240 };
					config.resolution = SceneResolution::Display;
					config.scalingFilter = TextureFilter::Nearest;
					Scene::Configure(config);
				}
				Test::SetFullscreenWithPause(false);
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
				Test::UpdateWithPause();
				desktop.check();
				CHECK(Window::GetState().bounds == windowed.bounds);
				CHECK(Window::GetState().virtualSize == windowed.virtualSize);
				CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest, Size{ 320, 240 });
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.resize_exits_fullscreen", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { Test::SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		Test::UpdateWithPause();
		const DesktopSnapshot desktop;
		REQUIRE_FALSE(desktop.monitors.isEmpty());
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::Fixed;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.fixedSceneSize = Size{ 400, 300 };
			config.resolution = SceneResolution::Display;
			config.scalingFilter = TextureFilter::Nearest;
			Scene::Configure(config);
		}
		for (size_t source = 0; source < desktop.monitors.size(); ++source)
		{
			for (size_t target = 0; target < desktop.monitors.size(); ++target)
			{
				for (int32 api = 0; api < 3; ++api)
				{
					CAPTURE(source, target, api);
					const WindowStyle style = (api == 0) ? WindowStyle::Fixed
						: ((api == 1) ? WindowStyle::Sizable : WindowStyle::Frameless);
					Window::SetStyle(style);
					REQUIRE(Window::Resize(600, 400, CenterWindow::No));
					REQUIRE(Window::CenterToScreen(source));
					Test::UpdateWithPause();
					const auto windowed = Window::GetState();
					Test::SetFullscreenWithPause(true, target);
					REQUIRE(Window::GetState().fullscreen);
					const auto full = Window::GetState();
					CHECK(Window::Resize(full.virtualSize, CenterWindow::No));
					CHECK(Window::ResizeVirtual(full.virtualSize, CenterWindow::No));
					CHECK(Window::ResizeActual(full.frameBufferSize, CenterWindow::No));
					CHECK(Window::GetState().fullscreen);
					CHECK_THROWS_AS(Window::Resize(0, 400, CenterWindow::No), Error);
					CHECK_THROWS_AS(Window::ResizeActual(8193, 400, CenterWindow::No), Error);
					CHECK(Window::GetState().bounds == full.bounds);
					desktop.check();

					const Size requested{ 640, 420 };
					if (api == 0) { REQUIRE(Window::Resize(requested, CenterWindow::No)); }
					else if (api == 1) { REQUIRE(Window::ResizeVirtual(requested, CenterWindow::No)); }
					else { REQUIRE(Window::ResizeActual(requested, CenterWindow::No)); }
					Test::PauseForWindowTransition();
					REQUIRE_FALSE(Window::GetState().fullscreen);
					CHECK(Window::GetStyle() == style);
					CHECK(System::GetCurrentMonitorIndex() == source);
					CHECK(Window::GetState().scaling == windowed.scaling);
					CHECK(Window::GetState().bounds.pos == windowed.bounds.pos);
					if (api == 2) { CHECK(Window::GetState().frameBufferSize == requested); }
					else { CHECK(Window::GetState().virtualSize == requested); }
					CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest);
					Test::UpdateWithPause();
					desktop.check();
					CheckPresentation(SceneSizeMode::Fixed, true, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.minimized_state_matches_native_window", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreVisibility{ [] { Window::Restore(); Test::PauseForWindowTransition(); } };
		auto* window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		const auto waitForIconified = [window](const bool iconified)
		{
			// Cocoa may finish its animation over several event-loop iterations.
			for (int32 attempt = 0; attempt < 6; ++attempt)
			{
				Test::UpdateWithPause();
				if ((::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE) == iconified) { return; }
			}
			FAIL("The native window did not complete the minimize/restore transition");
		};
		const DesktopSnapshot desktop;
		for (size_t mode = 0; mode <= desktop.monitors.size(); ++mode)
		{
			const bool fullscreen = (mode != 0);
			CAPTURE(mode, fullscreen);
			if (fullscreen) { Test::SetFullscreenWithPause(true, (mode - 1)); }
			Window::Minimize();
			Test::PauseForWindowTransition();
			waitForIconified(true);
			REQUIRE(::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE);
			CHECK(Window::GetState().minimized);
			desktop.check();
			Window::Restore();
			Test::PauseForWindowTransition();
			waitForIconified(false);
			REQUIRE(::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_FALSE);
			CHECK_FALSE(Window::GetState().minimized);
			desktop.check();
			CHECK(Window::GetState().fullscreen == fullscreen);
			CHECK(Scene::Size().x > 0);
			CHECK(Scene::Size().y > 0);
			if (fullscreen) { Test::SetFullscreenWithPause(false); }
			desktop.check();
		}
	}
}

# endif
