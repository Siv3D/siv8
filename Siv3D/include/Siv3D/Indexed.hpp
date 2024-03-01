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
# include <ranges>
# include "Concepts.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Indexed
	//
	////////////////////////////////////////////////////////////////

	/// @brief 0 から増える iota_view と与えられた範囲の zip_view を返します。 | Returns a zip_view of an iota_view that increases from 0 and the given range.
	/// @tparam IndexType インデックスの整数型、デフォルトは int32 | Index type, default is int32
	/// @param range 範囲 | Range
	/// @return zip_view
	template <Concept::Integral IndexType = int32>
	[[nodiscard]]
	constexpr auto Indexed(std::ranges::viewable_range auto&& range) noexcept;
}

# include "detail/Indexed.ipp"
