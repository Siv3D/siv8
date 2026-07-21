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

# pragma once
# include <Siv3D/PointVector.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Array.hpp>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(5311)
# include <boost/geometry.hpp>
# include <boost/geometry/geometries/register/point.hpp>
# include <boost/geometry/geometries/register/segment.hpp>
# include <boost/geometry/geometries/register/linestring.hpp>
# include <boost/geometry/geometries/register/multi_point.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	using CwOpenPolygon			= boost::geometry::model::polygon<Vec2, false, false, Array, Array>;
	using CwOpenMultiPolygon	= boost::geometry::model::multi_polygon<CwOpenPolygon>;
	using CWOpenRing			= boost::geometry::model::ring<Vec2, false, false, Array>;
	using InnersType			= Array<boost::geometry::model::ring<Vec2, false, false, Array>>;

	template <class Iterator>
	class MultiPointView : public boost::iterator_range<Iterator>
	{
		using base_type = boost::iterator_range<Iterator>;

	public:

		using base_type::base_type;
	};

	template<class Iterator>
	MultiPointView(Iterator, Iterator) -> MultiPointView<Iterator>;

	namespace detail
	{
		[[nodiscard]]
		size_t CalculateCircleQuality(double r) noexcept;
	}
}

BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Point, s3d::int32, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Float2, float, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Vec2, double, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_SEGMENT(s3d::Line, s3d::Vec2, start, end)
BOOST_GEOMETRY_REGISTER_LINESTRING(s3d::LineString)
BOOST_GEOMETRY_REGISTER_MULTI_POINT(s3d::Array<s3d::Vec2>)
BOOST_GEOMETRY_REGISTER_MULTI_POINT_TEMPLATED(s3d::MultiPointView)
