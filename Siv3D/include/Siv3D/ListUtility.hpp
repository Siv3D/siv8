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
# include <algorithm>
# include "Common.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Min
	//
	////////////////////////////////////////////////////////////////

	/// @brief 渡された初期化リストの中で最小の値を返します。 | Returns the least of the values in initializer list.
	/// @tparam Type 比較する値の型
	/// @param values 比較する値の初期化リスト | Initializer list with the values to compare 
	/// @return 初期化リストの中で最小大の値。複数が等しい場合はその中で最も左の値 | The least value in values. If several values are equivalent to the least, returns the leftmost one
	template <class Type>
	[[nodiscard]]
	constexpr Type Min(std::initializer_list<Type> values);

	////////////////////////////////////////////////////////////////
	//
	//	Max
	//
	////////////////////////////////////////////////////////////////

	/// @brief 渡された初期化リストの中で最大の値を返します。 | Returns the greatest of the values in initializer list.
	/// @tparam Type 比較する値の型
	/// @param values 比較する値の初期化リスト | Initializer list with the values to compare
	/// @return 初期化リストの中で最大の値。複数が等しい場合はその中で最も左の値 | The greatest value in values. If several values are equivalent to the greatest, returns the leftmost one
	template <class Type>
	[[nodiscard]]
	constexpr Type Max(std::initializer_list<Type> values);

	////////////////////////////////////////////////////////////////
	//
	//	MinMax
	//
	////////////////////////////////////////////////////////////////

	/// @brief 渡された初期化リストの中で最小の値と最大の値を返します。 | Returns the least and the greatest of the values in initializer list.
	/// @tparam Type 比較する値の型
	/// @param values 比較する値の初期化リスト | Initializer list with the values to compare
	/// @return 初期化リストの中で最小の値と最大の値 | The least and the greatest value in values
	template <class Type>
	[[nodiscard]]
	constexpr std::pair<Type, Type> MinMax(std::initializer_list<Type> values);
}

# include "detail/ListUtility.ipp"
