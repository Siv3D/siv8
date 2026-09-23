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
# include <tuple>
# include <Siv3D/ListUtility.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/PolynomialSolver.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/IntersectsAt.hpp>
# include "BezierGeometry.hpp"
# include "EllipseGeometry.hpp"
# include "PolygonGeometry.hpp"

namespace s3d
{
	namespace
	{
		inline constexpr double RootTolerance = 1.0e-11;
		inline constexpr double PointMergeTolerance = 1.0e-9;
		inline constexpr int32 CurvedRootSamples = 512;

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
			// Solver residual and coordinate rounding are separate error sources.
			// A translation must not turn the solver tolerance into a world-space radius.
			return (Abs(a - b) <= Max(PointMergeTolerance, (4.0 * std::numeric_limits<double>::epsilon() * scale)));
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

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Line& shape)
		{
			AppendLinePiece(pieces, shape);
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

		// On either half of an ellipse, (cos, sin) = (side * (1-t^2), 2t) / (1+t^2),
		// -1 <= t <= 1. Substitution into the other ellipse gives a quartic.
		// Its stationary points separate all roots, including a pair of crossings
		// too close together to bracket with fixed angular samples.
		template <class Accept>
		void ProcessEllipseEllipse(IntersectionAccumulator& accumulator,
			const Ellipse& first, const Ellipse& second, Accept&& accept)
		{
			const Ellipse* a = &first;
			const Ellipse* b = &second;
			if (std::tie(b->a, b->b, b->x, b->y) < std::tie(a->a, a->b, a->x, a->y))
			{
				std::swap(a, b);
			}
			const Vec2 delta = ((a->center - b->center) / b->axes);
			const Vec2 axes = (a->axes / b->axes);
			// Retain a contact candidate when classification accepts a roundoff-sized gap.
			// Convert its spatial allowance to the normalized implicit equation.
			const double contactTolerance = (2.0 * detail::EllipseContactTolerance * Max({
				1.0, Abs(delta.x), Abs(delta.y), axes.x, axes.y,
				(Max({ a->a, a->b, b->a, b->b }) / Min(b->a, b->b)) }));

			for (const double side : { -1.0, 1.0 })
			{
				const double sx = (side * axes.x), sy = axes.y;
				const double x0 = (delta.x + sx), x2 = (delta.x - sx);
				const double c1 = (4.0 * delta.y * sy);
				const double c2 = (2.0 * x0 * x2 + 2.0 * Square(delta.y) + 4.0 * Square(sy) - 2.0);
				const double c4 = (Square(x2) + Square(delta.y) - 1.0);
				// Overlap the two parameter domains so a near-seam tangent has an
				// interior stationary point in both searches. Emit only its own half.
				std::array<double, 5> parameters{ -2.0 };
				size_t count = 1;
				for (const double root : Math::SolveCubicEquation((4.0 * c4), (3.0 * c1), (2.0 * c2), c1))
				{
					if ((-2.0 < root) && (root < 2.0))
					{
						parameters[count++] = root;
					}
				}
				parameters[count++] = 2.0;

				auto Value = [&](const double t)
				{
					const double tt = (t * t), denominator = (1.0 + tt);
					const double x = std::fma(x2, tt, x0);
					const double y = std::fma(delta.y, denominator, (2.0 * sy * t));
					return std::fma(x, x, std::fma(y, y, -Square(denominator)));
				};
				auto EndpointValue = [&](const double t)
				{
					const double tt = (t * t), denominator = (1.0 + tt);
					const double x = std::fma(x2, tt, x0);
					const double y = std::fma(delta.y, denominator, (2.0 * sy * t));
					const double scale = (Abs(x) * (Abs(delta.x) * denominator + Abs(sx) * Abs(1.0 - tt))
						+ Abs(y) * (Abs(delta.y) * denominator + Abs(2.0 * sy * t)) + Square(denominator));
					const double value = Value(t);
					const double tolerance = Max((32.0 * std::numeric_limits<double>::epsilon() * scale),
						(contactTolerance * Square(denominator)));
					return (Abs(value) <= tolerance) ? 0.0 : value;
				};
				auto AddAt = [&](double t)
				{
					if ((1.0 + 8.0 * std::numeric_limits<double>::epsilon()) < Abs(t))
					{
						return;
					}
					t = Clamp(t, -1.0, 1.0);
					const double tt = (t * t), denominator = (1.0 + tt);
					const Vec2 point = (a->center + a->axes * Vec2{ (side * (1.0 - tt) / denominator), (2.0 * t / denominator) });
					if (accept(point))
					{
						AppendPoint(accumulator, point);
					}
				};

				double previous = EndpointValue(parameters[0]);
				if (previous == 0.0)
				{
					AddAt(parameters[0]);
				}
				for (size_t i = 1; i < count; ++i)
				{
					const double value = EndpointValue(parameters[i]);
					if (value == 0.0)
					{
						AddAt(parameters[i]);
					}
					else if (((previous < 0.0) && (0.0 < value)) || ((value < 0.0) && (0.0 < previous)))
					{
						double lo = parameters[i - 1], hi = parameters[i];
						for (int32 iteration = 0; iteration < 64; ++iteration)
						{
							const double mid = ((lo + hi) * 0.5);
							if ((mid == lo) || (mid == hi))
							{
								break;
							}
							const double f = Value(mid);
							if (f == 0.0)
							{
								lo = hi = mid;
								break;
							}
							if ((f < 0.0) == (previous < 0.0))
							{
								lo = mid;
							}
							else
							{
								hi = mid;
							}
						}
						AddAt((lo + hi) * 0.5);
					}
					previous = value;
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

			if (line.start == line.end)
			{
				if (Geometry2D::Intersects(line.start, bezier))
				{
					AppendPoint(accumulator, line.start);
				}
				return;
			}

			Line extent;
			if (detail::TryGetBezierSegment(bezier, extent))
			{
				ProcessLineLine(accumulator, line, extent);
				return;
			}

			const auto AppendRoot = [&](const double t)
			{
				const Vec2 point = bezier.pointAt(t);
				if (PointOnLine(point, line))
				{
					AppendPoint(accumulator, point);
				}
				return false;
			};
			const double c0 = d.cross(bezier.p0 - line.start);
			const double c1 = d.cross(bezier.p1 - line.start);
			const double c2 = d.cross(bezier.p2 - line.start);
			if constexpr (std::is_same_v<Bezier, Bezier2>)
			{
				(void)detail::CheckQuadraticRootsInUnitInterval(
					(c0 - 2.0 * c1 + c2), (2.0 * (c1 - c0)), c0, AppendRoot);
			}
			else
			{
				const double c3 = d.cross(bezier.p3 - line.start);
				(void)detail::CheckCubicRootsInUnitInterval(
					(-c0 + 3.0 * c1 - 3.0 * c2 + c3), (3.0 * c0 - 6.0 * c1 + 3.0 * c2),
					(3.0 * (c1 - c0)), c0, AppendRoot);
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

		// Work in a canonical, normalized frame so argument order, direction,
		// and world-space scale do not change the bounded search.
		void AppendBezierPairPoints(Array<Vec2>& result, Bezier3 a, Bezier3 b)
		{
			constexpr double Tolerance = detail::BezierRootTolerance;
			// Double roots have parameter uncertainty on the order of sqrt(epsilon).
			constexpr double ParameterMerge = 2.0e-7;
			constexpr int32 MaxNodes = 64, MaxCorrections = 128;
			if (detail::BezierLexicographicalLess(a.reversed(), a))
			{
				a = a.reversed();
			}
			if (detail::BezierLexicographicalLess(b.reversed(), b))
			{
				b = b.reversed();
			}
			if (detail::BezierLexicographicalLess(b, a))
			{
				std::swap(a, b);
			}
			const Vec2 origin = a.p0;
			double scale = 0.0;
			for (const auto& curve : { a, b })
			{
				for (const auto& p : detail::BezierControlPoints(curve))
				{
					scale = Max({ scale, Abs(p.x - origin.x), Abs(p.y - origin.y) });
				}
			}
			for (auto* curve : { &a, &b })
			{
				curve->p0 = ((curve->p0 - origin) / scale);
				curve->p1 = ((curve->p1 - origin) / scale);
				curve->p2 = ((curve->p2 - origin) / scale);
				curve->p3 = ((curve->p3 - origin) / scale);
			}
			const auto SameCurve = [&](const Bezier3& first, const Bezier3& second)
			{
				const auto ca = detail::BezierControlPoints(first), cb = detail::BezierControlPoints(second);
				for (size_t i = 0; i < ca.size(); ++i)
				{
					if ((ca[i] - cb[i]).lengthSq() > (Tolerance * Tolerance))
					{
						return false;
					}
				}
				return true;
			};
			if (SameCurve(a, b))
			{
				return;
			}

			// A common subcurve has matching endpoints and matching reparameterized
			// control points. Endpoint equality alone is insufficient for a loop.
			std::array<Vec2, 12> endpointPairs;
			size_t endpointCount = 0;
			const auto MatchEndpoint = [&](const Bezier3& curve, const Vec2& point, const double endpoint, const bool swap)
			{
				const auto [lo, hi] = detail::BezierControlBounds(curve);
				const bool useX = ((hi.y - lo.y) <= (hi.x - lo.x));
				const auto controls = detail::BezierControlPoints(curve);
				std::array<double, 4> values;
				for (size_t i = 0; i < 4; ++i)
				{
					values[i] = (useX ? (controls[i].x - point.x) : (controls[i].y - point.y));
				}
				(void)detail::CheckCubicRootsInUnitInterval(
					(-values[0] + 3 * values[1] - 3 * values[2] + values[3]),
					(3 * (values[0] - 2 * values[1] + values[2])), (3 * (values[1] - values[0])), values[0],
					[&](const double t)
					{
						if ((curve.pointAt(t) - point).lengthSq() <= (Tolerance * Tolerance))
						{
							endpointPairs[endpointCount++] = (swap ? Vec2{ t, endpoint } : Vec2{ endpoint, t });
						}
						return false;
					});
			};
			MatchEndpoint(b, a.p0, 0.0, false);
			MatchEndpoint(b, a.p3, 1.0, false);
			MatchEndpoint(a, b.p0, 0.0, true);
			MatchEndpoint(a, b.p3, 1.0, true);
			for (size_t i = 0; i < endpointCount; ++i)
			{
				for (size_t j = i + 1; j < endpointCount; ++j)
				{
					const Vec2 p = endpointPairs[i], q = endpointPairs[j];
					if ((Abs(p.x - q.x) <= Tolerance) || (Abs(p.y - q.y) <= Tolerance))
					{
						continue;
					}
					const auto partA = detail::BezierParameterRange(a, Min(p.x, q.x), Max(p.x, q.x));
					auto partB = detail::BezierParameterRange(b, Min(p.y, q.y), Max(p.y, q.y));
					if ((p.x < q.x) != (p.y < q.y))
					{
						partB = partB.reversed();
					}
					if (SameCurve(partA, partB))
					{
						return;
					}
				}
			}

			struct Event
			{
				double t, s, error;
				Vec2 point;
			};
			std::array<Event, 16> events;
			size_t eventCount = 0;
			int32 corrections = 0;
			const auto Include = [&](double t, double s, const double la, const double ha, const double lb, const double hb)
			{
				Event best{ t, s, std::numeric_limits<double>::infinity(), {} };
				for (int32 iteration = 0; iteration < 20; ++iteration)
				{
					const Vec2 pa = a.pointAt(t), pb = b.pointAt(s), delta = (pa - pb);
					const double error = delta.lengthSq();
					if (error < best.error)
					{
						best = { t, s, error, pa + (pb - pa) * 0.5 };
					}
					if ((error == 0.0) || (corrections == MaxCorrections))
					{
						break;
					}
					const Vec2 u = a.derivativeAt(t), v = b.derivativeAt(s);
					const double determinant = u.cross(v);
					if (determinant == 0.0)
					{
						break;
					}
					++corrections;
					const double nextT = Clamp(t - delta.cross(v) / determinant, la, ha);
					const double nextS = Clamp(s - delta.cross(u) / determinant, lb, hb);
					if ((nextT == t) && (nextS == s))
					{
						break;
					}
					t = nextT;
					s = nextS;
				}
				if (best.error > (Tolerance * Tolerance))
				{
					return false;
				}
				for (size_t i = 0; i < eventCount; ++i)
				{
					if (((Abs(events[i].t - best.t) <= ParameterMerge) && (Abs(events[i].s - best.s) <= ParameterMerge))
						|| ((events[i].point - best.point).lengthSq() <= (Tolerance * Tolerance)))
					{
						if (best.error < events[i].error)
						{
							events[i] = best;
						}
						return false;
					}
				}
				if (eventCount < events.size())
				{
					events[eventCount++] = best;
				}
				return false;
			};
			for (size_t i = 0; i < endpointCount; ++i)
			{
				Include(endpointPairs[i].x, endpointPairs[i].y, 0, 1, 0, 1);
			}
			bool contact = false;
			(void)detail::SeparateBezierPairByProjection(a, b,
				[&](const double t, const double s) { return Include(t, s, 0, 1, 0, 1); }, contact);
			struct Node
			{
				Bezier3 a, b;
				double la, ha, lb, hb;
			};
			std::array<Node, MaxNodes + 1> stack;
			int32 count = 1;
			stack[0] = { a, b, 0, 1, 0, 1 };
			for (int32 visit = 0; count && (visit < MaxNodes); ++visit)
			{
				Node node = stack[--count];
				const auto [loA, hiA] = detail::BezierControlBounds(node.a);
				const auto [loB, hiB] = detail::BezierControlBounds(node.b);
				if (((hiA.x + Tolerance) < loB.x) || ((hiB.x + Tolerance) < loA.x)
					|| ((hiA.y + Tolerance) < loB.y) || ((hiB.y + Tolerance) < loA.y))
				{
					continue;
				}
				const double widthA = (node.ha - node.la), widthB = (node.hb - node.lb);
				if (not detail::ClipBezierPair(node.a, node.b, node.lb, node.hb)
					|| not detail::ClipBezierPair(node.b, node.a, node.la, node.ha))
				{
					continue;
				}
				if (Max(node.ha - node.la, node.hb - node.lb) <= ParameterMerge)
				{
					Include((node.la + node.ha) * 0.5, (node.lb + node.hb) * 0.5, node.la, node.ha, node.lb, node.hb);
					continue;
				}
				if (((node.ha - node.la) < (0.8 * widthA)) || ((node.hb - node.lb) < (0.8 * widthB)))
				{
					stack[count++] = node;
				}
				else if ((hiB - loB).lengthSq() <= (hiA - loA).lengthSq())
				{
					const auto [left, right] = node.a.split(0.5);
					const double middle = (node.la + node.ha) * 0.5;
					stack[count++] = { right, node.b, middle, node.ha, node.lb, node.hb };
					stack[count++] = { left, node.b, node.la, middle, node.lb, node.hb };
				}
				else
				{
					const auto [left, right] = node.b.split(0.5);
					const double middle = (node.lb + node.hb) * 0.5;
					stack[count++] = { node.a, right, node.la, node.ha, middle, node.hb };
					stack[count++] = { node.a, left, node.la, node.ha, node.lb, middle };
				}
			}
			result.reserve(result.size() + eventCount);
			for (size_t i = 0; i < eventCount; ++i)
			{
				result.push_back(origin + events[i].point * scale);
			}
		}

		template <class BezierA, class BezierB>
		void ProcessBezierBezier(IntersectionAccumulator& accumulator, const BezierA& a, const BezierB& b)
		{
			const auto ca = detail::BezierCubicControlPoints(a), cb = detail::BezierCubicControlPoints(b);
			AppendBezierPairPoints(accumulator.points, { ca[0], ca[1], ca[2], ca[3] }, { cb[0], cb[1], cb[2], cb[3] });
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
				const Ellipse ellipse{ a.circle.center, a.circle.r, a.circle.r };
				if (SameEllipse(ellipse, b))
				{
					accumulator.positiveDimensionalComponents.emplace_back(a);
				}
				else
				{
					ProcessEllipseEllipse(accumulator, ellipse, b,
						[&](const Vec2& p) { return ArcContainsPoint(a, p); });
				}
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
					ProcessEllipseEllipse(accumulator, a, b, [](const Vec2&) { return true; });
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
			detail::BoundarySource<BoundaryPiece> boundaryPieces;
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
				data.boundaryPieces = detail::MakeBoundarySource<BoundaryPiece>(shape,
					[&](auto& pieces) { AppendBoundaryPieces(pieces, shape); });
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

			(void)detail::AnyBoundaryPiece(a.boundaryPieces, [&](const BoundaryPiece& pieceA)
				{
					(void)detail::AnyBoundaryPiece(b.boundaryPieces, [&](const BoundaryPiece& pieceB)
						{
							std::visit([&](const auto& primitiveA, const auto& primitiveB)
								{
									ProcessPiecePair(accumulator, primitiveA, primitiveB);
								}, pieceA, pieceB);
							return false;
						});
					return false;
				});

			return FinalizePoints(std::move(accumulator));
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<Array<Vec2>> ComputeIntersectsAt(const ShapeA& a, const ShapeB& b)
		{
			return detail::WithSimpleBezierPair(a, b, [](const auto& a, const auto& b) -> Optional<Array<Vec2>>
			{
				if (not Geometry2D::Intersects(a, b))
				{
					return none;
				}

				if constexpr (detail::IsBezier<std::decay_t<decltype(a)>> && detail::IsBezier<std::decay_t<decltype(b)>>)
				{
					IntersectionAccumulator accumulator;
					ProcessBezierBezier(accumulator, a, b);
					return std::move(accumulator.points);
				}

				return EnumerateKnownIntersection(
					MakeShapeIntersectionData(a), MakeShapeIntersectionData(b));
			});
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
