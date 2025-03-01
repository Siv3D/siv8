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
# include <span>
# include "../Common.hpp"
# include "../PointVector.hpp"
# include "../2DShapes.hpp"

namespace s3d
{
	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	BoundingRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Rect BoundingRect(std::span<const Point> points) noexcept;

		[[nodiscard]]
		RectF BoundingRect(std::span<const Float2> points) noexcept;

		[[nodiscard]]
		RectF BoundingRect(std::span<const Vec2> points) noexcept;
	}
}
