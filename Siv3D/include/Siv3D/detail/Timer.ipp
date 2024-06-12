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

	inline Timer::Timer(const Duration& duration, const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_durationMicrosec{ static_cast<int64>(duration.count() * 1'000'000) }
		, m_remainingMicrosec{ m_durationMicrosec }
		, m_pSteadyClock{ pSteadyClock }
	{
		if (startImmediately)
		{
			start();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	isStarted
	//
	////////////////////////////////////////////////////////////////

	inline bool Timer::isStarted() const noexcept
	{
		return m_isStarted;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isPaused
	//
	////////////////////////////////////////////////////////////////

	inline bool Timer::isPaused() const noexcept
	{
		return (m_isStarted && m_pausing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isRunning
	//
	////////////////////////////////////////////////////////////////

	inline bool Timer::isRunning() const noexcept
	{
		return (m_isStarted && (not m_pausing) && (not reachedZero()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	reachedZero
	//
	////////////////////////////////////////////////////////////////

	inline bool Timer::reachedZero() const noexcept
	{
		return (us() == 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	start
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::start() noexcept
	{
		if (m_isStarted && (not m_pausing))
		{
			return;
		}

		m_isStarted = true;

		m_pausing = false;

		m_startTimeMicrosec = ISteadyClock::GetMicrosec(m_pSteadyClock);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pause
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::pause() noexcept
	{
		m_remainingMicrosec = us();

		m_pausing = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resume
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::resume() noexcept
	{
		if (not isPaused())
		{
			return;
		}

		start();
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::reset() noexcept
	{
		m_remainingMicrosec = m_durationMicrosec;

		m_isStarted = false;

		m_pausing = false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	restart
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::restart() noexcept
	{
		setRemaining(MicrosecondsF(m_durationMicrosec));

		start();
	}

	inline void Timer::restart(const Duration& duration) noexcept
	{
		reset();

		set(duration);

		start();
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::set(const Duration& duration) noexcept
	{
		m_durationMicrosec = static_cast<int64>(duration.count() * 1'000'000);

		m_remainingMicrosec = m_durationMicrosec;

		m_startTimeMicrosec = ISteadyClock::GetMicrosec(m_pSteadyClock);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setRemaining
	//
	////////////////////////////////////////////////////////////////

	inline void Timer::setRemaining(const Duration& remaining) noexcept
	{
		m_remainingMicrosec = static_cast<int64>(remaining.count() * (1000LL * 1000LL));

		m_startTimeMicrosec = ISteadyClock::GetMicrosec(m_pSteadyClock);
	}

	////////////////////////////////////////////////////////////////
	//
	//	d, d64, dF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Timer::d() const noexcept
	{
		return static_cast<int32>(d64());
	}

	inline int64 Timer::d64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	inline double Timer::dF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	h, h64, hF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Timer::h() const noexcept
	{
		return static_cast<int32>(h64());
	}

	inline int64 Timer::h64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL * 60LL));
	}

	inline double Timer::hF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL * 60LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	min, mi64, miF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Timer::min() const noexcept
	{
		return static_cast<int32>(min64());
	}

	inline int64 Timer::min64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL));
	}

	inline double Timer::minF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	s, s64, sF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Timer::s() const noexcept
	{
		return static_cast<int32>(s64());
	}

	inline int64 Timer::s64() const noexcept
	{
		return (us() / (1000LL * 1000LL));
	}

	inline double Timer::sF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	s_ceil, s64_ceil
	//
	////////////////////////////////////////////////////////////////

	inline int32 Timer::s_ceil() const noexcept
	{
		return static_cast<int32>(s64_ceil());
	}

	inline int64 Timer::s64_ceil() const noexcept
	{
		return ((us() + 999'999LL) / (1000LL * 1000LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	ms, ms64, msF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Timer::ms() const noexcept
	{
		return static_cast<int32>(ms64());
	}

	inline int64 Timer::ms64() const noexcept
	{
		return (us() / (1000LL));
	}

	inline double Timer::msF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	us, us64, usF
	//
	////////////////////////////////////////////////////////////////

	inline int64 Timer::us() const noexcept
	{
		const int64 t = ISteadyClock::GetMicrosec(m_pSteadyClock);

		if (not m_isStarted)
		{
			return m_durationMicrosec;
		}

		if (m_pausing)
		{
			return Max<int64>(m_remainingMicrosec, 0);
		}

		return Max<int64>(m_remainingMicrosec - (t - m_startTimeMicrosec), 0);
	}

	inline int64 Timer::us64() const noexcept
	{
		return us();
	}

	inline double Timer::usF() const noexcept
	{
		return static_cast<double>(us());
	}

	////////////////////////////////////////////////////////////////
	//
	//	duration
	//
	////////////////////////////////////////////////////////////////

	inline Duration Timer::duration() const noexcept
	{
		return SecondsF{ m_durationMicrosec / static_cast<double>(1000LL * 1000LL) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	remaining
	//
	////////////////////////////////////////////////////////////////

	inline Duration Timer::remaining() const noexcept
	{
		return SecondsF{ sF() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	progress1_0
	//
	////////////////////////////////////////////////////////////////

	inline double Timer::progress1_0() const noexcept
	{
		if (m_durationMicrosec == 0)
		{
			return 0.0;
		}
		else
		{
			return (static_cast<double>(us()) / m_durationMicrosec);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	progress0_1
	//
	////////////////////////////////////////////////////////////////

	inline double Timer::progress0_1() const noexcept
	{
		return (1.0 - progress1_0());
	}
}
