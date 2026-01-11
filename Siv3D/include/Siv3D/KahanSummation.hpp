//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Concepts.hpp"
# include "Utility.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	KahanSummation
	//
	////////////////////////////////////////////////////////////////

	/// @brief Kahan-Babuska の加算アルゴリズム | Kahan-Babuska summation algorithm
	/// @tparam Float 浮動小数点型 | Floating point type
	template <Concept::FloatingPoint Float>
	class KahanSummation
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		KahanSummation() = default;

		[[nodiscard]] 
		constexpr KahanSummation(Float init) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	reset
		//
		////////////////////////////////////////////////////////////////

		constexpr void reset(Float init = 0) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 加算代入 | Addition assignment
		/// @param value 加算する値 | Value to add
		/// @return *this
		constexpr KahanSummation& operator +=(Float value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 減算代入 | Subtraction assignment
		/// @param value 減算する値 | Value to subtract
		/// @return *this
		constexpr KahanSummation& operator -=(Float value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	value
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の合計値を返します。 | Returns the current sum.
		/// @return 現在の合計値 | Current sum
		[[nodiscard]]
		constexpr Float value() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	correction
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の補正値を返します。 | Returns the current correction.
		/// @return 現在の補正値 | Current correction
		[[nodiscard]]
		constexpr Float correction() const noexcept;

	private:

		Float m_sum = 0;

		Float m_c = 0;
	};
}

# include "detail/KahanSummation.ipp"
