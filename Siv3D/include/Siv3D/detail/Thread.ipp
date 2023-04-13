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

# pragma once

namespace s3d
{
	inline Thread::Thread() noexcept
		: m_stopSource{ NoStopState }
		, m_thread{} {}

	template <class FTy, class... Args>
		requires (not std::is_same_v<std::remove_cvref_t<FTy>, Thread>)
	inline Thread::Thread(FTy&& f, Args&&... args)
		: m_stopSource{}
		, m_thread{ [](StopToken st, auto&& f, auto&&... args)
			{
				if constexpr (std::is_invocable_v<std::decay_t<FTy>, StopToken, std::decay_t<Args>...>)
				{
					std::invoke(std::forward<decltype(f)>(f), std::move(st), std::forward<decltype(args)>(args)...);
				}
				else
				{
					std::invoke(std::forward<decltype(f)>(f), std::forward<decltype(args)>(args)...);
				}
			}, m_stopSource.get_token(), std::forward<FTy>(f), std::forward<Args>(args)... } {}

	inline Thread::~Thread()
	{
		tryCancelAndJoin();
	}

	inline Thread& Thread::operator =(Thread&& other) noexcept
	{
		if (this == std::addressof(other))
		{
			return *this;
		}

		tryCancelAndJoin();
		m_thread = std::move(other.m_thread);
		m_stopSource = std::move(other.m_stopSource);
		return *this;
	}

	inline void Thread::swap(Thread& other) noexcept
	{
		m_thread.swap(other.m_thread);
		m_stopSource.swap(other.m_stopSource);
	}

	inline bool Thread::joinable() const noexcept
	{
		return m_thread.joinable();
	}

	inline void Thread::join()
	{
		m_thread.join();
	}

	inline void Thread::detach()
	{
		m_thread.detach();
	}

	inline Thread::id Thread::get_id() const noexcept
	{
		return m_thread.get_id();
	}

	inline Thread::native_handle_type Thread::native_handle() noexcept
	{
		return m_thread.native_handle();
	}

	inline StopSource Thread::get_stop_source() noexcept
	{
		return m_stopSource;
	}

	inline StopToken Thread::get_stop_token() const noexcept
	{
		return m_stopSource.get_token();
	}

	inline bool Thread::request_stop() noexcept
	{
		return m_stopSource.request_stop();
	}

	inline uint32 Thread::AvailableParallelism() noexcept
	{
		return std::thread::hardware_concurrency();
	}

	inline void Thread::tryCancelAndJoin() noexcept
	{
		if (m_thread.joinable())
		{
			m_stopSource.request_stop();
			m_thread.join();
		}
	}
}
