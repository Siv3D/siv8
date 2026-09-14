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

# include "D3D11DisplayFrequency.hpp"
# include <Siv3D/EngineLog.hpp>
# include <algorithm>
# include <cwchar>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Optional<double> ToFrequency(const uint32 numerator, const uint32 denominator)
		{
			if ((numerator == 0) || (denominator == 0))
			{
				return none;
			}

			return (static_cast<double>(numerator) / denominator);
		}
	}

	void D3D11DisplayFrequency::update(const HWND window, const D3D11DisplayFrequencyFunctions& functions)
	{
		// The HWND identifies the display even when the rendering adapter has no DXGI output (e.g. WARP).
		const HMONITOR monitor = functions.monitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEXW monitorInfo{};
		monitorInfo.cbSize = sizeof(monitorInfo);
		if (monitor && functions.getMonitorInfo(monitor, &monitorInfo))
		{
			if (const auto hz = queryDisplayConfig(monitorInfo.szDevice, functions))
			{
				setFrequency(*hz, "QueryDisplayConfig");
				return;
			}

			DEVMODEW mode{};
			mode.dmSize = sizeof(mode);
			if (functions.enumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &mode)
				&& (mode.dmFields & DM_DISPLAYFREQUENCY)
				&& (mode.dmDisplayFrequency > 1))
			{
				// 0 and 1 mean the hardware default, not a measured frequency.
				setFrequency(mode.dmDisplayFrequency, "EnumDisplaySettingsW");
				return;
			}
		}

		DWM_TIMING_INFO timing{};
		timing.cbSize = sizeof(timing);
		if (SUCCEEDED(functions.dwmGetCompositionTimingInfo(nullptr, &timing)))
		{
			if (const auto hz = ToFrequency(timing.rateRefresh.uiNumerator, timing.rateRefresh.uiDenominator))
			{
				setFrequency(*hz, "DWM fallback");
				return;
			}
		}

		LOG_WARN(fmt::format("Display refresh rate query failed; keeping {:.2f} Hz", m_hz));
	}

	Optional<double> D3D11DisplayFrequency::queryDisplayConfig(
		const LPCWSTR deviceName, const D3D11DisplayFrequencyFunctions& functions)
	{
		// Use the Windows 10-compatible view of active paths. This is a configured rate, not instantaneous VRR.
		constexpr uint32 Flags = QDC_ONLY_ACTIVE_PATHS;

		// A topology change can invalidate the buffer sizes. Retry once, then use another query method.
		for (uint32 attempt = 0; attempt < 2; ++attempt)
		{
			uint32 pathCount = 0, modeCount = 0;
			if (functions.getDisplayConfigBufferSizes(Flags, &pathCount, &modeCount) != ERROR_SUCCESS)
			{
				return none;
			}

			if ((pathCount == 0) || (modeCount == 0))
			{
				return none;
			}

			m_paths.resize(pathCount);
			m_modes.resize(modeCount);
			const LONG result = functions.queryDisplayConfig(
				Flags, &pathCount, m_paths.data(), &modeCount, m_modes.data(), nullptr);

			if (result == ERROR_INSUFFICIENT_BUFFER)
			{
				continue;
			}

			if (result != ERROR_SUCCESS)
			{
				return none;
			}

			Optional<double> frequency;
			for (uint32 i = 0; i < pathCount; ++i)
			{
				const auto& path = m_paths[i];
				DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
				source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
				source.header.size = sizeof(source);
				source.header.adapterId = path.sourceInfo.adapterId;
				source.header.id = path.sourceInfo.id;
				if ((functions.displayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS)
					|| (std::wcscmp(deviceName, source.viewGdiDeviceName) != 0))
				{
					continue;
				}

				const auto hz = ToFrequency(path.targetInfo.refreshRate.Numerator, path.targetInfo.refreshRate.Denominator);
				if (not hz)
				{
					return none;
				}

				// Cloned targets share a GDI source name. Do not choose an arbitrary target if their rates differ.
				if (frequency && (*frequency != *hz))
				{
					return none;
				}
				frequency = hz;
			}

			return frequency;
		}

		return none;
	}

	void D3D11DisplayFrequency::setFrequency(const double hz, const char* source)
	{
		m_hz = hz;
		// Bound the occlusion throttle to 1..1000 ms. The input is a positive, finite integer ratio.
		m_occludedSleepMillisec = static_cast<uint32>(std::clamp((900.0 / hz), 1.0, 1000.0));
		LOG_INFO(fmt::format("ℹ️ Display refresh rate: {:.2f} Hz ({})", m_hz, source));
	}
}
