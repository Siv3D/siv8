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

# include "PolygonBuffer.hpp"
# include "GeometryCommon.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Polygon ToPolygon(const CwOpenPolygon& polygon)
		{
			std::span<const Vec2> outer = polygon.outer();

			if ((2 < outer.size()) && (outer.front() == outer.back()))
			{
				outer = outer.subspan(0, (outer.size() - 1));
			}

			const auto& inners = polygon.inners();

			Array<Array<Vec2>> holes(inners.size());

			for (size_t i = 0; i < holes.size(); ++i)
			{
				std::span<const Vec2> inner = inners[i];

				if ((2 < inner.size()) && (inner.front() == inner.back()))
				{
					inner = inner.subspan(0, (inner.size() - 1));
				}

				holes[i].assign(inner.rbegin(), inner.rend());
			}

			return Polygon{ outer, std::move(holes), SkipValidation::Yes };
		}
	}

	Polygon CalculateBuffer(const Line& line, const double distance)
	{
		if (distance <= 0.0)
		{
			return{};
		}

		const boost::geometry::model::linestring<Vec2> segment{ line.start, line.end };

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(segment, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_miter{},
			boost::geometry::strategy::buffer::end_flat{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return ToPolygon(multiPolygon.front());
	}

	Polygon CalculateRoundBuffer(const Line& line, double distance, const double qualityFactor)
	{
		if (distance <= 0.0)
		{
			return{};
		}

		const boost::geometry::model::linestring<Vec2> segment{ line.start, line.end };

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(segment, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ },
			boost::geometry::strategy::buffer::end_round{ detail::CalculateCircleQuality(distance * qualityFactor) },
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return ToPolygon(multiPolygon.front());
	}
}
