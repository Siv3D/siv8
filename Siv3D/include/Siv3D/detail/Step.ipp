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
	//	step
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	constexpr auto step(const IndexType n) noexcept
	{
		return std::ranges::iota_view{ IndexType{ 0 }, n };
	}

	template <Concept::Integral IndexType>
	constexpr auto step(const IndexType start, const IndexType n, const IndexType step) noexcept
	{
		return (std::ranges::iota_view{ IndexType{ 0 }, n }
			| std::views::transform([=](IndexType i) { return (start + i * step); }));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	constexpr auto step_backward(const IndexType n) noexcept
	{
		return (std::ranges::iota_view{ IndexType{ 0 }, n }
			| std::views::reverse);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Range
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral IndexType>
	constexpr auto Range(const IndexType from, const IndexType to) noexcept
	{
		return std::ranges::iota_view{ from, (to + 1) };
	}
}
