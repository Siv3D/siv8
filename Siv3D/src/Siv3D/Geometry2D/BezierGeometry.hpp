//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <array>
# include <utility>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/Line.hpp>
# include <Siv3D/Quad.hpp>
# include <Siv3D/SuperEllipse.hpp>
# include <Siv3D/PolynomialSolver.hpp>

namespace s3d::detail
{
	inline constexpr double BezierRootTolerance = (64.0 * 2.2204460492503131e-16);

	template <class Fty>
	[[nodiscard]]
	bool CheckQuadraticRootsInUnitInterval(const double a, const double b, const double c, Fty&& callback)
	{
		// Normalize by the polynomial's own scale. Using a fixed 1.0 floor here
		// incorrectly classifies valid small-scale polynomials as identically zero.
		const double coefficientScale = Max({ Abs(a), Abs(b), Abs(c) });

		if (coefficientScale == 0.0)
		{
			return false;
		}

		const double na = (a / coefficientScale);
		const double nb = (b / coefficientScale);
		const double nc = (c / coefficientScale);

		auto CheckRoot = [&](double t)
		{
			if (InRange(t, -BezierRootTolerance, (1.0 + BezierRootTolerance)))
			{
				t = Clamp(t, 0.0, 1.0);
				return callback(t);
			}

			return false;
		};

		if (Abs(na) <= BezierRootTolerance)
		{
			if (Abs(nb) <= BezierRootTolerance)
			{
				return false;
			}

			return CheckRoot(-nc / nb);
		}

		const double discriminantScale = (Abs(nb * nb) + Abs(4.0 * na * nc));
		const double discriminantTolerance = (BezierRootTolerance * discriminantScale);
		double discriminant = std::fma(nb, nb, -4.0 * na * nc);

		if (discriminant < -discriminantTolerance)
		{
			return false;
		}

		if (discriminant < 0.0)
		{
			discriminant = 0.0;
		}

		const double s = std::sqrt(discriminant);

		if (s == 0.0)
		{
			return CheckRoot(-nb / (2.0 * na));
		}

		const double q = (-0.5 * (nb + ((nb < 0.0) ? -s : s)));

		if (q == 0.0)
		{
			return CheckRoot(-nb / (2.0 * na));
		}

		if (CheckRoot(q / na))
		{
			return true;
		}

		return CheckRoot(nc / q);
	}

	template <class Fty>
	[[nodiscard]]
	bool CheckCubicRootsInUnitInterval(const double a, const double b, const double c, const double d, Fty&& callback)
	{
		const double coefficientScale = Max({ Abs(a), Abs(b), Abs(c), Abs(d) });

		if (coefficientScale == 0.0)
		{
			return false;
		}

		const double na = (a / coefficientScale);
		const double nb = (b / coefficientScale);
		const double nc = (c / coefficientScale);
		const double nd = (d / coefficientScale);

		if (Abs(na) <= BezierRootTolerance)
		{
			return CheckQuadraticRootsInUnitInterval(nb, nc, nd, callback);
		}

		const auto roots = Math::SolveCubicEquation(na, nb, nc, nd);

		for (const double t0 : roots)
		{
			if (InRange(t0, -BezierRootTolerance, (1.0 + BezierRootTolerance)))
			{
				const double t = Clamp(t0, 0.0, 1.0);

				if (callback(t))
				{
					return true;
				}
			}
		}

		return false;
	}

	// Roots in (0, 1) of a Bernstein polynomial. Derivative roots divide the
	// interval into monotone pieces; coefficients of one sign exclude roots.
	template <size_t Degree>
	struct UnitPolynomialRoots
	{
		std::array<double, Degree> values{};
		size_t count = 0;
	};

	template <size_t Count>
	[[nodiscard]]
	double EvaluateBernstein(const std::array<double, Count>& values, const double t) noexcept
	{
		if constexpr (Count == 1)
		{
			return values[0];
		}
		else
		{
			return [&]<size_t... I>(std::index_sequence<I...>) noexcept
			{
				return EvaluateBernstein(std::array{ ((1.0 - t) * values[I] + t * values[I + 1])... }, t);
			}(std::make_index_sequence<Count - 1>{});
		}
	}

