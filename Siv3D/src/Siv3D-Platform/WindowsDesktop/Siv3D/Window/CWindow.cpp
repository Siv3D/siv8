//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <array>
# include "CWindow.hpp"
# include "DPIAwareness.hpp"
# include "WindowProc.hpp"
# include <Dbt.h>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Monitor.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		constexpr double GetScaling(uint32 dpi) noexcept
		{
			return (static_cast<double>(dpi) / USER_DEFAULT_SCREEN_DPI);
		}

		[[nodiscard]]
		constexpr Point CalculateWindowPos(const MonitorInfo& monitor, const Size& frameBufferSize) noexcept
		{
			const int32 offsetX = Max<int32>((monitor.workArea.w - frameBufferSize.x) / 2, 0);
			const int32 offsetY = Max<int32>((monitor.workArea.h - frameBufferSize.y) / 2, 0);
			return (monitor.displayRect.pos + Point{ offsetX, offsetY });
		}

		[[nodiscard]]
		constexpr uint32 GetWindowStyleFlags(const WindowStyle style) noexcept
		{
			switch (style)
			{
			case WindowStyle::Fixed:
				return (WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME));
			case WindowStyle::Frameless:
				return (WS_POPUP | WS_VISIBLE | WS_MINIMIZEBOX);
			case WindowStyle::Sizable:
			default:
				return WS_OVERLAPPEDWINDOW;
			}
		}

		[[nodiscard]]
		static Rect AdjustWindowRect(const HWND hWnd, decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi,
			const int32 dpi, const Point& windowPos, const Size& size, const int32 windowStyleFlags)
		{
			LOG_TRACE(U"AdjustWindowRect({}, {}, {:#x})"_fmt(windowPos, size, windowStyleFlags));

			const DWORD windowExStyleFlags = static_cast<DWORD>(::GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
			RECT rect{ windowPos.x, windowPos.y, (windowPos.x + size.x), (windowPos.y + size.y) };

			if (pAdjustWindowRectExForDpi)
			{
				pAdjustWindowRectExForDpi(&rect, windowStyleFlags, false, windowExStyleFlags, dpi);
			}
			else
			{
				::AdjustWindowRectEx(&rect, windowStyleFlags, false, windowExStyleFlags);
			}

			return{ rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top) };
		}

		static void DisableTouchFeedbackVisualization(HWND hWND, decltype(SetWindowFeedbackSetting)* pSetWindowFeedbackSetting)
		{
			LOG_TRACE("DisableTouchFeedbackVisualization()");

			if (pSetWindowFeedbackSetting)
			{
				static constexpr std::array<FEEDBACK_TYPE, 11> feedbackTypes =
				{
					FEEDBACK_TOUCH_CONTACTVISUALIZATION,
					FEEDBACK_PEN_BARRELVISUALIZATION,
					FEEDBACK_PEN_TAP,
					FEEDBACK_PEN_DOUBLETAP,
					FEEDBACK_PEN_PRESSANDHOLD,
					FEEDBACK_PEN_RIGHTTAP,
					FEEDBACK_TOUCH_TAP,
					FEEDBACK_TOUCH_DOUBLETAP,
					FEEDBACK_TOUCH_PRESSANDHOLD,
					FEEDBACK_TOUCH_RIGHTTAP,
					FEEDBACK_GESTURE_PRESSANDTAP,
				};

				for (const auto& feedbackType : feedbackTypes)
				{
					BOOL val = FALSE;
					pSetWindowFeedbackSetting(hWND, feedbackType, 0, sizeof(BOOL), &val);
				}
			}
		}

		[[nodiscard]]
		static HDEVNOTIFY StartDeviceNotification(const HWND hWnd)
		{
			static constexpr GUID _GUID_DEVINTERFACE_HID = { 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };

			DEV_BROADCAST_DEVICEINTERFACE_W dbi{};
			dbi.dbcc_size = sizeof(dbi);
			dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			dbi.dbcc_classguid = _GUID_DEVINTERFACE_HID;

			LOG_TRACE("RegisterDeviceNotificationW()");
			return ::RegisterDeviceNotificationW(hWnd, (DEV_BROADCAST_HDR*)&dbi, DEVICE_NOTIFY_WINDOW_HANDLE);
		}

		[[nodiscard]]
		static ComPtr<ITaskbarList3> CreateTaskbarList()
		{
			ComPtr<ITaskbarList3> taskbarList;

			if (SUCCEEDED(::CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&taskbarList))))
			{
				if (FAILED(taskbarList->HrInit()))
				{
					LOG_FAIL("Failed to initialize a TaskbarList object");
					taskbarList.Reset();
				}
			}
			else
			{
				LOG_FAIL("Failed to create a TaskbarList object");
			}

			return taskbarList;
		}
	}

	extern std::atomic_flag g_shouldDestroyWindow;

	CWindow::CWindow() = default;

	CWindow::~CWindow()
	{
		LOG_SCOPED_TRACE("CWindow::~CWindow()");

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
		LOG_SCOPED_TRACE("CWindow::init()");

		// hInstance を取得する
		m_hInstance = ::GetModuleHandleW(nullptr);

		// User32.dll の関数をロードする
		m_user32.load();

		// DPI awareness を有効化する
		detail::SetDPIAwareness(m_user32.library);

		// ウィンドウクラスを登録する
		m_windowClass.registerClass(m_hInstance);

		// モニタを取得する
		const Array<MonitorInfo> monitors = System::EnumerateMonitors();
		{
			if (monitors.empty())
			{
				throw EngineError{ "System::EnumActiveMonitors() failed" };
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
			m_state.scaling = detail::GetScaling(m_dpi);
			m_state.frameBufferSize = (m_state.virtualSize * scale).asPoint();

			const uint32 windowStyleFlags = detail::GetWindowStyleFlags(m_state.style);
			const Point windowPos = detail::CalculateWindowPos(monitor, m_state.frameBufferSize);
			const Rect windowRect = detail::AdjustWindowRect(m_hWnd, m_user32.pAdjustWindowRectExForDpi, m_dpi, windowPos, m_state.frameBufferSize, windowStyleFlags);

			LOG_TRACE("CreateWindowExW()");
			m_hWnd = ::CreateWindowExW(
				0,
				m_windowClass.name.c_str(),
				m_title.actual.toWstr().c_str(),
				windowStyleFlags,
				windowRect.x, windowRect.y,
				windowRect.w, windowRect.h,
				nullptr, // No parent window
				nullptr, // No menu
				m_hInstance,
				nullptr);

			if (not m_hWnd)
			{
				throw EngineError{ "CreateWindowExW() failed" };
			}

			break;
		}

		if (::GetSystemMetrics(SM_DIGITIZER) & NID_MULTI_INPUT)
		{
			LOG_INFO("ℹ️ An input digitizer with support for multiple inputs found");
			::RegisterTouchWindow(m_hWnd, TWF_WANTPALM);
		}

		// Disable touch feedback visualization that causes frame rate drops
		detail::DisableTouchFeedbackVisualization(m_hWnd, m_user32.pSetWindowFeedbackSetting);

		// DBT_DEVICEARRIVAL, DBT_DEVICEREMOVECOMPLETE が送られるようにする
		m_deviceNotificationHandle = detail::StartDeviceNotification(m_hWnd);

		// タスクバーリストを作成する
		m_taskbarList = detail::CreateTaskbarList();

		//SIV3D_ENGINE(TextInput)->init();
	}

	void CWindow::update()
	{
		//if (m_toggleFullscreenEnabled && m_toggleFullscreenRequest)
		//{
		//	if (not m_oldResizeMode)
		//	{
		//		m_oldResizeMode = Scene::GetResizeMode();
		//	}

		//	const bool toFullScreen = (not m_state.fullscreen);

		//	setFullscreen(not m_state.fullscreen, System::GetCurrentMonitorIndex(), true);

		//	if (toFullScreen)
		//	{
		//		Scene::SetResizeMode(ResizeMode::Keep);
		//	}
		//	else if (m_oldResizeMode)
		//	{
		//		Scene::SetResizeMode(*m_oldResizeMode);
		//		m_oldResizeMode.reset();
		//	}
		//}

		//m_toggleFullscreenRequest = false;

		if constexpr (SIV3D_BUILD(DEBUG))
		{
			setWindowTitle(m_title.title);
		}
	}

	void CWindow::setWindowTitle(const String& title)
	{
		String newActualTitle = title;

		//if constexpr (SIV3D_BUILD(DEBUG))
		//{
		//	const String statistics = SIV3D_ENGINE(Profiler)->getSimpleStatistics();
		//	newActualTitle += U" (Debug Build) | ";
		//	newActualTitle += statistics;
		//}

		if (m_title.actual != newActualTitle)
		{
			::SetWindowTextW(m_hWnd, newActualTitle.toWstr().c_str());
			m_title.actual.swap(newActualTitle);
		}

		m_title.title = title;
	}

	const String& CWindow::getWindowTitle() const noexcept
	{
		return m_title.title;
	}

	void* CWindow::getHandle() const noexcept
	{
		return m_hWnd;
	}
	
	void CWindow::show()
	{
		LOG_TRACE("ShowWindow()");
		
		::ShowWindow(m_hWnd, SW_SHOW);
		
		::ValidateRect(m_hWnd, 0);
		
		::UpdateWindow(m_hWnd);
		
		::SetForegroundWindow(m_hWnd);
		
		m_windowShown = true;
	}

	void CWindow::destroy()
	{
		LOG_SCOPED_TRACE("CWindow::destroy()");

		if (m_hWnd)
		{
			m_taskbarList.Reset();

			if (m_deviceNotificationHandle)
			{
				LOG_TRACE("UnregisterDeviceNotification()");
				::UnregisterDeviceNotification(m_deviceNotificationHandle);
				m_deviceNotificationHandle = nullptr;
			}

			LOG_TRACE("DestroyWindow()");
			[[maybe_unused]] const BOOL b = ::DestroyWindow(m_hWnd);
			LOG_TRACE(fmt::format("DestroyWindow() -> {}", static_cast<bool>(b)));
		}

		m_windowClass.unregisterClass(m_hInstance);
	}

	void CWindow::User32Lib::load()
	{
		library = DLL::LoadSystemLibrary(L"user32.dll");
		pGetSystemMetricsForDpi = DLL::GetFunctionNoThrow(library, "GetSystemMetricsForDpi");
		pAdjustWindowRectExForDpi = DLL::GetFunctionNoThrow(library, "AdjustWindowRectExForDpi");
		pSetWindowFeedbackSetting = DLL::GetFunctionNoThrow(library, "SetWindowFeedbackSetting");
	}

	void CWindow::User32Lib::unload()
	{
		DLL::Unload(library);
	}

	void CWindow::WindowClass::registerClass(const HINSTANCE hInstance)
	{
		// WindowClass の名前を生成
		name = FileSystem::ModulePath().toWstr();

		const WNDCLASSEX windowClass
		{
			.cbSize			= sizeof(WNDCLASSEX),
			.style			= (CS_HREDRAW | CS_VREDRAW),
			.lpfnWndProc	= WindowProc,
			.hInstance		= hInstance,
			.hIcon			= ::LoadIconW(hInstance, MAKEINTRESOURCEW(100)),
			.hCursor		= nullptr,
			.hbrBackground	= static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)),
			.lpszClassName	= name.c_str(),
		};

		{
			LOG_TRACE("RegisterClassExW()");
		
			if (not ::RegisterClassExW(&windowClass))
			{
				throw EngineError{ "RegisterClassExW() failed" };
			}
		}
	}

	void CWindow::WindowClass::unregisterClass(const HINSTANCE hInstance)
	{
		LOG_TRACE("UnregisterClassW()");	
		[[maybe_unused]] const BOOL b = ::UnregisterClassW(name.c_str(), hInstance);	
		LOG_TRACE(fmt::format("UnregisterClassW() -> {}", static_cast<bool>(b)));
	}
}
