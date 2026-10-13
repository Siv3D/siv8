//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_PLATFORM(WINDOWS)
# include <atomic>
# include <semaphore>
# include <thread>
# include <Siv3D/Window/WindowStateSync.hpp>
# include <Siv3D/Window/WindowProc.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer/D3D11/CRenderer_D3D11.hpp>
# include <dwmapi.h>

namespace s3d
{
	namespace
	{
		HWND MainWindow()
		{
			return static_cast<HWND>(Platform::Windows::Window::GetHWND());
		}

		template <class F>
		void NativeCommand(F function)
		{
			WindowThreadCommand command{ [](void* context) { (*static_cast<F*>(context))(); }, &function, {} };
			::SendMessageW(MainWindow(), WindowThreadMessage, 0, reinterpret_cast<LPARAM>(&command));
			command.wait(); // The runner also bounds the entire native test process.
			if (command.exception) { std::rethrow_exception(command.exception); }
		}

		struct RestoreWindow
		{
			const WindowState state = Window::GetState();
			const SceneConfig config = Scene::GetConfig();

			~RestoreWindow()
			{
				Window::Restore();
				Window::SetMinimumFrameBufferSize(state.minFrameBufferSize);
				Window::SetStyle(state.style);
				Window::SetPos(state.bounds.pos);
				Window::ResizeActual(state.frameBufferSize, CenterWindow::No);
				if (state.maximized) { Window::Maximize(); }
				if (state.minimized) { Window::Minimize(); }
				Scene::Configure(config);
			}
		};

		void CheckNativeGeometry()
		{
			const auto& state = Window::GetState();
			RECT bounds{}, client{};
			REQUIRE(SUCCEEDED(::DwmGetWindowAttribute(MainWindow(), DWMWA_EXTENDED_FRAME_BOUNDS, &bounds, sizeof(bounds))));
			REQUIRE(::GetClientRect(MainWindow(), &client));
			CHECK(state.bounds == Rect{ bounds.left, bounds.top, bounds.right - bounds.left, bounds.bottom - bounds.top });
			CHECK(state.scaling == (::GetDpiForWindow(MainWindow()) / 96.0));
			CHECK(state.minimized == (::IsIconic(MainWindow()) != FALSE));
			CHECK(state.maximized == (::IsZoomed(MainWindow()) != FALSE));
			if (not state.minimized)
			{
				CHECK(state.frameBufferSize == Size{ client.right, client.bottom });
				CHECK(state.virtualSize == Math::Round(state.frameBufferSize / state.scaling).asPoint());
			}
		}

		WindowState Generation(const int32 index)
		{
			WindowState state;
			state.bounds = { index, -index, 1000 + index, 700 + index };
			state.virtualSize = { 400 + index, 300 + index };
			state.scaling = ((index % 2) ? 2.0 : 1.0);
			state.frameBufferSize = (state.virtualSize * state.scaling).asPoint();
			state.frameThickness = { index, index + 1 };
			state.titleBarHeight = (20 + index);
			state.minFrameBufferSize = { index + 1, index + 2 };
			state.style = ((index % 2) ? WindowStyle::Sizable : WindowStyle::Fixed);
			state.fullscreen = ((index % 3) == 0);
			state.minimized = ((index % 3) == 1);
			state.maximized = ((index % 3) == 2);
			state.focused = ((index % 2) == 0);
			state.sizeMove = not state.focused;
			return state;
		}
	}

	TEST_CASE("WindowStateSync.handoff_is_complete_and_reference_is_stable")
	{
		// This tests the handoff, not Win32 callbacks. Force reads between the
		// outer and nested writes, rather than hoping a stress loop races them.
		WindowStateSync sync;
		const auto& reference = sync.get();
		const auto* address = &reference;
		WindowState expected;
		std::binary_semaphore written{ 0 }, consumed{ 0 };
		std::atomic<bool> timedOut = false;
		std::jthread writer{ [&]
		{
			WindowState pending;
			for (int32 i = 1; i <= 32; ++i)
			{
				sync.beginUpdate();
				pending.scaling = Generation(i).scaling;
				written.release();
				if (not consumed.try_acquire_for(std::chrono::seconds{ 5 })) { timedOut = true; return; }
				sync.beginUpdate();
				pending.frameBufferSize = Generation(i).frameBufferSize;
				sync.endUpdate(pending);
				written.release();
				if (not consumed.try_acquire_for(std::chrono::seconds{ 5 })) { timedOut = true; return; }
				pending = Generation(i);
				sync.endUpdate(pending);
				written.release();
				if (not consumed.try_acquire_for(std::chrono::seconds{ 5 })) { timedOut = true; return; }
			}
		} };
		for (int32 i = 1; i <= 32; ++i)
		{
			for (int32 phase = 0; phase < 3; ++phase)
			{
				CAPTURE(i, phase);
				REQUIRE(written.try_acquire_for(std::chrono::seconds{ 5 }));
				CHECK(reference == expected); // Commit alone never mutates a retained public reference.
				sync.synchronize();
				if (phase == 2) { expected = Generation(i); }
				CHECK(reference == expected);
				CHECK(&sync.get() == address);
				consumed.release();
			}
		}
		writer.join();
		CHECK_FALSE(timedOut.load());
	}

