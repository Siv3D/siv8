//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "WindowSceneTestHelper.hpp"

# if SIV3D_PLATFORM(MACOS)

// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d
{
	namespace
	{
		GLFWwindow* EngineWindow()
		{
			return static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		}

		struct RestoreWindowAndScene : Test::WindowSceneSnapshot
		{
			~RestoreWindowAndScene()
			{
				Window::SetStyle(window.style);
				Test::RestorePlacement(window, Test::WindowSizeUnit::Virtual, Test::PlacementOrder::SizeThenPosition);
				if (window.maximized) { Window::Maximize(); }
				Scene::Configure(config);
				Test::PauseForWindowTransition();
			}
		};

		void CheckNativeWindow()
		{
			const WindowState state = Window::GetState();
			NSWindow* window = ::glfwGetCocoaWindow(EngineWindow());
			const NSRect frame = [window frame];
			const NSRect content = [window contentRectForFrameRect:frame];
			const double desktopTop = [[[NSScreen screens] firstObject] frame].size.height;
			CAPTURE(state.bounds, state.virtualSize, state.frameBufferSize, state.scaling);
			CHECK(state.bounds.x == Test::Approx(frame.origin.x));
			CHECK((state.bounds.y + state.bounds.h) == Test::Approx(desktopTop - frame.origin.y));
			CHECK(state.bounds.w == Test::Approx(frame.size.width));
			CHECK(state.bounds.h == Test::Approx(frame.size.height));
			CHECK(state.virtualSize.x == Test::Approx(content.size.width));
			CHECK(state.virtualSize.y == Test::Approx(content.size.height));
			CHECK(state.titleBarHeight == Test::Approx(NSMaxY(frame) - NSMaxY(content)));
			CHECK(state.scaling == [window backingScaleFactor]);
			Size framebuffer;
			::glfwGetFramebufferSize(EngineWindow(), &framebuffer.x, &framebuffer.y);
			CHECK(state.frameBufferSize == framebuffer);
			CHECK(Window::GetPos() == state.bounds.pos);
		}

		void CheckPresentation(const SceneSizeMode mode, const bool highResolution)
		{
			const auto& window = Window::GetState();
			const Size logical = (mode == SceneSizeMode::Fixed) ? Size{ 400, 300 }
				: ((mode == SceneSizeMode::Window) ? window.virtualSize : window.frameBufferSize);
			const double fit = Min(static_cast<double>(window.frameBufferSize.x) / logical.x,
				static_cast<double>(window.frameBufferSize.y) / logical.y);
			CHECK(Scene::GetConfig().sizeMode == mode);
			CHECK((Scene::GetConfig().resolution == SceneResolution::Display) == highResolution);
			CHECK(Scene::Size() == logical);
			CHECK(Scene::RenderResolution() == (highResolution ? Math::Round(logical * Max(1.0, fit)).asPoint() : logical));
			CHECK(Scene::ClientToScene(window.frameBufferSize * 0.5).epsilonEquals(logical * 0.5, 1e-6));
		}
	}

	TEST_CASE("ScreenCoordinates.macOS.native_geometry_and_invalid_monitor")
	{
		REQUIRE(System::Update());
		CheckNativeWindow();
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		const double desktopTop = [[[NSScreen screens] firstObject] frame].size.height;
		for (const auto& monitor : monitors)
		{
			CAPTURE(monitor.name, monitor.displayRect, monitor.workArea, monitor.scaling);
			const CGDirectDisplayID displayID = Parse<uint32>(monitor.id);
			const CGRect native = ::CGDisplayBounds(displayID);
			CHECK(monitor.displayRect == Rect(native.origin.x, native.origin.y, native.size.width, native.size.height));
			bool found = false;
			for (NSScreen* screen in [NSScreen screens])
			{
				if ([screen.deviceDescription[@"NSScreenNumber"] unsignedIntValue] != displayID) { continue; }
				found = true;
				const NSRect work = [screen visibleFrame];
				CHECK(monitor.workArea.x == Test::Approx(work.origin.x));
				CHECK((monitor.workArea.y + monitor.workArea.h) == Test::Approx(desktopTop - work.origin.y));
				CHECK(monitor.workArea.w == Test::Approx(work.size.width));
				CHECK(monitor.workArea.h == Test::Approx(work.size.height));
				CHECK(monitor.scaling == [screen backingScaleFactor]);
				const NSSize backing = [screen convertRectToBacking:screen.frame].size;
				CHECK(monitor.fullscreenResolution == Size{ static_cast<int32>(std::round(backing.width)),
					static_cast<int32>(std::round(backing.height)) });
			}
			CHECK(found);
		}
		const Point before = Window::GetPos();
		CHECK_FALSE(Window::CenterToScreen(monitors.size()));
		CHECK_FALSE(Window::CenterToScreen(std::numeric_limits<size_t>::max()));
		CHECK(Window::GetPos() == before);
	}

	TEST_CASE("ScreenCoordinates.macOS.placement_round_trips", "[.screen-coordinates]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		{
			auto config = Scene::GetConfig();
			config.sizeMode = SceneSizeMode::Fixed;
			config.fullscreen = FullscreenSceneMode::FollowWindow;
			config.fixedSceneSize = Size{ 400, 300 };
			config.resolution = SceneResolution::Display;
			Scene::Configure(config);
		}
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (const WindowStyle style : { WindowStyle::Fixed, WindowStyle::Sizable, WindowStyle::Frameless })
		{
			Window::SetStyle(style);
			Test::PauseForWindowTransition();
			for (size_t i = 0; i < monitors.size(); ++i)
			{
				CAPTURE(style, i, monitors[i].displayRect, monitors[i].scaling);
				const Size before = Window::GetState().virtualSize;
				// The Dock can change visibleFrame while the test is running.
				const Rect workArea = System::EnumerateMonitors()[i].workArea;
				CAPTURE(monitors[i].workArea, workArea);
				REQUIRE(Window::CenterToScreen(i));
				const Rect bounds = Window::GetState().bounds;
				CHECK(bounds.pos == (workArea.pos + (workArea.size - bounds.size) / 2));
				CheckNativeWindow();
				CheckPresentation(SceneSizeMode::Fixed, true);
				Test::UpdateWithPause();
				CHECK(System::GetCurrentMonitorIndex() == i);
				CHECK(Window::GetState().virtualSize == before);
				CHECK(Window::GetState().bounds == bounds);
				const Point shifted = (bounds.pos + Point{ -37, 19 });
				Window::SetPos(shifted);
				CHECK(Window::GetPos() == shifted);
				Test::UpdateWithPause();
				CheckNativeWindow();
				CHECK(System::GetCurrentMonitorIndex() == i);
				Window::SetPos(bounds.pos);
				CHECK(Window::GetState().bounds == bounds);
				Test::UpdateWithPause();
				const Rect currentWorkArea = System::GetCurrentMonitor().workArea;
				REQUIRE(Window::CenterToScreen());
				CHECK(Window::GetPos() == (currentWorkArea.pos + (currentWorkArea.size - bounds.size) / 2));
				Test::UpdateWithPause();
			}
		}
	}

	TEST_CASE("ScreenCoordinates.macOS.cursor_pixels_and_scene_transforms", "[.screen-coordinates]")
	{
		REQUIRE_FALSE(Window::GetState().fullscreen);
		CGEventRef savedEvent = ::CGEventCreate(nullptr);
		REQUIRE(savedEvent != nullptr);
		const CGPoint savedCursor = ::CGEventGetLocation(savedEvent);
		::CFRelease(savedEvent);
		const ScopeExit restoreCursor{ [=] { ::CGWarpMouseCursorPosition(savedCursor); } };
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Fixed);
		REQUIRE(Window::Resize(600, 400, CenterWindow::No));
		::glfwFocusWindow(EngineWindow());
		Test::UpdateWithPause();
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			REQUIRE(Window::CenterToScreen(i));
			Test::UpdateWithPause();
			CHECK(System::GetCurrentMonitorIndex() == i);
			for (const SceneSizeMode mode : { SceneSizeMode::Fixed, SceneSizeMode::Window, SceneSizeMode::WindowPixels })
			{
				for (const bool highResolution : { false, true })
				{
					CAPTURE(i, mode, highResolution);
					{
						auto config = Scene::GetConfig();
						config.sizeMode = mode;
						config.fullscreen = FullscreenSceneMode::FollowWindow;
						config.fixedSceneSize = Size{ 400, 300 };
						config.resolution = (highResolution ? SceneResolution::Display : SceneResolution::SceneSize);
						Scene::Configure(config);
					}
					CheckPresentation(mode, highResolution);
					const Point logical{ 40, 30 };
					const Mat3x2 all = (Cursor::GetLocalTransform() * Cursor::GetCameraTransform() * Cursor::GetBaseWindowTransform());
					const Vec2 expectedRaw = all.transformPoint(logical);
					Cursor::SetPos(logical);
					Test::UpdateWithPause();
					CHECK(Cursor::PosF().epsilonEquals(logical, 1.0));
					CHECK(Vec2{ Cursor::PosRaw() }.epsilonEquals(expectedRaw, 1.0));
					CHECK(Circle{ logical, 3 }.mouseOver());
					CHECK_FALSE(Circle{ logical + Point{ 30, 0 }, 3 }.mouseOver());
					const auto& state = Window::GetState();
					const Vec2 expectedScreen = (state.bounds.pos + Point{ 0, state.titleBarHeight }) + expectedRaw / state.scaling;
					CHECK(Vec2{ Cursor::ScreenPos() }.epsilonEquals(expectedScreen, 1.0));
					CHECK(Cursor::ScreenDelta() == (Cursor::ScreenPos() - Cursor::ScreenPreviousPos()));
				}
			}

			// Crossing a monitor boundary must not change the unit of the raw client
			// coordinate. The cursor can be outside its window on another DPI screen.
			const size_t other = ((i + 1) % monitors.size());
			const Point target = monitors[other].workArea.center().asPoint();
			const Point previous = Cursor::ScreenPos();
			REQUIRE(::CGWarpMouseCursorPosition(CGPointMake(target.x, target.y)) == kCGErrorSuccess);
			Test::UpdateWithPause();
			CHECK(Cursor::ScreenPos() == target);
			CHECK(Cursor::ScreenPreviousPos() == previous);
			CHECK(Cursor::ScreenDelta() == (target - previous));
			NSWindow* window = ::glfwGetCocoaWindow(EngineWindow());
			NSView* view = ::glfwGetCocoaView(EngineWindow());
			const double desktopTop = [[[NSScreen screens] firstObject] frame].size.height;
			const NSPoint inWindow = [window convertPointFromScreen:NSMakePoint(target.x, (desktopTop - target.y))];
			const NSPoint inView = [view convertPoint:inWindow fromView:nil];
			const NSPoint inPixels = [view convertPointToBacking:inView];
			const double height = [view convertRectToBacking:view.bounds].size.height;
			CHECK(Vec2{ Cursor::PosRaw() }.epsilonEquals(Vec2{ inPixels.x, (height - inPixels.y) }, 1.0));
		}
	}
}

