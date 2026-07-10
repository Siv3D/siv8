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

# include <variant>
# include <Siv3D/ListUtility.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/IntersectsAt.hpp>

namespace s3d
{
	namespace
	{
		inline constexpr double RootTolerance = 1.0e-11;
		inline constexpr double PointMergeTolerance = 1.0e-9;
		inline constexpr int32 CurvedRootSamples = 512;
		inline constexpr int32 BezierPairSegments = 96;

		enum class ArcRegion : uint8
		{
			Full,
			TopLeft,
			TopRight,
			BottomRight,
			BottomLeft,
		};

		struct CircleArc
		{
			Circle circle;
			ArcRegion region = ArcRegion::Full;
		};

		using BoundaryPiece = std::variant<Line, CircleArc, Ellipse, SuperEllipse, Bezier2, Bezier3>;

		struct IntersectionAccumulator
		{
			Array<Vec2> points;
			Array<BoundaryPiece> positiveDimensionalComponents;
		};

		[[nodiscard]]
		constexpr double Square(const double x) noexcept
		{
			return (x * x);
		}

		[[nodiscard]]
		bool NearlyEqualCoordinate(const double a, const double b) noexcept
		{
			const double scale = Max({ Abs(a), Abs(b), 1.0 });
			return (Abs(a - b) <= (PointMergeTolerance * scale));
		}

		[[nodiscard]]
		bool NearlyEqualPoint(const Vec2& a, const Vec2& b) noexcept
		{
			return NearlyEqualCoordinate(a.x, b.x)
				&& NearlyEqualCoordinate(a.y, b.y);
		}

		[[nodiscard]]
		bool IsFinitePoint(const Vec2& p) noexcept
		{
			return std::isfinite(p.x) && std::isfinite(p.y);
		}

		void AppendPoint(IntersectionAccumulator& accumulator, const Vec2& p)
		{
			if (IsFinitePoint(p))
			{
				accumulator.points.push_back(p);
			}
		}

		[[nodiscard]]
		constexpr bool BetweenClosed(const double a, const double x, const double b) noexcept
		{
			return (a <= b)
				? ((a <= x) && (x <= b))
				: ((b <= x) && (x <= a));
		}

		[[nodiscard]]
		bool PointOnLine(const Vec2& p, const Line& line) noexcept
		{
			const Vec2 d = (line.end - line.start);
			const Vec2 v = (p - line.start);
			const double lengthSq = d.dot(d);

			if (lengthSq == 0.0)
			{
				return NearlyEqualPoint(p, line.start);
			}

			const double cross = d.cross(v);
			const double scale = Max({ std::sqrt(lengthSq), Abs(v.x), Abs(v.y), 1.0 });

			if (Abs(cross) > (PointMergeTolerance * scale * scale))
			{
				return false;
			}

			const double dot = v.dot(d);
			const double tolerance = (PointMergeTolerance * Max(lengthSq, 1.0));
			return ((-tolerance <= dot) && (dot <= (lengthSq + tolerance)));
		}

		[[nodiscard]]
		bool ArcContainsPoint(const CircleArc& arc, const Vec2& p) noexcept
		{
			if (arc.region == ArcRegion::Full)
			{
				return true;
			}

			const double tolerance = (PointMergeTolerance
				* Max({ Abs(p.x), Abs(p.y), Abs(arc.circle.center.x), Abs(arc.circle.center.y), arc.circle.r, 1.0 }));
			const Vec2 c = arc.circle.center;

			switch (arc.region)
			{
			case ArcRegion::TopLeft:
				return ((p.x <= (c.x + tolerance)) && (p.y <= (c.y + tolerance)));
			case ArcRegion::TopRight:
				return (((c.x - tolerance) <= p.x) && (p.y <= (c.y + tolerance)));
			case ArcRegion::BottomRight:
				return (((c.x - tolerance) <= p.x) && ((c.y - tolerance) <= p.y));
			case ArcRegion::BottomLeft:
				return ((p.x <= (c.x + tolerance)) && ((c.y - tolerance) <= p.y));
			default:
				return true;
			}
		}

		[[nodiscard]]
		bool PointOnCircleArc(const Vec2& p, const CircleArc& arc) noexcept
		{
			const Vec2 v = (p - arc.circle.center);
			const double radiusSq = Square(arc.circle.r);
			const double scale = Max(radiusSq, 1.0);
			return (Abs(v.dot(v) - radiusSq) <= (PointMergeTolerance * scale))
				&& ArcContainsPoint(arc, p);
		}

		[[nodiscard]]
		bool PointOnEllipseBoundary(const Vec2& p, const Ellipse& ellipse) noexcept
		{
			const double x = ((p.x - ellipse.center.x) / ellipse.axes.x);
			const double y = ((p.y - ellipse.center.y) / ellipse.axes.y);
			return (Abs((x * x + y * y) - 1.0) <= PointMergeTolerance);
		}

		[[nodiscard]]
		bool PointOnSuperEllipseBoundary(const Vec2& p, const SuperEllipse& superEllipse) noexcept
		{
			const double x = Abs((p.x - superEllipse.center.x) / superEllipse.axes.x);
			const double y = Abs((p.y - superEllipse.center.y) / superEllipse.axes.y);
			const double value = (std::pow(x, superEllipse.n) + std::pow(y, superEllipse.n));
			return (Abs(value - 1.0) <= (PointMergeTolerance * 8.0));
		}

