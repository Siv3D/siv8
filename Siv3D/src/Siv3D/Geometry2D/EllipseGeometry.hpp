//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <Siv3D/Ellipse.hpp>
# include <Siv3D/RectF.hpp>

namespace s3d::detail
{
	inline constexpr double EllipseDistanceRootTolerance = (16.0 * 2.2204460492503131e-16);
	inline constexpr double EllipseContactTolerance = (64.0 * 2.2204460492503131e-16);

	[[nodiscard]]
	inline double DistancePointEllipse(const Vec2& p, const Ellipse& ellipse) noexcept
	{
		const double ax = ellipse.axes.x;
		const double by = ellipse.axes.y;
		const double x = Abs(p.x - ellipse.center.x);
		const double y = Abs(p.y - ellipse.center.y);

		const double nx = (x / ax);
		const double ny = (y / by);

		if (((nx * nx) + (ny * ny)) <= 1.0)
		{
			return 0.0;
		}

		if (y == 0.0)
		{
			return (x - ax);
		}

		if (x == 0.0)
		{
			return (y - by);
		}

		// For an outside point, the closest ellipse point is obtained from the
		// unique non-negative Lagrange multiplier lambda satisfying
		//   (a*x/(lambda+a^2))^2 + (b*y/(lambda+b^2))^2 = 1.
		// The left-hand side is strictly decreasing, so a bracketed Newton step
		// cannot converge to the wrong stationary point as the angle-based
		// unbracketed Newton iteration can.
		const double scale = Max({ ax, by, x, y });
		const double a = (ax / scale);
		const double b = (by / scale);
		const double px = (x / scale);
		const double py = (y / scale);
		const double aa = (a * a);
		const double bb = (b * b);

		double lower = 0.0;
		double upper = 1.0;

		const double ux0 = (px / a);
		const double uy0 = (py / b);
		const double f0 = ((ux0 * ux0) + (uy0 * uy0) - 1.0);
		const double df0 = (-2.0 * (((ux0 * ux0) / aa) + ((uy0 * uy0) / bb)));
		const double initialNewton = (-f0 / df0);
		double lambda = (((0.0 < initialNewton) && (initialNewton < 1.0)) ? initialNewton : 0.5);

		// With the normalization above, lambda = 1 is always outside the root:
		// each squared term is at most 1/4. Newton from lambda = 0 gives a
		// useful lower-side initial estimate, and every later step remains bracketed.
		for (int32 i = 0; i < 64; ++i)
		{
			const double da = (lambda + aa);
			const double db = (lambda + bb);
			const double ux = ((a * px) / da);
			const double uy = ((b * py) / db);
			const double f = ((ux * ux) + (uy * uy) - 1.0);

			if (Abs(f) <= EllipseDistanceRootTolerance)
			{
				lower = lambda;
				upper = lambda;
				break;
			}

			if (0.0 < f)
			{
				lower = lambda;
			}
			else
			{
				upper = lambda;
			}

			const double df = (-2.0 * (((ux * ux) / da) + ((uy * uy) / db)));
			const double newton = (lambda - (f / df));

			if ((lower < newton) && (newton < upper))
			{
				lambda = newton;
			}
			else
			{
				lambda = ((lower + upper) * 0.5);
			}
		}

		lambda = ((lower + upper) * 0.5);

		const double closestX = ((aa * px) / (lambda + aa));
		const double closestY = ((bb * py) / (lambda + bb));

		return (scale * std::hypot((px - closestX), (py - closestY)));
	}

	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool EllipseDistanceWithinRadius(const double distance, const double radius, const double scale) noexcept
	{
		const double tolerance = (EllipseContactTolerance * scale);
		return IncludeBoundary ? (distance <= (radius + tolerance)) : ((distance + tolerance) < radius);
	}

	// Positive-area ellipses. Mapping one ellipse to the unit disk preserves
	// intersection and positive area; axis ordering makes the rounding symmetric.
	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool TestEllipseEllipseArea(const Ellipse& a, const Ellipse& b) noexcept
	{
		const Vec2 centerDistance{ Abs(a.x - b.x), Abs(a.y - b.y) };
		const Vec2 axesSum = (a.axes + b.axes);
		const double boundsTolerance = IncludeBoundary ? (EllipseContactTolerance * Max(axesSum.x, axesSum.y)) : 0.0;
		if (((axesSum.x + boundsTolerance) < centerDistance.x) || ((axesSum.y + boundsTolerance) < centerDistance.y))
		{
			return false;
		}

		const bool swap = ((a.axes.x < b.axes.x) || ((a.axes.x == b.axes.x) && (a.axes.y < b.axes.y)));
		const Vec2 basis = (swap ? b.axes : a.axes);
		const Vec2 other = (swap ? a.axes : b.axes);
		const Vec2 delta{ (centerDistance.x / basis.x), (centerDistance.y / basis.y) };
		const Ellipse local{ Vec2{ 0, 0 }, (other.x / basis.x), (other.y / basis.y) };
		const double scale = Max({ 1.0, delta.x, delta.y, local.axes.x, local.axes.y });

		// The ellipse with summed axes lies inside the Minkowski sum. Its radial
		// coordinate bounds the normalized point-to-ellipse distance from above.
		const Vec2 normalized{ (centerDistance.x / axesSum.x), (centerDistance.y / axesSum.y) };
		if ((normalized.lengthSq() + (2.0 * EllipseContactTolerance * scale)) < 1.0)
		{
			return true;
		}

		return EllipseDistanceWithinRadius<IncludeBoundary>(DistancePointEllipse(delta, local), 1.0, scale);
	}

	// Positive radius. A RoundRect is its core rectangle expanded by a disk.
	// Clamping the ellipse center gives the closest core point to the ellipse.
	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool TestEllipseRoundRectArea(const Ellipse& ellipse, const RectF& core, const double radius) noexcept
	{
		const Vec2 point{ Clamp(ellipse.x, core.x, (core.x + core.w)), Clamp(ellipse.y, core.y, (core.y + core.h)) };
		const Vec2 delta{ Abs(point.x - ellipse.x), Abs(point.y - ellipse.y) };
		const double scale = Max({ ellipse.axes.x, ellipse.axes.y, radius });
		const double boundsTolerance = IncludeBoundary ? (EllipseContactTolerance * scale) : 0.0;
		if (((ellipse.axes.x + radius + boundsTolerance) < delta.x) || ((ellipse.axes.y + radius + boundsTolerance) < delta.y))
		{
			return false;
		}

		return EllipseDistanceWithinRadius<IncludeBoundary>(DistancePointEllipse(delta, Ellipse{ Vec2{ 0, 0 }, ellipse.axes }), radius, scale);
	}
}
