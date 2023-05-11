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
# include <Siv3D/EngineLog.hpp>
# include "WindowMisc.hpp"
# include <Dbt.h>

namespace s3d::detail
{
	[[nodiscard]]
	double GetScaling(uint32 dpi) noexcept
	{
		return (static_cast<double>(dpi) / USER_DEFAULT_SCREEN_DPI);
	}

	[[nodiscard]]
	Point CalculateWindowPos(const MonitorInfo& monitor, const Size& frameBufferSize) noexcept
	{
		const int32 offsetX = Max<int32>((monitor.workArea.w - frameBufferSize.x) / 2, 0);
		const int32 offsetY = Max<int32>((monitor.workArea.h - frameBufferSize.y) / 2, 0);
		return (monitor.displayRect.pos + Point{ offsetX, offsetY });
	}

	[[nodiscard]]
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

	[[nodiscard]]
	Rect AdjustWindowRect(const HWND hWnd, decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi,
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

	void DisableTouchFeedbackVisualization(HWND hWND, decltype(SetWindowFeedbackSetting)* pSetWindowFeedbackSetting)
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
	HDEVNOTIFY StartDeviceNotification(const HWND hWnd)
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
	ComPtr<ITaskbarList3> CreateTaskbarList()
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