		[[nodiscard]]
		bool SameCircle(const Circle& a, const Circle& b) noexcept
		{
			return (a.center == b.center) && (a.r == b.r);
		}

		[[nodiscard]]
		bool SameEllipse(const Ellipse& a, const Ellipse& b) noexcept
		{
			return (a.center == b.center) && (a.axes == b.axes);
		}

		[[nodiscard]]
		bool SameSuperEllipse(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			return (a.center == b.center) && (a.axes == b.axes) && (a.n == b.n);
		}

		[[nodiscard]]
		bool SameBezier(const Bezier2& a, const Bezier2& b) noexcept
		{
			return (((a.p0 == b.p0) && (a.p1 == b.p1) && (a.p2 == b.p2))
				|| ((a.p0 == b.p2) && (a.p1 == b.p1) && (a.p2 == b.p0)));
		}

		[[nodiscard]]
		bool SameBezier(const Bezier3& a, const Bezier3& b) noexcept
		{
			return (((a.p0 == b.p0) && (a.p1 == b.p1) && (a.p2 == b.p2) && (a.p3 == b.p3))
				|| ((a.p0 == b.p3) && (a.p1 == b.p2) && (a.p2 == b.p1) && (a.p3 == b.p0)));
		}

		[[nodiscard]]
		bool PointOnPositiveDimensionalComponent(const Vec2& p, const BoundaryPiece& piece)
		{
			return std::visit([&](const auto& component)
			{
				using T = std::decay_t<decltype(component)>;

				if constexpr (std::is_same_v<T, Line>)
				{
					return PointOnLine(p, component);
				}
				else if constexpr (std::is_same_v<T, CircleArc>)
				{
					return PointOnCircleArc(p, component);
				}
				else if constexpr (std::is_same_v<T, Ellipse>)
				{
					return PointOnEllipseBoundary(p, component);
				}
				else if constexpr (std::is_same_v<T, SuperEllipse>)
				{
					return PointOnSuperEllipseBoundary(p, component);
				}
				else
				{
					return Geometry2D::Intersects(p, component);
				}
			}, piece);
		}

