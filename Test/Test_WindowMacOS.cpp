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
# include <objc/runtime.h>

namespace s3d
{
	namespace
	{
		GLFWwindow* MainWindow()
		{
			return static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		}

		struct RestoreWindowAndScene : Test::WindowSceneSnapshot
		{
			const bool toggleEnabled = Window::IsToggleFullscreenEnabled();

			~RestoreWindowAndScene()
			{
				if (Window::GetState().fullscreen)
				{
					Test::SetFullscreenWithPause(false);
				}
				Window::SetMinimumFrameBufferSize(window.minFrameBufferSize);
				Window::SetStyle(window.style);
				Test::RestorePlacement(window, Test::WindowSizeUnit::Virtual, Test::PlacementOrder::PositionThenSize);
				if (window.maximized) { Window::Maximize(); }
				if (window.minimized) { Window::Minimize(); }
				Window::SetToggleFullscreenEnabled(toggleEnabled);
				Scene::Configure(config);
			}
		};

		void CheckMinimum(const Size pixels, const double scale)
		{
			const NSSize points = [::glfwGetCocoaWindow(MainWindow()) contentMinSize];
			CAPTURE(pixels, scale, points.width, points.height);
			CHECK(Window::GetState().minFrameBufferSize == pixels);
			CHECK(points.width == std::ceil(pixels.x / scale));
			CHECK(points.height == std::ceil(pixels.y / scale));
			CHECK(points.width * scale >= pixels.x);
			CHECK(points.height * scale >= pixels.y);
		}

		// Control only this window's reported scale. This checks state refresh and
		// native constraints, without pretending to move to another physical display.
		class ReportedBackingScale
		{
		public:

			double value = 1.0;

			ReportedBackingScale()
			{
				NSWindow* target = ::glfwGetCocoaWindow(MainWindow());
				m_method = class_getInstanceMethod(object_getClass(target), @selector(backingScaleFactor));
				REQUIRE(m_method);
				m_original = method_getImplementation(m_method);
				const auto original = reinterpret_cast<CGFloat(*)(id, SEL)>(m_original);
				auto* probe = this;
				m_replacement = imp_implementationWithBlock(^CGFloat(id receiver)
				{
					return (receiver == target) ? probe->value : original(receiver, @selector(backingScaleFactor));
				});
				REQUIRE(m_replacement);
				method_setImplementation(m_method, m_replacement);
			}

			~ReportedBackingScale()
			{
				method_setImplementation(m_method, m_original);
				imp_removeBlock(m_replacement);
			}

		private:

			Method m_method = nullptr;
			IMP m_original = nullptr;
			IMP m_replacement = nullptr;
		};

		void CheckWindowScene(const SceneConfig& config)
		{
			const auto& window = Window::GetState();
			CHECK(Scene::GetConfig() == config);
			CHECK(Scene::Size() == window.virtualSize);
			CHECK(Scene::RenderResolution() == window.frameBufferSize);
			CHECK(Scene::ClientToScene(window.frameBufferSize * 0.5).epsilonEquals(Scene::CenterF(), 1e-6));
		}
	}