	template <size_t Count>
	[[nodiscard]]
	UnitPolynomialRoots<Count - 1> BernsteinRoots(const std::array<double, Count>& values) noexcept
	{
		constexpr size_t Degree = (Count - 1);
		constexpr double ParameterTolerance = 2.0e-15;
		UnitPolynomialRoots<Degree> roots;
		size_t variations = 0;
		int32 previousSign = 0;
		for (const double value : values)
		{
			const int32 sign = ((0.0 < value) - (value < 0.0));
			if (sign != 0)
			{
				variations += ((previousSign != 0) && (previousSign != sign));
				previousSign = sign;
			}
		}
		if (variations == 0)
		{
			return roots;
		}
		if constexpr (Degree == 1)
		{
			roots.values[roots.count++] = (values[0] / (values[0] - values[1]));
		}
		else
		{
			// Endpoint roots do not affect interior candidates. Remove their
			// t or (1 - t) factor before solving repeated stationary endpoints.
			if ((values.front() == 0.0) || (values.back() == 0.0))
			{
				std::array<double, Degree> reduced;
				for (size_t i = 0; i < Degree; ++i)
				{
					reduced[i] = ((values.front() == 0.0)
						? (values[i + 1] * Degree / (i + 1)) : (values[i] * Degree / (Degree - i)));
				}
				const auto interior = BernsteinRoots(reduced);
				roots.count = interior.count;
				for (size_t i = 0; i < roots.count; ++i)
				{
					roots.values[i] = interior.values[i];
				}
				return roots;
			}
			std::array<double, Degree> derivative;
			for (size_t i = 0; i < Degree; ++i)
			{
				derivative[i] = (Degree * (values[i + 1] - values[i]));
			}
			UnitPolynomialRoots<Degree - 1> critical;
			// One coefficient sign change gives a single interior root, so no
			// derivative roots are needed to bracket it.
			if (1 < variations)
			{
				critical = BernsteinRoots(derivative);
			}
			double lower = 0.0, lowerValue = values.front();
			for (size_t i = 0; i <= critical.count; ++i)
			{
				const double upper = ((i < critical.count) ? critical.values[i] : 1.0);
				const double upperValue = EvaluateBernstein(values, upper);
				if ((upperValue == 0.0) && (upper < 1.0))
				{
					roots.values[roots.count++] = upper;
				}
				else if (((lowerValue < 0.0) && (0.0 < upperValue)) || ((upperValue < 0.0) && (0.0 < lowerValue)))
				{
					double lo = lower, hi = upper, flo = lowerValue, fhi = upperValue;
					double t = ((lo + hi) * 0.5), previousStep = (hi - lo);
					int32 iteration = 0;
					for (; iteration < 32; ++iteration)
					{
						const double f = EvaluateBernstein(values, t);
						if (f == 0.0)
						{
							break;
						}
						if ((f < 0.0) == (flo < 0.0))
						{
							lo = t;
							flo = f;
						}
						else
						{
							hi = t;
							fhi = f;
						}
						const double next = (t - f / EvaluateBernstein(derivative, t));
						if ((Abs(next - t) <= ParameterTolerance) && (lower <= next) && (next <= upper))
						{
							t = next;
							break;
						}
						const double candidate = (((lo < next) && (next < hi) && (Abs(next - t) < (previousStep * 0.5)))
							? next : ((lo + hi) * 0.5));
						previousStep = Abs(candidate - t);
						t = candidate;
						if (previousStep <= ParameterTolerance)
						{
							break;
						}
					}
					// Keep the better evaluated endpoint if a fallback step exhausts the budget.
					if (iteration == 32)
					{
						t = ((Abs(flo) < Abs(fhi)) ? lo : hi);
					}
					roots.values[roots.count++] = t;
				}
				lower = upper;
				lowerValue = upperValue;
			}
		}
		return roots;
	}

