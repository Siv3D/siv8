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

# include <limits>
# include <Siv3D/Polygon.hpp>
# include <Siv3D/Number.hpp>
# include <Siv3D/LineCap.hpp>
# include "ClosedLineString.hpp"
# include "GeometryCommon.hpp"
# include "../Geometry2D/PolygonGeometry.hpp"
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Renderer2D/Vertex2DBuilder.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

// A local view opts only this GPU fallback into exact orientation predicates.
// Epsilon-collinearity is not transitive and can invalidate Boost's angular sort
// when coincident pieces of a closed stroke meet at a cluster.
namespace s3d::detail
{
	struct ClosedStrokeView : std::span<const Vec2>
	{
		using std::span<const Vec2>::span;
		using const_iterator = iterator;
	};
}

BOOST_GEOMETRY_REGISTER_LINESTRING(s3d::detail::ClosedStrokeView)

namespace boost::geometry::strategies::buffer::services
{
	template <>
	struct default_strategy<s3d::detail::ClosedStrokeView, cartesian_tag>
	{
		struct type : strategies::buffer::cartesian<>
		{
			static auto side()
			{
				return strategy::side::side_robust<void, strategy::side::fp_equals_policy>{};
			}
		};
	};
}

namespace s3d
{
	namespace
	{
		class JoinBevel
		{
		public:

			template <class Point, class DistanceType, class RangeOut>
			bool apply(const Point& ip, const Point& vertex,
				const Point& perp1, const Point& perp2,
				const DistanceType& buffer_distance,
				RangeOut& range_out) const
			{
				boost::geometry::equal_to<Point> equals;
				if (equals(perp1, perp2))
				{
					boost::ignore_unused(ip);
					return false;
				}

				const Vec2 center{ boost::geometry::get<0>(vertex), boost::geometry::get<1>(vertex) };
				const Vec2 p0{ boost::geometry::get<0>(perp1), boost::geometry::get<1>(perp1) };
				const Vec2 p1{ boost::geometry::get<0>(perp2), boost::geometry::get<1>(perp2) };
				const Vec2 top{ boost::geometry::get<0>(ip), boost::geometry::get<1>(ip) };

				generatePoints(center, p0, p1, top, boost::geometry::math::abs(buffer_distance), range_out);

				return true;
			}

			template <class NumericType>
			static NumericType max_distance(const NumericType& distance)
			{
				return distance;
			}

		private:

			template <class RangeOut>
			void generatePoints(const Vec2& center, const Vec2& p0, const Vec2& p1, const Vec2& top, double distance, RangeOut& range_out) const
			{
				range_out.push_back(p0);

				if (const double angle = (p0 - top).getAngle(p1 - top);
					((60.0_deg - 1e-12) <= angle))
				{
					range_out.push_back(top);
				}
				else
				{
					const Line l0{ p0, p1 };
					const Vec2 c0 = l0.closestPointTo(center);

					const Vec2 d1 = (c0 - center);
					const Line l1 = l0.movedBy(d1.withLength(distance - d1.length()));

					const Line l2{ p0, top };
					const Line l3{ p1, top };

					const auto p2 = l2.intersectsAt(l1);
					const auto p3 = l3.intersectsAt(l1);

					if (p2 && *p2 && p3 && *p3)
					{
						range_out.push_back(p2->front());
						range_out.push_back(p3->front());
					}
				}

				range_out.push_back(p1);
			}
		};

		// Match direct GPU joins even when buffer topology resolution is required.
		class JoinRound
		{
		public:

			explicit JoinRound(const float scale) : m_scale{ scale } {}

			template <class Point, class DistanceType, class RangeOut>
			bool apply(const Point&, const Point& vertex, const Point& perp1, const Point& perp2,
				const DistanceType& distance, RangeOut& output) const
			{
				if (boost::geometry::equal_to<Point>{}(perp1, perp2)) return false;
				const Vec2 center{ boost::geometry::get<0>(vertex), boost::geometry::get<1>(vertex) };
				const Vec2 from{ boost::geometry::get<0>(perp1) - center.x, boost::geometry::get<1>(perp1) - center.y };
				const Vec2 to{ boost::geometry::get<0>(perp2) - center.x, boost::geometry::get<1>(perp2) - center.y };
				double sweep = std::atan2(-from.cross(to), from.dot(to));
				if (sweep < 0.0) sweep += Math::TwoPi;
				const double radius = Abs(distance);
				const auto count = Vertex2DBuilder::CalculateCircleArcQuality(static_cast<float>(radius) * m_scale, static_cast<float>(sweep));
				const double start = std::atan2(from.y, from.x), step = (sweep / (count - 1));
				output.push_back(perp1);
				for (uint16 i = 1; i + 1 < count; ++i)
				{
					const double angle = (start - step * i);
					output.push_back(center + Vec2{ std::cos(angle), std::sin(angle) } * radius);
				}
				output.push_back(perp2);
				return true;
			}

