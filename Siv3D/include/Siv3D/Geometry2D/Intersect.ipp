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
		//
		//	http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
		//
		[[nodiscard]]
		constexpr double DistanceSq(const Vec2& start, const Vec2& end, const Vec2& p) noexcept
		{
			const double l2 = start.distanceFromSq(end);

			if (l2 == 0.0)
			{
				return start.distanceFromSq(p);
			}

			const double t = Max(0.0, Min(1.0, (p - start).dot(end - start) / l2));

			const Vec2 projection = (start + (end - start) * t);

			return p.distanceFromSq(projection);
		}

		[[nodiscard]]
		constexpr double Sign(const Vec2& p1, const Vec2& p2, const Vec2& p3) noexcept
		{
			return ((p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y));
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Point, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersect(const Point& a, const Point& b) noexcept
		{
			return (a == b);
		}

		constexpr bool Intersect(const Point& a, const Vec2& b) noexcept
		{
			return (b.distanceFromSq(a) < 1.0);
		}

		constexpr bool Intersect(const Point& a, const Line& b) noexcept
		{
			return (detail::DistanceSq(b.start, b.end, a) < 1.0);
		}

		constexpr bool Intersect(const Point& a, const Rect& b) noexcept
		{
			return ((b.pos.x <= a.x)
				 && (a.x < (b.pos.x + b.size.x))
				 && (b.pos.y <= a.y)
				 && (a.y < (b.pos.y + b.size.y)));
		}

		constexpr bool Intersect(const Point& a, const RectF& b) noexcept
		{
			return ((b.pos.x <= a.x)
				 && (a.x < (b.pos.x + b.size.x))
				 && (b.pos.y <= a.y)
				 && (a.y < (b.pos.y + b.size.y)));
		}

		constexpr bool Intersect(const Point& a, const Circle& b) noexcept
		{
			return (a.distanceFromSq(b.center) <= (b.r * b.r));
		}

		constexpr bool Intersect(const Point& a, const Ellipse& b) noexcept
		{
			if ((b.axes.x == 0.0)
				|| (b.axes.y == 0.0))
			{
				return false;
			}

			const double xh = (b.center.x - a.x);
			const double yk = (b.center.y - a.y);

			return (((xh * xh) / (b.axes.x * b.axes.x) + (yk * yk) / (b.axes.y * b.axes.y)) <= 1.0);
		}

		constexpr bool Intersect(const Point& a, const Triangle& b) noexcept
		{
			const bool b1 = (detail::Sign(a, b.p0, b.p1) < 0.0);
			const bool b2 = (detail::Sign(a, b.p1, b.p2) < 0.0);
			const bool b3 = (detail::Sign(a, b.p2, b.p0) < 0.0);

			return ((b1 == b2) && (b2 == b3));
		}

		constexpr bool Intersect(const Point& a, const Quad& b) noexcept
		{
			return (Intersect(a, b.triangleAtIndex(0))
				 || Intersect(a, b.triangleAtIndex(1)));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersect(const Vec2& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Vec2& a, const Vec2& b) noexcept
		{
			return (a.distanceFromSq(b) < 1.0);
		}

		constexpr bool Intersect(const Vec2& a, const Line& b) noexcept
		{
			return (detail::DistanceSq(b.start, b.end, a) < 1.0);
		}

		constexpr bool Intersect(const Vec2& a, const Rect& b) noexcept
		{
			return ((b.pos.x <= a.x) && (a.x < (b.pos.x + b.size.x))
				 && (b.pos.y <= a.y) && (a.y < (b.pos.y + b.size.y)));
		}

		constexpr bool Intersect(const Vec2& a, const RectF& b) noexcept
		{
			return ((b.pos.x <= a.x) && (a.x < (b.pos.x + b.size.x))
				 && (b.pos.y <= a.y) && (a.y < (b.pos.y + b.size.y)));
		}

		constexpr bool Intersect(const Vec2& a, const Circle& b) noexcept
		{
			return (a.distanceFromSq(b.center) <= (b.r * b.r));
		}

		constexpr bool Intersect(const Vec2& a, const Ellipse& b) noexcept
		{
			if ((b.axes.x == 0.0) || (b.axes.y == 0.0))
			{
				return false;
			}

			const double xh = (b.center.x - a.x);
			const double yk = (b.center.y - a.y);
			return (((xh * xh) / (b.axes.x * b.axes.x) + (yk * yk) / (b.axes.y * b.axes.y)) <= 1.0);
		}

		constexpr bool Intersect(const Vec2& a, const Triangle& b) noexcept
		{
			const bool b1 = (detail::Sign(a, b.p0, b.p1) < 0.0);
			const bool b2 = (detail::Sign(a, b.p1, b.p2) < 0.0);
			const bool b3 = (detail::Sign(a, b.p2, b.p0) < 0.0);
			return ((b1 == b2) && (b2 == b3));
		}

		constexpr bool Intersect(const Vec2& a, const Quad& b) noexcept
		{
			return ((Intersect(a, b.triangleAtIndex(0))
				 || Intersect(a, b.triangleAtIndex(1))));
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersect(Line, _)
		//
		////////////////////////////////////////////////////////////////

		constexpr bool Intersect(const Line& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Line& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Rect, _)
		//
		//////////////////////////////////////////////////

		constexpr bool Intersect(const Rect& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Rect& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Rect& a, const Rect& b) noexcept
		{
			return ((a.pos.x < (b.pos.x + b.size.x))
				 && (b.pos.x < (a.pos.x + a.size.x))
				 && (a.pos.y < (b.pos.y + b.size.y))
				 && (b.pos.y < (a.pos.y + a.size.y)));
		}

		constexpr bool Intersect(const Rect& a, const RectF& b) noexcept
		{
			return ((a.pos.x < (b.pos.x + b.size.x))
				 && (b.pos.x < (a.pos.x + a.size.x))
				 && (a.pos.y < (b.pos.y + b.size.y))
				 && (b.pos.y < (a.pos.y + a.size.y)));
		}

		constexpr bool Intersect(const Rect& a, const Circle& b) noexcept
		{
			return Intersect(RectF{ a }, b);
		}

		constexpr bool Intersect(const Rect& a, const Ellipse& b) noexcept
		{
			return Intersect(RectF{ a }, b);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(RectF, _)
		//
		//////////////////////////////////////////////////

		constexpr bool Intersect(const RectF& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const RectF& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const RectF& a, const Rect& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const RectF& a, const RectF& b) noexcept
		{
			return ((a.pos.x < (b.pos.x + b.size.x))
				 && (b.pos.x < (a.pos.x + a.size.x))
				 && (a.pos.y < (b.pos.y + b.size.y))
				 && (b.pos.y < (a.pos.y + a.size.y)));
		}

		constexpr bool Intersect(const RectF& a, const Circle& b) noexcept
		{
			const double aw = (a.size.x * 0.5);
			const double ah = (a.size.y * 0.5);
			const double cX = Abs(b.center.x - a.pos.x - aw);
			const double cY = Abs(b.center.y - a.pos.y - ah);

			if (((aw + b.r) < cX)
				|| ((ah + b.r) < cY))
			{
				return false;
			}

			if ((cX <= aw)
				|| (cY <= ah))
			{
				return true;
			}

			return ((cX - aw) * (cX - aw) + (cY - ah) * (cY - ah) <= (b.r * b.r));
		}

		constexpr bool Intersect(const RectF& a, const Ellipse& b) noexcept
		{
			RectF rect = a.movedBy(-b.center);

			const double v = (b.a / b.b);
			rect.y *= v;
			rect.h *= v;

			return Intersect(rect, Circle{ b.a });
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Circle, _)
		//
		//////////////////////////////////////////////////

		constexpr bool Intersect(const Circle& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Circle& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Circle& a, const Rect& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Circle& a, const RectF& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Circle& a, const Circle& b) noexcept
		{
			const double x = (a.center.x - b.center.x);
			const double y = (a.center.y - b.center.y);
			const double r = (a.r + b.r);
			return ((x * x + y * y) <= (r * r));
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Ellipse, _)
		//
		//////////////////////////////////////////////////

		constexpr bool Intersect(const Ellipse& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Ellipse& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Ellipse& a, const Rect& b) noexcept
		{
			return Intersect(RectF{ b }, a);
		}

		constexpr bool Intersect(const Ellipse& a, const RectF& b) noexcept
		{
			return Intersect(b, a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Triangle, _)
		//
		//////////////////////////////////////////////////

		constexpr bool Intersect(const Triangle& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Triangle& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}

		//////////////////////////////////////////////////
		//
		//	Intersect(Quad, _)
		//
		//////////////////////////////////////////////////

		constexpr bool Intersect(const Quad& a, const Point& b) noexcept
		{
			return Intersect(b, a);
		}

		constexpr bool Intersect(const Quad& a, const Vec2& b) noexcept
		{
			return Intersect(b, a);
		}
	}
}
