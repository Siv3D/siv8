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

# include <array>
# include <span>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include <Siv3D/Geometry2D/Intersects.hpp>
# include <Siv3D/Geometry2D/SignedDistance.hpp>
# include <Siv3D/Geometry2D/Raycast.hpp>
# include "EllipseGeometry.hpp"
# include "PolygonGeometry.hpp"
# include "SuperEllipseGeometry.hpp"

namespace s3d
{
	namespace
	{
		inline constexpr double DoubleEpsilon = 2.2204460492503131e-16;
		inline constexpr double RootTolerance = (128.0 * DoubleEpsilon);
		inline constexpr double CandidateMergeFactor = (1024.0 * DoubleEpsilon);

		enum class ArcRegion : uint8
		{
			Full,
			TopLeft,
			TopRight,
			BottomRight,
			BottomLeft,
		};

		struct LineBoundary
		{
			Line line;
			Vec2 outwardNormal;
			size_t order = 0;
		};

		struct CircleArcBoundary
		{
			Circle circle;
			ArcRegion region = ArcRegion::Full;
			size_t order = 0;
		};

		struct EllipseBoundary
		{
			Ellipse ellipse;
			size_t order = 0;
		};

		struct SuperEllipseBoundary
		{
			SuperEllipse superEllipse;
			size_t order = 0;
		};

		enum class RayHitCandidateKind : uint8
		{
			IsolatedPoint,
			BoundaryOverlap,
		};

		struct RayHitCandidate
		{
			Vec2 position{ 0.0, 0.0 };
			Vec2 normal{ 0.0, 0.0 };
			double distance = std::numeric_limits<double>::infinity();
			size_t order = 0;
			RayHitCandidateKind kind = RayHitCandidateKind::IsolatedPoint;
		};

		struct HitAccumulator
		{
			double maxDistance = std::numeric_limits<double>::infinity();
			double bestDistance = std::numeric_limits<double>::infinity();
			// Defer the origin classification until a hit has been found.
			RayHitCandidate fromOutside;
			RayHitCandidate fromInside;
			bool hasHit = false;
			Optional<bool> startsInside;
		};

		[[nodiscard]]
		constexpr Vec2 RightNormal(const Vec2& direction) noexcept
		{
			return Vec2{ direction.y, -direction.x };
		}

		[[nodiscard]]
		constexpr Vec2 LeftNormal(const Vec2& direction) noexcept
		{
			return Vec2{ -direction.y, direction.x };
		}

		[[nodiscard]]
		Vec2 NormalizeOrZero(const Vec2& value) noexcept
		{
			const double lengthSq = value.dot(value);

			if ((0.0 < lengthSq) && std::isfinite(lengthSq))
			{
				return (value / std::sqrt(lengthSq));
			}

			return Vec2{ 0.0, 0.0 };
		}

		[[nodiscard]]
		double MergeTolerance(const double distance) noexcept
		{
			return (CandidateMergeFactor * Max(1.0, Abs(distance)));
		}

		[[nodiscard]]
		bool PreferCandidate(const RayHitCandidate& candidate, const RayHitCandidate& selected,
			const Ray2D& ray, const bool startsInside) noexcept
		{
			// A shared boundary interval takes precedence over an adjacent vertex.
			if (candidate.kind != selected.kind)
			{
				return (candidate.kind == RayHitCandidateKind::BoundaryOverlap);
			}

			const double selectedDot = ray.direction.dot(selected.normal);
			const double candidateDot = ray.direction.dot(candidate.normal);
			const bool betterFacing = startsInside
				? (selectedDot < candidateDot)
				: (candidateDot < selectedDot);
			return betterFacing || ((Abs(candidateDot - selectedDot) <= RootTolerance)
				&& (candidate.order < selected.order));
		}

