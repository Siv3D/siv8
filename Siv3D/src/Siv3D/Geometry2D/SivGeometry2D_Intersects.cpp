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
# include "BezierGeometry.hpp"
# include "PolygonGeometry.hpp"
# include "EllipseGeometry.hpp"
# include "SuperEllipseGeometry.hpp"

namespace s3d
{
	namespace
	{
		inline constexpr double DoubleEpsilon = 2.2204460492503131e-16;
		using detail::CheckQuadraticRootsInUnitInterval;
		using detail::CheckCubicRootsInUnitInterval;
		inline constexpr double BezierPointTolerance = (64.0 * DoubleEpsilon);

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
			// The root already places p on the supporting line. A constant axis
			// must not reject it again because of polynomial evaluation roundoff.
			const Vec2 direction = (segment.end - segment.start);
			return ((Abs(direction.y) <= Abs(direction.x))
				? NearlyBetweenBezierCoordinate(segment.start.x, p.x, segment.end.x)
				: NearlyBetweenBezierCoordinate(segment.start.y, p.y, segment.end.y));
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

		template <class Bezier>
		[[nodiscard]]
		bool CollinearBezierIntersectsLine(const Line& segment, const Bezier& curve) noexcept
		{
			const Vec2 direction = (segment.end - segment.start);
			const bool useX = (Abs(direction.y) <= Abs(direction.x));
			const Line extent = detail::BezierSegmentExtent(curve, useX);
			const double a = (useX ? segment.start.x : segment.start.y);
			const double b = (useX ? segment.end.x : segment.end.y);
			const double lower = (useX ? extent.start.x : extent.start.y);
			const double upper = (useX ? extent.end.x : extent.end.y);
			return ((lower <= Max(a, b)) && (Min(a, b) <= upper));
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
				return CollinearBezierIntersectsLine(segment, curve);
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
				return CollinearBezierIntersectsLine(segment, curve);
			}

			return CheckCubicRootsInUnitInterval(a, b, c, e, [&](const double t)
			{
				return BezierRootPointIsOnSegmentRange(curve.pointAt(t), segment);
			});
		}

		[[nodiscard]]
		bool IntersectsPointSuperEllipseArea(const Vec2& p, const SuperEllipse& superEllipse) noexcept
		{
			const double dx = Abs((p.x - superEllipse.center.x) / superEllipse.axes.x);
			const double dy = Abs((p.y - superEllipse.center.y) / superEllipse.axes.y);

			if ((1.0 < dx) || (1.0 < dy))
			{
				return false;
			}

			return ((std::pow(dx, superEllipse.n) + std::pow(dy, superEllipse.n)) <= 1.0);
		}

		[[nodiscard]]
		bool IntersectsLineSuperEllipseArea(const Line& segment, const SuperEllipse& superEllipse) noexcept
		{
			return detail::TestLineSuperEllipseArea<true>(segment, superEllipse);
		}

		[[nodiscard]]
		bool IntersectsLineSuperEllipse(const Line& segment, const SuperEllipse& superEllipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(segment, detail::GetGeometry2DDegenerateSegment(superEllipse, kind));
			}

