//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <array>
# include "BezierGeometry.hpp"
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/PolynomialSolver.hpp>

namespace s3d::detail
{
	inline constexpr double SuperEllipseContactTolerance = (64.0 * 2.2204460492503131e-16);

	struct BezierSuperEllipseIntersection
	{
		BezierIntersectionKind kind = BezierIntersectionKind::Separated;
		double parameter = 0.0;
	};

	// Positive axes, n != 1, 2, and a genuinely curved Bezier. Point/segment,
	// diamond, and ellipse reductions belong to the callers.
	template <class Bezier>
	[[nodiscard]]
	BezierSuperEllipseIntersection ClassifyBezierSuperEllipse(const Bezier& original, const SuperEllipse& shape)
	{
		constexpr int32 MaxNodes = 64;
		constexpr double AxisLimit = (1.0 + SuperEllipseContactTolerance);
		const bool reversed = BezierLexicographicalLess(original.reversed(), original);
		Bezier curve = (reversed ? original.reversed() : original);
		const auto Normalize = [&](const Vec2& p) { return ((p - shape.center) / shape.axes / AxisLimit); };
		curve.p0 = Normalize(curve.p0);
		curve.p1 = Normalize(curve.p1);
		curve.p2 = Normalize(curve.p2);
		if constexpr (std::is_same_v<Bezier, Bezier3>)
		{
			curve.p3 = Normalize(curve.p3);
		}
		const auto [lower, upper] = BezierControlBounds(curve);
		BezierSuperEllipseIntersection result;
		if ((1.0 < lower.x) || (upper.x < -1.0) || (1.0 < lower.y) || (upper.y < -1.0))
		{
			return result;
		}
		const auto Powers = [&](const Vec2& p)
		{
			return Vec2{ std::pow(Abs(p.x), shape.n), std::pow(Abs(p.y), shape.n) };
		};
		double bestValue = std::numeric_limits<double>::infinity();
		const auto TestPoint = [&](const double t)
		{
			const double parameter = (reversed ? (1.0 - t) : t);
			// Verify the witness with the same curve evaluation used by callers.
			Vec2 p = Normalize(original.pointAt(parameter));
			if ((1.0 < Abs(p.x)) || (1.0 < Abs(p.y)))
			{
				return false;
			}
			// At a concave tip, a sub-ulp coordinate error becomes a much
			// larger implicit-value error after pow(). Use spatial tolerance.
			if (shape.n < 1.0)
			{
				p.x = ((Abs(p.x) <= SuperEllipseContactTolerance) ? 0.0 : p.x);
				p.y = ((Abs(p.y) <= SuperEllipseContactTolerance) ? 0.0 : p.y);
			}
			const Vec2 powers = Powers(p);
			const double value = (powers.x + powers.y);
			if (value < bestValue)
			{
				bestValue = value;
				result.parameter = parameter;
			}
			if (value <= 1.0)
			{
				result = { BezierIntersectionKind::Contact, parameter };
				return true;
			}
			return false;
		};
		if (TestPoint(0.0) || TestPoint(1.0) || TestPoint(0.5))
		{
			return result;
		}
		// Axis crossings find narrow targets without subdividing to their size,
		// and include the sharp tips of concave SuperEllipses.
		const auto controls = BezierControlPoints(curve);
		constexpr size_t Count = std::tuple_size_v<decltype(controls)>;
		for (const bool useX : { true, false })
		{
			std::array<double, Count> coordinates;
			for (size_t i = 0; i < Count; ++i)
			{
				coordinates[i] = (useX ? controls[i].x : controls[i].y);
			}
			bool contact;
			if constexpr (Count == 3)
			{
				contact = CheckQuadraticRootsInUnitInterval(
					(coordinates[0] - 2.0 * coordinates[1] + coordinates[2]),
					(2.0 * (coordinates[1] - coordinates[0])), coordinates[0], TestPoint);
			}
			else
			{
				contact = CheckCubicRootsInUnitInterval(
					(-coordinates[0] + 3.0 * coordinates[1] - 3.0 * coordinates[2] + coordinates[3]),
					(3.0 * (coordinates[0] - 2.0 * coordinates[1] + coordinates[2])),
					(3.0 * (coordinates[1] - coordinates[0])), coordinates[0], TestPoint);
			}
			if (contact)
			{
				return result;
			}
		}
		struct Node
		{
			Bezier curve;
			double lower, upper;
		};
		// A visit replaces one node with at most two children.
		std::array<Node, MaxNodes + 1> stack;
		int32 count = 1;
		stack[0] = { curve, 0.0, 1.0 };
		for (int32 visit = 0; count && (visit < MaxNodes); ++visit)
		{
			const Node node = stack[--count];
			const auto [lo, hi] = BezierControlBounds(node.curve);
			const Vec2 near{ Max({ 0.0, lo.x, -hi.x }), Max({ 0.0, lo.y, -hi.y }) };
			if ((1.0 < near.x) || (1.0 < near.y))
			{
				continue;
			}
			const Vec2 nearPowers = Powers(near);
			if ((1.0 + 8.0 * SuperEllipseContactTolerance) < (nearPowers.x + nearPowers.y))
			{
				continue;
			}
			const double middle = ((node.lower + node.upper) * 0.5);
			if ((visit != 0) && TestPoint(middle))
			{
				return result;
			}
			Vec2 gradient;
			double offset;
			if (1.0 < shape.n)
			{
				// A tangent plane is a global lower bound of the convex implicit
				// function. Clamp its anchor to the target's box before taking powers.
				const Vec2 midpoint = curve.pointAt(middle);
				const Vec2 anchor{ Clamp(midpoint.x, -1.0, 1.0), Clamp(midpoint.y, -1.0, 1.0) };
				const Vec2 powers = Powers(anchor);
				gradient = { ((anchor.x == 0.0) ? 0.0 : (powers.x / anchor.x)),
					((anchor.y == 0.0) ? 0.0 : (powers.y / anchor.y)) };
				offset = ((powers.x + powers.y - 1.0) / shape.n - gradient.dot(anchor));
			}
			else
			{
				// On either side of an axis, |x|^n is concave, so its endpoint
				// chord is a lower bound. An interval crossing the axis has bound 0.
				const auto Chord = [&](const double lo, const double hi, const double near, const double nearPower)
				{
					if (near == 0.0)
					{
						return std::pair{ 0.0, 0.0 };
					}
					const double far = Max(Abs(lo), Abs(hi));
					if (near == far)
					{
						return std::pair{ 0.0, nearPower };
					}
					const double slope = ((std::pow(far, shape.n) - nearPower) / (far - near));
					return std::pair{ ((hi < 0.0) ? -slope : slope), (nearPower - slope * near) };
				};
				const auto [gx, bx] = Chord(lo.x, hi.x, near.x, nearPowers.x);
				const auto [gy, by] = Chord(lo.y, hi.y, near.y, nearPowers.y);
				gradient = { gx, gy };
				offset = (bx + by - 1.0);
			}
			const auto points = BezierControlPoints(node.curve);
			std::array<double, Count> projected;
			double roundingScale = Abs(offset);
			for (size_t i = 0; i < Count; ++i)
			{
				projected[i] = (points[i].dot(gradient) + offset);
				roundingScale = Max(roundingScale, (Abs(points[i].x * gradient.x) + Abs(points[i].y * gradient.y) + Abs(offset)));
			}
			const auto [minimum, parameter] = MinimumBezierValue(projected);
			if ((8.0 * SuperEllipseContactTolerance * roundingScale) < minimum)
			{
				continue;
			}
			// The minimizer of the lower bound is also a useful contact candidate.
			if ((parameter != 0.5) && TestPoint(node.lower + (node.upper - node.lower) * parameter))
			{
				return result;
			}
			const auto [left, right] = node.curve.split(0.5);
			// Visit the child containing the bound's minimizer first. A fixed
			// left-first search can spend the budget just outside a contact.
			if (parameter <= 0.5)
			{
				stack[count++] = { right, middle, node.upper };
				stack[count++] = { left, node.lower, middle };
			}
			else
			{
				stack[count++] = { left, node.lower, middle };
				stack[count++] = { right, middle, node.upper };
			}
		}
		result.kind = (count ? BezierIntersectionKind::Unresolved : BezierIntersectionKind::Separated);
		return result;
	}

