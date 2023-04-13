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
# if __has_include(<stop_token>)
#	include <stop_token>
# else
#	include <ThirdParty/stop_token/stop_token.hpp>
# endif
# include <thread>
# include "Common.hpp"

namespace s3d
{
	using StopSource = std::stop_source;
	
	using StopToken = std::stop_token;
	
	inline constexpr std::nostopstate_t NoStopState{};

	class Thread
	{
	public:

		using id = std::thread::id;

		using native_handle_type = std::thread::native_handle_type;

		Thread() noexcept
			: m_stopSource{ NoStopState }
			, m_thread{} {}

		template <class FTy, class... Args, std::enable_if_t<not std::is_same_v<std::remove_cvref_t<FTy>, Thread>, int> = 0>
		[[nodiscard]]
		explicit Thread(FTy&& f, Args&&... args)
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

		~Thread()
		{
			tryCancelAndJoin();
		}

		Thread(const Thread&) = delete;

		Thread(Thread&&) noexcept = default;

		Thread& operator =(const Thread&) = delete;

		Thread& operator =(Thread&& other) noexcept
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

		void swap(Thread& other) noexcept
		{
			m_thread.swap(other.m_thread);
			m_stopSource.swap(other.m_stopSource);
		}

		[[nodiscard]]
		bool joinable() const noexcept
		{
			return m_thread.joinable();
		}

		void join()
		{
			m_thread.join();
		}

		void detach()
		{
			m_thread.detach();
		}

		[[nodiscard]]
		id get_id() const noexcept
		{
			return m_thread.get_id();
		}

		[[nodiscard]]
		native_handle_type native_handle() noexcept
		{
			return m_thread.native_handle();
		}

		[[nodiscard]]
		StopSource get_stop_source() noexcept
		{
			return m_stopSource;
		}

		[[nodiscard]]
		StopToken get_stop_token() const noexcept
		{
			return m_stopSource.get_token();
		}

		bool request_stop() noexcept
		{
			return m_stopSource.request_stop();
		}

		friend void swap(Thread& lhs, Thread& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		[[nodiscard]]
		static uint32 AvailableParallelism() noexcept
		{
			return std::thread::hardware_concurrency();
		}

	private:

		void tryCancelAndJoin() noexcept
		{
			if (m_thread.joinable())
			{
				m_stopSource.request_stop();
				m_thread.join();
			}
		}

		StopSource m_stopSource;

		std::thread m_thread;
	};
}
