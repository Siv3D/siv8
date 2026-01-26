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

		[[nodiscard]]
		double Distance(const Point& a, const Point& b) noexcept;

		[[nodiscard]]
		double Distance(const Point& a, const Vec2& b) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Distance(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double Distance(const Vec2& a, const Point& b) noexcept;

		[[nodiscard]]
		double Distance(const Vec2& a, const Vec2& b) noexcept;

		[[nodiscard]]
		double Distance(const Vec2& a, const Line& b);
		
		[[nodiscard]]
		double Distance(const Vec2& a, const Bezier2& b);

		[[nodiscard]]
		double Distance(const Vec2& a, const Bezier3& b);
	}
}

# include "Distance.ipp"
