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
	inline uint64 GetCycleCount() noexcept
	{
	# if SIV3D_PLATFORM(WINDOWS) && SIV3D_CPU(X86_64)
			
		return ::__rdtsc();
		
	# elif SIV3D_CPU(X86_64)

		uint32 hi, lo;
		__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
		return (static_cast<uint64>(lo) | (static_cast<uint64>(hi) << 32));

	# elif SIV3D_CPU(ARM64)

		uint64 value;
		__asm__ __volatile__("mrs %0, cntvct_el0" : "=r"(value));
		return value;

	# else

		# error Unimplemented

	# endif
	}

	inline uint64 GetCycleFrequency() noexcept
	{
	# if SIV3D_CPU(ARM64)

		uint64 value;
		__asm__ __volatile__("mrs %0, cntfrq_el0" : "=r"(value));
		return value;

	# else

		return 0;

	# endif
	}

	inline uint64 CycleClock::cycles() const noexcept
	{
		return (GetCycleCount() - m_start);
	}
}
