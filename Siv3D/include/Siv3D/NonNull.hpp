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
# include <cstddef> // std::nullptr_t
# include <cassert>
# include "Concepts.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	NoneNull
	//
	////////////////////////////////////////////////////////////////

	/// @brief ポインタが nullptr でないことを示すラッパークラス | Wrapper class that indicates that the pointer is not nullptr
	/// @tparam Pointer ポインタ型 | Pointer type
	template <Concept::Pointer Pointer>
	class NonNull
	{
	public:

		NonNull() = delete;

		NonNull(std::nullptr_t) = delete;

		[[nodiscard]]
		constexpr explicit NonNull(Pointer pointer) noexcept;

		[[nodiscard]]
		NonNull(const NonNull&) = default;

		NonNull& operator =(const NonNull&) = default;

		NonNull& operator=(std::nullptr_t) = delete;
		
		constexpr operator Pointer() const noexcept;
		
		/// @brief ポインタを取得します。 | Gets the pointer.
		/// @return ポインタ | Pointer
		constexpr decltype(auto) operator ->() const noexcept { return get(); }

		constexpr decltype(auto) operator *() const noexcept { return *get(); }

		constexpr std::conditional_t<std::is_copy_constructible_v<Pointer>, Pointer, const Pointer&> get() const noexcept;

	private:

		Pointer m_pointer;
	};
}

# include "detail/NonNull.ipp"
