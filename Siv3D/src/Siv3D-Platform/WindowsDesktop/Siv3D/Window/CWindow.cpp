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
# include "WindowMisc.hpp"
# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/UserAction.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include <dwmapi.h>

namespace s3d
{
	extern std::atomic_flag g_shouldDestroyWindow;

	namespace
	{
		[[nodiscard]]
		static constexpr Rect ToRect(const RECT& rect) noexcept
		{
			return{ rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	~CWindow
	//
	////////////////////////////////////////////////////////////////

	CWindow::~CWindow()
	{
		LOG_SCOPED_DEBUG("CWindow::~CWindow()");

		if (m_hWnd)
		{
			// DestroyWindow() must be called from the main thread.
			g_shouldDestroyWindow.test_and_set();

			for (int32 i = 0; i < 100; ++i)
			{
				if (g_shouldDestroyWindow.test() == false)
				{
					break;
				}

				::Sleep(2);
			}
		}

		m_user32.unload();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CWindow::init()
	{
		LOG_SCOPED_DEBUG("CWindow::init()");

		// モジュールハンドルを取得する
		m_moduleHandle = ::GetModuleHandleW(nullptr);

		// User32.dll の関数をロードする
		m_user32.load();

		// DPI awareness を有効化する
		WindowMisc::SetDPIAwareness(m_user32.library);

		// ウィンドウクラスを登録する
		m_windowClass.registerClass(m_moduleHandle);

		// プライマリモニターにウィンドウを作成する
		for (const auto& monitor : WindowMisc::GetMonitors())
		{
			if (monitor.isPrimary)
			{
				// ウィンドウを作成する
				m_hWnd = WindowMisc::CreateMainWindow(
					m_moduleHandle,
					monitor,
					m_windowClass.name,
					m_windowTitle.actual,
					m_user32.pAdjustWindowRectExForDpi,
					m_dpi,
					m_state);

				break;
			}
		}

		// ウィンドウの作成に失敗したらエラー
		if (not m_hWnd)
		{
			throw InternalEngineError{ "CreateWindowExW() failed" };
		}

		// タッチ操作時の palm rejection を無効化し、タッチメッセージの取得を高速化する。
		WindowMisc::DisablePalmRejection(m_hWnd);

		// タッチ操作時の視覚フィードバック（フレームレートを低下させることがある）を無効化する
		WindowMisc::DisableTouchFeedbackVisualization(m_hWnd, m_user32.pSetWindowFeedbackSetting);

		// DBT_DEVICEARRIVAL, DBT_DEVICEREMOVECOMPLETE が送られるようにする
		m_deviceNotificationHandle = WindowMisc::StartDeviceNotification(m_hWnd);

		// タスクバーリストを作成する
		m_taskbarList = WindowMisc::CreateTaskbarList();
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CWindow::update()
	{
		m_state.sizeMove = m_moving.load();

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			m_windowTitle.refresh(m_hWnd);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setWindowTitle
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setWindowTitle(const String& title)
	{
		m_windowTitle.set(m_hWnd, title);
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
		return m_hWnd;
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
		LOG_SCOPED_DEBUG(fmt::format("CWindow::setStyle(style = {})", FromEnum(style)));

		if (m_state.fullscreen)
		{
			LOG_FAIL("A window in fullscreen mode cannot change its style");
			return;
		}

		if (m_state.style == style)
		{
			return;
		}

		if (not m_windowShown)
		{
			show();
		}

		const auto currentStyle = m_state.style;
		const bool triggerWindowResize = ((currentStyle == WindowStyle::Frameless) || (style == WindowStyle::Frameless));
		const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(style);
		m_state.style = style;

		::SetWindowLongPtrW(m_hWnd, GWL_STYLE, windowStyleFlags);

		{
			const Point pos = m_state.bounds.pos;
			const double scaling = WindowMisc::GetScaling(m_dpi);
			const Size newFrameBufferSize = Math::Round(m_state.virtualSize * scaling).asPoint();
			const Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
				pos, newFrameBufferSize, windowStyleFlags);
			const uint32 flags = ((triggerWindowResize ? 0 : SWP_NOSIZE) | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			setWindowPos(windowRect, flags);
		}

		onBoundsUpdate();
		show();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setPos(const Point pos)
	{
		LOG_SCOPED_DEBUG(fmt::format("CWindow::setPos(pos = {})", pos));

		if (m_state.fullscreen)
		{
			LOG_FAIL("A window in fullscreen mode cannot be moved");
			return;
		}

		{
			const double scaling = WindowMisc::GetScaling(m_dpi);
			const Size newFrameBufferSize = Math::Round(m_state.virtualSize * scaling).asPoint();
			const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(m_state.style);
			const Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
				pos, newFrameBufferSize, windowStyleFlags);
			const uint32 flags = (SWP_DEFERERASE | SWP_NOOWNERZORDER | SWP_NOZORDER);

			setWindowPos(windowRect, flags);
		}
	}

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

		::ShowWindow(m_hWnd, SW_MAXIMIZE);
	}

	////////////////////////////////////////////////////////////////
	//
	//	restore
	//
	////////////////////////////////////////////////////////////////

	void CWindow::restore()
	{
		LOG_DEBUG("CWindow::restore()");
		::ShowWindow(m_hWnd, SW_RESTORE);
	}

	////////////////////////////////////////////////////////////////
	//
	//	minimize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::minimize()
	{
		LOG_DEBUG("CWindow::minimize()");
		::ShowWindow(m_hWnd, SW_MINIMIZE);
	}

	////////////////////////////////////////////////////////////////
	//
	//	show
	//
	////////////////////////////////////////////////////////////////

	void CWindow::show()
	{
		WindowMisc::ShowWindow(m_hWnd);

		m_windowShown = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	destroy
	//
	////////////////////////////////////////////////////////////////

	void CWindow::destroy()
	{
		LOG_SCOPED_DEBUG("CWindow::destroy()");

		if (m_hWnd)
		{
			m_taskbarList.Reset();

			if (m_deviceNotificationHandle)
			{
				LOG_DEBUG("UnregisterDeviceNotification()");
				[[maybe_unused]] const BOOL b = ::UnregisterDeviceNotification(m_deviceNotificationHandle);
				m_deviceNotificationHandle = nullptr;
				LOG_DEBUG(fmt::format("UnregisterDeviceNotification() -> {}", static_cast<bool>(b)));				
			}

			LOG_DEBUG("DestroyWindow()");		
			[[maybe_unused]] const BOOL b = ::DestroyWindow(m_hWnd);
			LOG_DEBUG(fmt::format("DestroyWindow() -> {}", static_cast<bool>(b)));
		}

		m_windowClass.unregisterClass(m_moduleHandle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	onResize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onResize(const bool minimized, const bool maximized)
	{
		LOG_DEBUG(fmt::format("CWindow::onResize(minimized = {}, maximized = {})", minimized, maximized));

		if (minimized)
		{
			m_state.minimized = true;
			m_state.maximized = false;
		}
		else if (maximized)
		{
			m_state.minimized = false;
			m_state.maximized = true;
		}
		else
		{
			m_state.minimized = false;
			m_state.maximized = false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onFocus
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onFocus(const bool focused)
	{
		LOG_DEBUG(fmt::format("CWindow::onFocus(focused = {})", focused));

		if (not focused)
		{
			SIV3D_ENGINE(UserAction)->reportUserActions(UserAction::WindowDeactivated);
		}

		m_state.focused = focused;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onFrameBufferResize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onFrameBufferResize(const Size size)
	{
		LOG_DEBUG(fmt::format("CWindow::onFrameBufferResize(size = {})", size));

		if (size.isZero())
		{
			// window minimized
		}
		else
		{
			m_state.frameBufferSize = size;
			m_state.virtualSize = Math::Round(m_state.frameBufferSize / m_state.scaling).asPoint();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	onBoundsUpdate
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onBoundsUpdate()
	{
		LOG_TRACE("CWindow::onBoundsUpdate()");

		// bounds
		{
			RECT windowRect;
			::DwmGetWindowAttribute(m_hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect, sizeof(RECT));
			m_state.bounds = ToRect(windowRect);
		}

		// frame thickness
		{
			m_state.frameThickness.set(getSystemMetrics(SM_CXBORDER), getSystemMetrics(SM_CYBORDER));
		}

		// title bar height
		{
			m_state.titleBarHeight = (getSystemMetrics(SM_CYCAPTION)
				+ getSystemMetrics(SM_CYFRAME)
				+ getSystemMetrics(SM_CXPADDEDBORDER));
		}

		// border
		{
			const DWORD windowStyleFlags = static_cast<DWORD>(::GetWindowLongPtrW(m_hWnd, GWL_STYLE));
			const Rect placeholderWindowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi, Point{ 0, 0 }, Size{ 0, 0 }, windowStyleFlags);
			m_border = placeholderWindowRect.size;
		}

		LOG_TRACE(U"- bounds: {}, frameThickness: {}, titleBarHeight: {}, border: {}"_fmt(
			m_state.bounds, m_state.frameThickness, m_state.titleBarHeight, m_border));
	}

	////////////////////////////////////////////////////////////////
	//
	//	onDPIChange
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onDPIChange(const uint32 dpi, const Point suggestedPos)
	{
		const double scaling = WindowMisc::GetScaling(dpi);
		
		LOG_SCOPED_DEBUG("CWindow::onDPIChange()");
		LOG_TRACE(fmt::format("- dpi = {}({:.0f}%), suggestedPos = {}", dpi, (scaling * 100), suggestedPos));

		m_dpi = dpi;
		m_state.scaling = scaling;

		onBoundsUpdate();

		const Size newFrameBufferSize = (m_state.virtualSize * scaling).asPoint();
		const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(m_state.style);
		Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
			suggestedPos, newFrameBufferSize, windowStyleFlags);

		if (m_state.style != WindowStyle::Frameless)
		{
			windowRect.y = (suggestedPos.y - m_state.titleBarHeight);
		}

		setWindowPos(windowRect, (SWP_NOACTIVATE | SWP_NOZORDER));
	}

	////////////////////////////////////////////////////////////////
	//
	//	onEnterSizeMove
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onEnterSizeMove()
	{
		m_moving.store(true);
	}

	////////////////////////////////////////////////////////////////
	//
	//	onExitSizeMove
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onExitSizeMove()
	{
		m_moving.store(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMinTrackSize
	//
	////////////////////////////////////////////////////////////////

	Size CWindow::getMinTrackSize() const noexcept
	{
		return (m_state.minFrameBufferSize + m_border);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSystemMetrics
	//
	////////////////////////////////////////////////////////////////

	int32 CWindow::getSystemMetrics(const int32 index) const
	{
		if (m_user32.pGetSystemMetricsForDpi)
		{
			return m_user32.pGetSystemMetricsForDpi(index, m_dpi);
		}
		else
		{
			return ::GetSystemMetrics(index);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	setWindowPos
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setWindowPos(const Rect& rect, const uint32 flags)
	{
		Point pos = rect.pos;
	
		if (m_state.style != WindowStyle::Frameless)
		{
			pos += Size{ m_state.frameThickness.x, m_state.titleBarHeight };
		}

		const Size size = rect.size;

		LOG_DEBUG(fmt::format("SetWindowPos({}, {}, {}, {}, {:#x})", pos.x, pos.y, size.x, size.y, flags));
		::SetWindowPos(m_hWnd, HWND_TOP, pos.x, pos.y, size.x, size.y, flags);
	}
}
