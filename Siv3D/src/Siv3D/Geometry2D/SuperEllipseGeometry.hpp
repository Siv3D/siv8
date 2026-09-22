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
}