	// Stationary parameters of squared distance to a quadratic or cubic Bezier.
	// The product (B(t) - point).B'(t) has degree 3 or 5; the common positive
	// derivative factor is omitted. Endpoints are considered by the caller.
	template <size_t Count>
	[[nodiscard]]
	auto BezierPointStationaryParameters(const std::array<Vec2, Count>& controls, const Vec2& point) noexcept
	{
		static_assert((Count == 3) || (Count == 4));
		constexpr size_t Degree = (Count - 1);
		constexpr auto weights = []
		{
			if constexpr (Degree == 2)
			{
				return std::array{ std::array{ 1.0, (1.0 / 3) }, std::array{ (2.0 / 3), (2.0 / 3) }, std::array{ (1.0 / 3), 1.0 } };
			}
			else
			{
				return std::array{ std::array{ 1.0, 0.4, 0.1 }, std::array{ 0.6, 0.6, 0.3 },
					std::array{ 0.3, 0.6, 0.6 }, std::array{ 0.1, 0.4, 1.0 } };
			}
		}();
		std::array<double, 2 * Degree> coefficients{};
		for (size_t i = 0; i <= Degree; ++i)
		{
			for (size_t j = 0; j < Degree; ++j)
			{
				coefficients[i + j] += (weights[i][j] * (controls[i] - point).dot(controls[j + 1] - controls[j]));
			}
		}
		return BernsteinRoots(coefficients);
	}

	template <class Bezier>
	[[nodiscard]]
	auto BezierControlPoints(const Bezier& curve) noexcept
	{
		if constexpr (std::is_same_v<Bezier, Bezier2>)
		{
			return std::array{ curve.p0, curve.p1, curve.p2 };
		}
		else
		{
			return std::array{ curve.p0, curve.p1, curve.p2, curve.p3 };
		}
	}

	// Control hulls give bounds without solving coordinate extrema.
	template <class Bezier>
	[[nodiscard]]
	std::pair<Vec2, Vec2> BezierControlBounds(const Bezier& curve) noexcept
	{
		Vec2 lower = curve.p0, upper = curve.p0;
		for (const Vec2& p : BezierControlPoints(curve))
		{
			lower = { Min(lower.x, p.x), Min(lower.y, p.y) };
			upper = { Max(upper.x, p.x), Max(upper.y, p.y) };
		}
		return { lower, upper };
	}

	enum class BezierPairIntersectionKind { Separated, Contact, Unresolved };

	struct BezierPairIntersection
	{
		BezierPairIntersectionKind kind = BezierPairIntersectionKind::Separated;
		double parameterA = 0.5, parameterB = 0.5;
	};

	template <class Bezier>
	[[nodiscard]]
	Bezier BezierParameterRange(const Bezier& curve, const double lower, const double upper) noexcept
	{
		if (lower == 0.0)
		{
			return curve.split(upper).first;
		}
		if (lower == 1.0)
		{
			return curve.split(1.0).second;
		}
		return curve.split(lower).second.split((upper - lower) / (1.0 - lower)).first;
	}

