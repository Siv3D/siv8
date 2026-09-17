//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)

// Xcode compiles this file as Objective-C++ with NO_S3D_USING.
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/GLFW/GLFW.hpp>
# import <QuartzCore/CAMetalLayer.h>

extern "C"
{
	const char* glfwGetKeysSiv3D(GLFWwindow* window);
	void glfwGetMonitorRect_Siv3D(GLFWmonitor* monitor, int* x, int* y, int* width, int* height);
	void glfwGetMonitorInfo_Siv3D(GLFWmonitor* monitor, uint32_t* displayID, uint32_t* unitNumber,
		int* x, int* y, int* width, int* height, int* workX, int* workY, int* workWidth, int* workHeight);
	int glfwIsCurrentMonitor_Siv3D(GLFWwindow* window, GLFWmonitor* monitor);
}

namespace s3d
{
	namespace
	{
		GLFWwindow* EngineWindow()
		{
			return static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
		}
	}

	TEST_CASE("GLFW.VersionAndTimer")
	{
		int major = 0, minor = 0, revision = 0;
		glfwGetVersion(&major, &minor, &revision);
		CHECK(major == 3);
		CHECK(minor == 5);
		CHECK(revision == 1);
		CHECK(major == GLFW_VERSION_MAJOR);
		CHECK(minor == GLFW_VERSION_MINOR);
		CHECK(revision == GLFW_VERSION_REVISION);
		CHECK(glfwGetPlatform() == GLFW_PLATFORM_COCOA);
		CHECK(glfwGetTimerFrequency() > 0);
		const uint64 before = glfwGetTimerValue();
		CHECK(glfwGetTimerValue() >= before);
	}

	TEST_CASE("GLFW.JISScancodes")
	{
		CHECK(glfwGetKeyScancode(SIV3D_KEY_JIS_YEN) == 0x5D);
		CHECK(glfwGetKeyScancode(SIV3D_KEY_JIS_UNDERSCORE) == 0x5E);
		CHECK(glfwGetKeyScancode(GLFW_KEY_BACKSLASH) == 0x2A);
		CHECK(glfwGetKeyScancode(GLFW_KEY_A) == 0x00);
	}

	TEST_CASE("GLFW.CocoaShortcutsUpdateSiv3DKeyBuffer")
	{
		@autoreleasepool
		{
			GLFWwindow* window = EngineWindow();
			NSView* view = glfwGetCocoaView(window);
			const char* keys = glfwGetKeysSiv3D(window);
			REQUIRE(keys != nullptr);
			REQUIRE(glfwGetInputMode(window, GLFW_STICKY_KEYS) == GLFW_FALSE);
			const GLFWkeyfun previousCallback = glfwSetKeyCallback(window, nullptr);
			const ScopeExit restoreCallback{ [=] { glfwSetKeyCallback(window, previousCallback); } };

			struct Shortcut
			{
				int key;
				unsigned short scancode;
				NSEventModifierFlags modifiers;
			};
			const Shortcut shortcuts[] = {
				{ GLFW_KEY_TAB, 0x30, NSEventModifierFlagControl },
				{ GLFW_KEY_ESCAPE, 0x35, NSEventModifierFlagControl },
				{ GLFW_KEY_PERIOD, 0x2F, NSEventModifierFlagCommand },
			};

			for (const auto& shortcut : shortcuts)
			{
				CAPTURE(shortcut.key);
				NSEvent* press = [NSEvent keyEventWithType:NSEventTypeKeyDown
					location:NSZeroPoint modifierFlags:shortcut.modifiers timestamp:0
					windowNumber:view.window.windowNumber context:nil characters:@""
					charactersIgnoringModifiers:@"" isARepeat:NO keyCode:shortcut.scancode];
				NSEvent* release = [NSEvent keyEventWithType:NSEventTypeKeyUp
					location:NSZeroPoint modifierFlags:shortcut.modifiers timestamp:0
					windowNumber:view.window.windowNumber context:nil characters:@""
					charactersIgnoringModifiers:@"" isARepeat:NO keyCode:shortcut.scancode];
				REQUIRE(press != nil);
				REQUIRE(release != nil);
				const int previousState = keys[shortcut.key];
				const ScopeExit restoreKey{ [=]
				{
					[view keyUp:release];
					if (previousState == GLFW_PRESS)
					{
						[view performKeyEquivalent:press];
					}
				} };

				CHECK([view performKeyEquivalent:press] == YES);
				CHECK(keys[shortcut.key] == GLFW_PRESS);
				CHECK(glfwGetKey(window, shortcut.key) == GLFW_PRESS);
				CHECK([view performKeyEquivalent:press] == YES);
				CHECK(keys[shortcut.key] == GLFW_PRESS);
				[view keyUp:release];
				CHECK(keys[shortcut.key] == GLFW_RELEASE);
				CHECK(glfwGetKey(window, shortcut.key) == GLFW_RELEASE);
				CHECK(glfwGetKeysSiv3D(window) == keys);
			}
		}
	}

