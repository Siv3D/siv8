
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
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/SignedDistance.hpp>
# include "PolygonGeometry.hpp"
# include "EllipseGeometry.hpp"
# include "SuperEllipseGeometry.hpp"

namespace s3d
{
	namespace
	{
		inline constexpr double Pi = 3.1415926535897932384626433832795029;
		inline constexpr double TwoPi = (2.0 * Pi);
		inline constexpr double DoubleEpsilon = 2.2204460492503131e-16;

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

		using BoundaryPiece = std::variant<Line, CircleArc, Ellipse, SuperEllipse>;

		struct ClosestBoundaryCandidate
		{
			Vec2 point{ 0.0, 0.0 };
			double distanceSq = std::numeric_limits<double>::infinity();
		};

		struct BoundaryData
		{
			bool empty = true;
			bool hasPositiveArea = false;
			Optional<Vec2> pointBoundary;
			detail::BoundarySource<BoundaryPiece> pieces;
		};

		[[nodiscard]]
		constexpr double ClampUnit(const double value) noexcept
		{
			return Clamp(value, 0.0, 1.0);
		}

		void UpdateCandidate(ClosestBoundaryCandidate& best,
			const Vec2& query, const Vec2& boundaryPoint) noexcept
		{
			const double distanceSq = query.distanceFromSq(boundaryPoint);

			if (distanceSq < best.distanceSq)
			{
				best.point = boundaryPoint;
				best.distanceSq = distanceSq;
			}
		}

		[[nodiscard]]
		constexpr std::pair<double, double> ArcAngleRange(const ArcRegion region) noexcept
		{
			switch (region)
			{
			case ArcRegion::TopLeft:
				return { Pi, (1.5 * Pi) };
			case ArcRegion::TopRight:
				return { (1.5 * Pi), TwoPi };
			case ArcRegion::BottomRight:
				return { 0.0, (0.5 * Pi) };
			case ArcRegion::BottomLeft:
				return { (0.5 * Pi), Pi };
			default:
				return { 0.0, TwoPi };
			}
		}

		[[nodiscard]]
		constexpr bool ArcContainsDirection(const ArcRegion region, const Vec2& direction) noexcept
		{
			switch (region)
			{
			case ArcRegion::TopLeft:
				return ((direction.x <= 0.0) && (direction.y <= 0.0));
			case ArcRegion::TopRight:
				return ((0.0 <= direction.x) && (direction.y <= 0.0));
			case ArcRegion::BottomRight:
				return ((0.0 <= direction.x) && (0.0 <= direction.y));
			case ArcRegion::BottomLeft:
				return ((direction.x <= 0.0) && (0.0 <= direction.y));
			default:
				return true;
			}
		}

		[[nodiscard]]
		Vec2 PointAt(const CircleArc& primitive, const double t) noexcept
		{
			const auto [beginAngle, endAngle] = ArcAngleRange(primitive.region);
			const double angle = (beginAngle + (endAngle - beginAngle) * t);
			return (primitive.circle.center
				+ Vec2{ std::cos(angle), std::sin(angle) } * primitive.circle.r);
		}

		[[nodiscard]]
		ClosestBoundaryCandidate ClosestPointOnSegment(
			const Vec2& point, const Vec2& start, const Vec2& end) noexcept
		{
			ClosestBoundaryCandidate result;
			const Vec2 direction = (end - start);
			const double lengthSq = direction.dot(direction);
			double t = 0.0;

			if (lengthSq != 0.0)
			{
				t = ClampUnit(((point - start).dot(direction) / lengthSq));
			}

			UpdateCandidate(result, point, (start + direction * t));
			return result;
		}

		[[nodiscard]]
		ClosestBoundaryCandidate ClosestPointCircleArc(
			const Vec2& point, const CircleArc& arc) noexcept
		{
			ClosestBoundaryCandidate result;
			const Vec2 direction = (point - arc.circle.center);
			const double length = direction.length();

			if ((length != 0.0) && ArcContainsDirection(arc.region, direction))
			{
				const Vec2 closest = (arc.circle.center + direction * (arc.circle.r / length));
				UpdateCandidate(result, point, closest);
			}

			UpdateCandidate(result, point, PointAt(arc, 0.0));
			UpdateCandidate(result, point, PointAt(arc, 1.0));
			return result;
		}

