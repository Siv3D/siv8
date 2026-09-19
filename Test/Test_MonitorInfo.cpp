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

# include <Siv3D/MonitorInfo/WindowsMonitor.hpp>
# include <cwchar>

namespace
{
	struct DisplayDevice
	{
		const wchar_t* name;
		DWORD flags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
		Array<DWORD> children{ DISPLAY_DEVICE_ACTIVE };
	};

	struct MonitorFixture
	{
		inline static MonitorFixture* active = nullptr;
		MonitorFixture* previous = active;

		HWND window = reinterpret_cast<HWND>(0x1234);
		HMONITOR monitor = reinterpret_cast<HMONITOR>(0x5678);
		const wchar_t* currentDeviceName = L"\\\\.\\DISPLAY2";
		BOOL monitorInfoResult = TRUE;
		size_t monitorInfoCalls = 0;
		size_t enumerationCalls = 0;
		Array<DisplayDevice> devices
		{
			{ L"\\\\.\\DISPLAY1" },
			{ L"\\\\.\\DISPLAY2", DISPLAY_DEVICE_ATTACHED_TO_DESKTOP | DISPLAY_DEVICE_PRIMARY_DEVICE },
			{ L"\\\\.\\DISPLAY3" },
		};

		MonitorFixture()
		{
			active = this;
		}

		~MonitorFixture()
		{
			active = previous;
		}

		static BOOL WINAPI EnumDisplayDevices(LPCWSTR name, DWORD index, PDISPLAY_DEVICEW output, DWORD flags)
		{
			auto& f = *active;
			++f.enumerationCalls;
			REQUIRE(output->cb == sizeof(DISPLAY_DEVICEW));

			if (not name)
			{
				CHECK(flags == 0);
				if (f.devices.size() <= index)
				{
					return FALSE;
				}

				const auto& device = f.devices[index];
				REQUIRE(::wcscpy_s(output->DeviceName, device.name) == 0);
				output->StateFlags = device.flags;
				return TRUE;
			}

			CHECK(flags == EDD_GET_DEVICE_INTERFACE_NAME);
			for (const auto& device : f.devices)
			{
				if (std::wcscmp(name, device.name) == 0)
				{
					CHECK((device.flags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) != 0);
					if (device.children.size() <= index)
					{
						return FALSE;
					}

					// Child names differ from the display name reported by GetMonitorInfoW.
					REQUIRE(::wcscpy_s(output->DeviceName, L"child-monitor") == 0);
					output->StateFlags = device.children[index];
					return TRUE;
				}
			}

			FAIL("Queried children of an unknown display device");
		}

		static HMONITOR WINAPI MonitorFromWindow(HWND window, DWORD flags)
		{
			auto& f = *active;
			CHECK(window == f.window);
			CHECK(flags == MONITOR_DEFAULTTOPRIMARY);
			return f.monitor;
		}

		static BOOL WINAPI GetMonitorInfo(HMONITOR monitor, LPMONITORINFO output)
		{
			auto& f = *active;
			++f.monitorInfoCalls;
			CHECK(monitor == f.monitor);
			REQUIRE(output->cbSize == sizeof(MONITORINFOEXW));
			if (f.monitorInfoResult)
			{
				auto* extended = static_cast<MONITORINFOEXW*>(output);
				REQUIRE(::wcscpy_s(extended->szDevice, f.currentDeviceName) == 0);
			}
			return f.monitorInfoResult;
		}

		size_t currentIndex()
		{
			const WindowsMonitor::Functions functions
			{
				.enumDisplayDevices = EnumDisplayDevices,
				.monitorFromWindow = MonitorFromWindow,
				.getMonitorInfo = GetMonitorInfo,
			};
			return WindowsMonitor::GetCurrentMonitorIndex(window, functions);
		}

		Array<std::wstring> enumerate(bool expectFallback = false)
		{
			Array<std::wstring> names;
			WindowsMonitor::EnumerateMonitorDevices([&](const DISPLAY_DEVICEW& device, const DISPLAY_DEVICEW* child)
			{
				CHECK((child == nullptr) == expectFallback);
				names.emplace_back(device.DeviceName);
				return true;
			}, EnumDisplayDevices);
			return names;
		}

		void removeChildren()
		{
			for (auto& device : devices)
			{
				device.children.clear();
			}
		}
	};
}