			template <class NumericType>
			static NumericType max_distance(const NumericType& distance) { return distance; }

		private:

			float m_scale;
		};

		template <class Ring>
		void RemoveCollinearStrokePoints(Ring& ring)
		{
			// Buffer intersections can leave roundoff-sized kinks on straight
			// edges. Earcut can turn them into slivers that overlap after GPU
			// rasterization. Remove only double-roundoff collinearity, in place.
			const size_t count = ring.size();
			if (count < 3) return;
			const Vec2 first = ring.front();
			Vec2 previous = ring.back();
			size_t dst = 0;
			for (size_t i = 0; i < count; ++i)
			{
				const Vec2 current = ring[i], next = ((i + 1 < count) ? ring[i + 1] : first);
				const Vec2 a = current - previous, b = next - current;
				const double tolerance = (32 * std::numeric_limits<double>::epsilon() * Max(a.lengthSq(), b.lengthSq()));
				if ((Abs(a.cross(b)) > tolerance) || (a.dot(b) < 0.0)) ring[dst++] = current;
				previous = current;
			}
			ring.resize(dst);
		}

		[[nodiscard]]
		static boost::geometry::model::multi_polygon<CwOpenPolygon> MakeBuffer(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const float scale = 1.0f)
		{
			const detail::ClosedStrokeView stroke{ points.data(), points.size() };
			const double halfThickness = (thickness * 0.5);
			const boost::geometry::strategy::buffer::distance_symmetric<double> distanceStrategy{ halfThickness };
			boost::geometry::model::multi_polygon<CwOpenPolygon> polygon;

			if (joinStyle == JoinStyle::Bevel)
			{
				boost::geometry::buffer(stroke, polygon,
					distanceStrategy,
					boost::geometry::strategy::buffer::side_straight{},
					JoinBevel{},
					boost::geometry::strategy::buffer::end_flat{},
					boost::geometry::strategy::buffer::point_square{});
			}
			else if (joinStyle == JoinStyle::Round)
			{
				boost::geometry::buffer(stroke, polygon,
					distanceStrategy,
					boost::geometry::strategy::buffer::side_straight{},
					JoinRound{ scale },
					boost::geometry::strategy::buffer::end_flat{},
					boost::geometry::strategy::buffer::point_square{});
			}
			else // JoinStyle::Miter
			{
				boost::geometry::buffer(stroke, polygon,
					distanceStrategy,
					boost::geometry::strategy::buffer::side_straight{},
					boost::geometry::strategy::buffer::join_miter{ Largest<double> },
					boost::geometry::strategy::buffer::end_flat{},
					boost::geometry::strategy::buffer::point_square{});
			}

			for (auto& part : polygon)
			{
				RemoveCollinearStrokePoints(part.outer());
				for (auto& hole : part.inners()) RemoveCollinearStrokePoints(hole);
			}
			return polygon;
		}
	}

	namespace
	{
		// Each side is clipped against the original filled region before union.
		// This also handles a disappearing hole and the two lobes of a crossed quad.
		[[nodiscard]]
		CwOpenMultiPolygon MakePolygonFrame(const std::span<const Vec2> input, const JoinStyle joinStyle,
			const double innerThickness, const double outerThickness, const float scale = 1.0f)
		{
			if ((innerThickness < 0.0) || (outerThickness < 0.0)
				|| ((innerThickness == 0.0) && (outerThickness == 0.0))) return {};
			Vec2 points[6];
			const size_t count = detail::NormalizeSmallPolygonRing(input, points);
			if (count == 0) return {};
			const auto interior = detail::CorrectPolygonRings(std::span<const Vec2>{points, count});
			if (interior.empty()) return {};
			points[count] = points[0]; points[count + 1] = points[1];
			const std::span<const Vec2> stroke{points, count + 2};
			if (innerThickness == outerThickness) return MakeBuffer(stroke, joinStyle, innerThickness * 2, scale);
			CwOpenMultiPolygon inside, outside, result;
			if (0.0 < innerThickness)
			{
				const auto band = MakeBuffer(stroke, joinStyle, innerThickness * 2, scale);
				boost::geometry::intersection(band, interior, inside);
			}
			if (0.0 < outerThickness)
			{
				const auto band = MakeBuffer(stroke, joinStyle, outerThickness * 2, scale);
				boost::geometry::difference(band, interior, outside);
			}
			if (inside.empty()) result = std::move(outside);
			else if (outside.empty()) result = std::move(inside);
			else boost::geometry::union_(inside, outside, result);
			for (auto& part : result)
			{
				RemoveCollinearStrokePoints(part.outer());
				for (auto& hole : part.inners()) RemoveCollinearStrokePoints(hole);
			}
			return result;
		}