# elif SIV3D_PLATFORM(WINDOWS)

# include <Siv3D/Windows/Windows.hpp>
# include <dwmapi.h>

namespace s3d
{
	namespace
	{
		HWND MainWindow()
		{
			return static_cast<HWND>(Platform::Windows::Window::GetHWND());
		}

		Rect NativeWindowRect()
		{
			RECT rect{};
			REQUIRE(::GetWindowRect(MainWindow(), &rect));
			return { rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top };
		}

		void CheckVisibleFrame(const Point expected)
		{
			RECT frame{};
			REQUIRE(SUCCEEDED(::DwmGetWindowAttribute(MainWindow(), DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame))));
			const Rect visible{ frame.left, frame.top, frame.right - frame.left, frame.bottom - frame.top };
			CHECK(visible.pos == expected);
			CHECK(Window::GetState().bounds == visible);
		}

		struct RestoreWindowAndScene : Test::WindowSceneSnapshot
		{
			~RestoreWindowAndScene()
			{
				Window::Restore();
				Window::SetMinimumFrameBufferSize(window.minFrameBufferSize);
				Window::SetStyle(window.style);
				Test::RestorePlacement(window, Test::WindowSizeUnit::FrameBuffer, Test::PlacementOrder::SizeThenPosition);
				if (window.maximized) { Window::Maximize(); }
				if (window.minimized) { Window::Minimize(); }
				Scene::Configure(config);
			}
		};
	}

	TEST_CASE("ScreenCoordinates.Windows.initial_placement")
	{
		// Run this case alone to exercise the initially hidden HWND before the first frame.
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		const bool visible = (::IsWindowVisible(MainWindow()) != FALSE);
		Catch::cout() << "initial HWND visible=" << visible << '\n';
		const Point requested = (System::GetCurrentMonitor().workArea.pos + Point{ 140, 160 });
		Window::SetPos(requested);
		CHECK((::IsWindowVisible(MainWindow()) != FALSE) == visible);
		CHECK(Window::GetPos() == requested);
		REQUIRE(System::Update());
		CheckVisibleFrame(requested);
	}

	TEST_CASE("ScreenCoordinates.Windows.placement_round_trips")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetMinimumFrameBufferSize({ 1, 1 });
		REQUIRE(Window::Resize(500, 350, CenterWindow::No));
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (const auto style : { WindowStyle::Fixed, WindowStyle::Sizable, WindowStyle::Frameless })
		{
			const Point beforeStyle = Window::GetPos();
			Window::SetStyle(style);
			CheckVisibleFrame(beforeStyle);
			for (size_t i = 0; i < monitors.size(); ++i)
			{
				CAPTURE(style, i, monitors[i].workArea, monitors[i].scaling);
				Catch::cout() << "placement style=" << static_cast<int32>(style) << " monitor[" << i << "] " << monitors[i].format().toUTF8() << '\n';
				const Point anchor = (monitors[i].workArea.pos + Point{ 140, 160 });
				// Includes a direct move across DPI boundaries, without a preliminary centering call.
				Window::SetPos(anchor);
				CheckVisibleFrame(anchor);
				CHECK(System::GetCurrentMonitorIndex() == i);
				for (int32 repeat = 0; repeat < 4; ++repeat)
				{
					CAPTURE(repeat);
					Window::SetPos(Window::GetPos());
					CheckVisibleFrame(anchor);
					Window::SetPos(anchor + Point{ 17, -11 });
					CheckVisibleFrame(anchor + Point{ 17, -11 });
					Window::SetPos(anchor);
					CheckVisibleFrame(anchor);
				}
				for (const bool indexed : { true, false })
				{
					REQUIRE((indexed ? Window::CenterToScreen(i) : Window::CenterToScreen()));
					const Point centered = (monitors[i].workArea.pos + (monitors[i].workArea.size - Window::GetState().bounds.size) / 2);
					CheckVisibleFrame(centered);
				}
				const auto immediate = Window::GetState();
				Test::PauseForWindowTransition();
				REQUIRE(System::Update());
				CHECK(Window::GetState().bounds == immediate.bounds);
				CheckVisibleFrame(immediate.bounds.pos);
			}
		}
	}

	TEST_CASE("ScreenCoordinates.Windows.resize_preserves_position")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetMinimumFrameBufferSize({ 1, 1 });
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		using ResizeFunction = bool (*)(Size, CenterWindow);
		const ResizeFunction resize[] = { Window::Resize, Window::ResizeVirtual, Window::ResizeActual };
		for (const auto style : { WindowStyle::Fixed, WindowStyle::Sizable, WindowStyle::Frameless })
		{
			Window::SetStyle(style);
			for (size_t i = 0; i < monitors.size(); ++i)
			{
				CAPTURE(style, i, monitors[i].scaling);
				REQUIRE(Window::CenterToScreen(i));
				const Point visible = Window::GetPos();
				const Point native = NativeWindowRect().pos;
				for (int32 repeat = 0; repeat < 4; ++repeat)
				{
					for (int32 api = 0; api < 3; ++api)
					{
						CAPTURE(repeat, api);
						const Size requested{ 500 + 13 * (repeat % 2) + api, 350 + 7 * (repeat % 2) + api };
						REQUIRE(resize[api](requested, CenterWindow::No));
						CHECK((api == 2 ? Window::GetState().frameBufferSize : Window::GetState().virtualSize) == requested);
						CHECK(NativeWindowRect().pos == native);
						CheckVisibleFrame(visible);
					}
				}
				Test::PauseForWindowTransition();
				REQUIRE(System::Update());
				CHECK(NativeWindowRect().pos == native);
				CheckVisibleFrame(visible);
			}
		}
	}

	TEST_CASE("ScreenCoordinates.Windows.dpi_notification_uses_native_origin")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		REQUIRE(Window::Resize(500, 350, CenterWindow::No));
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			CAPTURE(i, monitors[i].scaling);
			REQUIRE(Window::CenterToScreen(i));
			const Point visible = Window::GetPos();
			const Rect native = NativeWindowRect();
			const auto dpi = ::GetDpiForWindow(MainWindow());
			for (int32 repeat = 0; repeat < 4; ++repeat)
			{
				CAPTURE(repeat);
				RECT suggested{ native.x, native.y, native.x + native.w, native.y + native.h };
				// Dispatch through the real HWND thread. Same-DPI notification checks
				// the coordinate contract and reentry; this is not an OS scale-change test.
				::SendMessageW(MainWindow(), WM_DPICHANGED, MAKEWPARAM(dpi, dpi), reinterpret_cast<LPARAM>(&suggested));
				Window::SetMinimumFrameBufferSize(restore.window.minFrameBufferSize); // Publish the completed notification.
				CHECK(NativeWindowRect().pos == native.pos);
				CheckVisibleFrame(visible);
			}
			Test::PauseForWindowTransition();
		}
	}
}

# endif
