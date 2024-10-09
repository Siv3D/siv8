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
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/PolygonFailureType.hpp>
# include "PolygonDetail.hpp"
# include "Triangulate.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool HasDuplicatePoints(std::span<const Vec2> points)
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
		static CwOpenPolygon MakeCWOpenPolygon(std::span<const Vec2> outerVertices, const Array<Array<Vec2>>& holes)
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
		static size_t GetVertexCount(const Array<Array<Vec2>>& holes) noexcept
		{
			size_t count = 0;

			for (const auto& hole : holes)
			{
				count += hole.size();
			}

			return count;
		}

		[[nodiscard]]
		static Array<Float2> MakeVertices(std::span<const Vec2> outerVertices, const Array<Array<Vec2>>& holes)
		{
			Array<Float2> vertices{ Arg::reserve = (outerVertices.size() + GetVertexCount(holes)) };

			vertices.append_range(outerVertices);

			for (const auto& hole : holes)
			{
				vertices.append_range(hole);
			}

			return vertices;
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
	}

	Polygon::PolygonDetail::PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (not skipValidation)
		{
			if (Validate(polygon) != PolygonFailureType::OK)
			{
				return;
			}
		}

		if (not Triangulate(outer, holes, m_indices))
		{
			return;
		}

		m_polygon		= std::move(polygon);

		m_vertices		= MakeVertices(outer, holes);

		m_holes			= std::move(holes);

		m_boundingRect	= Geometry2D::BoundingRect(outer);
	}

	Polygon::PolygonDetail::PolygonDetail(std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, {});

		if (not skipValidation)
		{
			if (Validate(polygon) != PolygonFailureType::OK)
			{
				return;
			}
		}

		m_indices		= std::move(indices);

		m_polygon		= std::move(polygon);

		m_vertices.assign_range(outer);

		m_boundingRect	= boundingRect;
	}

	Polygon::PolygonDetail::PolygonDetail(std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (not skipValidation)
		{
			if (Validate(polygon) != PolygonFailureType::OK)
			{
				return;
			}
		}

		m_indices		= std::move(indices);

		m_polygon		= std::move(polygon);

		m_vertices		= std::move(vertices);

		m_holes			= std::move(holes);

		m_boundingRect	= boundingRect;
	}
}
