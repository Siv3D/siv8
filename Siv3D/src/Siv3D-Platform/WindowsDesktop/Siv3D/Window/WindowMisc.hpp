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

# pragma once
# include <Siv3D/DLL.hpp>
# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/WindowStyle.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include <ShObjIdl_core.h> // ITaskbarList3

namespace s3d
{
	void SetDPIAwareness(LibraryHandle user32);

	[[nodiscard]]
	double GetScaling(uint32 dpi) noexcept;

	[[nodiscard]]
	Point CalculateWindowPos(const MonitorInfo& monitor, const Size& frameBufferSize) noexcept;

	[[nodiscard]]
	uint32 GetWindowStyleFlags(WindowStyle style) noexcept;

	[[nodiscard]]
	Rect AdjustWindowRect(HWND hWnd, decltype(AdjustWindowRectExForDpi)* pAdjustWindowRectExForDpi,
		int32 dpi, const Point& windowPos, const Size& size, int32 windowStyleFlags);

	void DisableTouchFeedbackVisualization(HWND hWND, decltype(SetWindowFeedbackSetting)* pSetWindowFeedbackSetting);

	[[nodiscard]]
	HDEVNOTIFY StartDeviceNotification(HWND hWnd);

	[[nodiscard]]
	ComPtr<ITaskbarList3> CreateTaskbarList();
}
