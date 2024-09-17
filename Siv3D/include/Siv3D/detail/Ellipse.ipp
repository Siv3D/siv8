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
	constexpr Ellipse::Ellipse(const value_type r) noexcept
		: center{ 0, 0 }
		, axes{ r, r } {}

	constexpr Ellipse::Ellipse(const value_type _a, const value_type _b) noexcept
		: center{ 0, 0 }
		, axes{ _a, _b } {}

	constexpr Ellipse::Ellipse(const value_type _x, const value_type _y, const value_type r) noexcept
		: center{ _x, _y }
		, axes{ r, r } {}

	constexpr Ellipse::Ellipse(const value_type _x, const value_type _y, const value_type _a, const value_type _b) noexcept
		: center{ _x, _y }
		, axes{ _a, _b } {}

	constexpr Ellipse::Ellipse(const position_type _center) noexcept
		: center{ _center }
		, axes{ 0, 0 } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const value_type r) noexcept
		: center{ _center }
		, axes{ r, r } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const value_type _a, const value_type _b) noexcept
		: center{ _center }
		, axes{ _a, _b } {}

	constexpr Ellipse::Ellipse(const value_type _x, const value_type _y, const size_type& _axes) noexcept
		: center{ _x, _y }
		, axes{ _axes } {}

	constexpr Ellipse::Ellipse(const position_type& _center, const size_type& _axes) noexcept
		: center{ _center }
		, axes{ _axes } {}

	constexpr Ellipse::Ellipse(const Circle& circle) noexcept
		: center{ circle.center }
		, axes{ circle.r, circle.r } {}

	constexpr Ellipse::Ellipse(const RectF& rect) noexcept
		: center{ rect.center() }
		, axes{ (rect.size.x * 0.5), (rect.size.y * 0.5) } {}
}
