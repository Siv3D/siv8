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
# include <exception>
# include <mutex>
# include <condition_variable>
# include <Siv3D/Windows/Windows.hpp>

namespace s3d
{
	inline constexpr UINT WindowThreadMessage = (WM_APP + 0x3D0);

	// Stack-owned by the sender through SendMessage and deferred completion.
	struct WindowThreadCommand
	{
		void (*invoke)(void*);

		void* context;

		std::exception_ptr exception;

		WindowThreadCommand* next = nullptr; // Window thread only.

		std::mutex mutex;

		std::condition_variable ready;

		bool completed = false;

		void execute() noexcept
		{
			try { invoke(context); }
			catch (...) { exception = std::current_exception(); }
		}

		void complete()
		{
			std::lock_guard lock{ mutex };
			completed = true;
			ready.notify_one();
		}

		void wait()
		{
			std::unique_lock lock{ mutex };
			ready.wait(lock, [this] { return completed; });
		}
	};

	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
}
