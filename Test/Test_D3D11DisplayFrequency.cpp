//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
//
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
//
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)

# include <Siv3D/Renderer/D3D11/SwapChain/D3D11DisplayFrequency.hpp>
# include <cmath>
# include <cwchar>

namespace
{
	struct DisplayPath
	{
		LUID adapter;
		uint32 sourceID;
		const wchar_t* name;
		DISPLAYCONFIG_RATIONAL rate;
		LONG nameResult = ERROR_SUCCESS;
	};

	struct DisplayFrequencyFixture
	{
		inline static DisplayFrequencyFixture* active = nullptr;
		DisplayFrequencyFixture* previous = active;

		HWND window = reinterpret_cast<HWND>(0x1234);
		HMONITOR monitor = reinterpret_cast<HMONITOR>(0x5678);
		const wchar_t* deviceName = L"\\\\.\\DISPLAY2";
		BOOL monitorInfoResult = TRUE;
		LONG sizeResult = ERROR_SUCCESS;
		LONG queryResult = ERROR_SUCCESS;
		uint32 insufficientBufferResults = 0;
		bool growOnRetry = false;
		bool emptyPaths = false;
		bool emptyModes = false;
		BOOL settingsResult = TRUE;
		DWORD settingsFields = DM_DISPLAYFREQUENCY;
		DWORD settingsFrequency = 72;
		HRESULT dwmResult = S_OK;
		UNSIGNED_RATIO dwmFrequency{ 120, 1 };
		Array<DisplayPath> paths
		{
			{ { 1, 2 }, 5, L"\\\\.\\DISPLAY1", { 144, 1 } },
			{ { 3, 4 }, 7, L"\\\\.\\DISPLAY2", { 60000, 1001 } },
		};

		uint32 monitorCalls = 0;
		uint32 monitorInfoCalls = 0;
		uint32 sizeCalls = 0;
		uint32 queryCalls = 0;
		uint32 sourceNameCalls = 0;
		uint32 settingsCalls = 0;
		uint32 dwmCalls = 0;
		Array<DISPLAYCONFIG_PATH_INFO*> pathBuffers;
		Array<DISPLAYCONFIG_MODE_INFO*> modeBuffers;
		D3D11DisplayFrequency frequency;

		DisplayFrequencyFixture()
		{
			active = this;
		}

		~DisplayFrequencyFixture()
		{
			active = previous;
		}

		static HMONITOR WINAPI MonitorFromWindow(HWND window, DWORD flags)
		{
			auto& f = *active;
			++f.monitorCalls;
			CHECK(window == f.window);
			CHECK(flags == MONITOR_DEFAULTTONEAREST);
			return f.monitor;
		}

		static BOOL WINAPI GetMonitorInfo(HMONITOR monitor, LPMONITORINFO info)
		{
			auto& f = *active;
			++f.monitorInfoCalls;
			CHECK(monitor == f.monitor);
			REQUIRE(info->cbSize == sizeof(MONITORINFOEXW));
			auto* extended = static_cast<MONITORINFOEXW*>(info);
			REQUIRE(::wcscpy_s(extended->szDevice, f.deviceName) == 0);
			return f.monitorInfoResult;
		}

		static LONG WINAPI GetDisplayConfigBufferSizes(UINT32 flags, UINT32* paths, UINT32* modes)
		{
			auto& f = *active;
			++f.sizeCalls;
			CHECK(flags == QDC_ONLY_ACTIVE_PATHS);
			// Estimates can be larger than the actual result.
			*paths = (f.emptyPaths ? 0 : static_cast<UINT32>(f.paths.size() + 2));
			*modes = (f.emptyModes ? 0 : 6);
			return f.sizeResult;
		}

