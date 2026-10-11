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

# include "WindowMisc.hpp"
# include <Siv3D/Math.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d::WindowMisc
{
	void UpdateGeometry(GLFWwindow* glfwWindow, WindowState& state)
	{
		@autoreleasepool
		{
			NSWindow* window = ::glfwGetCocoaWindow(glfwWindow);
			const NSRect frameRect = [window frame];
			const NSRect contentRect = [window contentRectForFrameRect:frameRect];
			const NSSize clientSize = [::glfwGetCocoaView(glfwWindow) frame].size;
			const double desktopTop = [[[NSScreen screens] firstObject] frame].size.height;
			state.bounds = Rect{
				Math::Round(Vec2{ frameRect.origin.x, (desktopTop - NSMaxY(frameRect)) }).asPoint(),
				Math::Round(Vec2{ frameRect.size.width, frameRect.size.height }).asPoint() };
			state.virtualSize = Math::Round(Vec2{ clientSize.width, clientSize.height }).asPoint();
			state.titleBarHeight = static_cast<int32>(Math::Round(NSMaxY(frameRect) - NSMaxY(contentRect)));
			::glfwGetFramebufferSize(glfwWindow, &state.frameBufferSize.x, &state.frameBufferSize.y);
			state.scaling = [window backingScaleFactor];
		}
	}

	void SetPos(GLFWwindow* glfwWindow, const Point pos)
	{
		@autoreleasepool
		{
			NSWindow* window = ::glfwGetCocoaWindow(glfwWindow);
			const double desktopTop = [[[NSScreen screens] firstObject] frame].size.height;
			[window setFrameTopLeftPoint:NSMakePoint(pos.x, (desktopTop - pos.y))];
		}
	}
}
