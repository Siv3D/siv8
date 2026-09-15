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

# include <Siv3D/LineCap.hpp>
# include <Siv3D/LineString.hpp>
# include <Siv3D/Geometry2D/BoundingRect.hpp>
# include <Siv3D/Geometry2D/ConvexHull.hpp>
# include <Siv3D/Pattern/PatternParameters.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "PolygonDetail.hpp"
# include "GeometryCommon.hpp"
# include "Triangulate.hpp"
# include "PolygonParser.hpp"

namespace s3d
{
	namespace
	{
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

		struct PolygonIntegral
		{
			double area2x = 0.0;
			Vec2 moment6x{ 0, 0 };
		};

		// 各輪郭の先頭を原点にして積分する。穴の小さな面積も、外周からの距離に依存せず計算する。
		template <bool ComputeCentroid>
		[[nodiscard]]
		static PolygonIntegral IntegrateRing(const std::span<const Vec2> ring) noexcept
		{
			if (ring.size() < 3)
			{
				return {};
			}

			const Vec2 reference = ring.front();
			Vec2 a = (ring[1] - reference);
			KahanSummation<double> area2x, momentX, momentY;

			for (size_t i = 2; i < ring.size(); ++i)
			{
				const Vec2 b = (ring[i] - reference);
				const double cross = a.cross(b);
				area2x += cross;

				if constexpr (ComputeCentroid)
				{
					momentX += ((a.x + b.x) * cross);
					momentY += ((a.y + b.y) * cross);
				}

				a = b;
			}

			return { area2x.value(), { momentX.value(), momentY.value() } };
		}

		template <bool ComputeCentroid, class InnerRings>
		[[nodiscard]]
		static PolygonIntegral IntegratePolygon(const std::span<const Vec2> outer, const InnerRings& holes) noexcept
		{
			const auto outerIntegral = IntegrateRing<ComputeCentroid>(outer);
			KahanSummation<double> area2x{ outerIntegral.area2x };
			KahanSummation<double> momentX{ outerIntegral.moment6x.x };
			KahanSummation<double> momentY{ outerIntegral.moment6x.y };

			for (const auto& hole : holes)
			{
				const auto integral = IntegrateRing<ComputeCentroid>(hole);
				area2x += integral.area2x;

				if constexpr (ComputeCentroid)
				{
					// 穴のモーメントを外周と同じ原点へ移す。穴の符号は外周と逆になる。
					const Vec2 offset = (hole.front() - outer.front());
					momentX += (integral.moment6x.x + (3.0 * integral.area2x * offset.x));
					momentY += (integral.moment6x.y + (3.0 * integral.area2x * offset.y));
				}
			}

			return { area2x.value(), { momentX.value(), momentY.value() } };
		}

