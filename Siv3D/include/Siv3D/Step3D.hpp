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
# include "PointVector.hpp"

namespace s3d
{
# ifdef __cpp_lib_ranges_cartesian_product

	////////////////////////////////////////////////////////////////
	//
	//	step
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr auto step(Size3D n) noexcept;

	[[nodiscard]]
	constexpr auto step(Point3D start, Size3D n) noexcept;

	[[nodiscard]]
	constexpr auto step(Point3D start, Size3D n, Size3D step) noexcept;

	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr auto step_backward(Size3D n) noexcept;

# endif
}

# include "detail/Step3D.ipp"