	TEST_CASE("WindowStateSync.native_notifications_then_explicit_api")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindow restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		REQUIRE(Window::ResizeActual(610, 410, CenterWindow::No));
		const auto& reference = Window::GetState();
		const WindowState before = reference;
		const auto* address = &reference;
		DWORD callbackThread = 0;
		bool resized = false;
		NativeCommand([&]
		{
			callbackThread = ::GetCurrentThreadId();
			RECT outer{}, client{};
			::GetWindowRect(MainWindow(), &outer);
			::GetClientRect(MainWindow(), &client);
			resized = (::SetWindowPos(MainWindow(), nullptr, 0, 0,
				outer.right - outer.left + 30, outer.bottom - outer.top + 20,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE) != FALSE);
			::SendMessageW(MainWindow(), WM_ENTERSIZEMOVE, 0, 0);
		});
		REQUIRE(resized);
		CHECK(callbackThread == ::GetWindowThreadProcessId(MainWindow(), nullptr));
		CHECK(callbackThread != ::GetCurrentThreadId());
		CHECK(reference == before);
		// An explicit policy edit must preserve the newer OS geometry and flags.
		Window::SetMinimumFrameBufferSize({ 120, 90 });
		CHECK(&Window::GetState() == address);
		CHECK(reference.frameBufferSize == Size{ 640, 430 });
		CHECK(reference.minFrameBufferSize == Size{ 120, 90 });
		CHECK(reference.sizeMove);
		CheckNativeGeometry();
		const auto committed = reference;
		Scene::Configure(SceneConfig::Window(SceneResolution::Display));
		const Size previousScene = Scene::Size();
		NativeCommand([&]
		{
			RECT outer{};
			::GetWindowRect(MainWindow(), &outer);
			::SetWindowPos(MainWindow(), nullptr, 0, 0,
				outer.right - outer.left + 10, outer.bottom - outer.top + 10,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			::SendMessageW(MainWindow(), WM_EXITSIZEMOVE, 0, 0);
		});
		CHECK(reference == committed);
		CHECK(Scene::Size() == previousScene);
		REQUIRE(System::Update());
		CHECK_FALSE(reference.sizeMove);
		CHECK(reference.frameBufferSize == Size{ 650, 440 });
		CHECK(reference.minFrameBufferSize == committed.minFrameBufferSize);
		CHECK(Scene::Size() == reference.virtualSize);
		const auto& buffer = static_cast<CRenderer_D3D11*>(SIV3D_ENGINE(Renderer))->getBackBuffer();
		CHECK(buffer.getBackBufferSize() == reference.frameBufferSize);
		const auto [scale, area] = buffer.getLetterboxComposition();
		CHECK(Cursor::GetBaseWindowTransform() == Mat3x2::Scale(scale).translated(area.pos));
		CheckNativeGeometry();
	}

