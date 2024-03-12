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

# pragma once
# include <Siv3D/Window.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d
{
	struct WindowTitle
	{
		String title{ Window::DefaultTitle };

		String actual{ SIV3D_BUILD(DEBUG) ? U"Siv3D App (Debug Build)"_sv : Window::DefaultTitle };

		void set(GLFWwindow* glfwWindow, const String& newTitle);

		void refresh(GLFWwindow* glfwWindow);
	};
}