	// Clip the target's control graph against a strip containing the reference
	// curve. All graph edges suffice to bound its convex hull; no allocation.
	template <class Reference, class Target>
	[[nodiscard]]
	bool ClipBezierPair(const Reference& reference, Target& target, double& lower, double& upper) noexcept
	{
		const auto a = BezierControlPoints(reference);
		const auto b = BezierControlPoints(target);
		const Vec2 direction = (a.back() - a.front());
		if (direction == Vec2{ 0, 0 })
		{
			return true;
		}
		double stripLower = 0.0, stripUpper = 0.0;
		for (const Vec2& p : a)
		{
			const double value = direction.cross(p - a.front());
			stripLower = Min(stripLower, value);
			stripUpper = Max(stripUpper, value);
		}
		// Endpoint distances vanish. Interior Bernstein weights sum to at most
		// 1/2 for a quadratic and 3/4 for a cubic.
		constexpr double Weight = (std::is_same_v<Reference, Bezier2> ? 0.5 : 0.75);
		const double padding = (BezierRootTolerance * (Abs(direction.x) + Abs(direction.y)));
		stripLower = (stripLower * Weight - padding);
		stripUpper = (stripUpper * Weight + padding);
		constexpr size_t Degree = (std::tuple_size_v<decltype(b)> - 1);
		std::array<double, Degree + 1> values;
		double lo = 1.0, hi = 0.0;
		for (size_t i = 0; i <= Degree; ++i)
		{
			values[i] = direction.cross(b[i] - a.front());
			if (InRange(values[i], stripLower, stripUpper))
			{
				lo = Min(lo, (static_cast<double>(i) / Degree));
				hi = Max(hi, (static_cast<double>(i) / Degree));
			}
		}
		for (size_t i = 0; i < Degree; ++i)
		{
			for (size_t j = (i + 1); j <= Degree; ++j)
			{
				for (const double level : { stripLower, stripUpper })
				{
					if (((values[i] < level) && (level < values[j])) || ((values[j] < level) && (level < values[i])))
					{
						const double t = ((i + (j - i) * (level - values[i]) / (values[j] - values[i])) / Degree);
						lo = Min(lo, t);
						hi = Max(hi, t);
					}
				}
			}
		}
		if (hi < lo)
		{
			return false;
		}
		if ((hi - lo) < 0.9)
		{
			const double start = lower, width = (upper - lower);
			lower = (start + width * lo);
			upper = (start + width * hi);
			target = BezierParameterRange(target, lo, hi);
		}
		return true;
	}

	template <class Bezier>
	[[nodiscard]]
	std::array<Vec2, 4> BezierCubicControlPoints(const Bezier& curve) noexcept
	{
		if constexpr (std::is_same_v<Bezier, Bezier3>)
		{
			return BezierControlPoints(curve);
		}
		else
		{
			return { curve.p0, curve.p0 + (curve.p1 - curve.p0) * (2.0 / 3),
				curve.p2 + (curve.p1 - curve.p2) * (2.0 / 3), curve.p2 };
		}
	}

