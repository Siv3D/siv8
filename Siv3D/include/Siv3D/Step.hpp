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
# include <ranges>
# include "Concepts.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	step
	//
	////////////////////////////////////////////////////////////////

	/// @brief 0 から (n - 1) までの range を返します。 | Returns a range from 0 to (n - 1).
	/// @tparam Integral インデックスの整数型、デフォルトは int32 | Index type, default is int32
	/// @param n 終点（含まない） | End point (exclusive)
	/// @return 0 から (n - 1) までの range | Range from `0` to `(n - 1)`
	/// @remark `n` は 0 以上である必要があります。 | `n` must be greater than or equal to 0.
	template <Concept::Integral IndexType = int32>
	[[nodiscard]]
	constexpr auto step(IndexType n) noexcept;

	/// @brief start から (start + n * step) までの range を返します。 | Returns a range from start to (start + n * step).
	/// @tparam Integral インデックスの整数型、デフォルトは int32 | Index type, default is int32
	/// @param start 範囲の開始値 | Start value of the range
	/// @param n 範囲の要素数 | Number of elements in the range
	/// @param step 増分、デフォルトは 1 | Increment, default is 1
	/// @return `start` から `(start + n * step)` までの range | Range from `start` to `(start + n * step)`
	template <Concept::Integral IndexType = int32>
	[[nodiscard]]
	constexpr auto step(IndexType start, IndexType n, IndexType step = 1) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

	/// @brief (n - 1) から 0 までの range を返します。 | Returns a range from (n - 1) to 0.
	/// @tparam Integral インデックスの整数型、デフォルトは int32 | Index type, default is int32
	/// @param n 逆順の開始値（含まない） | Start value of the reverse range (exclusive)
	/// @return (n - 1) から 0 までの range | Range from `(n - 1)` to `0`
	/// @remark `n` は 0 以上である必要があります。 | `n` must be greater than or equal to 0.
	template <Concept::Integral IndexType = int32>
	[[nodiscard]]
	constexpr auto step_backward(IndexType n) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	Range
	//
	////////////////////////////////////////////////////////////////

	/// @brief from から to までの range を返します。 | Returns a range from from to to.
	/// @tparam Integral インデックスの整数型、デフォルトは int32 | Index type, default is int32
	/// @param from 範囲の開始値 | Start value of the range
	/// @param to 範囲の終点（含む） | End point of the range (inclusive)
	/// @return `from` から `to` までの range | Range from `from` to `to`
	template <Concept::Integral IndexType = int32>
	[[nodiscard]]
	constexpr auto Range(IndexType from, IndexType to) noexcept;
}

# include "detail/Step.ipp"
