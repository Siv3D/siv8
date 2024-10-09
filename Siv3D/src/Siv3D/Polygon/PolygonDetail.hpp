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

# pragma once
# include <Siv3D/Polygon.hpp>
# include <boost/geometry.hpp>
# include <boost/geometry/geometries/register/point.hpp>

BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Point, s3d::int32, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Float2, float, boost::geometry::cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_POINT_2D(s3d::Vec2, double, boost::geometry::cs::cartesian, x, y)

namespace s3d
{
	using CwOpenPolygon = boost::geometry::model::polygon<Vec2, false, false, Array, Array>;

	class Polygon::PolygonDetail
	{
	public:

		[[nodiscard]]
		PolygonDetail() = default;

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, SkipValidation skipValidation);

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation);

		[[nodiscard]]
		PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, SkipValidation skipValidation);

	private:

		CwOpenPolygon m_polygon;

		Array<Array<Vec2>> m_holes;

		Array<Float2> m_vertices;

		Array<TriangleIndex> m_indices;

		RectF m_boundingRect = RectF::Empty();
	};
}
