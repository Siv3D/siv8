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

# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/PolynomialSolver.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>

namespace s3d
{
	namespace
	{
		inline constexpr double DoubleEpsilon = 2.2204460492503131e-16;
		inline constexpr double BezierRootTolerance = (64.0 * DoubleEpsilon);
		inline constexpr double BezierPointTolerance = (64.0 * DoubleEpsilon);
		inline constexpr double EllipseDistanceRootTolerance = (16.0 * DoubleEpsilon);

		[[nodiscard]]
		constexpr bool NearlyEqualBezierCoordinate(const double a, const double b) noexcept
		{
			const double scale = Max({ Abs(a), Abs(b), 1.0 });
			return (Abs(a - b) <= (BezierPointTolerance * scale));
		}

		[[nodiscard]]
		constexpr bool NearlyEqualBezierPoint(const Vec2& a, const Vec2& b) noexcept
		{
			return (NearlyEqualBezierCoordinate(a.x, b.x)
				&& NearlyEqualBezierCoordinate(a.y, b.y));
		}

		[[nodiscard]]
		constexpr bool NearlyBetweenBezierCoordinate(const double a, const double x, const double b) noexcept
		{
			const double min = Min(a, b);
			const double max = Max(a, b);
			const double scale = Max(Max(Abs(a), Abs(x)), Max(Abs(b), 1.0));
			const double tolerance = (BezierPointTolerance * scale);

			return ((min - tolerance) <= x)
				&& (x <= (max + tolerance));
		}

		[[nodiscard]]
		constexpr bool BezierRootPointIsOnSegmentRange(const Vec2& p, const Line& segment) noexcept
		{
			return NearlyBetweenBezierCoordinate(segment.start.x, p.x, segment.end.x)
				&& NearlyBetweenBezierCoordinate(segment.start.y, p.y, segment.end.y);
		}

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

		[[nodiscard]]
		constexpr bool ControlPointsBoundingRectContains(const Vec2& p, const Bezier2& curve) noexcept
		{
			const double minX = Min({ curve.p0.x, curve.p1.x, curve.p2.x });
			const double maxX = Max({ curve.p0.x, curve.p1.x, curve.p2.x });
			const double minY = Min({ curve.p0.y, curve.p1.y, curve.p2.y });
			const double maxY = Max({ curve.p0.y, curve.p1.y, curve.p2.y });

			return ((minX <= p.x) && (p.x <= maxX)
				&& (minY <= p.y) && (p.y <= maxY));
		}

		[[nodiscard]]
		constexpr bool ControlPointsBoundingRectContains(const Vec2& p, const Bezier3& curve) noexcept
		{
			const double minX = Min({ curve.p0.x, curve.p1.x, curve.p2.x, curve.p3.x });
			const double maxX = Max({ curve.p0.x, curve.p1.x, curve.p2.x, curve.p3.x });
			const double minY = Min({ curve.p0.y, curve.p1.y, curve.p2.y, curve.p3.y });
			const double maxY = Max({ curve.p0.y, curve.p1.y, curve.p2.y, curve.p3.y });

			return ((minX <= p.x) && (p.x <= maxX)
				&& (minY <= p.y) && (p.y <= maxY));
		}

		[[nodiscard]]
		constexpr double AxisVariation(const double p0, const double p1, const double p2) noexcept
		{
			return Max({ Abs(p1 - p0), Abs(p2 - p1), Abs(p2 - p0) });
		}

		[[nodiscard]]
		constexpr double AxisVariation(const double p0, const double p1, const double p2, const double p3) noexcept
		{
			return Max({ Abs(p1 - p0), Abs(p2 - p1), Abs(p3 - p2), Abs(p3 - p0) });
		}

		[[nodiscard]]
		constexpr int32 CompareBezierPointLexicographically(const Vec2& a, const Vec2& b) noexcept
		{
			if (a.x < b.x)
			{
				return -1;
			}

			if (b.x < a.x)
			{
				return 1;
			}

			if (a.y < b.y)
			{
				return -1;
			}

			if (b.y < a.y)
			{
				return 1;
			}

			return 0;
		}

		[[nodiscard]]
		constexpr bool BezierLexicographicalLess(const Bezier2& a, const Bezier2& b) noexcept
		{
			if (const int32 order = CompareBezierPointLexicographically(a.p0, b.p0))
			{
				return (order < 0);
			}

			if (const int32 order = CompareBezierPointLexicographically(a.p1, b.p1))
			{
				return (order < 0);
			}

			return (CompareBezierPointLexicographically(a.p2, b.p2) < 0);
		}

		[[nodiscard]]
		constexpr bool BezierLexicographicalLess(const Bezier3& a, const Bezier3& b) noexcept
		{
			if (const int32 order = CompareBezierPointLexicographically(a.p0, b.p0))
			{
				return (order < 0);
			}

			if (const int32 order = CompareBezierPointLexicographically(a.p1, b.p1))
			{
				return (order < 0);
			}

			if (const int32 order = CompareBezierPointLexicographically(a.p2, b.p2))
			{
				return (order < 0);
			}

			return (CompareBezierPointLexicographically(a.p3, b.p3) < 0);
		}

		[[nodiscard]]
		bool CheckBezier2Axis(const Vec2& p, const Bezier2& curve, const bool useX)
		{
			const double p0 = (useX ? curve.p0.x : curve.p0.y);
			const double p1 = (useX ? curve.p1.x : curve.p1.y);
			const double p2 = (useX ? curve.p2.x : curve.p2.y);
			const double target = (useX ? p.x : p.y);

			const double a = (p0 - 2.0 * p1 + p2);
			const double b = (2.0 * (p1 - p0));
			const double c = (p0 - target);

			return CheckQuadraticRootsInUnitInterval(a, b, c, [&](const double t)
			{
				return NearlyEqualBezierPoint(curve.pointAt(t), p);
			});
		}

		[[nodiscard]]
		bool CheckBezier3Axis(const Vec2& p, const Bezier3& curve, const bool useX)
		{
			const double p0 = (useX ? curve.p0.x : curve.p0.y);
			const double p1 = (useX ? curve.p1.x : curve.p1.y);
			const double p2 = (useX ? curve.p2.x : curve.p2.y);
			const double p3 = (useX ? curve.p3.x : curve.p3.y);
			const double target = (useX ? p.x : p.y);

			const double a = (-p0 + 3.0 * p1 - 3.0 * p2 + p3);
			const double b = (3.0 * p0 - 6.0 * p1 + 3.0 * p2);
			const double c = (-3.0 * p0 + 3.0 * p1);
			const double d = (p0 - target);

			return CheckCubicRootsInUnitInterval(a, b, c, d, [&](const double t)
			{
				return NearlyEqualBezierPoint(curve.pointAt(t), p);
			});
		}

		[[nodiscard]]
		bool IntersectsPointBezier2(const Vec2& p, const Bezier2& curve)
		{
			if (not ControlPointsBoundingRectContains(p, curve))
			{
				return false;
			}

			if ((p == curve.p0) || (p == curve.p2))
			{
				return true;
			}

			const double xVariation = AxisVariation(curve.p0.x, curve.p1.x, curve.p2.x);
			const double yVariation = AxisVariation(curve.p0.y, curve.p1.y, curve.p2.y);

			if ((xVariation == 0.0) && (yVariation == 0.0))
			{
				return false;
			}

			if (yVariation < xVariation)
			{
				if ((0.0 < xVariation) && CheckBezier2Axis(p, curve, true))
				{
					return true;
				}

				return ((0.0 < yVariation) && CheckBezier2Axis(p, curve, false));
			}
			else
			{
				if ((0.0 < yVariation) && CheckBezier2Axis(p, curve, false))
				{
					return true;
				}

				return ((0.0 < xVariation) && CheckBezier2Axis(p, curve, true));
			}
		}

		[[nodiscard]]
		bool IntersectsPointBezier3(const Vec2& p, const Bezier3& curve)
		{
			if (not ControlPointsBoundingRectContains(p, curve))
			{
				return false;
			}

			if ((p == curve.p0) || (p == curve.p3))
			{
				return true;
			}

			const double xVariation = AxisVariation(curve.p0.x, curve.p1.x, curve.p2.x, curve.p3.x);
			const double yVariation = AxisVariation(curve.p0.y, curve.p1.y, curve.p2.y, curve.p3.y);

			if ((xVariation == 0.0) && (yVariation == 0.0))
			{
				return false;
			}

			if (yVariation < xVariation)
			{
				if ((0.0 < xVariation) && CheckBezier3Axis(p, curve, true))
				{
					return true;
				}

				return ((0.0 < yVariation) && CheckBezier3Axis(p, curve, false));
			}
			else
			{
				if ((0.0 < yVariation) && CheckBezier3Axis(p, curve, false))
				{
					return true;
				}

				return ((0.0 < xVariation) && CheckBezier3Axis(p, curve, true));
			}
		}

		[[nodiscard]]
		constexpr double Bezier2AxisValue(const double p0, const double p1, const double p2, const double t) noexcept
		{
			const double s = (1.0 - t);
			return ((s * s * p0) + (2.0 * s * t * p1) + (t * t * p2));
		}

		[[nodiscard]]
		constexpr double Bezier3AxisValue(const double p0, const double p1, const double p2, const double p3, const double t) noexcept
		{
			const double s = (1.0 - t);
			return ((s * s * s * p0) + (3.0 * s * s * t * p1) + (3.0 * s * t * t * p2) + (t * t * t * p3));
		}

		[[nodiscard]]
		bool CollinearBezier2IntersectsLine(const Line& segment, const Bezier2& curve) noexcept
		{
			const Vec2 d = (segment.end - segment.start);
			const bool useX = (Abs(d.y) <= Abs(d.x));

			const double s0 = (useX ? segment.start.x : segment.start.y);
			const double s1 = (useX ? segment.end.x : segment.end.y);
			const double segmentMin = Min(s0, s1);
			const double segmentMax = Max(s0, s1);

			const double p0 = (useX ? curve.p0.x : curve.p0.y);
			const double p1 = (useX ? curve.p1.x : curve.p1.y);
			const double p2 = (useX ? curve.p2.x : curve.p2.y);

			double curveMin = Min(p0, p2);
			double curveMax = Max(p0, p2);

			const double denominator = (p0 - 2.0 * p1 + p2);

			if (denominator != 0.0)
			{
				const double t = ((p0 - p1) / denominator);

				if (InRange(t, 0.0, 1.0))
				{
					const double v = Bezier2AxisValue(p0, p1, p2, t);
					curveMin = Min(curveMin, v);
					curveMax = Max(curveMax, v);
				}
			}

			return ((curveMin <= segmentMax) && (segmentMin <= curveMax));
		}

		[[nodiscard]]
		bool CollinearBezier3IntersectsLine(const Line& segment, const Bezier3& curve) noexcept
		{
			const Vec2 d = (segment.end - segment.start);
			const bool useX = (Abs(d.y) <= Abs(d.x));

			const double s0 = (useX ? segment.start.x : segment.start.y);
			const double s1 = (useX ? segment.end.x : segment.end.y);
			const double segmentMin = Min(s0, s1);
			const double segmentMax = Max(s0, s1);

			const double p0 = (useX ? curve.p0.x : curve.p0.y);
			const double p1 = (useX ? curve.p1.x : curve.p1.y);
			const double p2 = (useX ? curve.p2.x : curve.p2.y);
			const double p3 = (useX ? curve.p3.x : curve.p3.y);

			double curveMin = Min(p0, p3);
			double curveMax = Max(p0, p3);

			const double a = (-p0 + 3.0 * p1 - 3.0 * p2 + p3);
			const double b = (3.0 * p0 - 6.0 * p1 + 3.0 * p2);
			const double c = (-3.0 * p0 + 3.0 * p1);

			std::ignore = CheckQuadraticRootsInUnitInterval((3.0 * a), (2.0 * b), c, [&](const double t)
			{
				const double v = Bezier3AxisValue(p0, p1, p2, p3, t);
				curveMin = Min(curveMin, v);
				curveMax = Max(curveMax, v);
				return false;
			});

			return ((curveMin <= segmentMax) && (segmentMin <= curveMax));
		}

