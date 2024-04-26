//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
		NSWindow* window = ::glfwGetCocoaWindow(glfwWindow);
		CGFloat contentHeight = [window contentRectForFrameRect: window.frame].size.height;
		return (window.frame.size.height - contentHeight);
	}
}
