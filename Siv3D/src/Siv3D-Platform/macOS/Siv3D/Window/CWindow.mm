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
	namespace
	{
		[[nodiscard]]
		CWindow* GetWindow(GLFWwindow* glfwWindow)
		{
			return static_cast<CWindow*>(::glfwGetWindowUserPointer(glfwWindow));
		}
	}

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

		::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		
		// ウィンドウの作成
		{
			m_glfwWindow = ::glfwCreateWindow(800, 600, Unicode::ToUTF8(m_windowTitle.actual).c_str(), nullptr, nullptr);
			
			if (not m_glfwWindow)
			{
				throw InternalEngineError{ "glfwCreateWindow() failed" };
			}
		}
		
		::glfwSetWindowUserPointer(m_glfwWindow, this);
		::glfwSetWindowSizeLimits(m_glfwWindow, m_state.minFrameBufferSize.x, m_state.minFrameBufferSize.y,
								  GLFW_DONT_CARE, GLFW_DONT_CARE);
		::glfwSetWindowPosCallback(m_glfwWindow, CWindow::OnMove);
		::glfwSetWindowSizeCallback(m_glfwWindow, CWindow::OnResize);
		::glfwSetFramebufferSizeCallback(m_glfwWindow, CWindow::OnFrameBufferSize);
		::glfwSetWindowContentScaleCallback(m_glfwWindow, CWindow::OnScalingChange);
		::glfwSetWindowIconifyCallback(m_glfwWindow, CWindow::OnIconify);
		::glfwSetWindowMaximizeCallback(m_glfwWindow, CWindow::OnMaximize);
		::glfwSetWindowFocusCallback(m_glfwWindow, CWindow::OnFocus);

		updateState();
	}

	void CWindow::update()
	{
		::glfwPollEvents();
		
		updateState();

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			m_windowTitle.refresh(m_glfwWindow);
		}
		
		if (::glfwWindowShouldClose(m_glfwWindow))
		{
			SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::CloseButtonClicked);
			::glfwSetWindowShouldClose(m_glfwWindow, GLFW_FALSE);
		}
	}

	void CWindow::setWindowTitle(const String& title)
	{
		m_windowTitle.set(m_glfwWindow, title);
	}

	const String& CWindow::getWindowTitle() const noexcept
	{
		return m_windowTitle.title;
	}

	void* CWindow::getHandle() const noexcept
	{
		return m_glfwWindow;
	}

	const WindowState& CWindow::getState() const noexcept
	{
		return m_state;
	}









	void CWindow::updateState()
	{
		// frameBufferSize
		::glfwGetFramebufferSize(m_glfwWindow, &m_state.frameBufferSize.x, &m_state.frameBufferSize.y);
		
		// scaling
		float xScale, yScale;
		::glfwGetWindowContentScale(m_glfwWindow, &xScale, &yScale);
		m_state.scaling = Max(xScale, yScale);
		
		// titleBarHeight
		//Siv3D_MacOS_UpdateWindowState(m_glfwWindow, m_state);
		//LOG_TEST(U"title bar: {}"_fmt(m_state.titleBarHeight));
		
		// bounds
		int32 windowPosX, windowPosY, windowSizeX, windowSizeY;
		::glfwGetWindowPos(m_glfwWindow, &windowPosX, &windowPosY);
		::glfwGetWindowSize(m_glfwWindow, &windowSizeX, &windowSizeY);
		
		m_state.bounds.pos.set(windowPosX, (windowPosY - m_state.titleBarHeight));
		m_state.bounds.size.set(windowSizeX, (windowSizeY + m_state.titleBarHeight));
				
		// minimized
		m_state.maximized = (::glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED) == GLFW_TRUE);
		
		// maximized
		m_state.maximized = (::glfwGetWindowAttrib(m_glfwWindow, GLFW_MAXIMIZED) == GLFW_TRUE)
			&& (m_state.style == WindowStyle::Sizable);
		
		// focused
		m_state.focused = (::glfwGetWindowAttrib(m_glfwWindow, GLFW_FOCUSED) == GLFW_TRUE);

		// sizeMove
		if (m_state.sizeMove)
		{
			if (SecondsF{ 0.2 } < m_moveResizeStabilizer)
			{
				m_state.sizeMove = false;
				m_moveResizeStabilizer.reset();
			}
		}
	}

	void CWindow::OnMove(GLFWwindow* glfwWindow, const int flag, const int)
	{
		const bool move = (flag == 1);
		LOG_DEBUG(U"CWindow::OnMove({})"_fmt(move));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.sizeMove = true;
			pWindow->m_moveResizeStabilizer.restart();
		}
	}

	void CWindow::OnResize(GLFWwindow* glfwWindow, const int width, const int height)
	{
		const Size size{ width, height };
		LOG_DEBUG(U"CWindow::OnResize({})"_fmt(size));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.bounds.size = size;
			pWindow->m_state.virtualSize = size;
		}
	}

	void CWindow::OnFrameBufferSize(GLFWwindow* glfwWindow, const int width, const int height)
	{
		const Size size{ width, height };
		LOG_DEBUG(U"CWindow::OnFrameBufferSize({})"_fmt(size));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.frameBufferSize = size;
			//pWindow->m_state.clientSize = size;
		}
	}

	void CWindow::OnScalingChange(GLFWwindow* glfwWindow, const float sx, const float sy)
	{
		LOG_DEBUG(fmt::format("CWindow::OnScalingChange({}, {})", sx, sy));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.scaling = Max(sx, sy);
		}
	}

	void CWindow::OnIconify(GLFWwindow* glfwWindow, const int iconified)
	{
		LOG_DEBUG(fmt::format("CWindow::OnIconify({})", (iconified == GLFW_TRUE)));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			if (iconified) // 最小化
			{
				pWindow->m_state.minimized = true;
				pWindow->m_state.maximized = false;
			}
			else // 最小化解除
			{
				pWindow->m_state.minimized = false;
				pWindow->m_state.maximized = false;
			}
		}
	}

	void CWindow::OnMaximize(GLFWwindow* glfwWindow, const int maximized)
	{
		LOG_DEBUG(fmt::format("CWindow::OnMaximize({})", (maximized == GLFW_TRUE)));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			if (maximized) // 最大化
			{
				pWindow->m_state.minimized = false;
				pWindow->m_state.maximized = true;
			}
			else // 最大化解除
			{
				pWindow->m_state.minimized = false;
				pWindow->m_state.maximized = false;
			}
		}
	}

	void CWindow::OnFocus(GLFWwindow* glfwWindow, const int focused)
	{
		const bool hasFocus = (focused == GLFW_TRUE);
		LOG_DEBUG(fmt::format("CWindow::OnFocus({})", hasFocus));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.focused = hasFocus;
			
			if (not hasFocus) // フォーカスが外れた場合
			{
				SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::WindowDeactivated);
			}
		}
	}
}
