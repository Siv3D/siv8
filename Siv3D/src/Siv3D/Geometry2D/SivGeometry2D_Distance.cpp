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
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Bezier.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/IntersectsAt.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>

namespace s3d
{
	namespace
	{
		inline constexpr int32 EllipseSegments = 160;
		inline constexpr int32 SuperEllipseSegments = 192;
		inline constexpr int32 Bezier2Segments = 96;
		inline constexpr int32 Bezier3Segments = 128;
		inline constexpr int32 FullCircleSegments = 128;
		inline constexpr int32 QuarterCircleSegments = 40;
		inline constexpr int32 ParameterRefinementIterations = 80;
		inline constexpr double ParameterTolerance = 2.0e-15;

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

		struct ClosestPairCandidate
		{
			Vec2 pointA{ 0.0, 0.0 };
			Vec2 pointB{ 0.0, 0.0 };
			double distanceSq = std::numeric_limits<double>::infinity();
			double parameterA = 0.0;
			double parameterB = 0.0;
		};

		struct ShapeDistanceData
		{
			bool empty = true;
			Optional<Vec2> pointGeometry;
			Array<BoundaryPiece> boundaryPieces;
			Array<Vec2> representativePoints;
		};

		[[nodiscard]]
		constexpr double Square(const double x) noexcept
		{
			return (x * x);
		}

		[[nodiscard]]
		constexpr double ClampUnit(const double value) noexcept
		{
			return Clamp(value, 0.0, 1.0);
		}

		void UpdateCandidate(ClosestPairCandidate& best,
			const Vec2& pointA, const Vec2& pointB,
			const double parameterA = 0.0, const double parameterB = 0.0) noexcept
		{
			const double distanceSq = pointA.distanceFromSq(pointB);

			if (distanceSq < best.distanceSq)
			{
				best.pointA = pointA;
				best.pointB = pointB;
				best.distanceSq = distanceSq;
				best.parameterA = parameterA;
				best.parameterB = parameterB;
			}
		}

		[[nodiscard]]
		bool IsClosedPiece(const BoundaryPiece& piece) noexcept
		{
			return std::visit([](const auto& primitive) noexcept
			{
				using T = std::decay_t<decltype(primitive)>;

				if constexpr (std::is_same_v<T, CircleArc>)
				{
					return (primitive.region == ArcRegion::Full);
				}
				else
				{
					return std::is_same_v<T, Ellipse> || std::is_same_v<T, SuperEllipse>;
				}
			}, piece);
		}

		[[nodiscard]]
		double NormalizeParameter(const BoundaryPiece& piece, double t) noexcept
		{
			if (IsClosedPiece(piece))
			{
				t -= std::floor(t);

				if (t < 0.0)
				{
					t += 1.0;
				}

				return t;
			}

			return ClampUnit(t);
		}

