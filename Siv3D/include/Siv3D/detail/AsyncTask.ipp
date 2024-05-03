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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	AsyncTask<Type>::AsyncTask(base_type&& other) noexcept
		: m_data{ std::move(other) } {}

	template <class Type>
	AsyncTask<Type>::AsyncTask(AsyncTask&& other) noexcept
		: m_data{ std::move(other.m_data) } {}

	template <class Type>
	template <class Fty, class... Args>
	AsyncTask<Type>::AsyncTask(Fty&& f, Args&&... args) requires std::invocable<std::decay_t<Fty>, std::decay_t<Args>...>
# if !SIV3D_PLATFORM(WEB) || defined(__EMSCRIPTEN_PTHREADS__)
		: m_data{ std::async(std::launch::async, std::forward<Fty>(f), std::forward<Args>(args)...) } {}
# else
		: m_data{} {}
# endif

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	AsyncTask<Type>& AsyncTask<Type>::operator =(base_type&& other) noexcept
	{
		m_data = std::move(other);
		return *this;
	}

	template <class Type>
	AsyncTask<Type>& AsyncTask<Type>::operator =(AsyncTask&& other) noexcept
	{
		m_data = std::move(other.m_data);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isValid
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	bool AsyncTask<Type>::isValid() const noexcept
	{
		return m_data.valid();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isReady
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	bool AsyncTask<Type>::isReady() const
	{
		return (m_data.valid()
			&& (m_data.wait_for(std::chrono::seconds{ 0 }) == std::future_status::ready));
	}

	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type AsyncTask<Type>::get()
	{
		return m_data.get();
	}

	////////////////////////////////////////////////////////////////
	//
	//	wait
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	void AsyncTask<Type>::wait() const
	{
		m_data.wait();
	}

	////////////////////////////////////////////////////////////////
	//
	//	wait_for
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	template <class Rep, class Period>
	std::future_status AsyncTask<Type>::wait_for(const std::chrono::duration<Rep, Period>& relTime) const
	{
		return m_data.wait_for(relTime);
	}

	////////////////////////////////////////////////////////////////
	//
	//	wait_until
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	template <class Clock, class Duration>
	std::future_status AsyncTask<Type>::wait_until(const std::chrono::time_point<Clock, Duration>& absTime) const
	{
		return m_data.wait_until(absTime);
	}

	////////////////////////////////////////////////////////////////
	//
	//	share
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	std::shared_future<Type> AsyncTask<Type>::share() noexcept
	{
		return m_data.share();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Async
	//
	////////////////////////////////////////////////////////////////

	template <class Fty, class... Args> requires std::invocable<std::decay_t<Fty>, std::decay_t<Args>...>
	auto Async(Fty&& f, Args&&... args)
	{
		return AsyncTask<std::invoke_result_t<std::decay_t<Fty>, std::decay_t<Args>...>>{ std::forward<Fty>(f), std::forward<Args>(args)... };
	}
}
