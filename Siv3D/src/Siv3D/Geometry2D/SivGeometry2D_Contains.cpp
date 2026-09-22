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
# include <Siv3D/LineString.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/Contains.hpp>
# include "PolygonGeometry.hpp"

namespace s3d
{
	namespace
	{
		inline constexpr int32 CurvedContainmentSegments = 128;
		inline constexpr int32 SuperEllipseSearchIterations = 64;
		inline constexpr double TwoPi = 6.2831853071795864769252867665590058;
		inline constexpr double DoubleEpsilon = 2.2204460492503131e-16;


		[[nodiscard]]
		constexpr bool IsEmpty(const RectF& rect) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(rect)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		constexpr bool IsEmpty(const Circle& circle) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(circle)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		constexpr bool IsEmpty(const Ellipse& ellipse) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(ellipse)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		constexpr bool IsEmpty(const SuperEllipse& superEllipse) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(superEllipse)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		constexpr bool IsEmpty(const RoundRect& roundRect) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(roundRect)
				== detail::Geometry2DSizedShapeKind::Empty);
		}



		[[nodiscard]]
		constexpr bool BoundsContainsClosed(const RectF& outer, const RectF& inner) noexcept
		{
			const double outerRight = (outer.pos.x + outer.size.x);
			const double outerBottom = (outer.pos.y + outer.size.y);
			const double innerRight = (inner.pos.x + inner.size.x);
			const double innerBottom = (inner.pos.y + inner.size.y);

			return ((outer.pos.x <= inner.pos.x)
				&& (innerRight <= outerRight)
				&& (outer.pos.y <= inner.pos.y)
				&& (innerBottom <= outerBottom));
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsLineStringBySegments(const Container& container, const LineString& lineString) noexcept
		{
			const size_t n = lineString.size();

			if (n == 0)
			{
				return false;
			}

			const Vec2* points = lineString.data();

			if (n == 1)
			{
				return Geometry2D::Contains(container, points[0]);
			}

			for (size_t i = 0; i < (n - 1); ++i)
			{
				if (not Geometry2D::Contains(container, Line{ points[i], points[i + 1] }))
				{
					return false;
				}
			}

			return true;
		}

		template <class Shape>
		[[nodiscard]]
		bool ContainsBySingleMultiPolygonMember(const MultiPolygon& multiPolygon, const Shape& shape) noexcept
		{
			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				if (Geometry2D::Contains(polygon, shape))
				{
					return true;
				}
			}

			return false;
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsMultiPolygonByMembers(const Container& container, const MultiPolygon& multiPolygon) noexcept
		{
			bool hasNonEmptyMember = false;

			for (const auto& polygon : multiPolygon)
			{
				if (polygon.isEmpty())
				{
					continue;
				}

				hasNonEmptyMember = true;

				if (not Geometry2D::Contains(container, polygon))
				{
					return false;
				}
			}

			return hasNonEmptyMember;
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsRectFByDecomposition(const Container& container, const RectF& rect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(rect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Contains(container,
					detail::GetGeometry2DDegenerateSegment(rect, kind));
			}

			const double left = rect.pos.x;
			const double top = rect.pos.y;
			const double right = (left + rect.size.x);
			const double bottom = (top + rect.size.y);
			const Vec2 p0{ left, top };
			const Vec2 p1{ right, top };
			const Vec2 p2{ right, bottom };
			const Vec2 p3{ left, bottom };

			return Geometry2D::Contains(container, Triangle{ p0, p1, p2 })
				&& Geometry2D::Contains(container, Triangle{ p0, p2, p3 });
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsQuadByDecomposition(const Container& container, const Quad& quad) noexcept
		{
			return Geometry2D::Contains(container, Triangle{ quad.p0, quad.p1, quad.p2 })
				&& Geometry2D::Contains(container, Triangle{ quad.p0, quad.p2, quad.p3 });
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsPolygonByVertices(const Container& container, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}
			for (const Vec2& point : polygon.outer())
			{
				if (not Geometry2D::Contains(container, point))
				{
					return false;
				}
			}
			return true;
		}

		[[nodiscard]]
		Vec2 IntersectSupportLines(
			const Vec2& n0, const double h0,
			const Vec2& n1, const double h1) noexcept
		{
			const double determinant = n0.cross(n1);
			assert(determinant != 0.0);

			return Vec2{
				((h0 * n1.y - n0.y * h1) / determinant),
				((n0.x * h1 - h0 * n1.x) / determinant)
			};
		}

		template <class Container, class SupportFunction>
		[[nodiscard]]
		bool ContainsConvexSupportShape(
			const Container& container, const Vec2& center,
			SupportFunction&& supportFunction) noexcept
		{
			const double step = (TwoPi / CurvedContainmentSegments);

			auto SupportVertex = [&](const int32 i) noexcept
			{
				const double angle0 = (step * i);
				const double angle1 = (step * (i + 1));
				const Vec2 n0{ std::cos(angle0), std::sin(angle0) };
				const Vec2 n1{ std::cos(angle1), std::sin(angle1) };
				return IntersectSupportLines(
					n0, supportFunction(n0), n1, supportFunction(n1));
			};

			const Vec2 first = SupportVertex(0);
			Vec2 previous = first;

			for (int32 i = 1; i < CurvedContainmentSegments; ++i)
			{
				const Vec2 current = SupportVertex(i);

				if (not Geometry2D::Contains(container, Triangle{ center, previous, current }))
				{
					return false;
				}

				previous = current;
			}

			return Geometry2D::Contains(container, Triangle{ center, previous, first });
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsCircleByApproximation(const Container& container, const Circle& circle) noexcept
		{
			if (IsEmpty(circle))
			{
				return false;
			}

			return ContainsConvexSupportShape(container, circle.center,
				[&](const Vec2& normal) noexcept
				{
					return (circle.center.dot(normal) + circle.r);
				});
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsEllipseByApproximation(const Container& container, const Ellipse& ellipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(ellipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Contains(container,
					detail::GetGeometry2DDegenerateSegment(ellipse, kind));
			}

			return ContainsConvexSupportShape(container, ellipse.center,
				[&](const Vec2& normal) noexcept
				{
					const double x = (ellipse.axes.x * normal.x);
					const double y = (ellipse.axes.y * normal.y);
					return (ellipse.center.dot(normal) + std::sqrt(x * x + y * y));
				});
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsSuperEllipseByApproximation(
			const Container& container, const SuperEllipse& superEllipse) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Contains(container,
					detail::GetGeometry2DDegenerateSegment(superEllipse, kind));
			}

			if (superEllipse.n < 1.0)
			{
				// The non-convex case uses its bounding box as a conservative superset.
				return Geometry2D::Contains(container, superEllipse.boundingRect());
			}

			return ContainsConvexSupportShape(container, superEllipse.center,
				[&](const Vec2& normal) noexcept
				{
					double radialSupport = 0.0;

					if (superEllipse.n == 1.0)
					{
						radialSupport = Max(
							superEllipse.axes.x * Abs(normal.x),
							superEllipse.axes.y * Abs(normal.y));
					}
					else
					{
						const double q = (superEllipse.n / (superEllipse.n - 1.0));
						const double x = std::pow(superEllipse.axes.x * Abs(normal.x), q);
						const double y = std::pow(superEllipse.axes.y * Abs(normal.y), q);
						radialSupport = std::pow((x + y), (1.0 / q));
					}

					return (superEllipse.center.dot(normal) + radialSupport);
				});
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsRoundRectByApproximation(
			const Container& container, const RoundRect& roundRect) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(roundRect);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				return Geometry2D::Contains(container,
					detail::GetGeometry2DDegenerateSegment(roundRect, kind));
			}

			const double radius = detail::GetGeometry2DEffectiveRadius(roundRect);

			if (radius == 0.0)
			{
				return Geometry2D::Contains(container, roundRect.rect);
			}

			const Vec2 center{
				(roundRect.rect.pos.x + roundRect.rect.size.x * 0.5),
				(roundRect.rect.pos.y + roundRect.rect.size.y * 0.5)
			};
			const double halfCoreWidth = (roundRect.rect.size.x * 0.5 - radius);
			const double halfCoreHeight = (roundRect.rect.size.y * 0.5 - radius);

			return ContainsConvexSupportShape(container, center,
				[&](const Vec2& normal) noexcept
				{
					return (center.dot(normal)
						+ halfCoreWidth * Abs(normal.x)
						+ halfCoreHeight * Abs(normal.y)
						+ radius);
				});
		}

		[[nodiscard]]
		double SuperEllipseValueAt(
			const SuperEllipse& superEllipse, const Line& segment, const double t) noexcept
		{
			const Vec2 p = (segment.start + (segment.end - segment.start) * t);
			const double x = Abs((p.x - superEllipse.center.x) / superEllipse.axes.x);
			const double y = Abs((p.y - superEllipse.center.y) / superEllipse.axes.y);
			return (std::pow(x, superEllipse.n) + std::pow(y, superEllipse.n));
		}

		[[nodiscard]]
		bool ContainsLineNonConvexSuperEllipse(
			const SuperEllipse& superEllipse, const Line& segment) noexcept
		{
			if ((not Geometry2D::Contains(superEllipse, segment.start))
				|| (not Geometry2D::Contains(superEllipse, segment.end)))
			{
				return false;
			}

			if (segment.start == segment.end)
			{
				return true;
			}

			std::array<double, 4> breaks{ 0.0, 1.0, 0.0, 0.0 };
			size_t count = 2;
			const Vec2 d = (segment.end - segment.start);

			auto AddAxisCrossing = [&](const double start, const double delta, const double center) noexcept
			{
				if (delta == 0.0)
				{
					return;
				}

				const double t = ((center - start) / delta);

				if ((0.0 < t) && (t < 1.0))
				{
					breaks[count++] = t;
				}
			};

			AddAxisCrossing(segment.start.x, d.x, superEllipse.center.x);
			AddAxisCrossing(segment.start.y, d.y, superEllipse.center.y);
			std::sort(breaks.begin(), breaks.begin() + count);

			size_t uniqueCount = 1;

			for (size_t i = 1; i < count; ++i)
			{
				if (breaks[i] != breaks[uniqueCount - 1])
				{
					breaks[uniqueCount++] = breaks[i];
				}
			}

			const double tolerance = (64.0 * DoubleEpsilon);

			for (size_t i = 0; i < (uniqueCount - 1); ++i)
			{
				double left = breaks[i];
				double right = breaks[i + 1];

				for (int32 iteration = 0; iteration < SuperEllipseSearchIterations; ++iteration)
				{
					const double third = ((right - left) / 3.0);
					const double m0 = (left + third);
					const double m1 = (right - third);

					if (SuperEllipseValueAt(superEllipse, segment, m0)
						< SuperEllipseValueAt(superEllipse, segment, m1))
					{
						left = m0;
					}
					else
					{
						right = m1;
					}
				}

				const double maximum = Max({
					SuperEllipseValueAt(superEllipse, segment, breaks[i]),
					SuperEllipseValueAt(superEllipse, segment, breaks[i + 1]),
					SuperEllipseValueAt(superEllipse, segment, (left + right) * 0.5)
				});

				if ((1.0 + tolerance) < maximum)
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]]
		bool ContainsTriangleNonConvexSuperEllipse(
			const SuperEllipse& superEllipse, const Triangle& triangle) noexcept
		{
			const RectF bounds = triangle.boundingRect();
			const double right = (bounds.pos.x + bounds.size.x);
			const double bottom = (bounds.pos.y + bounds.size.y);

			return Geometry2D::Contains(superEllipse, bounds.pos)
				&& Geometry2D::Contains(superEllipse, Vec2{ right, bounds.pos.y })
				&& Geometry2D::Contains(superEllipse, Vec2{ right, bottom })
				&& Geometry2D::Contains(superEllipse, Vec2{ bounds.pos.x, bottom });
		}

		[[nodiscard]]
		bool ContainsLinePolygonNonEmpty(const Polygon& polygon, const Line& segment, Array<detail::PolygonSegmentEvent>& events)
		{
			const RectF& bounds = polygon.boundingRect();
			return detail::IntersectsPointRectFNonEmpty(segment.start, bounds)
				&& detail::IntersectsPointRectFNonEmpty(segment.end, bounds)
				&& detail::TestPolygonSegment<detail::PolygonSegmentTest::Covered>(detail::GetPolygonRings(polygon), segment, events);
		}

		[[nodiscard]]
		bool ContainsLinePolygonNonEmpty(const Polygon& polygon, const Line& segment) noexcept
		{
			Array<detail::PolygonSegmentEvent> events;
			return ContainsLinePolygonNonEmpty(polygon, segment, events);
		}

		[[nodiscard]]
		bool ContainsPolygonRings(const detail::PolygonRingsView container, const detail::PolygonRingsView target)
		{
			Array<detail::PolygonSegmentEvent> events;
			const int32 requiredDirection = (detail::PolygonRingOrientation(container.outer) * detail::PolygonRingOrientation(target.outer));
			if (detail::AnyPolylineSegment<true>(target.outer, [&](const Line& edge)
				{
					return not detail::TestPolygonSegment<detail::PolygonSegmentTest::Covered>(container, edge, events, requiredDirection);
				}))
			{
				return false;
			}
			// The target's outer boundary is covered. Only container holes can
			// exclude more area; target holes are excluded by this interior test.
			for (const auto& hole : container.holes)
			{
				if (detail::AnyPolylineSegment<true>(hole, [&](const Line& edge)
					{
						return detail::TestPolygonSegment<detail::PolygonSegmentTest::InteriorIntersection>(target, edge, events);
					}))
				{
					return false;
				}
			}
			return true;
		}

		[[nodiscard]]
		bool ContainsTrianglePolygonNonEmpty(const Polygon& polygon, const Triangle& triangle) noexcept
		{
			if (not BoundsContainsClosed(polygon.boundingRect(), triangle.boundingRect()))
			{
				return false;
			}
			const std::array<Vec2, 3> outer{ triangle.p0, triangle.p1, triangle.p2 };
			return ContainsPolygonRings(detail::GetPolygonRings(polygon), { outer, {} });
		}

		[[nodiscard]]
		bool SameEllipse(const Ellipse& a, const Ellipse& b) noexcept
		{
			return (a.center == b.center) && (a.axes == b.axes);
		}

		[[nodiscard]]
		bool SameSuperEllipse(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			return (a.center == b.center)
				&& (a.axes == b.axes)
				&& (a.n == b.n);
		}

		[[nodiscard]]
		bool SameRoundRect(const RoundRect& a, const RoundRect& b) noexcept
		{
			return (a.rect.pos == b.rect.pos)
				&& (a.rect.size == b.rect.size)
				&& (a.r == b.r);
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Contains(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const Rect& a, const Point& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Vec2& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Line& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const LineString& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Rect& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const RectF& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Circle& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Ellipse& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const SuperEllipse& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Triangle& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Quad& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const RoundRect& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const Polygon& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		bool Contains(const Rect& a, const MultiPolygon& b) noexcept
		{
			return Contains(RectF{ a }, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const RectF& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const RectF& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const RectF& a, const Line& b) noexcept
		{
			return (not IsEmpty(a))
				&& Contains(a, b.start)
				&& Contains(a, b.end);
		}

		bool Contains(const RectF& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const RectF& a, const Rect& b) noexcept
		{
			return Contains(a, RectF{ b });
		}

		bool Contains(const RectF& a, const RectF& b) noexcept
		{
			return (not IsEmpty(a)) && (not IsEmpty(b))
				&& BoundsContainsClosed(a, b);
		}

		bool Contains(const RectF& a, const Circle& b) noexcept
		{
			return (not IsEmpty(a)) && (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const Ellipse& b) noexcept
		{
			return (not IsEmpty(a)) && (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const SuperEllipse& b) noexcept
		{
			return (not IsEmpty(a)) && (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const Triangle& b) noexcept
		{
			return (not IsEmpty(a)) && BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const Quad& b) noexcept
		{
			return (not IsEmpty(a)) && BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const RoundRect& b) noexcept
		{
			return (not IsEmpty(a)) && (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.rect);
		}

		bool Contains(const RectF& a, const Polygon& b) noexcept
		{
			return (not IsEmpty(a)) && (not b.isEmpty())
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const Circle& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Circle& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Circle& a, const Line& b) noexcept
		{
			return Contains(a, b.start) && Contains(a, b.end);
		}

		bool Contains(const Circle& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const Circle& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const Circle& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
		}

		bool Contains(const Circle& a, const Circle& b) noexcept
		{
			if (IsEmpty(a) || IsEmpty(b))
			{
				return false;
			}

			if (a.r < b.r)
			{
				return false;
			}

			const double remainingRadius = (a.r - b.r);
			return (a.center.distanceFromSq(b.center)
				<= (remainingRadius * remainingRadius));
		}

		bool Contains(const Circle& a, const Ellipse& b) noexcept
		{
			if (b.axes.x == b.axes.y)
			{
				return Contains(a, Circle{ b.center, b.axes.x });
			}

			return ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const Circle& a, const SuperEllipse& b) noexcept
		{
			return ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const Circle& a, const Triangle& b) noexcept
		{
			return Contains(a, b.p0)
				&& Contains(a, b.p1)
				&& Contains(a, b.p2);
		}

		bool Contains(const Circle& a, const Quad& b) noexcept
		{
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const Circle& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const Circle& a, const Polygon& b) noexcept
		{
			return ContainsPolygonByVertices(a, b);
		}

		bool Contains(const Circle& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const Ellipse& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Ellipse& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Ellipse& a, const Line& b) noexcept
		{
			return Contains(a, b.start) && Contains(a, b.end);
		}

		bool Contains(const Ellipse& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const Ellipse& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const Ellipse& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
		}

		bool Contains(const Ellipse& a, const Circle& b) noexcept
		{
			if (a.axes.x == a.axes.y)
			{
				return Contains(Circle{ a.center, a.axes.x }, b);
			}

			return ContainsCircleByApproximation(a, b);
		}

		bool Contains(const Ellipse& a, const Ellipse& b) noexcept
		{
			if (IsEmpty(a) || IsEmpty(b))
			{
				return false;
			}

			return SameEllipse(a, b)
				|| ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const Ellipse& a, const SuperEllipse& b) noexcept
		{
			return ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const Ellipse& a, const Triangle& b) noexcept
		{
			return Contains(a, b.p0)
				&& Contains(a, b.p1)
				&& Contains(a, b.p2);
		}

		bool Contains(const Ellipse& a, const Quad& b) noexcept
		{
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const Ellipse& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const Ellipse& a, const Polygon& b) noexcept
		{
			return ContainsPolygonByVertices(a, b);
		}

		bool Contains(const Ellipse& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const SuperEllipse& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const SuperEllipse& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const SuperEllipse& a, const Line& b) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(a);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind) || (1.0 <= a.n))
			{
				return Contains(a, b.start) && Contains(a, b.end);
			}

			return ContainsLineNonConvexSuperEllipse(a, b);
		}

		bool Contains(const SuperEllipse& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const SuperEllipse& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const SuperEllipse& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
		}

		bool Contains(const SuperEllipse& a, const Circle& b) noexcept
		{
			return ContainsCircleByApproximation(a, b);
		}

		bool Contains(const SuperEllipse& a, const Ellipse& b) noexcept
		{
			return ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			if (IsEmpty(a) || IsEmpty(b))
			{
				return false;
			}

			return SameSuperEllipse(a, b)
				|| ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const SuperEllipse& a, const Triangle& b) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(a);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind) || (1.0 <= a.n))
			{
				return Contains(a, b.p0)
					&& Contains(a, b.p1)
					&& Contains(a, b.p2);
			}

			return ContainsTriangleNonConvexSuperEllipse(a, b);
		}

		bool Contains(const SuperEllipse& a, const Quad& b) noexcept
		{
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const SuperEllipse& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const SuperEllipse& a, const Polygon& b) noexcept
		{
			if (1.0 <= a.n)
			{
				return ContainsPolygonByVertices(a, b);
			}
			return (not b.isEmpty()) && not detail::AnyPolylineSegment<true>(b.outer(),
				[&](const Line& edge) { return not Contains(a, edge); });
		}

		bool Contains(const SuperEllipse& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const Triangle& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Triangle& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Triangle& a, const Line& b) noexcept
		{
			return Contains(a, b.start) && Contains(a, b.end);
		}

		bool Contains(const Triangle& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const Triangle& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const Triangle& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
		}

		bool Contains(const Triangle& a, const Circle& b) noexcept
		{
			return ContainsCircleByApproximation(a, b);
		}

		bool Contains(const Triangle& a, const Ellipse& b) noexcept
		{
			return ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const Triangle& a, const SuperEllipse& b) noexcept
		{
			return ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const Triangle& a, const Triangle& b) noexcept
		{
			return Contains(a, b.p0)
				&& Contains(a, b.p1)
				&& Contains(a, b.p2);
		}

		bool Contains(const Triangle& a, const Quad& b) noexcept
		{
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const Triangle& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const Triangle& a, const Polygon& b) noexcept
		{
			return ContainsPolygonByVertices(a, b);
		}

		bool Contains(const Triangle& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const Quad& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Quad& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Quad& a, const Line& b) noexcept
		{
			return Contains(a, b.start) && Contains(a, b.end);
		}

		bool Contains(const Quad& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const Quad& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const Quad& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
		}

		bool Contains(const Quad& a, const Circle& b) noexcept
		{
			return ContainsCircleByApproximation(a, b);
		}

		bool Contains(const Quad& a, const Ellipse& b) noexcept
		{
			return ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const Quad& a, const SuperEllipse& b) noexcept
		{
			return ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const Quad& a, const Triangle& b) noexcept
		{
			return Contains(a, b.p0)
				&& Contains(a, b.p1)
				&& Contains(a, b.p2);
		}

		bool Contains(const Quad& a, const Quad& b) noexcept
		{
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const Quad& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const Quad& a, const Polygon& b) noexcept
		{
			if (b.isEmpty())
			{
				return false;
			}
			const std::array<Vec2, 4> outer{ a.p0, a.p1, a.p2, a.p3 };
			return ContainsPolygonRings({ outer, {} }, detail::GetPolygonRings(b));
		}

		bool Contains(const Quad& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const RoundRect& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const RoundRect& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const RoundRect& a, const Line& b) noexcept
		{
			return Contains(a, b.start) && Contains(a, b.end);
		}

		bool Contains(const RoundRect& a, const LineString& b) noexcept
		{
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const RoundRect& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const RoundRect& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
		}

		bool Contains(const RoundRect& a, const Circle& b) noexcept
		{
			return ContainsCircleByApproximation(a, b);
		}

		bool Contains(const RoundRect& a, const Ellipse& b) noexcept
		{
			return ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const RoundRect& a, const SuperEllipse& b) noexcept
		{
			return ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const RoundRect& a, const Triangle& b) noexcept
		{
			return Contains(a, b.p0)
				&& Contains(a, b.p1)
				&& Contains(a, b.p2);
		}

		bool Contains(const RoundRect& a, const Quad& b) noexcept
		{
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const RoundRect& a, const RoundRect& b) noexcept
		{
			if (IsEmpty(a) || IsEmpty(b))
			{
				return false;
			}

			return SameRoundRect(a, b)
				|| ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const RoundRect& a, const Polygon& b) noexcept
		{
			return ContainsPolygonByVertices(a, b);
		}

		bool Contains(const RoundRect& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const Polygon& a, const Point& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Polygon& a, const Vec2& b) noexcept
		{
			return Intersects(b, a);
		}

		bool Contains(const Polygon& a, const Line& b) noexcept
		{
			return (not a.isEmpty())
				&& ContainsLinePolygonNonEmpty(a, b);
		}

		bool Contains(const Polygon& a, const LineString& b) noexcept
		{
			if (a.isEmpty() || b.isEmpty())
			{
				return false;
			}
			if (b.size() == 1)
			{
				return Contains(a, b.front());
			}
			Array<detail::PolygonSegmentEvent> events;
			return not detail::AnyPolylineSegment<false>(b, [&](const Line& edge)
				{
					return not ContainsLinePolygonNonEmpty(a, edge, events);
				});
		}

		bool Contains(const Polygon& a, const Rect& b) noexcept
		{
			return Contains(a, RectF{ b });
		}

		bool Contains(const Polygon& a, const RectF& b) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(b);
			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}
			if (detail::IsGeometry2DSegment(kind))
			{
				return Contains(a, detail::GetGeometry2DDegenerateSegment(b, kind));
			}
			return Contains(a, b.asQuad());
		}

		bool Contains(const Polygon& a, const Circle& b) noexcept
		{
			return ContainsCircleByApproximation(a, b);
		}

		bool Contains(const Polygon& a, const Ellipse& b) noexcept
		{
			return ContainsEllipseByApproximation(a, b);
		}

		bool Contains(const Polygon& a, const SuperEllipse& b) noexcept
		{
			return ContainsSuperEllipseByApproximation(a, b);
		}

		bool Contains(const Polygon& a, const Triangle& b) noexcept
		{
			return (not a.isEmpty())
				&& ContainsTrianglePolygonNonEmpty(a, b);
		}

		bool Contains(const Polygon& a, const Quad& b) noexcept
		{
			if (a.isEmpty() || not BoundsContainsClosed(a.boundingRect(), b.boundingRect()))
			{
				return false;
			}
			const std::array<Vec2, 4> outer{ b.p0, b.p1, b.p2, b.p3 };
			return ContainsPolygonRings(detail::GetPolygonRings(a), { outer, {} });
		}

		bool Contains(const Polygon& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const Polygon& a, const Polygon& b) noexcept
		{
			return (not a.isEmpty()) && (not b.isEmpty())
				&& BoundsContainsClosed(a.boundingRect(), b.boundingRect())
				&& ContainsPolygonRings(detail::GetPolygonRings(a), detail::GetPolygonRings(b));
		}

		bool Contains(const Polygon& a, const MultiPolygon& b) noexcept
		{
			return ContainsMultiPolygonByMembers(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Contains(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		bool Contains(const MultiPolygon& a, const Point& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Vec2& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Line& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const LineString& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Rect& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const RectF& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Circle& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Ellipse& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const SuperEllipse& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Triangle& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Quad& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const RoundRect& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const Polygon& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}

		bool Contains(const MultiPolygon& a, const MultiPolygon& b) noexcept
		{
			return ContainsBySingleMultiPolygonMember(a, b);
		}
	}
}
