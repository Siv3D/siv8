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

# include "CWindow.hpp"
# include <Siv3D/UserAction.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Window.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include "WindowMisc.hpp"

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

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CWindow::~CWindow()
	{
		LOG_SCOPED_DEBUG("CWindow::~CWindow()");

		::glfwTerminate();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

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
		
		::glfwPreserveVideoMode_Siv3D(m_glfwWindow);
		::glfwSetFullscreenToggleEnabled_Siv3D(m_glfwWindow, GLFW_TRUE);
		::glfwSetWindowUserPointer(m_glfwWindow, this);
		::glfwSetWindowPosCallback(m_glfwWindow, CWindow::OnMove);
		::glfwSetWindowSizeCallback(m_glfwWindow, CWindow::OnResize);
		::glfwSetFramebufferSizeCallback(m_glfwWindow, CWindow::OnFrameBufferSize);
		::glfwSetWindowContentScaleCallback(m_glfwWindow, CWindow::OnScalingChange);
		::glfwSetWindowIconifyCallback(m_glfwWindow, CWindow::OnIconify);
		::glfwSetWindowMaximizeCallback(m_glfwWindow, CWindow::OnMaximize);
		::glfwSetWindowFocusCallback(m_glfwWindow, CWindow::OnFocus);

		updateState();
		updateMinimumWindowSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CWindow::update()
	{
		::glfwPollEvents();
		
		updateState();

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			m_windowTitle.refresh(m_glfwWindow);
		}

		if (::glfwTakeFullscreenToggleRequest_Siv3D(m_glfwWindow))
		{
			Window::SetFullscreen(not m_state.fullscreen, System::GetCurrentMonitorIndex());
		}
		
		if (::glfwWindowShouldClose(m_glfwWindow))
		{
			SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::CloseButtonClicked);
			::glfwSetWindowShouldClose(m_glfwWindow, GLFW_FALSE);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setWindowTitle
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setWindowTitle(const String& title)
	{
		m_windowTitle.set(m_glfwWindow, title);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getWindowTitle
	//
	////////////////////////////////////////////////////////////////

	const String& CWindow::getWindowTitle() const noexcept
	{
		return m_windowTitle.title;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getHandle
	//
	////////////////////////////////////////////////////////////////

	void* CWindow::getHandle() const noexcept
	{
		return m_glfwWindow;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getState
	//
	////////////////////////////////////////////////////////////////

	const WindowState& CWindow::getState() const noexcept
	{
		return m_state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setStyle
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setStyle(const WindowStyle style)
	{
		LOG_DEBUG(fmt::format("CWindow::setStyle(style = {})", FromEnum(style)));
		
		if (m_state.fullscreen)
		{
			LOG_FAIL("A window in fullscreen mode cannot change its style");
			return;
		}

		if (m_state.style == style)
		{
			return;
		}

		if (style == WindowStyle::Fixed)
		{
			::glfwSetWindowAttrib(m_glfwWindow, GLFW_RESIZABLE, GLFW_FALSE);
			::glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, GLFW_TRUE);
		}
		else if (style == WindowStyle::Sizable)
		{
			::glfwSetWindowAttrib(m_glfwWindow, GLFW_RESIZABLE, GLFW_TRUE);
			::glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, GLFW_TRUE);
		}
		else if (style == WindowStyle::Frameless)
		{
			::glfwSetWindowAttrib(m_glfwWindow, GLFW_RESIZABLE, GLFW_FALSE);
			::glfwSetWindowAttrib(m_glfwWindow, GLFW_DECORATED, GLFW_FALSE);
		}
		
		m_state.style = style;
		
		updateState();
		// GLFW defers size limits while the window is not resizable.
		updateMinimumWindowSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setPos(const Point pos)
	{
		LOG_DEBUG(fmt::format("CWindow::setPos(pos = {})", pos));

		if (m_state.fullscreen)
		{
			LOG_FAIL("A window in fullscreen mode cannot be moved");
			return;
		}
		
		WindowMisc::SetPos(m_glfwWindow, pos);
		
		updateState();
	}

	////////////////////////////////////////////////////////////////
	//
	//	show
	//
	////////////////////////////////////////////////////////////////

	void CWindow::show() {} // do nothing

	////////////////////////////////////////////////////////////////
	//
	//	maximize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::maximize()
	{
		LOG_DEBUG("CWindow::maximize()");

		if (m_state.style == WindowStyle::Fixed)
		{
			LOG_FAIL("A window with WindowStyle::Fixed cannot be maximized");
			return;
		}
		
		::glfwMaximizeWindow(m_glfwWindow);
		
		updateState();
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	restore
	//
	////////////////////////////////////////////////////////////////

	void CWindow::restore()
	{
		LOG_DEBUG("CWindow::restore()");
		
		::glfwRestoreWindow(m_glfwWindow);
		
		updateState();
	}

	////////////////////////////////////////////////////////////////
	//
	//	minimize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::minimize()
	{
		LOG_DEBUG("CWindow::minimize()");
		
		::glfwIconifyWindow(m_glfwWindow);
		
		updateState();
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeByVirtualSize
	//
	////////////////////////////////////////////////////////////////

	bool CWindow::resizeByVirtualSize(const Size virtualSize)
	{
		LOG_DEBUG(fmt::format("CWindow::resizeByVirtualSize(size = {})", virtualSize));

		if (m_state.virtualSize == virtualSize)
		{
			return true;
		}

		if (m_state.fullscreen)
		{
			setFullscreen(false, System::GetCurrentMonitorIndex());
		}

		::glfwSetWindowSize(m_glfwWindow, virtualSize.x, virtualSize.y);

		updateState();
		
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeByFrameBufferSize
	//
	////////////////////////////////////////////////////////////////

	bool CWindow::resizeByFrameBufferSize(const Size frameBufferSize)
	{
		LOG_DEBUG(fmt::format("CWindow::resizeByFrameBufferSize(size = {})", frameBufferSize));
		
		if (m_state.frameBufferSize == frameBufferSize)
		{
			return true;
		}

		if (m_state.fullscreen)
		{
			// Convert using the restored window's screen, which can have another DPI.
			setFullscreen(false, System::GetCurrentMonitorIndex());
		}

		const double scaling = m_state.scaling;
		const Size newVirtualSize = Math::Round(frameBufferSize / scaling).asPoint();	
		return resizeByVirtualSize(newVirtualSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMinimumFrameBufferSize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setMinimumFrameBufferSize(const Size size)
	{
		LOG_DEBUG(fmt::format("CWindow::setMinimumFrameBufferSize(size = {})", size));
		
		updateState();
		m_state.minFrameBufferSize = size;
		updateMinimumWindowSize();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setFullscreen
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setFullscreen(const bool fullscreen, size_t monitorIndex)
	{
		LOG_DEBUG(fmt::format("CWindow::setFullscreen(fullscreen = {}, monitorIndex = {})", fullscreen, monitorIndex));

		updateState();
		if ((not fullscreen) && (not m_state.fullscreen))
		{
			return;
		}
		
		if (fullscreen)
		{
			int32 numMonitors = 0;
			GLFWmonitor** monitors = ::glfwGetMonitors(&numMonitors);
			if ((not monitors) || (numMonitors <= 0))
			{
				return;
			}

			if (static_cast<size_t>(numMonitors) <= monitorIndex)
			{
				// In fullscreen the fallback is the monitor already attached to this window.
				if (m_state.fullscreen) { return; }
				monitorIndex = System::GetCurrentMonitorIndex();
			}
			if (static_cast<size_t>(numMonitors) <= monitorIndex)
			{
				return;
			}
			GLFWmonitor* target = monitors[monitorIndex];
			if (target == ::glfwGetWindowMonitor(m_glfwWindow)) { return; }
			const GLFWvidmode* mode = ::glfwGetVideoMode(target);
			if (not mode)
			{
				return;
			}

			if (not m_state.fullscreen)
			{
				// Save only on entry; monitor changes retain the original windowed geometry.
				m_storedWindowRect.pos = (m_state.bounds.pos + Point{ 0, m_state.titleBarHeight });
				m_storedWindowRect.size = m_state.virtualSize;
			}
			
			// The Cocoa window policy bypasses video-mode selection, including modes
			// absent from GLFW's enumeration. These dimensions are desktop points.
			::glfwSetWindowMonitor(m_glfwWindow, target, 0, 0,
				mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			::glfwSetWindowMonitor(m_glfwWindow, nullptr, m_storedWindowRect.x, m_storedWindowRect.y,
								   m_storedWindowRect.w, m_storedWindowRect.h, GLFW_DONT_CARE);
		}
		
		updateState();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setToggleFullscreenEnabled
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setToggleFullscreenEnabled(const bool enabled)
	{
		::glfwSetFullscreenToggleEnabled_Siv3D(m_glfwWindow, enabled);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isToggleFullscreenEnabled
	//
	////////////////////////////////////////////////////////////////

	bool CWindow::isToggleFullscreenEnabled() const
	{
		return ::glfwIsFullscreenToggleEnabled_Siv3D(m_glfwWindow);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTaskbarProgressBar
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setTaskbarProgressBar(const double progress0_1)
	{
		m_dockProgress.setProgress(progress0_1);
	}

	////////////////////////////////////////////////////////////////
	//
	//	updateState
	//
	////////////////////////////////////////////////////////////////

	void CWindow::updateState()
	{
		const double previousScaling = m_state.scaling;
		WindowMisc::UpdateGeometry(m_glfwWindow, m_state);
		if (m_state.scaling != previousScaling)
		{
			updateMinimumWindowSize();
		}

		// GLFW also leaves fullscreen when its monitor is disconnected.
		m_state.fullscreen = (::glfwGetWindowMonitor(m_glfwWindow) != nullptr);

		// minimized
		m_state.minimized = (::glfwGetWindowAttrib(m_glfwWindow, GLFW_ICONIFIED) == GLFW_TRUE);
		
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

	////////////////////////////////////////////////////////////////
	//
	//	updateMinimumWindowSize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::updateMinimumWindowSize()
	{
		const auto toPoints = [scaling = m_state.scaling](const int32 pixels)
		{
			// Preserve GLFW's non-positive limit values; round positive minima up.
			return (0 < pixels) ? static_cast<int32>(Math::Ceil(pixels / scaling)) : pixels;
		};
		::glfwSetWindowSizeLimits(m_glfwWindow, toPoints(m_state.minFrameBufferSize.x),
			toPoints(m_state.minFrameBufferSize.y), GLFW_DONT_CARE, GLFW_DONT_CARE);
	}

	////////////////////////////////////////////////////////////////
	//
	//	OnMove
	//
	////////////////////////////////////////////////////////////////

	void CWindow::OnMove(GLFWwindow* glfwWindow, const int flag, const int)
	{
		const bool move = (flag == 1);
		LOG_DEBUG(fmt::format("CWindow::OnMove({})", move));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.sizeMove = true;
			pWindow->m_moveResizeStabilizer.restart();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	OnResize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::OnResize(GLFWwindow* glfwWindow, const int width, const int height)
	{
		const Size size{ width, height };
		LOG_DEBUG(fmt::format("CWindow::OnResize({})", size));
		
		if (CWindow* pWindow = GetWindow(glfwWindow))
		{
			pWindow->m_state.virtualSize = size;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	OnFrameBufferSize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::OnFrameBufferSize(GLFWwindow*, const int width, const int height)
	{
		const Size size{ width, height };
		LOG_DEBUG(fmt::format("CWindow::OnFrameBufferSize({})", size));
	}

	////////////////////////////////////////////////////////////////
	//
	//	OnScalingChange
	//
	////////////////////////////////////////////////////////////////

	void CWindow::OnScalingChange(GLFWwindow*, const float sx, const float sy)
	{
		LOG_DEBUG(fmt::format("CWindow::OnScalingChange({}, {})", sx, sy));
	}

	////////////////////////////////////////////////////////////////
	//
	//	OnIconify
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	OnMaximize
	//
	////////////////////////////////////////////////////////////////

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

	////////////////////////////////////////////////////////////////
	//
	//	OnFocus
	//
	////////////////////////////////////////////////////////////////

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
