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
		WindowMisc::SetDPIAwareness(m_user32.library);

		// ウィンドウクラスを登録する
		m_windowClass.registerClass(m_hInstance);

		// プライマリモニターにウィンドウを作成する
		for (const auto& monitor : WindowMisc::GetMonitors())
		{
			if (monitor.isPrimary)
			{
				// ウィンドウを作成する
				m_hWnd = WindowMisc::CreateMainWindow(
					m_hInstance,
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
		WindowMisc::ShowWindow(m_hWnd);

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
				LOG_DEBUG("UnregisterDeviceNotification()");
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
