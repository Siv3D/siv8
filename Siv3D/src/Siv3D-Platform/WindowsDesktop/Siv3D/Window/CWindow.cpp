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

# include "CWindow.hpp"
# include "DPIAwareness.hpp"
# include "WindowProc.hpp"
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	extern std::atomic_flag g_shouldDestroyWindow;

	CWindow::CWindow() = default;

	CWindow::~CWindow()
	{
		LOG_SCOPED_TRACE("CWindow::~CWindow()");

		if (m_hWnd)
		{
			// DestroyWindow() must be called from the main thread.
			g_shouldDestroyWindow.test_and_set();

			for (int32 i = 0; i < 100; ++i)
			{
				if (g_shouldDestroyWindow.test() == false)
				{
					break;
				}

				::Sleep(2);
			}
		}

		m_user32.unload();
	}

	void CWindow::init()
	{
		LOG_SCOPED_TRACE("CWindow::init()");

		//// hInstance を取得
		//m_hInstance = ::GetModuleHandleW(nullptr);

		//m_user32.load();

		//// DPI awareness を有効化
		//detail::SetDPIAwareness(m_user32.library);

		//m_windowClass.registerClass(m_hInstance);

		//// モニタを取得
		//const Array<MonitorInfo> monitors = System::EnumerateMonitors();

		//if (not monitors)
		//{
		//	throw EngineError(U"System::EnumActiveMonitors() failed");
		//}

		//for (auto [i, monitor] : Indexed(monitors))
		//{
		//	LOG_TRACE(U"🖥️ Monitor[{}]"_fmt(i) + monitor.format());
		//}
	}

	void CWindow::update()
	{

	}

	void CWindow::setWindowTitle(const String& title)
	{
		String newActualTitle = title;

		//if constexpr (SIV3D_BUILD(DEBUG))
		//{
		//	const String statistics = SIV3D_ENGINE(Profiler)->getSimpleStatistics();
		//	newActualTitle += U" (Debug Build) | ";
		//	newActualTitle += statistics;
		//}

		if (m_actualTitle != newActualTitle)
		{
			::SetWindowTextW(m_hWnd, newActualTitle.toWstr().c_str());
			m_actualTitle.swap(newActualTitle);
		}

		m_title = title;
	}

	const String& CWindow::getWindowTitle() const noexcept
	{
		return m_title;
	}

	void* CWindow::getHandle() const noexcept
	{
		return m_hWnd;
	}

	void CWindow::destroy()
	{
		LOG_SCOPED_TRACE("CWindow::destroy()");

		if (m_hWnd)
		{
			//m_taskbar.Reset();

			//if (m_deviceNotificationHandle)
			//{
			//	LOG_VERBOSE(U"UnregisterDeviceNotification()");
			//	::UnregisterDeviceNotification(m_deviceNotificationHandle);
			//	m_deviceNotificationHandle = nullptr;
			//}

			//LOG_VERBOSE(U"DestroyWindow()");
			//const BOOL b = ::DestroyWindow(m_hWnd);
			//LOG_VERBOSE(U"DestroyWindow() -> {}"_fmt(!!b));
		}

		m_windowClass.unregisterClass(m_hInstance);
	}

	void CWindow::User32Lib::load()
	{
		library = DLL::LoadSystemLibrary(L"user32.dll");
		pGetSystemMetricsForDpi = DLL::GetFunctionNoThrow(library, "GetSystemMetricsForDpi");
		pAdjustWindowRectExForDpi = DLL::GetFunctionNoThrow(library, "AdjustWindowRectExForDpi");
	}

	void CWindow::User32Lib::unload()
	{
		DLL::Unload(library);
	}

	void CWindow::WindowClass::registerClass(const HINSTANCE hInstance)
	{
		// WindowClass の名前を生成
		name = FileSystem::ModulePath().toWstr();

		const WNDCLASSEX windowClass
		{
			.cbSize			= sizeof(WNDCLASSEX),
			.style			= (CS_HREDRAW | CS_VREDRAW),
			.lpfnWndProc	= WindowProc,
			.hInstance		= hInstance,
			.hIcon			= ::LoadIconW(hInstance, MAKEINTRESOURCEW(100)),
			.hCursor		= nullptr,
			.hbrBackground	= static_cast<HBRUSH>(::GetStockObject(DKGRAY_BRUSH)),
			.lpszClassName	= name.c_str(),
		};

		{
			LOG_TRACE(U"RegisterClassExW()");
		
			if (not ::RegisterClassExW(&windowClass))
			{
				throw EngineError{ "RegisterClassExW() failed" };
			}
		}
	}

	void CWindow::WindowClass::unregisterClass(const HINSTANCE hInstance)
	{
		LOG_TRACE(U"UnregisterClassW()");
		
		const BOOL b = ::UnregisterClassW(name.c_str(), hInstance);
		
		LOG_TRACE(U"UnregisterClassW() -> {}"_fmt(!!b));
	}
}