		[[nodiscard]]
		bool IntersectsLineBezier2(const Line& segment, const Bezier2& curve)
		{
			if (segment.start == segment.end)
			{
				return Geometry2D::Intersects(segment.start, curve);
			}

			const Vec2 d = (segment.end - segment.start);
			const double c0 = d.cross(curve.p0 - segment.start);
			const double c1 = d.cross(curve.p1 - segment.start);
			const double c2 = d.cross(curve.p2 - segment.start);

			const double a = (c0 - 2.0 * c1 + c2);
			const double b = (2.0 * (c1 - c0));
			const double c = c0;

			if ((a == 0.0) && (b == 0.0) && (c == 0.0))
			{
				return CollinearBezier2IntersectsLine(segment, curve);
			}

			return CheckQuadraticRootsInUnitInterval(a, b, c, [&](const double t)
			{
				return BezierRootPointIsOnSegmentRange(curve.pointAt(t), segment);
			});
		}

		[[nodiscard]]
		bool IntersectsLineBezier3(const Line& segment, const Bezier3& curve)
		{
			if (segment.start == segment.end)
			{
				return Geometry2D::Intersects(segment.start, curve);
			}

			const Vec2 d = (segment.end - segment.start);
			const double c0 = d.cross(curve.p0 - segment.start);
			const double c1 = d.cross(curve.p1 - segment.start);
			const double c2 = d.cross(curve.p2 - segment.start);
			const double c3 = d.cross(curve.p3 - segment.start);

			const double a = (-c0 + 3.0 * c1 - 3.0 * c2 + c3);
			const double b = (3.0 * c0 - 6.0 * c1 + 3.0 * c2);
			const double c = (-3.0 * c0 + 3.0 * c1);
			const double e = c0;

			if ((a == 0.0) && (b == 0.0) && (c == 0.0) && (e == 0.0))
			{
				return CollinearBezier3IntersectsLine(segment, curve);
			}

			return CheckCubicRootsInUnitInterval(a, b, c, e, [&](const double t)
			{
				return BezierRootPointIsOnSegmentRange(curve.pointAt(t), segment);
			});
		}

		[[nodiscard]]
		double SuperEllipseLocalValue(const Vec2& p, const double n) noexcept
		{
			return (std::pow(Abs(p.x), n) + std::pow(Abs(p.y), n));
		}

		[[nodiscard]]
		double SuperEllipseLocalDerivative(const Vec2& p, const Vec2& d, const double n) noexcept
		{
			auto Term = [n](const double x, const double dx) noexcept
			{
				if (x < 0.0)
				{
					return (-dx * std::pow(-x, (n - 1.0)));
				}

				if (0.0 < x)
				{
					return (dx * std::pow(x, (n - 1.0)));
				}

				return 0.0;
			};

			return (n * (Term(p.x, d.x) + Term(p.y, d.y)));
		}

