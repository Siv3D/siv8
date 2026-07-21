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
# include <Siv3D/Polygon.hpp>
# include <Siv3D/MultiPolygon.hpp>
# include <Siv3D/Geometry2D/Geometry2DCommon.hpp>
# include <Siv3D/Geometry2D/IntersectsAt.hpp>
# include <Siv3D/Geometry2D/SignedDistance.hpp>
# include <Siv3D/Geometry2D/Raycast.hpp>

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

		using BoundaryPiece = std::variant<
			LineBoundary,
			CircleArcBoundary,
			EllipseBoundary,
			SuperEllipseBoundary>;

		struct RayHitCandidate
		{
			Vec2 position{ 0.0, 0.0 };
			Vec2 normal{ 0.0, 0.0 };
			double distance = std::numeric_limits<double>::infinity();
			size_t order = 0;
		};

		struct HitAccumulator
		{
			double maxDistance = std::numeric_limits<double>::infinity();
			double bestDistance = std::numeric_limits<double>::infinity();
			Array<RayHitCandidate> candidates;
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

		void AppendCandidate(HitAccumulator& accumulator,
			const Ray2D& ray, double distance, const Vec2& normal, const size_t order)
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

			const Vec2 unitNormal = NormalizeOrZero(normal);

			if (unitNormal == Vec2{ 0.0, 0.0 })
			{
				return;
			}

			if (accumulator.candidates.isEmpty())
			{
				accumulator.bestDistance = distance;
			}
			else if (distance < (accumulator.bestDistance - MergeTolerance(accumulator.bestDistance)))
			{
				accumulator.bestDistance = distance;
				accumulator.candidates.clear();
			}
			else if (MergeTolerance(accumulator.bestDistance) < Abs(distance - accumulator.bestDistance))
			{
				return;
			}

			accumulator.candidates.push_back(RayHitCandidate{
				ray.pointAt(distance), unitNormal, distance, order
			});
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
		double SignedTwiceArea(const Array<Vec2>& ring) noexcept
		{
			double result = 0.0;

			for (size_t i = 0; i < ring.size(); ++i)
			{
				result += ring[i].cross(ring[(i + 1) % ring.size()]);
			}

			return result;
		}

		void AppendLineBoundary(Array<BoundaryPiece>& pieces,
			const Vec2& start, const Vec2& end, const Vec2& outwardNormal)
		{
			if (start == end)
			{
				return;
			}

			pieces.emplace_back(LineBoundary{
				Line{ start, end }, NormalizeOrZero(outwardNormal), pieces.size()
			});
		}

		void AppendOrderedRing(Array<BoundaryPiece>& pieces,
			const Array<Vec2>& ring, const bool materialIsOnLeft)
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
				AppendLineBoundary(pieces, start, end,
					materialIsOnLeft ? RightNormal(edge) : LeftNormal(edge));
			}
		}

		void AppendFreeWindingRing(Array<BoundaryPiece>& pieces, const Array<Vec2>& ring)
		{
			if (ring.size() < 2)
			{
				return;
			}

			const bool materialIsOnLeft = (0.0 < SignedTwiceArea(ring));
			AppendOrderedRing(pieces, ring, materialIsOnLeft);
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const RectF& shape)
		{
			const double left = shape.pos.x;
			const double top = shape.pos.y;
			const double right = (left + shape.size.x);
			const double bottom = (top + shape.size.y);

			AppendLineBoundary(pieces, Vec2{ left, top }, Vec2{ right, top }, Vec2{ 0.0, -1.0 });
			AppendLineBoundary(pieces, Vec2{ right, top }, Vec2{ right, bottom }, Vec2{ 1.0, 0.0 });
			AppendLineBoundary(pieces, Vec2{ right, bottom }, Vec2{ left, bottom }, Vec2{ 0.0, 1.0 });
			AppendLineBoundary(pieces, Vec2{ left, bottom }, Vec2{ left, top }, Vec2{ -1.0, 0.0 });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Rect& shape)
		{
			AppendBoundaryPieces(pieces, RectF{ shape });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Circle& shape)
		{
			pieces.emplace_back(CircleArcBoundary{ shape, ArcRegion::Full, pieces.size() });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Ellipse& shape)
		{
			pieces.emplace_back(EllipseBoundary{ shape, pieces.size() });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const SuperEllipse& shape)
		{
			if (shape.n == 1.0)
			{
				const Array<Vec2> diamond{
					shape.center + Vec2{ 0.0, -shape.axes.y },
					shape.center + Vec2{ shape.axes.x, 0.0 },
					shape.center + Vec2{ 0.0, shape.axes.y },
					shape.center + Vec2{ -shape.axes.x, 0.0 },
				};
				AppendFreeWindingRing(pieces, diamond);
				return;
			}

			pieces.emplace_back(SuperEllipseBoundary{ shape, pieces.size() });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Triangle& shape)
		{
			AppendFreeWindingRing(pieces, Array<Vec2>{ shape.p0, shape.p1, shape.p2 });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Quad& shape)
		{
			AppendFreeWindingRing(pieces, Array<Vec2>{ shape.p0, shape.p1, shape.p2, shape.p3 });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const RoundRect& shape)
		{
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

			AppendLineBoundary(pieces, Vec2{ left + r, top }, Vec2{ right - r, top }, Vec2{ 0.0, -1.0 });
			AppendLineBoundary(pieces, Vec2{ right, top + r }, Vec2{ right, bottom - r }, Vec2{ 1.0, 0.0 });
			AppendLineBoundary(pieces, Vec2{ right - r, bottom }, Vec2{ left + r, bottom }, Vec2{ 0.0, 1.0 });
			AppendLineBoundary(pieces, Vec2{ left, bottom - r }, Vec2{ left, top + r }, Vec2{ -1.0, 0.0 });

			pieces.emplace_back(CircleArcBoundary{
				Circle{ Vec2{ left + r, top + r }, r }, ArcRegion::TopLeft, pieces.size() });
			pieces.emplace_back(CircleArcBoundary{
				Circle{ Vec2{ right - r, top + r }, r }, ArcRegion::TopRight, pieces.size() });
			pieces.emplace_back(CircleArcBoundary{
				Circle{ Vec2{ right - r, bottom - r }, r }, ArcRegion::BottomRight, pieces.size() });
			pieces.emplace_back(CircleArcBoundary{
				Circle{ Vec2{ left + r, bottom - r }, r }, ArcRegion::BottomLeft, pieces.size() });
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const Polygon& shape)
		{
			if (shape.isEmpty())
			{
				return;
			}

			// Valid Siv3D Polygon contours are oriented with material on the left
			// in the screen-space coordinate system. Therefore the right normal is
			// material-outward for both the outer contour and hole contours.
			AppendOrderedRing(pieces, shape.outer(), true);

			for (const auto& inner : shape.inners())
			{
				AppendOrderedRing(pieces, inner, true);
			}
		}

		void AppendBoundaryPieces(Array<BoundaryPiece>& pieces, const MultiPolygon& shape)
		{
			for (const auto& polygon : shape)
			{
				AppendBoundaryPieces(pieces, polygon);
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

		void ProcessRayLine(HitAccumulator& accumulator,
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
						boundary.outwardNormal, boundary.order);
				}
				return;
			}

			if (offset.cross(ray.direction) != 0.0)
			{
				return;
			}

			const double t0 = (boundary.line.start - ray.origin).dot(ray.direction);
			const double t1 = (boundary.line.end - ray.origin).dot(ray.direction);
			const double overlapStart = Max(0.0, Min(t0, t1));
			const double overlapEnd = Min(accumulator.maxDistance, Max(t0, t1));

			if (overlapStart <= overlapEnd)
			{
				AppendCandidate(accumulator, ray, overlapStart,
					boundary.outwardNormal, boundary.order);
			}
		}

		void ProcessRayCircleArc(HitAccumulator& accumulator,
			const Ray2D& ray, const CircleArcBoundary& boundary)
		{
			const Vec2 offset = (ray.origin - boundary.circle.center);
			const double b = (2.0 * offset.dot(ray.direction));
			const double c = (offset.dot(offset) - boundary.circle.r * boundary.circle.r);
			double discriminant = std::fma(b, b, (-4.0 * c));
			const double tolerance = (RootTolerance * (Abs(b * b) + Abs(4.0 * c) + 1.0));

			if (discriminant < -tolerance)
			{
				return;
			}

			if (discriminant < 0.0)
			{
				discriminant = 0.0;
			}

			const double root = std::sqrt(discriminant);
			const std::array<double, 2> roots{
				((-b - root) * 0.5),
				((-b + root) * 0.5)
			};

			for (const double distance : roots)
			{
				const Vec2 position = ray.pointAt(distance);

				if (ArcContainsPoint(boundary, position))
				{
					AppendCandidate(accumulator, ray, distance,
						(position - boundary.circle.center), boundary.order);
				}
			}
		}

		void ProcessRayEllipse(HitAccumulator& accumulator,
			const Ray2D& ray, const EllipseBoundary& boundary)
		{
			const Ellipse& ellipse = boundary.ellipse;
			const Vec2 p{
				((ray.origin.x - ellipse.center.x) / ellipse.axes.x),
				((ray.origin.y - ellipse.center.y) / ellipse.axes.y)
			};
			const Vec2 d{
				(ray.direction.x / ellipse.axes.x),
				(ray.direction.y / ellipse.axes.y)
			};
			const double a = d.dot(d);
			const double b = (2.0 * p.dot(d));
			const double c = (p.dot(p) - 1.0);
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
			const std::array<double, 2> roots{
				((-b - root) / denominator),
				((-b + root) / denominator)
			};

			for (const double distance : roots)
			{
				const Vec2 position = ray.pointAt(distance);
				const Vec2 relative = (position - ellipse.center);
				const Vec2 gradient{
					(relative.x / (ellipse.axes.x * ellipse.axes.x)),
					(relative.y / (ellipse.axes.y * ellipse.axes.y))
				};
				AppendCandidate(accumulator, ray, distance, gradient, boundary.order);
			}
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

		void ProcessRaySuperEllipse(HitAccumulator& accumulator,
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
				if (Geometry2D::SignedDistance(boundary.superEllipse, ray.origin) == 0.0)
				{
					AppendCandidate(accumulator, ray, 0.0,
						SuperEllipseNormal(boundary.superEllipse, ray.origin), boundary.order);
				}
				return;
			}

			const auto intersections = Geometry2D::IntersectsAt(
				Line{ ray.origin, ray.pointAt(tMax) }, boundary.superEllipse);

			if (not intersections.has_value())
			{
				return;
			}

			for (const Vec2& position : *intersections)
			{
				const double distance = (position - ray.origin).dot(ray.direction);
				AppendCandidate(accumulator, ray, distance,
					SuperEllipseNormal(boundary.superEllipse, position), boundary.order);
			}

			// A zero-distance boundary hit may be suppressed by a numerical
			// positive-dimensional classification. Preserve the closed-origin rule.
			if (Geometry2D::SignedDistance(boundary.superEllipse, ray.origin) == 0.0)
			{
				AppendCandidate(accumulator, ray, 0.0,
					SuperEllipseNormal(boundary.superEllipse, ray.origin), boundary.order);
			}
		}

		void ProcessBoundaryPiece(HitAccumulator& accumulator,
			const Ray2D& ray, const BoundaryPiece& piece)
		{
			std::visit([&](const auto& boundary)
			{
				using T = std::decay_t<decltype(boundary)>;

				if constexpr (std::is_same_v<T, LineBoundary>)
				{
					ProcessRayLine(accumulator, ray, boundary);
				}
				else if constexpr (std::is_same_v<T, CircleArcBoundary>)
				{
					ProcessRayCircleArc(accumulator, ray, boundary);
				}
				else if constexpr (std::is_same_v<T, EllipseBoundary>)
				{
					ProcessRayEllipse(accumulator, ray, boundary);
				}
				else
				{
					ProcessRaySuperEllipse(accumulator, ray, boundary);
				}
			}, piece);
		}

		[[nodiscard]]
		RayHitCandidate SelectCandidate(const HitAccumulator& accumulator,
			const Ray2D& ray, const bool startsInside) noexcept
		{
			RayHitCandidate selected = accumulator.candidates.front();
			double selectedDot = ray.direction.dot(selected.normal);

			for (size_t i = 1; i < accumulator.candidates.size(); ++i)
			{
				const RayHitCandidate& candidate = accumulator.candidates[i];
				const double candidateDot = ray.direction.dot(candidate.normal);
				const bool betterFacing = startsInside
					? (selectedDot < candidateDot)
					: (candidateDot < selectedDot);
				const bool equalFacing = (Abs(candidateDot - selectedDot) <= RootTolerance);

				if (betterFacing || (equalFacing && (candidate.order < selected.order)))
				{
					selected = candidate;
					selectedDot = candidateDot;
				}
			}

			return selected;
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

			if (not HasPositiveArea(shape))
			{
				return none;
			}

			Array<BoundaryPiece> pieces;
			AppendBoundaryPieces(pieces, shape);

			if (pieces.isEmpty())
			{
				return none;
			}

			HitAccumulator accumulator;
			accumulator.maxDistance = maxDistance;

			for (const BoundaryPiece& piece : pieces)
			{
				ProcessBoundaryPiece(accumulator, ray, piece);
			}

			if (accumulator.candidates.isEmpty())
			{
				return none;
			}

			const bool startsInside = (Geometry2D::SignedDistance(shape, ray.origin) < 0.0);
			const RayHitCandidate selected = SelectCandidate(accumulator, ray, startsInside);

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
