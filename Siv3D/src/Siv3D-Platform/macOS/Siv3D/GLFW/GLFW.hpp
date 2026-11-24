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
# define GLFW_INCLUDE_NONE
# define GLFW_EXPOSE_NATIVE_COCOA
# include <ThirdParty/GLFW/glfw3.h>
# include <ThirdParty/GLFW/glfw3native.h>

extern "C"
{
	// Enable on a windowed engine window before its first fullscreen transition.
	void glfwPreserveVideoMode_Siv3D(GLFWwindow* window);

	// Main-thread controls for the engine-owned fullscreen UI.
	void glfwSetFullscreenToggleEnabled_Siv3D(GLFWwindow* window, int enabled);
	int glfwIsFullscreenToggleEnabled_Siv3D(GLFWwindow* window);
	int glfwTakeFullscreenToggleRequest_Siv3D(GLFWwindow* window);
}
