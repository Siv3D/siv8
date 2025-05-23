﻿//-----------------------------------------------
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
# include <utility>
# include "Common.hpp"
# include "Concepts.hpp"
# include "detail/UtilityClass.ipp"

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
//	Signed Abs(Signed a);
//	Arithmetic AbsDiff(Arithmetic a, Arithmetic b);
//	auto FromEnum(Enum x);
//	Enum ToEnum(std::underlying_type_t<Enum> x);
//	bool IsOdd(Integral x);
//	bool IsEven(Integral x);
//	Type Identity(Type x);
//	
// 
//////////////////////////////////////////////////

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Min
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 つの値のうち小さい方の値を返します。 | Returns the lesser of the two.
	/// @tparam Scalar 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち小さい方の値。等しい場合は a | The lesser of a and b. If they are equivalent, returns a
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr auto Min(Scalar a, Scalar b) noexcept;

	/// @brief 2 つの値のうち小さい方の値を返します。 | Returns the lesser of the two.
	/// @tparam Type 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち小さい方の値。等しい場合は a | The lesser of a and b. If they are equivalent, returns a
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr const Type& Min(const Type& a SIV3D_LIFETIMEBOUND, const Type& b SIV3D_LIFETIMEBOUND) noexcept(noexcept(b < a));

	/// @brief 3 つの値のうち最小の値を返します。 | Returns the smallest of the three.
	/// @tparam Scalar 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @param c 比較する値 | Another value to compare
	/// @return 3 つの値のうち最小の値 | The smallest of the three
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr auto Min(Scalar a, Scalar b, Scalar c) noexcept;

	/// @brief 3 つの値のうち最小の値を返します。 | Returns the smallest of the three.
	/// @tparam Type 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @param c 比較する値 | Another value to compare
	/// @return 3 つの値のうち最小の値 | The smallest of the three
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr const Type& Min(const Type& a SIV3D_LIFETIMEBOUND, const Type& b SIV3D_LIFETIMEBOUND, const Type& c SIV3D_LIFETIMEBOUND) noexcept(noexcept(b < a));

	////////////////////////////////////////////////////////////////
	//
	//	Max
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 つの値のうち大きい方の値を返します。 | Returns the greater of the two.
	/// @tparam Scalar 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち大きい方の値。等しい場合は a | The greater of a and b. If they are equivalent, returns a
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr auto Max(Scalar a, Scalar b) noexcept;

	/// @brief 2 つの値のうち大きい方の値を返します。 | Returns the greater of the two.
	/// @tparam Type 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @return 2 つの値のうち大きい方の値。等しい場合は a | The greater of a and b. If they are equivalent, returns a
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr const Type& Max(const Type& a SIV3D_LIFETIMEBOUND, const Type& b SIV3D_LIFETIMEBOUND) noexcept(noexcept(a < b));

	/// @brief 3 つの値のうち最大の値を返します。 | Returns the greatest of the three.
	/// @tparam Scalar 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @param c 比較する値 | Another value to compare
	/// @return 3 つの値のうち最大の値 | The greatest of the three
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr auto Max(Scalar a, Scalar b, Scalar c) noexcept;

	/// @brief 3 つの値のうち最大の値を返します。 | Returns the greatest of the three.
	/// @tparam Type 比較する値の型 | The type of the values to compare
	/// @param a 比較する値 | A value to compare
	/// @param b 比較する値 | Another value to compare
	/// @param c 比較する値 | Another value to compare
	/// @return 3 つの値のうち最大の値 | The greatest of the three
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr const Type& Max(const Type& a SIV3D_LIFETIMEBOUND, const Type& b SIV3D_LIFETIMEBOUND, const Type& c SIV3D_LIFETIMEBOUND) noexcept(noexcept(a < b));

	////////////////////////////////////////////////////////////////
	//
	//	Clamp
	//
	////////////////////////////////////////////////////////////////

	/// @brief 最小値と最大値の範囲にクランプした値を返します。 | Clamps the value to the specified minimum and maximum range.
	/// @param v クランプする値 | A value to clamp
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return v をクランプした値 | The clamped value for the v
	template <Concept::Scalar Scalar>
	[[nodiscard]]
	constexpr auto Clamp(Scalar v, Scalar min, Scalar max) noexcept;

	/// @brief 最小値と最大値の範囲にクランプした値を返します。 | Clamps the value to the specified minimum and maximum range.
	/// @param v クランプする値 | A value to clamp
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return v をクランプした値 | The clamped value for the v
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr const Type& Clamp(const Type& v SIV3D_LIFETIMEBOUND, const Type& min SIV3D_LIFETIMEBOUND, const Type& max SIV3D_LIFETIMEBOUND) noexcept(noexcept(max < v));

	////////////////////////////////////////////////////////////////
	//
	//	InRange
	//
	////////////////////////////////////////////////////////////////

	/// @brief 値が閉区間 [min, max] にあるかを返します。 | Returns true if the value is in the closed interval [min, max].
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 閉区間 [min, max] にある場合 true, それ以外の場合は false | Returns true if the value is in the closed interval [min, max], false otherwise
	[[nodiscard]]
	constexpr bool InRange(Concept::Scalar auto v, Concept::Scalar auto min, Concept::Scalar auto max) noexcept;

	/// @brief 値が閉区間 [min, max] にあるかを返します。 | Returns true if the value is in the closed interval [min, max].
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 閉区間 [min, max] にある場合 true, それ以外の場合は false | Returns true if the value is in the closed interval [min, max], false otherwise
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr bool InRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min));

	////////////////////////////////////////////////////////////////
	//
	//	InOpenRange
	//
	////////////////////////////////////////////////////////////////

	/// @brief 値が開区間 (min, max) にあるかを返します。 | Returns true if the value is in the open interval (min, max).
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 開区間 (min, max) にある場合 true, それ以外の場合は false | Returns true if the value is in the open interval (min, max), false otherwise
	[[nodiscard]]
	constexpr bool InOpenRange(Concept::Scalar auto v, Concept::Scalar auto min, Concept::Scalar auto max) noexcept;

	/// @brief 値が開区間 (min, max) にあるかを返します。 | Returns true if the value is in the open interval (min, max).
	/// @param v 比較する値 | A value to compare
	/// @param min 範囲の最小値 | The specified minimum range
	/// @param max 範囲の最大値 | The specified maximum range
	/// @return 開区間 (min, max) にある場合 true, それ以外の場合は false | Returns true if the value is in the open interval (min, max), false otherwise
	template <class Type> requires (not Concept::Scalar<Type>)
	[[nodiscard]]
	constexpr bool InOpenRange(const Type& v, const Type& min, const Type& max) noexcept(noexcept(v < min));

	////////////////////////////////////////////////////////////////
	//
	//	Abs
	//
	////////////////////////////////////////////////////////////////

	/// @brief 絶対値を計算する関数オブジェクトです。
	inline constexpr auto Abs = detail::Abs_impl{};

	////////////////////////////////////////////////////////////////
	//
	//	AbsDiff
	//
	////////////////////////////////////////////////////////////////

	/// @brief 差の絶対値を返します。
	/// @param a 1 つ目の値
	/// @param b 2 つ目の値
	/// @return 2 つの値の差の絶対値
	template <Concept::Arithmetic Arithmetic>
	[[nodiscard]]
	inline constexpr auto AbsDiff(Arithmetic a, Arithmetic b) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	FromEnum
	//
	////////////////////////////////////////////////////////////////

	/// @brief 列挙型を整数型に変換する関数オブジェクトです。
	inline constexpr auto FromEnum = detail::FromEnum_impl{};

	////////////////////////////////////////////////////////////////
	//
	//	ToEnum
	//
	////////////////////////////////////////////////////////////////

	/// @brief 整数を列挙型の値に変換する関数オブジェクトです。
	/// @tparam Enum 変換先の列挙型
	template <Concept::Enum Enum>
	inline constexpr auto ToEnum = detail::ToEnum_impl<Enum>{};

	////////////////////////////////////////////////////////////////
	//
	//	IsOdd
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数が奇数かを判定する関数オブジェクトです。
	inline constexpr auto IsOdd = detail::IsOdd_impl{};

	////////////////////////////////////////////////////////////////
	//
	//	IsEven
	//
	////////////////////////////////////////////////////////////////

	/// @brief 数が偶数かを判定する関数オブジェクトです。
	inline constexpr auto IsEven = detail::IsEven_impl{};

	////////////////////////////////////////////////////////////////
	//
	//	Identity
	//
	////////////////////////////////////////////////////////////////

	/// @brief 渡された値をそのまま返す関数オブジェクトです。
	inline constexpr auto Identity = detail::Identity_impl{};
}

# include "detail/Utility.ipp"
