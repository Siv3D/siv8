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
# include <Siv3D/ScopeExit.hpp>
# include "WindowMisc.hpp"
# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/Math.hpp>
# include <Siv3D/Scene.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/UserAction/IUserAction.hpp>
# include <Siv3D/Window.hpp>
# include <Siv3D/Console/IConsole.hpp>
# include <Siv3D/UserAction.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>
# include <algorithm>
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
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CWindow::~CWindow()
	{
		LOG_SCOPED_DEBUG("CWindow::~CWindow()");

		if (m_hWnd)
		{
			// DestroyWindow() must be called from the main thread.
			g_shouldDestroyWindow.test_and_set();

			g_shouldDestroyWindow.wait(true);
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

		beginStateUpdate();
		const ScopeExit finishInitialization{ [this]
		{
			endStateUpdate();
			// The engine is still waiting on windowInitialized.get().
			m_stateSync.synchronize();
		} };

		// モジュールハンドルを取得する
		m_moduleHandle = ::GetModuleHandleW(nullptr);

		// User32.dll の関数をロードする
		m_user32.load();

		// DPI awareness を有効化する
		WindowMisc::SetDPIAwareness(m_user32.library);

		// ウィンドウクラスを登録する
		m_windowClass.registerClass(m_moduleHandle);

		// プライマリモニターを優先し、見つからなければ先頭のモニターにウィンドウを作成する
		{
			// モニターが見つからない場合は GetMonitors() が例外を送出する
			const Array<MonitorInfo> monitors = WindowMisc::GetMonitors();
			const auto primaryMonitor = std::find_if(monitors.begin(), monitors.end(), [](const MonitorInfo& monitor)
			{
				return monitor.isPrimary;
			});
			const MonitorInfo& monitor = ((primaryMonitor != monitors.end()) ? *primaryMonitor : monitors.front());

			m_hWnd = WindowMisc::CreateMainWindow(
				m_moduleHandle,
				monitor,
				m_windowClass.name,
				m_windowTitle.actual,
				m_user32.pAdjustWindowRectExForDpi,
				m_dpi,
				m_osState, this);
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
		onBoundsUpdate();
	}

	////////////////////////////////////////////////////////////////
	//
	//	update
	//
	////////////////////////////////////////////////////////////////

	void CWindow::update()
	{
		m_stateSync.synchronize();
		if (m_toggleFullscreenState.load() == ToggleFullscreenState::Requested)
		{
			auto expected = ToggleFullscreenState::Requested;
			if (m_toggleFullscreenState.compare_exchange_strong(expected, ToggleFullscreenState::Enabled))
			{
				Window::SetFullscreen(not getState().fullscreen, System::GetCurrentMonitorIndex());
			}
		}
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
		return m_stateSync.get();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setStyle
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setStyle(const WindowStyle style)
	{
		invokeOnWindowThread([&] { setStyleOnWindowThread(style); });
	}

	void CWindow::setStyleOnWindowThread(const WindowStyle style)
	{
		LOG_SCOPED_DEBUG(fmt::format("CWindow::setStyle(style = {})", FromEnum(style)));

		if (m_osState.fullscreen)
		{
			LOG_FAIL("A window in fullscreen mode cannot change its style");
			return;
		}

		if (m_osState.style == style)
		{
			return;
		}

		if (not m_windowShown)
		{
			showOnWindowThread();
		}

		const auto currentStyle = m_osState.style;
		const Point visibleFramePos = m_osState.bounds.pos;
		const bool triggerWindowResize = ((currentStyle == WindowStyle::Frameless) || (style == WindowStyle::Frameless));
		const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(style);
		m_osState.style = style;

		::SetWindowLongPtrW(m_hWnd, GWL_STYLE, windowStyleFlags);

		{
			const double scaling = WindowMisc::GetScaling(m_dpi);
			const Size newFrameBufferSize = Math::Round(m_osState.virtualSize * scaling).asPoint();
			const Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
				Point{ 0, 0 }, newFrameBufferSize, windowStyleFlags);
			
			const uint32 flags = ((triggerWindowResize ? 0 : SWP_NOSIZE) | SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			setWindowPos(windowRect, flags);
		}

		onBoundsUpdate();
		// The invisible border can change with the style. Preserve the visible origin
		// using the new frame, after SWP_FRAMECHANGED has applied the native style.
		if (m_osState.bounds.pos != visibleFramePos)
		{
			setWindowPos(Rect{ getNativeWindowPos(visibleFramePos), Size{ 0, 0 } },
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		showOnWindowThread();
	}

	////////////////////////////////////////////////////////////////
	//
	//	setPos
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setPos(const Point pos)
	{
		invokeOnWindowThread([&] { setPosOnWindowThread(pos); });
	}

	void CWindow::setPosOnWindowThread(const Point pos)
	{
		LOG_SCOPED_DEBUG(fmt::format("CWindow::setPos(pos = {})", pos));

		if (m_osState.fullscreen)
		{
			LOG_FAIL("A window in fullscreen mode cannot be moved");
			return;
		}

		{
			const int32 previousDPI = m_dpi;
			const double scaling = WindowMisc::GetScaling(m_dpi);
			const Size newFrameBufferSize = Math::Round(m_osState.virtualSize * scaling).asPoint();
			const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(m_osState.style);
			Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
				Point{ 0, 0 }, newFrameBufferSize, windowStyleFlags);
			windowRect.pos = getNativeWindowPos(pos);
			
			constexpr uint32 Flags = (SWP_DEFERERASE | SWP_NOOWNERZORDER | SWP_NOZORDER);
			setWindowPos(windowRect, Flags);
			if (m_dpi != previousDPI)
			{
				// WM_DPICHANGED may have selected a new native origin and border.
				// Finish this explicit placement at the requested visible origin once.
				setWindowPos(Rect{ getNativeWindowPos(pos), Size{ 0, 0 } }, (Flags | SWP_NOSIZE));
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	show
	//
	////////////////////////////////////////////////////////////////

	void CWindow::show()
	{
		invokeOnWindowThread([&] { showOnWindowThread(); });
	}

	void CWindow::showOnWindowThread()
	{
		WindowMisc::ShowWindow(m_hWnd);

		m_windowShown = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	maximize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::maximize()
	{
		LOG_DEBUG("CWindow::maximize()");

		invokeOnWindowThread([&]
		{
			if (m_osState.style == WindowStyle::Fixed)
			{
				LOG_FAIL("A window with WindowStyle::Fixed cannot be maximized");
				return;
			}
			::ShowWindow(m_hWnd, SW_MAXIMIZE);
		});
	}

	////////////////////////////////////////////////////////////////
	//
	//	restore
	//
	////////////////////////////////////////////////////////////////

	void CWindow::restore()
	{
		LOG_DEBUG("CWindow::restore()");
		invokeOnWindowThread([&] { ::ShowWindow(m_hWnd, SW_RESTORE); });
	}

	////////////////////////////////////////////////////////////////
	//
	//	minimize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::minimize()
	{
		LOG_DEBUG("CWindow::minimize()");
		invokeOnWindowThread([&] { ::ShowWindow(m_hWnd, SW_MINIMIZE); });
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeByVirtualSize
	//
	////////////////////////////////////////////////////////////////

	bool CWindow::resizeByVirtualSize(const Size virtualSize)
	{
		LOG_DEBUG(fmt::format("CWindow::resizeByVirtualSize(size = {})", virtualSize));

		return resize(virtualSize, true);
	}

	////////////////////////////////////////////////////////////////
	//
	//	resizeByFrameBufferSize
	//
	////////////////////////////////////////////////////////////////

	bool CWindow::resizeByFrameBufferSize(const Size frameBufferSize)
	{
		LOG_DEBUG(fmt::format("CWindow::resizeByFrameBufferSize(size = {})", frameBufferSize));

		return resize(frameBufferSize, false);
	}

	bool CWindow::resize(const Size size, const bool virtualSize)
	{
		// Read the latest native state before testing the size. Fullscreen exit
		// and restoration must publish the destination DPI before resizing.
		invokeOnWindowThread([] {});
		if ((virtualSize ? getState().virtualSize : getState().frameBufferSize) == size)
		{
			return true;
		}
		if (getState().fullscreen)
		{
			setFullscreen(false, System::GetCurrentMonitorIndex());
		}

		if (virtualSize && (getState().frameBufferSize == Math::Round(size * getState().scaling).asPoint()))
		{
			return true;
		}
		if (getState().maximized) { restore(); }

		bool resized = false;
		invokeOnWindowThread([&] { resized = resizeOnWindowThread(size, virtualSize); });
		return resized;
	}

	bool CWindow::resizeOnWindowThread(const Size size, const bool virtualSize)
	{
		const Size newFrameBufferSize = (virtualSize ? Math::Round(size * m_osState.scaling).asPoint() : size);

		if ((newFrameBufferSize.x < m_osState.minFrameBufferSize.x)
			|| (newFrameBufferSize.y < m_osState.minFrameBufferSize.y))
		{
			LOG_FAIL("✖ Failed to resize the window: The specified size is smaller than the minimum size");
			return false;
		}

		const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(m_osState.style);
		const Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
			Point{ 0, 0 }, newFrameBufferSize, windowStyleFlags);

		constexpr uint32 Flags = (SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		setWindowPos(windowRect, Flags);

		::ShowWindow(m_hWnd, SW_NORMAL);

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setMinimumFrameBufferSize
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setMinimumFrameBufferSize(const Size size)
	{
		LOG_DEBUG(fmt::format("CWindow::setMinimumFrameBufferSize(size = {})", size));
		
		invokeOnWindowThread([&] { m_osState.minFrameBufferSize = size; });
	}

	////////////////////////////////////////////////////////////////
	//
	//	setFullscreen
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setFullscreen(const bool fullscreen, const size_t monitorIndex)
	{
		setFullscreen(fullscreen, monitorIndex, {});
	}

	void CWindow::setFullscreen(const bool fullscreen, const size_t monitorIndex, const WindowsMonitor::Functions& functions)
	{
		invokeOnWindowThread([&] { setFullscreenOnWindowThread(fullscreen, monitorIndex, functions); });
	}

	bool CWindow::setFullscreenOnWindowThread(const bool fullscreen, const size_t monitorIndex, const WindowsMonitor::Functions& functions)
	{
		LOG_DEBUG(fmt::format("CWindow::setFullscreen(fullscreen = {}, monitorIndex = {})", fullscreen, monitorIndex));

		if ((not fullscreen) && (not m_osState.fullscreen))
		{
			return false;
		}

		if (not m_windowShown)
		{
			showOnWindowThread();
		}

		m_fullscreenTransition = true;
		const ScopeExit finishTransition{ [this] { m_fullscreenTransition = false; } };

		if (fullscreen && m_osState.fullscreen)
		{
			return WindowMisc::MoveFullscreen(m_hWnd, monitorIndex, functions);
		}
		else if (fullscreen)
		{
			// フルスクリーンモードにする
			if (not WindowMisc::SetFullscreen(m_hWnd, monitorIndex, m_storedWindowRect, WindowMisc::GetBaseWindowStyle(m_osState.style), functions))
			{
				return false;
			}
		}
		else
		{
			const Size size{
				(m_storedWindowRect.right - m_storedWindowRect.left),
				(m_storedWindowRect.bottom - m_storedWindowRect.top) };

			::SetWindowLongW(m_hWnd, GWL_STYLE, WindowMisc::GetBaseWindowStyle(m_osState.style));
			// Leave the maximized fullscreen state before applying the saved rectangle.
			::ShowWindow(m_hWnd, SW_NORMAL);
			::SetWindowPos(
				m_hWnd,
				HWND_NOTOPMOST,
				m_storedWindowRect.left,
				m_storedWindowRect.top,
				size.x,
				size.y,
				(SWP_FRAMECHANGED | SWP_NOACTIVATE));

			::ValidateRect(m_hWnd, nullptr);
		}

		m_osState.fullscreen = fullscreen;

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setToggleFullscreenEnabled
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setToggleFullscreenEnabled(const bool enabled)
	{
		if (enabled)
		{
			auto expected = ToggleFullscreenState::Disabled;
			m_toggleFullscreenState.compare_exchange_strong(expected, ToggleFullscreenState::Enabled);
		}
		else
		{
			// Cancel pending requests atomically with disabling message acceptance.
			m_toggleFullscreenState.store(ToggleFullscreenState::Disabled);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	isToggleFullscreenEnabled
	//
	////////////////////////////////////////////////////////////////

	bool CWindow::isToggleFullscreenEnabled() const
	{
		return (m_toggleFullscreenState.load() != ToggleFullscreenState::Disabled);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setTaskbarProgressBar
	//
	////////////////////////////////////////////////////////////////

	void CWindow::setTaskbarProgressBar(double progress0_1)
	{
		if (not m_taskbarList)
		{
			return;
		}

		progress0_1 = Clamp(progress0_1, 0.0, 1.0);
		
		if (progress0_1 < 1.0)
		{
			m_taskbarList->SetProgressValue(m_hWnd, static_cast<int32>(progress0_1 * 100), 100);
		}
		else
		{
			m_taskbarList->SetProgressState(m_hWnd, TBPF_NOPROGRESS);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	destroy
	//
	////////////////////////////////////////////////////////////////

	void CWindow::attach(const HWND hWnd) noexcept
	{
		m_hWnd = hWnd;
	}

	void CWindow::beginStateUpdate() noexcept
	{
		m_stateSync.beginUpdate();
	}

	void CWindow::endStateUpdate()
	{
		if (m_stateSync.endUpdate(m_osState) && m_deferredCommands)
		{
			auto* command = m_deferredCommands;
			m_deferredCommands = command->next;
			dispatchCommand(*command);
		}
	}

	void CWindow::dispatchCommand(WindowThreadCommand& command)
	{
		// A cross-thread SendMessage can itself reenter a native notification.
		// Defer that command until the outer notification has committed; never
		// block the window thread waiting for the engine to resume.
		if (m_stateSync.isUpdating())
		{
			auto** tail = &m_deferredCommands;
			while (*tail) { tail = &(*tail)->next; }
			*tail = &command;
			return;
		}
		beginStateUpdate();
		command.execute();
		endStateUpdate();
		command.complete();
	}

	const WindowState& CWindow::getOSState() const noexcept
	{
		return m_osState;
	}

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
			m_osState.minimized = true;
			m_osState.maximized = false;
		}
		else if (maximized)
		{
			m_osState.minimized = false;
			m_osState.maximized = true;
		}
		else
		{
			m_osState.minimized = false;
			m_osState.maximized = false;
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

		if (focused)
		{
			SIV3D_ENGINE(Console)->clearSelection();
		}

		m_osState.focused = focused;
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
			m_osState.frameBufferSize = size;
			m_osState.virtualSize = Math::Round(m_osState.frameBufferSize / m_osState.scaling).asPoint();
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
			RECT windowRect{};
			if (SUCCEEDED(::DwmGetWindowAttribute(m_hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect, sizeof(RECT))))
			{
				m_osState.bounds = ToRect(windowRect);
			}
		}

		// frame thickness
		{
			m_osState.frameThickness.set(getSystemMetrics(SM_CXBORDER), getSystemMetrics(SM_CYBORDER));
		}

		// title bar height
		{
			m_osState.titleBarHeight = (getSystemMetrics(SM_CYCAPTION)
				+ getSystemMetrics(SM_CYFRAME)
				+ getSystemMetrics(SM_CXPADDEDBORDER));
		}

		// border
		{
			const DWORD windowStyleFlags = static_cast<DWORD>(::GetWindowLongPtrW(m_hWnd, GWL_STYLE));
			const Rect placeholderWindowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi, Point{ 0, 0 }, Size{ 0, 0 }, windowStyleFlags);
			m_border = placeholderWindowRect.size;
		}

		LOG_TRACE(fmt::format("- bounds: {}, frameThickness: {}, titleBarHeight: {}, border: {}",
			m_osState.bounds, m_osState.frameThickness, m_osState.titleBarHeight, m_border));
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
		m_osState.scaling = scaling;

		onBoundsUpdate();

		if (m_fullscreenTransition)
		{
			// The transition already supplies a monitor-sized or saved physical rectangle.
			// Do not replace it with a size derived from the previous monitor's geometry.
			m_osState.virtualSize = Math::Round(m_osState.frameBufferSize / scaling).asPoint();
			return;
		}

		const Size newFrameBufferSize = Math::Round(m_osState.virtualSize * scaling).asPoint();
		const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(m_osState.style);
		Rect windowRect = WindowMisc::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi,
			Point{ 0, 0 }, newFrameBufferSize, windowStyleFlags);
		// WM_DPICHANGED supplies a native window origin, including invisible borders.
		windowRect.pos = suggestedPos;

		constexpr uint32 Flags = (SWP_NOACTIVATE | SWP_NOZORDER);
		setWindowPos(windowRect, Flags);
	
		m_osState.virtualSize = Math::Round(m_osState.frameBufferSize / m_osState.scaling).asPoint();
	}

	////////////////////////////////////////////////////////////////
	//
	//	onEnterSizeMove
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onEnterSizeMove()
	{
		m_osState.sizeMove = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	onExitSizeMove
	//
	////////////////////////////////////////////////////////////////

	void CWindow::onExitSizeMove()
	{
		m_osState.sizeMove = false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMinTrackSize
	//
	////////////////////////////////////////////////////////////////

	Size CWindow::getMinTrackSize() noexcept
	{
		return (m_osState.minFrameBufferSize + m_border);
	}

	////////////////////////////////////////////////////////////////
	//
	//	requestToggleFullscreen
	//
	////////////////////////////////////////////////////////////////

	void CWindow::requestToggleFullscreen()
	{
		auto expected = ToggleFullscreenState::Enabled;
		m_toggleFullscreenState.compare_exchange_strong(expected, ToggleFullscreenState::Requested);
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
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	Point CWindow::getNativeWindowPos(const Point visibleFramePos) const
	{
		RECT native{}, visible{};
		if (::GetWindowRect(m_hWnd, &native)
			&& SUCCEEDED(::DwmGetWindowAttribute(m_hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &visible, sizeof(visible))))
		{
			return (visibleFramePos + Point{ native.left - visible.left, native.top - visible.top });
		}
		return visibleFramePos;
	}

	void CWindow::setWindowPos(const Rect& rect, const uint32 flags)
	{
		const Point pos = rect.pos;
		const Size size = rect.size;

		LOG_DEBUG(fmt::format("SetWindowPos({}, {}, {}, {}, {:#x})", pos.x, pos.y, size.x, size.y, flags));
		::SetWindowPos(m_hWnd, HWND_TOP, pos.x, pos.y, size.x, size.y, flags);
	}
}