	// Nearby translated curves need a coupled-parameter bound: separate hulls
	// can overlap down to tiny subranges even when the curves do not intersect.
	template <class BezierA, class BezierB, class Check>
	[[nodiscard]]
	bool SeparateBezierPairByProjection(const BezierA& a, const BezierB& b, Check&& check, bool& contact)
	{
		const auto ca = BezierCubicControlPoints(a), cb = BezierCubicControlPoints(b);
		Vec2 normal{ 0, 0 };
		double normalSq = 0.0;
		for (size_t i = 0; i < 4; ++i)
		{
			const Vec2 delta = (ca[i] - cb[i]);
			if (normalSq < delta.lengthSq())
			{
				normal = delta;
				normalSq = delta.lengthSq();
			}
		}
		if (normalSq == 0.0)
		{
			contact = check(0.5, 0.5);
			return false;
		}
		const double tolerance = (BezierRootTolerance * std::sqrt(normalSq));
		std::array<double, 4> difference;
		std::array<double, 3> slopes;
		double projectionError = 0.0, speedB = 0.0;
		for (size_t i = 0; i < 4; ++i)
		{
			difference[i] = (ca[i] - cb[i]).dot(normal);
			projectionError = Max(projectionError, Abs((ca[i] - cb[i]).cross(normal)));
			if (i < 3)
			{
				slopes[i] = (3.0 * (ca[i + 1] - ca[i]).cross(normal));
				speedB = Max(speedB, Abs(3.0 * (cb[i + 1] - cb[i]).dot(normal)));
			}
		}
		// The exact quadratic range is much tighter than its control range.
		double minSlope = Min(slopes[0], slopes[2]), maxSlope = Max(slopes[0], slopes[2]);
		const double curvature = (slopes[0] - 2.0 * slopes[1] + slopes[2]);
		if (curvature != 0.0)
		{
			const double t = ((slopes[0] - slopes[1]) / curvature);
			if ((0.0 < t) && (t < 1.0))
			{
				const double slope = EvaluateBernstein(slopes, t);
				minSlope = Min(minSlope, slope);
				maxSlope = Max(maxSlope, slope);
			}
		}
		double lower = Min(difference[0], difference[3]), upper = Max(difference[0], difference[3]);
		const double cubic = (-difference[0] + 3.0 * difference[1] - 3.0 * difference[2] + difference[3]);
		const double quadratic = (difference[0] - 2.0 * difference[1] + difference[2]);
		const double linear = (difference[1] - difference[0]);
		contact = CheckQuadraticRootsInUnitInterval(cubic, (2.0 * quadratic), linear, [&](const double t)
		{
			const double value = EvaluateBernstein(difference, t);
			lower = Min(lower, value);
			upper = Max(upper, value);
			return ((Abs(value) <= (2.0 * tolerance)) && check(t, t));
		});
		if (not contact)
		{
			contact = CheckCubicRootsInUnitInterval(cubic, (3.0 * quadratic), (3.0 * linear), difference[0],
				[&](const double t) { return check(t, t); });
		}
		if (contact)
		{
			return false;
		}
		const double slope = (Max(minSlope, -maxSlope) - 8.0 * tolerance);
		if (slope <= 0.0)
		{
			// Projection extrema also seed contacts with parallel tangents.
			contact = CheckQuadraticRootsInUnitInterval(curvature, (2.0 * (slopes[1] - slopes[0])), slopes[0],
				[&](const double t)
				{
					const Vec2 delta = (a.pointAt(t) - b.pointAt(t)), velocity = b.derivativeAt(t);
					const double speedSq = velocity.lengthSq();
					const double s = ((0.0 < speedSq) ? (t + delta.dot(velocity) / speedSq) : t);
					return (InRange(s, 0.0, 1.0) && check(t, s));
				});
			return false;
		}
		// With g(t) = cross(A(t), normal) monotone, A(t) = B(s) requires
		// |t-s| <= projectionError / min|g'|, hence |dot(A(t)-B(t), normal)|
		// <= speedB * projectionError / min|g'|. Pad both bounds for roundoff.
		const double margin = ((projectionError + 4.0 * tolerance) * speedB / slope + 4.0 * tolerance);
		return ((margin < lower) || (upper < -margin));
	}

