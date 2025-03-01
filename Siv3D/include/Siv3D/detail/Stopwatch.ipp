//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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

	inline Stopwatch::Stopwatch(const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_pSteadyClock{ pSteadyClock }
	{
		if (startImmediately)
		{
			start();
		}
	}

	inline Stopwatch::Stopwatch(const Duration& startTime, const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_pSteadyClock{ pSteadyClock }
	{
		set(startTime);

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

	inline bool Stopwatch::isStarted() const noexcept
	{
		return m_isStarted;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	isPaused
	//
	////////////////////////////////////////////////////////////////

	inline bool Stopwatch::isPaused() const noexcept
	{
		return (m_isStarted && m_pausing);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	isRunning
	//
	////////////////////////////////////////////////////////////////

	inline bool Stopwatch::isRunning() const noexcept
	{
		return (m_isStarted && (not m_pausing));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	start
	//
	////////////////////////////////////////////////////////////////

	inline void Stopwatch::start() noexcept
	{
		if (not m_pausing)
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

	inline void Stopwatch::pause() noexcept
	{
		m_accumulatedTimeMicrosec = us();

		m_pausing = true;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	resume
	//
	////////////////////////////////////////////////////////////////

	inline void Stopwatch::resume() noexcept
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

	inline void Stopwatch::reset() noexcept
	{
		m_accumulatedTimeMicrosec = 0;

		m_isStarted = false;

		m_pausing = true;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	restart
	//
	////////////////////////////////////////////////////////////////

	inline void Stopwatch::restart() noexcept
	{
		reset();

		start();
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	inline void Stopwatch::set(const Duration& time) noexcept
	{
		m_isStarted = true;

		m_accumulatedTimeMicrosec = static_cast<int64>(time.count() * (1000LL * 1000LL));

		m_startTimeMicrosec = ISteadyClock::GetMicrosec(m_pSteadyClock);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	d, d64, dF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Stopwatch::d() const noexcept
	{
		return static_cast<int32>(d64());
	}

	inline int64 Stopwatch::d64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	inline double Stopwatch::dF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL * 60LL * 24LL));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	h, h64, hF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Stopwatch::h() const noexcept
	{
		return static_cast<int32>(h64());
	}

	inline int64 Stopwatch::h64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL * 60LL));
	}

	inline double Stopwatch::hF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL * 60LL));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	min, min64, minF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Stopwatch::min() const noexcept
	{
		return static_cast<int32>(min64());
	}

	inline int64 Stopwatch::min64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL));
	}

	inline double Stopwatch::minF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	s, s64, sF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Stopwatch::s() const noexcept
	{
		return static_cast<int32>(s64());
	}

	inline int64 Stopwatch::s64() const noexcept
	{
		return (us() / (1000LL * 1000LL));
	}

	inline double Stopwatch::sF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	ms, ms64, msF
	//
	////////////////////////////////////////////////////////////////

	inline int32 Stopwatch::ms() const noexcept
	{
		return static_cast<int32>(ms64());
	}

	inline int64 Stopwatch::ms64() const noexcept
	{
		return (us() / (1000LL));
	}

	inline double Stopwatch::msF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	us, us64, usF
	//
	////////////////////////////////////////////////////////////////

	inline int64 Stopwatch::us() const noexcept
	{
		const int64 t = ISteadyClock::GetMicrosec(m_pSteadyClock);

		if (not m_isStarted)
		{
			return 0;
		}

		if (m_pausing)
		{
			return m_accumulatedTimeMicrosec;
		}

		return (m_accumulatedTimeMicrosec + (t - m_startTimeMicrosec));
	}

	inline int64 Stopwatch::us64() const noexcept
	{
		return us();
	}

	inline double Stopwatch::usF() const noexcept
	{
		return static_cast<double>(us());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	elapsed
	//
	////////////////////////////////////////////////////////////////

	inline Duration Stopwatch::elapsed() const noexcept
	{
		return SecondsF{ sF() };
	}
}
