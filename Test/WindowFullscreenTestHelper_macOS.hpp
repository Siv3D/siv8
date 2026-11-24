//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(MACOS)
# include "WindowFullscreenTestHelper.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Window/IWindow.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d::Test::Fullscreen
{
	inline GLFWwindow* MainWindow()
	{
		return static_cast<GLFWwindow*>(SIV3D_ENGINE(Window)->getHandle());
	}

	inline NSMenuItem* FullscreenMenuItem(NSMenu* menu)
	{
		for (NSMenuItem* item in menu.itemArray)
		{
			if (item.action == NSSelectorFromString(@"toggleFullscreen_Siv3D:")) { return item; }
			if (NSMenuItem* found = FullscreenMenuItem(item.submenu)) { return found; }
		}
		return nil;
	}

	inline NSEvent* FullscreenKeyEvent(const bool repeat)
	{
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		return [NSEvent keyEventWithType:NSEventTypeKeyDown location:NSZeroPoint
			modifierFlags:(NSEventModifierFlagControl | NSEventModifierFlagCommand) timestamp:0
			windowNumber:native.windowNumber context:nil characters:@"f"
			charactersIgnoringModifiers:@"f" isARepeat:repeat keyCode:0x03];
	}

	inline void FullscreenShortcut(const bool repeat = false)
	{
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		NSEvent* event = FullscreenKeyEvent(repeat);
		REQUIRE([native performKeyEquivalent:event]);
	}

	enum class FullscreenRoute { Shortcut, Menu, API };

	inline void RequestFullscreen(const FullscreenRoute route)
	{
		NSWindow* native = glfwGetCocoaWindow(MainWindow());
		if (route == FullscreenRoute::API)
		{
			Test::SetFullscreenWithPause(not Window::GetState().fullscreen);
			return;
		}
		const bool previous = Window::GetState().fullscreen;
		if (route == FullscreenRoute::Shortcut) { FullscreenShortcut(); }
		else if (route == FullscreenRoute::Menu)
		{
			NSMenuItem* item = FullscreenMenuItem(NSApp.mainMenu);
			REQUIRE(item != nil);
			[item.menu update];
			REQUIRE(item.enabled);
			REQUIRE([NSApp targetForAction:item.action to:item.target from:item] == native);
			[item.menu performActionForItemAtIndex:[item.menu indexOfItem:item]];
		}
		CHECK(Window::GetState().fullscreen == previous);
		Test::UpdateWithPause();
	}

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

	inline Array<NativeDisplay> ReadNativeDisplays()
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

# endif
