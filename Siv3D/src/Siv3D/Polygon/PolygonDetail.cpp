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

# include <Siv3D/HashSet.hpp>
# include <Siv3D/PolygonFailureType.hpp>
# include "PolygonDetail.hpp"

namespace s3d
{
	[[nodiscard]]
	static bool HasDuplicatePoints(std::span<const Vec2> points)
	{
		return (HashSet<Vec2>{ points.begin(), points.end() }.size() != points.size());
	}

	[[nodiscard]]
	static PolygonFailureType ToPolygonFailureType(const boost::geometry::validity_failure_type failure)
	{
		// https://www.boost.org/doc/libs/1_84_0/libs/geometry/doc/html/geometry/reference/enumerations/validity_failure_type.html
		switch (failure)
		{
		case boost::geometry::no_failure:
			return PolygonFailureType::OK;
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

	[[nodiscard]]
	static CwOpenPolygon MakeCWOpenPolygon(std::span<const Vec2> outerVertices, std::span<const std::span<const Vec2>> holes)
	{
		CwOpenPolygon polygon;

		auto& outer = polygon.outer();
		{
			outer.assign_range(outerVertices);

			polygon.inners().reserve(holes.size());

			for (const auto& hole : holes)
			{
				polygon.inners().emplace_back(hole.begin(), hole.end());
			}
		}

		return polygon;
	}

	[[nodiscard]]
	static Array<Array<Vec2>> MakeHoles(std::span<const std::span<const Vec2>> holes)
	{
		Array<Array<Vec2>> result(holes.size());

		for (size_t i = 0; i < holes.size(); ++i)
		{
			result[i].assign(holes[i].begin(), holes[i].end());
		}

		return result;
	}

	[[nodiscard]]
	static RectF CalculateBoundingRect(std::span<const Vec2> points)
	{
		assert(points.size() != 0);

		const Vec2* it = points.data();
		const Vec2* itEnd = (it + points.size());
		double left = it->x;
		double top = it->y;
		double right = left;
		double bottom = top;
		++it;

		while (it != itEnd)
		{
			if (it->x < left)
			{
				left = it->x;
			}
			else if (right < it->x)
			{
				right = it->x;
			}

			if (it->y < top)
			{
				top = it->y;
			}
			else if (bottom < it->y)
			{
				bottom = it->y;
			}

			++it;
		}

		return{ left, top, (right - left), (bottom - top) };
	}

	[[nodiscard]]
	static PolygonFailureType Validate(const CwOpenPolygon& polygon)
	{
		boost::geometry::validity_failure_type failure = boost::geometry::no_failure;

		if (boost::geometry::is_valid(polygon, failure))
		{
			// 非連続な頂点どうしの重複は boost::geometry::is_valid() で取得できないので、HasDuplicatePoints() でチェック
			if (HasDuplicatePoints(polygon.outer()))
			{
				return PolygonFailureType::DuplicatePoints;
			}

			for (const auto& inner : polygon.inners())
			{
				if (inner.size() < 3)
				{
					return PolygonFailureType::FewPoints;
				}

				if (HasDuplicatePoints(inner))
				{
					return PolygonFailureType::DuplicatePoints;
				}
			}
		}

		return ToPolygonFailureType(failure);
	}

	Polygon::PolygonDetail::PolygonDetail(std::span<const Vec2> outer, std::span<const std::span<const Vec2>> holes, const SkipValidation skipValidation)
	{
		if (outer.size() < 3)
		{
			return;
		}

		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (not skipValidation)
		{
			if (Validate(polygon) != PolygonFailureType::OK)
			{
				return;
			}
		}

		m_holes = MakeHoles(holes);

		{
			Array<Vertex2D::IndexType> indices;
			//Triangulate(m_polygon.outer(), m_holes, m_vertices, indices);
			//assert(indices.size() % 3 == 0);
			//
			
			m_indices.resize(indices.size() / 3);
			std::memcpy(m_indices.data(), indices.data(), indices.size_bytes());
		}

		m_boundingRect = CalculateBoundingRect(outer);
	}
}