		[[nodiscard]]
		ClosestBoundaryCandidate ClosestPointPiece(
			const Vec2& point, const BoundaryPiece& piece)
		{
			if (const auto* ellipse = std::get_if<Ellipse>(&piece))
			{
				ClosestBoundaryCandidate result;
				UpdateCandidate(result, point, detail::ClosestPointOnEllipseBoundary(point, *ellipse));
				return result;
			}

			if (const auto* shape = std::get_if<SuperEllipse>(&piece); shape && ((shape->n == 1.0) || (shape->n == 2.0)))
			{
				ClosestBoundaryCandidate result;
				const Vec2 closest = (shape->n == 1.0)
					? detail::ClosestPointOnDiamondBoundary(point, *shape)
					: detail::ClosestPointOnEllipseBoundary(point, Ellipse{ shape->center, shape->axes });
				UpdateCandidate(result, point, closest);
				return result;
			}

			if (const auto* shape = std::get_if<SuperEllipse>(&piece); shape && (not Geometry2D::Intersects(point, *shape)))
			{
				ClosestBoundaryCandidate result;
				UpdateCandidate(result, point, detail::ClosestPointOnSuperEllipseBoundaryFromOutside(point, *shape));
				return result;
			}

			if (const Line* line = std::get_if<Line>(&piece))
			{
				return ClosestPointOnSegment(point, line->start, line->end);
			}

			if (const CircleArc* arc = std::get_if<CircleArc>(&piece))
			{
				return ClosestPointCircleArc(point, *arc);
			}

			ClosestBoundaryCandidate result;
			UpdateCandidate(result, point, detail::ClosestPointOnSuperEllipseBoundaryFromInside(point, std::get<SuperEllipse>(piece)));
			return result;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Rect& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(RectF{ shape })
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const RectF& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Circle& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Ellipse& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const SuperEllipse& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Triangle&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Quad&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const RoundRect& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Empty);
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Polygon& shape) noexcept
		{
			return shape.isEmpty();
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const MultiPolygon& shape) noexcept
		{
			for (const auto& polygon : shape)
			{
				if (not polygon.isEmpty())
				{
					return false;
				}
			}

			return true;
		}

		[[nodiscard]]
		bool HasPositiveArea(const Rect& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(RectF{ shape })
				== detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const RectF& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const Circle& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const Ellipse& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const SuperEllipse& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const Triangle& shape) noexcept
		{
			return ((shape.p1 - shape.p0).cross(shape.p2 - shape.p0) != 0.0);
		}

		[[nodiscard]]
		bool HasPositiveArea(const Quad& shape) noexcept
		{
			const double twiceArea = (shape.p0.cross(shape.p1)
				+ shape.p1.cross(shape.p2)
				+ shape.p2.cross(shape.p3)
				+ shape.p3.cross(shape.p0));
			return (twiceArea != 0.0);
		}

		[[nodiscard]]
		bool HasPositiveArea(const RoundRect& shape) noexcept
		{
			return (detail::ClassifyGeometry2DSizedShape(shape)
				== detail::Geometry2DSizedShapeKind::Area);
		}

		[[nodiscard]]
		bool HasPositiveArea(const Polygon& shape) noexcept
		{
			return (not shape.isEmpty());
		}

		[[nodiscard]]
		bool HasPositiveArea(const MultiPolygon& shape) noexcept
		{
			for (const auto& polygon : shape)
			{
				if (not polygon.isEmpty())
				{
					return true;
				}
			}

			return false;
		}

		[[nodiscard]]
		bool TryGetPointBoundary(const Triangle& shape, Vec2& point) noexcept
		{
			if ((shape.p0 == shape.p1) && (shape.p1 == shape.p2))
			{
				point = shape.p0;
				return true;
			}

			return false;
		}

		[[nodiscard]]
		bool TryGetPointBoundary(const Quad& shape, Vec2& point) noexcept
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
		bool TryGetPointBoundary(const Shape&, Vec2&) noexcept
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

