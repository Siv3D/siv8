//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Contains
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool Contains(const Rect& a, const Point& b) noexcept;

		[[nodiscard]]
		constexpr bool Contains(const Rect& a, const Vec2& b) noexcept;


		[[nodiscard]]
		constexpr bool Contains(const RectF& a, const Point& b) noexcept;

		[[nodiscard]]
		constexpr bool Contains(const RectF& a, const Vec2& b) noexcept;


		[[nodiscard]]
		bool Contains(const Polygon& a, const Vec2& b) noexcept;
	}
}

# include "Contains.ipp"
