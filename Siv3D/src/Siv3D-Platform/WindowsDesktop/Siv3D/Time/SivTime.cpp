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

# include <ctime>
# include <Siv3D/Windows/Windows.hpp>
# include <Siv3D/Time.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		inline static int64 GetQPCFrequency() noexcept
		{
			::LARGE_INTEGER frequency;
			::QueryPerformanceFrequency(&frequency);
			return frequency.QuadPart;
		}

		[[nodiscard]]
		inline static int64 GetQPCCounter() noexcept
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
		inline static int64 GetCount() noexcept
		{
			::LARGE_INTEGER counter;
			::QueryPerformanceCounter(&counter);
			return (counter.QuadPart - g_QPC_Base);
		}

		[[nodiscard]]
		inline static uint64 Get100NanoSecSinceEpoch() noexcept
		{
			::FILETIME fileTime;
			::GetSystemTimeAsFileTime(&fileTime);
			return ((static_cast<uint64>(fileTime.dwHighDateTime) << 32) + fileTime.dwLowDateTime - (11'644'473'600 * 10'000'000));
		}
	}

	namespace Time
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetSec
		//
		////////////////////////////////////////////////////////////////

		int64 GetSec() noexcept
		{
			return static_cast<int64>(GetCount() * g_QPC_FrequencyInvSec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetMillisec
		//
		////////////////////////////////////////////////////////////////

		int64 GetMillisec() noexcept
		{
			return static_cast<int64>(GetCount() * g_QPC_FrequencyInvMilliSec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetMicrosec
		//
		////////////////////////////////////////////////////////////////

		int64 GetMicrosec() noexcept
		{
			return static_cast<int64>(GetCount() * g_QPC_FrequencyInvMicroSec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetNanosec
		//
		////////////////////////////////////////////////////////////////

		int64 GetNanosec() noexcept
		{
			return static_cast<int64>(GetCount() * g_QPC_FrequencyInvNanoSec);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetUptimeSec
		//
		////////////////////////////////////////////////////////////////

		int64 GetUptimeSec() noexcept
		{
			return (::timeGetTime() / 1000);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetUptimeMillisec
		//
		////////////////////////////////////////////////////////////////

		int64 GetUptimeMillisec() noexcept
		{
			return ::timeGetTime();
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetSecSinceEpoch
		//
		////////////////////////////////////////////////////////////////

		uint64 GetSecSinceEpoch() noexcept
		{
			return (Get100NanoSecSinceEpoch() / 10'000'000);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetMillisecSinceEpoch
		//
		////////////////////////////////////////////////////////////////

		uint64 GetMillisecSinceEpoch() noexcept
		{
			return (Get100NanoSecSinceEpoch() / 10'000);
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetMicrosecSinceEpoch
		//
		////////////////////////////////////////////////////////////////

		uint64 GetMicrosecSinceEpoch() noexcept
		{
			return (Get100NanoSecSinceEpoch() / 10);
		}

		////////////////////////////////////////////////////////////////
		//
		//	UTCOffsetMinutes
		//
		////////////////////////////////////////////////////////////////

		int32 UTCOffsetMinutes() noexcept
		{
			long timeZone;
			::_get_timezone(&timeZone);
			return (-timeZone / 60);
		}
	}
}
