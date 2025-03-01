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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Zip
	//
	////////////////////////////////////////////////////////////////

	/// @brief 2 つの Range を zip します。
	/// @param range1 一方の Range
	/// @param range2 もう一方の Range
	/// @return zip された Range
	[[nodiscard]]
	constexpr auto Zip(std::ranges::viewable_range auto&& range1, std::ranges::viewable_range auto&& range2) noexcept;
}

# include "detail/Zip.ipp"