	// Positive axes; the point is outside the filled shape.
	[[nodiscard]]
	Vec2 ClosestPointOnSuperEllipseBoundaryFromOutside(const Vec2& point, const SuperEllipse& shape) noexcept;

	// Positive axes and n > 2. Reflection reduces the boundary to one quadrant.
	// On each half, use the smaller normalized coordinate directly: taking a
	// fractional power of sin/cos near their zeros loses spatial precision.
	[[nodiscard]]
	inline Vec2 ClosestPointOnSuperEllipseBoundary(const Vec2& point, const SuperEllipse& shape) noexcept
	{
		const Vec2 delta = (point - shape.center);
		const Vec2 query{ Abs(delta.x), Abs(delta.y) };
		Vec2 best{ shape.a, 0.0 };
		double bestDistanceSq = query.distanceFromSq(best);
		auto Update = [&](const Vec2& candidate) noexcept
		{
			const double distanceSq = query.distanceFromSq(candidate);
			if (distanceSq < bestDistanceSq)
			{
				best = candidate;
				bestDistanceSq = distanceSq;
			}
			return distanceSq;
		};
		auto Result = [&]() noexcept
		{
			return (shape.center + Vec2{ std::copysign(best.x, delta.x), std::copysign(best.y, delta.y) });
		};
		Update(Vec2{ 0.0, shape.b });
		if (bestDistanceSq == 0.0)
		{
			return Result();
		}

		const double inverseN = (1.0 / shape.n);
		const double split = std::pow(0.5, inverseN);
		constexpr int32 Segments = 24;
		for (const bool transpose : { false, true })
		{
			const Vec2 axes = (transpose ? Vec2{ shape.b, shape.a } : shape.axes);
			auto PointAt = [&](const double t) noexcept
			{
				const Vec2 p{ (axes.x * split * t), (axes.y * std::pow((1.0 - 0.5 * std::pow(t, shape.n)), inverseN)) };
				return (transpose ? Vec2{ p.y, p.x } : p);
			};
			// This also retains a query lying on a numerically flat part of the boundary.
			Update(PointAt(Clamp(((transpose ? query.y : query.x) / (axes.x * split)), 0.0, 1.0)));
			if (bestDistanceSq == 0.0)
			{
				return Result();
			}
			std::array<double, Segments + 1> values;
			for (int32 i = 0; i <= Segments; ++i)
			{
				values[i] = Update(PointAt(static_cast<double>(i) / Segments));
			}
			if (bestDistanceSq == 0.0)
			{
				return Result();
			}
			// Interior queries can have several local minima. Refine each sampled
			// minimum, keeping the tips, samples, and projected query as candidates.
			for (int32 i = 0; i <= Segments; ++i)
			{
				if (((0 < i) && (values[i - 1] < values[i])) || ((i < Segments) && (values[i + 1] < values[i])))
				{
					continue;
				}
				double lo = (static_cast<double>(Max(0, (i - 1))) / Segments);
				double hi = (static_cast<double>(Min(Segments, (i + 1))) / Segments);
				constexpr double Fraction = 0.3819660112501051518;
				double left = (lo + Fraction * (hi - lo)), right = (hi - Fraction * (hi - lo));
				double leftValue = Update(PointAt(left)), rightValue = Update(PointAt(right));
				for (int32 iteration = 0; iteration < 80; ++iteration)
				{
					if (leftValue < rightValue)
					{
						hi = right;
						right = left;
						rightValue = leftValue;
						left = (lo + Fraction * (hi - lo));
						leftValue = Update(PointAt(left));
					}
					else
					{
						lo = left;
						left = right;
						leftValue = rightValue;
						right = (hi - Fraction * (hi - lo));
						rightValue = Update(PointAt(right));
					}
					if ((hi - lo) <= 2.0e-15)
					{
						break;
					}
				}
			}
		}
		return Result();
	}

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

