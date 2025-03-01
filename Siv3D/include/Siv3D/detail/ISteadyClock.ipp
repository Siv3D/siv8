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
	//	getNanosec
	//
	////////////////////////////////////////////////////////////////

	inline uint64 ISteadyClock::getNanosec() noexcept
	{
		return (getMicrosec() * 1000);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	GetMicrosec
	//
	////////////////////////////////////////////////////////////////

	inline uint64 ISteadyClock::GetMicrosec(ISteadyClock* pSteadyClock) noexcept
	{
		if (pSteadyClock)
		{
			return pSteadyClock->getMicrosec();
		}
		else
		{
			return Time::GetMicrosec();
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	GetNanosec
	//
	////////////////////////////////////////////////////////////////

	inline uint64 ISteadyClock::GetNanosec(ISteadyClock* pSteadyClock) noexcept
	{
		if (pSteadyClock)
		{
			return pSteadyClock->getNanosec();
		}
		else
		{
			return Time::GetNanosec();
		}
	}
}
