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

# include "../Siv3D/src/Siv3D/Engine/Siv3DEngine.hpp"
# include "../Siv3D/src/Siv3D/Window/IWindow.hpp"
# include "../Siv3D/src/Siv3D/Scene/SceneUtility.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Windows/Windows.hpp>
# else
// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/GLFW/GLFW.hpp>
# endif

namespace s3d
{
	namespace
	{
		// Keep real-window transitions visible instead of rapidly flashing the desktop.
		constexpr int32 FullscreenPauseMillis = 500;

		void SetFullscreenWithPause(const bool fullscreen, const size_t monitorIndex = System::GetCurrentMonitorIndex())
		{
			Window::SetFullscreen(fullscreen, monitorIndex);
			System::Sleep(FullscreenPauseMillis);
		}

		void UpdateWithPause()
		{
			REQUIRE(System::Update());
			// Sleep does not consume another frame, preserving deferred-request tests.
			System::Sleep(FullscreenPauseMillis);
		}

		struct RestoreWindowAndScene
		{
			const WindowState window = Window::GetState();
			const Size sceneSize = Scene::Size();
			const ResizeMode mode = Scene::GetResizeMode();
			const bool highResolution = Scene::IsHighResolutionEnabled();
			const TextureFilter filter = Scene::GetTextureFilter();
			const bool toggleEnabled = Window::IsToggleFullscreenEnabled();

			~RestoreWindowAndScene()
			{
				SetFullscreenWithPause(false);
				Window::SetPos(window.bounds.pos);
				Window::Resize(window.virtualSize, CenterWindow::No);
				if (window.maximized) { Window::Maximize(); }
				Scene::SetHighResolutionEnabled(false);
				Scene::SetResizeMode(mode);
				Scene::Resize(sceneSize);
				Scene::SetHighResolutionEnabled(highResolution);
				Scene::SetTextureFilter(filter);
				Window::SetToggleFullscreenEnabled(toggleEnabled);
			}
		};

# if SIV3D_PLATFORM(WINDOWS)
		HWND MainWindow()
		{
			return static_cast<HWND>(SIV3D_ENGINE(Window)->getHandle());
		}

		void AltEnter()
		{
			// Exercise the real window message route without synthesizing global keyboard input.
			::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
			UpdateWithPause();
		}

# endif

		void CheckPresentation(const ResizeMode mode, const bool highResolution, const TextureFilter filter,
			const Size fixedSize = Size{ 400, 300 })
		{
			CHECK(Scene::GetResizeMode() == mode);
			CHECK(Scene::IsHighResolutionEnabled() == highResolution);
			CHECK(Scene::GetTextureFilter() == filter);
			const auto& window = Window::GetState();
			const Size expected = (mode == ResizeMode::Keep) ? fixedSize
				: ((mode == ResizeMode::Virtual) ? window.virtualSize : window.frameBufferSize);
			CHECK(Scene::Size() == expected);
			SceneMisc::RenderState reference;
			reference.size = expected;
			reference.frameBufferSize = window.frameBufferSize;
			reference.highResolutionEnabled = highResolution;
			CHECK(Scene::RenderSize() == reference.renderSize());
			const auto [scale, area] = reference.composition();
			CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
		}
	}

# if SIV3D_PLATFORM(WINDOWS)

