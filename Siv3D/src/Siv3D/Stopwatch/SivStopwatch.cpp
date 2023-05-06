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

# include <Siv3D/Stopwatch.hpp>

namespace s3d
{
	Stopwatch::Stopwatch(const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_pSteadyClock{ pSteadyClock }
	{
		if (startImmediately)
		{
			start();
		}
	}

	Stopwatch::Stopwatch(const Duration& startTime, const StartImmediately startImmediately, ISteadyClock* pSteadyClock) noexcept
		: m_pSteadyClock{ pSteadyClock }
	{
		set(startTime);

		if (startImmediately)
		{
			start();
		}
	}

	bool Stopwatch::isStarted() const noexcept
	{
		return m_isStarted;
	}

	bool Stopwatch::isPaused() const noexcept
	{
		return (m_isStarted && m_pausing);
	}

	bool Stopwatch::isRunning() const noexcept
	{
		return (m_isStarted && (not m_pausing));
	}

	void Stopwatch::start() noexcept
	{
		if (not m_pausing)
		{
			return;
		}

		m_isStarted = true;

		m_pausing = false;

		m_startTimeMicrosec = ISteadyClock::GetMicrosec(m_pSteadyClock);
	}

	void Stopwatch::pause() noexcept
	{
		m_accumulatedTimeMicrosec = us();

		m_pausing = true;
	}

	void Stopwatch::resume() noexcept
	{
		if (not isPaused())
		{
			return;
		}

		start();
	}

	void Stopwatch::reset() noexcept
	{
		m_accumulatedTimeMicrosec = 0;

		m_isStarted = false;

		m_pausing = true;
	}

	void Stopwatch::restart() noexcept
	{
		reset();

		start();
	}

	void Stopwatch::set(const Duration& time) noexcept
	{
		m_isStarted = true;

		m_accumulatedTimeMicrosec = static_cast<int64>(time.count() * (1000LL * 1000LL));

		m_startTimeMicrosec = ISteadyClock::GetMicrosec(m_pSteadyClock);
	}

	int32 Stopwatch::d() const noexcept
	{
		return static_cast<int32>(d64());
	}

	int64 Stopwatch::d64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	double Stopwatch::dF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL * 60LL * 24LL));
	}

	int32 Stopwatch::h() const noexcept
	{
		return static_cast<int32>(h64());
	}

	int64 Stopwatch::h64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL * 60LL));
	}

	double Stopwatch::hF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL * 60LL));
	}

	int32 Stopwatch::min() const noexcept
	{
		return static_cast<int32>(min64());
	}

	int64 Stopwatch::min64() const noexcept
	{
		return (us() / (1000LL * 1000LL * 60LL));
	}

	double Stopwatch::minF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL * 60LL));
	}

	int32 Stopwatch::s() const noexcept
	{
		return static_cast<int32>(s64());
	}

	int64 Stopwatch::s64() const noexcept
	{
		return (us() / (1000LL * 1000LL));
	}

	double Stopwatch::sF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL * 1000LL));
	}

	int32 Stopwatch::ms() const noexcept
	{
		return static_cast<int32>(ms64());
	}

	int64 Stopwatch::ms64() const noexcept
	{
		return (us() / (1000LL));
	}

	double Stopwatch::msF() const noexcept
	{
		return static_cast<double>(us() / static_cast<double>(1000LL));
	}

	int64 Stopwatch::us() const noexcept
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

	int64 Stopwatch::us64() const noexcept
	{
		return us();
	}

	double Stopwatch::usF() const noexcept
	{
		return static_cast<double>(us());
	}

	Duration Stopwatch::elapsed() const noexcept
	{
		return SecondsF{ sF() };
	}

	String Stopwatch::format(const StringView format) const
	{
		return FormatTime(elapsed(), format);
	}
}
