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

# include "WindowMisc.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>
# include <ShellScalingApi.h> // SetProcessDpiAwareness()
# include <Dbt.h>

namespace s3d::WindowMisc
{
	////////////////////////////////////////////////////////////////
	//
	//	SetDPIAwareness
	//
	////////////////////////////////////////////////////////////////

	void SetDPIAwareness(LibraryHandle user32)
	{
		LOG_SCOPED_DEBUG("SetDPIAwareness()");

		// Windows 10 1703-
		if (decltype(SetProcessDpiAwarenessContext) * p_SetProcessDpiAwarenessContext = DLL::GetFunctionNoThrow(user32, "SetProcessDpiAwarenessContext"))
		{
			LOG_DEBUG("SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)");
			p_SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
			return;
		}

		// Windows 10 1607-
		if (decltype(SetThreadDpiAwarenessContext) * p_SetThreadDpiAwarenessContext = DLL::GetFunctionNoThrow(user32, "SetThreadDpiAwarenessContext"))
		{
			LOG_DEBUG("SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)");
			p_SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

			LOG_DEBUG("SetProcessDPIAware()");
			::SetProcessDPIAware();
			return;
		}

		// Windows 8.1-
		if (LibraryHandle shcore = DLL::LoadSystemLibraryNoThrow(L"shcore.dll"))
		{
			if (decltype(SetProcessDpiAwareness) * p_SetProcessDpiAwareness = DLL::GetFunctionNoThrow(shcore, "SetProcessDpiAwareness"))
			{
				LOG_DEBUG("SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE)");
				p_SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
			}

			DLL::Unload(shcore);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	GetMonitors
	//
	////////////////////////////////////////////////////////////////

	Array<MonitorInfo> GetMonitors()
	{
		Array<MonitorInfo> monitors = System::EnumerateMonitors();

		if (not monitors)
		{
			throw InternalEngineError{ "System::EnumActiveMonitors() failed" };
		}

		String message = U"Active monitors:\n";

		for (size_t i = 0; i < monitors.size(); ++i)
		{
			if (0 < i)
			{
				message.push_back(U'\n');
			}

			message += U"---- 🖥️ Monitor[{}] ----\n"_fmt(i);
			message += monitors[i].format();
		}

		LOG_DEBUG(message);

		return monitors;
	}

	////////////////////////////////////////////////////////////////
	//
	//	GetScaling
	//
	////////////////////////////////////////////////////////////////

	double GetScaling(uint32 dpi) noexcept
	{
		return (static_cast<double>(dpi) / USER_DEFAULT_SCREEN_DPI);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CalculateWindowPos
	//
	////////////////////////////////////////////////////////////////

	Point CalculateWindowPos(const MonitorInfo& monitor, const Size& frameBufferSize) noexcept
	{
		const int32 offsetX = Max<int32>((monitor.workArea.w - frameBufferSize.x) / 2, 0);
		const int32 offsetY = Max<int32>((monitor.workArea.h - frameBufferSize.y) / 2, 0);
		return (monitor.displayRect.pos + Point{ offsetX, offsetY });
	}

	////////////////////////////////////////////////////////////////
	//
	//	GetWindowStyleFlags
	//
	////////////////////////////////////////////////////////////////

	uint32 GetWindowStyleFlags(const WindowStyle style) noexcept
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

	////////////////////////////////////////////////////////////////
	//
	//	AdjustWindowRect
	//
	////////////////////////////////////////////////////////////////

	Rect AdjustWindowRect(const HWND hWnd, decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi,
		const int32 dpi, const Point& windowPos, const Size& size, const uint32 windowStyleFlags)
	{
		LOG_DEBUG(U"AdjustWindowRect({}, {}, {:#x})"_fmt(windowPos, size, windowStyleFlags));

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

	////////////////////////////////////////////////////////////////
	//
	//	CreateMainWindow
	//
	////////////////////////////////////////////////////////////////

	HWND CreateMainWindow(HINSTANCE hInstance, const MonitorInfo& monitor,
		const std::wstring& windowClassName, const String& windowName, decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi,
		int32& dpi, WindowState& windowState)
	{
		const double scale = monitor.scaling.value_or(1.0);
		dpi = static_cast<int32>(std::round(USER_DEFAULT_SCREEN_DPI * scale));
		windowState.scaling = WindowMisc::GetScaling(dpi);
		windowState.frameBufferSize = (windowState.virtualSize * scale).asPoint();

		const uint32 windowStyleFlags = WindowMisc::GetWindowStyleFlags(windowState.style);
		const Point windowPos = WindowMisc::CalculateWindowPos(monitor, windowState.frameBufferSize);
		const Rect windowRect = WindowMisc::AdjustWindowRect(nullptr, pAdjustWindowRectExForDpi, dpi, windowPos, windowState.frameBufferSize, windowStyleFlags);

		LOG_DEBUG("CreateWindowExW()");
		return ::CreateWindowExW(
			0,
			windowClassName.c_str(),
			Unicode::ToWstring(windowName).c_str(),
			windowStyleFlags,
			windowRect.x, windowRect.y,
			windowRect.w, windowRect.h,
			nullptr, // No parent window
			nullptr, // No menu
			hInstance,
			nullptr);
	}

	////////////////////////////////////////////////////////////////
	//
	//	DisablePalmRejection
	//
	////////////////////////////////////////////////////////////////

	void DisablePalmRejection(HWND hWND)
	{
		if (::GetSystemMetrics(SM_DIGITIZER) & NID_MULTI_INPUT)
		{
			LOG_INFO("ℹ️ An input digitizer with support for multiple inputs found");
			::RegisterTouchWindow(hWND, TWF_WANTPALM);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	DisableTouchFeedbackVisualization
	//
	////////////////////////////////////////////////////////////////

	void DisableTouchFeedbackVisualization(HWND hWND, decltype(SetWindowFeedbackSetting)* pSetWindowFeedbackSetting)
	{
		LOG_DEBUG("DisableTouchFeedbackVisualization()");

		if (pSetWindowFeedbackSetting)
		{
			static constexpr std::array<FEEDBACK_TYPE, 11> feedbackTypes
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

	////////////////////////////////////////////////////////////////
	//
	//	StartDeviceNotification
	//
	////////////////////////////////////////////////////////////////

	HDEVNOTIFY StartDeviceNotification(const HWND hWnd)
	{
		LOG_DEBUG("StartDeviceNotification()");

		static constexpr GUID _GUID_DEVINTERFACE_HID = { 0x4D1E55B2L, 0xF16F, 0x11CF, 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };

		DEV_BROADCAST_DEVICEINTERFACE_W dbi{};
		dbi.dbcc_size = sizeof(dbi);
		dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		dbi.dbcc_classguid = _GUID_DEVINTERFACE_HID;

		LOG_DEBUG("RegisterDeviceNotificationW()");
		return ::RegisterDeviceNotificationW(hWnd, (DEV_BROADCAST_HDR*)&dbi, DEVICE_NOTIFY_WINDOW_HANDLE);
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateTaskbarList
	//
	////////////////////////////////////////////////////////////////

	ComPtr<ITaskbarList3> CreateTaskbarList()
	{
		LOG_DEBUG("CreateTaskbarList()");

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

	////////////////////////////////////////////////////////////////
	//
	//	ShowWindow
	//
	////////////////////////////////////////////////////////////////

	void ShowWindow(HWND hWnd)
	{
		LOG_DEBUG("ShowWindow()");

		::ShowWindow(hWnd, SW_SHOW);

		::ValidateRect(hWnd, 0);

		::UpdateWindow(hWnd);

		::SetForegroundWindow(hWnd);
	}
}