	TEST_CASE("WindowFullscreen.api_and_shortcut_preserve_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		for (const ResizeMode mode : { ResizeMode::Keep, ResizeMode::Virtual, ResizeMode::Actual })
		{
			for (const bool enabled : { false, true })
			{
				INFO(static_cast<int32>(mode));
				INFO(enabled);
				Scene::SetResizeMode(mode);
				if (mode == ResizeMode::Keep) { Scene::Resize(400, 300); }
				Scene::SetHighResolutionEnabled(enabled);
				Scene::SetTextureFilter(TextureFilter::Nearest);

				AltEnter();
				REQUIRE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				// Reapplying the current window mode must leave the scene settings intact.
				SetFullscreenWithPause(true);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				SetFullscreenWithPause(false);
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				UpdateWithPause();
				CheckPresentation(mode, enabled, TextureFilter::Nearest);

				// Also enter via the API and leave via the shortcut.
				SetFullscreenWithPause(true);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);
				AltEnter();
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(mode, enabled, TextureFilter::Nearest);

				// Cover both same-method round trips for every presentation policy, too.
				for (const bool shortcut : { false, true })
				{
					INFO(shortcut);
					if (shortcut) { AltEnter(); }
					else { SetFullscreenWithPause(true); }
					REQUIRE(Window::GetState().fullscreen);
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					if (shortcut) { AltEnter(); }
					else { SetFullscreenWithPause(false); }
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
		UpdateWithPause();
		for (const bool exitByShortcut : { false, true })
		{
			INFO(exitByShortcut);
			Scene::SetResizeMode(ResizeMode::Virtual);
			Scene::SetHighResolutionEnabled(false);
			Scene::SetTextureFilter(TextureFilter::Linear);
			AltEnter();
			REQUIRE(Window::GetState().fullscreen);

			Scene::SetResizeMode(ResizeMode::Keep);
			Scene::Resize(320, 240);
			Scene::SetHighResolutionEnabled(true);
			Scene::SetTextureFilter(TextureFilter::Nearest);
			if (exitByShortcut) { AltEnter(); }
			else { SetFullscreenWithPause(false); }
			REQUIRE_FALSE(Window::GetState().fullscreen);
			CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
			UpdateWithPause();
			CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
		}

		// A disabled shortcut does not change the window, or leave a deferred toggle behind.
		Window::SetToggleFullscreenEnabled(false);
		AltEnter();
		CHECK_FALSE(Window::GetState().fullscreen);
		SetFullscreenWithPause(true);
		REQUIRE(Window::GetState().fullscreen);
		CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
		SetFullscreenWithPause(false);
		REQUIRE_FALSE(Window::GetState().fullscreen);
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });

		// Re-enabling before the next frame must not revive an ignored message.
		Window::SetToggleFullscreenEnabled(false);
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		SetFullscreenWithPause(false);

		// Disabling also cancels an accepted request that has not executed yet.
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Window::SetToggleFullscreenEnabled(false);
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		CHECK_FALSE(Window::GetState().fullscreen);
		SetFullscreenWithPause(false);

		// Reapplying enabled without disabling must preserve a valid request.
		::SendMessageW(MainWindow(), WM_SYSKEYDOWN, VK_RETURN, (LPARAM{ 1 } << 29));
		Window::SetToggleFullscreenEnabled(true);
		UpdateWithPause();
		CHECK(Window::GetState().fullscreen);
		SetFullscreenWithPause(false);
	}

	TEST_CASE("WindowFullscreen.bounds_match_selected_monitor", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		Scene::SetResizeMode(ResizeMode::Actual);
		Scene::SetHighResolutionEnabled(false);
		Scene::SetTextureFilter(TextureFilter::Linear);
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			RECT windowedBounds{};
			REQUIRE(::GetWindowRect(MainWindow(), &windowedBounds));
			const auto windowed = Window::GetState();
			const Rect expected = monitors[i].displayRect;
			INFO(i);
			INFO(expected);
			SetFullscreenWithPause(true, i);
			UpdateWithPause();
			REQUIRE(Window::GetState().fullscreen);
			RECT bounds{};
			REQUIRE(::GetWindowRect(MainWindow(), &bounds));
			CHECK(Rect{ bounds.left, bounds.top, (bounds.right - bounds.left), (bounds.bottom - bounds.top) } == expected);
			CHECK(Window::GetState().frameBufferSize == expected.size);
			CheckPresentation(ResizeMode::Actual, false, TextureFilter::Linear);
			SetFullscreenWithPause(false);
			UpdateWithPause();
			REQUIRE(::GetWindowRect(MainWindow(), &bounds));
			CHECK(bounds.left == windowedBounds.left);
			CHECK(bounds.top == windowedBounds.top);
			CHECK(bounds.right == windowedBounds.right);
			CHECK(bounds.bottom == windowedBounds.bottom);
			CHECK(Window::GetState().virtualSize == windowed.virtualSize);
			CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
		}
	}

