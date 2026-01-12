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
		//	Distance(Point, _)
		//
		////////////////////////////////////////////////////////////////

		inline double Distance(const Point& a, const Point& b) noexcept
		{
			return a.distanceFrom(b);
		}

		inline double Distance(const Point& a, const Vec2& b) noexcept
		{
			return a.distanceFrom(b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance(Point, _)
		//
		////////////////////////////////////////////////////////////////

		inline double Distance(const Vec2& a, const Point& b) noexcept
		{
			return a.distanceFrom(b);
		}

		inline double Distance(const Vec2& a, const Vec2& b) noexcept
		{
			return a.distanceFrom(b);
		}
	}
}
