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

	// Positive axes and a query in the first quadrant, relative to the center.
	[[nodiscard]]
	inline Vec2 ClosestPointOnEllipseBoundaryInFirstQuadrant(Vec2 query, Vec2 axes) noexcept
	{
		if (axes.x == axes.y)
		{
			const double length = std::hypot(query.x, query.y);
			return (length == 0.0) ? Vec2{ axes.x, 0.0 } : (query * (axes.x / length));
		}

		const bool transpose = (axes.x < axes.y);
		if (transpose)
		{
			std::swap(axes.x, axes.y);
			std::swap(query.x, query.y);
		}

		Vec2 closest;
		if (query.y == 0.0)
		{
			// On the major axis, the closest point need not be an axis endpoint.
			const double cutoff = ((axes.x - axes.y) * (1.0 + axes.y / axes.x));
			if (query.x < cutoff)
			{
				const double u = (query.x / cutoff);
				closest = { (axes.x * u), (axes.y * std::sqrt((1.0 - u) * (1.0 + u))) };
			}
			else
			{
				closest = { axes.x, 0.0 };
			}
		}
		else if (query.x == 0.0)
		{
			closest = { 0.0, axes.y };
		}
		else
		{
			const double scale = Max({ axes.x, query.x, query.y });
			const double a = (axes.x / scale), b = (axes.y / scale);
			const double ax = (a * (query.x / scale)), by = (b * (query.y / scale));
			const double gap = ((a - b) * (a + b));
			// Set mu = lambda + b^2 in the Lagrange-multiplier equation:
			// (a*x/(mu + a^2 - b^2))^2 + (b*y/mu)^2 = 1.
			// It decreases strictly for mu > 0, for both interior and exterior queries.
			// The shift avoids subtracting nearly equal numbers near the major axis.
			double lower = Max(by, (ax - gap));
			double upper = std::hypot(ax, by);
			double mu = lower;
			for (int32 i = 0; i < 64; ++i)
			{
				const double da = (mu + gap);
				const double ux = (ax / da), uy = (by / mu);
				const double f = ((ux * ux) + (uy * uy) - 1.0);
				if (Abs(f) <= EllipseDistanceRootTolerance)
				{
					break;
				}
				if (0.0 < f)
				{
					lower = mu;
				}
				else
				{
					upper = mu;
				}
				const double derivative = (-2.0 * (((ux * ux) / da) + ((uy * uy) / mu)));
				const double newton = (mu - f / derivative);
				mu = ((lower < newton) && (newton < upper)) ? newton : ((lower + upper) * 0.5);
			}
			closest = { (axes.x * (ax / (mu + gap))), (axes.y * (by / mu)) };
		}
		return transpose ? Vec2{ closest.y, closest.x } : closest;
	}

	// Positive axes. The query may be inside, outside, or on the boundary.
	[[nodiscard]]
	inline Vec2 ClosestPointOnEllipseBoundary(const Vec2& point, const Ellipse& ellipse) noexcept
	{
		const Vec2 delta = (point - ellipse.center);
		const Vec2 closest = ClosestPointOnEllipseBoundaryInFirstQuadrant(Vec2{ Abs(delta.x), Abs(delta.y) }, ellipse.axes);
		return (ellipse.center + Vec2{ std::copysign(closest.x, delta.x), std::copysign(closest.y, delta.y) });
	}

	[[nodiscard]]
	inline double DistancePointEllipse(const Vec2& p, const Ellipse& ellipse) noexcept
	{
		const Vec2 query{ Abs(p.x - ellipse.center.x), Abs(p.y - ellipse.center.y) };
		const Vec2 normalized = (query / ellipse.axes);
		if (normalized.lengthSq() <= 1.0)
		{
			return 0.0;
		}
		if (query.y == 0.0)
		{
			return (query.x - ellipse.axes.x);
		}
		if (query.x == 0.0)
		{
			return (query.y - ellipse.axes.y);
		}
		if (ellipse.axes.x == ellipse.axes.y)
		{
			return (std::hypot(query.x, query.y) - ellipse.axes.x);
		}
		const Vec2 closest = ClosestPointOnEllipseBoundaryInFirstQuadrant(query, ellipse.axes);
		return std::hypot((query.x - closest.x), (query.y - closest.y));
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
