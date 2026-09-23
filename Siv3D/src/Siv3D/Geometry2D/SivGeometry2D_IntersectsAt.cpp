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
		inline constexpr double PointMergeTolerance = 1.0e-9;

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
			double pointMergeTolerance = PointMergeTolerance;
		};

		void SetPointMergeScale(IntersectionAccumulator& accumulator, const double scale) noexcept
		{
			accumulator.pointMergeTolerance = Min(accumulator.pointMergeTolerance, (detail::EllipseContactTolerance * scale));
		}

		[[nodiscard]]
		constexpr double Square(const double x) noexcept
		{
			return (x * x);
		}

		[[nodiscard]]
		double CoordinateTolerance(const double a, const double b, const double tolerance) noexcept
		{
			const double scale = Max(Abs(a), Abs(b));
			// Solver residual and coordinate rounding are separate error sources.
			// A translation must not turn the solver tolerance into a world-space radius.
			return Max(tolerance, (4.0 * std::numeric_limits<double>::epsilon() * scale));
		}

		[[nodiscard]]
		bool NearlyEqualPoint(const Vec2& a, const Vec2& b, const double tolerance) noexcept
		{
			return (Abs(a.x - b.x) <= CoordinateTolerance(a.x, b.x, tolerance))
				&& (Abs(a.y - b.y) <= CoordinateTolerance(a.y, b.y, tolerance));
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
			const double tolerance = (detail::BezierRootTolerance * Max(Abs(d.x), Abs(d.y)));
			const double tx = Max(tolerance, (4.0 * std::numeric_limits<double>::epsilon()
				* Max({ Abs(p.x), Abs(line.start.x), Abs(line.end.x) })));
			const double ty = Max(tolerance, (4.0 * std::numeric_limits<double>::epsilon()
				* Max({ Abs(p.y), Abs(line.start.y), Abs(line.end.y) })));
			return (((Min(line.start.x, line.end.x) - tx) <= p.x) && (p.x <= (Max(line.start.x, line.end.x) + tx))
				&& ((Min(line.start.y, line.end.y) - ty) <= p.y) && (p.y <= (Max(line.start.y, line.end.y) + ty))
				&& (Abs(d.cross(v)) <= (Abs(d.x) * ty + Abs(d.y) * tx)));
		}

		[[nodiscard]]
		bool ArcContainsPoint(const CircleArc& arc, const Vec2& p) noexcept
		{
			if (arc.region == ArcRegion::Full)
			{
				return true;
			}

			const Vec2 c = arc.circle.center;
			const double tolerance = (detail::EllipseContactTolerance * arc.circle.r);
			const double tx = CoordinateTolerance(p.x, c.x, tolerance);
			const double ty = CoordinateTolerance(p.y, c.y, tolerance);

			switch (arc.region)
			{
			case ArcRegion::TopLeft:
				return ((p.x <= (c.x + tx)) && (p.y <= (c.y + ty)));
			case ArcRegion::TopRight:
				return (((c.x - tx) <= p.x) && (p.y <= (c.y + ty)));
			case ArcRegion::BottomRight:
				return (((c.x - tx) <= p.x) && ((c.y - ty) <= p.y));
			case ArcRegion::BottomLeft:
				return ((p.x <= (c.x + tx)) && ((c.y - ty) <= p.y));
			default:
				return true;
			}
		}

		[[nodiscard]]
		bool PointOnCircleArc(const Vec2& p, const CircleArc& arc) noexcept
		{
			const Vec2 v = ((p - arc.circle.center) / arc.circle.r);
			return (Abs(v.dot(v) - 1.0) <= (4.0 * detail::EllipseContactTolerance))
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
		bool PointOnSuperEllipseBoundary(const Vec2& p, const SuperEllipse& shape) noexcept
		{
			const Vec2 normalized = ((p - shape.center) / shape.axes);
			const double x = Abs(normalized.x), y = Abs(normalized.y);
			const double tx = (CoordinateTolerance(p.x, shape.x, (detail::EllipseContactTolerance * shape.a)) / shape.a);
			const double ty = (CoordinateTolerance(p.y, shape.y, (detail::EllipseContactTolerance * shape.b)) / shape.b);
			return ((std::pow(Max(0.0, x - tx), shape.n) + std::pow(Max(0.0, y - ty), shape.n)) <= 1.0)
				&& (1.0 <= (std::pow(x + tx, shape.n) + std::pow(y + ty, shape.n)));
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
					if (NearlyEqualPoint(point, existing, accumulator.pointMergeTolerance))
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
				if (shape.n == 1.0)
				{
					AppendLinePiece(pieces, Line{ shape.top(), shape.right() });
					AppendLinePiece(pieces, Line{ shape.right(), shape.bottom() });
					AppendLinePiece(pieces, Line{ shape.bottom(), shape.left() });
					AppendLinePiece(pieces, Line{ shape.left(), shape.top() });
				}
				else if (shape.n == 2.0)
				{
					pieces.emplace_back(Ellipse{ shape.center, shape.axes });
				}
				else
				{
					pieces.emplace_back(shape);
				}
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
			accumulator.pointMergeTolerance = Min(accumulator.pointMergeTolerance,
				(detail::BezierRootTolerance * Max({ Abs(r.x), Abs(r.y), Abs(s.x), Abs(s.y) })));
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

		template <class Accept>
		void ProcessLineEllipse(IntersectionAccumulator& accumulator, const Line& line, const Ellipse& ellipse, Accept&& accept)
		{
			const Vec2 p0 = ((line.start - ellipse.center) / ellipse.axes);
			const Vec2 p1 = ((line.end - ellipse.center) / ellipse.axes);
			const Vec2 d = (p1 - p0);
			const double length = d.length();
			SetPointMergeScale(accumulator, Max({ ellipse.a, ellipse.b,
				Abs(line.end.x - line.start.x), Abs(line.end.y - line.start.y) }));
			const auto AddAt = [&](const double t, const Vec2& normalized)
			{
				if (InRange(t, -detail::EllipseContactTolerance, (1.0 + detail::EllipseContactTolerance)))
				{
					const Vec2 point = (ellipse.center + ellipse.axes * normalized);
					if (accept(point))
					{
						AppendPoint(accumulator, point);
					}
				}
			};
			if (length == 0.0)
			{
				if (PointOnEllipseBoundary(line.start, ellipse))
				{
					AddAt(0.0, p0);
				}
				return;
			}
			// Distance to the supporting line avoids cancellation between large
			// quadratic coefficients when a long segment crosses a small ellipse.
			const Vec2 unit = (d / length);
			const double normal = std::fma(p0.x, unit.y, (-p0.y * unit.x));
			const double heightSq = std::fma(-normal, normal, 1.0);
			const double tolerance = (2.0 * detail::EllipseContactTolerance);
			if (heightSq < -tolerance)
			{
				return;
			}
			const double middle = (-p0.dot(unit) / length);
			const double height = ((Abs(heightSq) <= tolerance) ? 0.0 : std::sqrt(heightSq));
			const double offset = (height / length);
			const Vec2 closest{ (normal * unit.y), (-normal * unit.x) };
			AddAt(middle - offset, closest - unit * height);
			if (offset != 0.0)
			{
				AddAt(middle + offset, closest + unit * height);
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
			SetPointMergeScale(accumulator, Max({ a->a, a->b, b->a, b->b }));
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
		bool AppendParametricRoots(IntersectionAccumulator& accumulator,
			PointAt&& pointAt, Function&& function, Accept&& accept, const bool closed)
		{
			constexpr int32 Samples = 32, MaxEvaluations = 256;
			std::array<double, Samples + 1> values;
			int32 evaluations = 0;
			const auto Wrap = [](const double t) { return ((t < 0.0) ? (t + 1.0) : ((1.0 < t) ? (t - 1.0) : t)); };
			const auto Value = [&](const double t) { ++evaluations; return function(pointAt(Wrap(t))); };
			bool allNearZero = true;
			for (int32 i = 0; i <= Samples; ++i)
			{
				const Vec2 point = pointAt(static_cast<double>(i) / Samples);
				++evaluations;
				const double value = function(point);
				values[i] = (((Abs(value) <= detail::EllipseContactTolerance) || accept(point)) ? 0.0 : value);
				allNearZero = (allNearZero && (Abs(values[i]) <= detail::EllipseContactTolerance));
			}
			if (allNearZero)
			{
				return true;
			}
			const auto AddAt = [&](const double t)
			{
				const Vec2 point = pointAt(Wrap(t));
				if (accept(point))
				{
					AppendPoint(accumulator, point);
					return true;
				}
				return false;
			};
			const auto Crossing = [](const double a, const double b)
			{
				return (((a < 0.0) && (0.0 < b)) || ((b < 0.0) && (0.0 < a)));
			};
			const auto Refine = [&](double lo, double hi, double flo, double fhi)
			{
				if (not Crossing(flo, fhi))
				{
					return;
				}
				double bestT = ((Abs(flo) < Abs(fhi)) ? lo : hi);
				double bestError = Min(Abs(flo), Abs(fhi));
				int32 retainedSide = 0;
				for (int32 i = 0; (i < 48) && (evaluations < MaxEvaluations); ++i)
				{
					const double estimate = (lo + (hi - lo) * (-flo / (fhi - flo)));
					const double margin = ((hi - lo) * 1.0e-6);
					const double t = (((lo + margin) < estimate) && (estimate < (hi - margin))) ? estimate : ((lo + hi) * 0.5);
					if ((t <= lo) || (hi <= t))
					{
						break;
					}
					const double value = Value(t);
					if ((Abs(value) <= 1.0e-7) && AddAt(t))
					{
						return;
					}
					if (Abs(value) < bestError)
					{
						bestT = t;
						bestError = Abs(value);
					}
					if (value == 0.0)
					{
						break;
					}
					// Illinois regula falsi prevents a shallow crossing from retaining
					// one distant endpoint for the entire refinement budget.
					if ((value < 0.0) == (flo < 0.0))
					{
						lo = t;
						flo = value;
						if (retainedSide == 1)
						{
							fhi *= 0.5;
						}
						retainedSide = 1;
					}
					else
					{
						hi = t;
						fhi = value;
						if (retainedSide == -1)
						{
							flo *= 0.5;
						}
						retainedSide = -1;
					}
				}
				// The acceptance callback verifies the actual point even after budget exhaustion.
				AddAt(bestT);
			};
			for (int32 i = 0; i <= Samples; ++i)
			{
				if (Abs(values[i]) <= detail::EllipseContactTolerance)
				{
					AddAt(static_cast<double>(i) / Samples);
				}
				if ((i < Samples) && (evaluations < MaxEvaluations))
				{
					Refine(static_cast<double>(i) / Samples, static_cast<double>(i + 1) / Samples, values[i], values[i + 1]);
				}
			}
			// Search signed extrema, not minima of |f|: two nearby crossings
			// can enclose an arbitrarily deep valley between uniform samples.
			for (int32 i = 0; (i < (closed ? Samples : (Samples + 1))) && (evaluations < MaxEvaluations); ++i)
			{
				const int32 previous = ((closed && (i == 0)) ? (Samples - 1) : Max(0, i - 1));
				const int32 next = Min(Samples, i + 1);
				const double sign = ((0.0 < values[i]) ? 1.0 : -1.0);
				if ((Abs(values[i]) <= detail::EllipseContactTolerance)
					|| Crossing(values[previous], values[i]) || Crossing(values[i], values[next])
					|| (((i != 0) || closed) && not (sign * values[i] < sign * values[previous]))
					|| ((i < Samples) && not (sign * values[i] <= sign * values[next])))
				{
					continue;
				}
				constexpr double Ratio = 0.6180339887498948482;
				const double left = (static_cast<double>(closed ? (i - 1) : Max(0, i - 1)) / Samples);
				const double right = (static_cast<double>(next) / Samples);
				double lo = left, hi = right;
				double t1 = (hi - Ratio * (hi - lo)), t2 = (lo + Ratio * (hi - lo));
				if ((evaluations + 2) > MaxEvaluations)
				{
					break;
				}
				double f1 = Value(t1), f2 = Value(t2);
				for (int32 j = 0; (j < 48) && (evaluations < MaxEvaluations); ++j)
				{
					if ((sign * f1) < (sign * f2))
					{
						hi = t2;
						t2 = t1;
						f2 = f1;
						t1 = (hi - Ratio * (hi - lo));
						f1 = Value(t1);
					}
					else
					{
						lo = t1;
						t1 = t2;
						f1 = f2;
						t2 = (lo + Ratio * (hi - lo));
						f2 = Value(t2);
					}
				}
				const bool first = ((sign * f1) < (sign * f2));
				const double t = (first ? t1 : t2), f = (first ? f1 : f2);
				if (AddAt(t))
				{
					continue;
				}
				if (Crossing(values[i], f))
				{
					Refine(left, t, values[previous], f);
					Refine(t, right, f, values[next]);
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
		Vec2 SuperEllipsePointAt(const SuperEllipse& shape, const double half, const double t) noexcept
		{
			const double phase = (t * 4.0);
			const int32 quadrant = Min(static_cast<int32>(phase), 3);
			const double part = (phase - quadrant);
			const double u = ((part <= 0.5) ? (2.0 * part) : (2.0 * (1.0 - part)));
			double x, y;
			// Use the coordinate with a bounded profile slope directly. Fractional
			// powers of sin/cos near a quadrant endpoint lose spatial precision.
			if (shape.n < 1.0)
			{
				x = ((1.0 - u) + u * half);
				y = std::pow(Max(0.0, (1.0 - std::pow(x, shape.n))), (1.0 / shape.n));
			}
			else
			{
				y = (u * half);
				x = std::pow(Max(0.0, (1.0 - std::pow(y, shape.n))), (1.0 / shape.n));
			}
			if (0.5 < part)
			{
				std::swap(x, y);
			}
			if (quadrant & 1)
			{
				std::swap(x, y);
			}
			if ((quadrant == 1) || (quadrant == 2))
			{
				x = -x;
			}
			if (2 <= quadrant)
			{
				y = -y;
			}
			return (shape.center + shape.axes * Vec2{ x, y });
		}

		[[nodiscard]]
		double SuperEllipseImplicit(const SuperEllipse& superEllipse, const Vec2& p) noexcept
		{
			const double x = Abs((p.x - superEllipse.center.x) / superEllipse.axes.x);
			const double y = Abs((p.y - superEllipse.center.y) / superEllipse.axes.y);
			return (std::pow(x, superEllipse.n) + std::pow(y, superEllipse.n) - 1.0);
		}

		void ProcessLineSuperEllipse(IntersectionAccumulator& accumulator, Line line, const SuperEllipse& shape)
		{
			if (std::tie(line.end.x, line.end.y) < std::tie(line.start.x, line.start.y))
			{
				std::swap(line.start, line.end);
			}
			const Vec2 p0 = ((line.start - shape.center) / shape.axes);
			const Vec2 p1 = ((line.end - shape.center) / shape.axes);
			const Vec2 d = (p1 - p0);
			SetPointMergeScale(accumulator, Max({ shape.a, shape.b,
				Abs(line.end.x - line.start.x), Abs(line.end.y - line.start.y) }));
			std::array<double, 5> parameters{ 0.0 };
			size_t count = 1;
			const auto Include = [&](const double t)
			{
				if ((0.0 < t) && (t < 1.0))
				{
					parameters[count++] = t;
				}
			};
			if (d.x != 0.0)
			{
				Include(-p0.x / d.x);
			}
			if (d.y != 0.0)
			{
				Include(-p0.y / d.y);
			}
			// On a quadrant the implicit function is convex (n>1) or concave
			// (n<1). Its sole stationary point has |x/y|^(n-1)=|dy/dx|.
			// Choose the ratio <=1 before pow, including exponents near one.
			if ((d.x != 0.0) && (d.y != 0.0))
			{
				const bool smallerX = (Abs(d.x) <= Abs(d.y));
				const double slope = (smallerX ? (d.x / d.y) : (d.y / d.x));
				const double ratio = -std::copysign(std::pow(Abs(slope), (1.0 / Abs(shape.n - 1.0))), slope);
				const Vec2 axis = ((smallerX == (1.0 < shape.n)) ? Vec2{ 1, ratio } : Vec2{ ratio, 1 });
				const double cross = d.cross(axis);
				if (cross != 0.0)
				{
					Include(-p0.cross(axis) / cross);
				}
			}
			parameters[count++] = 1.0;
			std::sort(parameters.begin(), parameters.begin() + count);
			const double tolerance = detail::EllipseContactTolerance;
			const auto Value = [&](const double t)
			{
				const double x = Abs(std::fma(d.x, t, p0.x));
				const double y = Abs(std::fma(d.y, t, p0.y));
				return (std::pow(x, shape.n) + std::pow(y, shape.n) - 1.0);
			};
			const auto EndpointValue = [&](const double t)
			{
				const double x = Abs(std::fma(d.x, t, p0.x));
				const double y = Abs(std::fma(d.y, t, p0.y));
				// Test a spatial band; implicit-value error alone is unsuitable at a concave tip.
				const double lower = (std::pow(Max(0.0, x - tolerance), shape.n) + std::pow(Max(0.0, y - tolerance), shape.n));
				const double upper = (std::pow(x + tolerance, shape.n) + std::pow(y + tolerance, shape.n));
				return ((lower <= 1.0) && (1.0 <= upper)) ? 0.0 : Value(t);
			};
			double previous = EndpointValue(parameters[0]);
			if (previous == 0.0)
			{
				AppendPoint(accumulator, line.start);
			}
			for (size_t i = 1; i < count; ++i)
			{
				const double value = EndpointValue(parameters[i]);
				if (value == 0.0)
				{
					AppendPoint(accumulator, line.interpolatedPointAt(parameters[i]));
				}
				else if (((previous < 0.0) && (0.0 < value)) || ((value < 0.0) && (0.0 < previous)))
				{
					double lo = parameters[i - 1], hi = parameters[i];
					for (int32 iteration = 0; iteration < 48; ++iteration)
					{
						const double middle = ((lo + hi) * 0.5);
						const double f = Value(middle);
						if ((f == 0.0) || (middle == lo) || (middle == hi))
						{
							lo = hi = middle;
							break;
						}
						if ((f < 0.0) == (previous < 0.0))
						{
							lo = middle;
						}
						else
						{
							hi = middle;
						}
					}
					AppendPoint(accumulator, line.interpolatedPointAt((lo + hi) * 0.5));
				}
				previous = value;
			}
		}

		template <class Bezier>
		void ProcessLineBezier(IntersectionAccumulator& accumulator, const Line& line, const Bezier& bezier)
		{
			const Vec2 d = (line.end - line.start);
			const double tolerance = detail::BezierEvaluationTolerance(bezier);
			accumulator.pointMergeTolerance = Min(accumulator.pointMergeTolerance, tolerance);

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
				if (detail::BezierRootPointIsOnSegmentRange(point, line, tolerance))
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
			SetPointMergeScale(accumulator, Max(a.circle.r, b.circle.r));
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
			const double tolerance = (detail::EllipseContactTolerance * Max({ r0 * r0, r1 * r1, distanceSq }));

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

		// The normalized squared radius has degree 4 or 6. Its stationary
		// parameters partition every crossing, including two within one sample
		// interval, and also supply candidates for tangential contacts.
		template <class Bezier, class Accept>
		void ProcessBezierEllipse(IntersectionAccumulator& accumulator, const Bezier& original,
			const Ellipse& ellipse, Accept&& accept)
		{
			const Bezier curve = (detail::BezierLexicographicalLess(original.reversed(), original) ? original.reversed() : original);
			Bezier local = curve;
			local.p0 = ((curve.p0 - ellipse.center) / ellipse.axes);
			local.p1 = ((curve.p1 - ellipse.center) / ellipse.axes);
			local.p2 = ((curve.p2 - ellipse.center) / ellipse.axes);
			if constexpr (std::is_same_v<Bezier, Bezier3>)
			{
				local.p3 = ((curve.p3 - ellipse.center) / ellipse.axes);
			}
			const auto controls = detail::BezierControlPoints(local);
			double localScale = 1.0, worldScale = Max(ellipse.a, ellipse.b);
			for (const Vec2& p : controls)
			{
				localScale = Max({ localScale, Abs(p.x), Abs(p.y) });
				worldScale = Max({ worldScale, Abs(p.x * ellipse.a), Abs(p.y * ellipse.b) });
			}
			const double tolerance = (2.0 * detail::EllipseContactTolerance * localScale);
			SetPointMergeScale(accumulator, worldScale);
			const auto stationary = detail::BezierPointStationaryParameters(controls, Vec2{ 0, 0 });
			std::array<double, 2 * std::tuple_size_v<decltype(controls)> - 1> parameters{};
			for (size_t i = 0; i < stationary.count; ++i)
			{
				parameters[i + 1] = stationary.values[i];
			}
			const size_t count = (stationary.count + 2);
			parameters[count - 1] = 1.0;
			const auto Value = [&](const double t)
			{
				const Vec2 p = local.pointAt(t);
				return std::fma(p.x, p.x, std::fma(p.y, p.y, -1.0));
			};
			const auto EndpointValue = [&](const double t)
			{
				const double value = Value(t);
				return ((Abs(value) <= tolerance) ? 0.0 : value);
			};
			const auto AddAt = [&](const double t)
			{
				const Vec2 point = curve.pointAt(t);
				if (accept(point))
				{
					AppendPoint(accumulator, point);
				}
			};
			std::array<double, parameters.size()> values;
			bool allNearZero = true;
			for (size_t i = 0; i < count; ++i)
			{
				values[i] = EndpointValue(parameters[i]);
				allNearZero = (allNearZero && (values[i] == 0.0));
			}
			if (allNearZero)
			{
				accumulator.positiveDimensionalComponents.emplace_back(curve);
				return;
			}
			double previous = values[0];
			if (previous == 0.0)
			{
				AddAt(0.0);
			}
			for (size_t i = 1; i < count; ++i)
			{
				const double value = values[i];
				if (value == 0.0)
				{
					AddAt(parameters[i]);
				}
				else if (((previous < 0.0) && (0.0 < value)) || ((value < 0.0) && (0.0 < previous)))
				{
					double lo = parameters[i - 1], hi = parameters[i], flo = previous;
					double t = ((lo + hi) * 0.5), bestT = t, bestError = std::numeric_limits<double>::infinity();
					for (int32 iteration = 0; iteration < 32; ++iteration)
					{
						const Vec2 p = local.pointAt(t);
						const double f = std::fma(p.x, p.x, std::fma(p.y, p.y, -1.0));
						if (Abs(f) < bestError)
						{
							bestError = Abs(f);
							bestT = t;
						}
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
						}
						const double next = (t - f / (2.0 * p.dot(local.derivativeAt(t))));
						if (next == t)
						{
							break;
						}
						const double candidate = (((lo < next) && (next < hi)) ? next : ((lo + hi) * 0.5));
						if (candidate == t)
						{
							break;
						}
						t = candidate;
					}
					// Exhausting the refinement budget must not emit an off-boundary point.
					if (bestError <= tolerance)
					{
						AddAt(bestT);
					}
				}
				previous = value;
			}
		}

		template <class PointAt, class Accept, class OverlapPiece>
		void ProcessCurveImplicit(
			IntersectionAccumulator& accumulator,
			PointAt&& pointAt, const SuperEllipse& shape, Accept&& accept,
			const OverlapPiece& overlapPiece)
		{
			SetPointMergeScale(accumulator, Max(shape.a, shape.b));
			const bool closed = [&]
			{
				if constexpr (std::is_same_v<OverlapPiece, CircleArc>)
				{
					return (overlapPiece.region == ArcRegion::Full);
				}
				else
				{
					return (std::is_same_v<OverlapPiece, Ellipse> || std::is_same_v<OverlapPiece, SuperEllipse>);
				}
			}();
			if (AppendParametricRoots(accumulator, pointAt,
				[&](const Vec2& p) { return SuperEllipseImplicit(shape, p); },
				[&](const Vec2& p) { return accept(p) && PointOnSuperEllipseBoundary(p, shape); }, closed))
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
				ProcessLineEllipse(accumulator, a, Ellipse{ b.circle.center, b.circle.r, b.circle.r },
					[&](const Vec2& p) { return ArcContainsPoint(b, p); });
			}
			else if constexpr (std::is_same_v<A, Line> && std::is_same_v<B, Ellipse>)
			{
				ProcessLineEllipse(accumulator, a, b, [](const Vec2&) { return true; });
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
					b,
					[&](const Vec2& p) { return ArcContainsPoint(a, p); }, a);
			}
			else if constexpr (std::is_same_v<A, CircleArc> && detail::IsBezier<B>)
			{
				ProcessBezierEllipse(accumulator, b, Ellipse{ a.circle.center, a.circle.r, a.circle.r },
					[&](const Vec2& p) { return ArcContainsPoint(a, p); });
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
					b,
					[](const Vec2&) { return true; }, a);
			}
			else if constexpr (std::is_same_v<A, Ellipse> && detail::IsBezier<B>)
			{
				ProcessBezierEllipse(accumulator, b, a, [](const Vec2&) { return true; });
			}
			else if constexpr (std::is_same_v<A, SuperEllipse> && std::is_same_v<B, SuperEllipse>)
			{
				if (SameSuperEllipse(a, b))
				{
					accumulator.positiveDimensionalComponents.emplace_back(a);
				}
				else
				{
					const bool swap = (std::tie(b.n, b.a, b.b, b.x, b.y) < std::tie(a.n, a.a, a.b, a.x, a.y));
					const SuperEllipse& source = (swap ? b : a);
					const SuperEllipse& target = (swap ? a : b);
					const double half = std::pow(0.5, (1.0 / source.n));
					ProcessCurveImplicit(accumulator,
						[&](const double t) { return SuperEllipsePointAt(source, half, t); }, target,
						[](const Vec2&) { return true; }, source);
				}
			}
			else if constexpr (std::is_same_v<A, SuperEllipse> && detail::IsBezier<B>)
			{
				const B curve = (detail::BezierLexicographicalLess(b.reversed(), b) ? b.reversed() : b);
				ProcessCurveImplicit(accumulator,
					[&](const double t) { return curve.pointAt(t); }, a,
					[](const Vec2&) { return true; }, curve);
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