		static LONG WINAPI QueryDisplayConfig(UINT32 flags, UINT32* pathCount, DISPLAYCONFIG_PATH_INFO* paths,
			UINT32* modeCount, DISPLAYCONFIG_MODE_INFO* modes, DISPLAYCONFIG_TOPOLOGY_ID* topology)
		{
			auto& f = *active;
			++f.queryCalls;
			CHECK(flags == QDC_ONLY_ACTIVE_PATHS);
			CHECK(topology == nullptr);
			REQUIRE(*pathCount >= f.paths.size());
			REQUIRE(*modeCount == 6);
			f.pathBuffers.push_back(paths);
			f.modeBuffers.push_back(modes);
			for (size_t i = 0; i < f.paths.size(); ++i)
			{
				paths[i] = {};
				paths[i].sourceInfo.adapterId = f.paths[i].adapter;
				paths[i].sourceInfo.id = f.paths[i].sourceID;
				paths[i].targetInfo.adapterId = { 99, 98 };
				paths[i].targetInfo.id = 97;
				paths[i].targetInfo.refreshRate = f.paths[i].rate;
				paths[i].flags = DISPLAYCONFIG_PATH_ACTIVE;
			}
			*pathCount = static_cast<UINT32>(f.paths.size());
			*modeCount = 0;
			if (f.insufficientBufferResults)
			{
				--f.insufficientBufferResults;
				if (f.growOnRetry)
				{
					// Simulate more paths than the original buffer can hold.
					for (uint32 id = 10; id < 13; ++id)
					{
						f.paths.push_back({ { 8, 9 }, id, L"\\\\.\\DISPLAY3", { 60, 1 } });
					}
				}
				return ERROR_INSUFFICIENT_BUFFER;
			}
			return f.queryResult;
		}

		static LONG WINAPI DisplayConfigGetDeviceInfo(DISPLAYCONFIG_DEVICE_INFO_HEADER* header)
		{
			auto& f = *active;
			++f.sourceNameCalls;
			CHECK(header->type == DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME);
			REQUIRE(header->size == sizeof(DISPLAYCONFIG_SOURCE_DEVICE_NAME));
			for (const auto& path : f.paths)
			{
				if ((header->adapterId.HighPart == path.adapter.HighPart)
					&& (header->adapterId.LowPart == path.adapter.LowPart)
					&& (header->id == path.sourceID))
				{
					auto* source = reinterpret_cast<DISPLAYCONFIG_SOURCE_DEVICE_NAME*>(header);
					REQUIRE(::wcscpy_s(source->viewGdiDeviceName, path.name) == 0);
					return path.nameResult;
				}
			}
			FAIL("The query used an unreturned path or a target ID instead of the source ID");
		}

		static BOOL WINAPI EnumDisplaySettings(LPCWSTR name, DWORD mode, DEVMODEW* settings)
		{
			auto& f = *active;
			++f.settingsCalls;
			CHECK(std::wcscmp(name, f.deviceName) == 0);
			CHECK(mode == ENUM_CURRENT_SETTINGS);
			CHECK(settings->dmSize == sizeof(DEVMODEW));
			settings->dmFields = f.settingsFields;
			settings->dmDisplayFrequency = f.settingsFrequency;
			return f.settingsResult;
		}

		static HRESULT WINAPI DwmGetCompositionTimingInfo(HWND window, DWM_TIMING_INFO* timing)
		{
			auto& f = *active;
			++f.dwmCalls;
			CHECK(window == nullptr);
			CHECK(timing->cbSize == sizeof(DWM_TIMING_INFO));
			timing->rateRefresh = f.dwmFrequency;
			// rateRefresh can be used directly, without dividing by a QPC period.
			timing->qpcRefreshPeriod = 0;
			return f.dwmResult;
		}

		void update()
		{
			const D3D11DisplayFrequencyFunctions functions
			{
				.monitorFromWindow = MonitorFromWindow,
				.getMonitorInfo = GetMonitorInfo,
				.getDisplayConfigBufferSizes = GetDisplayConfigBufferSizes,
				.queryDisplayConfig = QueryDisplayConfig,
				.displayConfigGetDeviceInfo = DisplayConfigGetDeviceInfo,
				.enumDisplaySettings = EnumDisplaySettings,
				.dwmGetCompositionTimingInfo = DwmGetCompositionTimingInfo,
			};
			frequency.update(window, functions);
		}
	};
}

TEST_CASE("D3D11DisplayFrequency.uses_fractional_rate_of_the_windows_monitor")
{
	DisplayFrequencyFixture f;
	f.update();
	CHECK(f.frequency.getHz() == Test::Approx(60000.0 / 1001));
	CHECK(f.frequency.getOccludedSleepMillisec() == 15);
	CHECK(f.sourceNameCalls == 2); // Not the larger buffer size estimate.
	CHECK(f.settingsCalls == 0);
	CHECK(f.dwmCalls == 0);
}

