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
	//	allocate
	//
	////////////////////////////////////////////////////////////////

	template <class Type, size_t Alignment>	
	Type* AlignedAllocator<Type, Alignment>::allocate(const size_t n)
	{   
		const size_t size = (n * sizeof(Type));

		size_t padding = 0;

		if (size % alignment)
		{
			padding = (alignment - (size % alignment));
		}

		const auto p = AlignedMalloc((size + padding), alignment);

		if (p == nullptr)
		{
			throw std::bad_alloc{};
		}

		return static_cast<Type*>(p);
	}

	////////////////////////////////////////////////////////////////
	//
	//	deallocate
	//
	////////////////////////////////////////////////////////////////

	template <class Type, size_t Alignment>	
	void AlignedAllocator<Type, Alignment>::deallocate(Type* const p, const size_t) noexcept
	{
		AlignedFree(p, alignment);
	}
}
