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

# pragma once
# include "Concepts.hpp"

//////////////////////////////////////////////////
//
//	ユーティリティ関数 | Utility functions
//
//	Scalar Min(Scalar a, Scalar b);
//	const Type& Min(const Type& a, const Type& b);
//	Scalar Max(Scalar a, Scalar b);
//	const Type& Max(const Type& a, const Type& b);
//	Scalar Clamp(Scalar v, Scalar min, Scalar max);
//	Type& Clamp(const Type& v, const Type& min, const Type& max);
//	bool InRange(Scalar v, Scalar min, Scalar max);
//	bool InRange(const Type& v, const Type& min, const Type& max);
//	bool InOpenRange(Scalar v, Scalar min, Scalar max);
//	bool InOpenRange(const Type& v, const Type& min, const Type& max);
// 
//////////////////////////////////////////////////

namespace s3d
{
	/// @brief 2 つの値のうち小さい方の値を返します。 | Returns the lesser of the two.
	/// @tparam Scalar 比較する値の型 | The type of the value to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち小さい方の値。等しい場合は a | The lesser of a and b. If they are equivalent, returns a
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr Scalar Min(Scalar a, Scalar b) noexcept;

	/// @brief 2 つの値のうち小さい方の値を返します。 | Returns the lesser of the two.
	/// @tparam Type 比較する値の型 | The type of the value to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち小さい方の値。等しい場合は a | The lesser of a and b. If they are equivalent, returns a
	template <class Type>
	[[nodiscard]]
	constexpr const Type& Min(const Type& a, const Type& b) noexcept(noexcept(b < a));

	/// @brief 2 つの値のうち大きい方の値を返します。 | Returns the greater of the two.
	/// @tparam Scalar 比較する値の型 | The type of the value to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち大きい方の値。等しい場合は a | The greater of a and b. If they are equivalent, returns a
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr Scalar Max(Scalar a, Scalar b) noexcept;

	/// @brief 2 つの値のうち大きい方の値を返します。 | Returns the greater of the two.
	/// @tparam Type 比較する値の型 | The type of the value to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち大きい方の値。等しい場合は a | The greater of a and b. If they are equivalent, returns a
	template <class Type>
	[[nodiscard]]
	constexpr const Type& Max(const Type& a, const Type& b) noexcept(noexcept(a < b));

	/// @brief 最小値と最大値の範囲にクランプした値を返します。 | Clamps the value to the specified minimum and maximum range.
	/// @tparam Scalar 比較する値の型 | The type of the value to compare
	/// @param v クランプする値 | A value to clamp
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return v をクランプした値 | The clamped value for the v
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr Scalar Clamp(Scalar v, Scalar min, Scalar max) noexcept;

	/// @brief 最小値と最大値の範囲にクランプした値を返します。 | Clamps the value to the specified minimum and maximum range.
	/// @tparam Type 比較する値の型 | The type of the value to compare
	/// @param v クランプする値 | A value to clamp
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return v をクランプした値 | The clamped value for the v
	template <class Type>
	[[nodiscard]]
	constexpr const Type& Clamp(const Type& v, const Type& min, const Type& max) noexcept(noexcept(max < v));

	/// @brief 値が閉区間 [min, max] にあるかを返します。 | Returns true if the value is in the closed interval [min, max].
	/// @tparam Scalar 比較する値の型 | The type of the value to compare
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 閉区間 [min, max] にある場合 true, それ以外の場合は false | Returns true if the value is in the closed interval [min, max], false otherwise
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr bool InRange(Scalar v, Scalar min, Scalar max) noexcept;

	/// @brief 値が閉区間 [min, max] にあるかを返します。 | Returns true if the value is in the closed interval [min, max].
	/// @tparam Type 比較する値の型 | The type of the value to compare
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 閉区間 [min, max] にある場合 true, それ以外の場合は false | Returns true if the value is in the closed interval [min, max], false otherwise
	template <class Type>
	[[nodiscard]]
	constexpr bool InRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min));

	/// @brief 値が開区間 (min, max) にあるかを返します。 | Returns true if the value is in the open interval (min, max).
	/// @tparam Scalar 比較する値の型 | The type of the value to compare
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 開区間 (min, max) にある場合 true, それ以外の場合は false | Returns true if the value is in the open interval (min, max), false otherwise
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr bool InOpenRange(Scalar v, Scalar min, Scalar max) noexcept;

	/// @brief 値が開区間 (min, max) にあるかを返します。 | Returns true if the value is in the open interval (min, max).
	/// @tparam Type 比較する値の型 | The type of the value to compare
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 開区間 (min, max) にある場合 true, それ以外の場合は false | Returns true if the value is in the open interval (min, max), false otherwise
	template <class Type>
	[[nodiscard]]
	constexpr bool InOpenRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min));
}

# include "detail/Utility.ipp"