	TEST_CASE("WindowMacOS.minimum_framebuffer_size_and_styles")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetStyle(WindowStyle::Sizable);
		for (const Size pixels : { Size{ 0, 0 }, Size{ 1, 1 }, Size{ 320, 240 }, Size{ 321, 241 } })
		{
			Window::SetMinimumFrameBufferSize(pixels);
			CheckMinimum(pixels, Window::GetState().scaling);
			REQUIRE(System::Update());
			CheckMinimum(pixels, Window::GetState().scaling);
		}
		for (const auto style : { WindowStyle::Fixed, WindowStyle::Frameless })
		{
			Window::SetStyle(style);
			Window::SetMinimumFrameBufferSize({ 351, 253 });
			Window::SetStyle(WindowStyle::Sizable);
			CheckMinimum({ 351, 253 }, Window::GetState().scaling);
			Window::SetMinimumFrameBufferSize({ 321, 241 });
		}
	}

	TEST_CASE("WindowMacOS.minimum_size_reapplied_on_reported_scale_change")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::SetStyle(WindowStyle::Sizable);
		Window::SetMinimumFrameBufferSize({ 321, 241 });
		{
			ReportedBackingScale scale;
			for (const double value : { 1.0, 2.0, 1.0 })
			{
				scale.value = value;
				// Refresh CWindow only: the framebuffer itself is not a simulated display.
				SIV3D_ENGINE(Window)->update();
				CHECK(Window::GetState().scaling == value);
				CheckMinimum({ 321, 241 }, value);
			}
		}
		REQUIRE(System::Update());
		CheckMinimum({ 321, 241 }, Window::GetState().scaling);
	}

	TEST_CASE("WindowMacOS.minimum_size_tracks_attached_monitors", "[.screen-coordinates][.window-ui]")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		Window::SetMinimumFrameBufferSize({ 321, 241 });
		REQUIRE(Window::Resize(480, 320, CenterWindow::No));
		const auto monitors = System::EnumerateMonitors();
		bool mixedDPI = false;
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			Catch::cout() << "monitor[" << i << "] " << monitors[i].format().toUTF8() << '\n';
			REQUIRE(Window::CenterToScreen(i));
			CHECK(Window::GetState().scaling == monitors[i].scaling.value_or(1.0));
			CheckMinimum({ 321, 241 }, Window::GetState().scaling);
			Test::UpdateWithPause();
			CheckMinimum({ 321, 241 }, Window::GetState().scaling);
			mixedDPI |= (monitors[i].scaling != monitors.front().scaling);
		}
		if (not mixedDPI) { Catch::cout() << "No attached mixed-DPI pair; physical scale transition not exercised.\n"; }
	}

	TEST_CASE("WindowMacOS.native_fullscreen_detach_updates_state", "[.fullscreen][.window-ui]")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindowAndScene restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		Window::SetToggleFullscreenEnabled(true);
		REQUIRE(Window::Resize(384, 256, CenterWindow::No));
		auto config = SceneConfig::Window(SceneResolution::Display);
		Scene::Configure(config);
		Window::SetFullscreen(true);
		Test::PauseForWindowTransition();
		REQUIRE(Window::GetState().fullscreen);
		REQUIRE(::glfwGetWindowMonitor(MainWindow()) != nullptr);
		CHECK(Scene::Size() == Size{ 384, 256 });
		config.scalingFilter = TextureFilter::Nearest;
		Scene::Configure(config);
		Window::SetMinimumFrameBufferSize({ 321, 241 });

		// GLFW's monitor-disconnect handler detaches with the current client size,
		// then positions the client using its frame offsets. Do not disconnect hardware.
		int width, height, xoff, yoff;
		::glfwGetWindowSize(MainWindow(), &width, &height);
		::glfwSetWindowMonitor(MainWindow(), nullptr, 0, 0, width, height, 0);
		::glfwGetWindowFrameSize(MainWindow(), &xoff, &yoff, nullptr, nullptr);
		::glfwSetWindowPos(MainWindow(), xoff, yoff);
		Test::PauseForWindowTransition();
		REQUIRE(::glfwGetWindowMonitor(MainWindow()) == nullptr);
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		CHECK(Scene::Size() != Size{ 384, 256 });
		CheckWindowScene(config);
		CheckMinimum({ 321, 241 }, Window::GetState().scaling);

		Window::SetStyle(WindowStyle::Fixed);
		CHECK(Window::GetState().style == WindowStyle::Fixed);
		Window::SetPos({ 40, 60 });
		CHECK(Window::GetPos() == Point{ 40, 60 });
		REQUIRE(Window::Resize(480, 320, CenterWindow::No));
		CHECK(Window::GetState().virtualSize == Size{ 480, 320 });
		CheckWindowScene(config);
		const auto windowed = Window::GetState();

		// A new shortcut must enter fullscreen, then API exit restores this new window.
		NSWindow* native = ::glfwGetCocoaWindow(MainWindow());
		NSEvent* event = [NSEvent keyEventWithType:NSEventTypeKeyDown location:NSZeroPoint
			modifierFlags:(NSEventModifierFlagControl | NSEventModifierFlagCommand) timestamp:0
			windowNumber:native.windowNumber context:nil characters:@"f"
			charactersIgnoringModifiers:@"f" isARepeat:NO keyCode:0x03];
		REQUIRE([native performKeyEquivalent:event]);
		Test::UpdateWithPause();
		REQUIRE(Window::GetState().fullscreen);
		CHECK(Scene::Size() == windowed.virtualSize);
		Window::SetFullscreen(false);
		Test::PauseForWindowTransition();
		REQUIRE_FALSE(Window::GetState().fullscreen);
		CHECK(Window::GetState().bounds.pos == windowed.bounds.pos);
		CHECK(Window::GetState().virtualSize == windowed.virtualSize);
		CheckWindowScene(config);
		Window::SetStyle(WindowStyle::Sizable);
		CheckMinimum({ 321, 241 }, Window::GetState().scaling);
	}
}

# endif
