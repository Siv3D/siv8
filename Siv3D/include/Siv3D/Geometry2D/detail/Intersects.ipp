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
//	Authors (OpenSiv3D challenge participants)
//	- Ebishu
//	- fal_rnd
//	- きつねび
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace detail
	{
		inline constexpr PointContainmentOptions ConvexClockwise{ .boundary = BoundaryPolicy::Included, .shape = PolygonShape::ConvexClockwise };

		[[nodiscard]]
		constexpr bool BetweenClosed(const double a, const double x, const double b) noexcept
		{
			return (a <= b)
				? ((a <= x) && (x <= b))
				: ((b <= x) && (x <= a));
		}

		[[nodiscard]]
		constexpr bool OppositeSigns(const double a, const double b) noexcept
		{
			return (((a < 0.0) && (0.0 < b))
				|| ((b < 0.0) && (0.0 < a)));
		}

		[[nodiscard]]
		constexpr bool UpdateLineClipInterval(const double p, const double d, const double min, const double max, double& t0, double& t1) noexcept
		{
			if (d == 0.0)
			{
				return ((min <= p) && (p <= max));
			}

			double a = ((min - p) / d);
			double b = ((max - p) / d);

			if (b < a)
			{
				const double t = a;
				a = b;
				b = t;
			}

			t0 = Max(t0, a);
			t1 = Min(t1, b);

			return (t0 <= t1);
		}

		[[nodiscard]]
		constexpr bool SegmentIntersectsDiskBox(const Line& segment, const Vec2& center, const double r, const double left, const double top, const double right, const double bottom) noexcept
		{
			const Vec2 d = (segment.end - segment.start);
			double t0 = 0.0;
			double t1 = 1.0;

			if ((not UpdateLineClipInterval(segment.start.x, d.x, left, right, t0, t1))
				|| (not UpdateLineClipInterval(segment.start.y, d.y, top, bottom, t0, t1)))
			{
				return false;
			}

			const double lengthSq = d.dot(d);

			if (lengthSq == 0.0)
			{
				const Vec2 v = (segment.start - center);
				return (v.dot(v) <= (r * r));
			}

			double t = ((center - segment.start).dot(d) / lengthSq);
			t = Clamp(t, t0, t1);

			const Vec2 v = ((segment.start + d * t) - center);
			return (v.dot(v) <= (r * r));
		}

		// The following helpers assume valid non-empty / positive-area input.
		// Public overloads perform the size classification once before calling them.

		[[nodiscard]]
		constexpr bool IntersectsPointRectFNonEmpty(const Vec2& p, const RectF& rect) noexcept
		{
			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

			return ((left <= p.x)
				&& (p.x <= right)
				&& (top <= p.y)
				&& (p.y <= bottom));
		}

		[[nodiscard]]
		constexpr bool IntersectsPointCircleArea(const Vec2& p, const Circle& circle) noexcept
		{
			return (p.distanceFromSq(circle.center) <= (circle.r * circle.r));
		}

		[[nodiscard]]
		constexpr bool IntersectsPointEllipseArea(const Vec2& p, const Ellipse& ellipse) noexcept
		{
			const double dx = ((p.x - ellipse.center.x) / ellipse.axes.x);
			const double dy = ((p.y - ellipse.center.y) / ellipse.axes.y);
			return ((dx * dx + dy * dy) <= 1.0);
		}

		[[nodiscard]]
		constexpr bool IntersectsPointRoundRectArea(const Vec2& p, const RoundRect& roundRect, const double effectiveRadius) noexcept
		{
			const RectF& rect = roundRect.rect;

			if (effectiveRadius == 0.0)
			{
				return IntersectsPointRectFNonEmpty(p, rect);
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

			if (not ((left <= p.x)
				&& (p.x <= right)
				&& (top <= p.y)
				&& (p.y <= bottom)))
			{
				return false;
			}

			const double innerLeft = (left + effectiveRadius);
			const double innerRight = (right - effectiveRadius);
			const double innerTop = (top + effectiveRadius);
			const double innerBottom = (bottom - effectiveRadius);

			if (((innerLeft <= p.x) && (p.x <= innerRight))
				|| ((innerTop <= p.y) && (p.y <= innerBottom)))
			{
				return true;
			}

			const double cx = ((p.x < innerLeft) ? innerLeft : innerRight);
			const double cy = ((p.y < innerTop) ? innerTop : innerBottom);
			const double dx = (p.x - cx);
			const double dy = (p.y - cy);

			return ((dx * dx + dy * dy) <= (effectiveRadius * effectiveRadius));
		}

		[[nodiscard]]
		constexpr bool IntersectsLineRectFNonEmpty(const Line& segment, const RectF& rect) noexcept
		{
			if (IntersectsPointRectFNonEmpty(segment.start, rect)
				|| IntersectsPointRectFNonEmpty(segment.end, rect))
			{
				return true;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

			return (Geometry2D::Intersects(segment, Line{ Vec2{ left, top }, Vec2{ right, top } })
				|| Geometry2D::Intersects(segment, Line{ Vec2{ right, top }, Vec2{ right, bottom } })
				|| Geometry2D::Intersects(segment, Line{ Vec2{ right, bottom }, Vec2{ left, bottom } })
				|| Geometry2D::Intersects(segment, Line{ Vec2{ left, bottom }, Vec2{ left, top } }));
		}

		[[nodiscard]]
		constexpr bool IntersectsLineCircleArea(const Line& segment, const Circle& circle) noexcept
		{
			if (IntersectsPointCircleArea(segment.start, circle)
				|| IntersectsPointCircleArea(segment.end, circle))
			{
				return true;
			}

			const Vec2 d = (segment.end - segment.start);
			const double lengthSq = d.dot(d);

			if (lengthSq == 0.0)
			{
				return false;
			}

			const Vec2 f = (circle.center - segment.start);
			const double tNumerator = f.dot(d);

			if ((tNumerator < 0.0) || (lengthSq < tNumerator))
			{
				return false;
			}

			const double cross = d.cross(f);
			return ((cross * cross) <= (circle.r * circle.r * lengthSq));
		}

		[[nodiscard]]
		constexpr bool IntersectsLineEllipseArea(const Line& segment, const Ellipse& ellipse) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;
			const Vec2 p0{ ((segment.start.x - ellipse.center.x) / ax), ((segment.start.y - ellipse.center.y) / by) };
			const Vec2 p1{ ((segment.end.x - ellipse.center.x) / ax), ((segment.end.y - ellipse.center.y) / by) };

			if ((p0.dot(p0) <= 1.0) || (p1.dot(p1) <= 1.0))
			{
				return true;
			}

			const Vec2 d = (p1 - p0);
			const double lengthSq = d.dot(d);

			if (lengthSq == 0.0)
			{
				return false;
			}

			const double tNumerator = -p0.dot(d);

			if ((tNumerator < 0.0) || (lengthSq < tNumerator))
			{
				return false;
			}

			const double cross = d.cross(p0);
			return ((cross * cross) <= lengthSq);
		}

		[[nodiscard]]
		constexpr bool IntersectsLineRoundRectArea(const Line& segment, const RoundRect& roundRect, const double effectiveRadius) noexcept
		{
			const RectF& rect = roundRect.rect;

			if (effectiveRadius == 0.0)
			{
				return IntersectsLineRectFNonEmpty(segment, rect);
			}

			if (IntersectsPointRoundRectArea(segment.start, roundRect, effectiveRadius)
				|| IntersectsPointRoundRectArea(segment.end, roundRect, effectiveRadius))
			{
				return true;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);
			const double innerLeft = (left + effectiveRadius);
			const double innerRight = (right - effectiveRadius);
			const double innerTop = (top + effectiveRadius);
			const double innerBottom = (bottom - effectiveRadius);

			if (IntersectsLineRectFNonEmpty(segment, RectF{ innerLeft, top, (innerRight - innerLeft), rect.size.y })
				|| IntersectsLineRectFNonEmpty(segment, RectF{ left, innerTop, rect.size.x, (innerBottom - innerTop) }))
			{
				return true;
			}

			return (SegmentIntersectsDiskBox(segment, Vec2{ innerLeft, innerTop }, effectiveRadius, left, top, innerLeft, innerTop)
				|| SegmentIntersectsDiskBox(segment, Vec2{ innerRight, innerTop }, effectiveRadius, innerRight, top, right, innerTop)
				|| SegmentIntersectsDiskBox(segment, Vec2{ innerRight, innerBottom }, effectiveRadius, innerRight, innerBottom, right, bottom)
				|| SegmentIntersectsDiskBox(segment, Vec2{ innerLeft, innerBottom }, effectiveRadius, left, innerBottom, innerLeft, bottom));
		}

	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Point, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Point& a, const Point& b) noexcept
		{
			return (a == b);
		}

		constexpr bool Intersects(const Point& a, const Vec2& b) noexcept
		{
			return (Vec2{ a } == b);
		}

		constexpr bool Intersects(const Point& p, const Line& segment) noexcept
		{
			return Intersects(Vec2{ p }, segment);
		}

		constexpr bool Intersects(const Point& p, const Rect& rect) noexcept
		{
			return Intersects(Vec2{ p }, RectF{ rect });
		}

		constexpr bool Intersects(const Point& p, const RectF& rect) noexcept
		{
			return Intersects(Vec2{ p }, rect);
		}

		constexpr bool Intersects(const Point& p, const Circle& circle) noexcept
		{
			return Intersects(Vec2{ p }, circle);
		}

		constexpr bool Intersects(const Point& p, const Ellipse& ellipse) noexcept
		{
			return Intersects(Vec2{ p }, ellipse);
		}

		constexpr bool Intersects(const Point& p, const Triangle& triangle) noexcept
		{
			return Intersects(Vec2{ p }, triangle);
		}

		constexpr bool Intersects(const Point& p, const Quad& quad) noexcept
		{
			return Intersects(Vec2{ p }, quad);
		}

		constexpr bool Intersects(const Point& p, const RoundRect& roundRect) noexcept
		{
			return Intersects(Vec2{ p }, roundRect);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Vec2& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		constexpr bool Intersects(const Vec2& a, const Vec2& b) noexcept
		{
			return (a == b);
		}

		constexpr bool Intersects(const Vec2& p, const Line& segment) noexcept
		{
			const Vec2 a = segment.start;
			const Vec2 b = segment.end;

			if ((not detail::BetweenClosed(a.x, p.x, b.x))
				|| (not detail::BetweenClosed(a.y, p.y, b.y)))
			{
				return false;
			}

			const Vec2 ab = (b - a);
			const Vec2 ap = (p - a);

			return ((ab.x * ap.y - ab.y * ap.x) == 0.0);
		}

		constexpr bool Intersects(const Vec2& p, const Rect& rect) noexcept
		{
			return Intersects(p, RectF{ rect });
		}

		constexpr bool Intersects(const Vec2& p, const RectF& rect) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(rect) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return detail::IntersectsPointRectFNonEmpty(p, rect);
		}

		constexpr bool Intersects(const Vec2& p, const Circle& circle) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return detail::IntersectsPointCircleArea(p, circle);
		}

		constexpr bool Intersects(const Vec2& p, const Ellipse& ellipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Intersects(p, detail::GetGeometry2DDegenerateSegment(ellipse, kind));
			}

			return detail::IntersectsPointEllipseArea(p, ellipse);
		}

		constexpr bool Intersects(const Vec2& p, const RoundRect& roundRect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Intersects(p, detail::GetGeometry2DDegenerateSegment(roundRect, kind));
			}

			return detail::IntersectsPointRoundRectArea(p, roundRect, detail::GetGeometry2DEffectiveRadius(roundRect));
		}

		constexpr bool Intersects(const Vec2& p, const Triangle& triangle) noexcept
		{
			return ContainsPoint<detail::ConvexClockwise>(triangle.p0, triangle.p1, triangle.p2, p);
		}

		constexpr bool Intersects(const Vec2& p, const Quad& quad) noexcept
		{
			return ContainsPoint<detail::ConvexClockwise>(quad.p0, quad.p1, quad.p2, quad.p3, p);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Line, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Line& segment, const Point& p) noexcept
		{
			return Intersects(p, segment);
		}

		constexpr bool Intersects(const Line& segment, const Vec2& p) noexcept
		{
			return Intersects(p, segment);
		}

		constexpr bool Intersects(const Line& a, const Line& b) noexcept
		{
			const Vec2 a0 = a.start;
			const Vec2 a1 = a.end;
			const Vec2 b0 = b.start;
			const Vec2 b1 = b.end;

			const Vec2 ad = (a1 - a0);
			const Vec2 bd = (b1 - b0);

			const double c0 = ad.cross(b0 - a0);
			const double c1 = ad.cross(b1 - a0);
			const double c2 = bd.cross(a0 - b0);
			const double c3 = bd.cross(a1 - b0);

			if ((c0 == 0.0) && Intersects(b0, a))
			{
				return true;
			}

			if ((c1 == 0.0) && Intersects(b1, a))
			{
				return true;
			}

			if ((c2 == 0.0) && Intersects(a0, b))
			{
				return true;
			}

			if ((c3 == 0.0) && Intersects(a1, b))
			{
				return true;
			}

			return (detail::OppositeSigns(c0, c1)
				&& detail::OppositeSigns(c2, c3));
		}

		constexpr bool Intersects(const Line& segment, const Rect& rect) noexcept
		{
			return Intersects(segment, RectF{ rect });
		}

		constexpr bool Intersects(const Line& segment, const RectF& rect) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(rect) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return detail::IntersectsLineRectFNonEmpty(segment, rect);
		}

		constexpr bool Intersects(const Line& segment, const Circle& circle) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return detail::IntersectsLineCircleArea(segment, circle);
		}

		constexpr bool Intersects(const Line& segment, const Ellipse& ellipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Intersects(segment, detail::GetGeometry2DDegenerateSegment(ellipse, kind));
			}

			return detail::IntersectsLineEllipseArea(segment, ellipse);
		}

		constexpr bool Intersects(const Line& segment, const Triangle& triangle) noexcept
		{
			if (Intersects(segment.start, triangle) || Intersects(segment.end, triangle))
			{
				return true;
			}

			return (Intersects(segment, Line{ triangle.p0, triangle.p1 })
				|| Intersects(segment, Line{ triangle.p1, triangle.p2 })
				|| Intersects(segment, Line{ triangle.p2, triangle.p0 }));
		}

		constexpr bool Intersects(const Line& segment, const Quad& quad) noexcept
		{
			if (Intersects(segment.start, quad) || Intersects(segment.end, quad))
			{
				return true;
			}

			return (Intersects(segment, Line{ quad.p0, quad.p1 })
				|| Intersects(segment, Line{ quad.p1, quad.p2 })
				|| Intersects(segment, Line{ quad.p2, quad.p3 })
				|| Intersects(segment, Line{ quad.p3, quad.p0 }));
		}

		constexpr bool Intersects(const Line& segment, const RoundRect& roundRect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Intersects(segment, detail::GetGeometry2DDegenerateSegment(roundRect, kind));
			}

			return detail::IntersectsLineRoundRectArea(segment, roundRect, detail::GetGeometry2DEffectiveRadius(roundRect));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Rect& rect, const Point& p) noexcept
		{
			return Intersects(p, rect);
		}

		constexpr bool Intersects(const Rect& rect, const Vec2& p) noexcept
		{
			return Intersects(p, rect);
		}

		constexpr bool Intersects(const Rect& rect, const Line& segment) noexcept
		{
			return Intersects(segment, rect);
		}

		constexpr bool Intersects(const Rect& a, const Rect& b) noexcept
		{
			if ((a.size.x < 0) || (a.size.y < 0) || (b.size.x < 0) || (b.size.y < 0))
			{
				assert((0 <= a.size.x) && (0 <= a.size.y) && (0 <= b.size.x) && (0 <= b.size.y));
				return false;
			}

			if (((a.size.x == 0) && (a.size.y == 0))
				|| ((b.size.x == 0) && (b.size.y == 0)))
			{
				return false;
			}

			const int64 aLeft = a.pos.x;
			const int64 aTop = a.pos.y;
			const int64 aRight = (static_cast<int64>(a.pos.x) + static_cast<int64>(a.size.x));
			const int64 aBottom = (static_cast<int64>(a.pos.y) + static_cast<int64>(a.size.y));

			const int64 bLeft = b.pos.x;
			const int64 bTop = b.pos.y;
			const int64 bRight = (static_cast<int64>(b.pos.x) + static_cast<int64>(b.size.x));
			const int64 bBottom = (static_cast<int64>(b.pos.y) + static_cast<int64>(b.size.y));

			return ((aLeft <= bRight)
				&& (bLeft <= aRight)
				&& (aTop <= bBottom)
				&& (bTop <= aBottom));
		}

		constexpr bool Intersects(const Rect& a, const RectF& b) noexcept
		{
			return Intersects(RectF{ a }, b);
		}

		constexpr bool Intersects(const Rect& rect, const Circle& circle) noexcept
		{
			return Intersects(RectF{ rect }, circle);
		}

		constexpr bool Intersects(const Rect& rect, const Ellipse& ellipse) noexcept
		{
			return Intersects(RectF{ rect }, ellipse);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const RectF& rect, const Point& p) noexcept
		{
			return Intersects(p, rect);
		}

		constexpr bool Intersects(const RectF& rect, const Vec2& p) noexcept
		{
			return Intersects(p, rect);
		}

		constexpr bool Intersects(const RectF& rect, const Line& segment) noexcept
		{
			return Intersects(segment, rect);
		}

		constexpr bool Intersects(const RectF& a, const Rect& b) noexcept
		{
			return Intersects(b, a);
		}

		constexpr bool Intersects(const RectF& a, const RectF& b) noexcept
		{
			if ((detail::ClassifyGeometry2DSizedShape(a) == detail::Geometry2DSizedShapeKind::Empty)
				|| (detail::ClassifyGeometry2DSizedShape(b) == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			const double aLeft = a.pos.x;
			const double aTop = a.pos.y;
			const double aRight = (a.pos.x + a.size.x);
			const double aBottom = (a.pos.y + a.size.y);
			const double bLeft = b.pos.x;
			const double bTop = b.pos.y;
			const double bRight = (b.pos.x + b.size.x);
			const double bBottom = (b.pos.y + b.size.y);

			return ((aLeft <= bRight)
				&& (bLeft <= aRight)
				&& (aTop <= bBottom)
				&& (bTop <= aBottom));
		}

		constexpr bool Intersects(const RectF& rect, const Circle& circle) noexcept
		{
			const auto rectKind = detail::ClassifyGeometry2DSizedShape(rect);

			if ((rectKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(rectKind))
			{
				return Intersects(detail::GetGeometry2DDegenerateSegment(rect, rectKind), circle);
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);
			const double closestX = (circle.center.x < left) ? left : ((right < circle.center.x) ? right : circle.center.x);
			const double closestY = (circle.center.y < top) ? top : ((bottom < circle.center.y) ? bottom : circle.center.y);
			const double dx = (closestX - circle.center.x);
			const double dy = (closestY - circle.center.y);

			return ((dx * dx + dy * dy) <= (circle.r * circle.r));
		}

		constexpr bool Intersects(const RectF& rect, const Ellipse& ellipse) noexcept
		{
			const auto rectKind = detail::ClassifyGeometry2DSizedShape(rect);
			const auto ellipseKind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if ((rectKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (ellipseKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(rectKind))
			{
				return Intersects(detail::GetGeometry2DDegenerateSegment(rect, rectKind), ellipse);
			}

			if (detail::IsGeometry2DSegment(ellipseKind))
			{
				return Intersects(rect, detail::GetGeometry2DDegenerateSegment(ellipse, ellipseKind));
			}

			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;
			const RectF localRect{
				((rect.pos.x - ellipse.center.x) / ax),
				((rect.pos.y - ellipse.center.y) / by),
				(rect.size.x / ax),
				(rect.size.y / by)
			};

			return Intersects(localRect, Circle{ Vec2{ 0.0, 0.0 }, 1.0 });
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Circle& circle, const Point& p) noexcept
		{
			return Intersects(p, circle);
		}

		constexpr bool Intersects(const Circle& circle, const Vec2& p) noexcept
		{
			return Intersects(p, circle);
		}

		constexpr bool Intersects(const Circle& circle, const Line& segment) noexcept
		{
			return Intersects(segment, circle);
		}

		constexpr bool Intersects(const Circle& circle, const Rect& rect) noexcept
		{
			return Intersects(rect, circle);
		}

		constexpr bool Intersects(const Circle& circle, const RectF& rect) noexcept
		{
			return Intersects(rect, circle);
		}

		constexpr bool Intersects(const Circle& a, const Circle& b) noexcept
		{
			if ((detail::ClassifyGeometry2DSizedShape(a) == detail::Geometry2DSizedShapeKind::Empty)
				|| (detail::ClassifyGeometry2DSizedShape(b) == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			const double r = (a.r + b.r);
			return (a.center.distanceFromSq(b.center) <= (r * r));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Ellipse& ellipse, const Point& p) noexcept
		{
			return Intersects(p, ellipse);
		}

		constexpr bool Intersects(const Ellipse& ellipse, const Vec2& p) noexcept
		{
			return Intersects(p, ellipse);
		}

		constexpr bool Intersects(const Ellipse& ellipse, const Line& segment) noexcept
		{
			return Intersects(segment, ellipse);
		}

		constexpr bool Intersects(const Ellipse& ellipse, const Rect& rect) noexcept
		{
			return Intersects(rect, ellipse);
		}

		constexpr bool Intersects(const Ellipse& ellipse, const RectF& rect) noexcept
		{
			return Intersects(rect, ellipse);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Triangle& triangle, const Point& p) noexcept
		{
			return Intersects(p, triangle);
		}

		constexpr bool Intersects(const Triangle& triangle, const Vec2& p) noexcept
		{
			return Intersects(p, triangle);
		}

		constexpr bool Intersects(const Triangle& triangle, const Line& segment) noexcept
		{
			return Intersects(segment, triangle);
		}

		constexpr bool Intersects(const Triangle& a, const Triangle& b) noexcept
		{
			if (Intersects(a.p0, b)
				|| Intersects(a.p1, b)
				|| Intersects(a.p2, b)
				|| Intersects(b.p0, a)
				|| Intersects(b.p1, a)
				|| Intersects(b.p2, a))
			{
				return true;
			}

			return (Intersects(Line{ a.p0, a.p1 }, b)
				|| Intersects(Line{ a.p1, a.p2 }, b)
				|| Intersects(Line{ a.p2, a.p0 }, b));
		}

		constexpr bool Intersects(const Triangle& triangle, const Quad& quad) noexcept
		{
			if (Intersects(triangle.p0, quad)
				|| Intersects(triangle.p1, quad)
				|| Intersects(triangle.p2, quad)
				|| Intersects(quad.p0, triangle)
				|| Intersects(quad.p1, triangle)
				|| Intersects(quad.p2, triangle)
				|| Intersects(quad.p3, triangle))
			{
				return true;
			}

			return (Intersects(Line{ triangle.p0, triangle.p1 }, quad)
				|| Intersects(Line{ triangle.p1, triangle.p2 }, quad)
				|| Intersects(Line{ triangle.p2, triangle.p0 }, quad));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Quad& quad, const Point& p) noexcept
		{
			return Intersects(p, quad);
		}

		constexpr bool Intersects(const Quad& quad, const Vec2& p) noexcept
		{
			return Intersects(p, quad);
		}

		constexpr bool Intersects(const Quad& quad, const Line& segment) noexcept
		{
			return Intersects(segment, quad);
		}

		constexpr bool Intersects(const Quad& quad, const Triangle& triangle) noexcept
		{
			return Intersects(triangle, quad);
		}

		constexpr bool Intersects(const Quad& a, const Quad& b) noexcept
		{
			if (Intersects(a.p0, b)
				|| Intersects(a.p1, b)
				|| Intersects(a.p2, b)
				|| Intersects(a.p3, b)
				|| Intersects(b.p0, a)
				|| Intersects(b.p1, a)
				|| Intersects(b.p2, a)
				|| Intersects(b.p3, a))
			{
				return true;
			}

			return (Intersects(Line{ a.p0, a.p1 }, b)
				|| Intersects(Line{ a.p1, a.p2 }, b)
				|| Intersects(Line{ a.p2, a.p3 }, b)
				|| Intersects(Line{ a.p3, a.p0 }, b));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const RoundRect& roundRect, const Point& p) noexcept
		{
			return Intersects(p, roundRect);
		}

		constexpr bool Intersects(const RoundRect& roundRect, const Vec2& p) noexcept
		{
			return Intersects(p, roundRect);
		}

		constexpr bool Intersects(const RoundRect& roundRect, const Line& segment) noexcept
		{
			return Intersects(segment, roundRect);
		}
	}
}
