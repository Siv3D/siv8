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
	namespace detail
	{
		constexpr bool IsGeometry2DSegment(const Geometry2DSizedShapeKind kind) noexcept
		{
			return ((kind == Geometry2DSizedShapeKind::VerticalSegment)
				|| (kind == Geometry2DSizedShapeKind::HorizontalSegment));
		}

		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const RectF& rect) noexcept
		{
			const double w = rect.size.x;
			const double h = rect.size.y;

			assert((0.0 <= w) && (0.0 <= h));

			if ((w == 0.0) && (h == 0.0))
			{
				return Geometry2DSizedShapeKind::Empty;
			}

			if (w == 0.0)
			{
				return Geometry2DSizedShapeKind::VerticalSegment;
			}

			if (h == 0.0)
			{
				return Geometry2DSizedShapeKind::HorizontalSegment;
			}

			return Geometry2DSizedShapeKind::Area;
		}

		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const Circle& circle) noexcept
		{
			assert(0.0 <= circle.r);

			return (circle.r == 0.0)
				? Geometry2DSizedShapeKind::Empty
				: Geometry2DSizedShapeKind::Area;
		}

		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const Ellipse& ellipse) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

			assert((0.0 <= ax) && (0.0 <= by));

			if ((ax == 0.0) && (by == 0.0))
			{
				return Geometry2DSizedShapeKind::Empty;
			}

			if (ax == 0.0)
			{
				return Geometry2DSizedShapeKind::VerticalSegment;
			}

			if (by == 0.0)
			{
				return Geometry2DSizedShapeKind::HorizontalSegment;
			}

			return Geometry2DSizedShapeKind::Area;
		}

		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const SuperEllipse& superEllipse) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;

			assert((0.0 <= ax) && (0.0 <= by));
			assert(0.0 < superEllipse.n);

			if ((ax == 0.0) && (by == 0.0))
			{
				return Geometry2DSizedShapeKind::Empty;
			}

			if (ax == 0.0)
			{
				return Geometry2DSizedShapeKind::VerticalSegment;
			}

			if (by == 0.0)
			{
				return Geometry2DSizedShapeKind::HorizontalSegment;
			}

			return Geometry2DSizedShapeKind::Area;
		}

		constexpr Geometry2DSizedShapeKind ClassifyGeometry2DSizedShape(const RoundRect& roundRect) noexcept
		{
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			assert((0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));

			if ((w == 0.0) && (h == 0.0))
			{
				return Geometry2DSizedShapeKind::Empty;
			}

			if (w == 0.0)
			{
				return Geometry2DSizedShapeKind::VerticalSegment;
			}

			if (h == 0.0)
			{
				return Geometry2DSizedShapeKind::HorizontalSegment;
			}

			return Geometry2DSizedShapeKind::Area;
		}

		constexpr Line GetGeometry2DDegenerateSegment(const RectF& rect, const Geometry2DSizedShapeKind kind) noexcept
		{
			assert(IsGeometry2DSegment(kind));

			const Vec2 topLeft = rect.pos;

			if (kind == Geometry2DSizedShapeKind::VerticalSegment)
			{
				return Line{ topLeft, Vec2{ topLeft.x, (topLeft.y + rect.size.y) } };
			}

			return Line{ topLeft, Vec2{ (topLeft.x + rect.size.x), topLeft.y } };
		}

		constexpr Line GetGeometry2DDegenerateSegment(const Ellipse& ellipse, const Geometry2DSizedShapeKind kind) noexcept
		{
			assert(IsGeometry2DSegment(kind));

			if (kind == Geometry2DSizedShapeKind::VerticalSegment)
			{
				return Line{
					Vec2{ ellipse.center.x, (ellipse.center.y - ellipse.axes.y) },
					Vec2{ ellipse.center.x, (ellipse.center.y + ellipse.axes.y) }
				};
			}

			return Line{
				Vec2{ (ellipse.center.x - ellipse.axes.x), ellipse.center.y },
				Vec2{ (ellipse.center.x + ellipse.axes.x), ellipse.center.y }
			};
		}

		constexpr Line GetGeometry2DDegenerateSegment(const SuperEllipse& superEllipse, const Geometry2DSizedShapeKind kind) noexcept
		{
			assert(IsGeometry2DSegment(kind));

			if (kind == Geometry2DSizedShapeKind::VerticalSegment)
			{
				return Line{
					Vec2{ superEllipse.center.x, (superEllipse.center.y - superEllipse.axes.y) },
					Vec2{ superEllipse.center.x, (superEllipse.center.y + superEllipse.axes.y) }
				};
			}

			return Line{
				Vec2{ (superEllipse.center.x - superEllipse.axes.x), superEllipse.center.y },
				Vec2{ (superEllipse.center.x + superEllipse.axes.x), superEllipse.center.y }
			};
		}

		constexpr Line GetGeometry2DDegenerateSegment(const RoundRect& roundRect, const Geometry2DSizedShapeKind kind) noexcept
		{
			return GetGeometry2DDegenerateSegment(roundRect.rect, kind);
		}

		constexpr double GetGeometry2DEffectiveRadius(const RoundRect& roundRect) noexcept
		{
			assert((0.0 < roundRect.rect.size.x)
				&& (0.0 < roundRect.rect.size.y)
				&& (0.0 <= roundRect.r));

			return Min(roundRect.r,
				Min((roundRect.rect.size.x * 0.5), (roundRect.rect.size.y * 0.5)));
		}

		constexpr RectF GetGeometry2DRoundRectCore(const RoundRect& roundRect, const double effectiveRadius) noexcept
		{
			const RectF& rect = roundRect.rect;
			assert((0.0 <= effectiveRadius)
				&& (effectiveRadius <= (rect.size.x * 0.5))
				&& (effectiveRadius <= (rect.size.y * 0.5)));

			return RectF{
				(rect.pos.x + effectiveRadius),
				(rect.pos.y + effectiveRadius),
				(rect.size.x - (effectiveRadius * 2.0)),
				(rect.size.y - (effectiveRadius * 2.0))
			};
		}
	}
}
