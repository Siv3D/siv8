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
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/Geometry2D/PointContainment.hpp>
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

			struct Component
			{
				CwOpenPolygon polygon;
				RectF bounds;
				Array<size_t> holeIndices;
			};

			Array<Component> outers;
			Array<std::span<const Vec2>> holes;

			for (const auto& ring : rings)
			{
				if (ring.size() < 3)
				{
					continue;
				}

				if (Geometry2D::IsClockwise(ring))
				{
					auto corrected = detail::CorrectPolygonRings(ring);

					for (auto& polygon : corrected)
					{
						const RectF bounds = Geometry2D::BoundingRect(polygon.outer());
						outers.push_back(Component{ std::move(polygon), bounds, {} });
					}
				}
				else
				{
					holes.emplace_back(ring);
				}
			}

			// 穴の割り当ては、描画用 Float2 頂点や三角形を作らずに元の輪郭で判定する。
			for (size_t holeIndex = 0; holeIndex < holes.size(); ++holeIndex)
			{
				size_t owner = outers.size();
				double distance = Inf<double>;
				const Vec2& point = holes[holeIndex].front();

				for (size_t i = 0; i < outers.size(); ++i)
				{
					const auto& component = outers[i];
					const auto& polygon = component.polygon;

					if (not Geometry2D::Contains(component.bounds, point)
						|| not Geometry2D::ContainsPoint<PointContainmentOptions{ .boundary = PointContainmentBoundaryPolicy::Included }, Vec2>(polygon.outer(), point))
					{
						continue;
					}

					// 外周の修復によって既に生じた穴の内部は、割り当て対象から除く。
					if (polygon.inners().any([&point](const auto& inner)
						{
							return Geometry2D::ContainsPoint<PointContainmentOptions{ .boundary = PointContainmentBoundaryPolicy::Excluded }, Vec2>(inner, point);
						}))
					{
						continue;
					}

					const auto& outer = polygon.outer();
					double d = Inf<double>;

					for (size_t j = 0; j < outer.size(); ++j)
					{
						d = Min(d, Geometry2D::Distance(point, Line{ outer[j], outer[(j + 1) % outer.size()] }));
					}

					if (d < distance)
					{
						distance = d;
						owner = i;
					}
				}

				if (owner != outers.size())
				{
					outers[owner].holeIndices.push_back(holeIndex);
				}
			}

			MultiPolygon results;
			results.reserve(outers.size());

			for (auto& component : outers)
			{
				auto& polygon = component.polygon;
				const size_t originalHoleCount = polygon.inners().size();
				polygon.inners().reserve(originalHoleCount + component.holeIndices.size());

				for (const size_t holeIndex : component.holeIndices)
				{
					const auto hole = holes[holeIndex];
					polygon.inners().emplace_back(hole.begin(), hole.end());
				}

				const bool valid = (component.holeIndices.isEmpty() || (detail::ValidatePolygon(polygon) == PolygonFailureType::Ok));

				if (not valid)
				{
					polygon.inners().resize(originalHoleCount);
				}

				Polygon result{ detail::OpenRingView(polygon.outer()), detail::CopyPolygonHoles(polygon), component.bounds, SkipValidation::Yes };

				if (not valid)
				{
					// 不正な穴が混在する場合も、従来どおり追加できる穴だけを入力順に採用する。
					for (const size_t holeIndex : component.holeIndices)
					{
						const auto hole = holes[holeIndex];
						result.addHole(Array<Vec2>{ hole.begin(), hole.end() });
					}
				}

				if (result)
				{
					results.push_back(std::move(result));
				}
			}

			return results;
		}
	}
}