	// The upper boundary in the first quadrant.
	struct SuperEllipseProfile
	{
		double a, b, n, inverseA, inverseN;

		SuperEllipseProfile(const Vec2& axes, const double exponent) noexcept
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

	// The area tests work with reflected centers and slightly expanded axes.
	// Only distance queries request a witness; predicate-only instantiations
	// discard this reconstruction and its additional profile evaluations.
	[[nodiscard]]
	inline Vec2 SuperEllipseCommonPoint(const SuperEllipse& a, const SuperEllipse& b, const Vec2& local) noexcept
	{
		return (a.center + Vec2{ std::copysign(local.x, (b.x - a.x)), std::copysign(local.y, (b.y - a.y)) }
			* (1.0 + SuperEllipseContactTolerance));
	}

	// Positive-area shapes with n <= 1. Both height functions are convex,
	// so heightA(x) + heightB(dx - x) has its maximum at a domain endpoint.
	template <bool IncludeBoundary, bool FindPoint = false>
	[[nodiscard]]
	inline bool TestConcaveSuperEllipseAreas(const SuperEllipse& a, const SuperEllipse& b, Optional<Vec2>* commonPoint = nullptr) noexcept
	{
		Vec2 delta{ Abs(a.x - b.x), Abs(a.y - b.y) };
		const double limit = (IncludeBoundary ? (1.0 + SuperEllipseContactTolerance) : (1.0 - SuperEllipseContactTolerance));
		delta /= limit;
		const Vec2 sum = (a.axes + b.axes);
		if ((sum.x < delta.x) || (sum.y < delta.y))
		{
			return false;
		}
		const SuperEllipseProfile p{ a.axes, a.n }, q{ b.axes, b.n };
		const auto TestHeight = [&](const double x, const double ha, const double hb) noexcept
		{
			const bool hit = (IncludeBoundary ? (delta.y <= (ha + hb)) : (delta.y < (ha + hb)));
			if constexpr (FindPoint)
			{
				if (hit)
				{
					const double y = ((Max(0.0, (delta.y - hb)) + Min(delta.y, ha)) * 0.5);
					*commonPoint = SuperEllipseCommonPoint(a, b, Vec2{ x, y });
				}
			}
			return hit;
		};
		const double left = Max(0.0, (delta.x - b.a)), right = Min(a.a, delta.x);
		return TestHeight(left, p.height(left), q.height(delta.x - left))
			|| TestHeight(right, p.height(right), q.height(delta.x - right));
	}

