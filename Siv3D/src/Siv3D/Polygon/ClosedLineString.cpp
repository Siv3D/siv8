//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Polygon.hpp>
# include <Siv3D/Number.hpp>
# include "ClosedLineString.hpp"
# include "GeometryCommon.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr size_t CalculateCircleQuality(const double r) noexcept
		{
			if (r <= 2.0)
			{
				return 6;
			}
			else if (r <= 4.0)
			{
				return 10;
			}
			else if (r < 8.0)
			{
				return 12;
			}
			else
			{
				return Min((12 + static_cast<int32>((r - 8.0) / 2.0)), 256);
			}
		}

		class JoinDefaultSymmetric
		{
		public:

			template <class Point, class DistanceType, class RangeOut>
			bool apply(const Point& ip, const Point& vertex,
				const Point& perp1, const Point& perp2,
				const DistanceType& buffer_distance,
				RangeOut& range_out) const
			{
				typedef typename boost::geometry::coordinate_type<Point>::type coordinate_type;
				typedef typename boost::range_value<RangeOut>::type output_point_type;

				typedef typename boost::geometry::select_most_precise
					<typename boost::geometry::select_most_precise
					<coordinate_type, typename  boost::geometry::coordinate_type<output_point_type>::type
					>::type, double>::type promoted_type;

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
					(60.0_deg <= angle))
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

					if (p2 && p3)
					{
						range_out.push_back(*p2);
						range_out.push_back(*p3);
					}
				}

				range_out.push_back(p1);
			}
		};
	}

	void DrawClosedLineString(const Array<Vec2>& points, const JoinStyle joinStyle, const double thickness, const ColorF& color)
	{
		if (joinStyle == JoinStyle::Default)
		{
			// [Siv3D ToDo]

			return;
		}

		const double halfThickness = (thickness * 0.5);
		const boost::geometry::strategy::buffer::distance_symmetric<double> distanceStrategy{ halfThickness };
		const GLineString ls{ points.begin(), points.end() };
		boost::geometry::model::multi_polygon<CwOpenPolygon> polygon;

		if (joinStyle == JoinStyle::Bevel)
		{
			boost::geometry::buffer(ls, polygon,
				distanceStrategy,
				boost::geometry::strategy::buffer::side_straight{},
				JoinDefaultSymmetric{},
				boost::geometry::strategy::buffer::end_flat{},
				boost::geometry::strategy::buffer::point_square{});
		}
		else if (joinStyle == JoinStyle::Round)
		{
			boost::geometry::buffer(ls, polygon,
				distanceStrategy,
				boost::geometry::strategy::buffer::side_straight{},
				boost::geometry::strategy::buffer::join_round{ CalculateCircleQuality(thickness) },
				boost::geometry::strategy::buffer::end_flat{},
				boost::geometry::strategy::buffer::point_square{});
		}
		else // JoinStyle::Miter
		{
			boost::geometry::buffer(ls, polygon,
				distanceStrategy,
				boost::geometry::strategy::buffer::side_straight{},
				boost::geometry::strategy::buffer::join_miter{ Largest<double> },
				boost::geometry::strategy::buffer::end_flat{},
				boost::geometry::strategy::buffer::point_square{});
		}

		if (polygon.empty())
		{
			return;
		}

		Array<Array<Vec2>> holes;

		if (const auto& inners = polygon[0].inners())
		{
			for (const auto& inner : inners)
			{
				holes.emplace_back(inner.begin(), inner.end());
			}
		}

		Polygon{ polygon[0].outer(), holes, SkipValidation::Yes }.draw(color);
	}
}
