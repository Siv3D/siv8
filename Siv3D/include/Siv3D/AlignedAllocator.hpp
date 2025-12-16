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
# include "Memory.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	AlignedAllocator
	//
	////////////////////////////////////////////////////////////////

	/// @brief アライメント指定付きのメモリアロケータ | Aligned allocator
	/// @tparam Type アロケーションするオブジェクトの型 | Type of the object to allocate
	/// @tparam Alignment アライメント（バイト） | Alignment in bytes
	template <class Type, size_t Alignment = alignof(Type)>
	class AlignedAllocator
	{
	public:

		static_assert((alignof(Type) <= Alignment), "Alignment must be at least alignof(Type)");

		using value_type								= Type;
		using size_type									= std::size_t;
		using difference_type							= std::ptrdiff_t;

		using propagate_on_container_copy_assignment	= std::false_type;
		using propagate_on_container_move_assignment	= std::true_type;
		using propagate_on_container_swap				= std::true_type;
		using is_always_equal							= std::true_type;

		static constexpr size_t alignment = Alignment;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		constexpr AlignedAllocator() noexcept = default;

		template <class U, size_t A>
		constexpr AlignedAllocator(const AlignedAllocator<U, A>&) noexcept {}

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		template <class U, size_t A>
		friend constexpr bool operator ==(const AlignedAllocator<Type, Alignment>& lhs, const AlignedAllocator<U, A>& rhs) noexcept
		{
			return (Alignment == A);
		}

		////////////////////////////////////////////////////////////////
		//
		//	allocate
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Type* allocate(size_type n);

		////////////////////////////////////////////////////////////////
		//
		//	deallocate
		//
		////////////////////////////////////////////////////////////////

		void deallocate(Type* const p, const size_type) noexcept;

		template <class U>
		struct rebind
		{
			using other = AlignedAllocator<U, Alignment>;
		};
	};
}

# include "detail/AlignedAllocator.ipp"