TEST_CASE("D3D11DisplayFrequency.requeries_the_monitor_and_reuses_storage")
{
	DisplayFrequencyFixture f;
	f.update();
	f.deviceName = L"\\\\.\\DISPLAY1";
	f.update();
	CHECK(f.frequency.getHz() == 144);
	CHECK(f.frequency.getOccludedSleepMillisec() == 6);
	CHECK(f.monitorCalls == 2);
	REQUIRE(f.pathBuffers.size() == 2);
	REQUIRE(f.modeBuffers.size() == 2);
	CHECK(f.pathBuffers[0] == f.pathBuffers[1]);
	CHECK(f.modeBuffers[0] == f.modeBuffers[1]);
}

TEST_CASE("D3D11DisplayFrequency.query_failures_fall_back_to_display_settings")
{
	for (const LONG result : { ERROR_ACCESS_DENIED, ERROR_NOT_SUPPORTED, ERROR_GEN_FAILURE })
	{
		for (const bool failSizing : { false, true })
		{
			DisplayFrequencyFixture f;
			(failSizing ? f.sizeResult : f.queryResult) = result;
			f.update();
			CHECK(f.frequency.getHz() == 72);
			CHECK(f.queryCalls == (failSizing ? 0u : 1u));
			CHECK(f.sourceNameCalls == 0);
			CHECK(f.settingsCalls == 1);
			CHECK(f.dwmCalls == 0);
		}
	}
}

TEST_CASE("D3D11DisplayFrequency.empty_display_configuration_falls_back")
{
	for (const bool noPaths : { false, true })
	{
		DisplayFrequencyFixture f;
		(noPaths ? f.emptyPaths : f.emptyModes) = true;
		f.update();
		CHECK(f.frequency.getHz() == 72);
		CHECK(f.queryCalls == 0);
	}
}

TEST_CASE("D3D11DisplayFrequency.topology_change_retries_once")
{
	DisplayFrequencyFixture f;
	f.insufficientBufferResults = 1;
	f.growOnRetry = true;
	f.update();
	CHECK(f.frequency.getHz() == Test::Approx(60000.0 / 1001));
	CHECK(f.sizeCalls == 2);
	CHECK(f.queryCalls == 2);
	CHECK(f.settingsCalls == 0);
	CHECK(f.sourceNameCalls == 5);
}

TEST_CASE("D3D11DisplayFrequency.repeated_topology_changes_fall_back_without_looping")
{
	DisplayFrequencyFixture f;
	f.insufficientBufferResults = 10;
	f.update();
	CHECK(f.frequency.getHz() == 72);
	CHECK(f.sizeCalls == 2);
	CHECK(f.queryCalls == 2);
	CHECK(f.sourceNameCalls == 0);
}

TEST_CASE("D3D11DisplayFrequency.unmatched_or_failed_source_names_fall_back")
{
	for (const bool nameFailure : { false, true })
	{
		DisplayFrequencyFixture f;
		if (nameFailure)
		{
			// A populated name must still be ignored after an API failure.
			f.paths[1].nameResult = ERROR_GEN_FAILURE;
		}
		else
		{
			f.paths[1].name = L"\\\\.\\DISPLAY3";
		}
		f.update();
		CHECK(f.frequency.getHz() == 72);
		CHECK(f.settingsCalls == 1);
	}
}

TEST_CASE("D3D11DisplayFrequency.cloned_displays_require_matching_rates")
{
	for (const bool matchingRates : { false, true })
	{
		DisplayFrequencyFixture f;
		f.paths[1].rate = { 60, 1 };
		auto clone = f.paths[1];
		clone.rate = { (matchingRates ? 60000u : 120000u), 1000 };
		f.paths.push_back(clone);
		f.update();
		CHECK(f.frequency.getHz() == (matchingRates ? 60 : 72));
		CHECK(f.settingsCalls == (matchingRates ? 0u : 1u));
	}
}