		template <class Fill>
		void DrawPolygonFallback(const std::span<const Vec2> points, const JoinStyle joinStyle,
			const double innerThickness, const double outerThickness, const Fill& fill)
		{
			const float scale = ((joinStyle == JoinStyle::Round) ? SIV3D_ENGINE(Renderer2D)->getRasterScaling() : 1.0f);
			for (const auto& part : MakePolygonFrame(points, joinStyle, innerThickness, outerThickness, scale))
			{
				DrawTriangles(part.outer(), part.inners(), fill);
			}
		}

		template <class Fill, class RendererFill>
		void DrawPolygon(const std::span<const Vec2> points, const JoinStyle joinStyle,
			const double innerThickness, const double outerThickness, const Fill& fill, const RendererFill& rendererFill)
		{
			if (SIV3D_ENGINE(Renderer2D)->tryAddSmallConvexFrame(points, joinStyle, innerThickness, outerThickness, rendererFill)) return;
			DrawPolygonFallback(points, joinStyle, innerThickness, outerThickness, fill);
		}
	}

	void DrawSmallPolygonFrameFallback(const std::span<const Vec2> points, const JoinStyle joinStyle,
		const double innerThickness, const double outerThickness, const ColorF& color)
	{
		DrawPolygonFallback(points, joinStyle, innerThickness, outerThickness, color);
	}

	void DrawSmallPolygonFrameFallback(const std::span<const Vec2> points, const JoinStyle joinStyle,
		const double innerThickness, const double outerThickness, const PatternParameters& pattern)
	{
		DrawPolygonFallback(points, joinStyle, innerThickness, outerThickness, pattern);
	}

	void DrawSmallPolygonFrame(const std::span<const Vec2> points, const JoinStyle joinStyle,
		const double innerThickness, const double outerThickness, const ColorF& color)
	{
		DrawPolygon(points, joinStyle, innerThickness, outerThickness, color, color.toFloat4());
	}

	void DrawSmallPolygonFrame(const std::span<const Vec2> points, const JoinStyle joinStyle,
		const double innerThickness, const double outerThickness, const PatternParameters& pattern)
	{
		DrawPolygon(points, joinStyle, innerThickness, outerThickness, pattern, pattern);
	}

	namespace
	{
		template <class Fill>
		void DrawFallback(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const Fill& fill)
		{
			const float scale = ((joinStyle == JoinStyle::Round) ? SIV3D_ENGINE(Renderer2D)->getRasterScaling() : 1.0f);
			const auto polygon = MakeBuffer(points, joinStyle, thickness, scale);
			if (polygon.empty()) return;
			DrawTriangles(polygon[0].outer(), polygon[0].inners(), fill);
		}

		template <class Fill, class RendererFill>
		void DrawClosed(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const Fill& fill, const RendererFill& rendererFill)
		{
			if (SIV3D_ENGINE(Renderer2D)->tryAddSmallConvexFrame(points.first(points.size() - 2), joinStyle, thickness * 0.5, thickness * 0.5, rendererFill)) return;
			DrawFallback(points, joinStyle, thickness, fill);
		}
	}

	void DrawClosedLineStringFallback(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const ColorF& color)
	{
		DrawFallback(points, joinStyle, thickness, color);
	}

	void DrawClosedLineStringFallback(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const PatternParameters& pattern)
	{
		DrawFallback(points, joinStyle, thickness, pattern);
	}

	void DrawClosedLineString(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const ColorF& color)
	{
		DrawClosed(points, joinStyle, thickness, color, color.toFloat4());
	}

	void DrawClosedLineString(const std::span<const Vec2> points, const JoinStyle joinStyle, const double thickness, const PatternParameters& pattern)
	{
		DrawClosed(points, joinStyle, thickness, pattern, pattern);
	}
}
