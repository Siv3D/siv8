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

		constexpr bool Contains(const Rect& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		constexpr bool Contains(const Rect& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		constexpr bool Contains(const RectF& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		constexpr bool Contains(const RectF& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		inline bool Contains(const Polygon& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}
	}
}
