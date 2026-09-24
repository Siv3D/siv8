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

		[[nodiscard]]
		bool IsCircularRoundRect(const RoundRect& roundRect) noexcept
		{
			return (roundRect.rect.w == roundRect.rect.h)
				&& ((roundRect.rect.w * 0.5) <= roundRect.r);
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
			if (ellipse.axes.x == ellipse.axes.y)
			{
				return Geometry2D::Contains(container, Circle{ ellipse.center, ellipse.axes.x });
			}

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
			if (superEllipse.n == 2.0)
			{
				return Geometry2D::Contains(container, Ellipse{ superEllipse.center, superEllipse.axes });
			}

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

			auto Diamond = [&](const Vec2& axes) noexcept
			{
				return Quad{
					{ (superEllipse.x + axes.x), superEllipse.y },
					{ superEllipse.x, (superEllipse.y + axes.y) },
					{ (superEllipse.x - axes.x), superEllipse.y },
					{ superEllipse.x, (superEllipse.y - axes.y) } };
			};
			if (superEllipse.n < 1.0)
			{
				// The four tips form the convex hull. This is exact for convex
				// containers; testing the whole hull also respects polygon holes.
				return Geometry2D::Contains(container, Diamond(superEllipse.axes));
			}

			// For n >= 1, |x/a| + |y/b| <= 2^(1-1/n). For n <= 2 this
			// expanded diamond is tight enough to cheaply establish containment.
			// Failure still needs the finer support polygon, especially for a
			// concave container. No false result is inferred from this bound.
			if ((superEllipse.n <= 2.0) && Geometry2D::Contains(container,
				Diamond(superEllipse.axes * std::exp2(1.0 - 1.0 / superEllipse.n))))
			{
				return true;
			}

			if (superEllipse.n == 1.0)
			{
				return ContainsConvexSupportShape(container, superEllipse.center,
					[&](const Vec2& normal) noexcept
					{
						return (superEllipse.center.dot(normal) + Max(
							(superEllipse.a * Abs(normal.x)), (superEllipse.b * Abs(normal.y))));
					});
			}

			const double q = (superEllipse.n / (superEllipse.n - 1.0));
			const double inverseQ = (1.0 / q);
			return ContainsConvexSupportShape(container, superEllipse.center,
				[&](const Vec2& normal) noexcept
				{
					const double x = (superEllipse.a * Abs(normal.x)), y = (superEllipse.b * Abs(normal.y));
					const double scale = Max(x, y);
					// Factoring out the larger term keeps the powered base <= 1,
					// including ordinary-sized shapes with n close to 1 (large q).
					const double radialSupport = (scale * std::pow((1.0 + std::pow((Min(x, y) / scale), q)), inverseQ));
					return (superEllipse.center.dot(normal) + radialSupport);
				});
		}

		template <class Container>
		[[nodiscard]]
		bool ContainsRoundRectByApproximation(
			const Container& container, const RoundRect& roundRect) noexcept
		{
			if (IsCircularRoundRect(roundRect))
			{
				return Geometry2D::Contains(container, Circle{ roundRect.rect.center(), (roundRect.rect.w * 0.5) });
			}

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

		template <class Container, class Shape>
		[[nodiscard]]
		bool ContainsCurvedShape(const Container& container, const Shape& shape) noexcept
		{
			// Reduce the container before approximating the target. Each delegation
			// removes a representation: SuperEllipse -> Ellipse -> Circle, RoundRect -> Circle.
			if constexpr (std::is_same_v<Container, Ellipse>)
			{
				if (container.axes.x == container.axes.y)
				{
					return Geometry2D::Contains(Circle{ container.center, container.axes.x }, shape);
				}
			}
			else if constexpr (std::is_same_v<Container, SuperEllipse>)
			{
				if (container.n == 2.0)
				{
					return Geometry2D::Contains(Ellipse{ container.center, container.axes }, shape);
				}
			}
			else if constexpr (std::is_same_v<Container, RoundRect>)
			{
				if (IsCircularRoundRect(container))
				{
					return Geometry2D::Contains(Circle{ container.rect.center(), (container.rect.w * 0.5) }, shape);
				}
			}

			if constexpr (std::is_same_v<Shape, Circle>)
			{
				return ContainsCircleByApproximation(container, shape);
			}
			else if constexpr (std::is_same_v<Shape, Ellipse>)
			{
				return ContainsEllipseByApproximation(container, shape);
			}
			else if constexpr (std::is_same_v<Shape, SuperEllipse>)
			{
				return ContainsSuperEllipseByApproximation(container, shape);
			}
			else
			{
				return ContainsRoundRectByApproximation(container, shape);
			}
		}

		// Endpoints are already contained; coordinates are normalized by the axes.
		[[nodiscard]]
		bool ContainsNonConvexSuperEllipseSegmentInterior(Vec2 a, Vec2 b, const double n) noexcept
		{
			if ((b.x < a.x) || ((b.x == a.x) && (b.y < a.y)))
			{
				std::swap(a, b);
			}

			Vec2 d = (b - a);

			if (Abs(d.x) < Abs(d.y))
			{
				std::swap(a.x, a.y);
				std::swap(b.x, b.y);
				std::swap(d.x, d.y);
			}

			if (d.y == 0.0)
			{
				return true;
			}

			// For 0 < n < 1, axis crossings are minima. The only possible interior
			// maximum satisfies y = -sign(slope) * |slope|^(1 / (1 - n)) * x.
			const double slope = (d.y / d.x);
			const double ratio = std::copysign(std::pow(Abs(slope), (1.0 / (1.0 - n))), slope);
			const double x = ((slope * a.x - a.y) / (slope + ratio));

			if ((x <= Min(a.x, b.x)) || (Max(a.x, b.x) <= x))
			{
				return true;
			}

			// Use the stationary relation to avoid cancellation near an axis.
			const double y = (-ratio * x);
			return ((std::pow(Abs(x), n) + std::pow(Abs(y), n)) <= (1.0 + 64.0 * DoubleEpsilon));
		}

		template <bool CloseRing, size_t N>
		[[nodiscard]]
		bool ContainsSuperEllipsePolyline(const SuperEllipse& superEllipse, std::array<Vec2, N> points) noexcept
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(superEllipse);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return false;
			}

			if (detail::IsGeometry2DSegment(kind) || (1.0 <= superEllipse.n))
			{
				for (const Vec2& point : points)
				{
					if (not Geometry2D::Contains(superEllipse, point))
					{
						return false;
					}
				}

				return true;
			}

			Vec2 bounds{ 0, 0 };

			for (Vec2& point : points)
			{
				point = ((point - superEllipse.center) / superEllipse.axes);

				if constexpr (CloseRing)
				{
					bounds.x = Max(bounds.x, Abs(point.x));
					bounds.y = Max(bounds.y, Abs(point.y));
				}
			}

			if constexpr (CloseRing)
			{
				if ((1.0 < bounds.x) || (1.0 < bounds.y))
				{
					return false;
				}

				// If the bounding box is contained, no edge maximum needs evaluation.
				if ((std::pow(bounds.x, superEllipse.n) + std::pow(bounds.y, superEllipse.n)) <= 1.0)
				{
					return true;
				}
			}

			for (const Vec2& point : points)
			{
				if constexpr (not CloseRing)
				{
					if ((1.0 < Abs(point.x)) || (1.0 < Abs(point.y)))
					{
						return false;
					}
				}

				if (1.0 < (std::pow(Abs(point.x), superEllipse.n) + std::pow(Abs(point.y), superEllipse.n)))
				{
					return false;
				}
			}

			for (size_t i = 1; i < N; ++i)
			{
				if (not ContainsNonConvexSuperEllipseSegmentInterior(points[i - 1], points[i], superEllipse.n))
				{
					return false;
				}
			}

			// Any exterior point has a ray to infinity outside the superellipse,
			// so a closed boundary inside it cannot enclose an exterior point.
			if constexpr (CloseRing)
			{
				return ContainsNonConvexSuperEllipseSegmentInterior(points.back(), points.front(), superEllipse.n);
			}
			else
			{
				return true;
			}
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
			return ContainsCurvedShape(a, b);
		}

		bool Contains(const Circle& a, const SuperEllipse& b) noexcept
		{
			return ContainsCurvedShape(a, b);
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
			return ContainsCurvedShape(a, b);
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
			return ContainsCurvedShape(a, b);
		}

		bool Contains(const Ellipse& a, const Ellipse& b) noexcept
		{
			if (IsEmpty(a) || IsEmpty(b))
			{
				return false;
			}

			return SameEllipse(a, b)
				|| ContainsCurvedShape(a, b);
		}

		bool Contains(const Ellipse& a, const SuperEllipse& b) noexcept
		{
			return ContainsCurvedShape(a, b);
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
			return ContainsCurvedShape(a, b);
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
			return ContainsSuperEllipsePolyline<false>(a, std::array{ b.start, b.end });
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
			return ContainsCurvedShape(a, b);
		}

		bool Contains(const SuperEllipse& a, const Ellipse& b) noexcept
		{
			return ContainsCurvedShape(a, b);
		}

		bool Contains(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			if (IsEmpty(a) || IsEmpty(b))
			{
				return false;
			}

			return SameSuperEllipse(a, b)
				|| ContainsCurvedShape(a, b);
		}

		bool Contains(const SuperEllipse& a, const Triangle& b) noexcept
		{
			return ContainsSuperEllipsePolyline<true>(a, std::array{ b.p0, b.p1, b.p2 });
		}

		bool Contains(const SuperEllipse& a, const Quad& b) noexcept
		{
			return ContainsSuperEllipsePolyline<true>(a, std::array{ b.p0, b.p1, b.p2, b.p3 });
		}

		bool Contains(const SuperEllipse& a, const RoundRect& b) noexcept
		{
			return ContainsCurvedShape(a, b);
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
			return ContainsCurvedShape(a, b);
		}

		bool Contains(const RoundRect& a, const Ellipse& b) noexcept
		{
			return ContainsCurvedShape(a, b);
		}

		bool Contains(const RoundRect& a, const SuperEllipse& b) noexcept
		{
			return ContainsCurvedShape(a, b);
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
				|| ContainsCurvedShape(a, b);
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