		void AppendCandidate(HitAccumulator& accumulator,
			const Ray2D& ray, double distance, const Vec2& normal, const size_t order,
			const RayHitCandidateKind kind = RayHitCandidateKind::IsolatedPoint)
		{
			const double tolerance = MergeTolerance(distance);

			if ((distance < -tolerance)
				|| (accumulator.maxDistance + tolerance < distance))
			{
				return;
			}

			if (distance < 0.0)
			{
				distance = 0.0;
			}

			if (accumulator.maxDistance < distance)
			{
				distance = accumulator.maxDistance;
			}

			const bool replace = (not accumulator.hasHit)
				|| (distance < (accumulator.bestDistance - MergeTolerance(accumulator.bestDistance)));
			if ((not replace) && (MergeTolerance(accumulator.bestDistance) < Abs(distance - accumulator.bestDistance)))
			{
				return;
			}

			const Vec2 unitNormal = NormalizeOrZero(normal);
			if (unitNormal == Vec2{ 0.0, 0.0 })
			{
				return;
			}

			const RayHitCandidate candidate{ ray.pointAt(distance), unitNormal, distance, order, kind };
			if (replace)
			{
				accumulator.bestDistance = distance;
				accumulator.fromOutside = accumulator.fromInside = candidate;
				accumulator.hasHit = true;
				return;
			}

			if (PreferCandidate(candidate, accumulator.fromOutside, ray, false))
			{
				accumulator.fromOutside = candidate;
			}
			if (PreferCandidate(candidate, accumulator.fromInside, ray, true))
			{
				accumulator.fromInside = candidate;
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
		bool ArcContainsPoint(const CircleArcBoundary& arc, const Vec2& point) noexcept
		{
			return ArcContainsDirection(arc.region, (point - arc.circle.center));
		}

		[[nodiscard]]
		double SignedTwiceArea(const std::span<const Vec2> ring) noexcept
		{
			double result = 0.0;

			for (size_t i = 0; i < ring.size(); ++i)
			{
				result += ring[i].cross(ring[(i + 1) % ring.size()]);
			}

			return result;
		}

		template <class Visitor>
		void VisitLineBoundary(Visitor& visitor,
			const Vec2& start, const Vec2& end, const Vec2& outwardNormal)
		{
			if (start == end)
			{
				return;
			}

			visitor(LineBoundary{
				Line{ start, end }, outwardNormal
				});
		}

		template <class Visitor>
		void VisitOrderedRing(Visitor& visitor,
			const std::span<const Vec2> ring, const bool materialIsOnLeft)
		{
			if (ring.size() < 2)
			{
				return;
			}

			for (size_t i = 0; i < ring.size(); ++i)
			{
				const Vec2 start = ring[i];
				const Vec2 end = ring[(i + 1) % ring.size()];
				const Vec2 edge = (end - start);
				VisitLineBoundary(visitor, start, end,
					materialIsOnLeft ? RightNormal(edge) : LeftNormal(edge));
			}
		}

		template <class Visitor>
		void VisitFreeWindingRing(Visitor& visitor, const std::span<const Vec2> ring)
		{
			if (ring.size() < 2)
			{
				return;
			}

			const bool materialIsOnLeft = (0.0 < SignedTwiceArea(ring));
			VisitOrderedRing(visitor, ring, materialIsOnLeft);
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const RectF& shape)
		{
			const double left = shape.pos.x;
			const double top = shape.pos.y;
			const double right = (left + shape.size.x);
			const double bottom = (top + shape.size.y);

			VisitLineBoundary(visitor, Vec2{ left, top }, Vec2{ right, top }, Vec2{ 0.0, -1.0 });
			VisitLineBoundary(visitor, Vec2{ right, top }, Vec2{ right, bottom }, Vec2{ 1.0, 0.0 });
			VisitLineBoundary(visitor, Vec2{ right, bottom }, Vec2{ left, bottom }, Vec2{ 0.0, 1.0 });
			VisitLineBoundary(visitor, Vec2{ left, bottom }, Vec2{ left, top }, Vec2{ -1.0, 0.0 });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Rect& shape)
		{
			VisitBoundaryPieces(visitor, RectF{ shape });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Circle& shape)
		{
			visitor(CircleArcBoundary{ shape, ArcRegion::Full });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Ellipse& shape)
		{
			visitor(EllipseBoundary{ shape });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const SuperEllipse& shape)
		{
			if (shape.n == 1.0)
			{
				const std::array<Vec2, 4> diamond{
					shape.center + Vec2{ 0.0, -shape.axes.y },
					shape.center + Vec2{ shape.axes.x, 0.0 },
					shape.center + Vec2{ 0.0, shape.axes.y },
					shape.center + Vec2{ -shape.axes.x, 0.0 },
				};
				VisitFreeWindingRing(visitor, diamond);
				return;
			}

			if (shape.n == 2.0)
			{
				visitor(EllipseBoundary{ Ellipse{ shape.center, shape.axes } });
				return;
			}

			visitor(SuperEllipseBoundary{ shape });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Triangle& shape)
		{
			VisitFreeWindingRing(visitor, std::array{ shape.p0, shape.p1, shape.p2 });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Quad& shape)
		{
			VisitFreeWindingRing(visitor, std::array{ shape.p0, shape.p1, shape.p2, shape.p3 });
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const RoundRect& shape)
		{
			const double r = detail::GetGeometry2DEffectiveRadius(shape);

			if (r == 0.0)
			{
				VisitBoundaryPieces(visitor, shape.rect);
				return;
			}

			const double left = shape.rect.pos.x;
			const double top = shape.rect.pos.y;
			const double right = (left + shape.rect.size.x);
			const double bottom = (top + shape.rect.size.y);

			VisitLineBoundary(visitor, Vec2{ left + r, top }, Vec2{ right - r, top }, Vec2{ 0.0, -1.0 });
			VisitLineBoundary(visitor, Vec2{ right, top + r }, Vec2{ right, bottom - r }, Vec2{ 1.0, 0.0 });
			VisitLineBoundary(visitor, Vec2{ right - r, bottom }, Vec2{ left + r, bottom }, Vec2{ 0.0, 1.0 });
			VisitLineBoundary(visitor, Vec2{ left, bottom - r }, Vec2{ left, top + r }, Vec2{ -1.0, 0.0 });

			visitor(CircleArcBoundary{
				Circle{ Vec2{ left + r, top + r }, r }, ArcRegion::TopLeft });
			visitor(CircleArcBoundary{
				Circle{ Vec2{ right - r, top + r }, r }, ArcRegion::TopRight });
			visitor(CircleArcBoundary{
				Circle{ Vec2{ right - r, bottom - r }, r }, ArcRegion::BottomRight });
			visitor(CircleArcBoundary{
				Circle{ Vec2{ left + r, bottom - r }, r }, ArcRegion::BottomLeft });
		}

		[[nodiscard]]
		bool HasPositiveArea(const Polygon& shape) noexcept
		{
			const RectF& bounds = shape.boundingRect();
			return (0.0 < bounds.w) && (0.0 < bounds.h)
				&& detail::PolygonRingHasArea(shape.outer(), bounds);
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const Polygon& shape)
		{
			if (not HasPositiveArea(shape))
			{
				return;
			}

			// Valid Siv3D Polygon contours are oriented with material on the left
			// in the screen-space coordinate system. Therefore the right normal is
			// material-outward for both the outer contour and hole contours.
			VisitOrderedRing(visitor, shape.outer(), true);

			for (const auto& inner : shape.inners())
			{
				VisitOrderedRing(visitor, inner, true);
			}
		}

		template <class Visitor>
		void VisitBoundaryPieces(Visitor& visitor, const MultiPolygon& shape)
		{
			for (const auto& polygon : shape)
			{
				VisitBoundaryPieces(visitor, polygon);
			}
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

		template <class Shape>
		[[nodiscard]]
		bool IsRayOriginInside(const Shape& shape, const Vec2& origin)
		{
			if constexpr (std::is_same_v<Shape, Polygon>)
			{
				if (not Geometry2D::Intersects(origin, shape.boundingRect()))
				{
					return false;
				}
			}
			else if (not Geometry2D::Intersects(origin, shape))
			{
				return false;
			}
			// Retain SignedDistance's numerical boundary tolerance for interior points.
			return (Geometry2D::SignedDistance(shape, origin) < 0.0);
		}

		[[nodiscard]]
		bool IsRayOriginInside(const MultiPolygon& shape, const Vec2& origin)
		{
			for (const auto& polygon : shape)
			{
				if (Geometry2D::Intersects(origin, polygon.boundingRect())
					&& HasPositiveArea(polygon) && (Geometry2D::SignedDistance(polygon, origin) < 0.0))
				{
					return true;
				}
			}

			return false;
		}

		void ProcessBoundaryPiece(HitAccumulator& accumulator,
			const Ray2D& ray, const LineBoundary& boundary)
		{
			const Vec2 segmentDirection = (boundary.line.end - boundary.line.start);
			const Vec2 offset = (boundary.line.start - ray.origin);
			const double denominator = ray.direction.cross(segmentDirection);

			if (denominator != 0.0)
			{
				const double distance = (offset.cross(segmentDirection) / denominator);
				const double segmentParameter = (offset.cross(ray.direction) / denominator);

				if (InRange(segmentParameter, -RootTolerance, (1.0 + RootTolerance)))
				{
					AppendCandidate(accumulator, ray, distance,
						NormalizeOrZero(boundary.outwardNormal), boundary.order);
				}
				return;
			}

			if (offset.cross(ray.direction) != 0.0)
			{
				return;
			}

			const double t0 = (boundary.line.start - ray.origin).dot(ray.direction);
			const double t1 = (boundary.line.end - ray.origin).dot(ray.direction);
			const double rawOverlapStart = Max(0.0, Min(t0, t1));
			const double rawOverlapEnd = Max(t0, t1);
			const double overlapEnd = Min(accumulator.maxDistance, rawOverlapEnd);

			if (rawOverlapStart <= overlapEnd)
			{
				const bool hasPositiveLengthOverlap =
					(MergeTolerance(rawOverlapStart) < (rawOverlapEnd - rawOverlapStart));

				AppendCandidate(accumulator, ray, rawOverlapStart,
					NormalizeOrZero(boundary.outwardNormal), boundary.order,
					hasPositiveLengthOverlap
					? RayHitCandidateKind::BoundaryOverlap
					: RayHitCandidateKind::IsolatedPoint);
			}
		}

		[[nodiscard]]
		bool IsOnUnitCircleBoundary(const Vec2& point) noexcept
		{
			return (Abs(point.dot(point) - 1.0) <= (2.0 * detail::EllipseContactTolerance));
		}

		void ProcessBoundaryPiece(HitAccumulator& accumulator,
			const Ray2D& ray, const CircleArcBoundary& boundary)
		{
			const Circle& circle = boundary.circle;
			const Vec2 origin = ((ray.origin - circle.center) / circle.r);
			if (IsOnUnitCircleBoundary(origin) && ArcContainsPoint(boundary, ray.origin))
			{
				AppendCandidate(accumulator, ray, 0.0, origin, boundary.order);
				accumulator.startsInside = false;
				return;
			}
			detail::VisitUnitCircleLineIntersections(origin, ray.direction, 1.0, [&](const double distance, const Vec2& normalized)
			{
				const Vec2 position = (circle.center + normalized * circle.r);
				if (ArcContainsPoint(boundary, position))
				{
					AppendCandidate(accumulator, ray, (distance * circle.r), normalized, boundary.order);
				}
				return true;
			});
		}

		void ProcessBoundaryPiece(HitAccumulator& accumulator,
			const Ray2D& ray, const EllipseBoundary& boundary)
		{
			const Ellipse& ellipse = boundary.ellipse;
			const Vec2 origin = ((ray.origin - ellipse.center) / ellipse.axes);
			if (IsOnUnitCircleBoundary(origin))
			{
				AppendCandidate(accumulator, ray, 0.0, (origin / ellipse.axes), boundary.order);
				accumulator.startsInside = false;
				return;
			}
			const Vec2 direction = (ray.direction / ellipse.axes);
			detail::VisitUnitCircleLineIntersections(origin, direction, direction.lengthSq(), [&](const double distance, const Vec2& normalized)
			{
				AppendCandidate(accumulator, ray, distance, (normalized / ellipse.axes), boundary.order);
				return (not accumulator.hasHit);
			});
		}

		[[nodiscard]]
		bool UpdateSlab(const double origin, const double direction,
			const double minimum, const double maximum,
			double& tMin, double& tMax) noexcept
		{
			if (direction == 0.0)
			{
				return ((minimum <= origin) && (origin <= maximum));
			}

			double t0 = ((minimum - origin) / direction);
			double t1 = ((maximum - origin) / direction);

			if (t1 < t0)
			{
				std::swap(t0, t1);
			}

			tMin = Max(tMin, t0);
			tMax = Min(tMax, t1);
			return (tMin <= tMax);
		}

		[[nodiscard]]
		bool RaySuperEllipseInterval(const Ray2D& ray,
			const SuperEllipse& shape, const double maxDistance,
			double& tMin, double& tMax) noexcept
		{
			tMin = 0.0;
			tMax = maxDistance;

			if (not UpdateSlab(ray.origin.x, ray.direction.x,
				(shape.center.x - shape.axes.x), (shape.center.x + shape.axes.x),
				tMin, tMax))
			{
				return false;
			}

			return UpdateSlab(ray.origin.y, ray.direction.y,
				(shape.center.y - shape.axes.y), (shape.center.y + shape.axes.y),
				tMin, tMax);
		}

		[[nodiscard]]
		Vec2 SuperEllipseNormal(const SuperEllipse& shape, const Vec2& position) noexcept
		{
			const double x = ((position.x - shape.center.x) / shape.axes.x);
			const double y = ((position.y - shape.center.y) / shape.axes.y);
			const double absX = Abs(x);
			const double absY = Abs(y);
			const double exponent = (shape.n - 1.0);
			const double gx = ((x < 0.0) ? -1.0 : 1.0)
				* std::pow(absX, exponent) / shape.axes.x;
			const double gy = ((y < 0.0) ? -1.0 : 1.0)
				* std::pow(absY, exponent) / shape.axes.y;
			const Vec2 gradient{ gx, gy };

			if (std::isfinite(gx) && std::isfinite(gy)
				&& (0.0 < gradient.dot(gradient)))
			{
				return gradient;
			}

			// At a cusp, the center-to-hit radial direction is a deterministic
			// material-outward fallback and is exact on the coordinate axes.
			return (position - shape.center);
		}

		void ProcessBoundaryPiece(HitAccumulator& accumulator,
			const Ray2D& ray, const SuperEllipseBoundary& boundary)
		{
			double tMin = 0.0;
			double tMax = accumulator.maxDistance;

			if (not RaySuperEllipseInterval(ray, boundary.superEllipse,
				accumulator.maxDistance, tMin, tMax))
			{
				return;
			}

			if (tMax < 0.0)
			{
				return;
			}

			tMax = Max(0.0, tMax);

			if (tMax == 0.0)
			{
				const double originDistance = Geometry2D::SignedDistance(boundary.superEllipse, ray.origin);
				accumulator.startsInside = (originDistance < 0.0);
				if (originDistance == 0.0)
				{
					AppendCandidate(accumulator, ray, 0.0,
						SuperEllipseNormal(boundary.superEllipse, ray.origin), boundary.order);
				}
				return;
			}

			const SuperEllipse& shape = boundary.superEllipse;
			const Line line{ ray.origin, ray.pointAt(tMax) };
			const auto Visit = [&](const Vec2& position)
			{
				const double distance = (position - ray.origin).dot(ray.direction);
				AppendCandidate(accumulator, ray, distance,
					SuperEllipseNormal(shape, position), boundary.order);
				return (not accumulator.hasHit);
			};
			if (not Geometry2D::Intersects(line, shape))
			{
				return;
			}
			detail::VisitLineSuperEllipseIntersections(line, shape, [&](const double t)
			{
				return Visit(line.interpolatedPointAt(t));
			});

			// The origin can be numerically on the boundary even when the
			// line solver cannot resolve an isolated root in the clipped interval.
			const double originDistance = Geometry2D::SignedDistance(boundary.superEllipse, ray.origin);
			accumulator.startsInside = (originDistance < 0.0);
			if (originDistance == 0.0)
			{
				AppendCandidate(accumulator, ray, 0.0,
					SuperEllipseNormal(boundary.superEllipse, ray.origin), boundary.order);
			}
		}

		template <class Shape>
		[[nodiscard]]
		Optional<RaycastHit2D> ComputeRaycast(
			const Ray2D& ray, const Shape& shape, const double maxDistance)
		{
			if (maxDistance < 0.0)
			{
				return none;
			}

			const double directionLengthSq = ray.direction.dot(ray.direction);

			if ((directionLengthSq == 0.0) || (not std::isfinite(directionLengthSq)))
			{
				return none;
			}

			if constexpr (not (std::is_same_v<Shape, Polygon> || std::is_same_v<Shape, MultiPolygon>))
			{
				if (not HasPositiveArea(shape))
				{
					return none;
				}
			}

			HitAccumulator accumulator;
			accumulator.maxDistance = maxDistance;
			size_t order = 0;
			auto Visit = [&](auto boundary)
			{
				boundary.order = order++;
				ProcessBoundaryPiece(accumulator, ray, boundary);
			};
			VisitBoundaryPieces(Visit, shape);

			if (not accumulator.hasHit)
			{
				return none;
			}

			const bool startsInside = accumulator.startsInside.has_value()
				? *accumulator.startsInside : IsRayOriginInside(shape, ray.origin);
			const RayHitCandidate& selected = startsInside ? accumulator.fromInside : accumulator.fromOutside;

			return RaycastHit2D{
				selected.position,
				selected.normal,
				selected.distance,
				startsInside
			};
		}
	}

	namespace Geometry2D
	{
		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Rect& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const RectF& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Circle& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Ellipse& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const SuperEllipse& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Triangle& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Quad& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const RoundRect& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const Polygon& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}

		Optional<RaycastHit2D> Raycast(const Ray2D& ray, const MultiPolygon& shape, const double maxDistance)
		{
			return ComputeRaycast(ray, shape, maxDistance);
		}
	}
}