	// Called after point/segment reduction. Coordinates are finite and scale
	// is nonzero under that precondition. Work and storage are both bounded.
	template <class BezierA, class BezierB>
	[[nodiscard]]
	BezierPairIntersection ClassifyCanonicalBezierPair(const BezierA& originalA, const BezierB& originalB)
	{
		constexpr int32 MaxNodes = 64, MaxCorrections = 64;
		const Vec2 origin = originalA.p0;
		double scale = 0.0;
		const auto IncludeScale = [&](const auto& curve)
		{
			for (const Vec2& p : BezierControlPoints(curve))
			{
				scale = Max({ scale, Abs(p.x - origin.x), Abs(p.y - origin.y) });
			}
		};
		IncludeScale(originalA);
		IncludeScale(originalB);
		const auto Normalize = [&](auto curve)
		{
			curve.p0 = ((curve.p0 - origin) / scale);
			curve.p1 = ((curve.p1 - origin) / scale);
			curve.p2 = ((curve.p2 - origin) / scale);
			if constexpr (std::is_same_v<decltype(curve), Bezier3>)
			{
				curve.p3 = ((curve.p3 - origin) / scale);
			}
			return curve;
		};
		const BezierA a = Normalize(originalA);
		const BezierB b = Normalize(originalB);
		const auto SeparatedBounds = [](const Vec2& la, const Vec2& ha, const Vec2& lb, const Vec2& hb)
		{
			return (((ha.x + BezierRootTolerance) < lb.x) || ((hb.x + BezierRootTolerance) < la.x)
				|| ((ha.y + BezierRootTolerance) < lb.y) || ((hb.y + BezierRootTolerance) < la.y));
		};
		const auto [minA, maxA] = BezierControlBounds(a);
		const auto [minB, maxB] = BezierControlBounds(b);
		BezierPairIntersection result;
		if (SeparatedBounds(minA, maxA, minB, maxB))
		{
			return result;
		}
		int32 corrections = 0;
		double bestDistanceSq = std::numeric_limits<double>::infinity();
		const auto CheckContact = [&](double t, double s)
		{
			for (int32 iteration = 0; iteration < 8; ++iteration)
			{
				const Vec2 delta = (a.pointAt(t) - b.pointAt(s));
				const double distanceSq = delta.lengthSq();
				if (distanceSq < bestDistanceSq)
				{
					bestDistanceSq = distanceSq;
					result.parameterA = t;
					result.parameterB = s;
				}
				if (distanceSq <= (BezierRootTolerance * BezierRootTolerance))
				{
					return true;
				}
				if (corrections == MaxCorrections)
				{
					break;
				}
				++corrections;
				const Vec2 u = a.derivativeAt(t), v = b.derivativeAt(s);
				const double determinant = u.cross(v);
				if (determinant == 0.0)
				{
					break;
				}
				const double nextT = (t - delta.cross(v) / determinant), nextS = (s - delta.cross(u) / determinant);
				if (not InRange(nextT, 0.0, 1.0) || not InRange(nextS, 0.0, 1.0))
				{
					break;
				}
				t = nextT;
				s = nextS;
			}
			return false;
		};
		bool contact = false;
		if (SeparateBezierPairByProjection(a, b, CheckContact, contact))
		{
			return result;
		}
		if (contact)
		{
			result.kind = BezierPairIntersectionKind::Contact;
			return result;
		}
		struct Node
		{
			BezierA a;
			BezierB b;
			double lowerA, upperA, lowerB, upperB;
		};
		// Each visit adds at most one pending node.
		std::array<Node, MaxNodes + 1> stack;
		int32 count = 1;
		stack[0] = { a, b, 0.0, 1.0, 0.0, 1.0 };
		for (int32 visit = 0; count && (visit < MaxNodes); ++visit)
		{
			Node node = stack[--count];
			const auto [la, ha] = BezierControlBounds(node.a);
			const auto [lb, hb] = BezierControlBounds(node.b);
			if (SeparatedBounds(la, ha, lb, hb))
			{
				continue;
			}
			if (CheckContact((node.lowerA + node.upperA) * 0.5, (node.lowerB + node.upperB) * 0.5))
			{
				result.kind = BezierPairIntersectionKind::Contact;
				return result;
			}
			const double widthA = (node.upperA - node.lowerA), widthB = (node.upperB - node.lowerB);
			if (not ClipBezierPair(node.a, node.b, node.lowerB, node.upperB)
				|| not ClipBezierPair(node.b, node.a, node.lowerA, node.upperA))
			{
				continue;
			}
			if (((node.upperA - node.lowerA) < (0.8 * widthA)) || ((node.upperB - node.lowerB) < (0.8 * widthB)))
			{
				stack[count++] = node;
			}
			else if ((hb - lb).lengthSq() <= (ha - la).lengthSq())
			{
				const auto [left, right] = node.a.split(0.5);
				const double middle = ((node.lowerA + node.upperA) * 0.5);
				stack[count++] = { right, node.b, middle, node.upperA, node.lowerB, node.upperB };
				stack[count++] = { left, node.b, node.lowerA, middle, node.lowerB, node.upperB };
			}
			else
			{
				const auto [left, right] = node.b.split(0.5);
				const double middle = ((node.lowerB + node.upperB) * 0.5);
				stack[count++] = { node.a, right, node.lowerA, node.upperA, middle, node.upperB };
				stack[count++] = { node.a, left, node.lowerA, node.upperA, node.lowerB, middle };
			}
		}
		result.kind = (count ? BezierPairIntersectionKind::Unresolved : BezierPairIntersectionKind::Separated);
		return result;
	}

