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
	//	Malloc
	//
	////////////////////////////////////////////////////////////////

	SIV3D_RESTRICT
	inline void* Malloc(const size_t size) noexcept
	{
	# if(SIV3D_USE_MIMALLOC)

		return ::mi_malloc(size);

	# else

		return std::malloc(size);

	# endif
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Calloc
	//
	////////////////////////////////////////////////////////////////

	SIV3D_RESTRICT
	inline void* Calloc(const size_t count, const size_t size) noexcept
	{
	# if(SIV3D_USE_MIMALLOC)

		return ::mi_calloc(count, size);

	# else

		return std::calloc(count, size);

	# endif
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Realloc
	//
	////////////////////////////////////////////////////////////////

	inline void* Realloc(void* p, const size_t newSize) noexcept
	{
	# if(SIV3D_USE_MIMALLOC)

		return ::mi_realloc(p, newSize);

	# else

		return std::realloc(p, newSize);

	# endif
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Free
	//
	////////////////////////////////////////////////////////////////

	inline void Free(void* p) noexcept
	{
	# if(SIV3D_USE_MIMALLOC)

		::mi_free(p);

	# else

		std::free(p);

	# endif
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	AlignedMalloc
	//
	////////////////////////////////////////////////////////////////

	SIV3D_RESTRICT
	inline void* AlignedMalloc(const size_t size, size_t alignment) noexcept
	{
	# if(SIV3D_USE_MIMALLOC)

		return ::mi_aligned_alloc(alignment, size);

	# else

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

	# endif
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	AlignedFree
	//
	////////////////////////////////////////////////////////////////

	inline void AlignedFree(void* p, [[maybe_unused]] const size_t alignment) noexcept
	{
	# if(SIV3D_USE_MIMALLOC)

		::mi_free_aligned(p, alignment);

	# else

		# if SIV3D_PLATFORM(WINDOWS)

			::_aligned_free(p);

		# else

			std::free(p);

		# endif

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	AlignedNew
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class ...Args>
		requires std::is_constructible_v<Type, Args...>
	Type* AlignedNew(Args&&... args)
	{
		Type* p = static_cast<Type*>(AlignedMalloc(sizeof(Type), alignof(Type)));

		if (p == nullptr)
		{
			throw std::bad_alloc{};
		}

		::new (p) Type(std::forward<Args>(args)...);

		return p;
	}

	////////////////////////////////////////////////////////////////
	//
	//	AlignedDelete
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	void AlignedDelete(Type* p, [[maybe_unused]] const size_t alignment)
	{
		if (p == nullptr)
		{
			return;
		}

		p->~Type();

		AlignedFree(p, alignment);
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsAligned
	//
	////////////////////////////////////////////////////////////////

	inline bool IsAligned(const void* p, const size_t alignment) noexcept
	{
		return ((reinterpret_cast<std::uintptr_t>(p) % alignment) == 0);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	GetAlignment
	//
	////////////////////////////////////////////////////////////////

	inline size_t GetAlignment(const void* p) noexcept
	{
		return (1ull << std::countr_zero(reinterpret_cast<std::uintptr_t>(p)));
	}
}
