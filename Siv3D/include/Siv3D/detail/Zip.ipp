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
	constexpr auto Zip(std::ranges::viewable_range auto&& range1, std::ranges::viewable_range auto&& range2) noexcept
	{
		return std::views::zip(std::forward<decltype(range1)>(range1), std::forward<decltype(range2)>(range2));
	}
}
