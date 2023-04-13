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
#	include <ThirdParty/jthread/stop_token.hpp>
# endif
# include <thread>
# include "Common.hpp"

namespace s3d
{
	using StopSource = std::stop_source;
	
	using StopToken = std::stop_token;

	template <class Callback>
	using StopCallback = std::stop_callback<Callback>;
	
	inline constexpr std::nostopstate_t NoStopState{};

	class Thread
	{
	public:

		using id = std::thread::id;

		using native_handle_type = std::thread::native_handle_type;

		Thread() noexcept;

		template <class FTy, class... Args>
			requires (not std::is_same_v<std::remove_cvref_t<FTy>, Thread>)
		[[nodiscard]]
		explicit Thread(FTy&& f, Args&&... args);

		~Thread();

		Thread(const Thread&) = delete;

		Thread(Thread&&) noexcept = default;

		Thread& operator =(const Thread&) = delete;

		Thread& operator =(Thread&& other) noexcept;

		void swap(Thread& other) noexcept;

		[[nodiscard]]
		bool joinable() const noexcept;

		void join();

		void detach();

		[[nodiscard]]
		id get_id() const noexcept;

		[[nodiscard]]
		native_handle_type native_handle() noexcept;

		[[nodiscard]]
		StopSource get_stop_source() noexcept;

		[[nodiscard]]
		StopToken get_stop_token() const noexcept;

		bool request_stop() noexcept;

		friend void swap(Thread& lhs, Thread& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		[[nodiscard]]
		static uint32 AvailableParallelism() noexcept;

	private:

		void tryCancelAndJoin() noexcept;

		StopSource m_stopSource;

		std::thread m_thread;
	};
}

# include "detail/Thread.ipp"
