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

# include "CWindow.hpp"
# include <Siv3D/UserAction.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	CWindow::~CWindow()
	{
		LOG_SCOPED_DEBUG("CWindow::~CWindow()");

		::glfwTerminate();
	}

	void CWindow::init()
	{
		LOG_SCOPED_DEBUG("CWindow::init()");
		
		::glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
		
		if (not ::glfwInit())
		{
			throw InternalEngineError{ "glfwInit() failed" };
		}
		
		{
			// OpenGL 4.1
			::glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			::glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			::glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
			::glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			::glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, SIV3D_BUILD(DEBUG) ? GLFW_TRUE : GLFW_FALSE);
		}

		::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		// ウィンドウの作成
		{
			m_window = ::glfwCreateWindow(800, 600,
										  "Siv3D App", nullptr, nullptr);
			
			if (not m_window)
			{
				throw InternalEngineError{ "glfwCreateWindow() failed" };
			}
		}
	}

	void CWindow::update()
	{
		::glfwSwapBuffers(m_window);
		::glfwPollEvents();
		
		//updateState();

		if (::glfwWindowShouldClose(m_window))
		{
			SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::CloseButtonClicked);
			::glfwSetWindowShouldClose(m_window, GLFW_FALSE);
		}
	}

	void CWindow::setWindowTitle(const String& title)
	{

	}

	const String& CWindow::getWindowTitle() const noexcept
	{
		return m_windowTitle;
	}

	void* CWindow::getHandle() const noexcept
	{
		return m_window;
	}
}
