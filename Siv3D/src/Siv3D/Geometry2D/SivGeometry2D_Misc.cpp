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
# include <Siv3D/Geometry2D/IsClockwise.hpp>
# include <Siv3D/Geometry2D/Contains.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Number.hpp>
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



		//////////////////////////////////////////////////
		//
		//	ComposePolygons
		//
		//////////////////////////////////////////////////

		MultiPolygon ComposePolygons(const Array<LineString>& rings)
		{
			//-----------------------------------------------
			//	Authors (OpenSiv3D challenge #10 participants)
			//	- Ebishu
			//	- fal_rnd
			//-----------------------------------------------

			MultiPolygon results, outers;
			Array<LineString> holes;

			for (const auto& ring : rings)
			{
				if (Geometry2D::IsClockwise(ring))
				{
					const Array<Polygon> polygons = Polygon::Correct(ring);
					outers.append_range(polygons);
					results.append_range(polygons);
				}
				else
				{
					holes << ring;
				}
			}

			const size_t outers_size = outers.size();

			for (const LineString& hole : holes)
			{
				size_t w = (size_t)-1;
				double dist = Inf<double>;

				const Vec2& point = hole.front();

				for (size_t i = 0; i < outers_size; ++i)
				{
					if (Geometry2D::Contains(outers[i], point))
					{
						const Array<Vec2>& outer = outers[i].outer();

						double d = Inf<double>;

						for (size_t j = 0, outer_size = outer.size(); j < outer_size; ++j)
						{
							d = Min(d, Geometry2D::Distance(point, Line{ outer[j], outer[(j + 1) % outer_size] }));
						}

						if (d < dist)
						{
							dist = d;
							w = i;
						}
					}
				}

				if (w != (size_t)-1)
				{
					results[w].addHole(hole);
				}
			}

			return results;
		}
	}
}