TEST_CASE("D3D11DisplayFrequency.invalid_path_ratios_fall_back")
{
	for (const DISPLAYCONFIG_RATIONAL rate : { DISPLAYCONFIG_RATIONAL{ 0, 1 }, { 60, 0 }, { 0, 0 } })
	{
		DisplayFrequencyFixture f;
		f.paths[1].rate = rate;
		f.update();
		CHECK(f.frequency.getHz() == 72);
		CHECK(f.settingsCalls == 1);
	}
}

TEST_CASE("D3D11DisplayFrequency.default_or_missing_display_settings_use_dwm")
{
	for (const DWORD frequency : { 0u, 1u, 60u })
	{
		DisplayFrequencyFixture f;
		f.sizeResult = ERROR_NOT_SUPPORTED;
		f.settingsFrequency = frequency;
		if (frequency == 60)
		{
			f.settingsFields = 0;
		}
		f.dwmFrequency = { 120000, 1001 };
		f.update();
		CHECK(f.frequency.getHz() == Test::Approx(120000.0 / 1001));
		CHECK(f.frequency.getOccludedSleepMillisec() == 7);
		CHECK(f.dwmCalls == 1);
	}
}

TEST_CASE("D3D11DisplayFrequency.failed_display_settings_ignore_populated_values")
{
	DisplayFrequencyFixture f;
	f.queryResult = ERROR_ACCESS_DENIED;
	f.settingsResult = FALSE;
	f.update();
	CHECK(f.frequency.getHz() == 120);
	CHECK(f.dwmCalls == 1);
}

TEST_CASE("D3D11DisplayFrequency.unavailable_monitor_uses_dwm")
{
	for (const bool missingMonitor : { false, true })
	{
		DisplayFrequencyFixture f;
		if (missingMonitor)
		{
			f.monitor = nullptr;
		}
		else
		{
			f.monitorInfoResult = FALSE;
		}
		f.update();
		CHECK(f.frequency.getHz() == 120);
		CHECK(f.monitorInfoCalls == (missingMonitor ? 0u : 1u));
		CHECK(f.sizeCalls == 0);
		CHECK(f.settingsCalls == 0);
		CHECK(f.dwmCalls == 1);
	}
}

TEST_CASE("D3D11DisplayFrequency.total_failure_preserves_default_or_previous_value")
{
	for (const bool havePreviousValue : { false, true })
	{
		DisplayFrequencyFixture f;
		if (havePreviousValue)
		{
			f.paths[1].rate = { 240, 1 };
			f.update();
		}
		f.sizeResult = ERROR_ACCESS_DENIED;
		f.settingsResult = FALSE;
		f.dwmResult = E_FAIL; // The populated 120/1 output must be ignored.
		f.update();
		CHECK(f.frequency.getHz() == (havePreviousValue ? 240 : 60));
		CHECK(f.frequency.getOccludedSleepMillisec() == (havePreviousValue ? 3u : 15u));
	}
}

TEST_CASE("D3D11DisplayFrequency.invalid_dwm_ratios_preserve_previous_value")
{
	for (const UNSIGNED_RATIO rate : { UNSIGNED_RATIO{ 0, 1 }, { 60, 0 }, { 0, 0 } })
	{
		DisplayFrequencyFixture f;
		f.paths[1].rate = { 144, 1 };
		f.update();
		f.monitor = nullptr;
		f.dwmFrequency = rate;
		f.update();
		CHECK(f.frequency.getHz() == 144);
		CHECK(f.frequency.getOccludedSleepMillisec() == 6);
	}
}

TEST_CASE("D3D11DisplayFrequency.occlusion_wait_is_bounded_at_query_time")
{
	for (const bool highRate : { false, true })
	{
		DisplayFrequencyFixture f;
		f.paths[1].rate = (highRate ? DISPLAYCONFIG_RATIONAL{ UINT32_MAX, 1 } : DISPLAYCONFIG_RATIONAL{ 1, UINT32_MAX });
		f.update();
		CHECK(f.frequency.getHz() > 0);
		CHECK(std::isfinite(f.frequency.getHz()));
		CHECK(f.frequency.getOccludedSleepMillisec() == (highRate ? 1u : 1000u));
		CHECK(f.settingsCalls == 0);
		CHECK(f.dwmCalls == 0);
	}
}

# endif