	// Positive-area shapes with n >= 1. Their center difference must lie in
	// the Minkowski sum; its boundary is max_x (heightA(x) + heightB(dx - x)).
	template <bool IncludeBoundary, bool FindPoint = false>
	[[nodiscard]]
	inline bool TestConvexSuperEllipseAreas(const SuperEllipse& a, const SuperEllipse& b, Optional<Vec2>* commonPoint = nullptr) noexcept
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
			if constexpr (FindPoint)
			{
				*commonPoint = SuperEllipseCommonPoint(a, b, (delta * aa / sum));
			}
			return true;
		}
		const bool swapAxes = (ny < nx);
		if (swapAxes)
		{
			std::swap(delta.x, delta.y);
			std::swap(aa.x, aa.y);
			std::swap(bb.x, bb.y);
			std::swap(sum.x, sum.y);
		}
		double an = a.n, bn = b.n;
		// Use the smaller horizontal axis as the search variable, retaining
		// precision near its endpoints and identical arithmetic in both orders.
		const bool swapShapes = ((bb.x < aa.x) || ((bb.x == aa.x) && ((bb.y < aa.y) || ((bb.y == aa.y) && (bn < an)))));
		if (swapShapes)
		{
			std::swap(aa, bb);
			std::swap(an, bn);
		}
		const SuperEllipseProfile p{ aa, an }, q{ bb, bn };
		const auto TestHeight = [&](const double x, const double ha, const double hb) noexcept
		{
			const bool hit = (IncludeBoundary ? (delta.y <= (ha + hb)) : (delta.y < (ha + hb)));
			if constexpr (FindPoint)
			{
				if (hit)
				{
					Vec2 point{ x, ((Max(0.0, (delta.y - hb)) + Min(delta.y, ha)) * 0.5) };
					if (swapShapes)
					{
						point = (delta - point);
					}
					if (swapAxes)
					{
						std::swap(point.x, point.y);
					}
					*commonPoint = SuperEllipseCommonPoint(a, b, point);
				}
			}
			return hit;
		};
		double left = Max(0.0, (delta.x - bb.x)), right = Min(aa.x, delta.x);
		if (TestHeight(left, p.height(left), q.height(delta.x - left))
			|| TestHeight(right, p.height(right), q.height(delta.x - right)))
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
			if (TestHeight(x, s.height, t.height))
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

