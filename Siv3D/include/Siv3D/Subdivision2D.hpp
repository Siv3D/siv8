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

# pragma once
# include "Common.hpp"
# include "Array.hpp"
# include "PointVector.hpp"
# include "2DShapes.hpp"

namespace s3d
{
	class Subdivision2D
	{
	public:
		
		using EdgeID = s3d::int32;

		using VertexID = s3d::int32;

		enum class PointLocation : int32
		{
			/// @brief エラー
			Error = -2,

			/// @brief Subdivision bounding rect の外
			OutsideRect = -1,

			/// @brief Facet 内
			Inside = 0,

			/// @brief 頂点の重複
			Vertex = 1,

			/// @brief Edge 上
			OnEdge = 2,
		};

		enum class EdgeType : int32
		{
			NextAroundOrgin = 0x00,

			NextAroundDst = 0x22,

			PreviousAroundOrigin = 0x11,

			PreviousAroundDst = 0x33,

			NextAroundLeft = 0x13,

			NextAroundRight = 0x31,

			PreviousAroundLeft = 0x20,

			PreviousAroundRight = 0x02
		};

		struct VoronoiFacet
		{
			Array<Vec2> points;

			Vec2 center;
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Subdivision2D() = default;

		[[nodiscard]]
		explicit Subdivision2D(const RectF& rect);

		[[nodiscard]]
		explicit Subdivision2D(const RectF& rect, const Array<Vec2>& points);

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	initDelaunay
		//
		////////////////////////////////////////////////////////////////

		void initDelaunay(const RectF& rect);

		////////////////////////////////////////////////////////////////
		//
		//	addPoint
		//
		////////////////////////////////////////////////////////////////

		VertexID addPoint(const Vec2& point);

		////////////////////////////////////////////////////////////////
		//
		//	addPoints
		//
		////////////////////////////////////////////////////////////////

		void addPoints(const Array<Vec2>& points);

		////////////////////////////////////////////////////////////////
		//
		//	findNearest
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Optional<VertexID> findNearest(const Vec2& point, Vec2* nearestPt = nullptr);

		////////////////////////////////////////////////////////////////
		//
		//	calculateEdges
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<Line> calculateEdges() const;

		void calculateEdges(Array<Line>& edgeList) const;

		////////////////////////////////////////////////////////////////
		//
		//	calculateLeadingEdges
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<EdgeID> calculateLeadingEdges() const;

		void calculateLeadingEdges(Array<EdgeID>& leadingEdgeList) const;

		////////////////////////////////////////////////////////////////
		//
		//	calculateTriangles
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<Triangle> calculateTriangles() const;

		void calculateTriangles(Array<Triangle>& triangleList) const;

		////////////////////////////////////////////////////////////////
		//
		//	calculateVoronoiFacets
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<VoronoiFacet> calculateVoronoiFacets();

		void calculateVoronoiFacets(Array<VoronoiFacet>& facetList);

		void calculateVoronoiFacets(const Array<VertexID>& indices, Array<VoronoiFacet>& facetList);

		////////////////////////////////////////////////////////////////
		//
		//	getVertex
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Vec2 getVertex(VertexID vertex, EdgeID* firstEdge = nullptr) const;

		////////////////////////////////////////////////////////////////
		//
		//	getEdge
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		EdgeID getEdge(EdgeID edge, EdgeType nextEdgeType) const;

		////////////////////////////////////////////////////////////////
		//
		//	nextEdge
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		EdgeID nextEdge(EdgeID edge) const;

		////////////////////////////////////////////////////////////////
		//
		//	rotateEdge
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		EdgeID rotateEdge(EdgeID edge, int32 rotate) const;

		////////////////////////////////////////////////////////////////
		//
		//	rotateEdge
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		EdgeID symEdge(EdgeID edge) const;

		////////////////////////////////////////////////////////////////
		//
		//	edgeBegin
		//
		////////////////////////////////////////////////////////////////

		VertexID edgeBegin(EdgeID edge, Vec2* beginPos = nullptr) const;

		////////////////////////////////////////////////////////////////
		//
		//	edgeEnd
		//
		////////////////////////////////////////////////////////////////

		VertexID edgeEnd(EdgeID edge, Vec2* endPos = nullptr) const;

	private:

		struct Vertex
		{
			EdgeID firstEdge = 0;

			int32 type = -1;

			Vec2 pt{ 0, 0 };

			Vertex() = default;

			constexpr Vertex(const Vec2& _pt, bool _isvirtual, EdgeID _firstEdge = 0) noexcept;

			[[nodiscard]]
			constexpr bool isVirtual() const noexcept;

			[[nodiscard]]
			constexpr bool isFree() const noexcept;
		};

		struct QuadEdge
		{
			EdgeID next[4] = {};

			VertexID pt[4] = {};

			QuadEdge() = default;

			explicit constexpr QuadEdge(EdgeID edgeidx);

			[[nodiscard]]
			constexpr bool isFree() const noexcept;
		};

		struct Internal
		{
			Array<Vertex> vertices;

			Array<QuadEdge> qEdges;

			size_t addedPoints = 0;

			int32 freeQEdge = 0;

			VertexID freePoint = 0;

			bool validGeometry = false;

			EdgeID recentEdge = 0;

			Vec2 topLeft{ 0, 0 };

			Vec2 bottomRight{ 0, 0 };

			[[nodiscard]]
			constexpr bool isEmpty() const noexcept;

			[[nodiscard]]
			constexpr RectF rect() const noexcept;

			void clear() noexcept;

		} m_internal;

		PointLocation locate(const Vec2& pt, EdgeID& _edge, VertexID& _vertex);

		EdgeID newEdge();

		void deleteEdge(EdgeID edge);

		VertexID newPoint(const Vec2& pt, bool isvirtual, EdgeID firstEdge = 0);

		void deletePoint(VertexID vidx);

		void setEdgePoints(EdgeID edge, VertexID orgPt, VertexID dstPt);

		void splice(EdgeID edgeA, EdgeID edgeB);

		EdgeID connectEdges(EdgeID edgeA, EdgeID edgeB);

		void swapEdges(EdgeID edge);

		int32 isRightOf(const Vec2& pt, EdgeID edge) const;

		////////////////////////////////////////////////////////////////
		//
		//	clearVoronoi
		//
		////////////////////////////////////////////////////////////////

		void clearVoronoi();

		////////////////////////////////////////////////////////////////
		//
		//	calcVoronoi
		//
		////////////////////////////////////////////////////////////////

		void calcVoronoi();
	};
}

# include "detail/Subdivision2D.ipp"
