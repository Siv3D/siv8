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

# include <Siv3D/Unicode.hpp>
# include "WindowTitle.hpp"
# include <Siv3D/Profiler/IProfiler.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	void WindowTitle::set(GLFWwindow* glfwWindow, const String& newTitle)
	{
		String newActualTitle = newTitle;

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			const String statistics = SIV3D_ENGINE(Profiler)->getSimpleStatistics();
			newActualTitle += U" (Debug Build) | ";
			newActualTitle += statistics;
		}

		if (actual != newActualTitle)
		{
			::glfwSetWindowTitle(glfwWindow, Unicode::ToUTF8(newActualTitle).c_str());
			actual.swap(newActualTitle);
		}

		title = newTitle;
	}

	void WindowTitle::refresh(GLFWwindow* glfwWindow)
	{
		set(glfwWindow, title);
	}
}
