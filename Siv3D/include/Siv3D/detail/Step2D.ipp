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

	constexpr auto step(const Size n) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);

	# ifdef __cpp_lib_ranges_cartesian_product

		return std::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) { return Point{ std::get<1>(p), std::get<0>(p) }; });

	# else

		return (tl::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) { return Point{ std::get<1>(p), std::get<0>(p) }; }));

	# endif
	}

	constexpr auto step(const Point start, const Size n) noexcept
	{
		const auto x_view = std::views::iota(start.x, (start.x + n.x));
		const auto y_view = std::views::iota(start.y, (start.y + n.y));

	# ifdef __cpp_lib_ranges_cartesian_product

		return std::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) { return Point{ std::get<1>(p), std::get<0>(p) }; });

	# else

		return (tl::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) { return Point{ std::get<1>(p), std::get<0>(p) }; }));

	# endif
	}

	constexpr auto step(const Point start, const Size n, const Size step) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);

	# ifdef __cpp_lib_ranges_cartesian_product

		return std::views::cartesian_product(y_view, x_view)
			| std::views::transform([=](const auto& p) { return Point{ (start.y + std::get<1>(p) * step.y),  (start.x + std::get<0>(p) * step.x) }; });

	# else

		return (tl::views::cartesian_product(y_view, x_view)
			| std::views::transform([=](const auto& p) { return Point{ (start.y + std::get<1>(p) * step.y),  (start.x + std::get<0>(p) * step.x) }; });

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	step_backward
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	constexpr auto step_backward(const Size n) noexcept
	{
		const auto x_view = std::views::iota(0, n.x);
		const auto y_view = std::views::iota(0, n.y);

	# ifdef __cpp_lib_ranges_cartesian_product

		return std::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) { return Point{ std::get<1>(p), std::get<0>(p) }; })
			| std::views::reverse;

	# else

		return (tl::views::cartesian_product(y_view, x_view)
			| std::views::transform([](const auto& p) { return Point{ std::get<1>(p), std::get<0>(p) }; })
			| std::views::reverse);

	# endif
	}
}