	TEST_CASE("GLFW.Siv3DWindowMoveNotifications")
	{
		GLFWwindow* window = EngineWindow();
		NSWindow* nativeWindow = glfwGetCocoaWindow(window);
		Array<Point> notifications;
		void* previousUserPointer = glfwGetWindowUserPointer(window);
		const GLFWwindowposfun previousCallback = glfwSetWindowPosCallback(window,
			[](GLFWwindow* sender, int x, int y)
			{
				static_cast<Array<Point>*>(glfwGetWindowUserPointer(sender))->emplace_back(x, y);
			});
		const ScopeExit restore{ [=]
		{
			glfwSetWindowPosCallback(window, previousCallback);
			glfwSetWindowUserPointer(window, previousUserPointer);
		} };
		glfwSetWindowUserPointer(window, &notifications);
		[nativeWindow.delegate windowWillMove:
			[NSNotification notificationWithName:NSWindowWillMoveNotification object:nativeWindow]];
		[nativeWindow.delegate windowDidMove:
			[NSNotification notificationWithName:NSWindowDidMoveNotification object:nativeWindow]];
		CHECK(notifications == Array<Point>{ Point{ 1, 0 }, Point{ 0, 0 } });
	}

	TEST_CASE("GLFW.Siv3DMonitorAndWindowCoordinates")
	{
		GLFWwindow* window = EngineWindow();
		NSWindow* nativeWindow = glfwGetCocoaWindow(window);
		int count = 0;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		REQUIRE(monitors != nullptr);
		REQUIRE(count > 0);
		bool foundCurrentMonitor = false;
		for (int i = 0; i < count; ++i)
		{
			uint32_t displayID = 0, unitNumber = 0;
			int x = 0, y = 0, width = 0, height = 0, workWidth = 0, workHeight = 0;
			glfwGetMonitorInfo_Siv3D(monitors[i], &displayID, &unitNumber,
				&x, &y, &width, &height, nullptr, nullptr, &workWidth, &workHeight);
			CHECK(displayID == glfwGetCocoaMonitor(monitors[i]));
			CHECK(unitNumber == CGDisplayUnitNumber(displayID));
			const CGRect bounds = CGDisplayBounds(displayID);
			CHECK(Rect(x, y, width, height) == Rect(bounds.origin.x, bounds.origin.y,
				bounds.size.width, bounds.size.height));
			CHECK(workWidth > 0);
			CHECK(workHeight > 0);
			int rectX = 0, rectY = 0, rectWidth = 0, rectHeight = 0;
			glfwGetMonitorRect_Siv3D(monitors[i], &rectX, &rectY, &rectWidth, &rectHeight);
			CHECK(Rect(rectX, rectY, rectWidth, rectHeight) == Rect(x, y, width, height));
			glfwGetMonitorInfo_Siv3D(monitors[i], nullptr, nullptr,
				nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
			const uint32_t currentDisplay = [nativeWindow.screen.deviceDescription[@"NSScreenNumber"] unsignedIntValue];
			const bool isCurrent = (glfwIsCurrentMonitor_Siv3D(window, monitors[i]) != 0);
			CHECK(isCurrent == (displayID == currentDisplay));
			foundCurrentMonitor |= isCurrent;
		}
		CHECK(foundCurrentMonitor);

		// Siv3D uses the outer frame in backing pixels, including the title bar.
		REQUIRE(nativeWindow.screen != nil);
		const NSRect frame = [nativeWindow.screen convertRectToBacking:nativeWindow.frame];
		const double top = NSScreen.screens[0].frame.size.height * nativeWindow.backingScaleFactor;
		int x = 0, y = 0;
		glfwGetWindowPos(window, &x, &y);
		CHECK(x == static_cast<int>(std::round(frame.origin.x)));
		CHECK(y == static_cast<int>(std::round(top - NSMaxY(frame))));
		CHECK(glfwGetWindowAttrib(window, GLFW_CLIENT_API) == GLFW_NO_API);
		CHECK([nativeWindow.contentView.layer isKindOfClass:[CAMetalLayer class]]);
	}
}

# endif
