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

namespace s3d
{
	namespace
	{
		inline constexpr int32 CurvedContainmentSegments = 128;
		inline constexpr int32 SuperEllipseSearchIterations = 64;
		inline constexpr double TwoPi = 6.2831853071795864769252867665590058;
		inline constexpr double ParameterMergeTolerance = 1.0e-11;
		inline constexpr double DoubleEpsilon = 2.2204460492503131e-16;

		[[nodiscard]]
		constexpr double Cross(const Vec2& a, const Vec2& b, const Vec2& c) noexcept
		{
			return ((b - a).cross(c - a));
		}


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
			if (IsEmpty(outer))
			{
				return false;
			}

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
		bool ContainsPolygonByTriangles(const Container& container, const Polygon& polygon) noexcept
		{
			if (polygon.isEmpty())
			{
				return false;
			}

			bool hasTriangle = false;
			const Float2* vertices = polygon.vertices().data();

			for (const auto& index : polygon.indices())
			{
				hasTriangle = true;
				const Triangle triangle{
					Vec2{ vertices[index.i0].x, vertices[index.i0].y },
					Vec2{ vertices[index.i1].x, vertices[index.i1].y },
					Vec2{ vertices[index.i2].x, vertices[index.i2].y }
				};

				if (not Geometry2D::Contains(container, triangle))
				{
					return false;
				}
			}

			return hasTriangle;
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
		bool ClipSegmentToTriangle(
			const Line& segment, const Triangle& triangle,
			double& t0, double& t1) noexcept
		{
			const double orientation = Cross(triangle.p0, triangle.p1, triangle.p2);
			assert(orientation != 0.0);
			const double sign = ((0.0 < orientation) ? 1.0 : -1.0);
			const Vec2 direction = (segment.end - segment.start);
			t0 = 0.0;
			t1 = 1.0;

			auto ClipEdge = [&](const Vec2& a, const Vec2& b) noexcept
			{
				const double q0 = (sign * Cross(a, b, segment.start));
				const double qd = (sign * (b - a).cross(direction));

				if (qd == 0.0)
				{
					return (0.0 <= q0);
				}

				const double t = (-q0 / qd);

				if (0.0 < qd)
				{
					t0 = Max(t0, t);
				}
				else
				{
					t1 = Min(t1, t);
				}

				return (t0 <= t1);
			};

			return ClipEdge(triangle.p0, triangle.p1)
				&& ClipEdge(triangle.p1, triangle.p2)
				&& ClipEdge(triangle.p2, triangle.p0);
		}

		[[nodiscard]]
		bool ContainsLinePolygonNonEmpty(const Polygon& polygon, const Line& segment) noexcept
		{
			if (segment.start == segment.end)
			{
				return Geometry2D::Contains(polygon, segment.start);
			}

			const RectF segmentBounds{
				Min(segment.start.x, segment.end.x),
				Min(segment.start.y, segment.end.y),
				Abs(segment.end.x - segment.start.x),
				Abs(segment.end.y - segment.start.y)
			};

			if (not BoundsContainsClosed(polygon.boundingRect(), segmentBounds))
			{
				return false;
			}

			Array<std::pair<double, double>> intervals;
			intervals.reserve(polygon.indices().size());
			const Float2* vertices = polygon.vertices().data();

			for (const auto& index : polygon.indices())
			{
				const Triangle part{
					Vec2{ vertices[index.i0].x, vertices[index.i0].y },
					Vec2{ vertices[index.i1].x, vertices[index.i1].y },
					Vec2{ vertices[index.i2].x, vertices[index.i2].y }
				};
				double t0 = 0.0;
				double t1 = 0.0;

				if (ClipSegmentToTriangle(segment, part, t0, t1))
				{
					intervals.emplace_back(Max(0.0, t0), Min(1.0, t1));
				}
			}

			if (intervals.isEmpty())
			{
				return false;
			}

			std::sort(intervals.begin(), intervals.end(),
				[](const auto& a, const auto& b) noexcept
				{
					return (a.first < b.first)
						|| ((a.first == b.first) && (a.second < b.second));
				});

			if (ParameterMergeTolerance < intervals.front().first)
			{
				return false;
			}

			double coveredEnd = intervals.front().second;

			for (size_t i = 1; i < intervals.size(); ++i)
			{
				if ((coveredEnd + ParameterMergeTolerance) < intervals[i].first)
				{
					return false;
				}

				coveredEnd = Max(coveredEnd, intervals[i].second);
			}

			return ((1.0 - ParameterMergeTolerance) <= coveredEnd);
		}

		[[nodiscard]]
		Line GetTriangleDegenerateExtent(const Triangle& triangle) noexcept
		{
			const double d01 = triangle.p0.distanceFromSq(triangle.p1);
			const double d12 = triangle.p1.distanceFromSq(triangle.p2);
			const double d20 = triangle.p2.distanceFromSq(triangle.p0);

			if ((d12 <= d01) && (d20 <= d01))
			{
				return Line{ triangle.p0, triangle.p1 };
			}

			if (d20 <= d12)
			{
				return Line{ triangle.p1, triangle.p2 };
			}

			return Line{ triangle.p2, triangle.p0 };
		}

		using ClipBuffer = std::array<Vec2, 8>;

		[[nodiscard]]
		size_t ClipPolygonAgainstEdge(
			const ClipBuffer& input, const size_t inputCount,
			ClipBuffer& output, const Vec2& edgeStart, const Vec2& edgeEnd,
			const double sign) noexcept
		{
			if (inputCount == 0)
			{
				return 0;
			}

			size_t outputCount = 0;
			Vec2 previous = input[inputCount - 1];
			double previousDistance = (sign * Cross(edgeStart, edgeEnd, previous));
			bool previousInside = (0.0 <= previousDistance);

			for (size_t i = 0; i < inputCount; ++i)
			{
				const Vec2 current = input[i];
				const double currentDistance = (sign * Cross(edgeStart, edgeEnd, current));
				const bool currentInside = (0.0 <= currentDistance);

				if (previousInside != currentInside)
				{
					const double denominator = (previousDistance - currentDistance);
					assert(denominator != 0.0);
					const double t = (previousDistance / denominator);
					output[outputCount++] = (previous + (current - previous) * t);
				}

				if (currentInside)
				{
					output[outputCount++] = current;
				}

				previous = current;
				previousDistance = currentDistance;
				previousInside = currentInside;
			}

			return outputCount;
		}

		[[nodiscard]]
		double PolygonArea(const ClipBuffer& points, const size_t count) noexcept
		{
			if (count < 3)
			{
				return 0.0;
			}

			double twiceArea = 0.0;

			for (size_t i = 0; i < count; ++i)
			{
				twiceArea += points[i].cross(points[(i + 1) % count]);
			}

			return (Abs(twiceArea) * 0.5);
		}

		[[nodiscard]]
		double TriangleIntersectionArea(const Triangle& subject, const Triangle& clip) noexcept
		{
			const double orientation = Cross(clip.p0, clip.p1, clip.p2);
			assert(orientation != 0.0);
			const double sign = ((0.0 < orientation) ? 1.0 : -1.0);
			ClipBuffer a{};
			ClipBuffer b{};
			a[0] = subject.p0;
			a[1] = subject.p1;
			a[2] = subject.p2;
			size_t count = 3;

			count = ClipPolygonAgainstEdge(a, count, b, clip.p0, clip.p1, sign);
			count = ClipPolygonAgainstEdge(b, count, a, clip.p1, clip.p2, sign);
			count = ClipPolygonAgainstEdge(a, count, b, clip.p2, clip.p0, sign);
			return PolygonArea(b, count);
		}

		[[nodiscard]]
		bool ContainsTrianglePolygonNonEmpty(
			const Polygon& polygon, const Triangle& triangle) noexcept
		{
			const double twiceArea = Abs(Cross(triangle.p0, triangle.p1, triangle.p2));

			if (twiceArea == 0.0)
			{
				return ContainsLinePolygonNonEmpty(polygon, GetTriangleDegenerateExtent(triangle));
			}

			if (not BoundsContainsClosed(polygon.boundingRect(), triangle.boundingRect()))
			{
				return false;
			}

			if ((not ContainsLinePolygonNonEmpty(polygon, Line{ triangle.p0, triangle.p1 }))
				|| (not ContainsLinePolygonNonEmpty(polygon, Line{ triangle.p1, triangle.p2 }))
				|| (not ContainsLinePolygonNonEmpty(polygon, Line{ triangle.p2, triangle.p0 })))
			{
				return false;
			}

			const double targetArea = (twiceArea * 0.5);
			double coveredArea = 0.0;
			const Float2* vertices = polygon.vertices().data();

			for (const auto& index : polygon.indices())
			{
				const Triangle part{
					Vec2{ vertices[index.i0].x, vertices[index.i0].y },
					Vec2{ vertices[index.i1].x, vertices[index.i1].y },
					Vec2{ vertices[index.i2].x, vertices[index.i2].y }
				};
				coveredArea += TriangleIntersectionArea(triangle, part);
			}

			const RectF bounds = triangle.boundingRect();
			const double coordinateScale = Max({
				Abs(bounds.pos.x), Abs(bounds.pos.y),
				Abs(bounds.pos.x + bounds.size.x),
				Abs(bounds.pos.y + bounds.size.y), 1.0
			});
			const double tolerance = Max(
				targetArea * 1.0e-10,
				256.0 * DoubleEpsilon * coordinateScale * coordinateScale);

			return ((targetArea - coveredArea) <= tolerance);
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
			return (not IsEmpty(b))
				&& BoundsContainsClosed(a, b);
		}

		bool Contains(const RectF& a, const Circle& b) noexcept
		{
			return (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const Ellipse& b) noexcept
		{
			return (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const SuperEllipse& b) noexcept
		{
			return (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const Triangle& b) noexcept
		{
			return BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const Quad& b) noexcept
		{
			return BoundsContainsClosed(a, b.boundingRect());
		}

		bool Contains(const RectF& a, const RoundRect& b) noexcept
		{
			return (not IsEmpty(b))
				&& BoundsContainsClosed(a, b.rect);
		}

		bool Contains(const RectF& a, const Polygon& b) noexcept
		{
			return (not b.isEmpty())
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
			return ContainsPolygonByTriangles(a, b);
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
			return ContainsPolygonByTriangles(a, b);
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
			return ContainsPolygonByTriangles(a, b);
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
			return ContainsPolygonByTriangles(a, b);
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
			return ContainsPolygonByTriangles(a, b);
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
			return ContainsPolygonByTriangles(a, b);
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
			return ContainsLineStringBySegments(a, b);
		}

		bool Contains(const Polygon& a, const Rect& b) noexcept
		{
			return ContainsRectFByDecomposition(a, RectF{ b });
		}

		bool Contains(const Polygon& a, const RectF& b) noexcept
		{
			return ContainsRectFByDecomposition(a, b);
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
			return ContainsQuadByDecomposition(a, b);
		}

		bool Contains(const Polygon& a, const RoundRect& b) noexcept
		{
			return ContainsRoundRectByApproximation(a, b);
		}

		bool Contains(const Polygon& a, const Polygon& b) noexcept
		{
			return ContainsPolygonByTriangles(a, b);
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