# else

	namespace
	{
		struct NativeDisplay
		{
			CGDirectDisplayID id;
			CGRect bounds;
			int32 modeID;
			Size modeSize;
			Size pixels;
			double refreshRate;
			uint32 flags;
		};

		Array<NativeDisplay> ReadNativeDisplays()
		{
			uint32 count = 0;
			REQUIRE(::CGGetActiveDisplayList(0, nullptr, &count) == kCGErrorSuccess);
			Array<CGDirectDisplayID> ids(count);
			REQUIRE(::CGGetActiveDisplayList(count, ids.data(), &count) == kCGErrorSuccess);
			ids.resize(count);
			Array<NativeDisplay> result;
			for (const auto id : ids)
			{
				CGDisplayModeRef mode = ::CGDisplayCopyDisplayMode(id);
				REQUIRE(mode != nullptr);
				const ScopeExit release{ [=] { ::CGDisplayModeRelease(mode); } };
				result.push_back({ id, ::CGDisplayBounds(id), ::CGDisplayModeGetIODisplayModeID(mode),
					Size{ static_cast<int32>(::CGDisplayModeGetWidth(mode)), static_cast<int32>(::CGDisplayModeGetHeight(mode)) },
					Size{ static_cast<int32>(::CGDisplayModeGetPixelWidth(mode)), static_cast<int32>(::CGDisplayModeGetPixelHeight(mode)) },
					::CGDisplayModeGetRefreshRate(mode), ::CGDisplayModeGetIOFlags(mode) });
			}
			return result;
		}

		struct DesktopSnapshot
		{
			const Array<NativeDisplay> native = ReadNativeDisplays();
			const Array<MonitorInfo> monitors = System::EnumerateMonitors();

			void check() const
			{
				const auto currentNative = ReadNativeDisplays();
				REQUIRE(currentNative.size() == native.size());
				for (const auto& expected : native)
				{
					const auto actual = currentNative.find_if([&](const auto& display) { return (display.id == expected.id); });
					REQUIRE(actual != nullptr);
					CAPTURE(expected.id, actual->modeID, expected.modeID, actual->modeSize, actual->pixels);
					CHECK(::CGRectEqualToRect(actual->bounds, expected.bounds));
					CHECK(actual->modeID == expected.modeID);
					CHECK(actual->modeSize == expected.modeSize);
					CHECK(actual->pixels == expected.pixels);
					CHECK(actual->refreshRate == expected.refreshRate);
					CHECK(actual->flags == expected.flags);
				}
				const auto currentMonitors = System::EnumerateMonitors();
				REQUIRE(currentMonitors.size() == monitors.size());
				for (size_t i = 0; i < monitors.size(); ++i)
				{
					CAPTURE(i, monitors[i].displayRect, currentMonitors[i].displayRect);
					CHECK(currentMonitors[i].id == monitors[i].id);
					CHECK(currentMonitors[i].displayRect == monitors[i].displayRect);
					CHECK(currentMonitors[i].scaling == monitors[i].scaling);
					CHECK(currentMonitors[i].fullscreenResolution == monitors[i].fullscreenResolution);
				}
			}
		};
	}

	TEST_CASE("WindowFullscreen.macOS.api_preserves_presentation", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		const DesktopSnapshot desktop;
		const Point previousCursor = Cursor::ScreenPos();
		const ScopeExit restoreCursor{ [=] { ::CGWarpMouseCursorPosition(CGPointMake(previousCursor.x, previousCursor.y)); } };
		for (size_t monitor = 0; monitor < desktop.monitors.size(); ++monitor)
		{
			CAPTURE(monitor);
			REQUIRE(Window::CenterToScreen(monitor));
			UpdateWithPause();
			for (const ResizeMode mode : { ResizeMode::Keep, ResizeMode::Virtual, ResizeMode::Actual })
			{
				for (const bool enabled : { false, true })
				{
					CAPTURE(mode, enabled);
					Scene::SetResizeMode(mode);
					if (mode == ResizeMode::Keep) { Scene::Resize(400, 300); }
					Scene::SetHighResolutionEnabled(enabled);
					Scene::SetTextureFilter(TextureFilter::Nearest);
					SetFullscreenWithPause(true);
					REQUIRE(Window::GetState().fullscreen);
					desktop.check();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					UpdateWithPause();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					{
						const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
						const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
						const Point logical{ 100, 80 };
						const Mat3x2 all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
						Cursor::SetPos(logical);
						Circle{ logical, 12 }.draw(Palette::Red);
						ScreenCapture::RequestCurrentFrame();
						UpdateWithPause();
						CHECK(Cursor::PosF().epsilonEquals(logical, 1.0));
						CHECK(Vec2{ Cursor::PosRaw() }.epsilonEquals(all.transformPoint(logical), 1.0));
						CHECK(Circle{ logical, 3 }.mouseOver());
						CHECK_FALSE(Circle{ logical + Point{ 30, 0 }, 3 }.mouseOver());
						REQUIRE(ScreenCapture::HasNewFrame());
						CHECK(ScreenCapture::GetFrame().size() == Scene::RenderSize());
					}
					// Reapplying fullscreen must not change the selected policy.
					SetFullscreenWithPause(true);
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					SetFullscreenWithPause(false);
					REQUIRE_FALSE(Window::GetState().fullscreen);
					desktop.check();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
					UpdateWithPause();
					CheckPresentation(mode, enabled, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.monitor_round_trip_and_changed_policy", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		UpdateWithPause();
		const DesktopSnapshot desktop;
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (size_t source = 0; source < monitors.size(); ++source)
		{
			CAPTURE(source);
			REQUIRE(Window::CenterToScreen(source));
			UpdateWithPause();
			REQUIRE(System::GetCurrentMonitorIndex() == source);
			for (size_t i = 0; i < monitors.size(); ++i)
			{
				CAPTURE(i, monitors[i].name, monitors[i].displayRect, monitors[i].scaling.value_or(1.0));
				const auto windowed = Window::GetState();
				CAPTURE(windowed.bounds.pos, windowed.virtualSize, windowed.frameBufferSize, windowed.scaling);
				Scene::SetResizeMode(ResizeMode::Virtual);
				Scene::SetHighResolutionEnabled(false);
				Scene::SetTextureFilter(TextureFilter::Linear);
				SetFullscreenWithPause(true, i);
				REQUIRE(Window::GetState().fullscreen);
				CheckPresentation(ResizeMode::Virtual, false, TextureFilter::Linear);
				UpdateWithPause();
				desktop.check();
				// Compare to the desktop before entering fullscreen, not the new mode.
				const auto& fullscreenMonitor = monitors[i];
				CAPTURE(fullscreenMonitor.displayRect, fullscreenMonitor.scaling.value_or(1.0));
				CHECK(System::GetCurrentMonitorIndex() == i);
				CHECK(Window::GetState().bounds.pos == fullscreenMonitor.displayRect.pos);
				CHECK(Window::GetState().virtualSize == fullscreenMonitor.displayRect.size);
				CHECK(Window::GetState().frameBufferSize == fullscreenMonitor.fullscreenResolution);
				CHECK(Window::GetState().scaling == fullscreenMonitor.scaling.value_or(1.0));
				CheckPresentation(ResizeMode::Virtual, false, TextureFilter::Linear);
				Scene::SetResizeMode(ResizeMode::Keep);
				Scene::Resize(320, 240);
				Scene::SetHighResolutionEnabled(true);
				Scene::SetTextureFilter(TextureFilter::Nearest);
				SetFullscreenWithPause(false);
				REQUIRE_FALSE(Window::GetState().fullscreen);
				CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
				UpdateWithPause();
				desktop.check();
				CHECK(Window::GetState().bounds == windowed.bounds);
				CHECK(Window::GetState().virtualSize == windowed.virtualSize);
				CHECK(Window::GetState().frameBufferSize == windowed.frameBufferSize);
				CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest, Size{ 320, 240 });
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.resize_exits_fullscreen", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreStyle{ [&] { SetFullscreenWithPause(false); Window::SetStyle(restore.window.style); } };
		UpdateWithPause();
		const DesktopSnapshot desktop;
		REQUIRE_FALSE(desktop.monitors.isEmpty());
		Scene::SetResizeMode(ResizeMode::Keep);
		Scene::Resize(400, 300);
		Scene::SetHighResolutionEnabled(true);
		Scene::SetTextureFilter(TextureFilter::Nearest);
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
					UpdateWithPause();
					const auto windowed = Window::GetState();
					SetFullscreenWithPause(true, target);
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
					System::Sleep(FullscreenPauseMillis);
					REQUIRE_FALSE(Window::GetState().fullscreen);
					CHECK(Window::GetStyle() == style);
					CHECK(System::GetCurrentMonitorIndex() == source);
					CHECK(Window::GetState().scaling == windowed.scaling);
					CHECK(Window::GetState().bounds.pos == windowed.bounds.pos);
					if (api == 2) { CHECK(Window::GetState().frameBufferSize == requested); }
					else { CHECK(Window::GetState().virtualSize == requested); }
					CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest);
					UpdateWithPause();
					desktop.check();
					CheckPresentation(ResizeMode::Keep, true, TextureFilter::Nearest);
				}
			}
		}
	}

	TEST_CASE("WindowFullscreen.macOS.minimized_state_matches_native_window", "[.fullscreen]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const ScopeExit restoreVisibility{ [] { Window::Restore(); System::Sleep(FullscreenPauseMillis); } };
		auto* window = static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		const auto waitForIconified = [window](const bool iconified)
		{
			// Cocoa may finish its animation over several event-loop iterations.
			for (int32 attempt = 0; attempt < 6; ++attempt)
			{
				UpdateWithPause();
				if ((::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE) == iconified) { return; }
			}
			FAIL("The native window did not complete the minimize/restore transition");
		};
		const DesktopSnapshot desktop;
		for (size_t mode = 0; mode <= desktop.monitors.size(); ++mode)
		{
			const bool fullscreen = (mode != 0);
			CAPTURE(mode, fullscreen);
			if (fullscreen) { SetFullscreenWithPause(true, (mode - 1)); }
			Window::Minimize();
			System::Sleep(FullscreenPauseMillis);
			waitForIconified(true);
			REQUIRE(::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_TRUE);
			CHECK(Window::GetState().minimized);
			desktop.check();
			Window::Restore();
			System::Sleep(FullscreenPauseMillis);
			waitForIconified(false);
			REQUIRE(::glfwGetWindowAttrib(window, GLFW_ICONIFIED) == GLFW_FALSE);
			CHECK_FALSE(Window::GetState().minimized);
			desktop.check();
			CHECK(Window::GetState().fullscreen == fullscreen);
			CHECK(Scene::Size().x > 0);
			CHECK(Scene::Size().y > 0);
			if (fullscreen) { SetFullscreenWithPause(false); }
			desktop.check();
		}
	}

# endif
}

# endif
