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
# include <Siv3D/Array.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <dwmapi.h>

namespace s3d
{
	// Passed only during refresh-rate queries; no hooks or validation are used by Present.
	struct D3D11DisplayFrequencyFunctions
	{
		decltype(::MonitorFromWindow)* monitorFromWindow = ::MonitorFromWindow;
		decltype(::GetMonitorInfoW)* getMonitorInfo = ::GetMonitorInfoW;
		decltype(::GetDisplayConfigBufferSizes)* getDisplayConfigBufferSizes = ::GetDisplayConfigBufferSizes;
		decltype(::QueryDisplayConfig)* queryDisplayConfig = ::QueryDisplayConfig;
		decltype(::DisplayConfigGetDeviceInfo)* displayConfigGetDeviceInfo = ::DisplayConfigGetDeviceInfo;
		decltype(::EnumDisplaySettingsW)* enumDisplaySettings = ::EnumDisplaySettingsW;
		decltype(::DwmGetCompositionTimingInfo)* dwmGetCompositionTimingInfo = ::DwmGetCompositionTimingInfo;
	};

	class D3D11DisplayFrequency
	{
	public:

		// Queries the window's monitor, then DWM. Failure preserves the previous value.
		void update(HWND window, const D3D11DisplayFrequencyFunctions& functions = {});

		[[nodiscard]]
		double getHz() const noexcept
		{
			return m_hz;
		}

		[[nodiscard]]
		uint32 getOccludedSleepMillisec() const noexcept
		{
			return m_occludedSleepMillisec;
		}

	private:

		double m_hz = 60.0;

		uint32 m_occludedSleepMillisec = 15;

		// Reused across queries, including a retry when the display topology changes.
		Array<DISPLAYCONFIG_PATH_INFO> m_paths;

		Array<DISPLAYCONFIG_MODE_INFO> m_modes;

		[[nodiscard]]
		Optional<double> queryDisplayConfig(LPCWSTR deviceName, const D3D11DisplayFrequencyFunctions& functions);

		void setFrequency(double hz, const char* source);
	};
}
