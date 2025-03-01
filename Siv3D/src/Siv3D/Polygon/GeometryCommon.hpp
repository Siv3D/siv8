//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Array.hpp>
# include <boost/geometry.hpp>
# include <boost/geometry/geometries/register/point.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Point, s3d::int32, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Float2, float, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Vec2, double, boost::geometry::cs::cartesian, x, y)

namespace s3d
{
	using CwOpenPolygon	= boost::geometry::model::polygon<Vec2, false, false, Array, Array>;
	using CWOpenRing	= boost::geometry::model::ring<Vec2, false, false, Array>;
	using GLineString	= boost::geometry::model::linestring<Vec2, Array>;
	using InnersType	= Array<boost::geometry::model::ring<Vec2, false, false, Array>>;
	
	namespace detail
	{
		[[nodiscard]]
		size_t CalculateCircleQuality(double r) noexcept;
	}
}