	// One term of the convex shape's implicit value along a concave boundary:
	// f(t) = (d - c*t^p)^m, with p > 1, m > 1 and d - c*t^p >= 0.
	struct MixedSuperEllipseTerm
	{
		double d, c, p, m;
		std::array<double, 4> fourthCoefficients;
		double slopeMinimum;

		MixedSuperEllipseTerm(const double offset, const double scale, const double power, const double exponent) noexcept
			: d{ offset }, c{ scale }, p{ power }, m{ exponent }
			, fourthCoefficients{
				(-c * m * p * (p - 1) * (p - 2) * (p - 3)),
				(c * c * m * (m - 1) * p * p * (p - 1) * (7 * p - 11)),
				(-6 * c * c * c * m * (m - 1) * (m - 2) * p * p * p * (p - 1)),
				(c * c * c * c * m * (m - 1) * (m - 2) * (m - 3) * p * p * p * p) }
			, slopeMinimum{ std::pow(((p - 1) * d / (c * (p * m - 1))), (1 / p)) } {}

		[[nodiscard]]
		double value(const double t) const noexcept
		{
			const double z = Max(0.0, (d - c * std::pow(t, p)));
			return (m == 2.0) ? (z * z) : std::pow(z, m);
		}

		struct Sample
		{
			double value, slope, curvature, third;
		};

		[[nodiscard]]
		Sample sample(const double t) const noexcept
		{
			const double power = std::pow(t, p);
			const double z = Max(0.0, (d - c * power));
			const double f = ((m == 2.0) ? (z * z) : std::pow(z, m));
			const double r = (c * p * power / (t * z)), inverseT = (1 / t);
			return{ f, (-m * f * r),
				(m * f * ((m - 1) * r * r - (p - 1) * r * inverseT)),
				(m * f * (-(m - 1) * (m - 2) * r * r * r
					+ 3 * (m - 1) * (p - 1) * r * r * inverseT
					- (p - 1) * (p - 2) * r * inverseT * inverseT)) };
		}

		[[nodiscard]]
		std::pair<double, double> slopeBounds(const double left, const double right) const noexcept
		{
			auto Slope = [&](const double t) noexcept
			{
				return (-m * c * p * std::pow(t, (p - 1)) * std::pow(Max(0.0, (d - c * std::pow(t, p))), (m - 1)));
			};
			const double a = Slope(left), b = Slope(right);
			const double lower = (((left < slopeMinimum) && (slopeMinimum < right))
				? Min(Min(a, b), Slope(slopeMinimum)) : Min(a, b));
			return{ lower, Max(a, b) };
		}

