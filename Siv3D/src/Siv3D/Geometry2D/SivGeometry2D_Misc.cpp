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

# include <Siv3D/Geometry2D/Misc.hpp>
# include <Siv3D/Polygon/GeometryCommon.hpp>
# include <Siv3D/Polygon/PolygonDetail.hpp>

namespace s3d
{
	using GBox = boost::geometry::model::box<Vec2>;

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		Array<Polygon> And(const RectF& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::intersection(GBox{ a.pos, a.br() }, b._detail()->getBoostPolygon(), results);
			return results.map(detail::ToPolygon);
		}

		Array<Polygon> And(const Polygon& a, const RectF& b)
		{
			return And(b, a);
		}

		Array<Polygon> And(const Polygon& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::intersection(a._detail()->getBoostPolygon(), b._detail()->getBoostPolygon(), results);
			return results.map(detail::ToPolygon);
		}
	}
}
