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

# include <ctime>
# include <sys/time.h>
# include <Siv3D/Time.hpp>

namespace s3d
{
	namespace Time
	{
		static const uint64 g_nsecBase = clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);
	
		int64 GetSec() noexcept
		{
			return static_cast<int64>((clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) - g_nsecBase) / 1'000'000'000);
		}

		int64 GetMillisec() noexcept
		{
			return static_cast<int64>((clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) - g_nsecBase) / 1'000'000);
		}

		int64 GetMicrosec() noexcept
		{
			return static_cast<int64>((clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) - g_nsecBase) / 1'000);
		}

		int64 GetNanosec() noexcept
		{
			return static_cast<int64>(clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) - g_nsecBase);
		}

		int64 GetUptimeSec() noexcept
		{
			return static_cast<int64>(clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) / 1'000'000'000);
		}

		int64 GetUptimeMillisec() noexcept
		{
			return static_cast<int64>(clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW) / 1'000'000);
		}

		uint64 GetSecSinceEpoch() noexcept
		{
			return (GetMicrosecSinceEpoch() / 1'000'000);
		}

		uint64 GetMillisecSinceEpoch() noexcept
		{
			return (GetMicrosecSinceEpoch() / 1'000);
		}

		uint64 GetMicrosecSinceEpoch() noexcept
		{
			 ::timeval tv;
			 ::gettimeofday(&tv, nullptr);
			 return (tv.tv_sec * 1'000'000ULL + tv.tv_usec);
		}

		int32 UTCOffsetMinutes() noexcept
		{
			struct timezone tz;
			::gettimeofday(nullptr, &tz);
			return -tz.tz_minuteswest;
		}
	}
}