		// Each derivative is a sum of C_k*t^(k*p-order)*(d-c*t^p)^(m-k).
		// Each power product has at most one interior extremum. Bounding the
		// terms separately gives a lower bound on the whole derivative.
		template <size_t Order>
		[[nodiscard]]
		double lowerDerivative(const double left, const double right, const std::array<double, Order>& coefficients) const noexcept
		{
			double result = 0.0;
			for (size_t i = 0; i < Order; ++i)
			{
				const double coefficient = coefficients[i];
				if (coefficient == 0.0)
				{
					continue;
				}
				const double k = static_cast<double>(i + 1);
				const double tExponent = (k * p - Order), zExponent = (m - k);
				auto Product = [&](const double t) noexcept
				{
					return (std::pow(t, tExponent) * std::pow(Max(0.0, (d - c * std::pow(t, p))), zExponent));
				};
				const double a = Product(left), b = Product(right);
				double extremum = ((0.0 < coefficient) ? Min(a, b) : Max(a, b));
				const double criticalPower = (tExponent * d / (c * (p * m - Order)));
				if (0.0 < criticalPower)
				{
					const double t = std::pow(criticalPower, (1 / p));
					if ((left < t) && (t < right))
					{
						const double v = Product(t);
						extremum = ((0.0 < coefficient) ? Min(extremum, v) : Max(extremum, v));
					}
				}
				result += (coefficient * extremum);
			}
			return result;
		}

		[[nodiscard]]
		double lowerCurvature(const double left, const double right) const noexcept
		{
			return lowerDerivative<2>(left, right,
				{ (-c * m * p * (p - 1)), (c * c * m * (m - 1) * p * p) });
		}
	};

