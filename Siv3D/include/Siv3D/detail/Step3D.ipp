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

namespace s3d
{
# ifdef __cpp_lib_ranges_cartesian_product

	////////////////////////////////////////////////////////////////
	//
	//	step
	//
	////////////////////////////////////////////////////////////////

	constexpr auto step(const Size3D n) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);
		const auto z_view = std::views::iota(0, n.z);
		return (std::views::cartesian_product(z_view, y_view, x_view)
			| std::views::transform([](const auto& p) -> Point3D
				{ return{ std::get<2>(p), std::get<1>(p), std::get<0>(p) }; }));
	}

	constexpr auto step(const Point3D start, const Size3D n) noexcept
		{
		const auto x_view = std::views::iota(start.x, (start.x + n.x));
		const auto y_view = std::views::iota(start.y, (start.y + n.y));
		const auto z_view = std::views::iota(start.z, (start.z + n.z));
		return (std::views::cartesian_product(z_view, y_view, x_view)
			| std::views::transform([](const auto& p) -> Point3D
				{ return{ std::get<2>(p), std::get<1>(p), std::get<0>(p) }; }));
	}

	constexpr auto step(const Point3D start, const Size3D n, const Size3D step) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);
		const auto z_view = std::views::iota(0, n.z);
		return (std::views::cartesian_product(z_view, y_view, x_view)
			| std::views::transform([=](const auto& p) -> Point3D
				{ return{ (start.x + std::get<2>(p) * step.x),  (start.y + std::get<1>(p) * step.y), (start.z + std::get<0>(p) * step.z) }; }));
	}

	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

	constexpr auto step_backward(const Size3D n) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);
		const auto z_view = std::views::iota(0, n.z);
		return (std::views::cartesian_product(z_view, y_view, x_view)
			| std::views::reverse
			| std::views::transform([](const auto& p) -> Point3D
				{ return{ std::get<2>(p), std::get<1>(p), std::get<0>(p) }; }));
	}

# endif
}
