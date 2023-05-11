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

# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/System/CSystem.hpp>
# include <Siv3D/Window/CWindow.hpp>
# include <Siv3D/AsyncTask.hpp>

// ユーザによって実装される | Implemented by the user
void Main();

//        [THREAD #0]                        [THREAD #1]
//                            
// WinMain() {                 
//   f = std::async(MainThread)       || MainThread() {
//   **LOCK**                         ||   Siv3DEngine engine;
//                                    ||   engine->System->preInit();
//                                    <=   **LOCK**
//   engine->Window->init();          ||
//   **LOCK**                         =>
//                                    ||   engine->System->init();
//                                    <=
//   while (!f._Is_ready()) {         ||   Main() { while(System::Update()) {} } // User code
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
	namespace
	{
		std::condition_variable g_cv;
		std::mutex g_mutex;
		int32 g_initStep = 0;
		bool g_hasCriticalError = false;
	}

	namespace detail::init
	{
		void InitFileSystem();
	}

	std::atomic_flag g_shouldDestroyWindow;

	static void PumpMessages()
	{
		for (int32 i = 0; i < 100; ++i)
		{
			MSG message{};

			if (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
			{
				::TranslateMessage(&message);
				::DispatchMessageW(&message);
			}
			else
			{
				return;
			}
		}
	}

	void DestroyWindow()
	{
		if (auto pCWindow = dynamic_cast<CWindow*>(SIV3D_ENGINE(Window)))
		{
			pCWindow->destroy();
		}

		g_shouldDestroyWindow.clear();
	}

	static void MainThread()
	{
		detail::init::InitFileSystem();

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
				//::OutputDebugStringW(Format(error, U'\n').toWstr().c_str());
				//FreestandingMessageBox::ShowError(Format(error));
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
		Main();
	}
}

// Windows 用エントリーポイント | Windows entry point
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
	::timeBeginPeriod(1);

	if (FAILED(::CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		::OutputDebugStringW(L"CoInitializeEx() failed\n");
	}

	using namespace s3d;
	{
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
				//::OutputDebugStringW(Format(error, U'\n').toWstr().c_str());
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
	}

	::CoUninitialize();

	::timeEndPeriod(1);
	
	return 0;
}
