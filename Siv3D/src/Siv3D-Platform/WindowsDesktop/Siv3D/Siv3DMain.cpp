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

# include <atomic>
# include <Siv3D/Common.hpp>
# include <Siv3D/AsyncTask.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/ScopeExit.hpp>
# include <Siv3D/System/CSystem.hpp>
# include <Siv3D/Window/CWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/FreestandingMessageBox/FreestandingMessageBox.hpp>
# include <Siv3D/System/ExitCode.hpp>
# include "Siv3DMainHelper.hpp"

// The window thread creates/destroys the HWND and pumps messages while the
// engine thread initializes Direct3D, runs Main(), and releases the engine.

namespace s3d
{
	std::atomic_flag g_shouldDestroyWindow;

	namespace
	{
		std::atomic_flag g_shouldInitWindow;

		////////////////////////////////////////////////////////////////
		//
		//	MainThread
		//
		////////////////////////////////////////////////////////////////

		static bool MainThread(std::future<bool> windowInitialized)
		{
			if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
			{
				::OutputDebugStringW(L"CoInitializeEx() failed\n");
				FreestandingMessageBox::ShowError("CoInitializeEx() failed");
				return false;
			}

			ScopeExit coUninitialize = [] { ::CoUninitialize(); };

			try
			{
				Siv3DEngine engine;

				if (auto pCSystem = dynamic_cast<CSystem*>(SIV3D_ENGINE(System)))
				{
					pCSystem->preInit();
				}

				g_shouldInitWindow.test_and_set();
				if (not windowInitialized.get())
				{
					return false;
				}

				SIV3D_ENGINE(System)->init();
				MainSEH();
				return true;
			}
			catch (const std::exception& error)
			{
				::OutputDebugStringW((Unicode::ToWstring(error.what()) + L'\n').c_str());
				FreestandingMessageBox::ShowError(error.what());
				return false;
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	DestroyWindow
		//
		////////////////////////////////////////////////////////////////

		static void DestroyWindow()
		{
			if (auto pCWindow = dynamic_cast<CWindow*>(SIV3D_ENGINE(Window)))
			{
				pCWindow->destroy();
			}

			g_shouldDestroyWindow.clear();
			g_shouldDestroyWindow.notify_one();
		}
	}

	namespace detail::init
	{
		void InitCommandLines(int argc, char** argv);
	}
}

////////////////////////////////////////////////////////////////
//
//	WinMain
//
////////////////////////////////////////////////////////////////

// Windows アプリケーションのエントリーポイント | Entry point for Windows applications
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	using namespace s3d;

	detail::init::InitCommandLines(__argc, __argv);

	SetWorkingDirectory();

	if (not WinMainInit())
	{
		return -1;
	}

	ScopeExit cleanup = [] { WinMainExit(); };

	std::promise<bool> windowInitialized;
	AsyncTask<bool> mainThread = Async(MainThread, windowInitialized.get_future());

	// There is no engine window to pump yet. Early worker failures also end this wait.
	while ((not g_shouldInitWindow.test()) && (not mainThread.isReady()))
	{
		::Sleep(1);
	}

	if (g_shouldInitWindow.test())
	{
		bool succeeded = false;
		try
		{
			SIV3D_ENGINE(Window)->init();
			succeeded = true;
		}
		catch (const std::exception& error)
		{
			::OutputDebugStringW((Unicode::ToWstring(error.what()) + L'\n').c_str());
		}
		windowInitialized.set_value(succeeded);
	}

	// DXGI initialization and teardown may synchronously send window messages.
	while (not mainThread.isReady())
	{
		PumpMessages();

		if (g_shouldDestroyWindow.test())
		{
			DestroyWindow();
		}

		::Sleep(1);
	}

	return (mainThread.get() ? GetExitCode() : -1);
}
