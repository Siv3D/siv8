//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <condition_variable>
# include <mutex>
# include <Siv3D/Common.hpp>
# include <Siv3D/AsyncTask.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/System/CSystem.hpp>
# include <Siv3D/Window/CWindow.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Windows/Windows.hpp>
# include "Siv3DMainHelper.hpp"

//        [THREAD #0]                        [THREAD #1]
//                            
// WinMain() {                 
//   f = Async(MainThread)            || MainThread() {
//   **LOCK**                         ||   Siv3DEngine engine;
//                                    ||   engine->System->preInit();
//                                    <=   **LOCK**
//   engine->Window->init();          ||
//   **LOCK**                         =>
//                                    ||   engine->System->init();
//                                    <=
//   while (not f.isReady()) {        ||   Main() { while(System::Update()) {} } // User code
//     PumMessages();                 ||
//                                    ||   engine::~Siv3DEngine() {
//     if (g_callWindowDestroy) {     ||     ...
//       engine->Window->destroy();   ||     g_callWindowDestroy = true
//       g_callWindowDestroy = false; ||     |
//     }                              ||     wait until (g_callWindowDestroy == false)
//                                    ||     ...  
//     Sleep(1);                      ||   }
//   }                                || }  
// }

namespace s3d
{
	std::atomic_flag g_shouldDestroyWindow;

	namespace
	{
		std::condition_variable g_cv;

		std::mutex g_mutex;

		int32 g_initStep = 0;

		bool g_hasCriticalError = false;

		////////////////////////////////////////////////////////////////
		//
		//	MainThread
		//
		////////////////////////////////////////////////////////////////

		static void MainThread()
		{
			Siv3DEngine engine;

			std::unique_lock lock{ g_mutex }; // (1)--
			{
				if (auto pCSystem = dynamic_cast<CSystem*>(SIV3D_ENGINE(System)))
				{
					pCSystem->preInit();
				}

				g_initStep = 1;

				lock.unlock(); // --(1)
				g_cv.notify_one();
			}

			lock.lock();
			{
				g_cv.wait(lock, []() { return ((g_initStep == 2) || g_hasCriticalError); }); // (3)--

				if (g_hasCriticalError)
				{
					lock.unlock();
					g_cv.notify_one();

					return;
				}

				try
				{
					SIV3D_ENGINE(System)->init();
				}
				catch (const std::exception& error)
				{
					::OutputDebugStringW((Unicode::ToWstring(error.what()) + L'\n').c_str());
					//FreestandingMessageBox::ShowError(error.what());

					g_hasCriticalError = true;

					lock.unlock(); // --(3)
					g_cv.notify_one();

					return;
				}

				g_initStep = 3;

				lock.unlock(); // --(3)
				g_cv.notify_one();
			}

			// (4b)--
			MainSEH();
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
		}
	}
}

using namespace s3d;

////////////////////////////////////////////////////////////////
//
//	WinMain
//
////////////////////////////////////////////////////////////////

// Windows アプリケーションのエントリーポイント | Entry point for Windows applications
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	WinMainInit();

	std::unique_lock lock{ g_mutex }; // (0)--

	const AsyncTask<void> mainThread = Async(MainThread);

	g_cv.wait(lock, [&]() { return (g_initStep == 1); }); // --(0) (2)--
	{
		try
		{
			SIV3D_ENGINE(Window)->init();
		}
		catch (const std::exception& error)
		{
			::OutputDebugStringW((Unicode::ToWstring(error.what()) + L'\n').c_str());
			g_hasCriticalError = true;

			return -1;
		}

		g_initStep = 2;

		lock.unlock(); // --(2)
		g_cv.notify_one();
		lock.lock();
	}

	g_cv.wait(lock, []() { return ((g_initStep == 3) || g_hasCriticalError); }); // (4a)--

	if (g_hasCriticalError)
	{
		while (not mainThread.isReady())
		{
			if (g_shouldDestroyWindow.test())
			{
				DestroyWindow();
			}

			::Sleep(1);
		}

		return -1;
	}

	while (not mainThread.isReady())
	{
		PumpMessages();

		if (g_shouldDestroyWindow.test())
		{
			DestroyWindow();
		}

		::Sleep(1);
	}

	WinMainExit();

	return 0;
}
