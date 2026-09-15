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

# include <Siv3D/Geometry2D/Misc.hpp>
# include <Siv3D/Geometry2D/IsClockwise.hpp>
# include <Siv3D/Geometry2D/Contains.hpp>
# include <Siv3D/Geometry2D/Distance.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Number.hpp>
# include <Siv3D/Polygon/GeometryCommon.hpp>

namespace s3d
{
	[[nodiscard]]
	static constexpr boost::geometry::model::box<Vec2> ToGBox(const RectF& rect) noexcept
	{
		return{ rect.pos, rect.br() };
	}

	namespace Geometry2D
	{
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		MultiPolygon And(const RectF& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::intersection(ToGBox(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		MultiPolygon And(const Polygon& a, const RectF& b)
		{
			return And(b, a);
		}

		MultiPolygon And(const Polygon& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::intersection(detail::ToCwOpenPolygon(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		//////////////////////////////////////////////////
		//
		//	Or
		//
		//////////////////////////////////////////////////

		MultiPolygon Or(const RectF& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::union_(ToGBox(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		MultiPolygon Or(const Polygon& a, const RectF& b)
		{
			return Or(b, a);
		}

		MultiPolygon Or(const Polygon& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::union_(detail::ToCwOpenPolygon(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		MultiPolygon Or(const MultiPolygon& a, const Polygon& b)
		{
			boost::geometry::model::multi_polygon<CwOpenPolygon> polygons;
			{
				for (const auto& ap : a)
				{
					polygons.push_back(detail::ToCwOpenPolygon(ap));
				}
			}

			boost::geometry::model::multi_polygon<CwOpenPolygon> unions;
			boost::geometry::union_(polygons, detail::ToCwOpenPolygon(b), unions);

			MultiPolygon results;
			{
				for (const auto& polygon : unions)
				{
					if (Polygon newPolygon = detail::ToPolygon(polygon, SkipValidation::No))
					{
						results.push_back(std::move(newPolygon));
					}
				}
			}

			return results;
		}

		//////////////////////////////////////////////////
		//
		//	Xor
		//
		//////////////////////////////////////////////////

		MultiPolygon Xor(const RectF& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::sym_difference(ToGBox(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		MultiPolygon Xor(const Polygon& a, const RectF& b)
		{
			return Xor(b, a);
		}

		MultiPolygon Xor(const Polygon& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::sym_difference(detail::ToCwOpenPolygon(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		//////////////////////////////////////////////////
		//
		//	Subtract
		//
		//////////////////////////////////////////////////

		MultiPolygon Subtract(const RectF& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::difference(ToGBox(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		MultiPolygon Subtract(const Polygon& a, const RectF& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::difference(detail::ToCwOpenPolygon(a), ToGBox(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		MultiPolygon Subtract(const Polygon& a, const Polygon& b)
		{
			Array<CwOpenPolygon> results;
			boost::geometry::difference(detail::ToCwOpenPolygon(a), detail::ToCwOpenPolygon(b), results);
			return results.map([](const CwOpenPolygon& polygon) { return detail::ToPolygon(polygon); });
		}

		//////////////////////////////////////////////////
		//
		//	DiscreteFrechetDistance
		//
		//////////////////////////////////////////////////

		double DiscreteFrechetDistance(const LineString& a, const LineString& b)
		{
			if (a.isEmpty() || b.isEmpty())
			{
				return Math::Inf;
			}

			return boost::geometry::discrete_frechet_distance(a, b);
		}

		//////////////////////////////////////////////////
		//
		//	DiscreteHausdorffDistance
		//
		//////////////////////////////////////////////////

		double DiscreteHausdorffDistance(const LineString& a, const LineString& b)
		{
			if (a.isEmpty() || b.isEmpty())
			{
				return Math::Inf;
			}

			return boost::geometry::discrete_hausdorff_distance(a, b);
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