	template <class Bezier>
	[[nodiscard]]
	bool BezierLexicographicalLess(const Bezier& a, const Bezier& b) noexcept
	{
		const auto ca = BezierControlPoints(a), cb = BezierControlPoints(b);
		for (size_t i = 0; i < ca.size(); ++i)
		{
			if (ca[i].x != cb[i].x)
			{
				return (ca[i].x < cb[i].x);
			}
			if (ca[i].y != cb[i].y)
			{
				return (ca[i].y < cb[i].y);
			}
		}
		return false;
	}

	// Canonical orientation/order makes the bounded search symmetric. Map its
	// best evaluated parameters back for callers that also need actual points.
	template <class BezierA, class BezierB>
	[[nodiscard]]
	BezierPairIntersection ClassifyBezierPair(const BezierA& originalA, const BezierB& originalB)
	{
		const bool reverseA = BezierLexicographicalLess(originalA.reversed(), originalA);
		const bool reverseB = BezierLexicographicalLess(originalB.reversed(), originalB);
		const BezierA a = (reverseA ? originalA.reversed() : originalA);
		const BezierB b = (reverseB ? originalB.reversed() : originalB);
		const bool swap = [&]
		{
			if constexpr (std::is_same_v<BezierA, BezierB>)
			{
				return BezierLexicographicalLess(b, a);
			}
			else
			{
				return std::is_same_v<BezierA, Bezier3>;
			}
		}();
		BezierPairIntersection result = (swap ? ClassifyCanonicalBezierPair(b, a) : ClassifyCanonicalBezierPair(a, b));
		if (swap)
		{
			std::swap(result.parameterA, result.parameterB);
		}
		if (reverseA)
		{
			result.parameterA = (1.0 - result.parameterA);
		}
		if (reverseB)
		{
			result.parameterB = (1.0 - result.parameterB);
		}
		return result;
	}

	// The image of a collinear Bezier is a segment, including any retracing.
	template <class Bezier>
	[[nodiscard]]
	Line BezierSegmentExtent(const Bezier& curve, const bool useX) noexcept
	{
		const auto controls = BezierControlPoints(curve);
		constexpr size_t Degree = (std::tuple_size_v<decltype(controls)> - 1);
		const auto Coordinate = [useX](const Vec2& p) { return (useX ? p.x : p.y); };
		Line extent{ controls.front(), controls.back() };
		if (Coordinate(extent.end) < Coordinate(extent.start))
		{
			std::swap(extent.start, extent.end);
		}
		std::array<double, Degree> derivative;
		for (size_t i = 0; i < Degree; ++i)
		{
			derivative[i] = Coordinate(controls[i + 1] - controls[i]);
		}
		const auto Include = [&](const double t)
		{
			const Vec2 point = curve.pointAt(t);
			if (Coordinate(point) < Coordinate(extent.start))
			{
				extent.start = point;
			}
			if (Coordinate(extent.end) < Coordinate(point))
			{
				extent.end = point;
			}
			return false;
		};
		if constexpr (Degree == 2)
		{
			const auto roots = BernsteinRoots(derivative);
			if (roots.count != 0)
			{
				Include(roots.values[0]);
			}
		}
		else
		{
			(void)CheckQuadraticRootsInUnitInterval(
				(derivative[0] - 2.0 * derivative[1] + derivative[2]),
				(2.0 * (derivative[1] - derivative[0])), derivative[0], Include);
		}
		return extent;
	}

