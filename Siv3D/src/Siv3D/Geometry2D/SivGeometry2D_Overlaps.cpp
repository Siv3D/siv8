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

# include <Siv3D/ListUtility.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Overlaps.hpp>

namespace s3d
{
	namespace
	{
		// Approximation-acceptable pairs use a fixed local tessellation.
		// Analytic and polygonal pairs do not depend on this value.
		inline constexpr int32 CurvedApproximationSegments = 64;
		inline constexpr double TwoPi = 6.2831853071795864769252867665590058;
		inline constexpr double EllipseDistanceRootTolerance = (16.0 * 2.2204460492503131e-16);

		[[nodiscard]]
		constexpr double Cross(const Vec2& a, const Vec2& b, const Vec2& c) noexcept
		{
			return ((b - a).cross(c - a));
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const RectF& rect) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(rect) == detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const Circle& circle) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(circle) == detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const Ellipse& ellipse) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(ellipse) == detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const SuperEllipse& superEllipse) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(superEllipse) == detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const Triangle& triangle) noexcept
		{
			return (Cross(triangle.p0, triangle.p1, triangle.p2) != 0.0);
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const Quad& quad) noexcept
		{
			const double twiceArea =
				quad.p0.cross(quad.p1)
				+ quad.p1.cross(quad.p2)
				+ quad.p2.cross(quad.p3)
				+ quad.p3.cross(quad.p0);

			return (twiceArea != 0.0);
		}

		[[nodiscard]]
		constexpr bool HasPositiveArea(const RoundRect& roundRect) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(roundRect) == detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const Polygon& polygon) noexcept
		{
			return (not polygon.isEmpty());
		}

		[[nodiscard]]
		bool HasPositiveArea(const MultiPolygon& multiPolygon) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (not polygon.isEmpty())
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		constexpr bool BoundsOverlapPositive(const RectF& a, const RectF& b) noexcept
		{
			return ((a.pos.x < (b.pos.x + b.size.x))
				&& (b.pos.x < (a.pos.x + a.size.x))
				&& (a.pos.y < (b.pos.y + b.size.y))
				&& (b.pos.y < (a.pos.y + a.size.y)));
		}

		[[nodiscard]]
		constexpr bool PointInTriangleClosed(const Vec2& p, const Triangle& triangle) noexcept
		{
			const double c0 = Cross(triangle.p0, triangle.p1, p);
			const double c1 = Cross(triangle.p1, triangle.p2, p);
			const double c2 = Cross(triangle.p2, triangle.p0, p);
			const bool hasNegative = ((c0 < 0.0) || (c1 < 0.0) || (c2 < 0.0));
			const bool hasPositive = ((0.0 < c0) || (0.0 < c1) || (0.0 < c2));
			return (not (hasNegative && hasPositive));
		}

		[[nodiscard]]
		constexpr double DistancePointSegmentSq(const Vec2& p, const Vec2& a, const Vec2& b) noexcept
		{
			const Vec2 d = (b - a);
			const double lengthSq = d.dot(d);

			if (lengthSq == 0.0)
			{
				return p.distanceFromSq(a);
			}

			const double t = Clamp(((p - a).dot(d) / lengthSq), 0.0, 1.0);
			return p.distanceFromSq(a + d * t);
		}

		[[nodiscard]]
		constexpr double DistancePointTriangleSq(const Vec2& p, const Triangle& triangle) noexcept
		{
			if (PointInTriangleClosed(p, triangle))
			{
				return 0.0;
			}

			return Min({
				DistancePointSegmentSq(p, triangle.p0, triangle.p1),
				DistancePointSegmentSq(p, triangle.p1, triangle.p2),
				DistancePointSegmentSq(p, triangle.p2, triangle.p0)
			});
		}

		[[nodiscard]]
		constexpr bool BetweenClosed(const double a, const double x, const double b) noexcept
		{
			return (a <= b)
				? ((a <= x) && (x <= b))
				: ((b <= x) && (x <= a));
		}

		[[nodiscard]]
		constexpr bool SegmentsIntersectClosed(const Vec2& a0, const Vec2& a1, const Vec2& b0, const Vec2& b1) noexcept
		{
			const double c0 = Cross(a0, a1, b0);
			const double c1 = Cross(a0, a1, b1);
			const double c2 = Cross(b0, b1, a0);
			const double c3 = Cross(b0, b1, a1);

			if ((c0 == 0.0) && BetweenClosed(a0.x, b0.x, a1.x) && BetweenClosed(a0.y, b0.y, a1.y))
			{
				return true;
			}

			if ((c1 == 0.0) && BetweenClosed(a0.x, b1.x, a1.x) && BetweenClosed(a0.y, b1.y, a1.y))
			{
				return true;
			}

			if ((c2 == 0.0) && BetweenClosed(b0.x, a0.x, b1.x) && BetweenClosed(b0.y, a0.y, b1.y))
			{
				return true;
			}

			if ((c3 == 0.0) && BetweenClosed(b0.x, a1.x, b1.x) && BetweenClosed(b0.y, a1.y, b1.y))
			{
				return true;
			}

			return ((((c0 < 0.0) && (0.0 < c1)) || ((c1 < 0.0) && (0.0 < c0)))
				&& (((c2 < 0.0) && (0.0 < c3)) || ((c3 < 0.0) && (0.0 < c2))));
		}

		[[nodiscard]]
		constexpr double DistanceSegmentSegmentSq(const Vec2& a0, const Vec2& a1, const Vec2& b0, const Vec2& b1) noexcept
		{
			if (SegmentsIntersectClosed(a0, a1, b0, b1))
			{
				return 0.0;
			}

			return Min({
				DistancePointSegmentSq(a0, b0, b1),
				DistancePointSegmentSq(a1, b0, b1),
				DistancePointSegmentSq(b0, a0, a1),
				DistancePointSegmentSq(b1, a0, a1)
			});
		}

		[[nodiscard]]
		constexpr double DistancePointRectSq(const Vec2& p, const RectF& rect) noexcept
		{
			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);
			const double closestX = Clamp(p.x, left, right);
			const double closestY = Clamp(p.y, top, bottom);
			const double dx = (p.x - closestX);
			const double dy = (p.y - closestY);
			return ((dx * dx) + (dy * dy));
		}

		[[nodiscard]]
		constexpr double DistanceClosedRects(const RectF& a, const RectF& b) noexcept
		{
			const double aRight = (a.pos.x + a.size.x);
			const double aBottom = (a.pos.y + a.size.y);
			const double bRight = (b.pos.x + b.size.x);
			const double bBottom = (b.pos.y + b.size.y);
			const double dx = Max({ (a.pos.x - bRight), (b.pos.x - aRight), 0.0 });
			const double dy = Max({ (a.pos.y - bBottom), (b.pos.y - aBottom), 0.0 });
			return std::hypot(dx, dy);
		}

		[[nodiscard]]
		constexpr double DistanceTriangleRectSq(const Triangle& triangle, const RectF& rect) noexcept
		{
			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);
			const std::array<Vec2, 4> corners{
				Vec2{ left, top },
				Vec2{ right, top },
				Vec2{ right, bottom },
				Vec2{ left, bottom }
			};
			const std::array<Vec2, 3> trianglePoints{ triangle.p0, triangle.p1, triangle.p2 };

			double result = Min({
				DistancePointRectSq(triangle.p0, rect),
				DistancePointRectSq(triangle.p1, rect),
				DistancePointRectSq(triangle.p2, rect),
				DistancePointTriangleSq(corners[0], triangle),
				DistancePointTriangleSq(corners[1], triangle),
				DistancePointTriangleSq(corners[2], triangle),
				DistancePointTriangleSq(corners[3], triangle)
			});

			for (size_t i = 0; i < trianglePoints.size(); ++i)
			{
				const Vec2 ta = trianglePoints[i];
				const Vec2 tb = trianglePoints[(i + 1) % trianglePoints.size()];

				for (size_t k = 0; k < corners.size(); ++k)
				{
					result = Min(result, DistanceSegmentSegmentSq(ta, tb, corners[k], corners[(k + 1) % corners.size()]));
				}
			}

			return result;
		}

		[[nodiscard]]
		bool ClippedTriangleIntersectionHasPositiveArea(const Triangle& subject, const Triangle& clip) noexcept
		{
			const double subjectOrientation = Cross(subject.p0, subject.p1, subject.p2);
			const double clipOrientation = Cross(clip.p0, clip.p1, clip.p2);

			if ((subjectOrientation == 0.0) || (clipOrientation == 0.0))
			{
				return false;
			}

			std::array<Vec2, 12> input{};
			std::array<Vec2, 12> output{};
			size_t inputCount = 3;
			input[0] = subject.p0;
			input[1] = subject.p1;
			input[2] = subject.p2;
			const double orientationSign = (0.0 < clipOrientation) ? 1.0 : -1.0;
			const std::array<Vec2, 3> clipPoints{ clip.p0, clip.p1, clip.p2 };

			for (size_t edgeIndex = 0; edgeIndex < clipPoints.size(); ++edgeIndex)
			{
				if (inputCount == 0)
				{
					return false;
				}

				const Vec2 edgeStart = clipPoints[edgeIndex];
				const Vec2 edgeEnd = clipPoints[(edgeIndex + 1) % clipPoints.size()];
				size_t outputCount = 0;
				Vec2 previous = input[inputCount - 1];
				double previousDistance = (orientationSign * Cross(edgeStart, edgeEnd, previous));
				bool previousInside = (0.0 <= previousDistance);

				for (size_t i = 0; i < inputCount; ++i)
				{
					const Vec2 current = input[i];
					const double currentDistance = (orientationSign * Cross(edgeStart, edgeEnd, current));
					const bool currentInside = (0.0 <= currentDistance);

					if (currentInside != previousInside)
					{
						const double denominator = (previousDistance - currentDistance);

						if (denominator != 0.0)
						{
							const double t = (previousDistance / denominator);
							output[outputCount++] = (previous + (current - previous) * t);
						}
					}

					if (currentInside)
					{
						output[outputCount++] = current;
					}

					previous = current;
					previousDistance = currentDistance;
					previousInside = currentInside;
				}

				inputCount = outputCount;

				for (size_t i = 0; i < outputCount; ++i)
				{
					input[i] = output[i];
				}
			}

			if (inputCount < 3)
			{
				return false;
			}

			double twiceArea = 0.0;

			for (size_t i = 0; i < inputCount; ++i)
			{
				twiceArea += input[i].cross(input[(i + 1) % inputCount]);
			}

			return (twiceArea != 0.0);
		}

		[[nodiscard]]
		bool OverlapsTriangles(const Triangle& a, const Triangle& b) noexcept
		{
			if (not BoundsOverlapPositive(a.boundingRect(), b.boundingRect()))
			{
				return false;
			}

			return ClippedTriangleIntersectionHasPositiveArea(a, b)
				|| ClippedTriangleIntersectionHasPositiveArea(b, a);
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitRectTriangles(const RectF& rect, Fty&& callback) noexcept
		{
			const Vec2 topLeft = rect.pos;
			const Vec2 topRight{ (rect.pos.x + rect.size.x), rect.pos.y };
			const Vec2 bottomRight{ (rect.pos.x + rect.size.x), (rect.pos.y + rect.size.y) };
			const Vec2 bottomLeft{ rect.pos.x, (rect.pos.y + rect.size.y) };

			return callback(Triangle{ topLeft, topRight, bottomRight })
				|| callback(Triangle{ topLeft, bottomRight, bottomLeft });
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitQuadTriangles(const Quad& quad, Fty&& callback) noexcept
		{
			const Triangle first{ quad.p0, quad.p1, quad.p2 };
			const Triangle second{ quad.p0, quad.p2, quad.p3 };

			return (HasPositiveArea(first) && callback(first))
				|| (HasPositiveArea(second) && callback(second));
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitPolygonTriangles(const Polygon& polygon, Fty&& callback) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const Float2* pVertex = polygon.vertices().data();

			for (const auto& triangleIndex : polygon.indices())
			{
				const Triangle triangle{
					Vec2{ pVertex[triangleIndex.i0].x, pVertex[triangleIndex.i0].y },
					Vec2{ pVertex[triangleIndex.i1].x, pVertex[triangleIndex.i1].y },
					Vec2{ pVertex[triangleIndex.i2].x, pVertex[triangleIndex.i2].y }
				};

				if (HasPositiveArea(triangle) && callback(triangle))
				{
					return true;
				}
			}

			return false;
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitEllipseFanTriangles(const Ellipse& ellipse, Fty&& callback) noexcept
		{
			const double step = (TwoPi / CurvedApproximationSegments);
			Vec2 previous{ (ellipse.center.x + ellipse.axes.x), ellipse.center.y };

			for (int32 i = 1; i <= CurvedApproximationSegments; ++i)
			{
				const Vec2 current = (i == CurvedApproximationSegments)
					? Vec2{ (ellipse.center.x + ellipse.axes.x), ellipse.center.y }
					: Vec2{
						(ellipse.center.x + std::cos(step * i) * ellipse.axes.x),
						(ellipse.center.y + std::sin(step * i) * ellipse.axes.y)
					};

				if (callback(Triangle{ ellipse.center, previous, current }))
				{
					return true;
				}

				previous = current;
			}

			return false;
		}

		template <class Fty>
		[[nodiscard]]
		bool VisitSuperEllipseFanTriangles(const SuperEllipse& superEllipse, Fty&& callback) noexcept
		{
			const double step = (TwoPi / CurvedApproximationSegments);
			const double exponent = (2.0 / superEllipse.n);

			auto PointAt = [&](const double angle) noexcept
			{
				const double c = std::cos(angle);
				const double s = std::sin(angle);
				const double x = std::copysign(std::pow(Abs(c), exponent) * superEllipse.axes.x, c);
				const double y = std::copysign(std::pow(Abs(s), exponent) * superEllipse.axes.y, s);
				return (superEllipse.center + Vec2{ x, y });
			};

			const Vec2 first{ (superEllipse.center.x + superEllipse.axes.x), superEllipse.center.y };
			Vec2 previous = first;

			for (int32 i = 1; i <= CurvedApproximationSegments; ++i)
			{
				const Vec2 current = (i == CurvedApproximationSegments) ? first : PointAt(step * i);

				if (callback(Triangle{ superEllipse.center, previous, current }))
				{
					return true;
				}

				previous = current;
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
				lambda = ((lower < newton) && (newton < upper))
					? newton
					: ((lower + upper) * 0.5);
			}

			lambda = ((lower + upper) * 0.5);
			const double closestX = ((aa * px) / (lambda + aa));
			const double closestY = ((bb * py) / (lambda + bb));
			return (scale * std::hypot((px - closestX), (py - closestY)));
		}

		[[nodiscard]]
		constexpr bool OverlapsRectRectArea(const RectF& a, const RectF& b) noexcept
		{
			return BoundsOverlapPositive(a, b);
		}

		[[nodiscard]]
		bool OverlapsRectRect(const RectF& a, const RectF& b) noexcept
		{
			return HasPositiveArea(a)
				&& HasPositiveArea(b)
				&& OverlapsRectRectArea(a, b);
		}

		[[nodiscard]]
		constexpr bool OverlapsRectCircleArea(const RectF& rect, const Circle& circle) noexcept
		{
			return (DistancePointRectSq(circle.center, rect) < (circle.r * circle.r));
		}

		[[nodiscard]]
		bool OverlapsRectCircle(const RectF& rect, const Circle& circle) noexcept
		{
			return HasPositiveArea(rect)
				&& HasPositiveArea(circle)
				&& OverlapsRectCircleArea(rect, circle);
		}

		[[nodiscard]]
		constexpr bool OverlapsRectEllipseArea(const RectF& rect, const Ellipse& ellipse) noexcept
		{
			const RectF localRect{
				((rect.pos.x - ellipse.center.x) / ellipse.axes.x),
				((rect.pos.y - ellipse.center.y) / ellipse.axes.y),
				(rect.size.x / ellipse.axes.x),
				(rect.size.y / ellipse.axes.y)
			};

			return (DistancePointRectSq(Vec2{ 0.0, 0.0 }, localRect) < 1.0);
		}

		[[nodiscard]]
		bool OverlapsRectEllipse(const RectF& rect, const Ellipse& ellipse) noexcept
		{
			return HasPositiveArea(rect)
				&& HasPositiveArea(ellipse)
				&& OverlapsRectEllipseArea(rect, ellipse);
		}

		[[nodiscard]]
		bool OverlapsRectSuperEllipseArea(const RectF& rect, const SuperEllipse& superEllipse) noexcept
		{
			const double right = (rect.pos.x + rect.size.x);
			const double bottom = (rect.pos.y + rect.size.y);
			const double closestX = Clamp(superEllipse.center.x, rect.pos.x, right);
			const double closestY = Clamp(superEllipse.center.y, rect.pos.y, bottom);
			const double dx = Abs((closestX - superEllipse.center.x) / superEllipse.axes.x);
			const double dy = Abs((closestY - superEllipse.center.y) / superEllipse.axes.y);
			return ((std::pow(dx, superEllipse.n) + std::pow(dy, superEllipse.n)) < 1.0);
		}

		[[nodiscard]]
		bool OverlapsRectSuperEllipse(const RectF& rect, const SuperEllipse& superEllipse) noexcept
		{
			return HasPositiveArea(rect)
				&& HasPositiveArea(superEllipse)
				&& OverlapsRectSuperEllipseArea(rect, superEllipse);
		}

		[[nodiscard]]
		bool OverlapsRectTriangleArea(const RectF& rect, const Triangle& triangle) noexcept
		{
			if (not BoundsOverlapPositive(rect, triangle.boundingRect()))
			{
				return false;
			}

			return VisitRectTriangles(rect, [&](const Triangle& rectTriangle)
			{
				return OverlapsTriangles(rectTriangle, triangle);
			});
		}

		[[nodiscard]]
		bool OverlapsRectTriangle(const RectF& rect, const Triangle& triangle) noexcept
		{
			return HasPositiveArea(rect)
				&& HasPositiveArea(triangle)
				&& OverlapsRectTriangleArea(rect, triangle);
		}

		[[nodiscard]]
		bool OverlapsRectPolygonAreaNonEmpty(
			const RectF& rect, const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsOverlapPositive(rect, polygonBounds))
			{
				return false;
			}

			return VisitPolygonTriangles(polygon, [&](const Triangle& part)
			{
				return OverlapsRectTriangleArea(rect, part);
			});
		}

		[[nodiscard]]
		bool OverlapsRectPolygonArea(const RectF& rect, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsRectPolygonAreaNonEmpty(rect, polygon, polygonBounds);
		}

		[[nodiscard]]
		constexpr bool OverlapsTriangleCircleArea(const Triangle& triangle, const Circle& circle) noexcept
		{
			return (DistancePointTriangleSq(circle.center, triangle) < (circle.r * circle.r));
		}

		[[nodiscard]]
		bool OverlapsTriangleCircle(const Triangle& triangle, const Circle& circle) noexcept
		{
			return HasPositiveArea(triangle)
				&& HasPositiveArea(circle)
				&& OverlapsTriangleCircleArea(triangle, circle);
		}

		[[nodiscard]]
		constexpr bool OverlapsTriangleEllipseArea(const Triangle& triangle, const Ellipse& ellipse) noexcept
		{
			const Triangle local{
				Vec2{ ((triangle.p0.x - ellipse.center.x) / ellipse.axes.x), ((triangle.p0.y - ellipse.center.y) / ellipse.axes.y) },
				Vec2{ ((triangle.p1.x - ellipse.center.x) / ellipse.axes.x), ((triangle.p1.y - ellipse.center.y) / ellipse.axes.y) },
				Vec2{ ((triangle.p2.x - ellipse.center.x) / ellipse.axes.x), ((triangle.p2.y - ellipse.center.y) / ellipse.axes.y) }
			};

			return (DistancePointTriangleSq(Vec2{ 0.0, 0.0 }, local) < 1.0);
		}

		[[nodiscard]]
		bool OverlapsTriangleEllipse(const Triangle& triangle, const Ellipse& ellipse) noexcept
		{
			return HasPositiveArea(triangle)
				&& HasPositiveArea(ellipse)
				&& OverlapsTriangleEllipseArea(triangle, ellipse);
		}

		[[nodiscard]]
		bool OverlapsTriangleSuperEllipseArea(const Triangle& triangle, const SuperEllipse& superEllipse) noexcept
		{
			if (superEllipse.n == 2.0)
			{
				return OverlapsTriangleEllipseArea(triangle,
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y });
			}

			return VisitSuperEllipseFanTriangles(superEllipse, [&](const Triangle& part)
			{
				return OverlapsTriangles(triangle, part);
			});
		}

		[[nodiscard]]
		bool OverlapsTriangleSuperEllipse(const Triangle& triangle, const SuperEllipse& superEllipse) noexcept
		{
			return HasPositiveArea(triangle)
				&& HasPositiveArea(superEllipse)
				&& OverlapsTriangleSuperEllipseArea(triangle, superEllipse);
		}

		[[nodiscard]]
		bool OverlapsTriangleRoundRectArea(const Triangle& triangle, const RoundRect& roundRect,
			const double effectiveRadius, const RectF& core) noexcept
		{
			if (effectiveRadius == 0.0)
			{
				return OverlapsRectTriangleArea(roundRect.rect, triangle);
			}

			return (DistanceTriangleRectSq(triangle, core) < (effectiveRadius * effectiveRadius));
		}

		[[nodiscard]]
		bool OverlapsTriangleRoundRect(const Triangle& triangle, const RoundRect& roundRect) noexcept
		{
			if ((not HasPositiveArea(triangle)) || (not HasPositiveArea(roundRect)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);
			return OverlapsTriangleRoundRectArea(triangle, roundRect, effectiveRadius, core);
		}

		[[nodiscard]]
		bool OverlapsCircleEllipseArea(const Circle& circle, const Ellipse& ellipse) noexcept
		{
			return (DistancePointEllipse(circle.center, ellipse) < circle.r);
		}

		[[nodiscard]]
		bool OverlapsCircleEllipse(const Circle& circle, const Ellipse& ellipse) noexcept
		{
			return HasPositiveArea(circle)
				&& HasPositiveArea(ellipse)
				&& OverlapsCircleEllipseArea(circle, ellipse);
		}

		[[nodiscard]]
		bool OverlapsCircleSuperEllipseArea(const Circle& circle, const SuperEllipse& superEllipse) noexcept
		{
			if (superEllipse.n == 2.0)
			{
				return OverlapsCircleEllipseArea(circle,
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y });
			}

			return VisitSuperEllipseFanTriangles(superEllipse, [&](const Triangle& part)
			{
				return OverlapsTriangleCircleArea(part, circle);
			});
		}

		[[nodiscard]]
		bool OverlapsCircleSuperEllipse(const Circle& circle, const SuperEllipse& superEllipse) noexcept
		{
			return HasPositiveArea(circle)
				&& HasPositiveArea(superEllipse)
				&& OverlapsCircleSuperEllipseArea(circle, superEllipse);
		}

		[[nodiscard]]
		constexpr bool OverlapsCircleRoundRectArea(const Circle& circle, const double effectiveRadius,
			const RectF& core) noexcept
		{
			const double radiusSum = (circle.r + effectiveRadius);
			return (DistancePointRectSq(circle.center, core) < (radiusSum * radiusSum));
		}

		[[nodiscard]]
		bool OverlapsCircleRoundRect(const Circle& circle, const RoundRect& roundRect) noexcept
		{
			if ((not HasPositiveArea(circle)) || (not HasPositiveArea(roundRect)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);
			return OverlapsCircleRoundRectArea(circle, effectiveRadius, core);
		}

		[[nodiscard]]
		bool OverlapsEllipseEllipseArea(const Ellipse& a, const Ellipse& b) noexcept
		{
			return VisitEllipseFanTriangles(a, [&](const Triangle& part)
			{
				return OverlapsTriangleEllipseArea(part, b);
			})
				|| VisitEllipseFanTriangles(b, [&](const Triangle& part)
				{
					return OverlapsTriangleEllipseArea(part, a);
				});
		}

		[[nodiscard]]
		bool OverlapsEllipseEllipse(const Ellipse& a, const Ellipse& b) noexcept
		{
			return HasPositiveArea(a)
				&& HasPositiveArea(b)
				&& OverlapsEllipseEllipseArea(a, b);
		}

		[[nodiscard]]
		bool OverlapsEllipseSuperEllipseArea(const Ellipse& ellipse, const SuperEllipse& superEllipse) noexcept
		{
			if (superEllipse.n == 2.0)
			{
				return OverlapsEllipseEllipseArea(ellipse,
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y });
			}

			return VisitSuperEllipseFanTriangles(superEllipse, [&](const Triangle& part)
			{
				return OverlapsTriangleEllipseArea(part, ellipse);
			});
		}

		[[nodiscard]]
		bool OverlapsEllipseSuperEllipse(const Ellipse& ellipse, const SuperEllipse& superEllipse) noexcept
		{
			return HasPositiveArea(ellipse)
				&& HasPositiveArea(superEllipse)
				&& OverlapsEllipseSuperEllipseArea(ellipse, superEllipse);
		}

		[[nodiscard]]
		bool OverlapsEllipseRoundRectArea(const Ellipse& ellipse, const RoundRect& roundRect,
			const double effectiveRadius, const RectF& core) noexcept
		{
			return VisitEllipseFanTriangles(ellipse, [&](const Triangle& part)
			{
				return OverlapsTriangleRoundRectArea(part, roundRect, effectiveRadius, core);
			});
		}

		[[nodiscard]]
		bool OverlapsEllipseRoundRect(const Ellipse& ellipse, const RoundRect& roundRect) noexcept
		{
			if ((not HasPositiveArea(ellipse)) || (not HasPositiveArea(roundRect)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);
			return OverlapsEllipseRoundRectArea(ellipse, roundRect, effectiveRadius, core);
		}

		[[nodiscard]]
		bool OverlapsSuperEllipseSuperEllipseArea(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			if (a.n == 2.0)
			{
				return OverlapsEllipseSuperEllipseArea(
					Ellipse{ a.center, a.axes.x, a.axes.y }, b);
			}

			if (b.n == 2.0)
			{
				return OverlapsEllipseSuperEllipseArea(
					Ellipse{ b.center, b.axes.x, b.axes.y }, a);
			}

			return VisitSuperEllipseFanTriangles(a, [&](const Triangle& aPart)
			{
				return VisitSuperEllipseFanTriangles(b, [&](const Triangle& bPart)
				{
					return OverlapsTriangles(aPart, bPart);
				});
			});
		}

		[[nodiscard]]
		bool OverlapsSuperEllipseSuperEllipse(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			return HasPositiveArea(a)
				&& HasPositiveArea(b)
				&& OverlapsSuperEllipseSuperEllipseArea(a, b);
		}

		[[nodiscard]]
		bool OverlapsSuperEllipseRoundRectArea(const SuperEllipse& superEllipse, const RoundRect& roundRect,
			const double effectiveRadius, const RectF& core) noexcept
		{
			if (superEllipse.n == 2.0)
			{
				return OverlapsEllipseRoundRectArea(
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y },
					roundRect, effectiveRadius, core);
			}

			return VisitSuperEllipseFanTriangles(superEllipse, [&](const Triangle& part)
			{
				return OverlapsTriangleRoundRectArea(part, roundRect, effectiveRadius, core);
			});
		}

		[[nodiscard]]
		bool OverlapsSuperEllipseRoundRect(const SuperEllipse& superEllipse, const RoundRect& roundRect) noexcept
		{
			if ((not HasPositiveArea(superEllipse)) || (not HasPositiveArea(roundRect)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);
			return OverlapsSuperEllipseRoundRectArea(superEllipse, roundRect, effectiveRadius, core);
		}

		[[nodiscard]]
		bool OverlapsRoundRectRoundRectArea(const RoundRect& a, const RoundRect& b) noexcept
		{
			const double ar = detail::GetGeometry2DEffectiveRadius(a);
			const double br = detail::GetGeometry2DEffectiveRadius(b);

			if ((ar == 0.0) && (br == 0.0))
			{
				return OverlapsRectRectArea(a.rect, b.rect);
			}

			const RectF aCore = detail::GetGeometry2DRoundRectCore(a, ar);
			const RectF bCore = detail::GetGeometry2DRoundRectCore(b, br);
			return (DistanceClosedRects(aCore, bCore) < (ar + br));
		}

		[[nodiscard]]
		bool OverlapsRoundRectRoundRect(const RoundRect& a, const RoundRect& b) noexcept
		{
			return HasPositiveArea(a)
				&& HasPositiveArea(b)
				&& OverlapsRoundRectRoundRectArea(a, b);
		}

		[[nodiscard]]
		bool OverlapsTrianglePolygonAreaNonEmpty(
			const Triangle& triangle, const RectF& triangleBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsOverlapPositive(triangleBounds, polygonBounds))
			{
				return false;
			}

			return VisitPolygonTriangles(polygon, [&](const Triangle& part)
			{
				return OverlapsTriangles(triangle, part);
			});
		}

		[[nodiscard]]
		bool OverlapsTrianglePolygonArea(const Triangle& triangle, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF triangleBounds = triangle.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsTrianglePolygonAreaNonEmpty(triangle, triangleBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool OverlapsTrianglePolygon(const Triangle& triangle, const Polygon& polygon) noexcept
		{
			return HasPositiveArea(triangle)
				&& HasPositiveArea(polygon)
				&& OverlapsTrianglePolygonArea(triangle, polygon);
		}

		[[nodiscard]]
		bool OverlapsCirclePolygonAreaNonEmpty(
			const Circle& circle, const RectF& circleBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsOverlapPositive(circleBounds, polygonBounds))
			{
				return false;
			}

			return VisitPolygonTriangles(polygon, [&](const Triangle& part)
			{
				return OverlapsTriangleCircleArea(part, circle);
			});
		}

		[[nodiscard]]
		bool OverlapsCirclePolygonArea(const Circle& circle, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF circleBounds = circle.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsCirclePolygonAreaNonEmpty(circle, circleBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool OverlapsCirclePolygon(const Circle& circle, const Polygon& polygon) noexcept
		{
			return HasPositiveArea(circle)
				&& HasPositiveArea(polygon)
				&& OverlapsCirclePolygonArea(circle, polygon);
		}

		[[nodiscard]]
		bool OverlapsEllipsePolygonAreaNonEmpty(
			const Ellipse& ellipse, const RectF& ellipseBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsOverlapPositive(ellipseBounds, polygonBounds))
			{
				return false;
			}

			return VisitPolygonTriangles(polygon, [&](const Triangle& part)
			{
				return OverlapsTriangleEllipseArea(part, ellipse);
			});
		}

		[[nodiscard]]
		bool OverlapsEllipsePolygonArea(const Ellipse& ellipse, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF ellipseBounds = ellipse.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsEllipsePolygonAreaNonEmpty(ellipse, ellipseBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool OverlapsEllipsePolygon(const Ellipse& ellipse, const Polygon& polygon) noexcept
		{
			return HasPositiveArea(ellipse)
				&& HasPositiveArea(polygon)
				&& OverlapsEllipsePolygonArea(ellipse, polygon);
		}

		[[nodiscard]]
		bool OverlapsSuperEllipsePolygonAreaNonEmpty(
			const SuperEllipse& superEllipse, const RectF& superEllipseBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsOverlapPositive(superEllipseBounds, polygonBounds))
			{
				return false;
			}

			if (superEllipse.n == 2.0)
			{
				return OverlapsEllipsePolygonAreaNonEmpty(
					Ellipse{ superEllipse.center, superEllipse.axes.x, superEllipse.axes.y },
					superEllipseBounds, polygon, polygonBounds);
			}

			return VisitSuperEllipseFanTriangles(superEllipse, [&](const Triangle& part)
			{
				const RectF partBounds = part.boundingRect();
				return OverlapsTrianglePolygonAreaNonEmpty(part, partBounds, polygon, polygonBounds);
			});
		}

		[[nodiscard]]
		bool OverlapsSuperEllipsePolygonArea(const SuperEllipse& superEllipse, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF superEllipseBounds = superEllipse.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsSuperEllipsePolygonAreaNonEmpty(
				superEllipse, superEllipseBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool OverlapsSuperEllipsePolygon(const SuperEllipse& superEllipse, const Polygon& polygon) noexcept
		{
			return HasPositiveArea(superEllipse)
				&& HasPositiveArea(polygon)
				&& OverlapsSuperEllipsePolygonArea(superEllipse, polygon);
		}

		[[nodiscard]]
		bool OverlapsRoundRectPolygonAreaNonEmpty(
			const RoundRect& roundRect, const Polygon& polygon, const RectF& polygonBounds,
			const double effectiveRadius, const RectF& core) noexcept
		{
			if (not BoundsOverlapPositive(roundRect.rect, polygonBounds))
			{
				return false;
			}

			return VisitPolygonTriangles(polygon, [&](const Triangle& part)
			{
				return OverlapsTriangleRoundRectArea(part, roundRect, effectiveRadius, core);
			});
		}

		[[nodiscard]]
		bool OverlapsRoundRectPolygonArea(const RoundRect& roundRect, const Polygon& polygon,
			const double effectiveRadius, const RectF& core) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsRoundRectPolygonAreaNonEmpty(
				roundRect, polygon, polygonBounds, effectiveRadius, core);
		}

		[[nodiscard]]
		bool OverlapsRoundRectPolygon(const RoundRect& roundRect, const Polygon& polygon) noexcept
		{
			if ((not HasPositiveArea(roundRect)) || (not HasPositiveArea(polygon)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);
			return OverlapsRoundRectPolygonArea(roundRect, polygon, effectiveRadius, core);
		}

		[[nodiscard]]
		bool OverlapsQuadPolygonAreaNonEmpty(
			const Quad& quad, const RectF& quadBounds,
			const Polygon& polygon, const RectF& polygonBounds) noexcept
		{
			if (not BoundsOverlapPositive(quadBounds, polygonBounds))
			{
				return false;
			}

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				const RectF partBounds = part.boundingRect();
				return OverlapsTrianglePolygonAreaNonEmpty(part, partBounds, polygon, polygonBounds);
			});
		}

		[[nodiscard]]
		bool OverlapsQuadPolygonArea(const Quad& quad, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			const RectF quadBounds = quad.boundingRect();
			const RectF polygonBounds = polygon.boundingRect();
			return OverlapsQuadPolygonAreaNonEmpty(quad, quadBounds, polygon, polygonBounds);
		}

		[[nodiscard]]
		bool OverlapsPolygonPolygonAreaNonEmpty(
			const Polygon& a, const RectF& aBounds,
			const Polygon& b, const RectF& bBounds) noexcept
		{
			if (not BoundsOverlapPositive(aBounds, bBounds))
			{
				return false;
			}

			return VisitPolygonTriangles(a, [&](const Triangle& aPart)
			{
				return VisitPolygonTriangles(b, [&](const Triangle& bPart)
				{
					return OverlapsTriangles(aPart, bPart);
				});
			});
		}

		[[nodiscard]]
		bool OverlapsPolygonPolygonArea(const Polygon& a, const Polygon& b) noexcept
		{
			if (a.isEmpty() || b.isEmpty())
			{
				return false;
			}

			const RectF aBounds = a.boundingRect();
			const RectF bBounds = b.boundingRect();
			return OverlapsPolygonPolygonAreaNonEmpty(a, aBounds, b, bBounds);
		}

		[[nodiscard]]
		bool OverlapsPolygonPolygon(const Polygon& a, const Polygon& b) noexcept
		{
			return HasPositiveArea(a)
				&& HasPositiveArea(b)
				&& OverlapsPolygonPolygonArea(a, b);
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const Rect& a, const Rect& b) noexcept
		{
			return Overlaps(RectF{ a }, RectF{ b });
		}

		bool Overlaps(const Rect& a, const RectF& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const Circle& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const Ellipse& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const SuperEllipse& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const Triangle& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const Quad& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const RoundRect& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const Polygon& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		bool Overlaps(const Rect& a, const MultiPolygon& b) noexcept
		{
			return Overlaps(RectF{ a }, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const RectF& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const RectF& a, const RectF& b) noexcept
		{
			return OverlapsRectRect(a, b);
		}

		bool Overlaps(const RectF& rect, const Circle& circle) noexcept
		{
			return OverlapsRectCircle(rect, circle);
		}

		bool Overlaps(const RectF& rect, const Ellipse& ellipse) noexcept
		{
			return OverlapsRectEllipse(rect, ellipse);
		}

		bool Overlaps(const RectF& rect, const SuperEllipse& superEllipse) noexcept
		{
			return OverlapsRectSuperEllipse(rect, superEllipse);
		}

		bool Overlaps(const RectF& rect, const Triangle& triangle) noexcept
		{
			return OverlapsRectTriangle(rect, triangle);
		}

		bool Overlaps(const RectF& rect, const Quad& quad) noexcept
		{
			if ((not HasPositiveArea(rect)) || (not HasPositiveArea(quad)))
			{
				return false;
			}

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				return OverlapsRectTriangleArea(rect, part);
			});
		}

		bool Overlaps(const RectF& rect, const RoundRect& roundRect) noexcept
		{
			if ((not HasPositiveArea(rect)) || (not HasPositiveArea(roundRect)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);

			if (effectiveRadius == 0.0)
			{
				return OverlapsRectRectArea(rect, roundRect.rect);
			}

			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);
			return (DistanceClosedRects(rect, core) < effectiveRadius);
		}

		bool Overlaps(const RectF& rect, const Polygon& polygon) noexcept
		{
			return HasPositiveArea(rect)
				&& HasPositiveArea(polygon)
				&& OverlapsRectPolygonArea(rect, polygon);
		}

		bool Overlaps(const RectF& rect, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(rect)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsRectPolygonAreaNonEmpty(rect, polygon, polygonBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const Circle& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const Circle& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Circle& a, const Circle& b) noexcept
		{
			if ((not HasPositiveArea(a)) || (not HasPositiveArea(b)))
			{
				return false;
			}

			const double radiusSum = (a.r + b.r);
			return (a.center.distanceFromSq(b.center) < (radiusSum * radiusSum));
		}

		bool Overlaps(const Circle& circle, const Ellipse& ellipse) noexcept
		{
			return OverlapsCircleEllipse(circle, ellipse);
		}

		bool Overlaps(const Circle& circle, const SuperEllipse& superEllipse) noexcept
		{
			return OverlapsCircleSuperEllipse(circle, superEllipse);
		}

		bool Overlaps(const Circle& circle, const Triangle& triangle) noexcept
		{
			return OverlapsTriangleCircle(triangle, circle);
		}

		bool Overlaps(const Circle& circle, const Quad& quad) noexcept
		{
			if ((not HasPositiveArea(circle)) || (not HasPositiveArea(quad)))
			{
				return false;
			}

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				return OverlapsTriangleCircleArea(part, circle);
			});
		}

		bool Overlaps(const Circle& circle, const RoundRect& roundRect) noexcept
		{
			return OverlapsCircleRoundRect(circle, roundRect);
		}

		bool Overlaps(const Circle& circle, const Polygon& polygon) noexcept
		{
			return OverlapsCirclePolygon(circle, polygon);
		}

		bool Overlaps(const Circle& circle, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(circle)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			const RectF circleBounds = circle.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsCirclePolygonAreaNonEmpty(circle, circleBounds, polygon, polygonBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const Ellipse& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const Ellipse& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Ellipse& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Ellipse& a, const Ellipse& b) noexcept
		{
			return OverlapsEllipseEllipse(a, b);
		}

		bool Overlaps(const Ellipse& ellipse, const SuperEllipse& superEllipse) noexcept
		{
			return OverlapsEllipseSuperEllipse(ellipse, superEllipse);
		}

		bool Overlaps(const Ellipse& ellipse, const Triangle& triangle) noexcept
		{
			return OverlapsTriangleEllipse(triangle, ellipse);
		}

		bool Overlaps(const Ellipse& ellipse, const Quad& quad) noexcept
		{
			if ((not HasPositiveArea(ellipse)) || (not HasPositiveArea(quad)))
			{
				return false;
			}

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				return OverlapsTriangleEllipseArea(part, ellipse);
			});
		}

		bool Overlaps(const Ellipse& ellipse, const RoundRect& roundRect) noexcept
		{
			return OverlapsEllipseRoundRect(ellipse, roundRect);
		}

		bool Overlaps(const Ellipse& ellipse, const Polygon& polygon) noexcept
		{
			return OverlapsEllipsePolygon(ellipse, polygon);
		}

		bool Overlaps(const Ellipse& ellipse, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(ellipse)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			const RectF ellipseBounds = ellipse.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsEllipsePolygonAreaNonEmpty(ellipse, ellipseBounds, polygon, polygonBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const SuperEllipse& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const SuperEllipse& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const SuperEllipse& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const SuperEllipse& a, const Ellipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			return OverlapsSuperEllipseSuperEllipse(a, b);
		}

		bool Overlaps(const SuperEllipse& superEllipse, const Triangle& triangle) noexcept
		{
			return OverlapsTriangleSuperEllipse(triangle, superEllipse);
		}

		bool Overlaps(const SuperEllipse& superEllipse, const Quad& quad) noexcept
		{
			if ((not HasPositiveArea(superEllipse)) || (not HasPositiveArea(quad)))
			{
				return false;
			}

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				return OverlapsTriangleSuperEllipseArea(part, superEllipse);
			});
		}

		bool Overlaps(const SuperEllipse& superEllipse, const RoundRect& roundRect) noexcept
		{
			return OverlapsSuperEllipseRoundRect(superEllipse, roundRect);
		}

		bool Overlaps(const SuperEllipse& superEllipse, const Polygon& polygon) noexcept
		{
			return OverlapsSuperEllipsePolygon(superEllipse, polygon);
		}

		bool Overlaps(const SuperEllipse& superEllipse, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(superEllipse)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			const RectF superEllipseBounds = superEllipse.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsSuperEllipsePolygonAreaNonEmpty(
					superEllipse, superEllipseBounds, polygon, polygonBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const Triangle& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const Triangle& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Triangle& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Triangle& a, const Ellipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Triangle& a, const SuperEllipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Triangle& a, const Triangle& b) noexcept
		{
			return OverlapsTriangles(a, b);
		}

		bool Overlaps(const Triangle& triangle, const Quad& quad) noexcept
		{
			if ((not HasPositiveArea(triangle)) || (not HasPositiveArea(quad)))
			{
				return false;
			}

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				return OverlapsTriangles(triangle, part);
			});
		}

		bool Overlaps(const Triangle& triangle, const RoundRect& roundRect) noexcept
		{
			return OverlapsTriangleRoundRect(triangle, roundRect);
		}

		bool Overlaps(const Triangle& triangle, const Polygon& polygon) noexcept
		{
			return OverlapsTrianglePolygon(triangle, polygon);
		}

		bool Overlaps(const Triangle& triangle, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(triangle)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			const RectF triangleBounds = triangle.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsTrianglePolygonAreaNonEmpty(triangle, triangleBounds, polygon, polygonBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const Quad& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const Quad& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Quad& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Quad& a, const Ellipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Quad& a, const SuperEllipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Quad& a, const Triangle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Quad& a, const Quad& b) noexcept
		{
			if ((not HasPositiveArea(a)) || (not HasPositiveArea(b)))
			{
				return false;
			}

			return VisitQuadTriangles(a, [&](const Triangle& aPart)
			{
				return VisitQuadTriangles(b, [&](const Triangle& bPart)
				{
					return OverlapsTriangles(aPart, bPart);
				});
			});
		}

		bool Overlaps(const Quad& quad, const RoundRect& roundRect) noexcept
		{
			if ((not HasPositiveArea(quad)) || (not HasPositiveArea(roundRect)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);

			return VisitQuadTriangles(quad, [&](const Triangle& part)
			{
				return OverlapsTriangleRoundRectArea(part, roundRect, effectiveRadius, core);
			});
		}

		bool Overlaps(const Quad& quad, const Polygon& polygon) noexcept
		{
			if ((not HasPositiveArea(quad)) || (not HasPositiveArea(polygon)))
			{
				return false;
			}

			return OverlapsQuadPolygonArea(quad, polygon);
		}

		bool Overlaps(const Quad& quad, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(quad)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			const RectF quadBounds = quad.boundingRect();

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsQuadPolygonAreaNonEmpty(quad, quadBounds, polygon, polygonBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const RoundRect& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const RoundRect& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const RoundRect& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const RoundRect& a, const Ellipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const RoundRect& a, const SuperEllipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const RoundRect& a, const Triangle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const RoundRect& a, const Quad& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const RoundRect& a, const RoundRect& b) noexcept
		{
			return OverlapsRoundRectRoundRect(a, b);
		}

		bool Overlaps(const RoundRect& roundRect, const Polygon& polygon) noexcept
		{
			return OverlapsRoundRectPolygon(roundRect, polygon);
		}

		bool Overlaps(const RoundRect& roundRect, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(roundRect)) || (not HasPositiveArea(multiPolygon)))
			{
				return false;
			}

			const double effectiveRadius = detail::GetGeometry2DEffectiveRadius(roundRect);
			const RectF core = detail::GetGeometry2DRoundRectCore(roundRect, effectiveRadius);

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				const RectF polygonBounds = polygon.boundingRect();

				if (OverlapsRoundRectPolygonAreaNonEmpty(
					roundRect, polygon, polygonBounds, effectiveRadius, core))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const Polygon& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const Polygon& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const Ellipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const SuperEllipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const Triangle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const Quad& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const RoundRect& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const Polygon& a, const Polygon& b) noexcept
		{
			return OverlapsPolygonPolygon(a, b);
		}

		bool Overlaps(const Polygon& polygon, const MultiPolygon& multiPolygon) noexcept
		{
			if ((not HasPositiveArea(polygon)) || (not HasPositiveArea(multiPolygon)))
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

				if (OverlapsPolygonPolygonAreaNonEmpty(polygon, polygonBounds, other, otherBounds))
				{
					return true;
				}
			}

			return false;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Overlaps(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		bool Overlaps(const MultiPolygon& a, const Rect& b) noexcept
		{
			return Overlaps(a, RectF{ b });
		}

		bool Overlaps(const MultiPolygon& a, const RectF& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const Circle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const Ellipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const SuperEllipse& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const Triangle& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const Quad& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const RoundRect& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const Polygon& b) noexcept
		{
			return Overlaps(b, a);
		}

		bool Overlaps(const MultiPolygon& a, const MultiPolygon& b) noexcept
		{
			if ((not HasPositiveArea(a)) || (not HasPositiveArea(b)))
			{
				return false;
			}

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

					if (OverlapsPolygonPolygonAreaNonEmpty(polygon, polygonBounds, other, otherBounds))
					{
						return true;
					}
				}
			}

			return false;
		}
	}
}
