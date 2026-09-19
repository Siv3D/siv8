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

# pragma once
# include <Siv3D/FunctionRef.hpp>
# include <Siv3D/Types.hpp>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d::WindowsMonitor
{
	struct Functions
	{
		decltype(::EnumDisplayDevicesW)* enumDisplayDevices = ::EnumDisplayDevicesW;
		decltype(::MonitorFromWindow)* monitorFromWindow = ::MonitorFromWindow;
		decltype(::GetMonitorInfoW)* getMonitorInfo = ::GetMonitorInfoW;
	};

	// Uses the public monitor order, including the fallback when no active child is found.
	// The visitor borrows each device, receives a null monitor for fallback entries,
	// and returns false to stop enumeration. No device list is allocated here.
	void EnumerateMonitorDevices(FunctionRef<bool(const DISPLAY_DEVICEW&, const DISPLAY_DEVICEW*)> visitor,
		decltype(::EnumDisplayDevicesW)* enumDisplayDevices = ::EnumDisplayDevicesW);

	[[nodiscard]]
	size_t GetCurrentMonitorIndex(HWND window, const Functions& functions = {});
}
