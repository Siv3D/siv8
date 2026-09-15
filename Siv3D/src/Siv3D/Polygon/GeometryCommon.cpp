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

# include "GeometryCommon.hpp"
# include <Siv3D/HashSet.hpp>
# include <algorithm>

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4127)
# include <ThirdParty/boost/geometry/extensions/algorithms/dissolve.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool HasDuplicatePoints(const std::span<const Vec2> points)
		{
			return (HashSet<Vec2>{ points.begin(), points.end() }.size() != points.size());
		}

		[[nodiscard]]
		static constexpr PolygonFailureType ToPolygonFailureType(const boost::geometry::validity_failure_type failure) noexcept
		{
			// https://www.boost.org/doc/libs/1_84_0/libs/geometry/doc/html/geometry/reference/enumerations/validity_failure_type.html
			switch (failure)
			{
			case boost::geometry::no_failure:
				return PolygonFailureType::Ok;
			case boost::geometry::failure_few_points:
				return PolygonFailureType::FewPoints;
			case boost::geometry::failure_wrong_topological_dimension:
				return PolygonFailureType::WrongTopologicalDimension;
			case boost::geometry::failure_spikes:
				return PolygonFailureType::Spikes;
			case boost::geometry::failure_duplicate_points:
				return PolygonFailureType::DuplicatePoints;
			case boost::geometry::failure_not_closed:
				return PolygonFailureType::NotClosed;
			case boost::geometry::failure_self_intersections:
				return PolygonFailureType::SelfIntersections;
			case boost::geometry::failure_wrong_orientation:
				return PolygonFailureType::WrongOrientation;
			case boost::geometry::failure_interior_rings_outside:
				return PolygonFailureType::InteriorRingsOutside;
			case boost::geometry::failure_nested_interior_rings:
				return PolygonFailureType::NestedInteriorRings;
			case boost::geometry::failure_disconnected_interior:
				return PolygonFailureType::DisconnectedInterior;
			case boost::geometry::failure_intersecting_interiors:
				return PolygonFailureType::IntersectingInteriors;
			case boost::geometry::failure_wrong_corner_order:
				return PolygonFailureType::WrongCornerOrder;
			case boost::geometry::failure_invalid_coordinate:
				return PolygonFailureType::InvalidCoordinate;
			default:
				return PolygonFailureType::Unknown;
			}
		}

		template <class InnerRing>
		[[nodiscard]]
		PolygonFailureType ValidateRings(const std::span<const Vec2> outer, const Array<InnerRing>& holes)
		{
			// boost::geometry::is_valid() が検出しない、非連続な頂点の重複も検査する。
			if (HasDuplicatePoints(outer))
			{
				return PolygonFailureType::DuplicatePoints;
			}

			for (const auto& hole : holes)
			{
				if (hole.size() < 3)
				{
					return PolygonFailureType::FewPoints;
				}

				if (HasDuplicatePoints(hole))
				{
					return PolygonFailureType::DuplicatePoints;
				}
			}

			return PolygonFailureType::Ok;
		}

		[[nodiscard]]
		PolygonFailureType ValidateTopology(const CwOpenPolygon& polygon)
		{
			boost::geometry::validity_failure_type failure = boost::geometry::no_failure;
			boost::geometry::is_valid(polygon, failure);
			return ToPolygonFailureType(failure);
		}
	}

	namespace detail
	{
		std::span<const Vec2> OpenRingView(std::span<const Vec2> ring) noexcept
		{
			if ((2 < ring.size()) && (ring.front() == ring.back()))
			{
				ring = ring.first(ring.size() - 1);
			}

			return ring;
		}

		Array<Array<Vec2>> CopyPolygonHoles(const CwOpenPolygon& polygon)
		{
			Array<Array<Vec2>> holes{ Arg::reserve = polygon.inners().size() };

			for (const auto& inner : polygon.inners())
			{
				const auto ring = OpenRingView(inner);
				holes.emplace_back(ring.begin(), ring.end());
			}

			return holes;
		}

		Polygon ToPolygon(const CwOpenPolygon& polygon, const SkipValidation skipValidation)
		{
			return Polygon{ OpenRingView(polygon.outer()), CopyPolygonHoles(polygon), skipValidation };
		}

		CwOpenPolygon ToCwOpenPolygon(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
		{
			CwOpenPolygon polygon;
			polygon.outer().assign_range(outer);
			polygon.inners().reserve(holes.size());

			for (const auto& hole : holes)
			{
				polygon.inners().emplace_back(hole.begin(), hole.end());
			}

			return polygon;
		}

		CwOpenPolygon ToCwOpenPolygon(const Polygon& polygon)
		{
			return ToCwOpenPolygon(polygon.outer(), polygon.inners());
		}

		PolygonFailureType ValidatePolygon(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
		{
			if (const auto failure = ValidateRings(outer, holes); failure != PolygonFailureType::Ok)
			{
				return failure;
			}

			return ValidateTopology(ToCwOpenPolygon(outer, holes));
		}

		PolygonFailureType ValidatePolygon(const CwOpenPolygon& polygon)
		{
			if (const auto failure = ValidateRings(polygon.outer(), polygon.inners()); failure != PolygonFailureType::Ok)
			{
				return failure;
			}

			return ValidateTopology(polygon);
		}

		CwOpenMultiPolygon CorrectPolygonRings(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
		{
			CwOpenPolygon polygon = ToCwOpenPolygon(outer, holes);
			CwOpenMultiPolygon results;

			if (ValidatePolygon(polygon) == PolygonFailureType::Ok)
			{
				results.push_back(std::move(polygon));
				return results;
			}

			boost::geometry::correct(polygon);
			boost::geometry::dissolve(polygon, results);
			results.erase(std::remove_if(results.begin(), results.end(), [](const CwOpenPolygon& result)
				{
					return (ValidatePolygon(result) != PolygonFailureType::Ok);
				}), results.end());
			return results;
		}

		size_t CalculateCircleQuality(const double r) noexcept
		{
			if (r <= 2.0)
			{
				return 6;
			}
			else if (r <= 4.0)
			{
				return 10;
			}
			else if (r < 8.0)
			{
				return 12;
			}
			else
			{
				return Min((12 + static_cast<int32>((r - 8.0) / 2.0)), 256);
			}
		}
	}
}
