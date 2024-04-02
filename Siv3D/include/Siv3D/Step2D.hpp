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
# include "PointVector.hpp"
# ifndef __cpp_lib_ranges_cartesian_product
#	include <ThirdParty/tl/cartesian_product.hpp>
# endif

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	step
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr auto step(Size n) noexcept;

	[[nodiscard]]
	constexpr auto step(Point start, Size n) noexcept;

	[[nodiscard]]
	constexpr auto step(Point start, Size n, Size step) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr auto step_backward(Size n) noexcept;
}

# include "detail/Step2D.ipp"
