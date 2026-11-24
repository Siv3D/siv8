//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <array>
# include <span>
# include <Siv3D/RoundRect.hpp>
# include <Siv3D/Optional.hpp>

namespace s3d::detail::RoundRectBalloon
{
	struct Placement
	{
		RoundRect body;
		Vec2 base0, tip, base1;
		uint8 side; // Top, right, bottom, left, in clockwise order.
	};

	// Finite, representable geometry is a precondition. No tail crosses a corner.
	[[nodiscard]]
	inline Optional<Placement> Place(const RoundRect& shape, const Vec2& target, const double tailWidth) noexcept
	{
		if ((shape.w <= 0) || (shape.h <= 0) || (tailWidth <= 0)) return none;
		const double left = shape.x, top = shape.y;
		const double right = (left + shape.w), bottom = (top + shape.h);
		if ((left <= target.x) && (target.x <= right) && (top <= target.y) && (target.y <= bottom)) return none;
		const double r = Min(Abs(shape.r), shape.w * 0.5, shape.h * 0.5);
		const Vec2 center = shape.rect.center(), d = (target - center);
		// A diagonal tie selects a vertical side; no angles or persistent state.
		const bool verticalSide = (Abs(d.x) * shape.h >= Abs(d.y) * shape.w);
		const double straight = ((verticalSide ? shape.h : shape.w) - 2 * r);
		if (straight <= 0) return none;
		const double halfWidth = (Min(tailWidth, straight) * 0.5);
		Placement result{ RoundRect{ shape.rect, r }, {}, target, {}, 0 };
		if (verticalSide)
		{
			const double x = ((d.x > 0) ? right : left);
			const double y = Clamp(center.y + d.y * ((x - center.x) / d.x), top + r + halfWidth, bottom - r - halfWidth);
			result.side = ((d.x > 0) ? 1 : 3);
			const double along = ((d.x > 0) ? halfWidth : -halfWidth);
			result.base0 = { x, y - along };
			result.base1 = { x, y + along };
		}
		else
		{
			const double y = ((d.y > 0) ? bottom : top);
			const double x = Clamp(center.x + d.x * ((y - center.y) / d.y), left + r + halfWidth, right - r - halfWidth);
			result.side = ((d.y > 0) ? 2 : 0);
			const double along = ((d.y > 0) ? -halfWidth : halfWidth);
			result.base0 = { x - along, y };
			result.base1 = { x + along, y };
		}
		return result;
	}

	struct Contour
	{
		// Four quarter arcs of at most 63 intervals, two roots, and one tip.
		std::array<Vec2, 259> vertices;
		uint16 count = 0;
		uint16 tipIndex = 0;

		[[nodiscard]]
		std::span<const Vec2> points() const noexcept { return { vertices.data(), count }; }
	};

	void MakeContour(Contour& destination, const Placement& placement, float scale);
}
