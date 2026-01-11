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
	//	CalculateLineRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon CalculateLineRoundBuffer(const Line& line, const double distance, const QualityFactor& qualityFactor)
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
			boost::geometry::strategy::buffer::end_round{ detail::CalculateCircleQuality(distance * qualityFactor.value()) },
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return detail::ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	CalculateRoundedQuad
	//
	////////////////////////////////////////////////////////////////

	Polygon CalculateRoundedQuad(const Quad& quad, const double distance, const QualityFactor& qualityFactor)
	{
		const CwOpenPolygon polygon{ { quad.p0, quad.p1, quad.p2, quad.p3 } };

		boost::geometry::model::multi_polygon<CwOpenPolygon> stretchedPolygon;

		boost::geometry::buffer(polygon, stretchedPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ -distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_miter{},
			boost::geometry::strategy::buffer::end_flat{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (stretchedPolygon.size() != 1)
		{
			return{};
		}

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(stretchedPolygon.front(), multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ detail::CalculateCircleQuality(distance * qualityFactor.value()) },
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
	//	CalculatePolygonBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon CalculatePolygonBuffer(std::initializer_list<Vec2> outer, const double distance)
	{
		const CwOpenPolygon polygon{ outer };

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
	//	CalculatePolygonRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon CalculatePolygonRoundBuffer(std::initializer_list<Vec2> outer, const double distance, const QualityFactor& qualityFactor)
	{
		const CwOpenPolygon polygon{ outer };

		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(polygon, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ detail::CalculateCircleQuality(distance * qualityFactor.value()) },
			boost::geometry::strategy::buffer::end_round{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return detail::ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	CircleToPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon CircleToPolygon(const Circle& circle, const QualityFactor& qualityFactor)
	{
		if (circle.r <= 0.0)
		{
			return{};
		}

		const size_t quality = detail::CalculateCircleQuality(circle.r * qualityFactor.value());

		Array<Vec2> vertices(quality, circle.center);
		Vec2* pPos = vertices.data();

		const double d = (Math::TwoPi / quality);

		for (uint32 i = 0; i < quality; ++i)
		{
			*pPos += Circular{ circle.r, (i * d) }.fastToVec2();
			++pPos;
		}

		const RectF boundingRect = circle.boundingRect();

		Array<TriangleIndex> indices(quality - 2);
		TriangleIndex* pIndex = indices.data();

		for (Vertex2D::IndexType i = 0; i < (quality - 2); ++i)
		{
			pIndex->i0 = 0;
			pIndex->i1 = (i + 1);
			pIndex->i2 = (i + 2);
			++pIndex;
		}

		return Polygon{ vertices, std::move(indices), boundingRect, SkipValidation::Yes };
	}
}
