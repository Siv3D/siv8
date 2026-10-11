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

# pragma once
# include <Siv3D/WindowState.hpp>

struct GLFWwindow;

namespace s3d::WindowMisc
{
	// Desktop geometry is in points; the framebuffer remains in backing pixels.
	void UpdateGeometry(GLFWwindow* glfwWindow, WindowState& state);

	void SetPos(GLFWwindow* glfwWindow, Point pos);
}
