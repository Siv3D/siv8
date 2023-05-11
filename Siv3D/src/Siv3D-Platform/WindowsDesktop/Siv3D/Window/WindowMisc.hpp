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

# pragma once
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Windows/ComPtr.hpp>
# include <ShObjIdl_core.h>
# include <Siv3D/Common.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/MonitorInfo.hpp>
# include <Siv3D/WindowStyle.hpp>

namespace s3d::detail
{
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
