//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <algorithm>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/detail/Bezier3ArcLengthTable.hpp>
# include <Siv3D/Geometry2D/BezierGeometry.hpp>

namespace s3d::detail
{
	inline double Bezier3SpeedIntegral5(const Bezier3& curve, const double a, const double b) noexcept
	{
		constexpr double X[] = { 0.0, -0.5384693101056831, 0.5384693101056831, -0.9061798459386640, 0.9061798459386640 };
		constexpr double W[] = { 0.5688888888888889, 0.4786286704993665, 0.4786286704993665, 0.2369268850561891, 0.2369268850561891 };
		const double middle = ((a + b) * 0.5);
		const double half = ((b - a) * 0.5);
		double result = 0.0;
		for (size_t i = 0; i < 5; ++i)
		{
			result += (W[i] * curve.derivativeAt(middle + half * X[i]).length());
		}
		return (half * result);
	}

	inline double RefineBezier3SpeedIntegral(const Bezier3& curve, const double a, const double b,
		const double estimate, const int32 depth) noexcept
	{
		const double middle = ((a + b) * 0.5);
		const double left = Bezier3SpeedIntegral5(curve, a, middle);
		const double right = Bezier3SpeedIntegral5(curve, middle, b);
		const double refined = (left + right);
		// Refinement also handles the non-smooth speed at a stationary point.
		if ((depth == 0) || (Abs(refined - estimate) <= (1e-11 * refined)))
		{
			return refined;
		}
		return (RefineBezier3SpeedIntegral(curve, a, middle, left, (depth - 1))
			+ RefineBezier3SpeedIntegral(curve, middle, b, right, (depth - 1)));
	}

	inline double IntegrateBezier3Speed(const Bezier3& curve, const double a, const double b) noexcept
	{
		return RefineBezier3SpeedIntegral(curve, a, b, Bezier3SpeedIntegral5(curve, a, b), 12);
	}

	inline Bezier3ArcLengthTable BuildBezier3ArcLengthTable(const Bezier3& curve) noexcept
	{
		Bezier3ArcLengthTable table{};
		constexpr double step = (1.0 / Bezier3ArcLengthTable::Subdivisions);
		for (size_t i = 0; i <= Bezier3ArcLengthTable::Subdivisions; ++i)
		{
			table.parameters[table.count++] = (i * step);
		}
		// Quadrature nodes can miss a reversal very close to an interval endpoint.
		// Put component extrema on interval boundaries once, during construction.
		const Vec2 d0 = (curve.p1 - curve.p0);
		const Vec2 d1 = (curve.p2 - curve.p1);
		const Vec2 d2 = (curve.p3 - curve.p2);
		const Vec2 a = (d0 - 2 * d1 + d2);
		const Vec2 b = (2 * (d1 - d0));
		const auto AddRoot = [&](const double t)
		{
			table.parameters[table.count++] = t;
			return false;
		};
		(void)CheckQuadraticRootsInUnitInterval(a.x, b.x, d0.x, AddRoot);
		(void)CheckQuadraticRootsInUnitInterval(a.y, b.y, d0.y, AddRoot);
		std::sort(table.parameters.begin(), (table.parameters.begin() + table.count));
		table.count = static_cast<size_t>(std::unique(table.parameters.begin(), (table.parameters.begin() + table.count)) - table.parameters.begin());
		for (size_t i = 1; i < table.count; ++i)
		{
			table.distances[i] = (table.distances[i - 1] + IntegrateBezier3Speed(curve, table.parameters[i - 1], table.parameters[i]));
		}
		return table;
	}

	inline double Bezier3DistanceAtT(const Bezier3& curve, const Bezier3ArcLengthTable& table, const double t) noexcept
	{
		if (t == 1.0)
		{
			return table.length();
		}
		const size_t cell = (static_cast<size_t>(std::upper_bound(table.parameters.begin(), (table.parameters.begin() + table.count), t) - table.parameters.begin()) - 1);
		return (table.distances[cell] + IntegrateBezier3Speed(curve, table.parameters[cell], t));
	}

	inline double Bezier3TAtDistance(const Bezier3& curve, const Bezier3ArcLengthTable& table, const double distance) noexcept
	{
		if ((distance <= 0.0) || (table.length() == 0.0))
		{
			return 0.0;
		}
		if (table.length() <= distance)
		{
			return 1.0;
		}
		const size_t cell = (static_cast<size_t>(std::upper_bound(table.distances.begin(), (table.distances.begin() + table.count), distance) - table.distances.begin()) - 1);
		const double a = table.parameters[cell];
		const double b = table.parameters[cell + 1];
		const double target = (distance - table.distances[cell]);
		double t = (a + (b - a) * target / (table.distances[cell + 1] - table.distances[cell]));
		double lower = a, upper = b;
		const double tolerance = (1e-10 * table.length());
		for (int32 iteration = 0; iteration < 32; ++iteration)
		{
			const double error = (IntegrateBezier3Speed(curve, a, t) - target);
			if (Abs(error) <= tolerance)
			{
				return t;
			}
			if (error < 0.0)
			{
				lower = t;
			}
			else
			{
				upper = t;
			}
			const double speed = curve.derivativeAt(t).length();
			double next = ((lower + upper) * 0.5);
			if (speed > 0.0)
			{
				const double candidate = (t - error / speed);
				if ((lower < candidate) && (candidate < upper))
				{
					next = candidate;
				}
			}
			t = next;
		}
		return t;
	}
}
