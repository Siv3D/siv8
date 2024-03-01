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
	//	Indexed
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	constexpr auto Indexed(std::ranges::viewable_range auto&& range) noexcept
	{
		return std::ranges::zip_view{ std::views::iota(IndexType{ 0 }),
			std::views::all_t<decltype((range))>(range) };
	}
}
