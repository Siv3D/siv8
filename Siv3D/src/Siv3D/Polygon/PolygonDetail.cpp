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

# include <Siv3D/HashSet.hpp>
# include <Siv3D/LineCap.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "PolygonDetail.hpp"
# include "Triangulate.hpp"

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
		static PolygonData MakePolygonData(const std::span<const Vec2> outerVertices, Array<Array<Vec2>> holes)
		{
			PolygonData polygonData;
			polygonData.outer.assign(outerVertices.begin(), outerVertices.end());
			polygonData.inners = std::move(holes);
			return polygonData;
		}

		[[nodiscard]]
		static PolygonData MakePolygonData(const std::span<const Float2> outerVertices)
		{
			PolygonData polygonData;
			polygonData.outer.assign(outerVertices.begin(), outerVertices.end());
			return polygonData;
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
		static CwOpenPolygon ToCWOpenPolygon(const PolygonData& polygonData)
		{
			return MakeCWOpenPolygon(polygonData.outer, polygonData.inners);
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
		static PolygonFailureType ValidatePolygon(const PolygonData& polygonData)
		{
			boost::geometry::validity_failure_type failure = boost::geometry::no_failure;

			// 非連続な頂点どうしの重複は boost::geometry::is_valid() で取得できないので、HasDuplicatePoints() でチェック
			if (HasDuplicatePoints(polygonData.outer))
			{
				return PolygonFailureType::DuplicatePoints;
			}

			for (const auto& inner : polygonData.inners)
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

			boost::geometry::is_valid(ToCWOpenPolygon(polygonData), failure);

			return ToPolygonFailureType(failure);
		}

		[[nodiscard]]
		static PolygonFailureType ValidatePolygon(const CwOpenPolygon& polygon)
		{
			boost::geometry::validity_failure_type failure = boost::geometry::no_failure;

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

			boost::geometry::is_valid(polygon, failure);

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

	namespace detail
	{
		Polygon ToPolygon(const CwOpenPolygon& polygon)
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

			return Polygon{ outer, std::move(holes), SkipValidation::Yes };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
	{
		auto polygon = MakePolygonData(outer, holes);

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

		m_boundingRect	= Geometry2D::BoundingRect(outer);
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		auto polygon = MakePolygonData(outer, holes);

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

		m_boundingRect	= boundingRect;
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		auto polygon = MakePolygonData(outer, {});

		if (not skipValidation)
		{
			if (ValidatePolygon(polygon) != PolygonFailureType::OK)
			{
				return;
			}
		}

		m_indices		= std::move(indices);

		m_polygon		= std::move(polygon);

		m_vertices.assign(outer.begin(), outer.end());

		m_boundingRect	= boundingRect;
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, Array<Float2> vertices, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		auto polygon = MakePolygonData(outer, std::move(holes));

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

		m_boundingRect	= boundingRect;
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Float2> outer, Array<TriangleIndex> indices, const RectF& boundingRect)
	{
		auto polygon = MakePolygonData(outer);

		m_indices				= std::move(indices);

		m_polygon				= std::move(polygon);

		m_vertices.assign_range(outer);

		m_boundingRect			= boundingRect;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::PolygonDetail::isEmpty() const noexcept
	{
		return m_polygon.outer.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	outer
	//
	////////////////////////////////////////////////////////////////

	const Array<Vec2>& Polygon::PolygonDetail::outer() const noexcept
	{
		return m_polygon.outer;
	}

	////////////////////////////////////////////////////////////////
	//
	//	inners
	//
	////////////////////////////////////////////////////////////////

	const Array<Array<Vec2>>& Polygon::PolygonDetail::inners() const noexcept
	{
		return m_polygon.inners;
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
	//	moveBy
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::moveBy(const Vec2 v) noexcept
	{
		if (outer().isEmpty())
		{
			return;
		}

		{
			for (auto& point : m_polygon.outer)
			{
				point.moveBy(v);
			}

			for (auto& inner : m_polygon.inners)
			{
				for (auto& point : inner)
				{
					point.moveBy(v);
				}
			}
		}

		{
			const Float2 vf{ v };

			for (auto& vertex : m_vertices)
			{
				vertex.moveBy(vf);
			}
		}

		m_boundingRect.moveBy(v);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateAt
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::rotateAt(const Vec2 pos, const double angle)
	{
		if (outer().isEmpty())
		{
			return;
		}

		if (not pos.isZero())
		{
			for (auto& point : m_polygon.outer)
			{
				point -= pos;
			}

			for (auto& inner : m_polygon.inners)
			{
				for (auto& point : inner)
				{
					point -= pos;
				}
			}

			const Float2 posF = pos;

			for (auto& vertex : m_vertices)
			{
				vertex -= posF;
			}
		}

		const double s = std::sin(angle);
		const double c = std::cos(angle);

		for (auto& point : m_polygon.outer)
		{
			const double x = (point.x * c - point.y * s);
			const double y = (point.x * s + point.y * c);
			point.set(x, y);
		}

		for (auto& inner : m_polygon.inners)
		{
			for (auto& point : inner)
			{
				const double x = (point.x * c - point.y * s);
				const double y = (point.x * s + point.y * c);
				point.set(x, y);
			}
		}

		const float sF = static_cast<float>(s);
		const float cF = static_cast<float>(c);

		for (auto& vertex : m_vertices)
		{
			const float x = (vertex.x * cF - vertex.y * sF);
			const float y = (vertex.x * sF + vertex.y * cF);
			vertex.set(x, y);
		}

		if (not pos.isZero())
		{
			for (auto& point : m_polygon.outer)
			{
				point += pos;
			}

			for (auto& inner : m_polygon.inners)
			{
				for (auto& point : inner)
				{
					point += pos;
				}
			}

			const Float2 posF = pos;

			for (auto& vertex : m_vertices)
			{
				vertex += posF;
			}
		}

		m_boundingRect = Geometry2D::BoundingRect(outer());
	}

	////////////////////////////////////////////////////////////////
	//
	//	transform
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::transform(const double s, const double c, const Vec2& pos)
	{
		if (outer().isEmpty())
		{
			return;
		}

		for (auto& point : m_polygon.outer)
		{
			const double x = (point.x * c - point.y * s + pos.x);
			const double y = (point.x * s + point.y * c + pos.y);
			point.set(x, y);
		}

		for (auto& inner : m_polygon.inners)
		{
			for (auto& point : inner)
			{
				const double x = (point.x * c - point.y * s + pos.x);
				const double y = (point.x * s + point.y * c + pos.y);
				point.set(x, y);
			}
		}

		const float sF = static_cast<float>(s);
		const float cF = static_cast<float>(c);
		const float xF = static_cast<float>(pos.x);
		const float yF = static_cast<float>(pos.y);

		for (auto& vertex : m_vertices)
		{
			const float x = (vertex.x * cF - vertex.y * sF + xF);
			const float y = (vertex.x * sF + vertex.y * cF + yF);
			vertex.set(x, y);
		}

		m_boundingRect = Geometry2D::BoundingRect(outer());
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFromOrigin
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::scaleFromOrigin(const double s)
	{
		if (outer().isEmpty())
		{
			return;
		}

		for (auto& point : m_polygon.outer)
		{
			point *= s;
		}

		for (auto& inner : m_polygon.inners)
		{
			for (auto& point : inner)
			{
				point *= s;
			}
		}

		const float sf = static_cast<float>(s);

		for (auto& point : m_vertices)
		{
			point *= sf;
		}

		m_boundingRect = m_boundingRect.scaledFrom(Vec2{ 0, 0 }, s);
	}

	void Polygon::PolygonDetail::scaleFromOrigin(const Vec2 s)
	{
		if (outer().isEmpty())
		{
			return;
		}
		
		for (auto& point : m_polygon.outer)
		{
			point *= s;
		}
		
		for (auto& inner : m_polygon.inners)
		{
			for (auto& point : inner)
			{
				point *= s;
			}
		}

		const Float2 sf = s;
		
		for (auto& point : m_vertices)
		{
			point *= sf;
		}
		
		m_boundingRect = m_boundingRect.scaledFrom(Vec2{ 0, 0 }, s);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaleFrom
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::scaleFrom(const Vec2 pos, const double s)
	{
		if (outer().isEmpty())
		{
			return;
		}

		for (auto& point : m_polygon.outer)
		{
			point = (pos + (point - pos) * s);
		}

		for (auto& inner : m_polygon.inners)
		{
			for (auto& point : inner)
			{
				point = (pos + (point - pos) * s);
			}
		}

		const float sf = static_cast<float>(s);
		const Float2 posF{ pos };

		for (auto& point : m_vertices)
		{
			point = (posF + (point - posF) * sf);
		}

		m_boundingRect = m_boundingRect.scaledFrom(pos, s);
	}

	void Polygon::PolygonDetail::scaleFrom(const Vec2 pos, const Vec2 s)
	{
		if (outer().isEmpty())
		{
			return;
		}
		
		for (auto& point : m_polygon.outer)
		{
			point = (pos + (point - pos) * s);
		}
		
		for (auto& inner : m_polygon.inners)
		{
			for (auto& point : inner)
			{
				point = (pos + (point - pos) * s);
			}
		}
	
		const Float2 sf = s;
		const Float2 posF{ pos };
		
		for (auto& point : m_vertices)
		{
			point = (posF + (point - posF) * sf);
		}
		
		m_boundingRect = m_boundingRect.scaledFrom(pos, s);
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
			const auto& outer = m_polygon.outer;
			
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
		for (const auto& inner : m_polygon.inners)
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

		boost::geometry::centroid(toCwOpenPolygon(), centroid);

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

		boost::geometry::convex_hull(m_polygon.outer, result);

		return Polygon{ result, m_boundingRect, SkipValidation::Yes };
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::computeMiterBufferPolygon(const double distance) const
	{
		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(toCwOpenPolygon(), multiPolygon,
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
	//	computeRoundBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::computeRoundBufferPolygon(const double distance, const QualityFactor& qualityFactor) const
	{
		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(toCwOpenPolygon(), multiPolygon,
			boost::geometry::strategy::buffer::distance_symmetric<double>{ distance },
			boost::geometry::strategy::buffer::side_straight{},
			boost::geometry::strategy::buffer::join_round{ detail::CalculateCircleQuality(Abs(distance) * qualityFactor.value()) },
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
	//	simplified
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::simplified(const double maxDistance) const
	{
		if (not m_polygon.outer)
		{
			return{};
		}

		CwOpenPolygon result;
		boost::geometry::simplify(toCwOpenPolygon(), result, maxDistance);

		if (result.outer().empty())
		{
			return{};
		}

		return detail::ToPolygon(result);
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	bool Polygon::PolygonDetail::append(const RectF& other)
	{
		const boost::geometry::model::box<Vec2> box{ other.pos, other.br() };

		Array<CwOpenPolygon> results;

		boost::geometry::union_(toCwOpenPolygon(), box, results);

		if (results.size() != 1)
		{
			return false;
		}

		auto& outer = results[0].outer();

		if ((2 < outer.size())
			&& (outer.front() == outer.back()))
		{
			outer.pop_back();
		}

		Array<Array<Vec2>> holes;

		const auto& result = results[0];

		if (const size_t num_holes = result.inners().size())
		{
			holes.resize(num_holes);

			for (size_t i = 0; i < num_holes; ++i)
			{
				const auto& resultHole = result.inners()[i];
				holes[i].assign(resultHole.begin(), resultHole.end());
			}
		}

		*this = PolygonDetail{ outer, holes, SkipValidation::Yes };

		return true;
	}

	bool Polygon::PolygonDetail::append(const Polygon& other)
	{
		Array<CwOpenPolygon> results;

		boost::geometry::union_(ToCWOpenPolygon(m_polygon), ToCWOpenPolygon(other._detail()->m_polygon), results);

		if (results.size() != 1)
		{
			return false;
		}

		auto& outer = results[0].outer();

		if ((2 < outer.size())
			&& (outer.front() == outer.back()))
		{
			outer.pop_back();
		}

		Array<Array<Vec2>> holes;

		const auto& result = results[0];

		if (const size_t num_holes = result.inners().size())
		{
			holes.resize(num_holes);

			for (size_t i = 0; i < num_holes; ++i)
			{
				const auto& resultHole = result.inners()[i];
				holes[i].assign(resultHole.begin(), resultHole.end());
			}
		}

		*this = PolygonDetail{ outer, holes, SkipValidation::Yes };

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::draw(const Optional<Float2>& offset, const ColorF& color) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(m_vertices, m_indices, offset, color.toFloat4());
	}

	void Polygon::PolygonDetail::draw(const Optional<Float2>& offset, const PatternParameters& pattern) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addPolygon(m_vertices, m_indices, offset, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawTransformed
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::drawTransformed(const double s, const double c, const Vec2& pos, const ColorF& color) const
	{
		SIV3D_ENGINE(Renderer2D)->addPolygonTransformed(m_vertices, m_indices,
			static_cast<float>(s), static_cast<float>(c), Float2{ pos }, color.toFloat4());
	}

	void Polygon::PolygonDetail::drawTransformed(const double s, const  double c, const Vec2& pos, const PatternParameters& pattern) const
	{
		SIV3D_ENGINE(Renderer2D)->addPolygonTransformed(m_vertices, m_indices,
			static_cast<float>(s), static_cast<float>(c), Float2{ pos }, pattern);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFrame
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::drawFrame(const Optional<Float2>& offset, const double thickness, const ColorF& color) const
	{
		if (not m_polygon.outer)
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			m_polygon.outer, offset,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			color.toFloat4());

		for (const auto& inner : m_polygon.inners)
		{
			SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
				inner, offset,
				Abs(static_cast<float>(thickness)), false,
				CloseRing::Yes,
				color.toFloat4());
		}
	}

	void Polygon::PolygonDetail::drawFrame(const Optional<Float2>& offset, const double thickness, const PatternParameters& pattern) const
	{
		if (not m_polygon.outer)
		{
			return;
		}

		SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
			m_polygon.outer, offset,
			Abs(static_cast<float>(thickness)), false,
			CloseRing::Yes,
			pattern);

		for (const auto& inner : m_polygon.inners)
		{
			SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
				inner, offset,
				Abs(static_cast<float>(thickness)), false,
				CloseRing::Yes,
				pattern);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawWireframe
	//
	////////////////////////////////////////////////////////////////

	void Polygon::PolygonDetail::drawWireframe(const Optional<Float2>& offset, const double thickness, const ColorF& color) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		const Float2* pVertex = m_vertices.data();
		const TriangleIndex* pIndex = m_indices.data();
		const TriangleIndex* const pIndexEnd = (pIndex + m_indices.size());
		const Float4 colorF = color.toFloat4();

		while (pIndex != pIndexEnd)
		{
			const Vec2 points[3] = { pVertex[pIndex->i0], pVertex[pIndex->i1], pVertex[pIndex->i2] };

			SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
				points, offset,
				Abs(static_cast<float>(thickness)), false,
				CloseRing::Yes,
				colorF);

			++pIndex;
		}
	}

	void Polygon::PolygonDetail::drawWireframe(const Optional<Float2>& offset, const double thickness, const PatternParameters& pattern) const
	{
		if (m_indices.isEmpty())
		{
			return;
		}

		const Float2* pVertex = m_vertices.data();
		const TriangleIndex* pIndex = m_indices.data();
		const TriangleIndex* const pIndexEnd = (pIndex + m_indices.size());

		while (pIndex != pIndexEnd)
		{
			const Vec2 points[3] = { pVertex[pIndex->i0], pVertex[pIndex->i1], pVertex[pIndex->i2] };

			SIV3D_ENGINE(Renderer2D)->addLineString(LineCap::Square, LineCap::Square,
				points, offset,
				Abs(static_cast<float>(thickness)), false,
				CloseRing::Yes,
				pattern);

			++pIndex;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	toCwOpenPolygon
	//
	////////////////////////////////////////////////////////////////

	CwOpenPolygon Polygon::PolygonDetail::toCwOpenPolygon() const
	{
		return MakeCWOpenPolygon(m_polygon.outer, m_polygon.inners);
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

		boost::geometry::correct(polygon);
		
		CwOpenMultiPolygon solvedPolygons;
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

	////////////////////////////////////////////////////////////////
	//
	//	simplified
	//
	////////////////////////////////////////////////////////////////

	LineString LineString::simplified(const double maxDistance, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return *this;
		}

		LineString result;

		if (closeRing && (front() != back()))
		{
			LineString input(begin(), end());
			input.push_back(input.front());

			boost::geometry::simplify(input, result, maxDistance);
			result.pop_back();
		}
		else
		{
			boost::geometry::simplify(*this, result, maxDistance);
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	densified
	//
	////////////////////////////////////////////////////////////////

	LineString LineString::densified(const double maxSegmentLength, const CloseRing closeRing) const
	{
		if (size() < 2)
		{
			return *this;
		}

		LineString result;

		if (closeRing && (front() != back()))
		{
			LineString input(begin(), end());
			input.push_back(input.front());

			boost::geometry::densify(input, result, maxSegmentLength);
			result.pop_back();
		}
		else
		{
			boost::geometry::densify(*this, result, maxSegmentLength);
		}

		return result;
	}
}