TEST_CASE("MonitorInfo.Windows.current_index_matches_normal_enumeration")
{
	MonitorFixture f;
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY1", L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate() == expected);
	for (size_t i = 0; i < expected.size(); ++i)
	{
		f.currentDeviceName = expected[i].c_str();
		CHECK(f.currentIndex() == i);
	}
}

TEST_CASE("MonitorInfo.Windows.current_index_matches_fallback_enumeration")
{
	MonitorFixture f;
	f.removeChildren();
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY1", L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate(true) == expected);
	for (size_t i = 0; i < expected.size(); ++i)
	{
		f.currentDeviceName = expected[i].c_str();
		CHECK(f.currentIndex() == i); // Includes a primary display at index 1.
	}
}

TEST_CASE("MonitorInfo.Windows.filters_detached_inactive_and_mirroring_devices")
{
	MonitorFixture f;
	f.devices[0].flags = 0; // Its active child must not be enumerated.
	f.devices[1].children = { 0, DISPLAY_DEVICE_ACTIVE | DISPLAY_DEVICE_MIRRORING_DRIVER, DISPLAY_DEVICE_ACTIVE };
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate() == expected);
	f.currentDeviceName = expected[1].c_str();
	CHECK(f.currentIndex() == 1);
}

TEST_CASE("MonitorInfo.Windows.fallback_skips_detached_devices")
{
	MonitorFixture f;
	f.removeChildren();
	f.devices[0].flags = 0;
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate(true) == expected);
	f.currentDeviceName = expected[1].c_str();
	CHECK(f.currentIndex() == 1);
}

TEST_CASE("MonitorInfo.Windows.falls_back_when_all_children_are_filtered")
{
	MonitorFixture f;
	for (auto& device : f.devices)
	{
		device.children = { 0, DISPLAY_DEVICE_ACTIVE | DISPLAY_DEVICE_MIRRORING_DRIVER };
	}
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY1", L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate(true) == expected);
	CHECK(f.currentIndex() == 1);
}

TEST_CASE("MonitorInfo.Windows.partial_normal_results_do_not_mix_in_fallback_entries")
{
	MonitorFixture f;
	f.devices[0].children.clear();
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate() == expected);
	CHECK(f.currentIndex() == 0);
	f.currentDeviceName = expected[1].c_str();
	CHECK(f.currentIndex() == 1);
	f.currentDeviceName = f.devices[0].name; // Missing from the public list.
	CHECK(f.currentIndex() == 0);
}

TEST_CASE("MonitorInfo.Windows.counts_each_active_child_in_the_public_order")
{
	MonitorFixture f;
	f.devices[0].children = { DISPLAY_DEVICE_ACTIVE, DISPLAY_DEVICE_ACTIVE };
	const Array<std::wstring> expected{ L"\\\\.\\DISPLAY1", L"\\\\.\\DISPLAY1", L"\\\\.\\DISPLAY2", L"\\\\.\\DISPLAY3" };
	CHECK(f.enumerate() == expected);
	CHECK(f.currentIndex() == 2);
	f.currentDeviceName = f.devices[0].name;
	CHECK(f.currentIndex() == 0); // Use the first entry when children share a display.
}

TEST_CASE("MonitorInfo.Windows.unavailable_or_unlisted_current_monitor_returns_zero")
{
	MonitorFixture f;
	SECTION("no monitor handle")
	{
		f.monitor = nullptr;
		CHECK(f.currentIndex() == 0);
		CHECK(f.monitorInfoCalls == 0);
		CHECK(f.enumerationCalls == 0);
	}
	SECTION("monitor information query fails")
	{
		f.monitorInfoResult = FALSE;
		CHECK(f.currentIndex() == 0);
		CHECK(f.monitorInfoCalls == 1);
		CHECK(f.enumerationCalls == 0);
	}
	SECTION("current display is absent")
	{
		f.currentDeviceName = L"missing-display";
		CHECK(f.currentIndex() == 0);
		f.removeChildren();
		CHECK(f.currentIndex() == 0);
	}
}

TEST_CASE("MonitorInfo.Windows.empty_monitor_list_returns_zero")
{
	MonitorFixture f;
	SECTION("no display devices")
	{
		f.devices.clear();
	}
	SECTION("all devices detached")
	{
		for (auto& device : f.devices)
		{
			device.flags = 0;
		}
	}
	CHECK(f.enumerate().empty());
	CHECK(f.currentIndex() == 0);
}

# endif