	// Positive-area shapes with a.n < 1 < b.n. The relevant concave arc is
	// (a.a*t^p, a.b*(1-t)^p), p=1/a.n. Minimize b's implicit value on it.
	template <bool IncludeBoundary, bool FindPoint = false>
	[[nodiscard]]
	inline bool TestMixedSuperEllipseAreas(const SuperEllipse& a, const SuperEllipse& b, Optional<Vec2>* commonPoint = nullptr) noexcept
	{
		Vec2 delta{ Abs(a.x - b.x), Abs(a.y - b.y) };
		const double limit = (IncludeBoundary ? (1.0 + SuperEllipseContactTolerance) : (1.0 - SuperEllipseContactTolerance));
		delta /= limit;
		const Vec2 sum = (a.axes + b.axes);
		if ((sum.x < delta.x) || (sum.y < delta.y))
		{
			return false;
		}
		if ((delta.x == 0.0) || (delta.y == 0.0))
		{
			const bool hit = (IncludeBoundary ? ((delta.x <= sum.x) && (delta.y <= sum.y))
				: ((delta.x < sum.x) && (delta.y < sum.y)));
			if constexpr (FindPoint)
			{
				if (hit)
				{
					*commonPoint = SuperEllipseCommonPoint(a, b, (delta * a.axes / sum));
				}
			}
			return hit;
		}
		const double nx = std::pow((delta.x / a.a), a.n), ny = std::pow((delta.y / a.b), a.n);
		if ((nx + ny) <= 1.0)
		{
			if constexpr (FindPoint)
			{
				*commonPoint = b.center;
			}
			return true;
		}
		const SuperEllipseProfile pa{ a.axes, a.n }, pb{ b.axes, b.n };
		const double height = Max(((delta.x <= b.a) ? (a.b + pb.height(delta.x)) : pa.height(delta.x - b.a)),
			((delta.x <= a.a) ? (b.b + pa.height(delta.x)) : pb.height(delta.x - a.a)));
		// Evaluate tips in height coordinates, before converting to t. This
		// retains precision when one curve is very thin near the other's tip.
		if (IncludeBoundary ? (delta.y <= height) : (delta.y < height))
		{
			if constexpr (FindPoint)
			{
				const double left = Max(0.0, (delta.x - b.a)), right = Min(a.a, delta.x);
				const double ha = pa.height(left), hb = pb.height(delta.x - left);
				const bool useLeft = (delta.y <= (ha + hb));
				const double x = (useLeft ? left : right);
				const double y = ((Max(0.0, (delta.y - (useLeft ? hb : pb.height(delta.x - right))))
					+ Min(delta.y, (useLeft ? ha : pa.height(right)))) * 0.5);
				*commonPoint = SuperEllipseCommonPoint(a, b, Vec2{ x, y });
			}
			return true;
		}
		const double left = Max((1.0 - ny), std::pow((Max(0.0, (delta.x - b.a)) / a.a), a.n));
		const double right = Min(nx, (1.0 - std::pow((Max(0.0, (delta.y - b.b)) / a.b), a.n)));
		if (right < left)
		{
			return false;
		}
		const MixedSuperEllipseTerm f{ (delta.x / b.a), (a.a / b.a), (1.0 / a.n), b.n };
		const MixedSuperEllipseTerm g{ (delta.y / b.b), (a.b / b.b), (1.0 / a.n), b.n };
		auto Value = [&](const double t) noexcept { return (f.value(t) + g.value(1.0 - t)); };
		const auto Hit = [&](const double v, const double t) noexcept
		{
			const bool hit = (IncludeBoundary ? (v <= 1.0) : (v < 1.0));
			if constexpr (FindPoint)
			{
				if (hit)
				{
					*commonPoint = SuperEllipseCommonPoint(a, b,
						Vec2{ (a.a * std::pow(t, (1.0 / a.n))), (a.b * std::pow(1.0 - t, (1.0 / a.n))) });
				}
			}
			return hit;
		};
		auto Separated = [&](const double v) noexcept { return IncludeBoundary ? (1.0 < v) : (1.0 <= v); };
		if (Hit(Value(left), left) || Hit(Value(right), right))
		{
			return true;
		}
		struct Interval { double left, right; };
		std::array<Interval, 66> stack;
		size_t stackSize = 1;
		stack[0] = { left, right };
		constexpr int32 MaxIterations = 128;
		int32 iterations = 0;
		while (stackSize && (iterations < MaxIterations))
		{
			++iterations;
			const auto [l, r] = stack[--stackSize];
			if (Separated(f.value(r) + g.value(1.0 - l)))
			{
				continue;
			}
			if ((l == 0.0) || (r == 1.0))
			{
				const auto [fl, fh] = f.slopeBounds(l, r);
				const auto [gl, gh] = g.slopeBounds((1.0 - r), (1.0 - l));
				if ((0.0 <= (fl - gh)) || ((fh - gl) <= 0.0))
				{
					continue; // A monotone interval's endpoints have already been tested.
				}
			}
			const double middle = (l + (r - l) * 0.5), width = ((r - l) * 0.5);
			if (not ((l < middle) && (middle < r)))
			{
				return IncludeBoundary;
			}
			const auto s = f.sample(middle), t = g.sample(1.0 - middle);
			const double c0 = (s.value + t.value);
			if (Hit(c0, middle))
			{
				return true;
			}
			const bool convex = ((f.slopeMinimum <= l) && (r <= (1.0 - g.slopeMinimum)));
			if (not convex)
			{
				// A fourth-order lower Taylor bound also resolves osculating
				// contacts, where the quadratic term vanishes. Scale to [-1,1].
				const double c1 = ((s.slope - t.slope) * width);
				const double c2 = ((s.curvature + t.curvature) * width * width * 0.5);
				const double c3 = ((s.third - t.third) * width * width * width / 6.0);
				const double c4 = ((f.lowerDerivative(l, r, f.fourthCoefficients)
					+ g.lowerDerivative((1.0 - r), (1.0 - l), g.fourthCoefficients)) * width * width * width * width / 24.0);
				// Derivatives can be unbounded at ordinary curve endpoints.
				if (std::isfinite(c4) && std::isfinite(c3))
				{
					auto Polynomial = [&](const double x) noexcept { return ((((c4 * x + c3) * x + c2) * x + c1) * x + c0); };
					double lower = Min(Polynomial(-1.0), Polynomial(1.0)), minimumAt = 0.0;
					for (const double root : Math::SolveCubicEquation((4 * c4), (3 * c3), (2 * c2), c1))
					{
						if ((-1.0 < root) && (root < 1.0))
						{
							const double v = Polynomial(root);
							if (v < lower)
							{
								lower = v;
								minimumAt = root;
							}
						}
					}
					if (Hit(Value(middle + width * minimumAt), (middle + width * minimumAt)))
					{
						return true;
					}
					lower -= (SuperEllipseContactTolerance * (Abs(c0) + Abs(c1) + Abs(c2) + Abs(c3) + Abs(c4)));
					if (Separated(lower))
					{
						continue;
					}
				}
			}
			if (convex || (0.0 <= (f.lowerCurvature(l, r) + g.lowerCurvature((1.0 - r), (1.0 - l)))))
			{
				double lo = l, hi = r, x = middle;
				auto fs = s, gs = t;
				while (iterations < MaxIterations)
				{
					const double h = (fs.value + gs.value), slope = (fs.slope - gs.slope);
					if (Hit(h, x))
					{
						return true;
					}
					// A tangent bounds a convex function from below.
					const double lower = (h + Min((slope * (lo - x)), (slope * (hi - x)))
						- SuperEllipseContactTolerance * (Abs(h) + Abs(slope) * (hi - lo)));
					if ((slope == 0.0) || Separated(lower))
					{
						break;
					}
					if (slope < 0.0)
					{
						lo = x;
					}
					else
					{
						hi = x;
					}
					const double next = (x - slope / (fs.curvature + gs.curvature));
					x = (((lo < next) && (next < hi)) ? next : (lo + (hi - lo) * 0.5));
					if (not ((lo < x) && (x < hi)))
					{
						return IncludeBoundary;
					}
					fs = f.sample(x);
					gs = g.sample(1.0 - x);
					++iterations;
				}
				if (iterations == MaxIterations)
				{
					return IncludeBoundary;
				}
				continue;
			}
			if (stack.size() < (stackSize + 2))
			{
				return IncludeBoundary;
			}
			stack[stackSize++] = { middle, r };
			stack[stackSize++] = { l, middle };
		}
		// Unresolved intervals may intersect, but do not establish overlap.
		return (stackSize && IncludeBoundary);
	}

