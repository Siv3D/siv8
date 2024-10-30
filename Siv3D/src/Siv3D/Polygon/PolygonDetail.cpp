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
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "PolygonDetail.hpp"
# include "Triangulate.hpp"
# include <ThirdParty/boost/geometry/extensions/algorithms/dissolve.hpp>

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
		static CwOpenPolygon MakeCWOpenPolygon(const std::span<const Vec2> outerVertices, const Array<Array<Vec2>>& holes)
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
		static CwOpenPolygon MakeCWOpenPolygon(const std::span<const Float2> outerVertices)
		{
			CwOpenPolygon polygon;

			auto& outer = polygon.outer();
			{
				outer.assign_range(outerVertices);
			}

			return polygon;
		}

		[[nodiscard]]
		static Polygon ToPolygon(const CwOpenPolygon& polygon)
		{
			std::span<const Vec2> outer = polygon.outer();

			if ((2 < outer.size()) && (outer.front() == outer.back()))
			{
				outer = outer.subspan(0, (outer.size() - 1));
			}

			const auto& inners = polygon.inners();

			Array<Array<Vec2>> holes(inners.size());

			for (size_t i = 0; i < holes.size(); ++i)
			{
				std::span<const Vec2> inner = inners[i];

				if ((2 < inner.size()) && (inner.front() == inner.back()))
				{
					inner = inner.subspan(0, (inner.size() - 1));
				}

				holes[i].assign(inner.rbegin(), inner.rend());
			}

			return Polygon{ outer, std::move(holes), SkipValidation::Yes};
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
		static PolygonFailureType ValidatePolygon(const CwOpenPolygon& polygon)
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

		/// @brief 三角形の面積の 2 倍を計算します。
		/// @param p0 頂点 0
		/// @param p1 頂点 1
		/// @param p2 頂点 2
		/// @return 三角形の面積の 2 倍
		[[nodiscard]]
		static constexpr double TriangleArea2x(const Float2& p0, const Float2& p1, const Float2& p2) noexcept
		{
			return Abs((p0.x - p2.x) * (p1.y - p0.y) - (p0.x - p1.x) * (p2.y - p0.y));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (not skipValidation)
		{
			if (ValidatePolygon(polygon) != PolygonFailureType::OK)
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

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (not skipValidation)
		{
			if (ValidatePolygon(polygon) != PolygonFailureType::OK)
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

		m_boundingRect	= boundingRect;
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, {});

		if (not skipValidation)
		{
			if (ValidatePolygon(polygon) != PolygonFailureType::OK)
			{
				return;
			}
		}

		m_indices		= std::move(indices);

		m_polygon		= std::move(polygon);

		m_vertices.assign_range(outer);

		m_boundingRect	= boundingRect;
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (not skipValidation)
		{
			if (ValidatePolygon(polygon) != PolygonFailureType::OK)
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

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Float2> outer, Array<TriangleIndex> indices)
	{
		CwOpenPolygon polygon	= MakeCWOpenPolygon(outer);

		m_indices				= std::move(indices);

		m_polygon				= std::move(polygon);

		m_vertices.assign_range(outer);

		m_boundingRect			= Geometry2D::BoundingRect(outer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::PolygonDetail::isEmpty() const noexcept
	{
		return m_polygon.outer().empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	const Array<Vec2>& Polygon::PolygonDetail::outer() const noexcept
	{
		return m_polygon.outer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	inners
	//
	////////////////////////////////////////////////////////////////

	const Array<Array<Vec2>>& Polygon::PolygonDetail::inners() const noexcept
	{
		return m_holes;
	}

	////////////////////////////////////////////////////////////////
	//
	//	vertices
	//
	////////////////////////////////////////////////////////////////

	const Array<Float2>& Polygon::PolygonDetail::vertices() const noexcept
	{
		return m_vertices;
	}

	////////////////////////////////////////////////////////////////
	//
	//	indices
	//
	////////////////////////////////////////////////////////////////

	const Array<TriangleIndex>& Polygon::PolygonDetail::indices() const noexcept
	{
		return m_indices;
	}

	////////////////////////////////////////////////////////////////
	//
	//	boundingRect
	//
	////////////////////////////////////////////////////////////////

	const RectF& Polygon::PolygonDetail::boundingRect() const noexcept
	{
		return m_boundingRect;
	}



	////////////////////////////////////////////////////////////////
	//
	//	area
	//
	////////////////////////////////////////////////////////////////

	double Polygon::PolygonDetail::area() const noexcept
	{
		const TriangleIndex* pIndex = m_indices.data();
		const TriangleIndex* const pIndexEnd = (pIndex + m_indices.size());
		const Float2* pVertex = m_vertices.data();

		double result = 0.0;

		while (pIndex != pIndexEnd)
		{
			result += TriangleArea2x(pVertex[pIndex->i0], pVertex[pIndex->i1], pVertex[pIndex->i2]);
			++pIndex;
		}

		return (result * 0.5);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	double Polygon::PolygonDetail::perimeter() const noexcept
	{
		double result = 0.0;

		// 外周の長さ
		{
			const auto& outer = m_polygon.outer();
			
			if (const size_t num_outer = outer.size())
			{
				const Vec2* pOuter = outer.data();

				for (size_t i = 0; i < (num_outer - 1); ++i)
				{
					result += pOuter[i].distanceFrom(pOuter[i + 1]);
				}

				result += pOuter[num_outer - 1].distanceFrom(pOuter[0]);
			}
		}

		// 各穴の周の長さ
		for (const auto& inner : m_polygon.inners())
		{
			if (const size_t num_inner = inner.size())
			{
				const Vec2* pInner = inner.data();
				
				for (size_t i = 0; i < (num_inner - 1); ++i)
				{
					result += pInner[i].distanceFrom(pInner[i + 1]);
				}
				
				result += pInner[num_inner - 1].distanceFrom(pInner[0]);
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	Vec2 Polygon::PolygonDetail::centroid() const
	{
		if (outer().isEmpty())
		{
			return{ 0, 0 };
		}

		Vec2 centroid;

		boost::geometry::centroid(m_polygon, centroid);

		return centroid;
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeConvexHull
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::computeConvexHull() const
	{
		CWOpenRing result;

		boost::geometry::convex_hull(m_polygon.outer(), result);

		return Polygon{ result, m_boundingRect, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	calculateBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::calculateBuffer(const double distance) const
	{
		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(m_polygon, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_miter{},
			boost::geometry::strategy::buffer::end_round{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	calculateRoundBuffer
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::calculateRoundBuffer(const double distance, const QualityFactor& qualityFactor) const
	{
		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(m_polygon, multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ detail::CalculateCircleQuality(distance * qualityFactor.value()) },
			boost::geometry::strategy::buffer::end_round{},
			boost::geometry::strategy::buffer::point_circle{ 0 });

		if (multiPolygon.size() != 1)
		{
			return{};
		}

		return ToPolygon(multiPolygon.front());
	}

	////////////////////////////////////////////////////////////////
	//
	//	simplified
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::simplified(const double maxDistance) const
	{
		if (not m_polygon.outer())
		{
			return{};
		}

		CwOpenPolygon result;
		boost::geometry::simplify(m_polygon, result, maxDistance);

		if (result.outer().empty())
		{
			return{};
		}

		return ToPolygon(result);
	}

	////////////////////////////////////////////////////////////////
	//
	//	intersects
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::PolygonDetail::intersects(const Vec2& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const Float2* pVertex = m_vertices.data();

		for (const auto& triangleIndex : m_indices)
		{
			const Triangle triangle{ pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2] };

			if (Geometry2D::Intersect(other, triangle))
			{
				return true;
			}
		}

		return false;
	}

	bool Polygon::PolygonDetail::intersects(const Line& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const Float2* pVertex = m_vertices.data();

		for (const auto& triangleIndex : m_indices)
		{
			const Triangle triangle{ pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2] };

			if (Geometry2D::Intersect(other, triangle))
			{
				return true;
			}
		}

		return false;
	}

	bool Polygon::PolygonDetail::intersects(const RectF& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const boost::geometry::model::box<Vec2> box{ other.pos, other.br() };

		return boost::geometry::intersects(m_polygon, box);
	}

	bool Polygon::PolygonDetail::intersects(const Circle& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const Float2* pVertex = m_vertices.data();

		for (const auto& triangleIndex : m_indices)
		{
			const Triangle triangle{ pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2] };

			if (Geometry2D::Intersect(other, triangle))
			{
				return true;
			}
		}

		return false;
	}

	bool Polygon::PolygonDetail::intersects(const Ellipse& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const Float2* pVertex = m_vertices.data();

		for (const auto& triangleIndex : m_indices)
		{
			const Triangle triangle{ pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2] };

			if (Geometry2D::Intersect(other, triangle))
			{
				return true;
			}
		}

		return false;
	}

	bool Polygon::PolygonDetail::intersects(const Triangle& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const Float2* pVertex = m_vertices.data();

		for (const auto& triangleIndex : m_indices)
		{
			const Triangle triangle{ pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2] };

			if (Geometry2D::Intersect(other, triangle))
			{
				return true;
			}
		}

		return false;
	}

	bool Polygon::PolygonDetail::intersects(const Quad& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other, m_boundingRect))
		{
			return false;
		}

		const Float2* pVertex = m_vertices.data();

		for (const auto& triangleIndex : m_indices)
		{
			const Triangle triangle{ pVertex[triangleIndex.i0], pVertex[triangleIndex.i1], pVertex[triangleIndex.i2] };

			if (Geometry2D::Intersect(other, triangle))
			{
				return true;
			}
		}

		return false;
	}

	bool Polygon::PolygonDetail::intersects(const PolygonDetail& other) const
	{
		if (isEmpty())
		{
			return false;
		}

		if (other.isEmpty())
		{
			return false;
		}

		if (not Geometry2D::Intersect(other.m_boundingRect, m_boundingRect))
		{
			return false;
		}

		return boost::geometry::intersects(m_polygon, other.m_polygon);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::draw(const ColorF& color) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(m_vertices, m_indices, none, color.toFloat4());
	}

	void Polygon::PolygonDetail::draw(const Vec2& offset, const ColorF& color) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(m_vertices, m_indices, offset, color.toFloat4());
	}

	void Polygon::PolygonDetail::draw(const PatternParameters& pattern) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(m_vertices, m_indices, none, pattern);
	}

	void Polygon::PolygonDetail::draw(const Vec2& offset, const PatternParameters& pattern) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(m_vertices, m_indices, offset, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Validate
	//
	////////////////////////////////////////////////////////////////

	PolygonFailureType Polygon::PolygonDetail::Validate(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		return ValidatePolygon(MakeCWOpenPolygon(outer, holes));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Correct
	//
	////////////////////////////////////////////////////////////////

	Array<Polygon> Polygon::PolygonDetail::Correct(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		CwOpenPolygon polygon = MakeCWOpenPolygon(outer, holes);

		if (ValidatePolygon(polygon) == PolygonFailureType::OK)
		{
			return{ Polygon{ outer, holes, SkipValidation::Yes } };
		}

		using MultiCwOpenPolygon = boost::geometry::model::multi_polygon<CwOpenPolygon>;
		boost::geometry::correct(polygon);
		
		MultiCwOpenPolygon solvedPolygons;
		boost::geometry::dissolve(polygon, solvedPolygons);

		Array<Polygon> results;

		for (const auto& solvedPolygon : solvedPolygons)
		{
			Array<Array<Vec2>> retHoles;

			for (const auto& hole : solvedPolygon.inners())
			{
				retHoles.emplace_back(hole.begin(), hole.end());
			}

			if (Validate(solvedPolygon.outer(), retHoles) == PolygonFailureType::OK)
			{
				results.emplace_back(solvedPolygon.outer(), retHoles);
			}
		}

		return results;
	}
}
