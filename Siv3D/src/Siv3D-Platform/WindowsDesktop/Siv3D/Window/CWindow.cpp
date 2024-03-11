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
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	extern std::atomic_flag g_shouldDestroyWindow;

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

	void CWindow::init()
	{
		LOG_SCOPED_DEBUG("CWindow::init()");

		// hInstance を取得する
		m_hInstance = ::GetModuleHandleW(nullptr);

		// User32.dll の関数をロードする
		m_user32.load();

		// DPI awareness を有効化する
		SetDPIAwareness(m_user32.library);

		// ウィンドウクラスを登録する
		m_windowClass.registerClass(m_hInstance);

		// モニタを取得する
		const Array<MonitorInfo> monitors = System::EnumerateMonitors();
		{
			if (not monitors)
			{
				throw InternalEngineError{ "System::EnumActiveMonitors() failed" };
			}

			for (size_t i = 0; i < monitors.size(); ++i)
			{
				LOG_TRACE(fmt::format("🖥️ Monitor[{}] ", i) + monitors[i].format().toUTF8());
			}
		}

		// ウィンドウを作成する
		for (const auto& monitor : monitors)
		{
			if (not monitor.isPrimary)
			{
				continue;
			}

			const double scale = monitor.scaling.value_or(1.0);
			m_dpi = static_cast<int32>(std::round(USER_DEFAULT_SCREEN_DPI * scale));
			m_state.scaling = GetScaling(m_dpi);
			m_state.frameBufferSize = (m_state.virtualSize * scale).asPoint();

			const uint32 windowStyleFlags = GetWindowStyleFlags(m_state.style);
			const Point windowPos = CalculateWindowPos(monitor, m_state.frameBufferSize);
			const Rect windowRect = AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi, windowPos, m_state.frameBufferSize, windowStyleFlags);

			LOG_DEBUG("CreateWindowExW()");
			m_hWnd = ::CreateWindowExW(
				0,
				m_windowClass.name.c_str(),
				Unicode::ToWstring(m_windowTitle.actual).c_str(),
				windowStyleFlags,
				windowRect.x, windowRect.y,
				windowRect.w, windowRect.h,
				nullptr, // No parent window
				nullptr, // No menu
				m_hInstance,
				nullptr);

			if (not m_hWnd)
			{
				throw InternalEngineError{ "CreateWindowExW() failed" };
			}

			break;
		}

		if (::GetSystemMetrics(SM_DIGITIZER) & NID_MULTI_INPUT)
		{
			LOG_INFO("ℹ️ An input digitizer with support for multiple inputs found");
			::RegisterTouchWindow(m_hWnd, TWF_WANTPALM);
		}

		// Disable touch feedback visualization that causes frame rate drops
		DisableTouchFeedbackVisualization(m_hWnd, m_user32.pSetWindowFeedbackSetting);

		// DBT_DEVICEARRIVAL, DBT_DEVICEREMOVECOMPLETE が送られるようにする
		m_deviceNotificationHandle = StartDeviceNotification(m_hWnd);

		// タスクバーリストを作成する
		m_taskbarList = CreateTaskbarList();
	}

	void CWindow::update()
	{

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			m_windowTitle.refresh(m_hWnd);
		}
	}

	void CWindow::setWindowTitle(const String& title)
	{
		m_windowTitle.set(m_hWnd, title);
	}

	const String& CWindow::getWindowTitle() const noexcept
	{
		return m_windowTitle.title;
	}

	void* CWindow::getHandle() const noexcept
	{
		return m_hWnd;
	}

	void CWindow::show()
	{
		LOG_DEBUG("ShowWindow()");

		::ShowWindow(m_hWnd, SW_SHOW);

		::ValidateRect(m_hWnd, 0);

		::UpdateWindow(m_hWnd);

		::SetForegroundWindow(m_hWnd);

		m_windowShown = true;
	}

	void CWindow::destroy()
	{
		LOG_SCOPED_DEBUG("CWindow::destroy()");

		if (m_hWnd)
		{
			m_taskbarList.Reset();

			if (m_deviceNotificationHandle)
			{
				LOG_TRACE("UnregisterDeviceNotification()");
				[[maybe_unused]] const BOOL b = ::UnregisterDeviceNotification(m_deviceNotificationHandle);
				m_deviceNotificationHandle = nullptr;
				LOG_DEBUG(fmt::format("UnregisterDeviceNotification() -> {}", static_cast<bool>(b)));				
			}

			LOG_DEBUG("DestroyWindow()");		
			[[maybe_unused]] const BOOL b = ::DestroyWindow(m_hWnd);
			LOG_DEBUG(fmt::format("DestroyWindow() -> {}", static_cast<bool>(b)));
		}

		m_windowClass.unregisterClass(m_hInstance);
	}
}
