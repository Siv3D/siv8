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

# include "PolygonBuffer.hpp"
# include "PolygonDetail.hpp"
# include "GeometryCommon.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ComputeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeMiterBufferPolygon(const Line& line, const double distance)
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
		
		return detail::ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeRoundBufferPolygon(const Line& line, const double distance, const QualityFactor& qualityFactor)
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
			boost::geometry::strategy::buffer::join_round{},
			boost::geometry::strategy::buffer::end_round{ detail::CalculateCircleQuality(Abs(distance) * qualityFactor.value()) },
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return detail::ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeRoundPolygon(const std::span<const Vec2> outer, const double distance, const QualityFactor& qualityFactor)
	{
		const CWOpenRing polygon{ outer.begin(), outer.end() };

		boost::geometry::model::multi_polygon<CwOpenPolygon> insetPolygon;

		boost::geometry::buffer(polygon, insetPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ -distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_miter{},
			boost::geometry::strategy::buffer::end_flat{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (insetPolygon.size() != 1)
		{
			return{};
		}

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(insetPolygon.front(), multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ detail::CalculateCircleQuality(Abs(distance) * qualityFactor.value()) },
			boost::geometry::strategy::buffer::end_round{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		// 異常なポリゴンを除外
		if (multiPolygon.front().outer().size() <= 4)
		{
			return{};
		}

		return detail::ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	ComputeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeMiterBufferPolygon(const std::span<const Vec2> outer, const double distance)
	{
		const CWOpenRing polygon{ outer.begin(), outer.end() };

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(polygon, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_miter{},
			boost::geometry::strategy::buffer::end_flat{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return detail::ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	ComputeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon ComputeRoundBufferPolygon(const std::span<const Vec2> outer, const double distance, const QualityFactor& qualityFactor)
	{
		const CWOpenRing polygon{ outer.begin(), outer.end() };

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(polygon, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ detail::CalculateCircleQuality(Abs(distance) * qualityFactor.value()) },
			boost::geometry::strategy::buffer::end_round{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return detail::ToPolygon(multiPolygon.front());
	}
}