	TEST_CASE("WindowStateSync.native_reentrant_command_and_dpi_notification")
	{
		REQUIRE(System::Update());
		const RestoreWindow restore;
		Window::Restore();
		Window::SetStyle(WindowStyle::Sizable);
		REQUIRE(Window::ResizeActual(620, 420, CenterWindow::No));
		bool nestedRan = false, ranInsideOuter = false;
		WindowThreadCommand nested{ [](void* context) { *static_cast<bool*>(context) = true; }, &nestedRan, {} };
		NativeCommand([&]
		{
			// A private command arriving inside another native operation must wait
			// for its outer state boundary without blocking the HWND thread.
			::SendMessageW(MainWindow(), WindowThreadMessage, 0, reinterpret_cast<LPARAM>(&nested));
			ranInsideOuter = nestedRan;
			RECT bounds{};
			::GetWindowRect(MainWindow(), &bounds);
			const auto dpi = ::GetDpiForWindow(MainWindow());
			// Exercise WM_DPICHANGED -> SetWindowPos -> nested native notifications.
			// The DPI is unchanged: this is reentry coverage, not mixed-DPI coverage.
			::SendMessageW(MainWindow(), WM_DPICHANGED, MAKEWPARAM(dpi, dpi), reinterpret_cast<LPARAM>(&bounds));
		});
		CHECK_FALSE(ranInsideOuter);
		CHECK(nestedRan);
		Window::SetMinimumFrameBufferSize(restore.state.minFrameBufferSize);
		CheckNativeGeometry();
		CHECK(Window::GetState().frameBufferSize == Size{ 620, 420 });
		// Exceptions stay on the initiating thread and do not poison later operations.
		CHECK_THROWS_AS(NativeCommand([] { throw std::runtime_error{ "native command" }; }), std::runtime_error);
		REQUIRE(Window::ResizeActual(630, 430, CenterWindow::No));
		CheckNativeGeometry();
	}

	TEST_CASE("WindowStateSync.resize_and_placement_are_immediate")
	{
		REQUIRE(System::Update());
		REQUIRE_FALSE(Window::GetState().fullscreen);
		const RestoreWindow restore;
		Window::Restore();
		Window::SetMinimumFrameBufferSize({ 1, 1 });
		const auto* address = &Window::GetState();
		using ResizeFunction = bool (*)(Size, CenterWindow);
		const ResizeFunction resizeFunctions[] = { Window::Resize, Window::ResizeVirtual, Window::ResizeActual };
		for (const auto style : { WindowStyle::Fixed, WindowStyle::Sizable, WindowStyle::Frameless })
		{
			Window::SetStyle(style);
			CHECK(Window::GetStyle() == style);
			for (int32 i = 0; i < 3; ++i)
			{
				CAPTURE(style, i);
				const Size requested{ 500 + 10 * i, 350 + 10 * i };
				REQUIRE(resizeFunctions[i](requested, CenterWindow::No));
				CHECK((i == 2 ? Window::GetState().frameBufferSize : Window::GetState().virtualSize) == requested);
				CheckNativeGeometry();
				const auto immediate = Window::GetState();
				REQUIRE(System::Update());
				CHECK(Window::GetState().frameBufferSize == immediate.frameBufferSize);
				CHECK(Window::GetState().bounds == immediate.bounds);
				CHECK(&Window::GetState() == address);
			}
		}
		Window::SetStyle(WindowStyle::Sizable);
		const auto monitors = System::EnumerateMonitors();
		bool mixedDPI = false;
		for (size_t i = 0; i < monitors.size(); ++i)
		{
			Catch::cout() << "placement monitor[" << i << "] " << monitors[i].format().toUTF8() << '\n';
			mixedDPI |= (monitors[i].scaling != monitors.front().scaling);
			const auto& work = monitors[i].workArea;
			// This is the intermediate read used by cross-DPI CenterToScreen().
			Window::SetPos(work.pos);
			CheckNativeGeometry();
			CHECK(System::GetCurrentMonitorIndex() == i);
			const auto beforeCenter = Window::GetState();
			REQUIRE(Window::CenterToScreen(i));
			const auto centered = Window::GetState();
			CheckNativeGeometry();
			const Point requestedCenter = work.pos + (work.size - beforeCenter.bounds.size) / 2;
			CHECK(centered.bounds.pos == requestedCenter);
			Window::SetPos(requestedCenter);
			CAPTURE(i, work, beforeCenter.bounds, beforeCenter.virtualSize, beforeCenter.frameBufferSize,
				Window::GetState().bounds, Window::GetState().virtualSize, Window::GetState().frameBufferSize,
				requestedCenter);
			CHECK(Window::GetState().bounds == centered.bounds);
			CHECK(Window::GetState().frameBufferSize == centered.frameBufferSize);
			CheckNativeGeometry();
			System::Sleep(500);
			REQUIRE(System::Update());
			CheckNativeGeometry();
			REQUIRE(Window::CenterToScreen());
			CheckNativeGeometry();
		}
		if (not mixedDPI) { WARN("No mixed-DPI monitors: placement checked only at the available DPI."); }
	}
}
# endif
