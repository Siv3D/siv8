//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	SIV3D_RESTRICT
	inline void* Malloc(const size_t size) noexcept
	{
		return std::malloc(size);
	}

	SIV3D_RESTRICT
	inline void* Calloc(const size_t count, const size_t size) noexcept
	{
		return std::calloc(count, size);
	}

	inline void* Realloc(void* p, const size_t newsize) noexcept
	{
		return std::realloc(p, newsize);
	}

	inline void Free(void* p) noexcept
	{
		std::free(p);
	}

	SIV3D_RESTRICT
	inline void* AlignedAlloc(const size_t size, size_t alignment) noexcept
	{
	# if SIV3D_PLATFORM(WINDOWS)

		return ::_aligned_malloc(size, alignment);

	# else

		void* p = nullptr;
		
		alignment = ((alignment < MinAlignment) ? MinAlignment : alignment);
		
		if (0 != ::posix_memalign(&p, alignment, size)) [[unlikely]]
		{
			return nullptr;
		}
		
		return p;

	# endif
	}

	inline void AlignedFree(void* p) noexcept
	{
	# if SIV3D_PLATFORM(WINDOWS)

		::_aligned_free(p);

	# else

		std::free(p);

	# endif
	}

	inline bool IsAligned(const void* p, const size_t alignment) noexcept
	{
		return ((reinterpret_cast<std::uintptr_t>(p) % alignment) == 0);
	}
}
