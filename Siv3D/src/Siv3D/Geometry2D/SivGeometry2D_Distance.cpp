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

# include <algorithm>
# include <array>
# include <tuple>
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
# include "BezierGeometry.hpp"
# include "EllipseGeometry.hpp"
# include "PolygonGeometry.hpp"
# include "SuperEllipseGeometry.hpp"

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
			Optional<Vec2> pointGeometry;
			detail::BoundarySource<BoundaryPiece> boundaryPieces;
		};

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

		// A positive-area ellipse and a disjoint ellipse or point. Reflect into
		// one quadrant and solve for the normal joining their closest points.
		template <class ShapeB>
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointEllipsePair(const Ellipse& a, const ShapeB& b) noexcept
		{
			constexpr bool IsPoint = std::is_same_v<ShapeB, Vec2>;
			const auto [centerB, initialAxesB] = [&]() noexcept
			{
				if constexpr (IsPoint)
				{
					return std::pair{ b, Vec2{ 0, 0 } };
				}
				else
				{
					return std::pair{ b.center, b.axes };
				}
			}();
			Vec2 delta = (centerB - a.center);
			const Vec2 sign{ std::copysign(1.0, delta.x), std::copysign(1.0, delta.y) };
			delta = { Abs(delta.x), Abs(delta.y) };
			ClosestPairCandidate result;
			if ((delta.x == 0.0) || (delta.y == 0.0))
			{
				const Vec2 normal = ((delta.x == 0.0) ? Vec2{ 0, sign.y } : Vec2{ sign.x, 0 });
				UpdateCandidate(result, (a.center + a.axes * normal), (centerB - initialAxesB * normal));
				return result;
			}

			const auto Support = [](const Vec2& axes, const double t) noexcept
			{
				const double h = std::hypot(axes.x, (axes.y * t));
				const double k = (axes.x * (axes.y / h));
				return std::pair{ Vec2{ (axes.x / h * axes.x), (axes.y * t / h * axes.y) }, (k * k / h) };
			};
			const auto SupportB = [&](const Vec2& axes, const double t) noexcept
			{
				if constexpr (IsPoint)
				{
					return std::pair{ Vec2{ 0, 0 }, 0.0 };
				}
				else
				{
					return Support(axes, t);
				}
			};
			Vec2 axesA = a.axes, axesB = initialAxesB;
			const Vec2 diagonalGap = (delta - (Support(axesA, 1.0).first + SupportB(axesB, 1.0).first));
			const bool transpose = (diagonalGap.x < diagonalGap.y);
			if (transpose)
			{
				std::swap(delta.x, delta.y);
				std::swap(axesA.x, axesA.y);
				std::swap(axesB.x, axesB.y);
			}

			// The normal (1, t), 0 <= t <= 1, avoids loss of precision near either
			// axis. For support sum s(t), solve (delta - s(t)).dot(-t, 1) = 0.
			// Outside the convex sum this has one root, with a negative derivative.
			double lower = 0.0, upper = 1.0;
			double t = Min((delta.y / delta.x), 1.0);
			const double tolerance = (16.0 * std::numeric_limits<double>::epsilon()
				* Max({ delta.x, delta.y, axesA.x, axesA.y, axesB.x, axesB.y }));
			Vec2 pointA, pointB;
			for (int32 iteration = 0; iteration < 64; ++iteration)
			{
				const auto [supportA, derivativeA] = Support(axesA, t);
				const auto [supportB, derivativeB] = SupportB(axesB, t);
				pointA = supportA;
				pointB = supportB;
				const Vec2 gap = (delta - (pointA + pointB));
				const double f = (gap.y - t * gap.x);
				if (Abs(f) <= tolerance)
				{
					break;
				}
				if (0.0 < f)
				{
					lower = t;
				}
				else
				{
					upper = t;
				}
				const double derivative = (-gap.x - (derivativeA + derivativeB) * (1.0 + t * t));
				const double next = (t - f / derivative);
				t = (((lower < next) && (next < upper)) ? next : ((lower + upper) * 0.5));
			}
			if (transpose)
			{
				std::swap(pointA.x, pointA.y);
				std::swap(pointB.x, pointB.y);
			}
			UpdateCandidate(result, (a.center + sign * pointA), (centerB - sign * pointB));
			return result;
		}

		template <class Support, class ClosestPoint>
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointLineConvexShape(const Line& line, const Vec2& center,
			const Support& supportPoint, const ClosestPoint& closestPoint) noexcept
		{
			const Vec2 direction = (line.end - line.start);
			const double lengthSq = direction.lengthSq();
			if (lengthSq != 0.0)
			{
				Vec2 normal{ -direction.y, direction.x };
				if ((line.start - center).dot(normal) < 0.0)
				{
					normal = -normal;
				}
				const Vec2 support = supportPoint(normal);
				const double t = ((support - line.start).dot(direction) / lengthSq);
				// An interior minimum has a separating tangent parallel to the line.
				if ((0.0 <= t) && (t <= 1.0) && (0.0 <= (line.start - support).dot(normal)))
				{
					ClosestPairCandidate result;
					UpdateCandidate(result, (line.start + direction * t), support, t);
					return result;
				}
			}

			auto result = closestPoint(line.start);
			if (lengthSq != 0.0)
			{
				const auto end = closestPoint(line.end);
				if (end.distanceSq < result.distanceSq)
				{
					result = end;
					result.parameterB = 1.0;
				}
			}
			std::swap(result.pointA, result.pointB);
			std::swap(result.parameterA, result.parameterB);
			return result;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointLineEllipse(const Line& line, const Ellipse& ellipse) noexcept
		{
			return ClosestDisjointLineConvexShape(line, ellipse.center,
				[&](const Vec2& normal) noexcept { return (ellipse.center + ellipse.axes * (ellipse.axes * normal).normalized()); },
				[&](const Vec2& point) noexcept { return ClosestDisjointEllipsePair(ellipse, point); });
		}

		template <bool IsPoint = false>
		struct ConvexSuperEllipseSupport
		{
			Vec2 axes;
			double n, power, inverseN, powerRatio, logAxisRatio;

			ConvexSuperEllipseSupport(const Vec2& axes_, const double n_, const double power_) noexcept
				: axes{ axes_ }, n{ n_ }, power{ power_ }, inverseN{ (1.0 / n) }
				, powerRatio{ (power / (n - 1.0)) }, logAxisRatio{ (IsPoint ? 0.0 : std::log(axes.y / axes.x)) } {}

			// Support point for normal (1, t^power), and its y derivative in t.
			[[nodiscard]]
			std::pair<Vec2, double> sample(const double t, const double normalY) const noexcept
			{
				if constexpr (IsPoint)
				{
					return { Vec2{ 0, 0 }, 0.0 };
				}

				if (n == 2.0)
				{
					const double h = std::hypot(axes.x, (axes.y * normalY));
					const double k = (axes.x * (axes.y / h));
					return { Vec2{ (axes.x / h * axes.x), (axes.y * normalY / h * axes.y) },
						(k * k / h * (power * normalY / t)) };
				}
				const double logRatio = ((logAxisRatio + power * std::log(t)) / (n - 1.0));
				const double ratio = std::exp(-Abs(logRatio));
				const double term = std::pow(ratio, n);
				const double factor = std::pow((1.0 + term), -inverseN);
				const Vec2 point = ((logRatio <= 0.0)
					? Vec2{ (axes.x * factor), (axes.y * ratio * factor) }
					: Vec2{ (axes.x * ratio * factor), (axes.y * factor) });
				return { point, (point.y * powerRatio / (t * (1.0 + term)) * ((logRatio <= 0.0) ? 1.0 : term)) };
			}
		};

		// A positive-area shape with n > 1 and a disjoint convex shape or point.
		template <class ShapeB>
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointConvexSuperEllipsePair(const SuperEllipse& a, const ShapeB& b) noexcept
		{
			constexpr bool IsPoint = std::is_same_v<ShapeB, Vec2>;
			const auto [centerB, axesB, nB] = [&]() noexcept
			{
				if constexpr (IsPoint)
				{
					return std::tuple{ b, Vec2{ 0, 0 }, 2.0 };
				}
				else
				{
					return std::tuple{ b.center, b.axes, b.n };
				}
			}();
			if ((a.n == 2.0) && (nB == 2.0))
			{
				if constexpr (IsPoint)
				{
					return ClosestDisjointEllipsePair(Ellipse{ a.center, a.axes }, b);
				}
				else
				{
					return ClosestDisjointEllipsePair(Ellipse{ a.center, a.axes }, Ellipse{ centerB, axesB });
				}
			}
			Vec2 delta = (centerB - a.center);
			const Vec2 sign{ std::copysign(1.0, delta.x), std::copysign(1.0, delta.y) };
			delta = { Abs(delta.x), Abs(delta.y) };
			ClosestPairCandidate result;
			if ((delta.x == 0.0) || (delta.y == 0.0))
			{
				const Vec2 normal = ((delta.x == 0.0) ? Vec2{ 0, sign.y } : Vec2{ sign.x, 0 });
				UpdateCandidate(result, (a.center + a.axes * normal), (centerB - axesB * normal));
				return result;
			}

			// Raising t to this power keeps support positions well resolved near
			// the axes, where an ordinary normal angle becomes too small for n > 2.
			const double power = Max({ 1.0, (a.n - 1.0), (nB - 1.0) });
			ConvexSuperEllipseSupport<> p{ a.axes, a.n, power };
			ConvexSuperEllipseSupport<IsPoint> q{ axesB, nB, power };
			Vec2 upperA = p.sample(1.0, 1.0).first, upperB = q.sample(1.0, 1.0).first;
			const Vec2 diagonalGap = (delta - upperA - upperB);
			const bool transpose = (diagonalGap.x < diagonalGap.y);
			if (transpose)
			{
				std::swap(delta.x, delta.y);
				std::swap(p.axes.x, p.axes.y);
				std::swap(q.axes.x, q.axes.y);
				p.logAxisRatio = -p.logAxisRatio;
				q.logAxisRatio = -q.logAxisRatio;
				std::swap(upperA.x, upperA.y);
				std::swap(upperB.x, upperB.y);
			}
			Vec2 lowerA{ p.axes.x, 0.0 }, lowerB{ q.axes.x, 0.0 };
			Vec2 bestA, bestB;
			double lower = 0.0, upper = 1.0, previousStep = 1.0;
			double t = std::pow(Min((delta.y / delta.x), 1.0), (1.0 / power));
			const double tolerance = ((IsPoint ? 4.0 : 16.0) * std::numeric_limits<double>::epsilon()
				* Max({ delta.x, delta.y, a.a, a.b, axesB.x, axesB.y }));
			bool converged = false;
			for (int32 iteration = 0; iteration < 32; ++iteration)
			{
				const double normalY = std::pow(t, power);
				const auto [pointA, derivativeA] = p.sample(t, normalY);
				const auto [pointB, derivativeB] = q.sample(t, normalY);
				const Vec2 gap = (delta - pointA - pointB);
				const double f = (gap.y - normalY * gap.x);
				if (Abs(f) <= tolerance)
				{
					bestA = pointA;
					bestB = pointB;
					converged = true;
					break;
				}
				if (0.0 < f)
				{
					lower = t;
					lowerA = pointA;
					lowerB = pointB;
				}
				else
				{
					upper = t;
					upperA = pointA;
					upperB = pointB;
				}
				const double derivative = (-power * normalY / t * gap.x - (1.0 + normalY * normalY) * (derivativeA + derivativeB));
				const double newton = (t - f / derivative);
				// Bisect if Newton steps are not shrinking, as can happen on thin shapes.
				const double next = (((lower < newton) && (newton < upper) && (Abs(newton - t) < (previousStep * 0.5)))
					? newton : ((lower + upper) * 0.5));
				previousStep = Abs(next - t);
				t = next;
				if (not ((lower < t) && (t < upper)))
				{
					break;
				}
			}
			// Interpolate the bracket's support pairs together. These witnesses
			// stay in the convex shapes even when a nearly flat face limits convergence.
			if (not converged)
			{
				const auto segment = ClosestPointOnSegment(Vec2{ 0, 0 }, (delta - lowerA - lowerB), (delta - upperA - upperB));
				bestA = (lowerA + (upperA - lowerA) * segment.parameterB);
				bestB = (lowerB + (upperB - lowerB) * segment.parameterB);
			}
			if (transpose)
			{
				std::swap(bestA.x, bestA.y);
				std::swap(bestB.x, bestB.y);
			}
			UpdateCandidate(result, (a.center + sign * bestA), (centerB - sign * bestB));
			return result;
		}

		// Positive axes and 0 < n < 1. Parameterize each half arc by its larger
		// normalized coordinate, keeping the normalized profile's slope bounded.
		struct ConcaveSuperEllipseProfile
		{
			Vec2 axes;
			double n, inverseN, split;

			explicit ConcaveSuperEllipseProfile(const SuperEllipse& shape) noexcept
				: axes{ shape.axes }, n{ shape.n }, inverseN{ (1.0 / n) }, split{ std::pow(0.5, inverseN) } {}

			[[nodiscard]]
			double yAt(const double x) const noexcept
			{
				return std::pow(Max(0.0, (1.0 - std::pow(x, n))), inverseN);
			}

			[[nodiscard]]
			Vec2 pointAt(const double x, const bool transpose) const noexcept
			{
				const double y = yAt(x);
				return (axes * (transpose ? Vec2{ y, x } : Vec2{ x, y }));
			}

			[[nodiscard]]
			std::pair<double, double> derivatives(const double x, const double y) const noexcept
			{
				const double slope = -std::pow((y / x), (1.0 - n));
				return { slope, (-(1.0 - n) * slope / (x * std::pow(y, n))) };
			}

			// Clip the arc to the rectangle between the origin and a first-quadrant point.
			[[nodiscard]]
			std::pair<double, double> rangeTo(const Vec2& point, const bool transpose) const noexcept
			{
				const Vec2 a = (transpose ? Vec2{ axes.y, axes.x } : axes);
				const Vec2 p = (transpose ? Vec2{ point.y, point.x } : point);
				return { Max(split, yAt(Min((p.y / a.y), 1.0))), Min((p.x / a.x), 1.0) };
			}
		};

		// Positive axes and 0 < n < 1; the point is outside the filled shape.
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointPointConcaveSuperEllipse(const Vec2& point, const SuperEllipse& shape) noexcept
		{
			const Vec2 delta = (point - shape.center), query{ Abs(delta.x), Abs(delta.y) };
			const ConcaveSuperEllipseProfile profile{ shape };
			Vec2 best{ shape.a, 0.0 };
			double bestDistanceSq = query.distanceFromSq(best);
			bool bestTranspose = false;
			auto Update = [&](const Vec2& p, const bool transpose) noexcept
			{
				const double distanceSq = query.distanceFromSq(p);
				if (distanceSq <= bestDistanceSq)
				{
					best = p;
					bestDistanceSq = distanceSq;
					bestTranspose = transpose;
				}
			};
			Update(Vec2{ 0.0, shape.b }, true);
			struct Interval
			{
				Vec2 lower, upper;
				double bound;
				bool transpose;
			};
			constexpr int32 MaxSubdivisions = 32;
			std::array<Interval, MaxSubdivisions + 2> queue;
			size_t count = 0;
			const auto Compare = [](const Interval& a, const Interval& b) noexcept { return (a.bound > b.bound); };
			auto Push = [&](const Vec2& lower, const Vec2& upper, const bool transpose) noexcept
			{
				// The clipped arc is southwest of its chord, and the query is
				// northeast of the entire arc. Distance to the chord is a lower bound.
				const double bound = ClosestPointOnSegment(query, lower, upper).distanceSq;
				if (bound < bestDistanceSq)
				{
					queue[count++] = { lower, upper, bound, transpose };
					std::push_heap(queue.begin(), (queue.begin() + count), Compare);
				}
			};
			for (const bool transpose : { false, true })
			{
				// A nearest point cannot have either coordinate greater than the query.
				const auto [lower, upper] = profile.rangeTo(query, transpose);
				if (upper < lower)
				{
					continue;
				}
				const Vec2 p0 = profile.pointAt(lower, transpose), p1 = profile.pointAt(upper, transpose);
				Update(p0, transpose);
				Update(p1, transpose);
				Push(p0, p1, transpose);
			}
			const double tolerance = (16.0 * std::numeric_limits<double>::epsilon()
				* Max({ shape.a, shape.b, query.x, query.y }));
			for (int32 iteration = 0; (iteration < MaxSubdivisions) && count; ++iteration)
			{
				std::pop_heap(queue.begin(), (queue.begin() + count), Compare);
				const Interval interval = queue[--count];
				if ((bestDistanceSq - interval.bound) <= (tolerance * (2.0 * std::sqrt(bestDistanceSq) + tolerance)))
				{
					break;
				}
				const double middle = (interval.transpose
					? ((interval.lower.y + interval.upper.y) / (2.0 * shape.b))
					: ((interval.lower.x + interval.upper.x) / (2.0 * shape.a)));
				const Vec2 p = profile.pointAt(middle, interval.transpose);
				Update(p, interval.transpose);
				Push(interval.lower, p, interval.transpose);
				Push(p, interval.upper, interval.transpose);
			}
			// Refine the selected minimum without extending the global search budget.
			// Near an evolute the squared distance can be almost flat; retain the
			// evaluated boundary point even if Newton cannot improve it.
			const bool transpose = bestTranspose;
			const Vec2 axes = (transpose ? Vec2{ shape.b, shape.a } : shape.axes);
			const Vec2 q = (transpose ? Vec2{ query.y, query.x } : query);
			double x = (transpose ? (best.y / shape.b) : (best.x / shape.a));
			for (int32 iteration = 0; iteration < 8; ++iteration)
			{
				const double y = profile.yAt(x);
				if ((x == 0.0) || (y == 0.0))
				{
					break;
				}
				const auto [slope, curvature] = profile.derivatives(x, y);
				const Vec2 gap = (axes * Vec2{ x, y } - q);
				const double f = (gap.x * axes.x + gap.y * axes.y * slope);
				const double derivative = (axes.x * axes.x + axes.y * axes.y * slope * slope + gap.y * axes.y * curvature);
				if (derivative <= 0.0)
				{
					break;
				}
				const double next = (x - f / derivative);
				if (not ((profile.split < next) && (next < 1.0)) || (next == x))
				{
					break;
				}
				x = next;
				Update(profile.pointAt(x, transpose), transpose);
			}
			ClosestPairCandidate result;
			UpdateCandidate(result, point, (shape.center + Vec2{ std::copysign(best.x, delta.x), std::copysign(best.y, delta.y) }));
			return result;
		}

		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointPointSuperEllipse(const Vec2& point, const SuperEllipse& shape) noexcept
		{
			if (shape.n < 1.0)
			{
				return ClosestDisjointPointConcaveSuperEllipse(point, shape);
			}
			if (shape.n == 1.0)
			{
				return ClosestPointOnSegment(point,
					(shape.center + Vec2{ std::copysign(shape.a, (point.x - shape.x)), 0.0 }),
					(shape.center + Vec2{ 0.0, std::copysign(shape.b, (point.y - shape.y)) }));
			}
			auto result = ClosestDisjointConvexSuperEllipsePair(shape, point);
			std::swap(result.pointA, result.pointB);
			return result;
		}

		// Bounded search for positive axes and 0 < a.n < 1 < b.n.
		[[nodiscard]]
		ClosestPairCandidate ClosestMixedSuperEllipsePair(const SuperEllipse& a, const SuperEllipse& originalB) noexcept
		{
			const Vec2 delta = (originalB.center - a.center);
			const Vec2 sign{ std::copysign(1.0, delta.x), std::copysign(1.0, delta.y) };
			const SuperEllipse b{ Vec2{ Abs(delta.x), Abs(delta.y) }, originalB.axes, originalB.n };
			const ConcaveSuperEllipseProfile profile{ a };
			const double tolerance = (64.0 * std::numeric_limits<double>::epsilon()
				* Max({ a.a, a.b, b.a, b.b, b.x, b.y }));
			const ConvexSuperEllipseSupport<> direct{ b.axes, b.n, 1.0 }, swapped{ Vec2{ b.b, b.a }, b.n, 1.0 };
			const auto SupportB = [&](const Vec2& normal) noexcept
			{
				if (normal.x == 0.0) return Vec2{ 0.0, b.b };
				if (normal.y == 0.0) return Vec2{ b.a, 0.0 };
				const bool transpose = (normal.x < normal.y);
				const double t = (transpose ? (normal.x / normal.y) : (normal.y / normal.x));
				const Vec2 point = (transpose ? swapped : direct).sample(t, t).first;
				return (transpose ? Vec2{ point.y, point.x } : point);
			};
			struct Sample
			{
				ClosestPairCandidate pair;
				Vec2 normal;
				double support;
			};
			ClosestPairCandidate best;
			double bestX = 1.0;
			bool bestTranspose = false;
			const auto Evaluate = [&](const double x, const bool transpose) noexcept
			{
				const Vec2 point = profile.pointAt(x, transpose);
				const Vec2 normalized{ Abs((point.x - b.x) / b.a), Abs((point.y - b.y) / b.b) };
				ClosestPairCandidate candidate;
				// A prior intersection search can be unresolved. Only an evaluated
				// point in both shapes establishes a zero-distance witness here.
				if ((std::pow(normalized.x, b.n) + std::pow(normalized.y, b.n)) <= 1.0)
				{
					UpdateCandidate(candidate, point, point);
				}
				else
				{
					candidate = ClosestDisjointPointSuperEllipse(point, b);
				}
				if (candidate.distanceSq < best.distanceSq)
				{
					best = candidate;
					bestX = x;
					bestTranspose = transpose;
				}
				Vec2 normal{ Max(0.0, (candidate.pointB.x - point.x)), Max(0.0, (candidate.pointB.y - point.y)) };
				const double length = normal.length();
				double support = 0.0;
				if (length != 0.0)
				{
					normal /= length;
					// The point solver may return an interior witness at its limit;
					// obtain the supporting line from the support function instead.
					support = (b.center - SupportB(normal)).dot(normal);
				}
				return Sample{ candidate, normal, support };
			};
			struct Interval
			{
				double lower, upper, bound;
				Sample left, right;
				bool transpose;
			};
			constexpr int32 MaxSubdivisions = 32;
			std::array<Interval, MaxSubdivisions + 2> queue;
			size_t count = 0;
			const auto Compare = [](const Interval& a, const Interval& b) noexcept { return (a.bound > b.bound); };
			const auto Push = [&](const double lower, const double upper,
				const Sample& left, const Sample& right, const bool transpose) noexcept
			{
				const Vec2 p0 = left.pair.pointA, p1 = right.pair.pointA;
				Vec2 normal{ Abs(p1.y - p0.y), Abs(p1.x - p0.x) };
				const double length = normal.length();
				if (length == 0.0)
				{
					return;
				}
				normal /= length;
				// The concave arc is southwest of its chord. The gap to a parallel
				// supporting line of B bounds the whole interval. Reuse the endpoint
				// supporting lines too, avoiding weak chord bounds near axial tips.
				const double bound = Max({ 0.0, ((b.center - SupportB(normal) - p0).dot(normal) - tolerance),
					(left.support - Max(p0.dot(left.normal), p1.dot(left.normal)) - tolerance),
					(right.support - Max(p0.dot(right.normal), p1.dot(right.normal)) - tolerance) });
				if ((bound * bound) < best.distanceSq)
				{
					queue[count++] = { lower, upper, (bound * bound), left, right, transpose };
					std::push_heap(queue.begin(), (queue.begin() + count), Compare);
				}
			};
			Evaluate(1.0, false);
			Evaluate(1.0, true);
			for (const bool transpose : { false, true })
			{
				// Axis symmetry allows a nearest pair between the two centers.
				const auto [lower, upper] = profile.rangeTo(b.center, transpose);
				if (upper < lower)
				{
					continue;
				}
				const auto left = Evaluate(lower, transpose), right = Evaluate(upper, transpose);
				Push(lower, upper, left, right, transpose);
			}
			for (int32 iteration = 0; (iteration < MaxSubdivisions) && count; ++iteration)
			{
				if ((best.distanceSq - queue[0].bound) <= (4.0 * tolerance * (2.0 * std::sqrt(best.distanceSq) + 4.0 * tolerance)))
				{
					break;
				}
				std::pop_heap(queue.begin(), (queue.begin() + count), Compare);
				const Interval interval = queue[--count];
				const double middle = (interval.lower + (interval.upper - interval.lower) * 0.5);
				if (not ((interval.lower < middle) && (middle < interval.upper)))
				{
					continue;
				}
				const auto sample = Evaluate(middle, interval.transpose);
				Push(interval.lower, middle, interval.left, sample, interval.transpose);
				Push(middle, interval.upper, sample, interval.right, interval.transpose);
			}
			// Refine the selected minimum using the Hessian of point-to-convex
			// squared distance. Retain evaluated witnesses if Newton cannot improve.
			double x = bestX;
			const bool transpose = bestTranspose;
			auto current = best;
			for (int32 iteration = 0; iteration < 8; ++iteration)
			{
				const double y = profile.yAt(x);
				if ((x == 0.0) || (y == 0.0))
				{
					break;
				}
				const Vec2 gap = (current.pointA - current.pointB);
				const double distance = gap.length();
				if (distance == 0.0)
				{
					break;
				}
				const auto [slope, secondDerivative] = profile.derivatives(x, y);
				const Vec2 dp = (a.axes * (transpose ? Vec2{ slope, 1.0 } : Vec2{ 1.0, slope }));
				const Vec2 ddp = (a.axes * (transpose ? Vec2{ secondDerivative, 0.0 } : Vec2{ 0.0, secondDerivative }));
				const Vec2 uv{ Abs((current.pointB.x - b.x) / b.a), Abs((current.pointB.y - b.y) / b.b) };
				const Vec2 gradient{ (std::pow(uv.x, (b.n - 1.0)) / b.a), (std::pow(uv.y, (b.n - 1.0)) / b.b) };
				const double g = gradient.length();
				const double curvature = ((b.n - 1.0) * std::pow((uv.x * uv.y), (b.n - 2.0))
					/ (b.a * b.a * b.b * b.b * g * g * g));
				const Vec2 tangent{ (-gap.y / distance), (gap.x / distance) };
				const double projection = dp.dot(tangent);
				const double derivative = (dp.lengthSq() - projection * projection / (1.0 + distance * curvature) + gap.dot(ddp));
				if (derivative <= 0.0)
				{
					break;
				}
				const double next = (x - gap.dot(dp) / derivative);
				if (not ((profile.split < next) && (next < 1.0)) || (next == x))
				{
					break;
				}
				x = next;
				current = Evaluate(x, transpose).pair;
			}
			best.pointA = (a.center + sign * best.pointA);
			best.pointB = (a.center + sign * best.pointB);
			return best;
		}

		[[nodiscard]]
		constexpr std::array<Vec2, 4> SuperEllipseAxisVertices(const SuperEllipse& shape) noexcept
		{
			return { Vec2{ (shape.x + shape.a), shape.y }, Vec2{ shape.x, (shape.y + shape.b) },
				Vec2{ (shape.x - shape.a), shape.y }, Vec2{ shape.x, (shape.y - shape.b) } };
		}

		[[nodiscard]]
		double DistanceSqToSuperEllipseBox(const Vec2& point, const SuperEllipse& shape) noexcept
		{
			const Vec2 gap{ Max(0.0, (Abs(point.x - shape.x) - shape.a)), Max(0.0, (Abs(point.y - shape.y) - shape.b)) };
			return gap.lengthSq();
		}

		// Disjoint positive-area shapes with n <= 1. Facing concave arcs cannot
		// have a strict distance minimum in both interiors. A minimizing pair
		// can be chosen with an axial tip, including for diamond edges.
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointConcaveSuperEllipsePair(const SuperEllipse& a, const SuperEllipse& b) noexcept
		{
			const auto verticesA = SuperEllipseAxisVertices(a), verticesB = SuperEllipseAxisVertices(b);
			ClosestPairCandidate result;
			for (const Vec2& pointA : verticesA)
			{
				for (const Vec2& pointB : verticesB)
				{
					UpdateCandidate(result, pointA, pointB);
				}
			}
			const auto TestTips = [&](const auto& vertices, const SuperEllipse& other, const bool reverse) noexcept
			{
				for (const Vec2& point : vertices)
				{
					if (result.distanceSq <= DistanceSqToSuperEllipseBox(point, other))
					{
						continue;
					}
					const auto closest = ClosestDisjointPointSuperEllipse(point, other);
					if (reverse) UpdateCandidate(result, closest.pointB, point);
					else UpdateCandidate(result, point, closest.pointB);
				}
			};
			TestTips(verticesA, b, false);
			TestTips(verticesB, a, true);
			return result;
		}

		// Positive axes and n <= 1; the line is disjoint from the filled shape.
		// A concave arc cannot have a strict interior minimum of distance to a
		// disjoint supporting line. Line endpoints and axial tips suffice.
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointLineConcaveSuperEllipse(const Line& line, const SuperEllipse& shape) noexcept
		{
			const auto vertices = SuperEllipseAxisVertices(shape);
			ClosestPairCandidate result;
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				const auto candidate = ClosestPointOnSegment(vertices[i], line.start, line.end);
				UpdateCandidate(result, candidate.pointB, vertices[i], candidate.parameterB, (i * 0.25));
			}
			const std::array endpoints{ line.start, line.end };
			for (size_t i = 0; i < endpoints.size(); ++i)
			{
				const Vec2& point = endpoints[i];
				// Skip endpoint searches that cannot beat the best tip projection.
				if (result.distanceSq <= DistanceSqToSuperEllipseBox(point, shape))
				{
					continue;
				}
				const auto candidate = ClosestDisjointPointSuperEllipse(point, shape);
				UpdateCandidate(result, point, candidate.pointB, static_cast<double>(i));
			}
			return result;
		}

		// Positive axes. The caller has excluded intersections.
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointLineSuperEllipse(const Line& line, const SuperEllipse& shape) noexcept
		{
			if (shape.n <= 1.0)
			{
				return ClosestDisjointLineConcaveSuperEllipse(line, shape);
			}
			if (shape.n == 2.0)
			{
				return ClosestDisjointLineEllipse(line, Ellipse{ shape.center, shape.axes });
			}
			return ClosestDisjointLineConvexShape(line, shape.center,
				[&](const Vec2& normal) noexcept
				{
					if ((normal.x == 0.0) || (normal.y == 0.0))
					{
						return (shape.center + ((normal.x == 0.0)
							? Vec2{ 0, std::copysign(shape.b, normal.y) } : Vec2{ std::copysign(shape.a, normal.x), 0 }));
					}
					const bool transpose = (Abs(normal.x) < Abs(normal.y));
					const double t = Abs(transpose ? (normal.x / normal.y) : (normal.y / normal.x));
					const ConvexSuperEllipseSupport<> support{ (transpose ? Vec2{ shape.b, shape.a } : shape.axes), shape.n, 1.0 };
					Vec2 point = support.sample(t, t).first;
					if (transpose)
					{
						std::swap(point.x, point.y);
					}
					return (shape.center + Vec2{ std::copysign(point.x, normal.x), std::copysign(point.y, normal.y) });
				},
				[&](const Vec2& point) noexcept { return ClosestDisjointConvexSuperEllipsePair(shape, point); });
		}

		template <class Shape>
		[[nodiscard]]
		ClosestPairCandidate ClosestDisjointShapeDiamond(const Shape& shape, const SuperEllipse& diamond) noexcept
		{
			const auto vertices = SuperEllipseAxisVertices(diamond);
			ClosestPairCandidate result;
			for (size_t i = 0; i < vertices.size(); ++i)
			{
				const Line edge{ vertices[i], vertices[(i + 1) % vertices.size()] };
				const auto candidate = [&]() noexcept
				{
					if constexpr (std::is_same_v<Shape, Ellipse>) return ClosestDisjointLineEllipse(edge, shape);
					else return ClosestDisjointLineSuperEllipse(edge, shape);
				}();
				UpdateCandidate(result, candidate.pointB, candidate.pointA);
			}
			return result;
		}

		template <class Bezier>
		[[nodiscard]]
		ClosestPairCandidate ClosestPointBezier(const Vec2& point, const Bezier& curve) noexcept
		{
			const auto closest = detail::ClosestPointOnBezier(curve, point);
			return { point, closest.point, closest.distanceSq, 0.0, closest.parameter };
		}

		template <class Bezier>
		[[nodiscard]]
		ClosestPairCandidate ClosestLineBezier(const Line& line, const Bezier& curve) noexcept
		{
			const auto controls = detail::BezierControlPoints(curve);
			constexpr size_t Degree = (std::tuple_size_v<decltype(controls)> - 1);
			const Vec2 direction = (line.end - line.start);
			const double lengthSq = direction.lengthSq();
			ClosestPairCandidate best;
			double minHeight = std::numeric_limits<double>::infinity(), maxHeight = -minHeight;
			double minAbsHeight = minHeight;
			bool lineMinimumInside = false;
			const auto Consider = [&](const double t)
			{
				const Vec2 p = curve.pointAt(t);
				const auto projected = ClosestPointOnSegment(p, line.start, line.end);
				UpdateCandidate(best, projected.pointB, p, projected.parameterB, t);
				const double height = (p - line.start).cross(direction);
				minHeight = Min(minHeight, height);
				maxHeight = Max(maxHeight, height);
				if (Abs(height) <= minAbsHeight)
				{
					const double projection = (p - line.start).dot(direction);
					const bool inside = ((0.0 <= projection) && (projection <= lengthSq));
					lineMinimumInside = (inside || ((Abs(height) == minAbsHeight) && lineMinimumInside));
					minAbsHeight = Abs(height);
				}
			};
			Consider(0.0);
			Consider(1.0);
			std::array<double, Degree> normalDerivative;
			for (size_t i = 0; i < Degree; ++i)
			{
				normalDerivative[i] = (controls[i + 1] - controls[i]).cross(direction);
			}
			const auto stationary = detail::BernsteinRoots(normalDerivative);
			for (size_t i = 0; i < stationary.count; ++i)
			{
				Consider(stationary.values[i]);
			}
			// A minimum on the infinite line is also global on the segment when
			// its projection lies inside and the curve stays on one side of the line.
			if ((lengthSq != 0.0) && lineMinimumInside && ((0.0 <= minHeight) || (maxHeight <= 0.0)))
			{
				return best;
			}
			if ((minHeight < 0.0) && (0.0 < maxHeight))
			{
				std::array<double, Degree + 1> heights;
				for (size_t i = 0; i <= Degree; ++i)
				{
					heights[i] = (controls[i] - line.start).cross(direction);
				}
				const auto crossings = detail::BernsteinRoots(heights);
				for (size_t i = 0; i < crossings.count; ++i)
				{
					Consider(crossings.values[i]);
				}
			}
			for (const Vec2& endpoint : { line.start, line.end })
			{
				const auto roots = detail::BezierPointStationaryParameters(controls, endpoint);
				for (size_t i = 0; i < roots.count; ++i)
				{
					Consider(roots.values[i]);
				}
			}
			return best;
		}

		// Inflating the second set by a disk reduces its distance by the radius.
		// The nearest point on the curve is also a common point on overlap.
		[[nodiscard]]
		ClosestPoints2D InflateClosestPair(const Vec2& curvePoint, const Vec2& corePoint,
			const double distanceSq, const double radius) noexcept
		{
			if (distanceSq <= (radius * radius))
			{
				return { curvePoint, curvePoint, 0.0 };
			}
			const double distance = std::sqrt(distanceSq);
			return { curvePoint, (corePoint + (curvePoint - corePoint) * (radius / distance)), (distance - radius) };
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<ClosestPoints2D> TryClosestBezierRoundedShape(const ShapeA& curve, const ShapeB& shape) noexcept
		{
			if constexpr ((std::is_same_v<ShapeA, Bezier2> || std::is_same_v<ShapeA, Bezier3>)
				&& std::is_same_v<ShapeB, Circle>)
			{
				const auto closest = detail::ClosestPointOnBezier(curve, shape.center);
				return InflateClosestPair(closest.point, shape.center, closest.distanceSq, shape.r);
			}
			else if constexpr ((std::is_same_v<ShapeA, Bezier2> || std::is_same_v<ShapeA, Bezier3>)
				&& std::is_same_v<ShapeB, RoundRect>)
			{
				if ((shape.r == 0.0) || (shape.rect.w == 0.0) || (shape.rect.h == 0.0))
				{
					return none;
				}
				const double radius = detail::GetGeometry2DEffectiveRadius(shape);
				const RectF core = detail::GetGeometry2DRoundRectCore(shape, radius);
				const Vec2 lo = core.tl(), hi = core.br();
				if ((lo.x <= curve.p0.x) && (curve.p0.x <= hi.x)
					&& (lo.y <= curve.p0.y) && (curve.p0.y <= hi.y))
				{
					return ClosestPoints2D{ curve.p0, curve.p0, 0.0 };
				}
				if ((core.w == 0.0) && (core.h == 0.0))
				{
					const auto closest = detail::ClosestPointOnBezier(curve, lo);
					return InflateClosestPair(closest.point, lo, closest.distanceSq, radius);
				}
				if ((core.w == 0.0) || (core.h == 0.0))
				{
					const auto closest = ClosestLineBezier(Line{ lo, hi }, curve);
					return InflateClosestPair(closest.pointB, closest.pointA, closest.distanceSq, radius);
				}

				ClosestPairCandidate best;
				for (const Line& edge : { core.top(), core.right(), core.bottom(), core.left() })
				{
					const auto closest = ClosestLineBezier(edge, curve);
					if (closest.distanceSq < best.distanceSq)
					{
						best = closest;
						if (best.distanceSq <= (radius * radius))
						{
							break;
						}
					}
				}
				return InflateClosestPair(best.pointB, best.pointA, best.distanceSq, radius);
			}
			else
			{
				return none;
			}
		}

		template <class Bezier>
		[[nodiscard]]
		Vec2 BezierSecondDerivative(const Bezier& curve, const double t) noexcept
		{
			if constexpr (std::is_same_v<Bezier, Bezier2>)
			{
				return curve.secondDerivative();
			}
			else
			{
				return curve.secondDerivativeAt(t);
			}
		}

		template <class BezierA, class BezierB>
		void RefineBezierPair(const BezierA& a, const BezierB& b, ClosestPairCandidate& best) noexcept
		{
			double t = best.parameterA, s = best.parameterB;
			for (int32 iteration = 0; iteration < 24; ++iteration)
			{
				const Vec2 r = (a.pointAt(t) - b.pointAt(s));
				const Vec2 u = a.derivativeAt(t), v = b.derivativeAt(s);
				const double ru = r.dot(u), rv = r.dot(v);
				const double uu = u.lengthSq(), vv = v.lengthSq();
				const double ra = r.dot(BezierSecondDerivative(a, t));
				const double rb = r.dot(BezierSecondDerivative(b, s));
				const double h1 = (uu + ra), h2 = (vv - rb), cross = u.cross(v);
				// Cross products avoid cancellation in the Newton system when
				// the tangents are nearly parallel at a small separation.
				const double determinant = (cross * cross + ra * vv - rb * uu - ra * rb);
				const bool fixedA = (((t == 0.0) && (0.0 <= ru)) || ((t == 1.0) && (ru <= 0.0)));
				const bool fixedB = (((s == 0.0) && (rv <= 0.0)) || ((s == 1.0) && (0.0 <= rv)));
				double dt = 0.0, ds = 0.0;
				if ((not fixedA) && (not fixedB) && (0.0 < h1) && (0.0 < h2) && (0.0 < determinant))
				{
					dt = ((-r.cross(v) * cross + rb * ru) / determinant);
					ds = ((-r.cross(u) * cross + ra * rv) / determinant);
				}
				else
				{
					if ((not fixedA) && (0.0 < Max(uu, h1)))
					{
						dt = (-ru / Max(uu, h1));
					}
					if ((not fixedB) && (0.0 < Max(vv, h2)))
					{
						ds = (rv / Max(vv, h2));
					}
				}
				dt = (ClampUnit(t + dt) - t);
				ds = (ClampUnit(s + ds) - s);
				if ((Abs(dt) <= ParameterTolerance) && (Abs(ds) <= ParameterTolerance))
				{
					break;
				}

				bool accepted = false;
				for (int32 backtrack = 0; backtrack < 12; ++backtrack)
				{
					const double nextA = ClampUnit(t + dt), nextB = ClampUnit(s + ds);
					const Vec2 pointA = a.pointAt(nextA), pointB = b.pointAt(nextB);
					const double distanceSq = pointA.distanceFromSq(pointB);
					if (distanceSq <= best.distanceSq)
					{
						best = { pointA, pointB, distanceSq, nextA, nextB };
						t = nextA;
						s = nextB;
						accepted = true;
						break;
					}
					dt *= 0.5;
					ds *= 0.5;
				}
				if (not accepted)
				{
					break;
				}
			}
		}

		template <class BezierA, class BezierB>
		[[nodiscard]]
		ClosestPairCandidate ClosestBezierPair(const BezierA& a, const BezierB& b, ClosestPairCandidate best = {}) noexcept
		{
			constexpr int32 MaxEvaluations = 128;
			struct Node
			{
				BezierA a;
				BezierB b;
				double lowerA, upperA, lowerB, upperB;
				double boundSq, distanceSq, parameterA, parameterB;
				bool splitA;
			};
			// Each split consumes two evaluations and adds at most one queued node.
			std::array<Node, ((MaxEvaluations + 1) / 2)> queue;
			int32 count = 0, evaluations = 0;
			const auto Compare = [](const Node& lhs, const Node& rhs) noexcept
			{
				return ((lhs.boundSq != rhs.boundSq) ? (lhs.boundSq > rhs.boundSq) : (lhs.distanceSq > rhs.distanceSq));
			};
			double bestDistance = std::sqrt(best.distanceSq);
			double scaleSq = 0.0;
			for (const Vec2& p : detail::BezierControlPoints(a))
			{
				scaleSq = Max(scaleSq, p.distanceFromSq(a.p0));
			}
			for (const Vec2& p : detail::BezierControlPoints(b))
			{
				scaleSq = Max(scaleSq, p.distanceFromSq(a.p0));
			}
			const double tolerance = (64.0 * std::numeric_limits<double>::epsilon() * std::sqrt(scaleSq));
			const auto Add = [&](const BezierA& partA, const BezierB& partB,
				const double lowerA, const double upperA, const double lowerB, const double upperB)
			{
				++evaluations;
				const auto pointsA = detail::BezierControlPoints(partA);
				const auto pointsB = detail::BezierControlPoints(partB);
				const auto [minA, maxA] = detail::BezierControlBounds(partA);
				const auto [minB, maxB] = detail::BezierControlBounds(partB);
				const Vec2 boxGap{ Max({ 0.0, (minA.x - maxB.x), (minB.x - maxA.x) }),
					Max({ 0.0, (minA.y - maxB.y), (minB.y - maxA.y) }) };
				double boundSq = boxGap.lengthSq();
				double cutoff = Max(0.0, (bestDistance - tolerance));
				if ((cutoff * cutoff) <= boundSq)
				{
					return;
				}

				const auto seed = ClosestSegmentSegment(pointsA.front(), pointsA.back(), pointsB.front(), pointsB.back());
				const double t = (lowerA + (upperA - lowerA) * seed.parameterA);
				const double s = (lowerB + (upperB - lowerB) * seed.parameterB);
				const Vec2 pointA = a.pointAt(t), pointB = b.pointAt(s);
				UpdateCandidate(best, pointA, pointB, t, s);
				if (evaluations == 1)
				{
					RefineBezierPair(a, b, best);
				}
				bestDistance = std::sqrt(best.distanceSq);
				if (bestDistance <= tolerance)
				{
					return;
				}

				// Project the control hulls onto the candidate separation direction
				// for a lower bound that also works for oblique, nearby curves.
				const Vec2 normal = (pointB - pointA);
				double projectionA = -std::numeric_limits<double>::infinity();
				double projectionB = std::numeric_limits<double>::infinity();
				for (const Vec2& p : pointsA)
				{
					projectionA = Max(projectionA, (p - partA.p0).dot(normal));
				}
				for (const Vec2& p : pointsB)
				{
					projectionB = Min(projectionB, (p - partA.p0).dot(normal));
				}
				const double gap = Max(0.0, (projectionB - projectionA));
				boundSq = Max(boundSq, (gap * gap / normal.lengthSq()));
				cutoff = Max(0.0, (bestDistance - tolerance));
				if (((cutoff * cutoff) <= boundSq)
					|| (((upperA - lowerA) <= ParameterTolerance) && ((upperB - lowerB) <= ParameterTolerance)))
				{
					return;
				}
				const bool splitA = ((ParameterTolerance < (upperA - lowerA))
					&& (((upperB - lowerB) <= ParameterTolerance) || ((maxB - minB).lengthSq() <= (maxA - minA).lengthSq())));
				queue[count++] = { partA, partB, lowerA, upperA, lowerB, upperB, boundSq, normal.lengthSq(), t, s, splitA };
				std::push_heap(queue.begin(), (queue.begin() + count), Compare);
			};

			Add(a, b, 0.0, 1.0, 0.0, 1.0);
			while (count && ((evaluations + 2) <= MaxEvaluations))
			{
				const double cutoff = Max(0.0, (bestDistance - tolerance));
				if ((cutoff * cutoff) <= queue.front().boundSq)
				{
					break;
				}
				std::pop_heap(queue.begin(), (queue.begin() + count), Compare);
				const Node node = queue[--count];
				if (node.splitA)
				{
					const auto [left, right] = node.a.split(0.5);
					const double middle = ((node.lowerA + node.upperA) * 0.5);
					Add(left, node.b, node.lowerA, middle, node.lowerB, node.upperB);
					Add(right, node.b, middle, node.upperA, node.lowerB, node.upperB);
				}
				else
				{
					const auto [left, right] = node.b.split(0.5);
					const double middle = ((node.lowerB + node.upperB) * 0.5);
					Add(node.a, left, node.lowerA, node.upperA, node.lowerB, middle);
					Add(node.a, right, node.lowerA, node.upperA, middle, node.upperB);
				}
			}

			RefineBezierPair(a, b, best);
			// Finish two promising alternatives as well: the best sampled pair
			// can belong to a different local minimum when the budget is reached.
			for (int32 i = 0; (i < 2) && count; ++i)
			{
				std::pop_heap(queue.begin(), (queue.begin() + count), Compare);
				const Node& node = queue[--count];
				if (best.distanceSq <= node.boundSq)
				{
					break;
				}
				ClosestPairCandidate candidate;
				UpdateCandidate(candidate, a.pointAt(node.parameterA), b.pointAt(node.parameterB), node.parameterA, node.parameterB);
				RefineBezierPair(a, b, candidate);
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
			if (const auto* ellipse = std::get_if<Ellipse>(&piece))
			{
				auto result = ClosestDisjointEllipsePair(*ellipse, point);
				std::swap(result.pointA, result.pointB);
				return result;
			}

			if (const auto* shape = std::get_if<SuperEllipse>(&piece))
			{
				return ClosestDisjointPointSuperEllipse(point, *shape);
			}

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

			if (const auto* curve = std::get_if<Bezier2>(&piece))
			{
				return ClosestPointBezier(point, *curve);
			}
			if (const auto* curve = std::get_if<Bezier3>(&piece))
			{
				return ClosestPointBezier(point, *curve);
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
				if (const Ellipse* ellipseB = std::get_if<Ellipse>(&pieceB))
				{
					return ClosestDisjointLineEllipse(*lineA, *ellipseB);
				}
				if (const SuperEllipse* shapeB = std::get_if<SuperEllipse>(&pieceB))
				{
					return ClosestDisjointLineSuperEllipse(*lineA, *shapeB);
				}
				if (const Bezier2* curveB = std::get_if<Bezier2>(&pieceB))
				{
					return ClosestLineBezier(*lineA, *curveB);
				}
				if (const Bezier3* curveB = std::get_if<Bezier3>(&pieceB))
				{
					return ClosestLineBezier(*lineA, *curveB);
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
				if (const Ellipse* ellipseB = std::get_if<Ellipse>(&pieceB); ellipseB && (arcA->region == ArcRegion::Full))
				{
					return ClosestDisjointEllipsePair(Ellipse{ arcA->circle.center, arcA->circle.r, arcA->circle.r }, *ellipseB);
				}
			}

			if (const Ellipse* ellipseA = std::get_if<Ellipse>(&pieceA))
			{
				if (const Line* lineB = std::get_if<Line>(&pieceB))
				{
					auto result = ClosestDisjointLineEllipse(*lineB, *ellipseA);
					std::swap(result.pointA, result.pointB);
					std::swap(result.parameterA, result.parameterB);
					return result;
				}
				if (const Ellipse* ellipseB = std::get_if<Ellipse>(&pieceB))
				{
					return ClosestDisjointEllipsePair(*ellipseA, *ellipseB);
				}
				if (const CircleArc* arcB = std::get_if<CircleArc>(&pieceB); arcB && (arcB->region == ArcRegion::Full))
				{
					return ClosestDisjointEllipsePair(*ellipseA, Ellipse{ arcB->circle.center, arcB->circle.r, arcB->circle.r });
				}
			}

			const auto TrySuperEllipsePair = [](const BoundaryPiece& first, const BoundaryPiece& second) -> Optional<ClosestPairCandidate>
			{
				const auto* superEllipse = std::get_if<SuperEllipse>(&second);
				if (not superEllipse)
				{
					return none;
				}
				if (const auto* line = std::get_if<Line>(&first))
				{
					return ClosestDisjointLineSuperEllipse(*line, *superEllipse);
				}
				if (superEllipse->n <= 1.0)
				{
					const auto* other = std::get_if<SuperEllipse>(&first);
					if (other && (other->n <= 1.0))
					{
						return ClosestDisjointConcaveSuperEllipsePair(*other, *superEllipse);
					}
					if (superEllipse->n == 1.0)
					{
						if (const auto* ellipse = std::get_if<Ellipse>(&first))
						{
							return ClosestDisjointShapeDiamond(*ellipse, *superEllipse);
						}
						if (other)
						{
							return ClosestDisjointShapeDiamond(*other, *superEllipse);
						}
					}
					else
					{
						const auto* ellipse = std::get_if<Ellipse>(&first);
						if (other || ellipse)
						{
							auto result = ClosestMixedSuperEllipsePair(*superEllipse, (other ? *other : SuperEllipse{ *ellipse, 2.0 }));
							std::swap(result.pointA, result.pointB);
							return result;
						}
					}
					return none;
				}
				if (const auto* ellipse = std::get_if<Ellipse>(&first))
				{
					return ClosestDisjointConvexSuperEllipsePair(SuperEllipse{ *ellipse, 2.0 }, *superEllipse);
				}
				if (const auto* arc = std::get_if<CircleArc>(&first); arc && (arc->region == ArcRegion::Full))
				{
					return ClosestDisjointConvexSuperEllipsePair(SuperEllipse{ arc->circle.center, arc->circle.r, arc->circle.r, 2.0 }, *superEllipse);
				}
				if (const auto* other = std::get_if<SuperEllipse>(&first); other && (1.0 < other->n))
				{
					return ClosestDisjointConvexSuperEllipsePair(*other, *superEllipse);
				}
				return none;
			};
			if (const auto result = TrySuperEllipsePair(pieceA, pieceB))
			{
				return *result;
			}
			if (auto result = TrySuperEllipsePair(pieceB, pieceA))
			{
				std::swap(result->pointA, result->pointB);
				return *result;
			}

			if (const Bezier2* bezierA = std::get_if<Bezier2>(&pieceA))
			{
				if (const Line* lineB = std::get_if<Line>(&pieceB))
				{
					auto result = ClosestLineBezier(*lineB, *bezierA);
					std::swap(result.pointA, result.pointB);
					std::swap(result.parameterA, result.parameterB);
					return result;
				}
				if (const Bezier2* bezierB = std::get_if<Bezier2>(&pieceB))
				{
					return ClosestBezierPair(*bezierA, *bezierB);
				}
				if (const Bezier3* bezierB = std::get_if<Bezier3>(&pieceB))
				{
					return ClosestBezierPair(*bezierA, *bezierB);
				}
			}
			if (const Bezier3* bezierA = std::get_if<Bezier3>(&pieceA))
			{
				if (const Line* lineB = std::get_if<Line>(&pieceB))
				{
					auto result = ClosestLineBezier(*lineB, *bezierA);
					std::swap(result.pointA, result.pointB);
					std::swap(result.parameterA, result.parameterB);
					return result;
				}
				if (const Bezier2* bezierB = std::get_if<Bezier2>(&pieceB))
				{
					auto result = ClosestBezierPair(*bezierB, *bezierA);
					std::swap(result.pointA, result.pointB);
					std::swap(result.parameterA, result.parameterB);
					return result;
				}
				if (const Bezier3* bezierB = std::get_if<Bezier3>(&pieceB))
				{
					return ClosestBezierPair(*bezierA, *bezierB);
				}
			}

			const int32 segmentsA = SegmentCount(pieceA);
			const int32 segmentsB = SegmentCount(pieceB);
			// Each target sample is shared by every source segment. In particular,
			// SuperEllipse boundary powers must not be recomputed in the inner loop.
			constexpr int32 MaxSegments = Max({ EllipseSegments, SuperEllipseSegments,
				Bezier2Segments, Bezier3Segments, FullCircleSegments, QuarterCircleSegments, 1 });
			std::array<Vec2, MaxSegments + 1> pointsB;
			for (int32 j = 0; j <= segmentsB; ++j)
			{
				pointsB[j] = PointAt(pieceB, (static_cast<double>(j) / segmentsB));
			}
			ClosestPairCandidate seed;
			Vec2 a0 = PointAt(pieceA, 0.0);

			for (int32 i = 0; i < segmentsA; ++i)
			{
				const Vec2 a1 = PointAt(pieceA, (static_cast<double>(i + 1) / segmentsA));

				for (int32 j = 0; j < segmentsB; ++j)
				{
					const auto local = ClosestSegmentSegment(a0, a1, pointsB[j], pointsB[j + 1]);

					if (local.distanceSq < seed.distanceSq)
					{
						seed = local;
						seed.parameterA = ((static_cast<double>(i) + local.parameterA) / segmentsA);
						seed.parameterB = ((static_cast<double>(j) + local.parameterB) / segmentsB);
					}
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
		}

		void AppendRepresentativePoints(Array<Vec2>& points, const MultiPolygon& shape)
		{
			for (const auto& polygon : shape)
			{
				AppendRepresentativePoints(points, polygon);
			}
		}

		// 呼び出し側で空形状を除外してから、必要な経路だけで作る。
		template <class Shape>
		[[nodiscard]]
		ShapeDistanceData MakeShapeDistanceData(const Shape& shape)
		{
			ShapeDistanceData data;
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

		template <class Shape>
		[[nodiscard]]
		Array<Vec2> MakeRepresentativePoints(const Shape& shape)
		{
			Vec2 point;
			if (TryGetPointGeometry(shape, point))
			{
				return { point };
			}

			Array<Vec2> points;
			AppendRepresentativePoints(points, shape);
			return points;
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<Vec2> FindCommonPoint(const ShapeA& a, const ShapeB& b)
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

			const Array<Vec2> pointsA = MakeRepresentativePoints(a);
			if (const auto point = TestPoints(pointsA))
			{
				return *point;
			}

			const Array<Vec2> pointsB = MakeRepresentativePoints(b);
			if (const auto point = TestPoints(pointsB))
			{
				return *point;
			}

			auto TestBoundary = [](const auto& source, const auto& other) -> Optional<Vec2>
			{
				const ShapeDistanceData data = MakeShapeDistanceData(source);
				Optional<Vec2> result;
				(void)detail::AnyBoundaryPiece<true>(data.boundaryPieces, [&](const BoundaryPiece& piece)
					{
						for (int32 i = 0; i <= 32; ++i)
						{
							const Vec2 point = PointAt(piece, (static_cast<double>(i) / 32.0));
							if (Geometry2D::Intersects(point, other))
							{
								result = point;
								return true;
							}
						}
						return false;
					});
				return result;
			};

			if (const auto point = TestBoundary(a, b))
			{
				return *point;
			}
			if (const auto point = TestBoundary(b, a))
			{
				return *point;
			}

			const size_t countA = std::min(pointsA.size(), static_cast<size_t>(12));
			const size_t countB = std::min(pointsB.size(), static_cast<size_t>(12));

			for (size_t i = 0; i < countA; ++i)
			{
				for (size_t j = 0; j < countB; ++j)
				{
					for (int32 k = 1; k < 32; ++k)
					{
						const Vec2 point = pointsA[i].lerp(
							pointsB[j], (static_cast<double>(k) / 32.0));

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
				(void)detail::AnyBoundaryPiece<true>(dataB.boundaryPieces, [&](const BoundaryPiece& pieceB)
					{
						const auto candidate = ClosestPointPiece(*dataA.pointGeometry, pieceB);
						if (candidate.distanceSq < best.distanceSq)
						{
							best = candidate;
						}
						return false;
					});
				return best;
			}

			if (dataB.pointGeometry)
			{
				(void)detail::AnyBoundaryPiece<true>(dataA.boundaryPieces, [&](const BoundaryPiece& pieceA)
					{
						auto candidate = ClosestPointPiece(*dataB.pointGeometry, pieceA);
						std::swap(candidate.pointA, candidate.pointB);
						std::swap(candidate.parameterA, candidate.parameterB);
						if (candidate.distanceSq < best.distanceSq)
						{
							best = candidate;
						}
						return false;
					});
				return best;
			}

			(void)detail::AnyBoundaryPiece<true>(dataA.boundaryPieces, [&](const BoundaryPiece& pieceA)
				{
					(void)detail::AnyBoundaryPiece<true>(dataB.boundaryPieces, [&](const BoundaryPiece& pieceB)
						{
							const auto candidate = ClosestPiecePair(pieceA, pieceB);
							if (candidate.distanceSq < best.distanceSq)
							{
								best = candidate;
							}
							return false;
						});
					return false;
				});

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

		template <class BezierA, class BezierB>
		[[nodiscard]]
		ClosestPoints2D ClosestBezierPairGeometry(const BezierA& a, const BezierB& b)
		{
			const auto intersection = detail::ClassifyBezierPair(a, b);
			const Vec2 pointA = a.pointAt(intersection.parameterA), pointB = b.pointAt(intersection.parameterB);
			if (intersection.kind == detail::BezierIntersectionKind::Contact)
			{
				const Vec2 common = (pointA + (pointB - pointA) * 0.5);
				return { common, common, 0.0 };
			}
			// An unresolved predicate supplies a candidate, never a fabricated
			// zero distance or a common point from a polyline approximation.
			ClosestPairCandidate seed;
			UpdateCandidate(seed, pointA, pointB, intersection.parameterA, intersection.parameterB);
			const auto closest = ClosestBezierPair(a, b, seed);
			return { closest.pointA, closest.pointB, std::sqrt(closest.distanceSq) };
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<ClosestPoints2D> TryClosestBezierSuperEllipse(const ShapeA& curve, const ShapeB& shape)
		{
			if constexpr (detail::IsBezier<ShapeA> && std::is_same_v<ShapeB, SuperEllipse>)
			{
				if ((0.0 < shape.a) && (0.0 < shape.b) && (shape.n != 1.0) && (shape.n != 2.0))
				{
					const auto intersection = detail::ClassifyBezierSuperEllipse(curve, shape);
					if (intersection.kind == detail::BezierIntersectionKind::Contact)
					{
						const Vec2 point = curve.pointAt(intersection.parameter);
						return ClosestPoints2D{ point, point, 0.0 };
					}
					// A conservative true cannot establish a common point. Keep the
					// evaluated boundary pair from the distance approximation instead.
					auto closest = ClosestPiecePair(BoundaryPiece{ curve }, BoundaryPiece{ shape });
					if (intersection.kind == detail::BezierIntersectionKind::Unresolved)
					{
						const auto candidate = ClosestPointPiece(curve.pointAt(intersection.parameter), BoundaryPiece{ shape });
						if (candidate.distanceSq < closest.distanceSq)
						{
							closest = candidate;
						}
					}
					return ClosestPoints2D{ closest.pointA, closest.pointB, std::sqrt(closest.distanceSq) };
				}
			}
			return none;
		}

		// A conservative predicate result without a point continues through the
		// boundary-distance path. Ordinary predicates need no witness reconstruction.
		struct AreaCommonPoint
		{
			bool tested = false;
			Optional<Vec2> point;
		};

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		AreaCommonPoint FindSuperEllipseCommonPoint(const ShapeA& a, const ShapeB& b)
		{
			constexpr bool Supported = (std::is_same_v<ShapeB, SuperEllipse>
				&& (std::is_same_v<ShapeA, Circle> || std::is_same_v<ShapeA, Ellipse> || std::is_same_v<ShapeA, SuperEllipse>))
				|| (std::is_same_v<ShapeA, SuperEllipse> && std::is_same_v<ShapeB, RoundRect>);
			if constexpr (Supported)
			{
				if ((detail::ClassifyGeometry2DSizedShape(a) != detail::Geometry2DSizedShapeKind::Area)
					|| (detail::ClassifyGeometry2DSizedShape(b) != detail::Geometry2DSizedShapeKind::Area))
				{
					return {};
				}
				const auto AsArea = [](const auto& shape, const Vec2& otherCenter) -> SuperEllipse
				{
					using Shape = std::decay_t<decltype(shape)>;
					if constexpr (std::is_same_v<Shape, SuperEllipse>)
					{
						return shape;
					}
					else if constexpr (std::is_same_v<Shape, Circle>)
					{
						return { shape.center, shape.r, shape.r, 2.0 };
					}
					else if constexpr (std::is_same_v<Shape, Ellipse>)
					{
						return { shape, 2.0 };
					}
					else
					{
						const double radius = detail::GetGeometry2DEffectiveRadius(shape);
						const RectF core = detail::GetGeometry2DRoundRectCore(shape, radius);
						return { Vec2{ Clamp(otherCenter.x, core.x, (core.x + core.w)),
							Clamp(otherCenter.y, core.y, (core.y + core.h)) }, radius, radius, 2.0 };
					}
				};
				const SuperEllipse first = AsArea(a, Vec2{ 0, 0 }), second = AsArea(b, first.center);
				if (((first.n == 2.0) && (second.n == 2.0)) || (second.a == 0.0))
				{
					return {};
				}
				Optional<Vec2> common;
				(void)detail::TestSuperEllipseAreas<true, true>(first, second, &common);
				return { true, common };
			}
			return {};
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<ClosestPoints2D> TryClosestEllipticSimpleShape(const ShapeA& a, const ShapeB& b)
		{
			if constexpr (std::is_same_v<ShapeB, SuperEllipse>
				&& (std::is_same_v<ShapeA, Ellipse> || std::is_same_v<ShapeA, SuperEllipse>))
			{
				const bool firstIsCircle = [&]() noexcept
				{
					if constexpr (std::is_same_v<ShapeA, SuperEllipse>) return ((a.n == 2.0) && (a.a == a.b));
					else return (a.a == a.b);
				}();
				if (firstIsCircle)
				{
					return TryClosestEllipticSimpleShape(Circle{ a.center, a.a }, b);
				}
				if constexpr (std::is_same_v<ShapeA, SuperEllipse>)
				{
					if ((b.n == 2.0) && (b.a == b.b))
					{
						auto result = TryClosestEllipticSimpleShape(Circle{ b.center, b.a }, a);
						if (result) std::swap(result->pointA, result->pointB);
						return result;
					}
				}
			}
			constexpr bool PointFirst = (std::is_same_v<ShapeA, Point> || std::is_same_v<ShapeA, Vec2>);
			constexpr bool ShapeFirst = ((std::is_same_v<ShapeA, Ellipse> || std::is_same_v<ShapeA, SuperEllipse>)
				&& std::is_same_v<ShapeB, RoundRect>);
			if constexpr (ShapeFirst || (std::is_same_v<ShapeB, SuperEllipse> && (PointFirst || std::is_same_v<ShapeA, Circle>)))
			{
				if constexpr (ShapeFirst)
				{
					if (detail::ClassifyGeometry2DSizedShape(b) != detail::Geometry2DSizedShapeKind::Area)
					{
						return none;
					}
				}
				const auto& shape = [&]() -> const auto&
				{
					if constexpr (ShapeFirst) return a;
					else return b;
				}();
				if ((shape.a <= 0.0) || (shape.b <= 0.0))
				{
					return none;
				}
				const auto [point, radius] = [&]()
				{
					if constexpr (ShapeFirst)
					{
						// Both shapes are symmetric in each axis. The nearest core point
						// is the clamp of the ellipse / SuperEllipse center, including for n < 1.
						const double r = detail::GetGeometry2DEffectiveRadius(b);
						const RectF core = detail::GetGeometry2DRoundRectCore(b, r);
						return std::pair{ Vec2{ Clamp(shape.x, core.x, (core.x + core.w)), Clamp(shape.y, core.y, (core.y + core.h)) }, r };
					}
					else if constexpr (PointFirst)
					{
						return std::pair{ Vec2{ a }, 0.0 };
					}
					else
					{
						return std::pair{ a.center, a.r };
					}
				}();
				const auto closest = [&]() noexcept
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Ellipse>)
					{
						auto result = ClosestDisjointEllipsePair(shape, point);
						std::swap(result.pointA, result.pointB);
						return result;
					}
					else return ClosestDisjointPointSuperEllipse(point, shape);
				}();
				const double distance = std::sqrt(closest.distanceSq);
				const bool contact = [&]() noexcept
				{
					if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Ellipse>)
					{
						return detail::EllipseDistanceWithinRadius<true>(distance, radius, Max({ shape.a, shape.b, radius }));
					}
					else return (distance <= radius);
				}();
				if (contact)
				{
					return ClosestPoints2D{ closest.pointB, closest.pointB, 0.0 };
				}
				const Vec2 onRoundedShape = (point + (closest.pointB - point) * (radius / distance));
				if constexpr (ShapeFirst)
				{
					return ClosestPoints2D{ closest.pointB, onRoundedShape, (distance - radius) };
				}
				else
				{
					return ClosestPoints2D{ onRoundedShape, closest.pointB, (distance - radius) };
				}
			}
			return none;
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		Optional<ClosestPoints2D> ComputeClosestPointsCanonical(const ShapeA& a, const ShapeB& b)
		{
			if constexpr (detail::IsBezier<ShapeA> && detail::IsBezier<ShapeB>)
			{
				return ClosestBezierPairGeometry(a, b);
			}

			if (IsEmptyGeometry(a) || IsEmptyGeometry(b))
			{
				return none;
			}

			if (const auto rounded = TryClosestBezierRoundedShape(a, b))
			{
				return rounded;
			}

			if (const auto closest = TryClosestBezierSuperEllipse(a, b))
			{
				return closest;
			}

			const auto area = FindSuperEllipseCommonPoint(a, b);
			if (area.point)
			{
				return ClosestPoints2D{ *area.point, *area.point, 0.0 };
			}

			if ((not area.tested) && Geometry2D::Intersects(a, b))
			{
				if (const auto commonPoint = FindCommonPoint(a, b))
				{
					return ClosestPoints2D{ *commonPoint, *commonPoint, 0.0 };
				}
			}

			if (const auto closest = TryClosestEllipticSimpleShape(a, b))
			{
				return closest;
			}

			const ShapeDistanceData dataA = MakeShapeDistanceData(a);
			const ShapeDistanceData dataB = MakeShapeDistanceData(b);
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
			return detail::WithSimpleBezierPair(a, b, [](const auto& a, const auto& b)
			{
				if constexpr (ShapeRank<std::decay_t<decltype(b)>> < ShapeRank<std::decay_t<decltype(a)>>)
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
			});
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		double ComputeDistanceCanonical(const ShapeA& a, const ShapeB& b)
		{
			if constexpr (detail::IsBezier<ShapeA> && detail::IsBezier<ShapeB>)
			{
				return ClosestBezierPairGeometry(a, b).distance;
			}

			if (IsEmptyGeometry(a) || IsEmptyGeometry(b))
			{
				return std::numeric_limits<double>::infinity();
			}

			if (const auto rounded = TryClosestBezierRoundedShape(a, b))
			{
				return rounded->distance;
			}

			if (const auto closest = TryClosestBezierSuperEllipse(a, b))
			{
				return closest->distance;
			}

			const auto area = FindSuperEllipseCommonPoint(a, b);
			if (area.point)
			{
				return 0.0;
			}

			if ((not area.tested) && Geometry2D::Intersects(a, b))
			{
				return 0.0;
			}

			if (const auto closest = TryClosestEllipticSimpleShape(a, b))
			{
				return closest->distance;
			}

			const ShapeDistanceData dataA = MakeShapeDistanceData(a);
			const ShapeDistanceData dataB = MakeShapeDistanceData(b);
			const ClosestPairCandidate candidate = ComputeDisjointClosestPair(dataA, dataB);
			assert(std::isfinite(candidate.distanceSq));
			return std::sqrt(Max(0.0, candidate.distanceSq));
		}

		template <class ShapeA, class ShapeB>
		[[nodiscard]]
		double ComputeDistance(const ShapeA& a, const ShapeB& b)
		{
			return detail::WithSimpleBezierPair(a, b, [](const auto& a, const auto& b)
			{
				if constexpr (ShapeRank<std::decay_t<decltype(b)>> < ShapeRank<std::decay_t<decltype(a)>>)
				{
					return ComputeDistanceCanonical(b, a);
				}
				else
				{
					return ComputeDistanceCanonical(a, b);
				}
			});
		}
	}

	namespace detail
	{
		Vec2 ClosestPointOnSuperEllipseBoundaryFromOutside(const Vec2& point, const SuperEllipse& shape) noexcept
		{
			return ClosestDisjointPointSuperEllipse(point, shape).pointB;
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