		void AppendLinePiece(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const Line& line)
		{
			if (line.start != line.end)
			{
				pieces.emplace_back(line);
			}
		}

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const RectF& shape)
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

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const Rect& shape)
		{
			AppendBoundaryPieces(pieces, RectF{ shape });
		}

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const Circle& shape)
		{
			if (detail::ClassifyGeometry2DSizedShape(shape) == detail::Geometry2DSizedShapeKind::Area)
			{
				pieces.emplace_back(CircleArc{ shape, ArcRegion::Full });
			}
		}

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const Ellipse& shape)
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

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const SuperEllipse& shape)
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

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const Triangle& shape)
		{
			if ((shape.p1 - shape.p0).cross(shape.p2 - shape.p0) == 0.0)
			{
				AppendLinePiece(pieces, TriangleDegenerateExtent(shape));
				return;
			}

			AppendLinePiece(pieces, Line{ shape.p0, shape.p1 });
			AppendLinePiece(pieces, Line{ shape.p1, shape.p2 });
			AppendLinePiece(pieces, Line{ shape.p2, shape.p0 });
		}

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const Quad& shape)
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

			AppendLinePiece(pieces, Line{ shape.p0, shape.p1 });
			AppendLinePiece(pieces, Line{ shape.p1, shape.p2 });
			AppendLinePiece(pieces, Line{ shape.p2, shape.p3 });
			AppendLinePiece(pieces, Line{ shape.p3, shape.p0 });
		}

		void AppendBoundaryPieces(detail::BoundaryPieceBuffer<BoundaryPiece>& pieces, const RoundRect& shape)
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
		[[nodiscard]]
		BoundaryData MakeBoundaryData(const Shape& shape)
		{
			BoundaryData data;

			if (IsEmptyGeometry(shape))
			{
				return data;
			}

			data.empty = false;
			data.hasPositiveArea = HasPositiveArea(shape);
			Vec2 point;

			if (TryGetPointBoundary(shape, point))
			{
				data.pointBoundary = point;
				return data;
			}

			data.pieces = detail::MakeBoundarySource<BoundaryPiece>(shape,
				[&](auto& pieces) { AppendBoundaryPieces(pieces, shape); });
			return data;
		}

		[[nodiscard]]
		ClosestBoundaryCandidate ComputeClosestBoundaryPoint(
			const BoundaryData& data, const Vec2& point)
		{
			ClosestBoundaryCandidate best;

			if (data.pointBoundary)
			{
				UpdateCandidate(best, point, *data.pointBoundary);
				return best;
			}

			(void)detail::AnyBoundaryPiece<true>(data.pieces, [&](const BoundaryPiece& piece)
			{
				const auto candidate = ClosestPointPiece(point, piece);

				if (candidate.distanceSq < best.distanceSq)
				{
					best = candidate;
				}
				return false;
			});

			return best;
		}

		[[nodiscard]]
		bool IsNumericallyOnBoundary(
			const Vec2& query, const Vec2& boundaryPoint,
			const double distance) noexcept
		{
			const double scale = Max({
				Abs(query.x), Abs(query.y),
				Abs(boundaryPoint.x), Abs(boundaryPoint.y),
				1.0
			});
			const double tolerance = (128.0 * DoubleEpsilon * scale);
			return (distance <= tolerance);
		}

		template <class Shape>
		[[nodiscard]]
		Optional<Vec2> ComputeClosestPointOnBoundary(
			const Shape& shape, const Vec2& point)
		{
			const BoundaryData data = MakeBoundaryData(shape);

			if (data.empty)
			{
				return none;
			}

			const ClosestBoundaryCandidate candidate = ComputeClosestBoundaryPoint(data, point);
			assert(std::isfinite(candidate.distanceSq));
			return candidate.point;
		}

		template <class Shape>
		[[nodiscard]]
		double ComputeSignedDistance(const Shape& shape, const Vec2& point)
		{
			const BoundaryData data = MakeBoundaryData(shape);

			if (data.empty)
			{
				return std::numeric_limits<double>::infinity();
			}

			const ClosestBoundaryCandidate candidate = ComputeClosestBoundaryPoint(data, point);
			assert(std::isfinite(candidate.distanceSq));
			const double distance = std::sqrt(Max(0.0, candidate.distanceSq));

			if (IsNumericallyOnBoundary(point, candidate.point, distance))
			{
				return 0.0;
			}

			if (not data.hasPositiveArea)
			{
				return distance;
			}

			return Geometry2D::Intersects(point, shape)
				? -distance
				: distance;
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Rect, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const Rect& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const Rect& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const Rect& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const Rect& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(RectF, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const RectF& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const RectF& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const RectF& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const RectF& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Circle, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const Circle& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const Circle& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const Circle& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const Circle& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Ellipse, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const Ellipse& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const Ellipse& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const Ellipse& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const Ellipse& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(SuperEllipse, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const SuperEllipse& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const SuperEllipse& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const SuperEllipse& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const SuperEllipse& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Triangle, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const Triangle& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const Triangle& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const Triangle& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const Triangle& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Quad, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const Quad& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const Quad& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const Quad& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const Quad& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(RoundRect, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const RoundRect& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const RoundRect& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const RoundRect& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const RoundRect& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(Polygon, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const Polygon& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const Polygon& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const Polygon& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const Polygon& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

		////////////////////////////////////////////////////////////////
		//
		//	ClosestPointOnBoundary / SignedDistance(MultiPolygon, point)
		//
		////////////////////////////////////////////////////////////////

		Optional<Vec2> ClosestPointOnBoundary(const MultiPolygon& shape, const Point& point)
		{
			return ComputeClosestPointOnBoundary(shape, Vec2{ point });
		}

		double SignedDistance(const MultiPolygon& shape, const Point& point)
		{
			return ComputeSignedDistance(shape, Vec2{ point });
		}

		Optional<Vec2> ClosestPointOnBoundary(const MultiPolygon& shape, const Vec2& point)
		{
			return ComputeClosestPointOnBoundary(shape, point);
		}

		double SignedDistance(const MultiPolygon& shape, const Vec2& point)
		{
			return ComputeSignedDistance(shape, point);
		}

	}
}
