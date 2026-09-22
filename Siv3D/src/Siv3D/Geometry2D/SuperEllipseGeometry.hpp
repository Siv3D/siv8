//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <Siv3D/2DShapes.hpp>

namespace s3d::detail
{
	inline constexpr double SuperEllipseContactTolerance = (64.0 * 2.2204460492503131e-16);

	[[nodiscard]]
	inline RectF SuperEllipseLineTestBounds(const SuperEllipse& shape) noexcept
	{
		const Vec2 axes = (shape.axes * (1.0 + SuperEllipseContactTolerance));
		return RectF{ (shape.center - axes), (axes * 2.0) };
	}

	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool TestLineSuperEllipseArea(const Line& segment, const SuperEllipse& shape) noexcept
	{
		Vec2 p0{ ((segment.start.x - shape.x) / shape.a), ((segment.start.y - shape.y) / shape.b) };
		Vec2 p1{ ((segment.end.x - shape.x) / shape.a), ((segment.end.y - shape.y) / shape.b) };
		const double axisLimit = (IncludeBoundary ? (1.0 + SuperEllipseContactTolerance) : (1.0 - SuperEllipseContactTolerance));
		if (((axisLimit < p0.x) && (axisLimit < p1.x)) || ((p0.x < -axisLimit) && (p1.x < -axisLimit))
			|| ((axisLimit < p0.y) && (axisLimit < p1.y)) || ((p0.y < -axisLimit) && (p1.y < -axisLimit)))
		{
			return false;
		}

		const double n = shape.n;
		// Apply the contact tolerance in normalized coordinates independently of n.
		const double inverseLimit = (1.0 / axisLimit);
		auto TestPoint = [&](const Vec2& p) noexcept
		{
			const double value = (std::pow((Abs(p.x) * inverseLimit), n) + std::pow((Abs(p.y) * inverseLimit), n));
			return IncludeBoundary ? (value <= 1.0) : (value < 1.0);
		};
		auto TestAxis = [&](const double coordinate) noexcept
		{
			return IncludeBoundary ? (Abs(coordinate) <= axisLimit) : (Abs(coordinate) < axisLimit);
		};

		if (TestPoint(p0) || TestPoint(p1))
		{
			return true;
		}

		// Use the same arithmetic when the segment endpoints are reversed.
		if ((p1.x < p0.x) || ((p1.x == p0.x) && (p1.y < p0.y)))
		{
			std::swap(p0, p1);
		}
		const Vec2 d = (p1 - p0);
		if (d == Vec2{ 0, 0 })
		{
			return false;
		}

		if ((p0.x < 0.0) && (0.0 < p1.x)
			&& TestAxis(p0.y - p0.x * (d.y / d.x)))
		{
			return true;
		}
		if ((((p0.y < 0.0) && (0.0 < p1.y)) || ((p1.y < 0.0) && (0.0 < p0.y)))
			&& TestAxis(p0.x - p0.y * (d.x / d.y)))
		{
			return true;
		}

		// For n <= 1 the implicit value is concave in each quadrant, so its
		// segment minimum is at an endpoint or an axis crossing.
		if (n <= 1.0)
		{
			return false;
		}

		// For n > 1 it is convex. On the supporting line, the stationary point
		// satisfies |x/y|^(n-1) = |dy/dx|. Use a slope of magnitude <= 1.
		Vec2 closest;
		if (Abs(d.x) < Abs(d.y))
		{
			const double slope = (d.x / d.y);
			const double ratio = std::pow(Abs(slope), (1.0 / (n - 1.0)));
			closest.x = ((p0.x - slope * p0.y) / (1.0 + Abs(slope) * ratio));
			closest.y = (-std::copysign(ratio, slope) * closest.x);
			if ((closest.y < Min(p0.y, p1.y)) || (Max(p0.y, p1.y) < closest.y))
			{
				return false;
			}
		}
		else
		{
			const double slope = (d.y / d.x);
			const double ratio = std::pow(Abs(slope), (1.0 / (n - 1.0)));
			closest.y = ((p0.y - slope * p0.x) / (1.0 + Abs(slope) * ratio));
			closest.x = (-std::copysign(ratio, slope) * closest.y);
			if ((closest.x < p0.x) || (p1.x < closest.x))
			{
				return false;
			}
		}
		return TestPoint(closest);
	}

	// The upper boundary in the first quadrant is concave for n >= 1.
	struct ConvexSuperEllipseProfile
	{
		double a, b, n, inverseA, inverseN;

		ConvexSuperEllipseProfile(const Vec2& axes, const double exponent) noexcept
			: a{ axes.x }, b{ axes.y }, n{ exponent }, inverseA{ (1.0 / a) }, inverseN{ (1.0 / n) } {}

		[[nodiscard]]
		double height(const double x) const noexcept
		{
			if (x <= 0.0)
			{
				return b;
			}
			if (a <= x)
			{
				return 0.0;
			}
			const double u = (x * inverseA);
			if (n == 1.0)
			{
				return (b * (1.0 - u));
			}
			return (n == 2.0) ? (b * std::sqrt(Max(0.0, (1.0 - u * u))))
				: (b * std::pow(Max(0.0, (1.0 - std::pow(u, n))), inverseN));
		}