	template <bool IncludeBoundary, bool FindPoint = false>
	[[nodiscard]]
	inline bool TestSuperEllipseAreas(const SuperEllipse& a, const SuperEllipse& b, Optional<Vec2>* commonPoint = nullptr) noexcept
	{
		static_assert(IncludeBoundary || not FindPoint);
		if ((1.0 <= a.n) && (1.0 <= b.n))
		{
			return TestConvexSuperEllipseAreas<IncludeBoundary, FindPoint>(a, b, commonPoint);
		}
		if ((a.n <= 1.0) && (b.n <= 1.0))
		{
			return TestConcaveSuperEllipseAreas<IncludeBoundary, FindPoint>(a, b, commonPoint);
		}
		return (a.n < 1.0) ? TestMixedSuperEllipseAreas<IncludeBoundary, FindPoint>(a, b, commonPoint)
			: TestMixedSuperEllipseAreas<IncludeBoundary, FindPoint>(b, a, commonPoint);
	}

	// A RoundRect is its core expanded by a disk. For any axis-aligned
	// SuperEllipse, distance cannot increase towards its center on either axis.
	template <bool IncludeBoundary>
	[[nodiscard]]
	inline bool TestSuperEllipseRoundRectArea(const SuperEllipse& shape, const RectF& core, const double radius) noexcept
	{
		const Vec2 point{ Clamp(shape.x, core.x, (core.x + core.w)), Clamp(shape.y, core.y, (core.y + core.h)) };
		return TestSuperEllipseAreas<IncludeBoundary>(shape, SuperEllipse{ point, radius, radius, 2.0 });
	}
}
