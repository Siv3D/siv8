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
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Point, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersects(const Point& p1, const Point& p2) noexcept
		{
			return (p1 == p2);
		}

		constexpr bool Intersects(const Point& p1, const Vec2& p2) noexcept
		{
			return (Vec2{ p1 } == p2);
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

		constexpr bool Intersects(const Vec2& p1, const Point& p2) noexcept
		{
			return Intersects(p2, p1);
		}

		constexpr bool Intersects(const Vec2& p1, const Vec2& p2) noexcept
		{
			return (p1 == p2);
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
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h));
				return false;
			}

			if ((w == 0.0) && (h == 0.0))
			{
				return false;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

			return ((left <= p.x)
				&& (p.x <= right)
				&& (top <= p.y)
				&& (p.y <= bottom));
		}

		constexpr bool Intersects(const Vec2& p, const Circle& circle) noexcept
		{
			if (circle.r < 0.0)
			{
				assert(0.0 <= circle.r);
				return false;
			}

			if (circle.r == 0.0)
			{
				return false;
			}

			return (p.distanceFromSq(circle.center) <= (circle.r * circle.r));
		}

		constexpr bool Intersects(const Vec2& p, const Ellipse& ellipse) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

			if ((ax < 0.0) || (by < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return ((p.x == ellipse.center.x)
					&& (Abs(p.y - ellipse.center.y) <= by));
			}

			if (by == 0.0)
			{
				return ((p.y == ellipse.center.y)
					&& (Abs(p.x - ellipse.center.x) <= ax));
			}

			const double dx = ((p.x - ellipse.center.x) / ax);
			const double dy = ((p.y - ellipse.center.y) / by);
			return ((dx * dx + dy * dy) <= 1.0);
		}

		constexpr bool Intersects(const Vec2& p, const RoundRect& roundRect) noexcept
		{
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));
				return false;
			}

			if ((w == 0.0) && (h == 0.0))
			{
				return false;
			}

			if ((w == 0.0) || (h == 0.0))
			{
				return Intersects(p, rect);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return Intersects(p, rect);
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (not ((left <= p.x)
				&& (p.x <= right)
				&& (top <= p.y)
				&& (p.y <= bottom)))
			{
				return false;
			}

			const double innerLeft = (left + er);
			const double innerRight = (right - er);
			const double innerTop = (top + er);
			const double innerBottom = (bottom - er);

			if (((innerLeft <= p.x) && (p.x <= innerRight))
				|| ((innerTop <= p.y) && (p.y <= innerBottom)))
			{
				return true;
			}

			const double cx = ((p.x < innerLeft) ? innerLeft : innerRight);
			const double cy = ((p.y < innerTop) ? innerTop : innerBottom);

			const double dx = (p.x - cx);
			const double dy = (p.y - cy);

			return ((dx * dx + dy * dy) <= (er * er));
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
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h));
				return false;
			}

			if ((w == 0.0) && (h == 0.0))
			{
				return false;
			}

			if (Intersects(segment.start, rect) || Intersects(segment.end, rect))
			{
				return true;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Intersects(segment, Line{ Vec2{ left, top }, Vec2{ left, bottom } });
			}

			if (h == 0.0)
			{
				return Intersects(segment, Line{ Vec2{ left, top }, Vec2{ right, top } });
			}

			return (Intersects(segment, Line{ Vec2{ left, top }, Vec2{ right, top } })
				|| Intersects(segment, Line{ Vec2{ right, top }, Vec2{ right, bottom } })
				|| Intersects(segment, Line{ Vec2{ right, bottom }, Vec2{ left, bottom } })
				|| Intersects(segment, Line{ Vec2{ left, bottom }, Vec2{ left, top } }));
		}

		constexpr bool Intersects(const Line& segment, const Circle& circle) noexcept
		{
			if (circle.r < 0.0)
			{
				assert(0.0 <= circle.r);
				return false;
			}

			if (circle.r == 0.0)
			{
				return false;
			}

			if (Intersects(segment.start, circle) || Intersects(segment.end, circle))
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

		constexpr bool Intersects(const Line& segment, const Ellipse& ellipse) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

			if ((ax < 0.0) || (by < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Intersects(segment, Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Intersects(segment, Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } });
			}

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
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));
				return false;
			}

			if ((w == 0.0) && (h == 0.0))
			{
				return false;
			}

			if ((w == 0.0) || (h == 0.0))
			{
				return Intersects(segment, rect);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return Intersects(segment, rect);
			}

			if (Intersects(segment.start, roundRect) || Intersects(segment.end, roundRect))
			{
				return true;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);
			const double innerLeft = (left + er);
			const double innerRight = (right - er);
			const double innerTop = (top + er);
			const double innerBottom = (bottom - er);

			if (Intersects(segment, RectF{ innerLeft, top, (innerRight - innerLeft), h })
				|| Intersects(segment, RectF{ left, innerTop, w, (innerBottom - innerTop) }))
			{
				return true;
			}

			return (detail::SegmentIntersectsDiskBox(segment, Vec2{ innerLeft, innerTop }, er, left, top, innerLeft, innerTop)
				|| detail::SegmentIntersectsDiskBox(segment, Vec2{ innerRight, innerTop }, er, innerRight, top, right, innerTop)
				|| detail::SegmentIntersectsDiskBox(segment, Vec2{ innerRight, innerBottom }, er, innerRight, innerBottom, right, bottom)
				|| detail::SegmentIntersectsDiskBox(segment, Vec2{ innerLeft, innerBottom }, er, left, innerBottom, innerLeft, bottom));
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
			return Intersects(a, RectF{ b });
		}

		constexpr bool Intersects(const RectF& a, const RectF& b) noexcept
		{
			const double aw = a.size.x;
			const double ah = a.size.y;
			const double bw = b.size.x;
			const double bh = b.size.y;

			if ((aw < 0.0) || (ah < 0.0) || (bw < 0.0) || (bh < 0.0))
			{
				assert((0.0 <= aw) && (0.0 <= ah) && (0.0 <= bw) && (0.0 <= bh));
				return false;
			}

			if (((aw == 0.0) && (ah == 0.0))
				|| ((bw == 0.0) && (bh == 0.0)))
			{
				return false;
			}

			const double aLeft = a.pos.x;
			const double aTop = a.pos.y;
			const double aRight = (a.pos.x + aw);
			const double aBottom = (a.pos.y + ah);
			const double bLeft = b.pos.x;
			const double bTop = b.pos.y;
			const double bRight = (b.pos.x + bw);
			const double bBottom = (b.pos.y + bh);

			return ((aLeft <= bRight)
				&& (bLeft <= aRight)
				&& (aTop <= bBottom)
				&& (bTop <= aBottom));
		}

		constexpr bool Intersects(const RectF& rect, const Circle& circle) noexcept
		{
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0) || (circle.r < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h) && (0.0 <= circle.r));
				return false;
			}

			if (((w == 0.0) && (h == 0.0)) || (circle.r == 0.0))
			{
				return false;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, circle);
			}

			if (h == 0.0)
			{
				return Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, circle);
			}

			const double closestX = (circle.center.x < left) ? left : ((right < circle.center.x) ? right : circle.center.x);
			const double closestY = (circle.center.y < top) ? top : ((bottom < circle.center.y) ? bottom : circle.center.y);
			const double dx = (closestX - circle.center.x);
			const double dy = (closestY - circle.center.y);

			return ((dx * dx + dy * dy) <= (circle.r * circle.r));
		}

		constexpr bool Intersects(const RectF& rect, const Ellipse& ellipse) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

			if ((ax < 0.0) || (by < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Intersects(rect, Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Intersects(rect, Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } });
			}

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