	template <class Bezier>
	[[nodiscard]]
	bool TryGetBezierSegment(const Bezier& curve, Line& segment) noexcept
	{
		const auto controls = BezierControlPoints(curve);
		Vec2 direction = (controls.back() - controls.front());
		if (direction == Vec2{ 0, 0 })
		{
			for (const Vec2& point : controls)
			{
				direction = (point - controls.front());
				if (direction != Vec2{ 0, 0 })
				{
					break;
				}
			}
		}
		for (const Vec2& point : controls)
		{
			// Do not flatten thin curves using an absolute distance tolerance.
			if (direction.cross(point - controls.front()) != 0.0)
			{
				return false;
			}
		}
		segment = BezierSegmentExtent(curve, (Abs(direction.y) <= Abs(direction.x)));
		return true;
	}

	template <class T>
	inline constexpr bool IsBezier = (std::is_same_v<T, Bezier2> || std::is_same_v<T, Bezier3>);

	template <class Bezier, class Callback>
	decltype(auto) WithSimpleBezier(const Bezier& curve, const Callback& callback)
	{
		Line segment;
		if (TryGetBezierSegment(curve, segment))
		{
			if (segment.start == segment.end)
			{
				return callback(segment.start);
			}
			return callback(segment);
		}
		return callback(curve);
	}

	// Normalize the pairs whose general kernels approximate Bezier curves.
	// Predicates, intersection points, and distances must use the same images.
	template <class A, class B, class Callback>
	decltype(auto) WithSimpleBezierPair(const A& a, const B& b, const Callback& callback)
	{
		if constexpr (IsBezier<A> && IsBezier<B>)
		{
			return WithSimpleBezier(a, [&](const auto& simpleA)
			{
				return WithSimpleBezier(b, [&](const auto& simpleB) { return callback(simpleA, simpleB); });
			});
		}
		else if constexpr (IsBezier<A> && std::is_same_v<B, SuperEllipse>)
		{
			return WithSimpleBezier(a, [&](const auto& simpleA)
			{
				if ((b.n == 1.0) && (0.0 < b.axes.x) && (0.0 < b.axes.y))
				{
					return callback(simpleA, Quad{ b.top(), b.right(), b.bottom(), b.left() });
				}
				return callback(simpleA, b);
			});
		}
		else if constexpr (std::is_same_v<A, SuperEllipse> && IsBezier<B>)
		{
			return WithSimpleBezierPair(b, a, [&](const auto& simpleB, const auto& simpleA)
			{
				return callback(simpleA, simpleB);
			});
		}
		else
		{
			return callback(a, b);
		}
	}

	struct BezierClosestPoint
	{
		Vec2 point;
		double parameter;
		double distanceSq;
	};

	template <class Bezier>
	[[nodiscard]]
	BezierClosestPoint ClosestPointOnBezier(const Bezier& curve, const Vec2& target) noexcept
	{
		const auto controls = BezierControlPoints(curve);
		if constexpr (std::is_same_v<Bezier, Bezier2>)
		{
			if ((curve.p1 - curve.p0) == (curve.p2 - curve.p1))
			{
				const Vec2 direction = (curve.p2 - curve.p0);
				const double lengthSq = direction.lengthSq();
				const double t = ((lengthSq == 0.0) ? 0.0 : Clamp((target - curve.p0).dot(direction) / lengthSq, 0.0, 1.0));
				const Vec2 point = curve.pointAt(t);
				return { point, t, target.distanceFromSq(point) };
			}
		}
		BezierClosestPoint best{ controls.front(), 0.0, target.distanceFromSq(controls.front()) };
		const double endDistanceSq = target.distanceFromSq(controls.back());
		if (endDistanceSq < best.distanceSq)
		{
			best = { controls.back(), 1.0, endDistanceSq };
		}
		const auto roots = BezierPointStationaryParameters(controls, target);
		for (size_t i = 0; i < roots.count; ++i)
		{
			const double t = roots.values[i];
			const Vec2 point = curve.pointAt(t);
			const double distanceSq = target.distanceFromSq(point);
			if (distanceSq < best.distanceSq)
			{
				best = { point, t, distanceSq };
			}
		}
		return best;
	}
}