		template <class InnerRings>
		[[nodiscard]]
		static double PolygonArea(const std::span<const Vec2> outer, const InnerRings& holes) noexcept
		{
			return (Abs(IntegratePolygon<false>(outer, holes).area2x) * 0.5);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
	{
		if (initialize(outer, std::move(holes), skipValidation))
		{
			m_boundingRect = Geometry2D::BoundingRect(m_polygon.outer);
		}
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		if (initialize(outer, std::move(holes), skipValidation))
		{
			m_boundingRect = boundingRect;
		}
	}

	bool Polygon::PolygonDetail::initialize(const std::span<const Vec2> outer, Array<Array<Vec2>> holes, const SkipValidation skipValidation)
	{
		auto polygon = MakePolygonData(outer, std::move(holes));

		if (not skipValidation)
		{
			if (detail::ValidatePolygon(polygon.outer, polygon.inners) != PolygonFailureType::Ok)
			{
				return false;
			}
		}

		if (not Triangulate(polygon.outer, polygon.inners, m_indices))
		{
			return false;
		}

		m_vertices = MakeVertices(polygon.outer, polygon.inners);
		m_polygon = std::move(polygon);
		return true;
	}

	Polygon::PolygonDetail::PolygonDetail(const std::span<const Vec2> outer, Array<TriangleIndex> indices, const RectF& boundingRect, const SkipValidation skipValidation)
	{
		auto polygon = MakePolygonData(outer, {});

		if (not skipValidation)
		{
			if (detail::ValidatePolygon(polygon.outer, polygon.inners) != PolygonFailureType::Ok)
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
			if (detail::ValidatePolygon(polygon.outer, polygon.inners) != PolygonFailureType::Ok)
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
		return PolygonArea(m_polygon.outer, m_polygon.inners);
	}

	////////////////////////////////////////////////////////////////
	//
	//	perimeter
	//
	////////////////////////////////////////////////////////////////

	double Polygon::PolygonDetail::perimeter() const noexcept
	{
		KahanSummation<double> result;

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

		return result.value();
	}

	////////////////////////////////////////////////////////////////
	//
	//	centroid
	//
	////////////////////////////////////////////////////////////////

	Optional<PolygonCentroidResult> Polygon::PolygonDetail::centroid() const
	{
		if (m_polygon.outer.empty())
		{
			return none;
		}

		const auto integral = IntegratePolygon<true>(m_polygon.outer, m_polygon.inners);
		if (integral.area2x == 0.0)
		{
			return none;
		}

		return PolygonCentroidResult{
			.centroid = (m_polygon.outer.front() + (integral.moment6x / (3.0 * integral.area2x))),
			.area = (Abs(integral.area2x) * 0.5)
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeConvexHull
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::computeConvexHull() const
	{
		return Geometry2D::ConvexHull(m_polygon.outer);
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeMiterBufferPolygon
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::computeMiterBufferPolygon(const double distance) const
	{
		boost::geometry::model::multi_polygon<CwOpenPolygon> multiPolygon;

		boost::geometry::buffer(detail::ToCwOpenPolygon(m_polygon.outer, m_polygon.inners), multiPolygon,
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

		boost::geometry::buffer(detail::ToCwOpenPolygon(m_polygon.outer, m_polygon.inners), multiPolygon,
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
		boost::geometry::simplify(detail::ToCwOpenPolygon(m_polygon.outer, m_polygon.inners), result, maxDistance);

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

		boost::geometry::union_(detail::ToCwOpenPolygon(m_polygon.outer, m_polygon.inners), box, results);

		if (results.size() != 1)
		{
			return false;
		}

		const auto& result = results.front();
		*this = PolygonDetail{ detail::OpenRingView(result.outer()), detail::CopyPolygonHoles(result), SkipValidation::Yes };

		return true;
	}

	bool Polygon::PolygonDetail::append(const Polygon& other)
	{
		Array<CwOpenPolygon> results;

		boost::geometry::union_(detail::ToCwOpenPolygon(m_polygon.outer, m_polygon.inners), detail::ToCwOpenPolygon(other), results);

		if (results.size() != 1)
		{
			return false;
		}

		const auto& result = results.front();
		*this = PolygonDetail{ detail::OpenRingView(result.outer()), detail::CopyPolygonHoles(result), SkipValidation::Yes };

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
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	Optional<Polygon> Polygon::PolygonDetail::Parse(const StringView s)
	{
		return PolygonParser{ s }.parse();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Validate
	//
	////////////////////////////////////////////////////////////////

	PolygonFailureType Polygon::PolygonDetail::Validate(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		return detail::ValidatePolygon(outer, holes);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Correct
	//
	////////////////////////////////////////////////////////////////

	Array<Polygon> Polygon::PolygonDetail::Correct(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		const auto corrected = detail::CorrectPolygonRings(outer, holes);
		Array<Polygon> results{ Arg::reserve = corrected.size() };

		for (const auto& polygon : corrected)
		{
			if (Polygon result = detail::ToPolygon(polygon))
			{
				results.push_back(std::move(result));
			}
		}

		return results;
	}

	////////////////////////////////////////////////////////////////
	//
	//	CorrectOne
	//
	////////////////////////////////////////////////////////////////

	Polygon Polygon::PolygonDetail::CorrectOne(const std::span<const Vec2> outer, const Array<Array<Vec2>>& holes)
	{
		auto corrected = detail::CorrectPolygonRings(outer, holes);

		while (1 < corrected.size())
		{
			size_t largestIndex = 0;
			double largestArea = PolygonArea(corrected.front().outer(), corrected.front().inners());

			for (size_t i = 1; i < corrected.size(); ++i)
			{
				const double area = PolygonArea(corrected[i].outer(), corrected[i].inners());
				if (largestArea < area)
				{
					largestIndex = i;
					largestArea = area;
				}
			}

			// Polygon::area() と同じ積分で比較し、選択した成分だけを三角形化する。
			if (Polygon result = detail::ToPolygon(corrected[largestIndex]))
			{
				return result;
			}

			// 三角形化できない成分は Correct() の結果にも含まれない。
			corrected.erase(corrected.begin() + largestIndex);
		}

		return (corrected.empty() ? Polygon{} : detail::ToPolygon(corrected.front()));
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
