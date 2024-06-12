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

	inline VariableSpeedStopwatch::VariableSpeedStopwatch(const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: VariableSpeedStopwatch{ DefaultSpeed, startImmediately, pSteadyClock } {}

	inline VariableSpeedStopwatch::VariableSpeedStopwatch(const double speed, const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_speed{ speed }
		, m_pSteadyClock{ pSteadyClock }
	{
		if (startImmediately)
		{
			start();
		}
	}

	inline VariableSpeedStopwatch::VariableSpeedStopwatch(const Duration& startTime, const double speed, const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_speed{ speed }
		, m_pSteadyClock{ pSteadyClock }
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

	inline bool VariableSpeedStopwatch::isStarted() const noexcept
	{
		return m_isStarted;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isPaused
	//
	////////////////////////////////////////////////////////////////

	inline bool VariableSpeedStopwatch::isPaused() const noexcept
	{
		return (m_isStarted && m_pausing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isRunning
	//
	////////////////////////////////////////////////////////////////

	inline bool VariableSpeedStopwatch::isRunning() const noexcept
	{
		return (m_isStarted && (not m_pausing));
	}

	////////////////////////////////////////////////////////////////
	//
	//	start
	//
	////////////////////////////////////////////////////////////////

	inline void VariableSpeedStopwatch::start() noexcept
	{
		if (not m_pausing)
		{
			return;
		}

		m_isStarted = true;

		m_pausing = false;

		m_lastTimeNanosec = ISteadyClock::GetNanosec(m_pSteadyClock);
	}

	////////////////////////////////////////////////////////////////
	//
	//	pause
	//
	////////////////////////////////////////////////////////////////

	inline void VariableSpeedStopwatch::pause() noexcept
	{
		m_accumulatedNanosec = ns();

		m_pausing = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	resume
	//
	////////////////////////////////////////////////////////////////

	inline void VariableSpeedStopwatch::resume() noexcept
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

	inline void VariableSpeedStopwatch::reset() noexcept
	{
		m_accumulatedNanosec = 0;

		m_isStarted = false;

		m_pausing = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	restart
	//
	////////////////////////////////////////////////////////////////

	inline void VariableSpeedStopwatch::restart() noexcept
	{
		reset();

		start();
	}

	////////////////////////////////////////////////////////////////
	//
	//	set
	//
	////////////////////////////////////////////////////////////////

	inline void VariableSpeedStopwatch::set(const Duration& time) noexcept
	{
		m_isStarted = true;

		m_accumulatedNanosec = static_cast<int64>(time.count() * (1000LL * 1000LL * 1000LL));

		m_lastTimeNanosec = ISteadyClock::GetNanosec(m_pSteadyClock);
	}

	////////////////////////////////////////////////////////////////
	//
	//	setSpeed
	//
	////////////////////////////////////////////////////////////////

	inline void VariableSpeedStopwatch::setSpeed(const double speed) noexcept
	{
		assert(0.0 <= speed);

		if (speed == m_speed)
		{
			return;
		}

		m_accumulatedNanosec = ns();

		m_speed = speed;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSpeed
	//
	////////////////////////////////////////////////////////////////

	inline double VariableSpeedStopwatch::getSpeed() const noexcept
	{
		return m_speed;
	}

	////////////////////////////////////////////////////////////////
	//
	//	d, d64, dF
	//
	////////////////////////////////////////////////////////////////

	inline int32 VariableSpeedStopwatch::d() const noexcept
	{
		return static_cast<int32>(d64());
	}

	inline int64 VariableSpeedStopwatch::d64() const noexcept
	{
		return (ns() / (1000LL * 1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	inline double VariableSpeedStopwatch::dF() const noexcept
	{
		return static_cast<double>(ns() / static_cast<double>(1000LL * 1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	h, h64, hF
	//
	////////////////////////////////////////////////////////////////

	inline int32 VariableSpeedStopwatch::h() const noexcept
	{
		return static_cast<int32>(h64());
	}

	inline int64 VariableSpeedStopwatch::h64() const noexcept
	{
		return (ns() / (1000LL * 1000LL * 1000LL * 60LL * 60LL));
	}

	inline double VariableSpeedStopwatch::hF() const noexcept
	{
		return static_cast<double>(ns() / static_cast<double>(1000LL * 1000LL * 1000LL * 60LL * 60LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	min, min64, minF
	//
	////////////////////////////////////////////////////////////////

	inline int32 VariableSpeedStopwatch::min() const noexcept
	{
		return static_cast<int32>(min64());
	}

	inline int64 VariableSpeedStopwatch::min64() const noexcept
	{
		return (ns() / (1000LL * 1000LL * 1000LL * 60LL));
	}

	inline double VariableSpeedStopwatch::minF() const noexcept
	{
		return static_cast<double>(ns() / static_cast<double>(1000LL * 1000LL * 1000LL * 60LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	s, s64, sF
	//
	////////////////////////////////////////////////////////////////

	inline int32 VariableSpeedStopwatch::s() const noexcept
	{
		return static_cast<int32>(s64());
	}

	inline int64 VariableSpeedStopwatch::s64() const noexcept
	{
		return (ns() / (1000LL * 1000LL * 1000LL));
	}

	inline double VariableSpeedStopwatch::sF() const noexcept
	{
		return static_cast<double>(ns() / static_cast<double>(1000LL * 1000LL * 1000LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	ms, ms64, msF
	//
	////////////////////////////////////////////////////////////////

	inline int32 VariableSpeedStopwatch::ms() const noexcept
	{
		return static_cast<int32>(ms64());
	}

	inline int64 VariableSpeedStopwatch::ms64() const noexcept
	{
		return (ns() / (1000LL * 1000LL));
	}

	inline double VariableSpeedStopwatch::msF() const noexcept
	{
		return static_cast<double>(ns() / static_cast<double>(1000LL * 1000LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	us, us64, usF
	//
	////////////////////////////////////////////////////////////////

	inline int64 VariableSpeedStopwatch::us() const noexcept
	{
		return us64();
	}

	inline int64 VariableSpeedStopwatch::us64() const noexcept
	{
		return (ns() / 1000LL);
	}

	inline double VariableSpeedStopwatch::usF() const noexcept
	{
		return static_cast<double>(ns() / static_cast<double>(1000LL));
	}

	////////////////////////////////////////////////////////////////
	//
	//	elapsed
	//
	////////////////////////////////////////////////////////////////

	inline Duration VariableSpeedStopwatch::elapsed() const noexcept
	{
		return SecondsF{ sF() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	inline int64 VariableSpeedStopwatch::ns() const noexcept
	{
		const int64 t = ISteadyClock::GetNanosec(m_pSteadyClock);

		if (not m_isStarted)
		{
			return 0;
		}

		if (m_pausing)
		{
			return m_accumulatedNanosec;
		}

		const int64 delta = static_cast<int64>((t - m_lastTimeNanosec) * m_speed);

		m_accumulatedNanosec += delta;

		m_lastTimeNanosec = t;

		return m_accumulatedNanosec;
	}
}
