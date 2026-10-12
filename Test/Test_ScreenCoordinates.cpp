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

		void UpdateWithPause()
		{
			REQUIRE(System::Update());
			System::Sleep(500);
		}

		struct RestoreWindowAndScene
		{
			const WindowState window = Window::GetState();
			const SceneConfig config = Scene::GetConfig();

			~RestoreWindowAndScene()
			{
				Window::SetStyle(window.style);
				Window::Resize(window.virtualSize, CenterWindow::No);
				Window::SetPos(window.bounds.pos);
				if (window.maximized) { Window::Maximize(); }
				Scene::Configure(config);
				System::Sleep(500);
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
			System::Sleep(500);
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
				UpdateWithPause();
				CHECK(System::GetCurrentMonitorIndex() == i);
				CHECK(Window::GetState().virtualSize == before);
				CHECK(Window::GetState().bounds == bounds);
				const Point shifted = (bounds.pos + Point{ -37, 19 });
				Window::SetPos(shifted);
				CHECK(Window::GetPos() == shifted);
				UpdateWithPause();
				CheckNativeWindow();
				CHECK(System::GetCurrentMonitorIndex() == i);
				Window::SetPos(bounds.pos);
				CHECK(Window::GetState().bounds == bounds);
				UpdateWithPause();
				const Rect currentWorkArea = System::GetCurrentMonitor().workArea;
				REQUIRE(Window::CenterToScreen());
				CHECK(Window::GetPos() == (currentWorkArea.pos + (currentWorkArea.size - bounds.size) / 2));
				UpdateWithPause();
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
		UpdateWithPause();
		const auto monitors = System::EnumerateMonitors();
		REQUIRE_FALSE(monitors.isEmpty());
		const Transformer2D local{ Mat3x2::Translate(20, 10), TransformCursor::Yes, Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Scale(1.5), TransformCursor::Yes, Transformer2D::Target::SetCamera };
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			REQUIRE(Window::CenterToScreen(i));
			UpdateWithPause();
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
					UpdateWithPause();
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
			UpdateWithPause();
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

# endif