			if (superEllipse.n == 2.0)
			{
				return detail::IntersectsLineEllipseArea(segment, Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y });
			}

			return IntersectsLineSuperEllipseArea(segment, superEllipse);
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

		template <class PointIntersector, class SegmentIntersector>
		[[nodiscard]]
		bool IntersectsLineStringWith(const LineString& segments, PointIntersector&& pointIntersector, SegmentIntersector&& segmentIntersector) noexcept
		{
			const size_t n = segments.size();

			if (n == 0)
			{
				return false;
			}

			const Vec2* ps = segments.data();

			if (n == 1)
			{
				return pointIntersector(ps[0]);
			}

			for (size_t i = 0; i < (n - 1); ++i)
			{
				if (segmentIntersector(Line{ ps[i], ps[i + 1] }))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsLineStringRectF(const LineString& segments, const RectF& rect) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(rect) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return IntersectsLineStringWith(segments,
				[&](const Vec2& p) noexcept { return detail::IntersectsPointRectFNonEmpty(p, rect); },
				[&](const Line& segment) noexcept { return detail::IntersectsLineRectFNonEmpty(segment, rect); });
		}

		[[nodiscard]]
		bool IntersectsLineStringCircle(const LineString& segments, const Circle& circle) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return IntersectsLineStringWith(segments,
				[&](const Vec2& p) noexcept { return detail::IntersectsPointCircleArea(p, circle); },
				[&](const Line& segment) noexcept { return detail::IntersectsLineCircleArea(segment, circle); });
		}

		[[nodiscard]]
		bool IntersectsLineStringEllipse(const LineString& segments, const Ellipse& ellipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(segments, detail::GetGeometry2DDegenerateSegment(ellipse, kind));
			}

			return IntersectsLineStringWith(segments,
				[&](const Vec2& p) noexcept { return detail::IntersectsPointEllipseArea(p, ellipse); },
				[&](const Line& segment) noexcept { return detail::IntersectsLineEllipseArea(segment, ellipse); });
		}

		[[nodiscard]]
		bool IntersectsLineStringSuperEllipse(const LineString& segments, const SuperEllipse& superEllipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(segments, detail::GetGeometry2DDegenerateSegment(superEllipse, kind));
			}

			if (superEllipse.n == 2.0)
			{
				return IntersectsLineStringEllipse(segments, Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y });
			}

			return IntersectsLineStringWith(segments,
				[&](const Vec2& p) noexcept { return IntersectsPointSuperEllipseArea(p, superEllipse); },
				[&](const Line& segment) noexcept { return IntersectsLineSuperEllipseArea(segment, superEllipse); });
		}

		[[nodiscard]]
		bool IntersectsLineStringRoundRect(const LineString& segments, const RoundRect& roundRect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(segments, detail::GetGeometry2DDegenerateSegment(roundRect, kind));
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);

			return IntersectsLineStringWith(segments,
				[&](const Vec2& p) noexcept { return detail::IntersectsPointRoundRectArea(p, roundRect, effectiveRadius); },
				[&](const Line& segment) noexcept { return detail::IntersectsLineRoundRectArea(segment, roundRect, effectiveRadius); });
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
		bool IntersectsPointPolygonNonEmpty(const Vec2& p, const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not detail::IntersectsPointRectFNonEmpty(p, polygonBounds))
			{
				return false;
			}

			return detail::PolygonContainsPoint(detail::GetPolygonRings(polygon), p);
		}

		[[nodiscard]]
		bool IntersectsLinePolygonNonEmpty(const Line& segment, const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not detail::IntersectsLineRectFNonEmpty(segment, polygonBounds))
			{
				return false;
			}

			return IntersectsPointPolygonNonEmpty(segment.start, polygon, polygonBounds)
				|| detail::AnyPolygonEdge(polygon, [&](const Line& edge)
					{
						return Geometry2D::Intersects(segment, edge);
					});
		}

		[[nodiscard]]
		bool IntersectsLineStringPolygon(const LineString& segments, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();

			return IntersectsLineStringWith(segments,
				[&](const Vec2& p) noexcept { return IntersectsPointPolygonNonEmpty(p, polygon, polygonBounds); },
				[&](const Line& segment) noexcept { return IntersectsLinePolygonNonEmpty(segment, polygon, polygonBounds); });
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
		bool IntersectsBezier2RectF(const Bezier2& curve, const RectF& rect)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, kind), curve);
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

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
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
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

			return (detail::ClosestPointOnBezier(curve, circle.center).distanceSq <= (circle.r * circle.r));
		}

		[[nodiscard]]
		bool IntersectsBezier2Ellipse(const Bezier2& curve, const Ellipse& ellipse)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, kind), curve);
			}

			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

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

			return (detail::ClosestPointOnBezier(local, Vec2{ 0, 0 }).distanceSq <= 1.0);
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
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, kind), curve);
			}

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

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

		template <class Bezier>
		[[nodiscard]]
		bool IntersectsBezierRoundRect(const Bezier& curve, const RoundRect& roundRect)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(roundRect, kind), curve);
			}

			const RectF& rect = roundRect.rect;
			const double er = detail::GetGeometry2DEffectiveRadius(roundRect);

			if (er == 0.0)
			{
				return Geometry2D::Intersects(rect, curve);
			}

			const RectF curveBounds = curve.computeBoundingRect();
			if (not BoundsIntersectClosed(curveBounds, rect))
			{
				return false;
			}

			if (Geometry2D::Intersects(curve.p0, roundRect))
			{
				return true;
			}

			// Two central rectangles and the corner disks cover the rounded rectangle.
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, er);
			if ((0.0 < core.w) && Geometry2D::Intersects(curve, RectF{ core.x, rect.y, core.w, rect.h }))
			{
				return true;
			}
			if ((0.0 < core.h) && Geometry2D::Intersects(curve, RectF{ rect.x, core.y, rect.w, core.h }))
			{
				return true;
			}

			// Circle and capsule cores have fewer distinct corner disks.
			const int32 columns = ((core.w == 0.0) ? 1 : 2);
			const int32 rows = ((core.h == 0.0) ? 1 : 2);
			for (int32 y = 0; y < rows; ++y)
			{
				for (int32 x = 0; x < columns; ++x)
				{
					const Vec2 center{ (core.x + x * core.w), (core.y + y * core.h) };
					const RectF circleBounds{ (center.x - er), (center.y - er), (2.0 * er), (2.0 * er) };
					if (BoundsIntersectClosed(curveBounds, circleBounds)
						&& (detail::ClosestPointOnBezier(curve, center).distanceSq <= (er * er)))
					{
						return true;
					}
				}
			}
			return false;
		}

		[[nodiscard]]
		bool IntersectsBezier2PolygonNonEmpty(
			const Bezier2& curve, const RectF& curveBounds,
			const Polygon& polygon, const RectF& polygonBounds)
		{
			if (not BoundsIntersectClosed(curveBounds, polygonBounds))
			{
				return false;
			}

			return IntersectsPointPolygonNonEmpty(curve.p0, polygon, polygonBounds)
				|| detail::AnyPolygonEdge(polygon, [&](const Line& edge)
					{
						return IntersectsLineBezier2(edge, curve);
					});
		}

		[[nodiscard]]
		bool IntersectsBezier2Polygon(const Bezier2& curve, const Polygon& polygon)
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF curveBounds = curve.computeBoundingRect();
			const RectF polygonBounds = polygon.boundingRect();

			return IntersectsBezier2PolygonNonEmpty(curve, curveBounds, polygon, polygonBounds);
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
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, kind), curve);
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

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
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
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

			return (detail::ClosestPointOnBezier(curve, circle.center).distanceSq <= (circle.r * circle.r));
		}

		[[nodiscard]]
		bool IntersectsBezier3Ellipse(const Bezier3& curve, const Ellipse& ellipse)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, kind), curve);
			}

			const double ax = ellipse.axes.x;
			const double by = ellipse.axes.y;

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

			return (detail::ClosestPointOnBezier(local, Vec2{ 0, 0 }).distanceSq <= 1.0);
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
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, kind), curve);
			}

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

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
		bool IntersectsBezier3PolygonNonEmpty(
			const Bezier3& curve, const RectF& curveBounds,
			const Polygon& polygon, const RectF& polygonBounds)
		{
			if (not BoundsIntersectClosed(curveBounds, polygonBounds))
			{
				return false;
			}

			return IntersectsPointPolygonNonEmpty(curve.p0, polygon, polygonBounds)
				|| detail::AnyPolygonEdge(polygon, [&](const Line& edge)
					{
						return IntersectsLineBezier3(edge, curve);
					});
		}

		[[nodiscard]]
		bool IntersectsBezier3Polygon(const Bezier3& curve, const Polygon& polygon)
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF curveBounds = curve.computeBoundingRect();
			const RectF polygonBounds = polygon.boundingRect();

			return IntersectsBezier3PolygonNonEmpty(curve, curveBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool IntersectsRectFTriangleArea(const RectF& rect, const Triangle& triangle) noexcept
		{
			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

			if (not BoundsIntersectClosed(rect, triangle.boundingRect()))
			{
				return false;
			}

			if (detail::IntersectsPointRectFNonEmpty(triangle.p0, rect)
				|| detail::IntersectsPointRectFNonEmpty(triangle.p1, rect)
				|| detail::IntersectsPointRectFNonEmpty(triangle.p2, rect))
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
		bool IntersectsRectFTriangle(const RectF& rect, const Triangle& triangle) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, kind), triangle);
			}

			return IntersectsRectFTriangleArea(rect, triangle);
		}

		[[nodiscard]]
		bool IntersectsRectFQuad(const RectF& rect, const Quad& quad) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, kind), quad);
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

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
			const auto rectKind = detail::ClassifyGeometry2DSizedShape(rect);
			const auto superEllipseKind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if ((rectKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (superEllipseKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(rectKind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, rectKind), superEllipse);
			}

			if (detail::IsGeometry2DSegment(superEllipseKind))
			{
				return Geometry2D::Intersects(rect, detail::GetGeometry2DDegenerateSegment(superEllipse, superEllipseKind));
			}

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if (n == 2.0)
			{
				return Geometry2D::Intersects(rect, Ellipse{ superEllipse.center, ax, by });
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);

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
			const auto rectKind = detail::ClassifyGeometry2DSizedShape(rect);
			const auto roundRectKind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if ((rectKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (roundRectKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			const RectF& rrRect = roundRect.rect;

			if (not BoundsIntersectClosed(rect, rrRect))
			{
				return false;
			}

			const double er = (roundRectKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DEffectiveRadius(roundRect)
				: 0.0;
			const RectF core = (roundRectKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DRoundRectCore(roundRect, er)
				: rrRect;

			// A positive-size RoundRect is the Minkowski sum of its core
			// rectangle and a closed disk of radius er. This also covers
			// er == 0 and one-dimensional RectF / RoundRect degeneracies.
			return (DistanceClosedRects(rect, core) <= er);
		}

		[[nodiscard]]
		bool IntersectsRectFPolygonArea(const RectF& rect, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;

			const RectF polygonBounds = polygon.boundingRect();

			if (not BoundsIntersectClosed(rect, polygonBounds))
			{
				return false;
			}

			return IntersectsPointPolygonNonEmpty(Vec2{ left, top }, polygon, polygonBounds)
				|| detail::AnyPolygonEdge(polygon, [&](const Line& edge)
					{
						return Geometry2D::Intersects(edge, rect);
					});
		}

		[[nodiscard]]
		bool IntersectsRectFPolygon(const RectF& rect, const Polygon& polygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, kind), polygon);
			}

			return IntersectsRectFPolygonArea(rect, polygon);
		}

		[[nodiscard]]
		bool IntersectsRectFMultiPolygon(const RectF& rect, const MultiPolygon& multiPolygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(rect, kind), multiPolygon);
			}

			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsRectFPolygonArea(rect, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsCircleEllipse(const Circle& circle, const Ellipse& ellipse) noexcept
		{
			const auto circleKind = detail::ClassifyGeometry2DSizedShape(circle);
			const auto ellipseKind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if ((circleKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (ellipseKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(ellipseKind))
			{
				return Geometry2D::Intersects(circle, detail::GetGeometry2DDegenerateSegment(ellipse, ellipseKind));
			}

			const double scale = Max({ circle.r, ellipse.a, ellipse.b });
			const double tolerance = (detail::EllipseContactTolerance * scale);
			if (((circle.r + ellipse.a + tolerance) < Abs(circle.x - ellipse.x))
				|| ((circle.r + ellipse.b + tolerance) < Abs(circle.y - ellipse.y)))
			{
				return false;
			}

			return detail::EllipseDistanceWithinRadius<true>(
				detail::DistancePointEllipse(circle.center, ellipse), circle.r, scale);
		}

		[[nodiscard]]
		bool IntersectsCircleSuperEllipse(const Circle& circle, const SuperEllipse& superEllipse) noexcept
		{
			const auto circleKind = detail::ClassifyGeometry2DSizedShape(circle);
			const auto superEllipseKind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if ((circleKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (superEllipseKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(superEllipseKind))
			{
				return Geometry2D::Intersects(circle, detail::GetGeometry2DDegenerateSegment(superEllipse, superEllipseKind));
			}

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if (n == 2.0)
			{
				return IntersectsCircleEllipse(circle, Ellipse{ superEllipse.center, ax, by });
			}

			return detail::TestSuperEllipseAreas<true>(SuperEllipse{ circle.center, circle.r, circle.r, 2.0 }, superEllipse);
		}

		[[nodiscard]]
		bool IntersectsCircleRoundRect(const Circle& circle, const RoundRect& roundRect) noexcept
		{
			const auto circleKind = detail::ClassifyGeometry2DSizedShape(circle);
			const auto roundRectKind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if ((circleKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (roundRectKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			const RectF& rect = roundRect.rect;

			if (not BoundsIntersectClosed(circle.boundingRect(), rect))
			{
				return false;
			}

			const double er = (roundRectKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DEffectiveRadius(roundRect)
				: 0.0;
			const RectF core = (roundRectKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DRoundRectCore(roundRect, er)
				: rect;

			// (core + disk(er)) intersects disk(circle.r) exactly when the
			// circle center is no farther than the sum of the two radii.
			return (DistancePointClosedRect(circle.center, core) <= (circle.r + er));
		}

		[[nodiscard]]
		bool IntersectsEllipseTriangleArea(const Ellipse& ellipse, const RectF& ellipseBounds, const Triangle& triangle) noexcept
		{
			if (not BoundsIntersectClosed(ellipseBounds, triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(ellipse.center, triangle)
				|| detail::IntersectsPointEllipseArea(triangle.p0, ellipse)
				|| detail::IntersectsPointEllipseArea(triangle.p1, ellipse)
				|| detail::IntersectsPointEllipseArea(triangle.p2, ellipse))
			{
				return true;
			}

			return (detail::IntersectsLineEllipseArea(Line{ triangle.p0, triangle.p1 }, ellipse)
				|| detail::IntersectsLineEllipseArea(Line{ triangle.p1, triangle.p2 }, ellipse)
				|| detail::IntersectsLineEllipseArea(Line{ triangle.p2, triangle.p0 }, ellipse));
		}

		[[nodiscard]]
		bool IntersectsEllipseTriangle(const Ellipse& ellipse, const Triangle& triangle) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, kind), triangle);
			}

			return IntersectsEllipseTriangleArea(ellipse, ellipse.boundingRect(), triangle);
		}

		[[nodiscard]]
		bool IntersectsEllipseQuad(const Ellipse& ellipse, const Quad& quad) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, kind), quad);
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
		bool IntersectsEllipsePolygonArea(const Ellipse& ellipse, const RectF& ellipseBounds, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();

			if (not BoundsIntersectClosed(ellipseBounds, polygonBounds))
			{
				return false;
			}

			if (IntersectsPointPolygonNonEmpty(ellipse.center, polygon, polygonBounds))
			{
				return true;
			}

			return detail::AnyPolygonEdge(polygon, [&](const Line& edge)
				{
					return Geometry2D::Intersects(edge, ellipse);
				});
		}

		[[nodiscard]]
		bool IntersectsEllipsePolygon(const Ellipse& ellipse, const Polygon& polygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, kind), polygon);
			}

			return IntersectsEllipsePolygonArea(ellipse, ellipse.boundingRect(), polygon);
		}

		[[nodiscard]]
		bool IntersectsEllipseMultiPolygon(const Ellipse& ellipse, const MultiPolygon& multiPolygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, kind), multiPolygon);
			}

			const RectF ellipseBounds = ellipse.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsEllipsePolygonArea(ellipse, ellipseBounds, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseTriangleArea(const SuperEllipse& superEllipse, const RectF& superEllipseBounds, const Triangle& triangle) noexcept
		{
			if (not BoundsIntersectClosed(superEllipseBounds, triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(superEllipse.center, triangle)
				|| IntersectsPointSuperEllipseArea(triangle.p0, superEllipse)
				|| IntersectsPointSuperEllipseArea(triangle.p1, superEllipse)
				|| IntersectsPointSuperEllipseArea(triangle.p2, superEllipse))
			{
				return true;
			}

			return (IntersectsLineSuperEllipseArea(Line{ triangle.p0, triangle.p1 }, superEllipse)
				|| IntersectsLineSuperEllipseArea(Line{ triangle.p1, triangle.p2 }, superEllipse)
				|| IntersectsLineSuperEllipseArea(Line{ triangle.p2, triangle.p0 }, superEllipse));
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseTriangle(const SuperEllipse& superEllipse, const Triangle& triangle) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, kind), triangle);
			}

			if (superEllipse.n == 2.0)
			{
				return IntersectsEllipseTriangleArea(
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y },
					superEllipse.boundingRect(), triangle);
			}

			return IntersectsSuperEllipseTriangleArea(superEllipse, detail::SuperEllipseLineTestBounds(superEllipse), triangle);
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseQuad(const SuperEllipse& superEllipse, const Quad& quad) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, kind), quad);
			}

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const double n = superEllipse.n;

			if (n == 2.0)
			{
				return IntersectsEllipseQuad(Ellipse{ superEllipse.center, ax, by }, quad);
			}

			if (not BoundsIntersectClosed(detail::SuperEllipseLineTestBounds(superEllipse), quad.boundingRect()))
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
		bool IntersectsSuperEllipsePolygonArea(const SuperEllipse& superEllipse, const RectF& superEllipseBounds, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();

			if (not BoundsIntersectClosed(superEllipseBounds, polygonBounds))
			{
				return false;
			}

			if (IntersectsPointPolygonNonEmpty(superEllipse.center, polygon, polygonBounds))
			{
				return true;
			}

			return detail::AnyPolygonEdge(polygon, [&](const Line& edge)
				{
					return Geometry2D::Intersects(edge, superEllipse);
				});
		}

		[[nodiscard]]
		bool IntersectsSuperEllipsePolygon(const SuperEllipse& superEllipse, const Polygon& polygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, kind), polygon);
			}

			if (superEllipse.n == 2.0)
			{
				return IntersectsEllipsePolygonArea(
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y },
					superEllipse.boundingRect(), polygon);
			}

			return IntersectsSuperEllipsePolygonArea(superEllipse, detail::SuperEllipseLineTestBounds(superEllipse), polygon);
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseMultiPolygon(const SuperEllipse& superEllipse, const MultiPolygon& multiPolygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, kind), multiPolygon);
			}

			if (superEllipse.n == 2.0)
			{
				const Ellipse ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y };
				const RectF superEllipseBounds = superEllipse.boundingRect();

				for (const auto& polygon : multiPolygon)
				{
					if (IntersectsEllipsePolygonArea(ellipse, superEllipseBounds, polygon))
					{
						return true;
					}
				}

				return false;
			}

			const RectF superEllipseBounds = detail::SuperEllipseLineTestBounds(superEllipse);
			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsSuperEllipsePolygonArea(superEllipse, superEllipseBounds, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsTrianglePolygonNonEmpty(
			const Triangle& triangle, const RectF& triangleBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsIntersectClosed(triangleBounds, polygonBounds))
			{
				return false;
			}

			return IntersectsPointPolygonNonEmpty(triangle.p0, polygon, polygonBounds)
				|| detail::AnyPolygonEdge(polygon, [&](const Line& edge)
					{
						return Geometry2D::Intersects(edge, triangle);
					});
		}

		[[nodiscard]]
		bool IntersectsTrianglePolygon(const Triangle& triangle, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF triangleBounds = triangle.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();

			return IntersectsTrianglePolygonNonEmpty(triangle, triangleBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool IntersectsQuadPolygonNonEmpty(
			const Quad& quad, const RectF& quadBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsIntersectClosed(quadBounds, polygonBounds))
			{
				return false;
			}

			return IntersectsPointPolygonNonEmpty(quad.p0, polygon, polygonBounds)
				|| detail::AnyPolygonEdge(polygon, [&](const Line& edge)
					{
						return Geometry2D::Intersects(edge, quad);
					});
		}

		[[nodiscard]]
		bool IntersectsQuadPolygon(const Quad& quad, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF quadBounds = quad.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();

			return IntersectsQuadPolygonNonEmpty(quad, quadBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool IntersectsCircleTriangleArea(const Circle& circle, const RectF& circleBounds, const Triangle& triangle) noexcept
		{
			if (not BoundsIntersectClosed(circleBounds, triangle.boundingRect()))
			{
				return false;
			}

			if (Geometry2D::Intersects(circle.center, triangle)
				|| detail::IntersectsPointCircleArea(triangle.p0, circle)
				|| detail::IntersectsPointCircleArea(triangle.p1, circle)
				|| detail::IntersectsPointCircleArea(triangle.p2, circle))
			{
				return true;
			}

			return (detail::IntersectsLineCircleArea(Line{ triangle.p0, triangle.p1 }, circle)
				|| detail::IntersectsLineCircleArea(Line{ triangle.p1, triangle.p2 }, circle)
				|| detail::IntersectsLineCircleArea(Line{ triangle.p2, triangle.p0 }, circle));
		}

		[[nodiscard]]
		bool IntersectsCircleTriangle(const Circle& circle, const Triangle& triangle) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return IntersectsCircleTriangleArea(circle, circle.boundingRect(), triangle);
		}

		[[nodiscard]]
		bool IntersectsCircleQuad(const Circle& circle, const Quad& quad) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
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
		bool IntersectsCirclePolygonArea(const Circle& circle, const RectF& circleBounds, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();

			if (not BoundsIntersectClosed(circleBounds, polygonBounds))
			{
				return false;
			}

			if (IntersectsPointPolygonNonEmpty(circle.center, polygon, polygonBounds))
			{
				return true;
			}

			return detail::AnyPolygonEdge(polygon, [&](const Line& edge)
				{
					return Geometry2D::Intersects(edge, circle);
				});
		}

		[[nodiscard]]
		bool IntersectsCirclePolygon(const Circle& circle, const Polygon& polygon) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			return IntersectsCirclePolygonArea(circle, circle.boundingRect(), polygon);
		}

		[[nodiscard]]
		bool IntersectsCircleMultiPolygon(const Circle& circle, const MultiPolygon& multiPolygon) noexcept
		{
			if (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			const RectF circleBounds = circle.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsCirclePolygonArea(circle, circleBounds, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsEllipseEllipse(const Ellipse& a, const Ellipse& b) noexcept
		{
			const auto aKind = detail::ClassifyGeometry2DSizedShape(a);
			const auto bKind = detail::ClassifyGeometry2DSizedShape(b);

			if ((aKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (bKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(aKind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(a, aKind), b);
			}

			if (detail::IsGeometry2DSegment(bKind))
			{
				return Geometry2D::Intersects(a, detail::GetGeometry2DDegenerateSegment(b, bKind));
			}

			return detail::TestEllipseEllipseArea<true>(a, b);
		}

		[[nodiscard]]
		bool IntersectsEllipseSuperEllipse(const Ellipse& ellipse, const SuperEllipse& superEllipse) noexcept
		{
			const auto ellipseKind = detail::ClassifyGeometry2DSizedShape(ellipse);
			const auto superEllipseKind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if ((ellipseKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (superEllipseKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(ellipseKind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, ellipseKind), superEllipse);
			}

			if (detail::IsGeometry2DSegment(superEllipseKind))
			{
				return Geometry2D::Intersects(ellipse, detail::GetGeometry2DDegenerateSegment(superEllipse, superEllipseKind));
			}

			const double sx = superEllipse.axes.x;
			const double sy = superEllipse.axes.y;
			const double n = superEllipse.n;

			if (n == 2.0)
			{
				return IntersectsEllipseEllipse(ellipse, Ellipse{ superEllipse.center, sx, sy });
			}

			return detail::TestSuperEllipseAreas<true>(SuperEllipse{ ellipse, 2.0 }, superEllipse);
		}

		[[nodiscard]]
		bool IntersectsEllipseRoundRect(const Ellipse& ellipse, const RoundRect& roundRect) noexcept
		{
			const auto ellipseKind = detail::ClassifyGeometry2DSizedShape(ellipse);
			const auto roundRectKind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if ((ellipseKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (roundRectKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(ellipseKind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(ellipse, ellipseKind), roundRect);
			}

			if (detail::IsGeometry2DSegment(roundRectKind))
			{
				return Geometry2D::Intersects(ellipse, detail::GetGeometry2DDegenerateSegment(roundRect, roundRectKind));
			}

			const double er = detail::GetGeometry2DEffectiveRadius(roundRect);

			if (er == 0.0)
			{
				return Geometry2D::Intersects(ellipse, roundRect.rect);
			}

			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, er);
			return detail::TestEllipseRoundRectArea<true>(ellipse, core, er);
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseSuperEllipse(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			const auto aKind = detail::ClassifyGeometry2DSizedShape(a);
			const auto bKind = detail::ClassifyGeometry2DSizedShape(b);

			if ((aKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (bKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(aKind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(a, aKind), b);
			}

			if (detail::IsGeometry2DSegment(bKind))
			{
				return Geometry2D::Intersects(a, detail::GetGeometry2DDegenerateSegment(b, bKind));
			}

			const double ax = a.axes.x;
			const double ay = a.axes.y;
			const double bx = b.axes.x;
			const double by = b.axes.y;

			if (a.n == 2.0)
			{
				return IntersectsEllipseSuperEllipse(Ellipse{ a.center, ax, ay }, b);
			}

			if (b.n == 2.0)
			{
				return IntersectsEllipseSuperEllipse(Ellipse{ b.center, bx, by }, a);
			}

			return detail::TestSuperEllipseAreas<true>(a, b);
		}

		[[nodiscard]]
		bool IntersectsSuperEllipseRoundRect(const SuperEllipse& superEllipse, const RoundRect& roundRect) noexcept
		{
			const auto superEllipseKind = detail::ClassifyGeometry2DSizedShape(superEllipse);
			const auto roundRectKind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if ((superEllipseKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (roundRectKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(superEllipseKind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(superEllipse, superEllipseKind), roundRect);
			}

			if (detail::IsGeometry2DSegment(roundRectKind))
			{
				return Geometry2D::Intersects(superEllipse, detail::GetGeometry2DDegenerateSegment(roundRect, roundRectKind));
			}

			const double ax = superEllipse.axes.x;
			const double by = superEllipse.axes.y;
			const RectF& rect = roundRect.rect;

			if (superEllipse.n == 2.0)
			{
				return IntersectsEllipseRoundRect(Ellipse{ superEllipse.center, ax, by }, roundRect);
			}

			const double er = detail::GetGeometry2DEffectiveRadius(roundRect);

			if (er == 0.0)
			{
				return IntersectsRectFSuperEllipse(rect, superEllipse);
			}

			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, er);
			return detail::TestSuperEllipseRoundRectArea<true>(superEllipse, core, er);
		}

		[[nodiscard]]
		bool IntersectsTriangleRoundRectArea(const Triangle& triangle, const RoundRect& roundRect, const double effectiveRadius) noexcept
		{
			const RectF& rect = roundRect.rect;

			if (effectiveRadius == 0.0)
			{
				return IntersectsRectFTriangleArea(rect, triangle);
			}

			if (not BoundsIntersectClosed(rect, triangle.boundingRect()))
			{
				return false;
			}

			const Vec2 center{ (rect.pos.x + (rect.size.x * 0.5)), (rect.pos.y + (rect.size.y * 0.5)) };

			if (Geometry2D::Intersects(center, triangle)
				|| detail::IntersectsPointRoundRectArea(triangle.p0, roundRect, effectiveRadius)
				|| detail::IntersectsPointRoundRectArea(triangle.p1, roundRect, effectiveRadius)
				|| detail::IntersectsPointRoundRectArea(triangle.p2, roundRect, effectiveRadius))
			{
				return true;
			}

			return (detail::IntersectsLineRoundRectArea(Line{ triangle.p0, triangle.p1 }, roundRect, effectiveRadius)
				|| detail::IntersectsLineRoundRectArea(Line{ triangle.p1, triangle.p2 }, roundRect, effectiveRadius)
				|| detail::IntersectsLineRoundRectArea(Line{ triangle.p2, triangle.p0 }, roundRect, effectiveRadius));
		}

		[[nodiscard]]
		bool IntersectsTriangleRoundRect(const Triangle& triangle, const RoundRect& roundRect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(roundRect, kind), triangle);
			}

			return IntersectsTriangleRoundRectArea(triangle, roundRect, detail::GetGeometry2DEffectiveRadius(roundRect));
		}

		[[nodiscard]]
		bool IntersectsQuadRoundRect(const Quad& quad, const RoundRect& roundRect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(roundRect, kind), quad);
			}

			const RectF& rect = roundRect.rect;
			const double er = detail::GetGeometry2DEffectiveRadius(roundRect);

			if (er == 0.0)
			{
				return IntersectsRectFQuad(rect, quad);
			}

			if (not BoundsIntersectClosed(rect, quad.boundingRect()))
			{
				return false;
			}

			const Vec2 center{ (rect.pos.x + (rect.size.x * 0.5)), (rect.pos.y + (rect.size.y * 0.5)) };

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
			const auto aKind = detail::ClassifyGeometry2DSizedShape(a);
			const auto bKind = detail::ClassifyGeometry2DSizedShape(b);

			if ((aKind == detail::Geometry2DSizedShapeKind::Empty)
				|| (bKind == detail::Geometry2DSizedShapeKind::Empty))
			{
				return false;
			}

			const RectF& ar = a.rect;
			const RectF& br = b.rect;

			if (not BoundsIntersectClosed(ar, br))
			{
				return false;
			}

			const double aer = (aKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DEffectiveRadius(a)
				: 0.0;
			const double ber = (bKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DEffectiveRadius(b)
				: 0.0;
			const RectF aCore = (aKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DRoundRectCore(a, aer)
				: ar;
			const RectF bCore = (bKind == detail::Geometry2DSizedShapeKind::Area)
				? detail::GetGeometry2DRoundRectCore(b, ber)
				: br;

			// Each RoundRect is core + disk(radius). The two sets intersect
			// exactly when the distance between the closed core rectangles is
			// at most the sum of the effective radii.
			return (DistanceClosedRects(aCore, bCore) <= (aer + ber));
		}

		[[nodiscard]]
		bool IntersectsRoundRectPolygonArea(const RoundRect& roundRect, const double effectiveRadius, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF& rect = roundRect.rect;

			if (effectiveRadius == 0.0)
			{
				return IntersectsRectFPolygonArea(rect, polygon);
			}

			const RectF polygonBounds = polygon.boundingRect();

			if (not BoundsIntersectClosed(rect, polygonBounds))
			{
				return false;
			}

			const Vec2 center{ (rect.pos.x + (rect.size.x * 0.5)), (rect.pos.y + (rect.size.y * 0.5)) };

			if (IntersectsPointPolygonNonEmpty(center, polygon, polygonBounds))
			{
				return true;
			}

			return detail::AnyPolygonEdge(polygon, [&](const Line& edge)
				{
					return detail::IntersectsLineRoundRectArea(edge, roundRect, effectiveRadius);
				});
		}

		[[nodiscard]]
		bool IntersectsRoundRectPolygon(const RoundRect& roundRect, const Polygon& polygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(roundRect, kind), polygon);
			}

			return IntersectsRoundRectPolygonArea(roundRect, detail::GetGeometry2DEffectiveRadius(roundRect), polygon);
		}

		[[nodiscard]]
		bool IntersectsRoundRectMultiPolygon(const RoundRect& roundRect, const MultiPolygon& multiPolygon) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(detail::GetGeometry2DDegenerateSegment(roundRect, kind), multiPolygon);
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);

			for (const auto& polygon : multiPolygon)
			{
				if (IntersectsRoundRectPolygonArea(roundRect, effectiveRadius, polygon))
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool IntersectsPolygonPolygonNonEmpty(
			const Polygon& a, const RectF& aBounds,
			const Polygon& b, const RectF& bBounds) noexcept
		{
			if (not BoundsIntersectClosed(aBounds, bBounds))
			{
				return false;
			}

			if (IntersectsPointPolygonNonEmpty(a.outer().front(), b, bBounds)
				|| IntersectsPointPolygonNonEmpty(b.outer().front(), a, aBounds))
			{
				return true;
			}

			return detail::AnyPolygonEdge(a, [&](const Line& edgeA)
				{
					return detail::IntersectsLineRectFNonEmpty(edgeA, bBounds)
						&& detail::AnyPolygonEdge(b, [&](const Line& edgeB)
							{
								return Geometry2D::Intersects(edgeA, edgeB);
							});
				});
		}

		[[nodiscard]]
		bool IntersectsPolygonPolygon(const Polygon& a, const Polygon& b) noexcept
		{
			if (a.isEmpty() || b.isEmpty())
			{
				return false;
			}

			const RectF aBounds = a.boundingRect();
			const RectF bBounds = b.boundingRect();

			return IntersectsPolygonPolygonNonEmpty(a, aBounds, b, bBounds);
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
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Intersects(p, detail::GetGeometry2DDegenerateSegment(superEllipse, kind));
			}

			if (superEllipse.n == 2.0)
			{
				return detail::IntersectsPointEllipseArea(p, Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y });
			}

			return IntersectsPointSuperEllipseArea(p, superEllipse);
		}

		bool Intersects(const Vec2& p, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();
			return IntersectsPointPolygonNonEmpty(p, polygon, polygonBounds);
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

			const RectF polygonBounds = polygon.boundingRect();
			return IntersectsLinePolygonNonEmpty(segment, polygon, polygonBounds);
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
			return Intersects(p, segments);
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
			return IntersectsLineStringRectF(segments, RectF{ rect });
		}

		bool Intersects(const LineString& segments, const RectF& rect) noexcept
		{
			return IntersectsLineStringRectF(segments, rect);
		}

		bool Intersects(const LineString& segments, const Circle& circle) noexcept
		{
			return IntersectsLineStringCircle(segments, circle);
		}

		bool Intersects(const LineString& segments, const Ellipse& ellipse) noexcept
		{
			return IntersectsLineStringEllipse(segments, ellipse);
		}

		bool Intersects(const LineString& segments, const SuperEllipse& superEllipse) noexcept
		{
			return IntersectsLineStringSuperEllipse(segments, superEllipse);
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
			return IntersectsLineStringRoundRect(segments, roundRect);
		}

		bool Intersects(const LineString& segments, const Polygon& polygon) noexcept
		{
			return IntersectsLineStringPolygon(segments, polygon);
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
			return detail::WithSimpleBezierPair(curve1, curve2, [](const auto& a, const auto& b)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(a)>, Bezier2>
					&& std::is_same_v<std::decay_t<decltype(b)>, Bezier2>)
				{
					return IntersectsBezier2Bezier2Approximate(a, b);
				}
				else
				{
					return Geometry2D::Intersects(a, b);
				}
			});
		}

		bool Intersects(const Bezier2& curve1, const Bezier3& curve2)
		{
			return detail::WithSimpleBezierPair(curve1, curve2, [](const auto& a, const auto& b)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(a)>, Bezier2>
					&& std::is_same_v<std::decay_t<decltype(b)>, Bezier3>)
				{
					return IntersectsBezier2Bezier3Approximate(a, b);
				}
				else
				{
					return Geometry2D::Intersects(a, b);
				}
			});
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
			return detail::WithSimpleBezierPair(curve, superEllipse, [](const auto& a, const auto& b)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(a)>, Bezier2>
					&& std::is_same_v<std::decay_t<decltype(b)>, SuperEllipse>)
				{
					return IntersectsBezier2SuperEllipse(a, b);
				}
				else
				{
					return Geometry2D::Intersects(a, b);
				}
			});
		}

		bool Intersects(const Bezier2& curve, const RoundRect& roundRect)
		{
			return IntersectsBezierRoundRect(curve, roundRect);
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
			return detail::WithSimpleBezierPair(curve1, curve2, [](const auto& a, const auto& b)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(a)>, Bezier3>
					&& std::is_same_v<std::decay_t<decltype(b)>, Bezier3>)
				{
					return IntersectsBezier3Bezier3Approximate(a, b);
				}
				else
				{
					return Geometry2D::Intersects(a, b);
				}
			});
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
			return detail::WithSimpleBezierPair(curve, superEllipse, [](const auto& a, const auto& b)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(a)>, Bezier3>
					&& std::is_same_v<std::decay_t<decltype(b)>, SuperEllipse>)
				{
					return IntersectsBezier3SuperEllipse(a, b);
				}
				else
				{
					return Geometry2D::Intersects(a, b);
				}
			});
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
			return IntersectsBezierRoundRect(curve, roundRect);
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
			return IntersectsRectFMultiPolygon(RectF{ rect }, multiPolygon);
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
			return IntersectsRectFMultiPolygon(rect, multiPolygon);
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
			return IntersectsTriangleRoundRect(triangle, roundRect);
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
			return IntersectsQuadRoundRect(quad, roundRect);
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
			return Intersects(triangle, roundRect);
		}

		bool Intersects(const RoundRect& roundRect, const Quad& quad) noexcept
		{
			return Intersects(quad, roundRect);
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
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();

			for (const auto& other : multiPolygon)
			{
				if (other.isEmpty())
				{
					continue;
				}

				const RectF otherBounds = other.boundingRect();

				if (IntersectsPolygonPolygonNonEmpty(polygon, polygonBounds, other, otherBounds))
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
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				for (const auto& other : b)
				{
					if (other.isEmpty())
					{
						continue;
					}

					const RectF otherBounds = other.boundingRect();

					if (IntersectsPolygonPolygonNonEmpty(polygon, polygonBounds, other, otherBounds))
					{
						return true;
					}
				}
			}

			return false;
		}
	}
}