		[[nodiscard]]
		Array<Vec2> FinalizePoints(IntersectionAccumulator&& accumulator)
		{
			Array<Vec2> result;
			result.reserve(accumulator.points.size());

			for (const Vec2& point : accumulator.points)
			{
				bool belongsToPositiveDimensionalComponent = false;

				for (const auto& component : accumulator.positiveDimensionalComponents)
				{
					if (PointOnPositiveDimensionalComponent(point, component))
					{
						belongsToPositiveDimensionalComponent = true;
						break;
					}
				}

				if (belongsToPositiveDimensionalComponent)
				{
					continue;
				}

				bool duplicate = false;

				for (const Vec2& existing : result)
				{
					if (NearlyEqualPoint(point, existing))
					{
						duplicate = true;
						break;
					}
				}

				if (not duplicate)
				{
					result.push_back(point);
				}
			}

			return result;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Point& shape, Vec2& point) noexcept
		{
			point = Vec2{ shape };
			return true;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Vec2& shape, Vec2& point) noexcept
		{
			point = shape;
			return true;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Line& shape, Vec2& point) noexcept
		{
			if (shape.start == shape.end)
			{
				point = shape.start;
				return true;
			}

			return false;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const LineString& shape, Vec2& point) noexcept
		{
			if (shape.isEmpty())
			{
				return false;
			}

			point = shape.front();

			for (const Vec2& p : shape)
			{
				if (p != point)
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Bezier2& shape, Vec2& point) noexcept
		{
			if ((shape.p0 == shape.p1) && (shape.p1 == shape.p2))
			{
				point = shape.p0;
				return true;
			}

			return false;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Bezier3& shape, Vec2& point) noexcept
		{
			if ((shape.p0 == shape.p1) && (shape.p1 == shape.p2) && (shape.p2 == shape.p3))
			{
				point = shape.p0;
				return true;
			}

			return false;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Triangle& shape, Vec2& point) noexcept
		{
			if ((shape.p0 == shape.p1) && (shape.p1 == shape.p2))
			{
				point = shape.p0;
				return true;
			}

			return false;
		}

		[[nodiscard]]
		bool TryGetPointGeometry(const Quad& shape, Vec2& point) noexcept
		{
			if ((shape.p0 == shape.p1) && (shape.p1 == shape.p2) && (shape.p2 == shape.p3))
			{
				point = shape.p0;
				return true;
			}

			return false;
		}

		template <class Shape>
		[[nodiscard]]
		bool TryGetPointGeometry(const Shape&, Vec2&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		Line TriangleDegenerateExtent(const Triangle& triangle) noexcept
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

		void AppendLinePiece(Array<BoundaryPiece>& pieces, const Line& line)
		{
			if (line.start != line.end)
			{
				pieces.emplace_back(line);
			}
		}

		void AppendRingPieces(Array<BoundaryPiece>& pieces, const std::span<const Vec2> ring)
		{
			if (ring.size() < 2)
			{
				return;
			}

			for (size_t i = 0; i < ring.size(); ++i)
			{
				AppendLinePiece(pieces, Line{ ring[i], ring[(i + 1) % ring.size()] });
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Line& shape)
		{
			AppendLinePiece(pieces, shape);
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const LineString& shape)
		{
			if (shape.size() < 2)
			{
				return;
			}

			for (size_t i = 0; i < (shape.size() - 1); ++i)
			{
				AppendLinePiece(pieces, Line{ shape[i], shape[i + 1] });
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Bezier2& shape)
		{
			pieces.emplace_back(shape);
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Bezier3& shape)
		{
			pieces.emplace_back(shape);
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const RectF& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendLinePiece(pieces, detail::GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			const double left = shape.pos.x;
			const double top = shape.pos.y;
			const double right = (left + shape.size.x);
			const double bottom = (top + shape.size.y);
			const Vec2 tl{ left, top };
			const Vec2 tr{ right, top };
			const Vec2 br{ right, bottom };
			const Vec2 bl{ left, bottom };
			AppendLinePiece(pieces, Line{ tl, tr });
			AppendLinePiece(pieces, Line{ tr, br });
			AppendLinePiece(pieces, Line{ br, bl });
			AppendLinePiece(pieces, Line{ bl, tl });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Rect& shape)
		{
			AppendBoundaryPieces(pieces, RectF{ shape });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Circle& shape)
		{
			if (detail::ClassifyGeometry2DSizedShape(shape) == detail::Geometry2DSizedShapeKind::Area)
			{
				pieces.emplace_back(CircleArc{ shape, ArcRegion::Full });
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Ellipse& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendLinePiece(pieces, detail::GetGeometry2DDegenerateSegment(shape, kind));
			}
			else if (kind == detail::Geometry2DSizedShapeKind::Area)
			{
				pieces.emplace_back(shape);
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const SuperEllipse& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendLinePiece(pieces, detail::GetGeometry2DDegenerateSegment(shape, kind));
			}
			else if (kind == detail::Geometry2DSizedShapeKind::Area)
			{
				pieces.emplace_back(shape);
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Triangle& shape)
		{
			const double cross = (shape.p1 - shape.p0).cross(shape.p2 - shape.p0);

			if (cross == 0.0)
			{
				AppendLinePiece(pieces, TriangleDegenerateExtent(shape));
				return;
			}

			AppendLinePiece(pieces, Line{ shape.p0, shape.p1 });
			AppendLinePiece(pieces, Line{ shape.p1, shape.p2 });
			AppendLinePiece(pieces, Line{ shape.p2, shape.p0 });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Quad& shape)
		{
			const double twiceArea = (shape.p0.cross(shape.p1)
				+ shape.p1.cross(shape.p2)
				+ shape.p2.cross(shape.p3)
				+ shape.p3.cross(shape.p0));

			if (twiceArea != 0.0)
			{
				AppendLinePiece(pieces, Line{ shape.p0, shape.p1 });
				AppendLinePiece(pieces, Line{ shape.p1, shape.p2 });
				AppendLinePiece(pieces, Line{ shape.p2, shape.p3 });
				AppendLinePiece(pieces, Line{ shape.p3, shape.p0 });
				return;
			}

			if ((shape.p1 == shape.p2) && (shape.p3 == shape.p0))
			{
				AppendLinePiece(pieces, Line{ shape.p0, shape.p1 });
				return;
			}

			if ((shape.p0 == shape.p1) && (shape.p2 == shape.p3))
			{
				AppendLinePiece(pieces, Line{ shape.p0, shape.p2 });
				return;
			}

			if (shape.p2 == shape.p3)
			{
				AppendBoundaryPieces(pieces, Triangle{ shape.p0, shape.p1, shape.p2 });
				return;
			}

			if (shape.p1 == shape.p2)
			{
				AppendBoundaryPieces(pieces, Triangle{ shape.p0, shape.p1, shape.p3 });
				return;
			}

			if (shape.p0 == shape.p1)
			{
				AppendBoundaryPieces(pieces, Triangle{ shape.p0, shape.p2, shape.p3 });
				return;
			}

			if (shape.p3 == shape.p0)
			{
				AppendBoundaryPieces(pieces, Triangle{ shape.p0, shape.p1, shape.p2 });
				return;
			}

			// Invalid zero-area Quad is outside the semantic contract. Keeping its
			// ordered edges here avoids unsafe assumptions while preserving bounds.
			AppendLinePiece(pieces, Line{ shape.p0, shape.p1 });
			AppendLinePiece(pieces, Line{ shape.p1, shape.p2 });
			AppendLinePiece(pieces, Line{ shape.p2, shape.p3 });
			AppendLinePiece(pieces, Line{ shape.p3, shape.p0 });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const RoundRect& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendLinePiece(pieces, detail::GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			const double r = detail::GetGeometry2DEffectiveRadius(shape);

			if (r == 0.0)
			{
				AppendBoundaryPieces(pieces, shape.rect);
				return;
			}

			const double left = shape.rect.pos.x;
			const double top = shape.rect.pos.y;
			const double right = (left + shape.rect.size.x);
			const double bottom = (top + shape.rect.size.y);
			AppendLinePiece(pieces, Line{ Vec2{ left + r, top }, Vec2{ right - r, top } });
			AppendLinePiece(pieces, Line{ Vec2{ right, top + r }, Vec2{ right, bottom - r } });
			AppendLinePiece(pieces, Line{ Vec2{ right - r, bottom }, Vec2{ left + r, bottom } });
			AppendLinePiece(pieces, Line{ Vec2{ left, bottom - r }, Vec2{ left, top + r } });
			pieces.emplace_back(CircleArc{ Circle{ Vec2{ left + r, top + r }, r }, ArcRegion::TopLeft });
			pieces.emplace_back(CircleArc{ Circle{ Vec2{ right - r, top + r }, r }, ArcRegion::TopRight });
			pieces.emplace_back(CircleArc{ Circle{ Vec2{ right - r, bottom - r }, r }, ArcRegion::BottomRight });
			pieces.emplace_back(CircleArc{ Circle{ Vec2{ left + r, bottom - r }, r }, ArcRegion::BottomLeft });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Polygon& shape)
		{
			if (shape.isEmpty())
			{
				return;
			}

			AppendRingPieces(pieces, shape.outer());

			for (const auto& inner : shape.inners())
			{
				AppendRingPieces(pieces, inner);
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const MultiPolygon& shape)
		{
			for (const auto& polygon : shape)
			{
				AppendBoundaryPieces(pieces, polygon);
			}
		}

		template <class Shape>
		void AppendBoundaryPieces(Array<BoundaryPiece>&, const Shape&)
		{
		}

		void ProcessLineLine(IntersectionAccumulator& accumulator, const Line& a, const Line& b)
		{
			const Vec2 r = (a.end - a.start);
			const Vec2 s = (b.end - b.start);
			const double rLengthSq = r.dot(r);
			const double sLengthSq = s.dot(s);

			if ((rLengthSq == 0.0) || (sLengthSq == 0.0))
			{
				if ((rLengthSq == 0.0) && PointOnLine(a.start, b))
				{
					AppendPoint(accumulator, a.start);
				}
				else if ((sLengthSq == 0.0) && PointOnLine(b.start, a))
				{
					AppendPoint(accumulator, b.start);
				}
				return;
			}

			const Vec2 qMinusP = (b.start - a.start);
			const double denominator = r.cross(s);

			if (denominator != 0.0)
			{
				const double t = (qMinusP.cross(s) / denominator);
				const double u = (qMinusP.cross(r) / denominator);

				if (InRange(t, 0.0, 1.0) && InRange(u, 0.0, 1.0))
				{
					AppendPoint(accumulator, (a.start + r * t));
				}
				return;
			}

			if (qMinusP.cross(r) != 0.0)
			{
				return;
			}

			const bool useX = (Abs(r.x) >= Abs(r.y));
			const double a0 = (useX ? a.start.x : a.start.y);
			const double a1 = (useX ? a.end.x : a.end.y);
			const double b0 = (useX ? b.start.x : b.start.y);
			const double b1 = (useX ? b.end.x : b.end.y);
			const double overlapMin = Max(Min(a0, a1), Min(b0, b1));
			const double overlapMax = Min(Max(a0, a1), Max(b0, b1));

			if (overlapMax < overlapMin)
			{
				return;
			}

			auto PointAtAxis = [&](const double value)
			{
				const double t = ((value - a0) / (a1 - a0));
				return (a.start + r * t);
			};

			if (overlapMin == overlapMax)
			{
				AppendPoint(accumulator, PointAtAxis(overlapMin));
				return;
			}

			accumulator.positiveDimensionalComponents.emplace_back(
				Line{ PointAtAxis(overlapMin), PointAtAxis(overlapMax) });
		}

		void ProcessLineCircleArc(IntersectionAccumulator& accumulator, const Line& line, const CircleArc& arc)
		{
			const Vec2 d = (line.end - line.start);
			const Vec2 f = (line.start - arc.circle.center);
			const double a = d.dot(d);

			if (a == 0.0)
			{
				if (PointOnCircleArc(line.start, arc))
				{
					AppendPoint(accumulator, line.start);
				}
				return;
			}

			const double b = (2.0 * f.dot(d));
			const double c = (f.dot(f) - Square(arc.circle.r));
			double discriminant = std::fma(b, b, (-4.0 * a * c));
			const double scale = (Abs(b * b) + Abs(4.0 * a * c) + 1.0);
			const double tolerance = (RootTolerance * scale);

			if (discriminant < -tolerance)
			{
				return;
			}

			if (discriminant < 0.0)
			{
				discriminant = 0.0;
			}

			const double root = std::sqrt(discriminant);
			const double denominator = (2.0 * a);
			const std::array<double, 2> roots{ ((-b - root) / denominator), ((-b + root) / denominator) };

			for (double t : roots)
			{
				if (InRange(t, -RootTolerance, (1.0 + RootTolerance)))
				{
					t = Clamp(t, 0.0, 1.0);
					const Vec2 point = (line.start + d * t);

					if (ArcContainsPoint(arc, point))
					{
						AppendPoint(accumulator, point);
					}
				}
			}
		}

		void ProcessLineEllipse(IntersectionAccumulator& accumulator, const Line& line, const Ellipse& ellipse)
		{
			const Vec2 p0{
				((line.start.x - ellipse.center.x) / ellipse.axes.x),
				((line.start.y - ellipse.center.y) / ellipse.axes.y)
			};
			const Vec2 p1{
				((line.end.x - ellipse.center.x) / ellipse.axes.x),
				((line.end.y - ellipse.center.y) / ellipse.axes.y)
			};
			const Vec2 d = (p1 - p0);
			const double a = d.dot(d);

			if (a == 0.0)
			{
				if (Abs(p0.dot(p0) - 1.0) <= PointMergeTolerance)
				{
					AppendPoint(accumulator, line.start);
				}
				return;
			}

			const double b = (2.0 * p0.dot(d));
			const double c = (p0.dot(p0) - 1.0);
			double discriminant = std::fma(b, b, (-4.0 * a * c));
			const double tolerance = (RootTolerance * (Abs(b * b) + Abs(4.0 * a * c) + 1.0));

			if (discriminant < -tolerance)
			{
				return;
			}

			if (discriminant < 0.0)
			{
				discriminant = 0.0;
			}

			const double root = std::sqrt(discriminant);
			const double denominator = (2.0 * a);
			const std::array<double, 2> roots{ ((-b - root) / denominator), ((-b + root) / denominator) };

			for (double t : roots)
			{
				if (InRange(t, -RootTolerance, (1.0 + RootTolerance)))
				{
					t = Clamp(t, 0.0, 1.0);
					AppendPoint(accumulator, line.start + (line.end - line.start) * t);
				}
			}
		}

		template <class PointAt, class Function, class Accept>
		[[nodiscard]]
		bool AppendParametricRoots(
			IntersectionAccumulator& accumulator,
			PointAt&& pointAt, Function&& function, Accept&& accept,
			const int32 samples = CurvedRootSamples)
		{
			std::vector<double> values(static_cast<size_t>(samples + 1));
			bool allNearZero = true;

			for (int32 i = 0; i <= samples; ++i)
			{
				const double t = (static_cast<double>(i) / samples);
				values[static_cast<size_t>(i)] = function(t);
				allNearZero = allNearZero && (Abs(values[static_cast<size_t>(i)]) <= RootTolerance);
			}

			if (allNearZero)
			{
				return true;
			}

			std::vector<double> addedParameters;

			auto AddAt = [&](double t)
			{
				t = Clamp(t, 0.0, 1.0);

				// A tangential root can be discovered both as an exact sample and as a
				// refined local minimum. Deduplicate in parameter space before converting
				// to points, so the spatial point tolerance does not have to merge
				// genuinely distinct nearby intersections.
				for (const double existing : addedParameters)
				{
					if (Abs(t - existing) <= 1.0e-7)
					{
						return;
					}
				}

				const Vec2 point = pointAt(t);

				if (accept(point))
				{
					addedParameters.push_back(t);
					AppendPoint(accumulator, point);
				}
			};

			for (int32 i = 0; i < samples; ++i)
			{
				const double t0 = (static_cast<double>(i) / samples);
				const double t1 = (static_cast<double>(i + 1) / samples);
				const double f0 = values[static_cast<size_t>(i)];
				const double f1 = values[static_cast<size_t>(i + 1)];

				if (Abs(f0) <= RootTolerance)
				{
					AddAt(t0);
				}

				if (((f0 < 0.0) && (0.0 < f1)) || ((f1 < 0.0) && (0.0 < f0)))
				{
					double lo = t0;
					double hi = t1;
					double flo = f0;

					for (int32 iteration = 0; iteration < 64; ++iteration)
					{
						const double mid = ((lo + hi) * 0.5);
						const double fm = function(mid);

						if (((flo < 0.0) && (0.0 < fm)) || ((fm < 0.0) && (0.0 < flo)))
						{
							hi = mid;
						}
						else
						{
							lo = mid;
							flo = fm;
						}
					}

					AddAt((lo + hi) * 0.5);
				}
			}

			if (Abs(values.back()) <= RootTolerance)
			{
				AddAt(1.0);
			}

			// Roots of even multiplicity do not change sign. Refine local minima of
			// |f| and accept only minima that converge close to zero.
			for (int32 i = 1; i < samples; ++i)
			{
				const double previous = Abs(values[static_cast<size_t>(i - 1)]);
				const double current = Abs(values[static_cast<size_t>(i)]);
				const double next = Abs(values[static_cast<size_t>(i + 1)]);

				if ((current > 1.0e-4) || (current > previous) || (current > next))
				{
					continue;
				}

				double lo = (static_cast<double>(i - 1) / samples);
				double hi = (static_cast<double>(i + 1) / samples);

				for (int32 iteration = 0; iteration < 48; ++iteration)
				{
					const double m1 = ((2.0 * lo + hi) / 3.0);
					const double m2 = ((lo + 2.0 * hi) / 3.0);

					if (Abs(function(m1)) < Abs(function(m2)))
					{
						hi = m2;
					}
					else
					{
						lo = m1;
					}
				}

				const double t = ((lo + hi) * 0.5);

				if (Abs(function(t)) <= 1.0e-9)
				{
					AddAt(t);
				}
			}

			return false;
		}

		[[nodiscard]]
		Vec2 CircleArcPointAt(const CircleArc& arc, const double t) noexcept
		{
			double startAngle = 0.0;
			double angleLength = Math::TwoPi;

			switch (arc.region)
			{
			case ArcRegion::TopLeft:
				startAngle = Math::Pi;
				angleLength = (Math::Pi * 0.5);
				break;
			case ArcRegion::TopRight:
				startAngle = (Math::Pi * 1.5);
				angleLength = (Math::Pi * 0.5);
				break;
			case ArcRegion::BottomRight:
				startAngle = 0.0;
				angleLength = (Math::Pi * 0.5);
				break;
			case ArcRegion::BottomLeft:
				startAngle = (Math::Pi * 0.5);
				angleLength = (Math::Pi * 0.5);
				break;
			default:
				break;
			}

			const double angle = (startAngle + angleLength * t);
			return (arc.circle.center + Vec2{ std::cos(angle), std::sin(angle) } * arc.circle.r);
		}

		[[nodiscard]]
		Vec2 EllipsePointAt(const Ellipse& ellipse, const double t) noexcept
		{
			const double angle = (Math::TwoPi * t);
			return ellipse.center + Vec2{
				(ellipse.axes.x * std::cos(angle)),
				(ellipse.axes.y * std::sin(angle))
			};
		}

		[[nodiscard]]
		Vec2 SuperEllipsePointAt(const SuperEllipse& superEllipse, const double t) noexcept
		{
			const double angle = (Math::TwoPi * t);
			const double c = std::cos(angle);
			const double s = std::sin(angle);
			const double exponent = (2.0 / superEllipse.n);
			const double x = std::copysign(std::pow(Abs(c), exponent), c);
			const double y = std::copysign(std::pow(Abs(s), exponent), s);
			return superEllipse.center + Vec2{
				(superEllipse.axes.x * x),
				(superEllipse.axes.y * y)
			};
		}

		[[nodiscard]]
		double CircleImplicit(const CircleArc& arc, const Vec2& p) noexcept
		{
			const Vec2 v = (p - arc.circle.center);
			return ((v.dot(v) / Square(arc.circle.r)) - 1.0);
		}

		[[nodiscard]]
		double EllipseImplicit(const Ellipse& ellipse, const Vec2& p) noexcept
		{
			const double x = ((p.x - ellipse.center.x) / ellipse.axes.x);
			const double y = ((p.y - ellipse.center.y) / ellipse.axes.y);
			return (x * x + y * y - 1.0);
		}

		[[nodiscard]]
		double SuperEllipseImplicit(const SuperEllipse& superEllipse, const Vec2& p) noexcept
		{
			const double x = Abs((p.x - superEllipse.center.x) / superEllipse.axes.x);
			const double y = Abs((p.y - superEllipse.center.y) / superEllipse.axes.y);
			return (std::pow(x, superEllipse.n) + std::pow(y, superEllipse.n) - 1.0);
		}

		void ProcessLineSuperEllipse(IntersectionAccumulator& accumulator, const Line& line, const SuperEllipse& superEllipse)
		{
			const Vec2 d = (line.end - line.start);
			if (AppendParametricRoots(accumulator,
				[&](const double t) { return (line.start + d * t); },
				[&](const double t) { return SuperEllipseImplicit(superEllipse, line.start + d * t); },
				[](const Vec2&) { return true; }))
			{
				accumulator.positiveDimensionalComponents.emplace_back(line);
			}
		}

		template <class Bezier>
		void ProcessLineBezier(IntersectionAccumulator& accumulator, const Line& line, const Bezier& bezier)
		{
			const Vec2 d = (line.end - line.start);
			const double length = std::sqrt(d.dot(d));

			if (length == 0.0)
			{
				if (Geometry2D::Intersects(line.start, bezier))
				{
					AppendPoint(accumulator, line.start);
				}
				return;
			}

			if (AppendParametricRoots(accumulator,
				[&](const double t) { return bezier.pointAt(t); },
				[&](const double t)
				{
					return (d.cross(bezier.pointAt(t) - line.start) / Max(length, 1.0));
				},
				[&](const Vec2& point) { return PointOnLine(point, line); }))
			{
				accumulator.positiveDimensionalComponents.emplace_back(bezier);
			}
		}

		void ProcessCircleArcCircleArc(IntersectionAccumulator& accumulator, const CircleArc& a, const CircleArc& b)
		{
			if (SameCircle(a.circle, b.circle))
			{
				if (a.region == b.region)
				{
					accumulator.positiveDimensionalComponents.emplace_back(a);
					return;
				}

				if ((a.region == ArcRegion::Full) || (b.region == ArcRegion::Full))
				{
					accumulator.positiveDimensionalComponents.emplace_back(
						(a.region == ArcRegion::Full) ? b : a);
					return;
				}

				// Quarter-circle pieces on the same supporting circle can meet at
				// one seam point without sharing a positive-dimensional arc.
				const std::array<Vec2, 4> candidates{
					CircleArcPointAt(a, 0.0), CircleArcPointAt(a, 1.0),
					CircleArcPointAt(b, 0.0), CircleArcPointAt(b, 1.0)
				};

				for (const Vec2& point : candidates)
				{
					if (ArcContainsPoint(a, point) && ArcContainsPoint(b, point))
					{
						AppendPoint(accumulator, point);
					}
				}

				return;
			}

			const Vec2 delta = (b.circle.center - a.circle.center);
			const double distanceSq = delta.dot(delta);
			const double distance = std::sqrt(distanceSq);
			const double r0 = a.circle.r;
			const double r1 = b.circle.r;

			if ((distance == 0.0) || ((r0 + r1) < distance) || (distance < Abs(r0 - r1)))
			{
				return;
			}

			const double x = ((distanceSq + r0 * r0 - r1 * r1) / (2.0 * distance));
			double hSq = (r0 * r0 - x * x);
			const double tolerance = (RootTolerance * Max({ r0 * r0, r1 * r1, distanceSq, 1.0 }));

			if (hSq < -tolerance)
			{
				return;
			}

			if (hSq < 0.0)
			{
				hSq = 0.0;
			}

			const Vec2 unit = (delta / distance);
			const Vec2 base = (a.circle.center + unit * x);
			const Vec2 normal{ -unit.y, unit.x };
			const double h = std::sqrt(hSq);
			const std::array<Vec2, 2> points{ (base + normal * h), (base - normal * h) };

			for (const Vec2& point : points)
			{
				if (ArcContainsPoint(a, point) && ArcContainsPoint(b, point))
				{
					AppendPoint(accumulator, point);
				}
			}
		}

		template <class PointAt, class Implicit, class Accept, class OverlapPiece>
		void ProcessCurveImplicit(
			IntersectionAccumulator& accumulator,
			PointAt&& pointAt, Implicit&& implicit, Accept&& accept,
			const OverlapPiece& overlapPiece)
		{
			if (AppendParametricRoots(accumulator,
				std::forward<PointAt>(pointAt),
				[&](const double t) { return implicit(pointAt(t)); },
				std::forward<Accept>(accept)))
			{
				accumulator.positiveDimensionalComponents.emplace_back(overlapPiece);
			}
		}

		template <class BezierA, class BezierB>
		void ProcessBezierBezier(IntersectionAccumulator& accumulator, const BezierA& a, const BezierB& b)
		{
			if constexpr (std::is_same_v<BezierA, BezierB>)
			{
				if (SameBezier(a, b))
				{
					accumulator.positiveDimensionalComponents.emplace_back(a);
					return;
				}
			}

			Vec2 a0 = a.pointAt(0.0);

			for (int32 i = 0; i < BezierPairSegments; ++i)
			{
				const double at = (static_cast<double>(i + 1) / BezierPairSegments);
				const Vec2 a1 = a.pointAt(at);
				Vec2 b0 = b.pointAt(0.0);

				for (int32 j = 0; j < BezierPairSegments; ++j)
				{
					const double bt = (static_cast<double>(j + 1) / BezierPairSegments);
					const Vec2 b1 = b.pointAt(bt);
					ProcessLineLine(accumulator, Line{ a0, a1 }, Line{ b0, b1 });
					b0 = b1;
				}

				a0 = a1;
			}
		}

		template <class A, class B>
		void ProcessPiecePairCanonical(IntersectionAccumulator& accumulator, const A& a, const B& b)
		{
			if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, Line>)
			{
				ProcessLineLine(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, CircleArc>)
			{
				ProcessLineCircleArc(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, Ellipse>)
			{
				ProcessLineEllipse(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, SuperEllipse>)
			{
				ProcessLineSuperEllipse(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, Bezier2>)
			{
				ProcessLineBezier(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, Bezier3>)
			{
				ProcessLineBezier(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, CircleArc> && std::is_same_v<B, CircleArc>)
			{
				ProcessCircleArcCircleArc(accumulator, a, b);
			}
			else if constexpr (std::is_same_v<A, CircleArc> && std::is_same_v<B, Ellipse>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return CircleArcPointAt(a, t); },
					[&](const Vec2& p) { return EllipseImplicit(b, p); },
					[&](const Vec2& p) { return ArcContainsPoint(a, p); }, a);
			}
			else if constexpr (std::is_same_v<A, CircleArc> && std::is_same_v<B, SuperEllipse>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return CircleArcPointAt(a, t); },
					[&](const Vec2& p) { return SuperEllipseImplicit(b, p); },
					[&](const Vec2& p) { return ArcContainsPoint(a, p); }, a);
			}
			else if constexpr (std::is_same_v<A, CircleArc> && std::is_same_v<B, Bezier2>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return b.pointAt(t); },
					[&](const Vec2& p) { return CircleImplicit(a, p); },
					[&](const Vec2& p) { return ArcContainsPoint(a, p); }, b);
			}
			else if constexpr (std::is_same_v<A, CircleArc> && std::is_same_v<B, Bezier3>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return b.pointAt(t); },
					[&](const Vec2& p) { return CircleImplicit(a, p); },
					[&](const Vec2& p) { return ArcContainsPoint(a, p); }, b);
			}
			else if constexpr (std::is_same_v<A, Ellipse> && std::is_same_v<B, Ellipse>)
			{
				if (SameEllipse(a, b))
				{
					accumulator.positiveDimensionalComponents.emplace_back(a);
				}
				else
				{
					ProcessCurveImplicit(accumulator,
						[&](const double t) { return EllipsePointAt(a, t); },
						[&](const Vec2& p) { return EllipseImplicit(b, p); },
						[](const Vec2&) { return true; }, a);
				}
			}
			else if constexpr (std::is_same_v<A, Ellipse> && std::is_same_v<B, SuperEllipse>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return EllipsePointAt(a, t); },
					[&](const Vec2& p) { return SuperEllipseImplicit(b, p); },
					[](const Vec2&) { return true; }, a);
			}
			else if constexpr (std::is_same_v<A, Ellipse> && std::is_same_v<B, Bezier2>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return b.pointAt(t); },
					[&](const Vec2& p) { return EllipseImplicit(a, p); },
					[](const Vec2&) { return true; }, b);
			}
			else if constexpr (std::is_same_v<A, Ellipse> && std::is_same_v<B, Bezier3>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return b.pointAt(t); },
					[&](const Vec2& p) { return EllipseImplicit(a, p); },
					[](const Vec2&) { return true; }, b);
			}
			else if constexpr (std::is_same_v<A, SuperEllipse> && std::is_same_v<B, SuperEllipse>)
			{
				if (SameSuperEllipse(a, b))
				{
					accumulator.positiveDimensionalComponents.emplace_back(a);
				}
				else
				{
					ProcessCurveImplicit(accumulator,
						[&](const double t) { return SuperEllipsePointAt(a, t); },
						[&](const Vec2& p) { return SuperEllipseImplicit(b, p); },
						[](const Vec2&) { return true; }, a);
				}
			}
			else if constexpr (std::is_same_v<A, SuperEllipse> && std::is_same_v<B, Bezier2>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return b.pointAt(t); },
					[&](const Vec2& p) { return SuperEllipseImplicit(a, p); },
					[](const Vec2&) { return true; }, b);
			}
			else if constexpr (std::is_same_v<A, SuperEllipse> && std::is_same_v<B, Bezier3>)
			{
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return b.pointAt(t); },
					[&](const Vec2& p) { return SuperEllipseImplicit(a, p); },
					[](const Vec2&) { return true; }, b);
			}
			else if constexpr ((std::is_same_v<A, Bezier2> || std::is_same_v<A, Bezier3>)
				&& (std::is_same_v<B, Bezier2> || std::is_same_v<B, Bezier3>))
			{
				ProcessBezierBezier(accumulator, a, b);
			}
		}

		template <class T>
		inline constexpr int32 PieceRank =
			std::is_same_v<T, Line> ? 0
			: std::is_same_v<T, CircleArc> ? 1
			: std::is_same_v<T, Ellipse> ? 2
			: std::is_same_v<T, SuperEllipse> ? 3
			: std::is_same_v<T, Bezier2> ? 4
			: 5;

		template <class A, class B>
		void ProcessPiecePair(IntersectionAccumulator& accumulator, const A& a, const B& b)
		{
			if constexpr (PieceRank<B> < PieceRank<A>)
			{
				ProcessPiecePairCanonical(accumulator, b, a);
			}
			else
			{
				ProcessPiecePairCanonical(accumulator, a, b);
			}
		}

		struct ShapeIntersectionData
		{
			Optional<Vec2> pointGeometry;
			Array<BoundaryPiece> boundaryPieces;
		};

		template <class Shape>
		[[nodiscard]]
		ShapeIntersectionData MakeShapeIntersectionData(const Shape& shape)
		{
			ShapeIntersectionData data;
			Vec2 point;

			if (TryGetPointGeometry(shape, point))
			{
				data.pointGeometry = point;
			}
			else
			{
				AppendBoundaryPieces(data.boundaryPieces, shape);
			}

			return data;
		}

		[[nodiscard]]
		Array<Vec2> EnumerateKnownIntersection(
			const ShapeIntersectionData& a, const ShapeIntersectionData& b)
		{
			if (a.pointGeometry)
			{
				return Array<Vec2>{ *a.pointGeometry };
			}

			if (b.pointGeometry)
			{
				return Array<Vec2>{ *b.pointGeometry };
			}

			IntersectionAccumulator accumulator;

			for (const BoundaryPiece& pieceA : a.boundaryPieces)
			{
				for (const BoundaryPiece& pieceB : b.boundaryPieces)
				{
					std::visit([&](const auto& primitiveA, const auto& primitiveB)
					{
						ProcessPiecePair(accumulator, primitiveA, primitiveB);
					}, pieceA, pieceB);
				}
			}

			return FinalizePoints(std::move(accumulator));
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<Array<Vec2>> ComputeIntersectsAt(const ShapeA& a, const ShapeB& b)
		{
			if (not Geometry2D::Intersects(a, b))
			{
				return none;
			}

			return EnumerateKnownIntersection(
				MakeShapeIntersectionData(a), MakeShapeIntersectionData(b));
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Point, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Point& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Vec2& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Line, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Line& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(LineString, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const LineString& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Bezier2, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier2& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Bezier3, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Bezier3& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Rect& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RectF& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Circle& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Ellipse& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const SuperEllipse& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Triangle& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Quad& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const RoundRect& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const Polygon& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	IntersectsAt(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Point& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Vec2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Line& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const LineString& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Bezier2& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Bezier3& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Rect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const RectF& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Circle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Ellipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const SuperEllipse& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Triangle& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Quad& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const RoundRect& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const Polygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

		Optional<Array<Vec2>> IntersectsAt(const MultiPolygon& a, const MultiPolygon& b)
		{
			return ComputeIntersectsAt(a, b);
		}

	}
}