		struct Sample
		{
			double height, slope, curvature;
		};

		[[nodiscard]]
		Sample sample(const double x) const noexcept
		{
			const double u = (x * inverseA);
			if (n == 1.0)
			{
				return{ (b * (1.0 - u)), (-b * inverseA), 0.0 };
			}
			const double power = ((n == 2.0) ? (u * u) : std::pow(u, n));
			const double remaining = (1.0 - power);
			const double h = (b * ((n == 2.0) ? std::sqrt(remaining) : std::pow(remaining, inverseN)));
			const double slope = (-h * inverseA * power / (u * remaining));
			return{ h, slope, (slope * (n - 1.0) * inverseA / (u * remaining)) };
		}
	};

	// Positive-area shapes with n >= 1. Their center difference must lie in
	// the Minkowski sum; its boundary is max_x (heightA(x) + heightB(dx - x)).
	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool TestConvexSuperEllipseAreas(const SuperEllipse& a, const SuperEllipse& b) noexcept
	{
		Vec2 delta{ Abs(a.x - b.x), Abs(a.y - b.y) };
		Vec2 aa = a.axes, bb = b.axes;
		Vec2 sum = (aa + bb);
		const double limit = (IncludeBoundary ? (1.0 + SuperEllipseContactTolerance) : (1.0 - SuperEllipseContactTolerance));
		delta /= limit;
		if ((sum.x < delta.x) || (sum.y < delta.y))
		{
			return false;
		}
		const double nx = (delta.x / sum.x), ny = (delta.y / sum.y);
		// The diamond with summed axes lies inside the Minkowski sum, as does
		// the ellipse with summed axes when both exponents are at least 2.
		if (((nx + ny + SuperEllipseContactTolerance) < 1.0)
			|| ((2.0 <= a.n) && (2.0 <= b.n)
				&& ((nx * nx + ny * ny + 2.0 * SuperEllipseContactTolerance) < 1.0)))
		{
			return true;
		}
		if (ny < nx)
		{
			std::swap(delta.x, delta.y);
			std::swap(aa.x, aa.y);
			std::swap(bb.x, bb.y);
			std::swap(sum.x, sum.y);
		}
		double an = a.n, bn = b.n;
		// Use the smaller horizontal axis as the search variable, retaining
		// precision near its endpoints and identical arithmetic in both orders.
		if ((bb.x < aa.x) || ((bb.x == aa.x) && ((bb.y < aa.y) || ((bb.y == aa.y) && (bn < an)))))
		{
			std::swap(aa, bb);
			std::swap(an, bn);
		}
		const ConvexSuperEllipseProfile p{ aa, an }, q{ bb, bn };
		auto TestHeight = [&](const double h) noexcept
		{
			return IncludeBoundary ? (delta.y <= h) : (delta.y < h);
		};
		double left = Max(0.0, (delta.x - bb.x)), right = Min(aa.x, delta.x);
		if (TestHeight(Max((p.height(left) + q.height(delta.x - left)),
			(p.height(right) + q.height(delta.x - right)))))
		{
			return true;
		}
		if ((left == right) || ((an == 1.0) && (bn == 1.0)))
		{
			return false;
		}

		double x = (delta.x * (aa.x / sum.x));
		for (int32 i = 0; i < 64; ++i)
		{
			if (not ((left < x) && (x < right)))
			{
				x = (left + (right - left) * 0.5);
				if (not ((left < x) && (x < right)))
				{
					break;
				}
			}
			const auto s = p.sample(x), t = q.sample(delta.x - x);
			const double height = (s.height + t.height);
			if (TestHeight(height))
			{
				return true;
			}
			const double slope = (s.slope - t.slope);
			// A tangent to a concave function bounds its maximum from above.
			const double upper = (height + Max((slope * (left - x)), (slope * (right - x))));
			if ((slope == 0.0) || (IncludeBoundary ? (upper < delta.y) : (upper <= delta.y)))
			{
				return false;
			}
			if (0.0 < slope)
			{
				left = x;
			}
			else
			{
				right = x;
			}
			x -= (slope / (s.curvature + t.curvature));
		}
		// An unresolved search may intersect, but is not evidence of overlap.
		return IncludeBoundary;
	}

	// A RoundRect is its core expanded by a disk. For an axis-aligned convex
	// SuperEllipse, clamping its center gives the closest point of that core.
	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool TestConvexSuperEllipseRoundRectArea(const SuperEllipse& shape, const RectF& core, const double radius) noexcept
	{
		const Vec2 point{ Clamp(shape.x, core.x, (core.x + core.w)), Clamp(shape.y, core.y, (core.y + core.h)) };
		return TestConvexSuperEllipseAreas<IncludeBoundary>(shape, SuperEllipse{ point, radius, radius, 2.0 });
	}
}
