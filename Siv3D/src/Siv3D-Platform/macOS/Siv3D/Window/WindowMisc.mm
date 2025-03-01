//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "WindowMisc.hpp"
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d::WindowMisc
{
	int32 GetTitleBarHeight(GLFWwindow* glfwWindow)
	{
		@autoreleasepool
		{
			NSWindow* window = ::glfwGetCocoaWindow(glfwWindow);
			const NSRect frameRect = [window frame];
			const NSRect retinaFrameRect = [window convertRectToBacking:frameRect];
			const NSRect contentRect = [window contentRectForFrameRect: frameRect];
			const NSRect retinaContentRect = [window convertRectToBacking: contentRect];
			return (retinaFrameRect.size.height - retinaContentRect.size.height);
		}
	}
}