		[[nodiscard]]
		constexpr std::pair<double, double> ArcAngleRange(const ArcRegion region) noexcept
		{
			switch (region)
			{
			case ArcRegion::TopLeft:
				return { Math::Pi, (1.5 * Math::Pi) };
			case ArcRegion::TopRight:
				return { (1.5 * Math::Pi), (2.0 * Math::Pi) };
			case ArcRegion::BottomRight:
				return { 0.0, (0.5 * Math::Pi) };
			case ArcRegion::BottomLeft:
				return { (0.5 * Math::Pi), Math::Pi };
			default:
				return { 0.0, (2.0 * Math::Pi) };
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
		Vec2 PointAt(const Line& primitive, const double t) noexcept
		{
			return primitive.start.lerp(primitive.end, t);
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
		Vec2 PointAt(const Ellipse& primitive, const double t) noexcept
		{
			const double angle = (Math::TwoPi * t);
			return (primitive.center + Vec2{
				primitive.axes.x * std::cos(angle),
				primitive.axes.y * std::sin(angle)
			});
		}

		[[nodiscard]]
		Vec2 PointAt(const SuperEllipse& primitive, const double t) noexcept
		{
			const double angle = (Math::TwoPi * t);
			const double c = std::cos(angle);
			const double s = std::sin(angle);
			const double exponent = (2.0 / primitive.n);
			const double x = std::copysign(std::pow(Abs(c), exponent), c);
			const double y = std::copysign(std::pow(Abs(s), exponent), s);
			return (primitive.center + Vec2{ primitive.axes.x * x, primitive.axes.y * y });
		}

		[[nodiscard]]
		Vec2 PointAt(const Bezier2& primitive, const double t) noexcept
		{
			return primitive.pointAt(t);
		}

		[[nodiscard]]
		Vec2 PointAt(const Bezier3& primitive, const double t) noexcept
		{
			return primitive.pointAt(t);
		}

		[[nodiscard]]
		Vec2 PointAt(const BoundaryPiece& piece, const double t)
		{
			return std::visit([&](const auto& primitive)
			{
				return PointAt(primitive, NormalizeParameter(piece, t));
			}, piece);
		}

		[[nodiscard]]
		int32 SegmentCount(const BoundaryPiece& piece) noexcept
		{
			return std::visit([](const auto& primitive) noexcept -> int32
			{
				using T = std::decay_t<decltype(primitive)>;

				if constexpr (std::is_same_v<T, Line>)
				{
					return 1;
				}
				else if constexpr (std::is_same_v<T, CircleArc>)
				{
					return (primitive.region == ArcRegion::Full)
						? FullCircleSegments : QuarterCircleSegments;
				}
				else if constexpr (std::is_same_v<T, Ellipse>)
				{
					return EllipseSegments;
				}
				else if constexpr (std::is_same_v<T, SuperEllipse>)
				{
					return SuperEllipseSegments;
				}
				else if constexpr (std::is_same_v<T, Bezier2>)
				{
					return Bezier2Segments;
				}
				else
				{
					return Bezier3Segments;
				}
			}, piece);
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestPointOnSegment(
			const Vec2& point, const Vec2& start, const Vec2& end) noexcept
		{
			ClosestPairCandidate result;
			const Vec2 direction = (end - start);
			const double lengthSq = direction.dot(direction);
			double t = 0.0;

			if (lengthSq != 0.0)
			{
				t = ClampUnit(((point - start).dot(direction) / lengthSq));
			}

			const Vec2 closest = (start + direction * t);
			UpdateCandidate(result, point, closest, 0.0, t);
			return result;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestSegmentSegment(
			const Vec2& a0, const Vec2& a1,
			const Vec2& b0, const Vec2& b1) noexcept
		{
			const Vec2 d1 = (a1 - a0);
			const Vec2 d2 = (b1 - b0);
			const Vec2 r = (a0 - b0);
			const double a = d1.dot(d1);
			const double e = d2.dot(d2);
			const double f = d2.dot(r);
			double s = 0.0;
			double t = 0.0;

			if ((a == 0.0) && (e == 0.0))
			{
				ClosestPairCandidate result;
				UpdateCandidate(result, a0, b0, 0.0, 0.0);
				return result;
			}

			if (a == 0.0)
			{
				t = ClampUnit(f / e);
			}
			else
			{
				const double c = d1.dot(r);

				if (e == 0.0)
				{
					s = ClampUnit(-c / a);
				}
				else
				{
					const double b = d1.dot(d2);
					const double denominator = (a * e - b * b);

					if (denominator != 0.0)
					{
						s = ClampUnit((b * f - c * e) / denominator);
					}

					t = ((b * s + f) / e);

					if (t < 0.0)
					{
						t = 0.0;
						s = ClampUnit(-c / a);
					}
					else if (1.0 < t)
					{
						t = 1.0;
						s = ClampUnit((b - c) / a);
					}
				}
			}

			ClosestPairCandidate result;
			UpdateCandidate(result, (a0 + d1 * s), (b0 + d2 * t), s, t);
			return result;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestPointCircleArc(const Vec2& point, const CircleArc& arc) noexcept
		{
			ClosestPairCandidate result;
			const Vec2 direction = (point - arc.circle.center);
			const double length = direction.length();

			if ((length != 0.0) && ArcContainsDirection(arc.region, direction))
			{
				const Vec2 closest = (arc.circle.center + direction * (arc.circle.r / length));
				UpdateCandidate(result, point, closest);
			}

			UpdateCandidate(result, point, PointAt(arc, 0.0), 0.0, 0.0);
			UpdateCandidate(result, point, PointAt(arc, 1.0), 0.0, 1.0);
			return result;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestLineCircleArc(const Line& line, const CircleArc& arc) noexcept
		{
			ClosestPairCandidate best;

			const auto centerProjection = ClosestPointOnSegment(
				arc.circle.center, line.start, line.end);
			const Vec2 linePoint = centerProjection.pointB;
			const Vec2 direction = (linePoint - arc.circle.center);
			const double length = direction.length();

			if ((length != 0.0) && ArcContainsDirection(arc.region, direction))
			{
				const Vec2 arcPoint = (arc.circle.center + direction * (arc.circle.r / length));
				UpdateCandidate(best, linePoint, arcPoint, centerProjection.parameterB, 0.0);
			}

			for (const Vec2& endpoint : { line.start, line.end })
			{
				const auto candidate = ClosestPointCircleArc(endpoint, arc);

				if (candidate.distanceSq < best.distanceSq)
				{
					best = candidate;
				}
			}

			for (const double arcParameter : { 0.0, 1.0 })
			{
				const Vec2 arcPoint = PointAt(arc, arcParameter);
				auto candidate = ClosestPointOnSegment(arcPoint, line.start, line.end);
				std::swap(candidate.pointA, candidate.pointB);
				candidate.parameterA = candidate.parameterB;
				candidate.parameterB = arcParameter;

				if (candidate.distanceSq < best.distanceSq)
				{
					best = candidate;
				}
			}

			return best;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestCircleArcCircleArc(
			const CircleArc& arcA, const CircleArc& arcB) noexcept
		{
			ClosestPairCandidate best;
			const Vec2 centerDirection = (arcB.circle.center - arcA.circle.center);
			const double centerDistance = centerDirection.length();

			if (centerDistance != 0.0)
			{
				const Vec2 unit = (centerDirection / centerDistance);

				for (const double signA : { -1.0, 1.0 })
				{
					for (const double signB : { -1.0, 1.0 })
					{
						const Vec2 directionA = (unit * signA);
						const Vec2 directionB = (unit * signB);

						if (ArcContainsDirection(arcA.region, directionA)
							&& ArcContainsDirection(arcB.region, directionB))
						{
							UpdateCandidate(best,
								arcA.circle.center + directionA * arcA.circle.r,
								arcB.circle.center + directionB * arcB.circle.r);
						}
					}
				}
			}

			for (const double parameterA : { 0.0, 1.0 })
			{
				const Vec2 pointA = PointAt(arcA, parameterA);
				const auto candidate = ClosestPointCircleArc(pointA, arcB);

				if (candidate.distanceSq < best.distanceSq)
				{
					best = candidate;
					best.parameterA = parameterA;
				}
			}

			for (const double parameterB : { 0.0, 1.0 })
			{
				const Vec2 pointB = PointAt(arcB, parameterB);
				auto candidate = ClosestPointCircleArc(pointB, arcA);
				std::swap(candidate.pointA, candidate.pointB);
				std::swap(candidate.parameterA, candidate.parameterB);
				candidate.parameterB = parameterB;

				if (candidate.distanceSq < best.distanceSq)
				{
					best = candidate;
				}
			}

			return best;
		}

		[[nodiscard]]
		ClosestPairCandidate RefinePointPiece(
			const Vec2& point, const BoundaryPiece& piece,
			double parameter, double step)
		{
			ClosestPairCandidate best;
			parameter = NormalizeParameter(piece, parameter);
			UpdateCandidate(best, point, PointAt(piece, parameter), 0.0, parameter);

			for (int32 iteration = 0; iteration < ParameterRefinementIterations; ++iteration)
			{
				bool improved = false;

				for (const double direction : { -1.0, 1.0 })
				{
					const double candidateParameter = NormalizeParameter(piece, parameter + direction * step);
					const Vec2 candidatePoint = PointAt(piece, candidateParameter);
					const double candidateDistanceSq = point.distanceFromSq(candidatePoint);

					if (candidateDistanceSq < best.distanceSq)
					{
						best.pointB = candidatePoint;
						best.distanceSq = candidateDistanceSq;
						best.parameterB = candidateParameter;
						parameter = candidateParameter;
						improved = true;
					}
				}

				if (not improved)
				{
					step *= 0.5;

					if (step <= ParameterTolerance)
					{
						break;
					}
				}
			}

			best.pointA = point;
			return best;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestPointPiece(const Vec2& point, const BoundaryPiece& piece)
		{
			if (const Line* line = std::get_if<Line>(&piece))
			{
				return ClosestPointOnSegment(point, line->start, line->end);
			}

			if (const CircleArc* arc = std::get_if<CircleArc>(&piece))
			{
				if (arc->region == ArcRegion::Full)
				{
					return ClosestPointCircleArc(point, *arc);
				}
			}

			const int32 segments = SegmentCount(piece);
			ClosestPairCandidate seed;
			Vec2 previous = PointAt(piece, 0.0);

			for (int32 i = 0; i < segments; ++i)
			{
				const double t1 = (static_cast<double>(i + 1) / segments);
				const Vec2 current = PointAt(piece, t1);
				const auto local = ClosestPointOnSegment(point, previous, current);

				if (local.distanceSq < seed.distanceSq)
				{
					seed = local;
					seed.parameterB = ((static_cast<double>(i) + local.parameterB) / segments);
				}

				previous = current;
			}

			return RefinePointPiece(point, piece, seed.parameterB, (2.0 / segments));
		}

		[[nodiscard]]
		ClosestPairCandidate RefinePiecePair(
			const BoundaryPiece& pieceA, const BoundaryPiece& pieceB,
			double parameterA, double parameterB,
			double stepA, double stepB)
		{
			ClosestPairCandidate best;
			parameterA = NormalizeParameter(pieceA, parameterA);
			parameterB = NormalizeParameter(pieceB, parameterB);
			UpdateCandidate(best,
				PointAt(pieceA, parameterA), PointAt(pieceB, parameterB),
				parameterA, parameterB);

			for (int32 iteration = 0; iteration < ParameterRefinementIterations; ++iteration)
			{
				bool improved = false;
				double nextA = parameterA;
				double nextB = parameterB;

				for (const int32 da : { -1, 0, 1 })
				{
					for (const int32 db : { -1, 0, 1 })
					{
						if ((da == 0) && (db == 0))
						{
							continue;
						}

						const double candidateA = NormalizeParameter(pieceA, parameterA + da * stepA);
						const double candidateB = NormalizeParameter(pieceB, parameterB + db * stepB);
						const Vec2 pointA = PointAt(pieceA, candidateA);
						const Vec2 pointB = PointAt(pieceB, candidateB);
						const double distanceSq = pointA.distanceFromSq(pointB);

						if (distanceSq < best.distanceSq)
						{
							best.pointA = pointA;
							best.pointB = pointB;
							best.distanceSq = distanceSq;
							best.parameterA = candidateA;
							best.parameterB = candidateB;
							nextA = candidateA;
							nextB = candidateB;
							improved = true;
						}
					}
				}

				if (improved)
				{
					parameterA = nextA;
					parameterB = nextB;
				}
				else
				{
					stepA *= 0.5;
					stepB *= 0.5;

					if ((stepA <= ParameterTolerance) && (stepB <= ParameterTolerance))
					{
						break;
					}
				}
			}

			return best;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestPiecePair(const BoundaryPiece& pieceA, const BoundaryPiece& pieceB)
		{
			if (const Line* lineA = std::get_if<Line>(&pieceA))
			{
				if (const Line* lineB = std::get_if<Line>(&pieceB))
				{
					return ClosestSegmentSegment(lineA->start, lineA->end, lineB->start, lineB->end);
				}

				if (const CircleArc* arcB = std::get_if<CircleArc>(&pieceB))
				{
					return ClosestLineCircleArc(*lineA, *arcB);
				}
			}

			if (const CircleArc* arcA = std::get_if<CircleArc>(&pieceA))
			{
				if (const Line* lineB = std::get_if<Line>(&pieceB))
				{
					auto result = ClosestLineCircleArc(*lineB, *arcA);
					std::swap(result.pointA, result.pointB);
					std::swap(result.parameterA, result.parameterB);
					return result;
				}

				if (const CircleArc* arcB = std::get_if<CircleArc>(&pieceB))
				{
					return ClosestCircleArcCircleArc(*arcA, *arcB);
				}
			}

			const int32 segmentsA = SegmentCount(pieceA);
			const int32 segmentsB = SegmentCount(pieceB);
			ClosestPairCandidate seed;
			Vec2 a0 = PointAt(pieceA, 0.0);

			for (int32 i = 0; i < segmentsA; ++i)
			{
				const Vec2 a1 = PointAt(pieceA, (static_cast<double>(i + 1) / segmentsA));
				Vec2 b0 = PointAt(pieceB, 0.0);

				for (int32 j = 0; j < segmentsB; ++j)
				{
					const Vec2 b1 = PointAt(pieceB, (static_cast<double>(j + 1) / segmentsB));
					const auto local = ClosestSegmentSegment(a0, a1, b0, b1);

					if (local.distanceSq < seed.distanceSq)
					{
						seed = local;
						seed.parameterA = ((static_cast<double>(i) + local.parameterA) / segmentsA);
						seed.parameterB = ((static_cast<double>(j) + local.parameterB) / segmentsB);
					}

					b0 = b1;
				}

				a0 = a1;
			}

			return RefinePiecePair(pieceA, pieceB,
				seed.parameterA, seed.parameterB,
				Min(1.0, (2.0 / segmentsA)),
				Min(1.0, (2.0 / segmentsB)));
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Point&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Vec2&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Line&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const LineString& shape) noexcept
		{
			return shape.isEmpty();
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Bezier2&) noexcept
		{
			return false;
		}

		[[nodiscard]]
		bool IsEmptyGeometry(const Bezier3&) noexcept
		{
			return false;
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
			for (size_t i = 0; (i + 1) < shape.size(); ++i)
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
			if ((shape.p1 - shape.p0).cross(shape.p2 - shape.p0) == 0.0)
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

		void AppendRepresentativePoints(Array<Vec2>& points, const Point& shape)
		{
			points.push_back(Vec2{ shape });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Vec2& shape)
		{
			points.push_back(shape);
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Line& shape)
		{
			points.push_back(shape.start);
			points.push_back(shape.end);
			points.push_back(shape.start.lerp(shape.end, 0.5));
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const LineString& shape)
		{
			for (const Vec2& point : shape)
			{
				points.push_back(point);
			}

			for (size_t i = 0; (i + 1) < shape.size(); ++i)
			{
				points.push_back(shape[i].lerp(shape[i + 1], 0.5));
			}
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Bezier2& shape)
		{
			for (const double t : { 0.0, 0.25, 0.5, 0.75, 1.0 })
			{
				points.push_back(shape.pointAt(t));
			}
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Bezier3& shape)
		{
			for (const double t : { 0.0, 0.25, 0.5, 0.75, 1.0 })
			{
				points.push_back(shape.pointAt(t));
			}
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const RectF& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendRepresentativePoints(points, detail::GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			const double left = shape.pos.x;
			const double top = shape.pos.y;
			const double right = (left + shape.size.x);
			const double bottom = (top + shape.size.y);
			points.push_back(Vec2{ (left + right) * 0.5, (top + bottom) * 0.5 });
			points.push_back(Vec2{ left, top });
			points.push_back(Vec2{ right, top });
			points.push_back(Vec2{ right, bottom });
			points.push_back(Vec2{ left, bottom });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Rect& shape)
		{
			AppendRepresentativePoints(points, RectF{ shape });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Circle& shape)
		{
			if (IsEmptyGeometry(shape))
			{
				return;
			}

			points.push_back(shape.center);
			points.push_back(shape.center + Vec2{ shape.r, 0.0 });
			points.push_back(shape.center + Vec2{ -shape.r, 0.0 });
			points.push_back(shape.center + Vec2{ 0.0, shape.r });
			points.push_back(shape.center + Vec2{ 0.0, -shape.r });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Ellipse& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendRepresentativePoints(points, detail::GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			points.push_back(shape.center);
			points.push_back(shape.center + Vec2{ shape.axes.x, 0.0 });
			points.push_back(shape.center + Vec2{ -shape.axes.x, 0.0 });
			points.push_back(shape.center + Vec2{ 0.0, shape.axes.y });
			points.push_back(shape.center + Vec2{ 0.0, -shape.axes.y });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const SuperEllipse& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendRepresentativePoints(points, detail::GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			points.push_back(shape.center);
			points.push_back(shape.center + Vec2{ shape.axes.x, 0.0 });
			points.push_back(shape.center + Vec2{ -shape.axes.x, 0.0 });
			points.push_back(shape.center + Vec2{ 0.0, shape.axes.y });
			points.push_back(shape.center + Vec2{ 0.0, -shape.axes.y });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Triangle& shape)
		{
			points.push_back(shape.p0);
			points.push_back(shape.p1);
			points.push_back(shape.p2);
			points.push_back((shape.p0 + shape.p1 + shape.p2) / 3.0);
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Quad& shape)
		{
			points.push_back(shape.p0);
			points.push_back(shape.p1);
			points.push_back(shape.p2);
			points.push_back(shape.p3);
			points.push_back((shape.p0 + shape.p1 + shape.p2) / 3.0);
			points.push_back((shape.p0 + shape.p2 + shape.p3) / 3.0);
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const RoundRect& shape)
		{
			const auto kind = detail::ClassifyGeometry2DSizedShape(shape);

			if (kind == detail::Geometry2DSizedShapeKind::Empty)
			{
				return;
			}

			if (detail::IsGeometry2DSegment(kind))
			{
				AppendRepresentativePoints(points, detail::GetGeometry2DDegenerateSegment(shape, kind));
				return;
			}

			const double left = shape.rect.pos.x;
			const double top = shape.rect.pos.y;
			const double right = (left + shape.rect.size.x);
			const double bottom = (top + shape.rect.size.y);
			points.push_back(Vec2{ (left + right) * 0.5, (top + bottom) * 0.5 });
			points.push_back(Vec2{ (left + right) * 0.5, top });
			points.push_back(Vec2{ right, (top + bottom) * 0.5 });
			points.push_back(Vec2{ (left + right) * 0.5, bottom });
			points.push_back(Vec2{ left, (top + bottom) * 0.5 });
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const Polygon& shape)
		{
			if (shape.isEmpty())
			{
				return;
			}

			for (const Vec2& point : shape.outer())
			{
				points.push_back(point);
			}

			const Float2* vertices = shape.vertices().data();

			for (const auto& index : shape.indices())
			{
				const Vec2 p0{ vertices[index.i0].x, vertices[index.i0].y };
				const Vec2 p1{ vertices[index.i1].x, vertices[index.i1].y };
				const Vec2 p2{ vertices[index.i2].x, vertices[index.i2].y };
				points.push_back((p0 + p1 + p2) / 3.0);
			}
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const MultiPolygon& shape)
		{
			for (const auto& polygon : shape)
			{
				AppendRepresentativePoints(points, polygon);
			}
		}

		template <class Shape>
		[[nodiscard]]
		ShapeDistanceData MakeShapeDistanceData(const Shape& shape)
		{
			ShapeDistanceData data;

			if (IsEmptyGeometry(shape))
			{
				return data;
			}

			data.empty = false;
			Vec2 point;

			if (TryGetPointGeometry(shape, point))
			{
				data.pointGeometry = point;
				data.representativePoints.push_back(point);
				return data;
			}

			AppendBoundaryPieces(data.boundaryPieces, shape);
			AppendRepresentativePoints(data.representativePoints, shape);
			return data;
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<Vec2> FindCommonPoint(
			const ShapeA& a, const ShapeB& b,
			const ShapeDistanceData& dataA, const ShapeDistanceData& dataB)
		{
			if (const auto events = Geometry2D::IntersectsAt(a, b))
			{
				if (not events->isEmpty())
				{
					return events->front();
				}
			}

			auto TestPoints = [&](const Array<Vec2>& points) -> Optional<Vec2>
			{
				for (const Vec2& point : points)
				{
					if (Geometry2D::Intersects(point, a)
						&& Geometry2D::Intersects(point, b))
					{
						return point;
					}
				}

				return none;
			};

			if (const auto point = TestPoints(dataA.representativePoints))
			{
				return point;
			}

			if (const auto point = TestPoints(dataB.representativePoints))
			{
				return point;
			}

			for (const BoundaryPiece& piece : dataA.boundaryPieces)
			{
				for (int32 i = 0; i <= 32; ++i)
				{
					const Vec2 point = PointAt(piece, (static_cast<double>(i) / 32.0));

					if (Geometry2D::Intersects(point, b))
					{
						return point;
					}
				}
			}

			for (const BoundaryPiece& piece : dataB.boundaryPieces)
			{
				for (int32 i = 0; i <= 32; ++i)
				{
					const Vec2 point = PointAt(piece, (static_cast<double>(i) / 32.0));

					if (Geometry2D::Intersects(point, a))
					{
						return point;
					}
				}
			}

			const size_t countA = std::min(dataA.representativePoints.size(), static_cast<size_t>(12));
			const size_t countB = std::min(dataB.representativePoints.size(), static_cast<size_t>(12));

			for (size_t i = 0; i < countA; ++i)
			{
				for (size_t j = 0; j < countB; ++j)
				{
					for (int32 k = 1; k < 32; ++k)
					{
						const Vec2 point = dataA.representativePoints[i].lerp(
							dataB.representativePoints[j], (static_cast<double>(k) / 32.0));

						if (Geometry2D::Intersects(point, a)
							&& Geometry2D::Intersects(point, b))
						{
							return point;
						}
					}
				}
			}

			return none;
		}

		[[nodiscard]]
		ClosestPairCandidate ComputeDisjointClosestPair(
			const ShapeDistanceData& dataA, const ShapeDistanceData& dataB)
		{
			ClosestPairCandidate best;

			if (dataA.pointGeometry && dataB.pointGeometry)
			{
				UpdateCandidate(best, *dataA.pointGeometry, *dataB.pointGeometry);
				return best;
			}

			if (dataA.pointGeometry)
			{
				for (const BoundaryPiece& pieceB : dataB.boundaryPieces)
				{
					const auto candidate = ClosestPointPiece(*dataA.pointGeometry, pieceB);

					if (candidate.distanceSq < best.distanceSq)
					{
						best = candidate;
					}
				}

				return best;
			}

			if (dataB.pointGeometry)
			{
				for (const BoundaryPiece& pieceA : dataA.boundaryPieces)
				{
					auto candidate = ClosestPointPiece(*dataB.pointGeometry, pieceA);
					std::swap(candidate.pointA, candidate.pointB);
					std::swap(candidate.parameterA, candidate.parameterB);

					if (candidate.distanceSq < best.distanceSq)
					{
						best = candidate;
					}
				}

				return best;
			}

			for (const BoundaryPiece& pieceA : dataA.boundaryPieces)
			{
				for (const BoundaryPiece& pieceB : dataB.boundaryPieces)
				{
					const auto candidate = ClosestPiecePair(pieceA, pieceB);

					if (candidate.distanceSq < best.distanceSq)
					{
						best = candidate;
					}
				}
			}

			return best;
		}

		template <class T>
		inline constexpr int32 ShapeRank =
			std::is_same_v<T, Point> ? 0
			: std::is_same_v<T, Vec2> ? 1
			: std::is_same_v<T, Line> ? 2
			: std::is_same_v<T, LineString> ? 3
			: std::is_same_v<T, Bezier2> ? 4
			: std::is_same_v<T, Bezier3> ? 5
			: std::is_same_v<T, Rect> ? 6
			: std::is_same_v<T, RectF> ? 7
			: std::is_same_v<T, Circle> ? 8
			: std::is_same_v<T, Ellipse> ? 9
			: std::is_same_v<T, SuperEllipse> ? 10
			: std::is_same_v<T, Triangle> ? 11
			: std::is_same_v<T, Quad> ? 12
			: std::is_same_v<T, RoundRect> ? 13
			: std::is_same_v<T, Polygon> ? 14
			: 15;

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<ClosestPoints2D> ComputeClosestPointsCanonical(const ShapeA& a, const ShapeB& b)
		{
			const ShapeDistanceData dataA = MakeShapeDistanceData(a);
			const ShapeDistanceData dataB = MakeShapeDistanceData(b);

			if (dataA.empty || dataB.empty)
			{
				return none;
			}

			if (Geometry2D::Intersects(a, b))
			{
				if (const auto commonPoint = FindCommonPoint(a, b, dataA, dataB))
				{
					return ClosestPoints2D{ *commonPoint, *commonPoint, 0.0 };
				}

				// A valid intersection always has a common point. The exhaustive witness
				// paths above cover supported valid geometry. This fallback preserves the
				// normative zero distance if an implementation-specific numeric case fails
				// to enumerate a witness.
				assert(false);
				const Vec2 fallback = not dataA.representativePoints.isEmpty()
					? dataA.representativePoints.front()
					: dataB.representativePoints.front();
				return ClosestPoints2D{ fallback, fallback, 0.0 };
			}

			const ClosestPairCandidate candidate = ComputeDisjointClosestPair(dataA, dataB);
			assert(std::isfinite(candidate.distanceSq));
			return ClosestPoints2D{
				candidate.pointA,
				candidate.pointB,
				std::sqrt(Max(0.0, candidate.distanceSq))
			};
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<ClosestPoints2D> ComputeClosestPoints(const ShapeA& a, const ShapeB& b)
		{
			if constexpr (ShapeRank<ShapeB> < ShapeRank<ShapeA>)
			{
				auto result = ComputeClosestPointsCanonical(b, a);

				if (result)
				{
					std::swap(result->pointA, result->pointB);
				}

				return result;
			}
			else
			{
				return ComputeClosestPointsCanonical(a, b);
			}
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		double ComputeDistanceCanonical(const ShapeA& a, const ShapeB& b)
		{
			const ShapeDistanceData dataA = MakeShapeDistanceData(a);
			const ShapeDistanceData dataB = MakeShapeDistanceData(b);

			if (dataA.empty || dataB.empty)
			{
				return std::numeric_limits<double>::infinity();
			}

			if (Geometry2D::Intersects(a, b))
			{
				return 0.0;
			}

			const ClosestPairCandidate candidate = ComputeDisjointClosestPair(dataA, dataB);
			assert(std::isfinite(candidate.distanceSq));
			return std::sqrt(Max(0.0, candidate.distanceSq));
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		double ComputeDistance(const ShapeA& a, const ShapeB& b)
		{
			if constexpr (ShapeRank<ShapeB> < ShapeRank<ShapeA>)
			{
				return ComputeDistanceCanonical(b, a);
			}
			else
			{
				return ComputeDistanceCanonical(a, b);
			}
		}
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Point, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Point& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Point& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Point& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Vec2, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Vec2& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Vec2& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Vec2& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Line, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Line& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Line& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Line& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(LineString, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const LineString& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const LineString& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const LineString& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Bezier2, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Bezier2& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier2& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier2& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Bezier3, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Bezier3& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Bezier3& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Bezier3& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Rect, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Rect& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Rect& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Rect& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(RectF, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const RectF& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RectF& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RectF& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Circle, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Circle& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Circle& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Circle& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Ellipse, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Ellipse& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Ellipse& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Ellipse& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(SuperEllipse, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const SuperEllipse& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const SuperEllipse& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const SuperEllipse& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Triangle, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Triangle& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Triangle& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Triangle& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Quad, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Quad& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Quad& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Quad& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(RoundRect, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const RoundRect& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const RoundRect& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const RoundRect& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(Polygon, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const Polygon& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const Polygon& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const Polygon& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Distance / ClosestPoints(MultiPolygon, _)
		//
		////////////////////////////////////////////////////////////////

		double Distance(const MultiPolygon& a, const Point& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Point& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Vec2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Vec2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Line& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Line& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const LineString& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const LineString& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Bezier2& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Bezier2& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Bezier3& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Bezier3& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Rect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Rect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const RectF& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const RectF& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Circle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Circle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Ellipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Ellipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const SuperEllipse& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const SuperEllipse& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Triangle& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Triangle& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Quad& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Quad& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const RoundRect& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const RoundRect& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const Polygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const Polygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

		double Distance(const MultiPolygon& a, const MultiPolygon& b)
		{
			return ComputeDistance(a, b);
		}

		Optional<ClosestPoints2D> ClosestPoints(const MultiPolygon& a, const MultiPolygon& b)
		{
			return ComputeClosestPoints(a, b);
		}

	}
}