		[[nodiscard]]
		bool IntersectsLineSuperEllipse(const Line& segment, const SuperEllipse& superEllipse) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(segment, Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(segment, Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } });
			}

			if (n == 2.0)
			{
				return Geometry2D::Intersects(segment, Ellipse{ superEllipse.center, ax, by });
			}

			const Vec2 p0{ ((segment.start.x - superEllipse.center.x) / ax), ((segment.start.y - superEllipse.center.y) / by) };
			const Vec2 p1{ ((segment.end.x - superEllipse.center.x) / ax), ((segment.end.y - superEllipse.center.y) / by) };
			const Vec2 d = (p1 - p0);

			if ((SuperEllipseLocalValue(p0, n) <= 1.0)
				|| (SuperEllipseLocalValue(p1, n) <= 1.0))
			{
				return true;
			}

			if (d == Vec2{ 0, 0 })
			{
				return false;
			}

			std::array<double, 4> ts{ 0.0, 1.0, 0.0, 0.0 };
			size_t tCount = 2;

			if (d.x != 0.0)
			{
				const double tx = (-p0.x / d.x);

				if (InRange(tx, 0.0, 1.0))
				{
					ts[tCount++] = tx;
				}
			}

			if (d.y != 0.0)
			{
				const double ty = (-p0.y / d.y);

				if (InRange(ty, 0.0, 1.0))
				{
					ts[tCount++] = ty;
				}
			}

			std::sort(ts.begin(), (ts.begin() + tCount));

			auto ValueAt = [&](const double t) noexcept
			{
				return SuperEllipseLocalValue((p0 + d * t), n);
			};

			auto DerivativeAt = [&](const double t) noexcept
			{
				return SuperEllipseLocalDerivative((p0 + d * t), d, n);
			};

			for (size_t i = 1; i < tCount; ++i)
			{
				const double t0 = ts[i - 1];
				const double t1 = ts[i];

				if (t0 == t1)
				{
					continue;
				}

				if ((ValueAt(t0) <= 1.0) || (ValueAt(t1) <= 1.0))
				{
					return true;
				}

				if (n <= 1.0)
				{
					continue;
				}

				const double derivative0 = DerivativeAt(t0);
				const double derivative1 = DerivativeAt(t1);

				if ((derivative0 < 0.0) && (0.0 < derivative1))
				{
					double left = t0;
					double right = t1;

					for (int32 k = 0; k < 48; ++k)
					{
						const double mid = ((left + right) * 0.5);

						if (DerivativeAt(mid) < 0.0)
						{
							left = mid;
						}
						else
						{
							right = mid;
						}
					}

					if (ValueAt((left + right) * 0.5) <= 1.0)
					{
						return true;
					}
				}
			}

			return false;
		}

		template <class Shape>
		[[nodiscard]]
		bool IntersectsLineStringShape(const LineString& segments, const Shape& shape)
		{
			const size_t n = segments.size();

			if (n == 0)
			{
				return false;
			}

			const Vec2* ps = segments.data();

			if (n == 1)
			{
				return Geometry2D::Intersects(ps[0], shape);
			}

			for (size_t i = 0; i < (n - 1); ++i)
			{
				if (Geometry2D::Intersects(Line{ ps[i], ps[i + 1] }, shape))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		constexpr bool BoundsIntersectClosed(const RectF& a, const RectF& b) noexcept
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

		[[nodiscard]]
		constexpr double DistanceBetweenClosedIntervals(
			const double aMin, const double aMax,
			const double bMin, const double bMax) noexcept
		{
			if ((aMin <= bMax) && (bMin <= aMax))
			{
				return 0.0;
			}

			if (aMax < bMin)
			{
				return (bMin - aMax);
			}

			// For valid ordered finite intervals, this is the remaining
			// disjoint case bMax < aMin. If a coordinate is NaN, the
			// subtraction propagates NaN and the final <= test rejects it.
			return (aMin - bMax);
		}

		[[nodiscard]]
		double DistancePointClosedRect(const Vec2& p, const RectF& rect) noexcept
		{
			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (left + rect.size.x);
			const double bottom = (top + rect.size.y);
			const double dx = DistanceBetweenClosedIntervals(p.x, p.x, left, right);
			const double dy = DistanceBetweenClosedIntervals(p.y, p.y, top, bottom);

			return std::hypot(dx, dy);
		}

		[[nodiscard]]
		double DistanceClosedRects(const RectF& a, const RectF& b) noexcept
		{
			const double aLeft = a.pos.x;
			const double aTop = a.pos.y;
			const double aRight = (aLeft + a.size.x);
			const double aBottom = (aTop + a.size.y);
			const double bLeft = b.pos.x;
			const double bTop = b.pos.y;
			const double bRight = (bLeft + b.size.x);
			const double bBottom = (bTop + b.size.y);
			const double dx = DistanceBetweenClosedIntervals(aLeft, aRight, bLeft, bRight);
			const double dy = DistanceBetweenClosedIntervals(aTop, aBottom, bTop, bBottom);

			return std::hypot(dx, dy);
		}

		[[nodiscard]]
		constexpr RectF RoundRectCoreRect(const RectF& rect, const double effectiveRadius) noexcept
		{
			return RectF{
				(rect.pos.x + effectiveRadius),
				(rect.pos.y + effectiveRadius),
				(rect.size.x - (effectiveRadius * 2.0)),
				(rect.size.y - (effectiveRadius * 2.0))
			};
		}

		[[nodiscard]]
		bool IntersectsBezier2RectF(const Bezier2& curve, const RectF& rect)
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
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, curve);
			}

			if (h == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, curve);
			}

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), rect))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, rect)
				|| Geometry2D::Intersects(curve.p2, rect))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, curve)
				|| Geometry2D::Intersects(Line{ Vec2{ right, top }, Vec2{ right, bottom } }, curve)
				|| Geometry2D::Intersects(Line{ Vec2{ right, bottom }, Vec2{ left, bottom } }, curve)
				|| Geometry2D::Intersects(Line{ Vec2{ left, bottom }, Vec2{ left, top } }, curve));
		}

		[[nodiscard]]
		bool IntersectsBezier2Circle(const Bezier2& curve, const Circle& circle)
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

			const RectF circleBounds{
				(circle.center.x - circle.r),
				(circle.center.y - circle.r),
				(circle.r * 2.0),
				(circle.r * 2.0)
			};

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), circleBounds))
			{
				return false;
			}

			const double t = curve.computeClosestT(circle.center);
			return (curve.pointAt(t).distanceFromSq(circle.center) <= (circle.r * circle.r));
		}

		[[nodiscard]]
		bool IntersectsBezier2Ellipse(const Bezier2& curve, const Ellipse& ellipse)
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
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } }, curve);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } }, curve);
			}

			const RectF ellipseBounds{
				(ellipse.center.x - ax),
				(ellipse.center.y - by),
				(ax * 2.0),
				(by * 2.0)
			};

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), ellipseBounds))
			{
				return false;
			}

			const Bezier2 local{
				Vec2{ ((curve.p0.x - ellipse.center.x) / ax), ((curve.p0.y - ellipse.center.y) / by) },
				Vec2{ ((curve.p1.x - ellipse.center.x) / ax), ((curve.p1.y - ellipse.center.y) / by) },
				Vec2{ ((curve.p2.x - ellipse.center.x) / ax), ((curve.p2.y - ellipse.center.y) / by) }
			};

			const double t = local.computeClosestT(Vec2{ 0, 0 });
			return (local.pointAt(t).lengthSq() <= 1.0);
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitBezier2ApproximateLineSegments(const Bezier2& curve, Fty&& callback, const double maxError = 0.25, const int32 maxDepth = 8)
		{
			const double maxErrorSq = (maxError * maxError);
			const double flatnessK = (4.0 * maxErrorSq);
			const double kNearlyZeroSq = 1e-12;
			const int32 depthLimit = Max(0, maxDepth);

			auto Visit = [&](auto&& self, const Bezier2& c, const int32 depth) -> bool
				{
					const Vec2 chord = (c.p2 - c.p0);
					const double chordLenSq = chord.lengthSq();

					if (chordLenSq < kNearlyZeroSq)
					{
						const double p0p1LenSq = (c.p1 - c.p0).lengthSq();
						const double p1p2LenSq = (c.p2 - c.p1).lengthSq();
						const double ctrlSpanSq = Max(p0p1LenSq, p1p2LenSq);

						if (ctrlSpanSq < kNearlyZeroSq)
						{
							return callback(Line{ c.p0, c.p2 });
						}

						if (depthLimit <= depth)
						{
							return (callback(Line{ c.p0, c.p1 }) || callback(Line{ c.p1, c.p2 }));
						}
					}
					else
					{
						const Vec2 v = (c.p1 - c.p0);
						const double cross = chord.cross(v);
						bool acceptSegment = ((cross * cross) <= (flatnessK * chordLenSq));

						if (acceptSegment)
						{
							const double dot = v.dot(chord);

							if ((dot < 0.0) || (chordLenSq < dot))
							{
								acceptSegment = false;
							}
						}

						if (acceptSegment || (depthLimit <= depth))
						{
							return callback(Line{ c.p0, c.p2 });
						}
					}

					const auto [left, right] = c.split(0.5);
					return (self(self, left, (depth + 1)) || self(self, right, (depth + 1)));
				};

			return Visit(Visit, curve, 0);
		}

		template <class Shape>
		[[nodiscard]]
		bool IntersectsBezier2ApproximateShape(const Bezier2& curve, const Shape& shape)
		{
			return VisitBezier2ApproximateLineSegments(curve, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, shape);
			});
		}

		[[nodiscard]]
		bool IntersectsBezier2Bezier2Approximate(const Bezier2& a, const Bezier2& b)
		{
			if (not BoundsIntersectClosed(a.computeBoundingRect(), b.computeBoundingRect()))
			{
				return false;
			}

			// The approximation direction must not depend on operand order.
			// Invalid non-finite control points are outside the geometry contract.
			if (BezierLexicographicalLess(b, a))
			{
				return IntersectsBezier2ApproximateShape(b, a);
			}

			return IntersectsBezier2ApproximateShape(a, b);
		}

		[[nodiscard]]
		bool IntersectsBezier2Bezier3Approximate(const Bezier2& a, const Bezier3& b)
		{
			if (not BoundsIntersectClosed(a.computeBoundingRect(), b.computeBoundingRect()))
			{
				return false;
			}

			return IntersectsBezier2ApproximateShape(a, b);
		}

		[[nodiscard]]
		bool IntersectsBezier2SuperEllipse(const Bezier2& curve, const SuperEllipse& superEllipse)
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } }, curve);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } }, curve);
			}

			if (n == 2.0)
			{
				return IntersectsBezier2Ellipse(curve, Ellipse{ superEllipse.center, ax, by });
			}

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), superEllipse.boundingRect()))
			{
				return false;
			}

			return IntersectsBezier2ApproximateShape(curve, superEllipse);
		}

		[[nodiscard]]
		bool IntersectsBezier2Triangle(const Bezier2& curve, const RectF& curveBounds, const Triangle& triangle)
		{
			if (not BoundsIntersectClosed(curveBounds, triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, triangle)
				|| Geometry2D::Intersects(curve.p2, triangle))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, curve)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, curve)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, curve));
		}

		[[nodiscard]]
		bool IntersectsBezier2Triangle(const Bezier2& curve, const Triangle& triangle)
		{
			return IntersectsBezier2Triangle(curve, curve.computeBoundingRect(), triangle);
		}

		[[nodiscard]]
		bool IntersectsBezier2Quad(const Bezier2& curve, const Quad& quad)
		{
			const RectF curveBounds = curve.computeBoundingRect();

			if (not BoundsIntersectClosed(curveBounds, quad.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, quad)
				|| Geometry2D::Intersects(curve.p2, quad))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, curve)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, curve)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, curve)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, curve));
		}

		[[nodiscard]]
		bool IntersectsBezier2RoundRect(const Bezier2& curve, const RoundRect& roundRect)
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
				return IntersectsBezier2RectF(curve, rect);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return IntersectsBezier2RectF(curve, rect);
			}

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), rect))
			{
				return false;
			}

			return IntersectsBezier2ApproximateShape(curve, roundRect);
		}

		[[nodiscard]]
		bool IntersectsBezier2Polygon(const Bezier2& curve, const Polygon& polygon)
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF curveBounds = curve.computeBoundingRect();

			if (not BoundsIntersectClosed(curveBounds, polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, polygon)
				|| Geometry2D::Intersects(curve.p2, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsBezier2Triangle(curve, curveBounds, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		template <class Shape>
		[[nodiscard]]
		bool IntersectsBezier3ApproximateShape(const Bezier3& curve, const Shape& shape)
		{
			return VisitBezier3ApproximateLineSegments(curve, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, shape);
			});
		}

		[[nodiscard]]
		bool IntersectsBezier3Bezier3Approximate(const Bezier3& a, const Bezier3& b)
		{
			if (not BoundsIntersectClosed(a.computeBoundingRect(), b.computeBoundingRect()))
			{
				return false;
			}

			// The approximation direction must not depend on operand order.
			// Invalid non-finite control points are outside the geometry contract.
			if (BezierLexicographicalLess(b, a))
			{
				return IntersectsBezier3ApproximateShape(b, a);
			}

			return IntersectsBezier3ApproximateShape(a, b);
		}

		[[nodiscard]]
		bool IntersectsBezier3RectF(const Bezier3& curve, const RectF& rect)
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
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, curve);
			}

			if (h == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, curve);
			}

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), rect))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, rect)
				|| Geometry2D::Intersects(curve.p3, rect))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, curve)
				|| Geometry2D::Intersects(Line{ Vec2{ right, top }, Vec2{ right, bottom } }, curve)
				|| Geometry2D::Intersects(Line{ Vec2{ right, bottom }, Vec2{ left, bottom } }, curve)
				|| Geometry2D::Intersects(Line{ Vec2{ left, bottom }, Vec2{ left, top } }, curve));
		}

		[[nodiscard]]
		bool IntersectsBezier3Circle(const Bezier3& curve, const Circle& circle)
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

			const RectF circleBounds{
				(circle.center.x - circle.r),
				(circle.center.y - circle.r),
				(circle.r * 2.0),
				(circle.r * 2.0)
			};

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), circleBounds))
			{
				return false;
			}

			const double t = curve.computeClosestT(circle.center);
			return (curve.pointAt(t).distanceFromSq(circle.center) <= (circle.r * circle.r));
		}

		[[nodiscard]]
		bool IntersectsBezier3Ellipse(const Bezier3& curve, const Ellipse& ellipse)
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
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } }, curve);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } }, curve);
			}

			const RectF ellipseBounds{
				(ellipse.center.x - ax),
				(ellipse.center.y - by),
				(ax * 2.0),
				(by * 2.0)
			};

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), ellipseBounds))
			{
				return false;
			}

			const Bezier3 local{
				Vec2{ ((curve.p0.x - ellipse.center.x) / ax), ((curve.p0.y - ellipse.center.y) / by) },
				Vec2{ ((curve.p1.x - ellipse.center.x) / ax), ((curve.p1.y - ellipse.center.y) / by) },
				Vec2{ ((curve.p2.x - ellipse.center.x) / ax), ((curve.p2.y - ellipse.center.y) / by) },
				Vec2{ ((curve.p3.x - ellipse.center.x) / ax), ((curve.p3.y - ellipse.center.y) / by) }
			};

			const double t = local.computeClosestT(Vec2{ 0, 0 });
			return (local.pointAt(t).lengthSq() <= 1.0);
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitBezier3ApproximateLineSegments(const Bezier3& curve, Fty&& callback, const double maxError = 0.25, const int32 maxDepth = 8)
		{
			const double maxErrorSq = (maxError * maxError);
			const double flatnessK = (4.0 * maxErrorSq);
			const double kNearlyZeroSq = 1e-12;
			const int32 depthLimit = Max(0, maxDepth);

			auto Visit = [&](auto&& self, const Bezier3& c, const int32 depth) -> bool
				{
					const Vec2 chord = (c.p3 - c.p0);
					const double chordLenSq = chord.lengthSq();

					if (chordLenSq < kNearlyZeroSq)
					{
						const double p0p1LenSq = (c.p1 - c.p0).lengthSq();
						const double p1p2LenSq = (c.p2 - c.p1).lengthSq();
						const double p2p3LenSq = (c.p3 - c.p2).lengthSq();
						const double ctrlSpanSq = Max(Max(p0p1LenSq, p1p2LenSq), p2p3LenSq);

						if (ctrlSpanSq < kNearlyZeroSq)
						{
							return callback(Line{ c.p0, c.p3 });
						}

						if (depthLimit <= depth)
						{
							return (callback(Line{ c.p0, c.p1 })
								|| callback(Line{ c.p1, c.p2 })
								|| callback(Line{ c.p2, c.p3 }));
						}
					}
					else
					{
						const Vec2 v1 = (c.p1 - c.p0);
						const Vec2 v2 = (c.p2 - c.p0);
						const double cross1 = chord.cross(v1);
						const double cross2 = chord.cross(v2);
						bool acceptSegment = (((cross1 * cross1) <= (flatnessK * chordLenSq))
							&& ((cross2 * cross2) <= (flatnessK * chordLenSq)));

						if (acceptSegment)
						{
							const double dot1 = v1.dot(chord);
							const double dot2 = v2.dot(chord);

							if ((dot1 < 0.0) || (chordLenSq < dot1)
								|| (dot2 < 0.0) || (chordLenSq < dot2))
							{
								acceptSegment = false;
							}
						}

						if (acceptSegment || (depthLimit <= depth))
						{
							return callback(Line{ c.p0, c.p3 });
						}
					}

					const auto [left, right] = c.split(0.5);
					return (self(self, left, (depth + 1)) || self(self, right, (depth + 1)));
				};

			return Visit(Visit, curve, 0);
		}

		[[nodiscard]]
		bool IntersectsBezier3SuperEllipse(const Bezier3& curve, const SuperEllipse& superEllipse)
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } }, curve);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } }, curve);
			}

			if (n == 2.0)
			{
				return IntersectsBezier3Ellipse(curve, Ellipse{ superEllipse.center, ax, by });
			}

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), superEllipse.boundingRect()))
			{
				return false;
			}

			return IntersectsBezier3ApproximateShape(curve, superEllipse);
		}

		[[nodiscard]]
		bool IntersectsBezier3Triangle(const Bezier3& curve, const RectF& curveBounds, const Triangle& triangle)
		{
			if (not BoundsIntersectClosed(curveBounds, triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, triangle)
				|| Geometry2D::Intersects(curve.p3, triangle))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, curve)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, curve)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, curve));
		}

		[[nodiscard]]
		bool IntersectsBezier3Triangle(const Bezier3& curve, const Triangle& triangle)
		{
			return IntersectsBezier3Triangle(curve, curve.computeBoundingRect(), triangle);
		}

		[[nodiscard]]
		bool IntersectsBezier3Quad(const Bezier3& curve, const Quad& quad)
		{
			const RectF curveBounds = curve.computeBoundingRect();

			if (not BoundsIntersectClosed(curveBounds, quad.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, quad)
				|| Geometry2D::Intersects(curve.p3, quad))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, curve)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, curve)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, curve)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, curve));
		}

		[[nodiscard]]
		bool IntersectsBezier3RoundRect(const Bezier3& curve, const RoundRect& roundRect)
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
				return IntersectsBezier3RectF(curve, rect);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return IntersectsBezier3RectF(curve, rect);
			}

			if (not BoundsIntersectClosed(curve.computeBoundingRect(), rect))
			{
				return false;
			}

			return IntersectsBezier3ApproximateShape(curve, roundRect);
		}

		[[nodiscard]]
		bool IntersectsBezier3Polygon(const Bezier3& curve, const Polygon& polygon)
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF curveBounds = curve.computeBoundingRect();

			if (not BoundsIntersectClosed(curveBounds, polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, polygon)
				|| Geometry2D::Intersects(curve.p3, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsBezier3Triangle(curve, curveBounds, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsRectFTriangle(const RectF& rect, const Triangle& triangle) noexcept
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
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, triangle);
			}

			if (h == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, triangle);
			}

			if (not BoundsIntersectClosed(rect, triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(triangle.p0, rect)
				|| Geometry2D::Intersects(triangle.p1, rect)
				|| Geometry2D::Intersects(triangle.p2, rect))
			{
				return true;
			}

			if (Geometry2D::Intersects(Vec2{ left, top }, triangle)
				|| Geometry2D::Intersects(Vec2{ right, top }, triangle)
				|| Geometry2D::Intersects(Vec2{ right, bottom }, triangle)
				|| Geometry2D::Intersects(Vec2{ left, bottom }, triangle))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, triangle)
				|| Geometry2D::Intersects(Line{ Vec2{ right, top }, Vec2{ right, bottom } }, triangle)
				|| Geometry2D::Intersects(Line{ Vec2{ right, bottom }, Vec2{ left, bottom } }, triangle)
				|| Geometry2D::Intersects(Line{ Vec2{ left, bottom }, Vec2{ left, top } }, triangle));
		}

		[[nodiscard]]
		bool IntersectsRectFQuad(const RectF& rect, const Quad& quad) noexcept
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
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, quad);
			}

			if (h == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, quad);
			}

			if (not BoundsIntersectClosed(rect, quad.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(quad.p0, rect)
				|| Geometry2D::Intersects(quad.p1, rect)
				|| Geometry2D::Intersects(quad.p2, rect)
				|| Geometry2D::Intersects(quad.p3, rect))
			{
				return true;
			}

			if (Geometry2D::Intersects(Vec2{ left, top }, quad)
				|| Geometry2D::Intersects(Vec2{ right, top }, quad)
				|| Geometry2D::Intersects(Vec2{ right, bottom }, quad)
				|| Geometry2D::Intersects(Vec2{ left, bottom }, quad))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, quad)
				|| Geometry2D::Intersects(Line{ Vec2{ right, top }, Vec2{ right, bottom } }, quad)
				|| Geometry2D::Intersects(Line{ Vec2{ right, bottom }, Vec2{ left, bottom } }, quad)
				|| Geometry2D::Intersects(Line{ Vec2{ left, bottom }, Vec2{ left, top } }, quad));
		}

		[[nodiscard]]
		bool IntersectsRectFSuperEllipse(const RectF& rect, const SuperEllipse& superEllipse) noexcept
		{
			const double w = rect.size.x;
			const double h = rect.size.y;
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((w < 0.0) || (h < 0.0) || (ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h) && (0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if (((w == 0.0) && (h == 0.0))
				|| ((ax == 0.0) && (by == 0.0)))
			{
				return false;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, superEllipse);
			}

			if (h == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, superEllipse);
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(rect, Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(rect, Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } });
			}

			if (n == 2.0)
			{
				return Geometry2D::Intersects(rect, Ellipse{ superEllipse.center, ax, by });
			}

			const RectF superEllipseBounds{
				(superEllipse.center.x - ax),
				(superEllipse.center.y - by),
				(ax * 2.0),
				(by * 2.0)
			};

			if (not BoundsIntersectClosed(rect, superEllipseBounds))
			{
				return false;
			}

			if (Geometry2D::Intersects(superEllipse.center, rect)
				|| Geometry2D::Intersects(Vec2{ left, top }, superEllipse)
				|| Geometry2D::Intersects(Vec2{ right, top }, superEllipse)
				|| Geometry2D::Intersects(Vec2{ right, bottom }, superEllipse)
				|| Geometry2D::Intersects(Vec2{ left, bottom }, superEllipse))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, superEllipse)
				|| Geometry2D::Intersects(Line{ Vec2{ right, top }, Vec2{ right, bottom } }, superEllipse)
				|| Geometry2D::Intersects(Line{ Vec2{ right, bottom }, Vec2{ left, bottom } }, superEllipse)
				|| Geometry2D::Intersects(Line{ Vec2{ left, bottom }, Vec2{ left, top } }, superEllipse));
		}

		[[nodiscard]]
		bool IntersectsRectFRoundRect(const RectF& rect, const RoundRect& roundRect) noexcept
		{
			const double w = rect.size.x;
			const double h = rect.size.y;
			const RectF& rrRect = roundRect.rect;
			const double rrW = rrRect.size.x;
			const double rrH = rrRect.size.y;

			if ((w < 0.0) || (h < 0.0) || (rrW < 0.0) || (rrH < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h) && (0.0 <= rrW) && (0.0 <= rrH) && (0.0 <= roundRect.r));
				return false;
			}

			if (((w == 0.0) && (h == 0.0))
				|| ((rrW == 0.0) && (rrH == 0.0)))
			{
				return false;
			}

			if (not BoundsIntersectClosed(rect, rrRect))
			{
				return false;
			}

			const double er = Min(roundRect.r, Min((rrW * 0.5), (rrH * 0.5)));
			const RectF core = RoundRectCoreRect(rrRect, er);

			// A positive-size RoundRect is the Minkowski sum of its core
			// rectangle and a closed disk of radius er. This also covers
			// er == 0 and one-dimensional RectF / RoundRect degeneracies.
			return (DistanceClosedRects(rect, core) <= er);
		}

		[[nodiscard]]
		bool IntersectsRectFPolygon(const RectF& rect, const Polygon& polygon) noexcept
		{
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h));
				return false;
			}

			if (((w == 0.0) && (h == 0.0))
				|| polygon.isEmpty())
			{
				return false;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + w);
			const double bottom = (rect.pos.y + h);

			if (w == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ left, bottom } }, polygon);
			}

			if (h == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, polygon);
			}

			if (not BoundsIntersectClosed(rect, polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(Vec2{ left, top }, polygon)
				|| Geometry2D::Intersects(Vec2{ right, top }, polygon)
				|| Geometry2D::Intersects(Vec2{ right, bottom }, polygon)
				|| Geometry2D::Intersects(Vec2{ left, bottom }, polygon))
			{
				return true;
			}

			for (const auto& vertex : polygon.vertices())
			{
				if (Geometry2D::Intersects(Vec2{ vertex.x, vertex.y }, rect))
				{
					return true;
				}
			}

			if (Geometry2D::Intersects(Line{ Vec2{ left, top }, Vec2{ right, top } }, polygon)
				|| Geometry2D::Intersects(Line{ Vec2{ right, top }, Vec2{ right, bottom } }, polygon)
				|| Geometry2D::Intersects(Line{ Vec2{ right, bottom }, Vec2{ left, bottom } }, polygon)
				|| Geometry2D::Intersects(Line{ Vec2{ left, bottom }, Vec2{ left, top } }, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsRectFTriangle(rect, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		double DistancePointEllipse(const Vec2& p, const Ellipse& ellipse) noexcept
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

		template <class Fty>
		[[nodiscard]]
		bool VisitCircleApproximateLineSegments(const Circle& circle, Fty&& callback)
		{
			constexpr int32 SegmentCount = 64;
			constexpr double TwoPi = 6.2831853071795864769252867665590058;
			constexpr double Step = (TwoPi / SegmentCount);

			Vec2 previous{ (circle.center.x + circle.r), circle.center.y };

			for (int32 i = 1; i <= SegmentCount; ++i)
			{
				const double angle = (Step * i);
				const Vec2 current{ (circle.center.x + std::cos(angle) * circle.r), (circle.center.y + std::sin(angle) * circle.r) };

				if (callback(Line{ previous, current }))
				{
					return true;
				}

				previous = current;
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsCircleEllipse(const Circle& circle, const Ellipse& ellipse) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

			if ((circle.r < 0.0) || (ax < 0.0) || (by < 0.0))
			{
				assert((0.0 <= circle.r) && (0.0 <= ax) && (0.0 <= by));
				return false;
			}

			if ((circle.r == 0.0) || ((ax == 0.0) && (by == 0.0)))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(circle, Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(circle, Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } });
			}

			if (not BoundsIntersectClosed(circle.boundingRect(), ellipse.boundingRect()))
			{
				return false;
			}

			return (DistancePointEllipse(circle.center, ellipse) <= circle.r);
		}

		[[nodiscard]]
		bool IntersectsCircleSuperEllipse(const Circle& circle, const SuperEllipse& superEllipse) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((circle.r < 0.0) || (ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= circle.r) && (0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((circle.r == 0.0) || ((ax == 0.0) && (by == 0.0)))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(circle, Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(circle, Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } });
			}

			if (n == 2.0)
			{
				return IntersectsCircleEllipse(circle, Ellipse{ superEllipse.center, ax, by });
			}

			const RectF superEllipseBounds{ (superEllipse.center.x - ax), (superEllipse.center.y - by), (ax * 2.0), (by * 2.0) };

			if (not BoundsIntersectClosed(circle.boundingRect(), superEllipseBounds))
			{
				return false;
			}

			if (Geometry2D::Intersects(circle.center, superEllipse)
				|| Geometry2D::Intersects(superEllipse.center, circle))
			{
				return true;
			}

			return VisitCircleApproximateLineSegments(circle, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, superEllipse);
			});
		}

		[[nodiscard]]
		bool IntersectsCircleRoundRect(const Circle& circle, const RoundRect& roundRect) noexcept
		{
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((circle.r < 0.0) || (w < 0.0) || (h < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= circle.r) && (0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));
				return false;
			}

			if ((circle.r == 0.0) || ((w == 0.0) && (h == 0.0)))
			{
				return false;
			}

			if (not BoundsIntersectClosed(circle.boundingRect(), rect))
			{
				return false;
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));
			const RectF core = RoundRectCoreRect(rect, er);

			// (core + disk(er)) intersects disk(circle.r) exactly when the
			// circle center is no farther than the sum of the two radii.
			return (DistancePointClosedRect(circle.center, core) <= (circle.r + er));
		}

		[[nodiscard]]
		bool IntersectsEllipseTriangle(const Ellipse& ellipse, const Triangle& triangle) noexcept
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
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } }, triangle);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } }, triangle);
			}

			if (not BoundsIntersectClosed(ellipse.boundingRect(), triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(ellipse.center, triangle)
				|| Geometry2D::Intersects(triangle.p0, ellipse)
				|| Geometry2D::Intersects(triangle.p1, ellipse)
				|| Geometry2D::Intersects(triangle.p2, ellipse))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, ellipse)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, ellipse)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, ellipse));
		}

		[[nodiscard]]
		bool IntersectsEllipseQuad(const Ellipse& ellipse, const Quad& quad) noexcept
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
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } }, quad);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } }, quad);
			}

			if (not BoundsIntersectClosed(ellipse.boundingRect(), quad.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(ellipse.center, quad)
				|| Geometry2D::Intersects(quad.p0, ellipse)
				|| Geometry2D::Intersects(quad.p1, ellipse)
				|| Geometry2D::Intersects(quad.p2, ellipse)
				|| Geometry2D::Intersects(quad.p3, ellipse))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, ellipse)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, ellipse)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, ellipse)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, ellipse));
		}

		[[nodiscard]]
		bool IntersectsEllipsePolygon(const Ellipse& ellipse, const Polygon& polygon) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

			if ((ax < 0.0) || (by < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by));
				return false;
			}

			if (((ax == 0.0) && (by == 0.0)) || polygon.isEmpty())
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } }, polygon);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } }, polygon);
			}

			if (not BoundsIntersectClosed(ellipse.boundingRect(), polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(ellipse.center, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsEllipseTriangle(ellipse, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsEllipseMultiPolygon(const Ellipse& ellipse, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsEllipsePolygon(ellipse, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseTriangle(const SuperEllipse& superEllipse, const Triangle& triangle) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } }, triangle);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } }, triangle);
			}

			if (n == 2.0)
			{
				return IntersectsEllipseTriangle(Ellipse{ superEllipse.center, ax, by }, triangle);
			}

			if (not BoundsIntersectClosed(superEllipse.boundingRect(), triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(superEllipse.center, triangle)
				|| Geometry2D::Intersects(triangle.p0, superEllipse)
				|| Geometry2D::Intersects(triangle.p1, superEllipse)
				|| Geometry2D::Intersects(triangle.p2, superEllipse))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, superEllipse)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, superEllipse)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, superEllipse));
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseQuad(const SuperEllipse& superEllipse, const Quad& quad) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } }, quad);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } }, quad);
			}

			if (n == 2.0)
			{
				return IntersectsEllipseQuad(Ellipse{ superEllipse.center, ax, by }, quad);
			}

			if (not BoundsIntersectClosed(superEllipse.boundingRect(), quad.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(superEllipse.center, quad)
				|| Geometry2D::Intersects(quad.p0, superEllipse)
				|| Geometry2D::Intersects(quad.p1, superEllipse)
				|| Geometry2D::Intersects(quad.p2, superEllipse)
				|| Geometry2D::Intersects(quad.p3, superEllipse))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, superEllipse)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, superEllipse)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, superEllipse)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, superEllipse));
		}

		[[nodiscard]]
		bool IntersectsSuperEllipsePolygon(const SuperEllipse& superEllipse, const Polygon& polygon) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if (((ax == 0.0) && (by == 0.0)) || polygon.isEmpty())
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } }, polygon);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } }, polygon);
			}

			if (n == 2.0)
			{
				return IntersectsEllipsePolygon(Ellipse{ superEllipse.center, ax, by }, polygon);
			}

			if (not BoundsIntersectClosed(superEllipse.boundingRect(), polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(superEllipse.center, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsSuperEllipseTriangle(superEllipse, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseMultiPolygon(const SuperEllipse& superEllipse, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsSuperEllipsePolygon(superEllipse, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsTrianglePolygon(const Triangle& triangle, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			if (not BoundsIntersectClosed(triangle.boundingRect(), polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(triangle.p0, polygon)
				|| Geometry2D::Intersects(triangle.p1, polygon)
				|| Geometry2D::Intersects(triangle.p2, polygon))
			{
				return true;
			}

			for (const auto& vertex : polygon.vertices())
			{
				if (Geometry2D::Intersects(Vec2{ vertex.x, vertex.y }, triangle))
				{
					return true;
				}
			}

			if (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, polygon)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, polygon)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (Geometry2D::Intersects(triangle, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsQuadPolygon(const Quad& quad, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			if (not BoundsIntersectClosed(quad.boundingRect(), polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(quad.p0, polygon)
				|| Geometry2D::Intersects(quad.p1, polygon)
				|| Geometry2D::Intersects(quad.p2, polygon)
				|| Geometry2D::Intersects(quad.p3, polygon))
			{
				return true;
			}

			for (const auto& vertex : polygon.vertices())
			{
				if (Geometry2D::Intersects(Vec2{ vertex.x, vertex.y }, quad))
				{
					return true;
				}
			}

			if (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, polygon)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, polygon)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, polygon)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (Geometry2D::Intersects(quad, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsCircleTriangle(const Circle& circle, const Triangle& triangle) noexcept
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

			if (not BoundsIntersectClosed(circle.boundingRect(), triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(circle.center, triangle)
				|| Geometry2D::Intersects(triangle.p0, circle)
				|| Geometry2D::Intersects(triangle.p1, circle)
				|| Geometry2D::Intersects(triangle.p2, circle))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, circle)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, circle)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, circle));
		}

		[[nodiscard]]
		bool IntersectsCircleQuad(const Circle& circle, const Quad& quad) noexcept
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

			if (not BoundsIntersectClosed(circle.boundingRect(), quad.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(circle.center, quad)
				|| Geometry2D::Intersects(quad.p0, circle)
				|| Geometry2D::Intersects(quad.p1, circle)
				|| Geometry2D::Intersects(quad.p2, circle)
				|| Geometry2D::Intersects(quad.p3, circle))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, circle)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, circle)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, circle)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, circle));
		}

		[[nodiscard]]
		bool IntersectsCirclePolygon(const Circle& circle, const Polygon& polygon) noexcept
		{
			if (circle.r < 0.0)
			{
				assert(0.0 <= circle.r);
				return false;
			}

			if ((circle.r == 0.0) || polygon.isEmpty())
			{
				return false;
			}

			if (not BoundsIntersectClosed(circle.boundingRect(), polygon.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(circle.center, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsCircleTriangle(circle, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsCircleMultiPolygon(const Circle& circle, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsCirclePolygon(circle, polygon))
				{
					return true;
				}
			}

			return false;
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitEllipseApproximateLineSegments(const Ellipse& ellipse, Fty&& callback)
		{
			constexpr int32 SegmentCount = 64;
			constexpr double TwoPi = 6.2831853071795864769252867665590058;
			constexpr double Step = (TwoPi / SegmentCount);

			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;
			Vec2 previous{ (ellipse.center.x + ax), ellipse.center.y };

			for (int32 i = 1; i <= SegmentCount; ++i)
			{
				const double angle = (Step * i);
				const Vec2 current{ (ellipse.center.x + std::cos(angle) * ax), (ellipse.center.y + std::sin(angle) * by) };

				if (callback(Line{ previous, current }))
				{
					return true;
				}

				previous = current;
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsEllipseEllipse(const Ellipse& a, const Ellipse& b) noexcept
		{
			const double ax = a.axes.x;
			const double ay = a.axes.y;
			const double bx = b.axes.x;
			const double by = b.axes.y;

			if ((ax < 0.0) || (ay < 0.0) || (bx < 0.0) || (by < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= ay) && (0.0 <= bx) && (0.0 <= by));
				return false;
			}

			if (((ax == 0.0) && (ay == 0.0))
				|| ((bx == 0.0) && (by == 0.0)))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ a.center.x, (a.center.y - ay) }, Vec2{ a.center.x, (a.center.y + ay) } }, b);
			}

			if (ay == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (a.center.x - ax), a.center.y }, Vec2{ (a.center.x + ax), a.center.y } }, b);
			}

			if (bx == 0.0)
			{
				return Geometry2D::Intersects(a, Line{ Vec2{ b.center.x, (b.center.y - by) }, Vec2{ b.center.x, (b.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(a, Line{ Vec2{ (b.center.x - bx), b.center.y }, Vec2{ (b.center.x + bx), b.center.y } });
			}

			if (not BoundsIntersectClosed(a.boundingRect(), b.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(a.center, b)
				|| Geometry2D::Intersects(b.center, a))
			{
				return true;
			}

			return VisitEllipseApproximateLineSegments(a, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, b);
			});
		}

		[[nodiscard]]
		bool IntersectsEllipseSuperEllipse(const Ellipse& ellipse, const SuperEllipse& superEllipse) noexcept
		{
			const double ex = ellipse.axes.x;
			const double ey = ellipse.axes.y;
			const double sx = superEllipse.axes.x;
			const double sy = superEllipse.axes.y;
			const double n = superEllipse.n;

			if ((ex < 0.0) || (ey < 0.0) || (sx < 0.0) || (sy < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ex) && (0.0 <= ey) && (0.0 <= sx) && (0.0 <= sy) && (0.0 < n));
				return false;
			}

			if (((ex == 0.0) && (ey == 0.0))
				|| ((sx == 0.0) && (sy == 0.0)))
			{
				return false;
			}

			if (ex == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - ey) }, Vec2{ ellipse.center.x, (ellipse.center.y + ey) } }, superEllipse);
			}

			if (ey == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ex), ellipse.center.y }, Vec2{ (ellipse.center.x + ex), ellipse.center.y } }, superEllipse);
			}

			if (sx == 0.0)
			{
				return Geometry2D::Intersects(ellipse, Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - sy) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + sy) } });
			}

			if (sy == 0.0)
			{
				return Geometry2D::Intersects(ellipse, Line{ Vec2{ (superEllipse.center.x - sx), superEllipse.center.y }, Vec2{ (superEllipse.center.x + sx), superEllipse.center.y } });
			}

			if (n == 2.0)
			{
				return IntersectsEllipseEllipse(ellipse, Ellipse{ superEllipse.center, sx, sy });
			}

			const RectF superEllipseBounds{ (superEllipse.center.x - sx), (superEllipse.center.y - sy), (sx * 2.0), (sy * 2.0) };

			if (not BoundsIntersectClosed(ellipse.boundingRect(), superEllipseBounds))
			{
				return false;
			}

			if (Geometry2D::Intersects(ellipse.center, superEllipse)
				|| Geometry2D::Intersects(superEllipse.center, ellipse))
			{
				return true;
			}

			return VisitEllipseApproximateLineSegments(ellipse, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, superEllipse);
			});
		}

		[[nodiscard]]
		bool IntersectsEllipseRoundRect(const Ellipse& ellipse, const RoundRect& roundRect) noexcept
		{
			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((ax < 0.0) || (by < 0.0) || (w < 0.0) || (h < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));
				return false;
			}

			if (((ax == 0.0) && (by == 0.0))
				|| ((w == 0.0) && (h == 0.0)))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ ellipse.center.x, (ellipse.center.y - by) }, Vec2{ ellipse.center.x, (ellipse.center.y + by) } }, roundRect);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (ellipse.center.x - ax), ellipse.center.y }, Vec2{ (ellipse.center.x + ax), ellipse.center.y } }, roundRect);
			}

			if ((w == 0.0) || (h == 0.0))
			{
				return Geometry2D::Intersects(ellipse, rect);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return Geometry2D::Intersects(ellipse, rect);
			}

			if (not BoundsIntersectClosed(ellipse.boundingRect(), rect))
			{
				return false;
			}

			const Vec2 roundRectCenter{ (rect.pos.x + (w * 0.5)), (rect.pos.y + (h * 0.5)) };

			if (Geometry2D::Intersects(ellipse.center, roundRect)
				|| Geometry2D::Intersects(roundRectCenter, ellipse))
			{
				return true;
			}

			return VisitEllipseApproximateLineSegments(ellipse, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, roundRect);
			});
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitSuperEllipseApproximateLineSegments(const SuperEllipse& superEllipse, Fty&& callback)
		{
			constexpr int32 SegmentCount = 96;
			constexpr double TwoPi = 6.2831853071795864769252867665590058;
			constexpr double Step = (TwoPi / SegmentCount);

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double exponent = (2.0 / superEllipse.n);

			auto SignedUnitPower = [exponent](const double v) noexcept
			{
				const double p = std::pow(Abs(v), exponent);
				return ((v < 0.0) ? -p : p);
			};

			auto PointAtAngle = [&](const double angle) noexcept
			{
				return Vec2{
					(superEllipse.center.x + ax * SignedUnitPower(std::cos(angle))),
					(superEllipse.center.y + by * SignedUnitPower(std::sin(angle)))
				};
			};

			Vec2 previous = PointAtAngle(0.0);

			for (int32 i = 1; i <= SegmentCount; ++i)
			{
				const Vec2 current = PointAtAngle(Step * i);

				if ((previous != current) && callback(Line{ previous, current }))
				{
					return true;
				}

				previous = current;
			}

			return false;
		}

		[[nodiscard]]
		double SuperEllipseVerticalRadiusAtX(const SuperEllipse& shape, const double x) noexcept
		{
			const double normalizedX = Abs((x - shape.center.x) / shape.axes.x);

			if (1.0 <= normalizedX)
			{
				return 0.0;
			}

			const double remaining = Max(0.0, (1.0 - std::pow(normalizedX, shape.n)));
			return (shape.axes.y * std::pow(remaining, (1.0 / shape.n)));
		}

		[[nodiscard]]
		double MaximumSuperEllipseVerticalRadiusInInterval(const SuperEllipse& shape, const double left, const double right) noexcept
		{
			return SuperEllipseVerticalRadiusAtX(shape, Clamp(shape.center.x, left, right));
		}

		[[nodiscard]]
		bool IntersectsPositiveAreaSuperEllipses(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			const double left = Max((a.center.x - a.axes.x), (b.center.x - b.axes.x));
			const double right = Min((a.center.x + a.axes.x), (b.center.x + b.axes.x));

			if (right < left)
			{
				return false;
			}

			const double centerDistanceY = Abs(a.center.y - b.center.y);

			auto VerticalIntervalsOverlapAt = [&](const double x) noexcept
			{
				return (centerDistanceY <= (SuperEllipseVerticalRadiusAtX(a, x)
					+ SuperEllipseVerticalRadiusAtX(b, x)));
			};

			// These points include the interval endpoints and both possible cusp / maximum locations.
			if (VerticalIntervalsOverlapAt(left)
				|| VerticalIntervalsOverlapAt(right)
				|| VerticalIntervalsOverlapAt(Clamp(a.center.x, left, right))
				|| VerticalIntervalsOverlapAt(Clamp(b.center.x, left, right)))
			{
				return true;
			}

			struct Interval
			{
				double left;
				double right;
				int32 depth;
			};

			constexpr int32 MaxDepth = 64;
			std::array<Interval, (MaxDepth + 2)> stack;
			size_t stackSize = 0;
			stack[stackSize++] = Interval{ left, right, 0 };

			while (stackSize)
			{
				const Interval interval = stack[--stackSize];
				const double upperBound = (MaximumSuperEllipseVerticalRadiusInInterval(a, interval.left, interval.right)
					+ MaximumSuperEllipseVerticalRadiusInInterval(b, interval.left, interval.right));

				if (upperBound < centerDistanceY)
				{
					continue;
				}

				const double middle = (interval.left + ((interval.right - interval.left) * 0.5));

				if (VerticalIntervalsOverlapAt(middle))
				{
					return true;
				}

				if ((MaxDepth <= interval.depth)
					|| (middle == interval.left)
					|| (middle == interval.right))
				{
					// At machine-resolution ambiguity, prefer the closed-set result.
					return true;
				}

				assert((stackSize + 2) <= stack.size());
				stack[stackSize++] = Interval{ middle, interval.right, (interval.depth + 1) };
				stack[stackSize++] = Interval{ interval.left, middle, (interval.depth + 1) };
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseSuperEllipse(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			const double ax = a.axes.x;
			const double ay = a.axes.y;
			const double bx = b.axes.x;
			const double by = b.axes.y;

			if ((ax < 0.0) || (ay < 0.0) || (a.n <= 0.0)
				|| (bx < 0.0) || (by < 0.0) || (b.n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= ay) && (0.0 < a.n)
					&& (0.0 <= bx) && (0.0 <= by) && (0.0 < b.n));
				return false;
			}

			if (((ax == 0.0) && (ay == 0.0))
				|| ((bx == 0.0) && (by == 0.0)))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ a.center.x, (a.center.y - ay) }, Vec2{ a.center.x, (a.center.y + ay) } }, b);
			}

			if (ay == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (a.center.x - ax), a.center.y }, Vec2{ (a.center.x + ax), a.center.y } }, b);
			}

			if (bx == 0.0)
			{
				return Geometry2D::Intersects(a, Line{ Vec2{ b.center.x, (b.center.y - by) }, Vec2{ b.center.x, (b.center.y + by) } });
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(a, Line{ Vec2{ (b.center.x - bx), b.center.y }, Vec2{ (b.center.x + bx), b.center.y } });
			}

			if (a.n == 2.0)
			{
				return IntersectsEllipseSuperEllipse(Ellipse{ a.center, ax, ay }, b);
			}

			if (b.n == 2.0)
			{
				return IntersectsEllipseSuperEllipse(Ellipse{ b.center, bx, by }, a);
			}

			return IntersectsPositiveAreaSuperEllipses(a, b);
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseRoundRect(const SuperEllipse& superEllipse, const RoundRect& roundRect) noexcept
		{
			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((ax < 0.0) || (by < 0.0) || (superEllipse.n <= 0.0)
				|| (w < 0.0) || (h < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < superEllipse.n)
					&& (0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));
				return false;
			}

			if (((ax == 0.0) && (by == 0.0))
				|| ((w == 0.0) && (h == 0.0)))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ superEllipse.center.x, (superEllipse.center.y - by) }, Vec2{ superEllipse.center.x, (superEllipse.center.y + by) } }, roundRect);
			}

			if (by == 0.0)
			{
				return Geometry2D::Intersects(Line{ Vec2{ (superEllipse.center.x - ax), superEllipse.center.y }, Vec2{ (superEllipse.center.x + ax), superEllipse.center.y } }, roundRect);
			}

			if (superEllipse.n == 2.0)
			{
				return IntersectsEllipseRoundRect(Ellipse{ superEllipse.center, ax, by }, roundRect);
			}

			if ((w == 0.0) || (h == 0.0))
			{
				return Geometry2D::Intersects(superEllipse, rect);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return IntersectsRectFSuperEllipse(rect, superEllipse);
			}

			if (not BoundsIntersectClosed(superEllipse.boundingRect(), rect))
			{
				return false;
			}

			const Vec2 roundRectCenter{ (rect.pos.x + (w * 0.5)), (rect.pos.y + (h * 0.5)) };

			if (Geometry2D::Intersects(superEllipse.center, roundRect)
				|| Geometry2D::Intersects(roundRectCenter, superEllipse))
			{
				return true;
			}

			return VisitSuperEllipseApproximateLineSegments(superEllipse, [&](const Line& segment)
			{
				return Geometry2D::Intersects(segment, roundRect);
			});
		}

		[[nodiscard]]
		bool IntersectsRoundRectTriangle(const RoundRect& roundRect, const Triangle& triangle) noexcept
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
				return IntersectsRectFTriangle(rect, triangle);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return IntersectsRectFTriangle(rect, triangle);
			}

			if (not BoundsIntersectClosed(rect, triangle.boundingRect()))
			{
				return false;
			}

			const Vec2 center{ (rect.pos.x + (w * 0.5)), (rect.pos.y + (h * 0.5)) };

			if (Geometry2D::Intersects(center, triangle)
				|| Geometry2D::Intersects(triangle.p0, roundRect)
				|| Geometry2D::Intersects(triangle.p1, roundRect)
				|| Geometry2D::Intersects(triangle.p2, roundRect))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, roundRect)
				|| Geometry2D::Intersects(Line{ triangle.p1, triangle.p2 }, roundRect)
				|| Geometry2D::Intersects(Line{ triangle.p2, triangle.p0 }, roundRect));
		}

		[[nodiscard]]
		bool IntersectsRoundRectQuad(const RoundRect& roundRect, const Quad& quad) noexcept
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
				return IntersectsRectFQuad(rect, quad);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return IntersectsRectFQuad(rect, quad);
			}

			if (not BoundsIntersectClosed(rect, quad.boundingRect()))
			{
				return false;
			}

			const Vec2 center{ (rect.pos.x + (w * 0.5)), (rect.pos.y + (h * 0.5)) };

			if (Geometry2D::Intersects(center, quad)
				|| Geometry2D::Intersects(quad.p0, roundRect)
				|| Geometry2D::Intersects(quad.p1, roundRect)
				|| Geometry2D::Intersects(quad.p2, roundRect)
				|| Geometry2D::Intersects(quad.p3, roundRect))
			{
				return true;
			}

			return (Geometry2D::Intersects(Line{ quad.p0, quad.p1 }, roundRect)
				|| Geometry2D::Intersects(Line{ quad.p1, quad.p2 }, roundRect)
				|| Geometry2D::Intersects(Line{ quad.p2, quad.p3 }, roundRect)
				|| Geometry2D::Intersects(Line{ quad.p3, quad.p0 }, roundRect));
		}

		[[nodiscard]]
		bool IntersectsRoundRectRoundRect(const RoundRect& a, const RoundRect& b) noexcept
		{
			const RectF& ar = a.rect;
			const RectF& br = b.rect;
			const double aw = ar.size.x;
			const double ah = ar.size.y;
			const double bw = br.size.x;
			const double bh = br.size.y;

			if ((aw < 0.0) || (ah < 0.0) || (a.r < 0.0)
				|| (bw < 0.0) || (bh < 0.0) || (b.r < 0.0))
			{
				assert((0.0 <= aw) && (0.0 <= ah) && (0.0 <= a.r)
					&& (0.0 <= bw) && (0.0 <= bh) && (0.0 <= b.r));
				return false;
			}

			if (((aw == 0.0) && (ah == 0.0))
				|| ((bw == 0.0) && (bh == 0.0)))
			{
				return false;
			}

			if (not BoundsIntersectClosed(ar, br))
			{
				return false;
			}

			const double aer = Min(a.r, Min((aw * 0.5), (ah * 0.5)));
			const double ber = Min(b.r, Min((bw * 0.5), (bh * 0.5)));
			const RectF aCore = RoundRectCoreRect(ar, aer);
			const RectF bCore = RoundRectCoreRect(br, ber);

			// Each RoundRect is core + disk(radius). The two sets intersect
			// exactly when the distance between the closed core rectangles is
			// at most the sum of the effective radii.
			return (DistanceClosedRects(aCore, bCore) <= (aer + ber));
		}

		[[nodiscard]]
		bool IntersectsRoundRectPolygon(const RoundRect& roundRect, const Polygon& polygon) noexcept
		{
			const RectF& rect = roundRect.rect;
			const double w = rect.size.x;
			const double h = rect.size.y;

			if ((w < 0.0) || (h < 0.0) || (roundRect.r < 0.0))
			{
				assert((0.0 <= w) && (0.0 <= h) && (0.0 <= roundRect.r));
				return false;
			}

			if (((w == 0.0) && (h == 0.0)) || polygon.isEmpty())
			{
				return false;
			}

			if ((w == 0.0) || (h == 0.0))
			{
				return IntersectsRectFPolygon(rect, polygon);
			}

			const double er = Min(roundRect.r, Min((w * 0.5), (h * 0.5)));

			if (er == 0.0)
			{
				return IntersectsRectFPolygon(rect, polygon);
			}

			if (not BoundsIntersectClosed(rect, polygon.boundingRect()))
			{
				return false;
			}

			const Vec2 center{ (rect.pos.x + (w * 0.5)), (rect.pos.y + (h * 0.5)) };

			if (Geometry2D::Intersects(center, polygon))
			{
				return true;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsRoundRectTriangle(roundRect, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsRoundRectMultiPolygon(const RoundRect& roundRect, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsRoundRectPolygon(roundRect, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsPolygonPolygon(const Polygon& a, const Polygon& b) noexcept
		{
			if (a.isEmpty() || b.isEmpty())
			{
				return false;
			}

			if (not BoundsIntersectClosed(a.boundingRect(), b.boundingRect()))
			{
				return false;
			}

			const Float2* pVertex = a.vertices().data();

			for (const auto& triangleIndex : a.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (IntersectsTrianglePolygon(Triangle{ p0, p1, p2 }, b))
				{
					return true;
				}
			}

			return false;
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Point, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Point& p, const LineString& segments) noexcept
		{
			return Intersects(Vec2{ p }, segments);
		}

		bool Intersects(const Point& p, const Bezier2& curve)
		{
			return Intersects(Vec2{ p }, curve);
		}

		bool Intersects(const Point& p, const Bezier3& curve)
		{
			return Intersects(Vec2{ p }, curve);
		}

		bool Intersects(const Point& p, const SuperEllipse& superEllipse) noexcept
		{
			return Intersects(Vec2{ p }, superEllipse);
		}

		bool Intersects(const Point& p, const Polygon& polygon) noexcept
		{
			return Intersects(Vec2{ p }, polygon);
		}

		bool Intersects(const Point& p, const MultiPolygon& multiPolygon) noexcept
		{
			return Intersects(Vec2{ p }, multiPolygon);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Vec2& p, const LineString& segments) noexcept
		{
			const size_t n = segments.size();

			if (n == 0)
			{
				return false;
			}

			const Vec2* ps = segments.data();

			if (n == 1)
			{
				return (p == ps[0]);
			}

			for (size_t i = 0; i < (n - 1); ++i)
			{
				if (Intersects(p, Line{ ps[i], ps[i + 1] }))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersects(const Vec2& p, const Bezier2& curve)
		{
			return IntersectsPointBezier2(p, curve);
		}

		bool Intersects(const Vec2& p, const Bezier3& curve)
		{
			return IntersectsPointBezier3(p, curve);
		}

		bool Intersects(const Vec2& p, const SuperEllipse& superEllipse) noexcept
		{
			const double ax	= superEllipse.axes.x;
			const double by	= superEllipse.axes.y;
			const double n	= superEllipse.n;

			if ((ax < 0.0) || (by < 0.0) || (n <= 0.0))
			{
				assert((0.0 <= ax) && (0.0 <= by) && (0.0 < n));
				return false;
			}

			if ((ax == 0.0) && (by == 0.0))
			{
				return false;
			}

			if (ax == 0.0)
			{
				return ((p.x == superEllipse.center.x)
					&& (Abs(p.y - superEllipse.center.y) <= by));
			}

			if (by == 0.0)
			{
				return ((p.y == superEllipse.center.y)
					&& (Abs(p.x - superEllipse.center.x) <= ax));
			}

			const double dx = Abs((p.x - superEllipse.center.x) / ax);
			const double dy = Abs((p.y - superEllipse.center.y) / by);

			if ((1.0 < dx) || (1.0 < dy))
			{
				return false;
			}

			if (n == 2.0)
			{
				return ((dx * dx + dy * dy) <= 1.0);
			}

			return ((std::pow(dx, n) + std::pow(dy, n)) <= 1.0);
		}

		bool Intersects(const Vec2& p, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			if (not Intersects(p, polygon.boundingRect()))
			{
				return false;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				if (ContainsPoint<detail::ConvexClockwise>(pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2], p))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersects(const Vec2& p, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Intersects(p, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Line, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Line& segment, const LineString& segments) noexcept
		{
			const size_t n = segments.size();

			if (n == 0)
			{
				return false;
			}

			const Vec2* ps = segments.data();

			if (n == 1)
			{
				return Intersects(segment, ps[0]);
			}

			for (size_t i = 0; i < (n - 1); ++i)
			{
				if (Intersects(segment, Line{ ps[i], ps[i + 1] }))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersects(const Line& segment, const Bezier2& curve)
		{
			return IntersectsLineBezier2(segment, curve);
		}

		bool Intersects(const Line& segment, const Bezier3& curve)
		{
			return IntersectsLineBezier3(segment, curve);
		}

		bool Intersects(const Line& segment, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsLineSuperEllipse(segment, superEllipse);
		}

		bool Intersects(const Line& segment, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			if (not Intersects(segment, polygon.boundingRect()))
			{
				return false;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Vec2 p0{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y };
				const Vec2 p1{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y };
				const Vec2 p2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y };

				if (Intersects(segment, Triangle{ p0, p1, p2 }))
				{
					return true;
				}
			}

			return false;
		}

		bool Intersects(const Line& segment, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Intersects(segment, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(LineString, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const LineString& segments, const Point& p) noexcept
		{
			return Intersects(segments, Vec2{ p });
		}

		bool Intersects(const LineString& segments, const Vec2& p) noexcept
		{
			return Intersects(p, segments);
		}

		bool Intersects(const LineString& segments, const Line& segment) noexcept
		{
			return Intersects(segment, segments);
		}

		bool Intersects(const LineString& segments1, const LineString& segments2) noexcept
		{
			return IntersectsLineStringShape(segments1, segments2);
		}

		bool Intersects(const LineString& segments, const Bezier2& curve)
		{
			return IntersectsLineStringShape(segments, curve);
		}

		bool Intersects(const LineString& segments, const Bezier3& curve)
		{
			return IntersectsLineStringShape(segments, curve);
		}

		bool Intersects(const LineString& segments, const Rect& rect) noexcept
		{
			return IntersectsLineStringShape(segments, rect);
		}

		bool Intersects(const LineString& segments, const RectF& rect) noexcept
		{
			return IntersectsLineStringShape(segments, rect);
		}

		bool Intersects(const LineString& segments, const Circle& circle) noexcept
		{
			return IntersectsLineStringShape(segments, circle);
		}

		bool Intersects(const LineString& segments, const Ellipse& ellipse) noexcept
		{
			return IntersectsLineStringShape(segments, ellipse);
		}

		bool Intersects(const LineString& segments, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsLineStringShape(segments, superEllipse);
		}

		bool Intersects(const LineString& segments, const Triangle& triangle) noexcept
		{
			return IntersectsLineStringShape(segments, triangle);
		}

		bool Intersects(const LineString& segments, const Quad& quad) noexcept
		{
			return IntersectsLineStringShape(segments, quad);
		}

		bool Intersects(const LineString& segments, const RoundRect& roundRect) noexcept
		{
			return IntersectsLineStringShape(segments, roundRect);
		}

		bool Intersects(const LineString& segments, const Polygon& polygon) noexcept
		{
			return IntersectsLineStringShape(segments, polygon);
		}

		bool Intersects(const LineString& segments, const MultiPolygon& multiPolygon) noexcept
		{
			return IntersectsLineStringShape(segments, multiPolygon);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Bezier2, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Bezier2& curve, const Point& p)
		{
			return Intersects(p, curve);
		}

		bool Intersects(const Bezier2& curve, const Vec2& p)
		{
			return Intersects(p, curve);
		}

		bool Intersects(const Bezier2& curve, const Line& segment)
		{
			return Intersects(segment, curve);
		}

		bool Intersects(const Bezier2& curve, const LineString& segments)
		{
			return Intersects(segments, curve);
		}

		bool Intersects(const Bezier2& curve1, const Bezier2& curve2)
		{
			return IntersectsBezier2Bezier2Approximate(curve1, curve2);
		}

		bool Intersects(const Bezier2& curve1, const Bezier3& curve2)
		{
			return IntersectsBezier2Bezier3Approximate(curve1, curve2);
		}

		bool Intersects(const Bezier2& curve, const Rect& rect)
		{
			return IntersectsBezier2RectF(curve, RectF{ rect });
		}

		bool Intersects(const Bezier2& curve, const RectF& rect)
		{
			return IntersectsBezier2RectF(curve, rect);
		}

		bool Intersects(const Bezier2& curve, const Circle& circle)
		{
			return IntersectsBezier2Circle(curve, circle);
		}

		bool Intersects(const Bezier2& curve, const Ellipse& ellipse)
		{
			return IntersectsBezier2Ellipse(curve, ellipse);
		}

		bool Intersects(const Bezier2& curve, const Triangle& triangle)
		{
			return IntersectsBezier2Triangle(curve, triangle);
		}

		bool Intersects(const Bezier2& curve, const Quad& quad)
		{
			return IntersectsBezier2Quad(curve, quad);
		}

		bool Intersects(const Bezier2& curve, const SuperEllipse& superEllipse)
		{
			return IntersectsBezier2SuperEllipse(curve, superEllipse);
		}

		bool Intersects(const Bezier2& curve, const RoundRect& roundRect)
		{
			return IntersectsBezier2RoundRect(curve, roundRect);
		}

		bool Intersects(const Bezier2& curve, const Polygon& polygon)
		{
			return IntersectsBezier2Polygon(curve, polygon);
		}

		bool Intersects(const Bezier2& curve, const MultiPolygon& multiPolygon)
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Intersects(curve, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Bezier3, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Bezier3& curve, const Point& p)
		{
			return Intersects(p, curve);
		}

		bool Intersects(const Bezier3& curve, const Vec2& p)
		{
			return Intersects(p, curve);
		}

		bool Intersects(const Bezier3& curve, const Line& segment)
		{
			return Intersects(segment, curve);
		}

		bool Intersects(const Bezier3& curve, const LineString& segments)
		{
			return Intersects(segments, curve);
		}

		bool Intersects(const Bezier3& curve1, const Bezier2& curve2)
		{
			return Intersects(curve2, curve1);
		}

		bool Intersects(const Bezier3& curve1, const Bezier3& curve2)
		{
			return IntersectsBezier3Bezier3Approximate(curve1, curve2);
		}

		bool Intersects(const Bezier3& curve, const Rect& rect)
		{
			return IntersectsBezier3RectF(curve, RectF{ rect });
		}

		bool Intersects(const Bezier3& curve, const RectF& rect)
		{
			return IntersectsBezier3RectF(curve, rect);
		}

		bool Intersects(const Bezier3& curve, const Circle& circle)
		{
			return IntersectsBezier3Circle(curve, circle);
		}

		bool Intersects(const Bezier3& curve, const Ellipse& ellipse)
		{
			return IntersectsBezier3Ellipse(curve, ellipse);
		}

		bool Intersects(const Bezier3& curve, const SuperEllipse& superEllipse)
		{
			return IntersectsBezier3SuperEllipse(curve, superEllipse);
		}

		bool Intersects(const Bezier3& curve, const Triangle& triangle)
		{
			return IntersectsBezier3Triangle(curve, triangle);
		}

		bool Intersects(const Bezier3& curve, const Quad& quad)
		{
			return IntersectsBezier3Quad(curve, quad);
		}

		bool Intersects(const Bezier3& curve, const RoundRect& roundRect)
		{
			return IntersectsBezier3RoundRect(curve, roundRect);
		}

		bool Intersects(const Bezier3& curve, const Polygon& polygon)
		{
			return IntersectsBezier3Polygon(curve, polygon);
		}

		bool Intersects(const Bezier3& curve, const MultiPolygon& multiPolygon)
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Intersects(curve, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Rect& rect, const LineString& segments) noexcept
		{
			return Intersects(segments, rect);
		}

		bool Intersects(const Rect& rect, const Bezier2& curve)
		{
			return Intersects(curve, rect);
		}

		bool Intersects(const Rect& rect, const Bezier3& curve)
		{
			return Intersects(curve, rect);
		}

		bool Intersects(const Rect& rect, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsRectFSuperEllipse(RectF{ rect }, superEllipse);
		}

		bool Intersects(const Rect& rect, const Triangle& triangle) noexcept
		{
			return IntersectsRectFTriangle(RectF{ rect }, triangle);
		}

		bool Intersects(const Rect& rect, const Quad& quad) noexcept
		{
			return IntersectsRectFQuad(RectF{ rect }, quad);
		}

		bool Intersects(const Rect& rect, const RoundRect& roundRect) noexcept
		{
			return IntersectsRectFRoundRect(RectF{ rect }, roundRect);
		}

		bool Intersects(const Rect& rect, const Polygon& polygon) noexcept
		{
			return IntersectsRectFPolygon(RectF{ rect }, polygon);
		}

		bool Intersects(const Rect& rect, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Intersects(rect, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const RectF& rect, const LineString& segments) noexcept
		{
			return Intersects(segments, rect);
		}

		bool Intersects(const RectF& rect, const Bezier2& curve)
		{
			return Intersects(curve, rect);
		}

		bool Intersects(const RectF& rect, const Bezier3& curve)
		{
			return Intersects(curve, rect);
		}

		bool Intersects(const RectF& rect, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsRectFSuperEllipse(rect, superEllipse);
		}

		bool Intersects(const RectF& rect, const Triangle& triangle) noexcept
		{
			return IntersectsRectFTriangle(rect, triangle);
		}

		bool Intersects(const RectF& rect, const Quad& quad) noexcept
		{
			return IntersectsRectFQuad(rect, quad);
		}

		bool Intersects(const RectF& rect, const RoundRect& roundRect) noexcept
		{
			return IntersectsRectFRoundRect(rect, roundRect);
		}

		bool Intersects(const RectF& rect, const Polygon& polygon) noexcept
		{
			return IntersectsRectFPolygon(rect, polygon);
		}

		bool Intersects(const RectF& rect, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Intersects(rect, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Circle& circle, const LineString& segments) noexcept
		{
			return Intersects(segments, circle);
		}

		bool Intersects(const Circle& circle, const Bezier2& curve)
		{
			return Intersects(curve, circle);
		}

		bool Intersects(const Circle& circle, const Bezier3& curve)
		{
			return Intersects(curve, circle);
		}

		bool Intersects(const Circle& circle, const Ellipse& ellipse) noexcept
		{
			return IntersectsCircleEllipse(circle, ellipse);
		}

		bool Intersects(const Circle& circle, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsCircleSuperEllipse(circle, superEllipse);
		}

		bool Intersects(const Circle& circle, const Triangle& triangle) noexcept
		{
			return IntersectsCircleTriangle(circle, triangle);
		}

		bool Intersects(const Circle& circle, const Quad& quad) noexcept
		{
			return IntersectsCircleQuad(circle, quad);
		}

		bool Intersects(const Circle& circle, const RoundRect& roundRect) noexcept
		{
			return IntersectsCircleRoundRect(circle, roundRect);
		}

		bool Intersects(const Circle& circle, const Polygon& polygon) noexcept
		{
			return IntersectsCirclePolygon(circle, polygon);
		}

		bool Intersects(const Circle& circle, const MultiPolygon& multiPolygon) noexcept
		{
			return IntersectsCircleMultiPolygon(circle, multiPolygon);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Ellipse& ellipse, const LineString& segments) noexcept
		{
			return Intersects(segments, ellipse);
		}

		bool Intersects(const Ellipse& ellipse, const Bezier2& curve)
		{
			return Intersects(curve, ellipse);
		}

		bool Intersects(const Ellipse& ellipse, const Bezier3& curve)
		{
			return Intersects(curve, ellipse);
		}

		bool Intersects(const Ellipse& ellipse, const Circle& circle) noexcept
		{
			return Intersects(circle, ellipse);
		}

		bool Intersects(const Ellipse& a, const Ellipse& b) noexcept
		{
			return IntersectsEllipseEllipse(a, b);
		}

		bool Intersects(const Ellipse& ellipse, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsEllipseSuperEllipse(ellipse, superEllipse);
		}

		bool Intersects(const Ellipse& ellipse, const Triangle& triangle) noexcept
		{
			return IntersectsEllipseTriangle(ellipse, triangle);
		}

		bool Intersects(const Ellipse& ellipse, const Quad& quad) noexcept
		{
			return IntersectsEllipseQuad(ellipse, quad);
		}

		bool Intersects(const Ellipse& ellipse, const RoundRect& roundRect) noexcept
		{
			return IntersectsEllipseRoundRect(ellipse, roundRect);
		}

		bool Intersects(const Ellipse& ellipse, const Polygon& polygon) noexcept
		{
			return IntersectsEllipsePolygon(ellipse, polygon);
		}

		bool Intersects(const Ellipse& ellipse, const MultiPolygon& multiPolygon) noexcept
		{
			return IntersectsEllipseMultiPolygon(ellipse, multiPolygon);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const SuperEllipse& superEllipse, const Point& p) noexcept
		{
			return Intersects(p, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Vec2& p) noexcept
		{
			return Intersects(p, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Line& segment) noexcept
		{
			return Intersects(segment, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const LineString& segments) noexcept
		{
			return Intersects(segments, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Bezier2& curve)
		{
			return Intersects(curve, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Bezier3& curve)
		{
			return Intersects(curve, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Rect& rect) noexcept
		{
			return Intersects(rect, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const RectF& rect) noexcept
		{
			return Intersects(rect, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Circle& circle) noexcept
		{
			return Intersects(circle, superEllipse);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Ellipse& ellipse) noexcept
		{
			return Intersects(ellipse, superEllipse);
		}

		bool Intersects(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			return IntersectsSuperEllipseSuperEllipse(a, b);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Triangle& triangle) noexcept
		{
			return IntersectsSuperEllipseTriangle(superEllipse, triangle);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Quad& quad) noexcept
		{
			return IntersectsSuperEllipseQuad(superEllipse, quad);
		}

		bool Intersects(const SuperEllipse& superEllipse, const RoundRect& roundRect) noexcept
		{
			return IntersectsSuperEllipseRoundRect(superEllipse, roundRect);
		}

		bool Intersects(const SuperEllipse& superEllipse, const Polygon& polygon) noexcept
		{
			return IntersectsSuperEllipsePolygon(superEllipse, polygon);
		}

		bool Intersects(const SuperEllipse& superEllipse, const MultiPolygon& multiPolygon) noexcept
		{
			return IntersectsSuperEllipseMultiPolygon(superEllipse, multiPolygon);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Triangle& triangle, const LineString& segments) noexcept
		{
			return Intersects(segments, triangle);
		}

		bool Intersects(const Triangle& triangle, const Bezier2& curve)
		{
			return Intersects(curve, triangle);
		}

		bool Intersects(const Triangle& triangle, const Bezier3& curve)
		{
			return Intersects(curve, triangle);
		}

		bool Intersects(const Triangle& triangle, const Rect& rect) noexcept
		{
			return Intersects(rect, triangle);
		}

		bool Intersects(const Triangle& triangle, const RectF& rect) noexcept
		{
			return Intersects(rect, triangle);
		}

		bool Intersects(const Triangle& triangle, const Circle& circle) noexcept
		{
			return Intersects(circle, triangle);
		}

		bool Intersects(const Triangle& triangle, const Ellipse& ellipse) noexcept
		{
			return Intersects(ellipse, triangle);
		}

		bool Intersects(const Triangle& triangle, const SuperEllipse& superEllipse) noexcept
		{
			return Intersects(superEllipse, triangle);
		}

		bool Intersects(const Triangle& triangle, const RoundRect& roundRect) noexcept
		{
			return Intersects(roundRect, triangle);
		}

		bool Intersects(const Triangle& triangle, const Polygon& polygon) noexcept
		{
			return IntersectsTrianglePolygon(triangle, polygon);
		}

		bool Intersects(const Triangle& triangle, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Geometry2D::Intersects(triangle, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Quad& quad, const LineString& segments) noexcept
		{
			return Intersects(segments, quad);
		}

		bool Intersects(const Quad& quad, const Bezier2& curve)
		{
			return Intersects(curve, quad);
		}

		bool Intersects(const Quad& quad, const Bezier3& curve)
		{
			return Intersects(curve, quad);
		}

		bool Intersects(const Quad& quad, const Rect& rect) noexcept
		{
			return Intersects(rect, quad);
		}

		bool Intersects(const Quad& quad, const RectF& rect) noexcept
		{
			return Intersects(rect, quad);
		}

		bool Intersects(const Quad& quad, const Circle& circle) noexcept
		{
			return Intersects(circle, quad);
		}

		bool Intersects(const Quad& quad, const Ellipse& ellipse) noexcept
		{
			return Intersects(ellipse, quad);
		}

		bool Intersects(const Quad& quad, const SuperEllipse& superEllipse) noexcept
		{
			return Intersects(superEllipse, quad);
		}

		bool Intersects(const Quad& quad, const RoundRect& roundRect) noexcept
		{
			return Intersects(roundRect, quad);
		}

		bool Intersects(const Quad& quad, const Polygon& polygon) noexcept
		{
			return IntersectsQuadPolygon(quad, polygon);
		}

		bool Intersects(const Quad& quad, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (Geometry2D::Intersects(quad, polygon))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const RoundRect& roundRect, const LineString& segments) noexcept
		{
			return Intersects(segments, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Bezier2& curve)
		{
			return Intersects(curve, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Bezier3& curve)
		{
			return Intersects(curve, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Rect& rect) noexcept
		{
			return Intersects(rect, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const RectF& rect) noexcept
		{
			return Intersects(rect, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Circle& circle) noexcept
		{
			return Intersects(circle, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Ellipse& ellipse) noexcept
		{
			return Intersects(ellipse, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const SuperEllipse& superEllipse) noexcept
		{
			return Intersects(superEllipse, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Triangle& triangle) noexcept
		{
			return IntersectsRoundRectTriangle(roundRect, triangle);
		}

		bool Intersects(const RoundRect& roundRect, const Quad& quad) noexcept
		{
			return IntersectsRoundRectQuad(roundRect, quad);
		}

		bool Intersects(const RoundRect& a, const RoundRect& b) noexcept
		{
			return IntersectsRoundRectRoundRect(a, b);
		}

		bool Intersects(const RoundRect& roundRect, const Polygon& polygon) noexcept
		{
			return IntersectsRoundRectPolygon(roundRect, polygon);
		}

		bool Intersects(const RoundRect& roundRect, const MultiPolygon& multiPolygon) noexcept
		{
			return IntersectsRoundRectMultiPolygon(roundRect, multiPolygon);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Polygon& polygon, const Point& p) noexcept
		{
			return Intersects(p, polygon);
		}

		bool Intersects(const Polygon& polygon, const Vec2& p) noexcept
		{
			return Intersects(p, polygon);
		}

		bool Intersects(const Polygon& polygon, const Line& segment) noexcept
		{
			return Intersects(segment, polygon);
		}

		bool Intersects(const Polygon& polygon, const LineString& segments) noexcept
		{
			return Intersects(segments, polygon);
		}

		bool Intersects(const Polygon& polygon, const Bezier2& curve)
		{
			return Intersects(curve, polygon);
		}

		bool Intersects(const Polygon& polygon, const Bezier3& curve)
		{
			return Intersects(curve, polygon);
		}

		bool Intersects(const Polygon& polygon, const Rect& rect) noexcept
		{
			return Intersects(rect, polygon);
		}

		bool Intersects(const Polygon& polygon, const RectF& rect) noexcept
		{
			return Intersects(rect, polygon);
		}

		bool Intersects(const Polygon& polygon, const Circle& circle) noexcept
		{
			return Intersects(circle, polygon);
		}

		bool Intersects(const Polygon& polygon, const Ellipse& ellipse) noexcept
		{
			return Intersects(ellipse, polygon);
		}

		bool Intersects(const Polygon& polygon, const SuperEllipse& superEllipse) noexcept
		{
			return Intersects(superEllipse, polygon);
		}

		bool Intersects(const Polygon& polygon, const Triangle& triangle) noexcept
		{
			return Intersects(triangle, polygon);
		}

		bool Intersects(const Polygon& polygon, const Quad& quad) noexcept
		{
			return Intersects(quad, polygon);
		}

		bool Intersects(const Polygon& polygon, const RoundRect& roundRect) noexcept
		{
			return Intersects(roundRect, polygon);
		}

		bool Intersects(const Polygon& a, const Polygon& b) noexcept
		{
			return IntersectsPolygonPolygon(a, b);
		}

		bool Intersects(const Polygon& polygon, const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& other : multiPolygon)
			{
				if (Geometry2D::Intersects(polygon, other))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Intersects(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const MultiPolygon& multiPolygon, const Point& p) noexcept
		{
			return Intersects(p, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Vec2& p) noexcept
		{
			return Intersects(p, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Line& segment) noexcept
		{
			return Intersects(segment, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const LineString& segments) noexcept
		{
			return Intersects(segments, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Bezier2& curve)
		{
			return Intersects(curve, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Bezier3& curve)
		{
			return Intersects(curve, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Rect& rect) noexcept
		{
			return Intersects(rect, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const RectF& rect) noexcept
		{
			return Intersects(rect, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Circle& circle) noexcept
		{
			return Intersects(circle, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Ellipse& ellipse) noexcept
		{
			return Intersects(ellipse, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const SuperEllipse& superEllipse) noexcept
		{
			return Intersects(superEllipse, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Triangle& triangle) noexcept
		{
			return Intersects(triangle, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Quad& quad) noexcept
		{
			return Intersects(quad, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const RoundRect& roundRect) noexcept
		{
			return Intersects(roundRect, multiPolygon);
		}

		bool Intersects(const MultiPolygon& multiPolygon, const Polygon& polygon) noexcept
		{
			return Intersects(polygon, multiPolygon);
		}

		bool Intersects(const MultiPolygon& a, const MultiPolygon& b) noexcept
		{
			for (const auto& polygon : a)
			{
				if (Geometry2D::Intersects(polygon, b))
				{
					return true;
				}
			}

			return false;
		}
	}
}
