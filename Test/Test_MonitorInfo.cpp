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
# include <Siv3D/Window/CWindow.hpp>
# include <Siv3D/Window/WindowMisc.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
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
		size_t snapshotCalls = 0;
		Array<MonitorInfo> snapshot;
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

		WindowsMonitor::Functions functions()
		{
			return
			{
				.enumDisplayDevices = EnumDisplayDevices,
				.monitorFromWindow = MonitorFromWindow,
				.getMonitorInfo = GetMonitorInfo,
				.enumerateMonitors = []
				{
					++active->snapshotCalls;
					return active->snapshot;
				},
			};
		}

		size_t currentIndex()
		{
			return WindowsMonitor::GetCurrentMonitorIndex(window, functions());
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

		void takeSnapshot(const bool fallback = false)
		{
			snapshot.clear();
			for (const auto& name : enumerate(fallback))
			{
				MonitorInfo info;
				info.displayDeviceName = Unicode::FromWstring(name);
				snapshot.push_back(std::move(info));
			}
			enumerationCalls = 0;
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

TEST_CASE("MonitorInfo.Windows.snapshot_resolution_preserves_identity")
{
	MonitorFixture f;
	SECTION("normal") { f.takeSnapshot(); }
	SECTION("fallback") { f.removeChildren(); f.takeSnapshot(true); }
	SECTION("multiple active children")
	{
		f.devices[0].children.push_back(DISPLAY_DEVICE_ACTIVE);
		f.takeSnapshot();
	}
	SECTION("later enumeration reordered")
	{
		f.takeSnapshot();
		std::swap(f.devices[0], f.devices[1]);
	}
	SECTION("later enumeration grew")
	{
		f.takeSnapshot();
		f.devices.insert(f.devices.begin(), DisplayDevice{ L"new-display" });
	}
	SECTION("later enumeration shrank")
	{
		f.takeSnapshot();
		f.devices.erase(f.devices.begin());
	}
	for (size_t i = 0; i < f.snapshot.size(); ++i)
	{
		const auto name = f.snapshot[i].displayDeviceName.toWstr();
		f.currentDeviceName = name.c_str();
		const auto index = WindowsMonitor::FindCurrentMonitorIndex(f.window, f.snapshot, f.functions());
		REQUIRE(index);
		// When active children share one display, the first matching entry wins.
		const size_t first = ((i == 1) && (f.snapshot[0].displayDeviceName == f.snapshot[1].displayDeviceName)) ? 0 : i;
		CHECK(*index == first);
		CHECK(WindowsMonitor::GetCurrentMonitor(f.window, f.functions()).displayDeviceName == f.snapshot[i].displayDeviceName);
	}
	CHECK(f.enumerationCalls == 0);
	CHECK(f.snapshotCalls == f.snapshot.size()); // One snapshot per information query.
}

TEST_CASE("MonitorInfo.Windows.snapshot_resolution_failure_is_explicit")
{
	MonitorFixture f;
	f.takeSnapshot();
	SECTION("empty snapshot") { f.snapshot.clear(); }
	SECTION("current display disappeared") { f.currentDeviceName = L"unlisted-display"; }
	SECTION("no native monitor") { f.monitor = nullptr; }
	SECTION("native information unavailable") { f.monitorInfoResult = FALSE; }
	CHECK_FALSE(WindowsMonitor::FindCurrentMonitorIndex(f.window, f.snapshot, f.functions()));
	CHECK_THROWS_AS(WindowsMonitor::GetCurrentMonitor(f.window, f.functions()), Error);
	CHECK(f.enumerationCalls == 0);
}

namespace
{
	// Only monitor input is replaced. The caller, HWND command path and Win32
	// mutations are real; no Catch2 assertions execute on the HWND thread.
	struct MonitorSnapshot
	{
		inline static MonitorSnapshot* active = nullptr;
		MonitorSnapshot* previous = active;
		Array<MonitorInfo> monitors = System::EnumerateMonitors();
		WindowsMonitor::Functions functions;

		MonitorSnapshot()
		{
			active = this;
			functions.enumerateMonitors = [] { return active->monitors; };
		}
		~MonitorSnapshot() { active = previous; }
	};
}

TEST_CASE("MonitorInfo.Windows.failed_resolution_preserves_window_and_recovers")
{
	REQUIRE(System::Update());
	REQUIRE_FALSE(Window::GetState().fullscreen);
	auto& window = *static_cast<CWindow*>(SIV3D_ENGINE(Window));
	window.show();
	const auto original = Window::GetState();
	const auto config = Scene::GetConfig();
	const ScopeExit restore{ [&] { Window::SetPos(original.bounds.pos); } };
	const HWND hwnd = static_cast<HWND>(window.getHandle());
	const LONG_PTR style = ::GetWindowLongPtrW(hwnd, GWL_STYLE);
	const Size scene = Scene::Size(), render = Scene::RenderResolution();
	MonitorSnapshot input;
	const auto realMonitors = input.monitors;
	REQUIRE_FALSE(realMonitors.isEmpty());
	SECTION("empty list") { input.monitors.clear(); }
	SECTION("native current display is absent")
	{
		for (auto& monitor : input.monitors) { monitor.displayDeviceName = U"not-an-attached-display"; }
	}
	const size_t invalid = std::numeric_limits<size_t>::max();
	// Exercise the actual CWindow entry used by the public fullscreen API.
	window.setFullscreen(true, invalid, input.functions);
	CHECK_FALSE(Window::GetState().fullscreen);
	CHECK(Window::GetState().bounds == original.bounds);
	CHECK(Window::GetState().frameBufferSize == original.frameBufferSize);
	CHECK(::GetWindowLongPtrW(hwnd, GWL_STYLE) == style);
	CHECK(Scene::GetConfig() == config);
	CHECK(Scene::Size() == scene);
	CHECK(Scene::RenderResolution() == render);
	CHECK_FALSE(WindowMisc::CenterToScreen(window, none, input.functions));
	CHECK_FALSE(WindowMisc::CenterToScreen(window, 0, input.functions));
	CHECK(Window::GetState().bounds == original.bounds);
	CHECK_THROWS_AS(WindowsMonitor::GetCurrentMonitor(hwnd, input.functions), Error);

	// Check the saved rectangle at the native helper's mutation boundary, too.
	RECT saved{ 11, 22, 33, 44 };
	bool changed = true;
	auto operation = [&] { changed = WindowMisc::SetFullscreen(hwnd, invalid, saved,
		static_cast<uint32>(style), input.functions); };
	WindowThreadCommand command{ [](void* context) { (*static_cast<decltype(operation)*>(context))(); }, &operation, {} };
	::SendMessageW(hwnd, WindowThreadMessage, 0, reinterpret_cast<LPARAM>(&command));
	command.wait();
	CHECK_FALSE(changed);
	CHECK_FALSE(command.exception);
	CHECK(saved.left == 11);
	CHECK(saved.top == 22);
	CHECK(saved.right == 33);
	CHECK(saved.bottom == 44);
	CHECK(::GetWindowLongPtrW(hwnd, GWL_STYLE) == style);

	input.monitors = realMonitors;
	const auto expectedCurrent = System::GetCurrentMonitor();
	std::rotate(input.monitors.begin(), input.monitors.begin() + 1, input.monitors.end());
	REQUIRE(WindowMisc::CenterToScreen(window, none, input.functions));
	CHECK(System::GetCurrentMonitor().displayDeviceName == expectedCurrent.displayDeviceName);
	const auto resolvedBounds = Window::GetState().bounds;
	Window::SetPos(expectedCurrent.workArea.pos + (expectedCurrent.workArea.size - resolvedBounds.size) / 2);
	CHECK(Window::GetState().bounds == resolvedBounds);
	const auto currentIndex = WindowsMonitor::FindCurrentMonitorIndex(hwnd, input.monitors, input.functions);
	REQUIRE(currentIndex);
	REQUIRE(WindowMisc::CenterToScreen(window, *currentIndex, input.functions));
	REQUIRE(Window::CenterToScreen());
	const auto current = System::GetCurrentMonitor();
	MONITORINFOEXW native{};
	native.cbSize = sizeof(native);
	REQUIRE(::GetMonitorInfoW(::MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &native));
	CHECK(current.displayDeviceName == Unicode::FromWstring(native.szDevice));
	const auto centered = Window::GetState();
	Window::SetPos(current.workArea.pos + (current.workArea.size - centered.bounds.size) / 2);
	CHECK(Window::GetState().bounds == centered.bounds);
	CHECK_FALSE(Window::CenterToScreen(invalid));
	CHECK(Window::GetState().bounds == centered.bounds);
}

# endif
