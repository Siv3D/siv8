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
# include <Siv3D/ListUtility.hpp>
# include <Siv3D/PolynomialSolver.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>

namespace s3d
{
	namespace
	{
		inline constexpr double BezierRootTolerance = (64.0 * 2.2204460492503131e-16);
		inline constexpr double BezierPointTolerance = (64.0 * 2.2204460492503131e-16);

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
			const double scale = Max({ Abs(a), Abs(b), Abs(c), 1.0 });
			const double coefficientTolerance = (BezierRootTolerance * scale);

			auto CheckRoot = [&](double t)
			{
				if (InRange(t, -BezierRootTolerance, (1.0 + BezierRootTolerance)))
				{
					t = Clamp(t, 0.0, 1.0);
					return callback(t);
				}

				return false;
			};

			if (Abs(a) <= coefficientTolerance)
			{
				if (Abs(b) <= coefficientTolerance)
				{
					return false;
				}

				return CheckRoot(-c / b);
			}

			const double discriminantScale = Max({ b * b, Abs(4.0 * a * c), 1.0 });
			const double discriminantTolerance = (BezierRootTolerance * discriminantScale);
			double discriminant = std::fma(b, b, -4.0 * a * c);

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
				return CheckRoot(-b / (2.0 * a));
			}

			const double q = (-0.5 * (b + ((b < 0.0) ? -s : s)));

			if (q == 0.0)
			{
				return CheckRoot(-b / (2.0 * a));
			}

			if (CheckRoot(q / a))
			{
				return true;
			}

			return CheckRoot(c / q);
		}

		template <class Fty>
		[[nodiscard]]
		bool CheckCubicRootsInUnitInterval(const double a, const double b, const double c, const double d, Fty&& callback)
		{
			const double scale = Max({ Abs(a), Abs(b), Abs(c), Abs(d), 1.0 });
			const double coefficientTolerance = (BezierRootTolerance * scale);

			if (Abs(a) <= coefficientTolerance)
			{
				return CheckQuadraticRootsInUnitInterval(b, c, d, callback);
			}

			const auto roots = Math::SolveCubicEquation(a, b, c, d);

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
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Intersects(Point, _)
		//
		////////////////////////////////////////////////////////////////

		bool Intersects(const Point& a, const LineString& b) noexcept
		{
			return Intersects(Vec2{ a }, b);
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

		bool Intersects(const Bezier2& a, const Bezier2& b)
		{
			return IntersectsBezier2Bezier2Approximate(a, b);
		}

		bool Intersects(const Bezier2& a, const Bezier3& b)
		{
			return IntersectsBezier2Bezier3Approximate(a, b);
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

		bool Intersects(const Quad& quad, const Rect& rect) noexcept
		{
			return Intersects(rect, quad);
		}

		bool Intersects(const Quad& quad, const RectF& rect) noexcept
		{
			return Intersects(rect, quad);
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
	}
}
