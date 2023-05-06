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

# include <ctime>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Time.hpp>

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		inline int64 GetQPCFrequency() noexcept
		{
			::LARGE_INTEGER frequency;
			::QueryPerformanceFrequency(&frequency);
			return frequency.QuadPart;
		}

		[[nodiscard]]
		inline int64 GetQPCCounter() noexcept
		{
			::LARGE_INTEGER counter;
			::QueryPerformanceCounter(&counter);
			return counter.QuadPart;
		}

		static const double g_QPC_FrequencyInvSec{ 1.0 / GetQPCFrequency() };

		static const double g_QPC_FrequencyInvMilliSec{ 1'000.0 / GetQPCFrequency() };

		static const double g_QPC_FrequencyInvMicroSec{ 1'000'000.0 / GetQPCFrequency() };

		static const double g_QPC_FrequencyInvNanoSec{ 1'000'000'000.0 / GetQPCFrequency() };

		static const int64 g_QPC_Base = GetQPCCounter();

		[[nodiscard]]
		inline int64 GetCount() noexcept
		{
			::LARGE_INTEGER counter;
			::QueryPerformanceCounter(&counter);
			return (counter.QuadPart - g_QPC_Base);
		}

		[[nodiscard]]
		inline uint64 Get100NanoSecSinceEpoch() noexcept
		{
			::FILETIME fileTime;
			::GetSystemTimeAsFileTime(&fileTime);
			return ((static_cast<uint64>(fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime - (11'644'473'600 * 10'000'000));
		}
	}

	namespace Time
	{
		int64 GetSec() noexcept
		{
			return static_cast<int64>(detail::GetCount() * detail::g_QPC_FrequencyInvSec);
		}

		int64 GetMillisec() noexcept
		{
			return static_cast<int64>(detail::GetCount() * detail::g_QPC_FrequencyInvMilliSec);
		}

		int64 GetMicrosec() noexcept
		{
			return static_cast<int64>(detail::GetCount() * detail::g_QPC_FrequencyInvMicroSec);
		}

		int64 GetNanosec() noexcept
		{
			return static_cast<int64>(detail::GetCount() * detail::g_QPC_FrequencyInvNanoSec);
		}

		int64 GetUptimeSec() noexcept
		{
			return (::timeGetTime() / 1000);
		}

		int64 GetUptimeMillisec() noexcept
		{
			return ::timeGetTime();
		}

		uint64 GetSecSinceEpoch() noexcept
		{
			return (detail::Get100NanoSecSinceEpoch() / 10'000'000);
		}

		uint64 GetMillisecSinceEpoch() noexcept
		{
			return (detail::Get100NanoSecSinceEpoch() / 10'000);
		}

		uint64 GetMicrosecSinceEpoch() noexcept
		{
			return (detail::Get100NanoSecSinceEpoch() / 10);
		}

		int32 UTCOffsetMinutes() noexcept
		{
			long timeZone;
			::_get_timezone(&timeZone);
			return (-timeZone / 60);
		}
	}
}
