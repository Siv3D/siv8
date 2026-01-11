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

# include <cfloat>
# include <Siv3D/Number.hpp>
# include <Siv3D/Subdivision2D.hpp>
# include <Siv3D/Error.hpp>

//////////////////////////////////////////////////////////////////////////////////////////
//
//	https://github.com/opencv/opencv/blob/master/modules/imgproc/src/subdivision2d.cpp
//
/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr Vec2 ComputeVoronoiPoint(const Vec2& org0, const Vec2& dst0, const Vec2& org1, const Vec2& dst1) noexcept
		{
			const double a0 = (dst0.x - org0.x);
			const double b0 = (dst0.y - org0.y);
			const double c0 = (-0.5 * (a0 * (dst0.x + org0.x) + b0 * (dst0.y + org0.y)));

			const double a1 = (dst1.x - org1.x);
			const double b1 = (dst1.y - org1.y);
			const double c1 = (-0.5 * (a1 * (dst1.x + org1.x) + b1 * (dst1.y + org1.y)));

			if (double det = (a0 * b1 - a1 * b0); det != 0)
			{
				det = (1.0 / det);
				return{ ((b0 * c1 - b1 * c0) * det), ((a1 * c0 - a0 * c1) * det) };
			}

			return{ Largest<double>, Largest<double> };
		}

		[[nodiscard]]
		static constexpr double TriangleArea(const Vec2& a, const Vec2& b, const Vec2& c) noexcept
		{
			return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
		}

		[[nodiscard]]
		static constexpr int32 IsPtInCircle3(const Vec2& pt, const Vec2& a, const Vec2& b, const Vec2& c) noexcept
		{
			const double eps = (DBL_EPSILON * 0.125);
			double val = ((a.x * a.x + a.y * a.y) * TriangleArea(b, c, pt));
			val -= ((b.x * b.x + b.y * b.y) * TriangleArea(a, c, pt));
			val += ((c.x * c.x + c.y * c.y) * TriangleArea(a, b, pt));
			val -= ((pt.x * pt.x + pt.y * pt.y) * TriangleArea(a, b, c));

			return ((eps < val) ? 1 : (val < -eps) ? -1 : 0);
		}

		[[nodiscard]]
		static constexpr int32 IsRightOf2(const Vec2& pt, const Vec2& org, const Vec2& diff) noexcept
		{
			const double cwArea = ((org.x - pt.x) * diff.y - (org.y - pt.y) * diff.x);
			return ((0 < cwArea) - (cwArea < 0));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::Subdivision2D::Subdivision2D(const RectF& rect)
		: Subdivision2D{ rect, {} } {}

	Subdivision2D::Subdivision2D(const RectF& rect, const Array<Vec2>& points)
	{
		reset(rect);

		if (points)
		{
			addPoints(points);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool Subdivision2D::isEmpty() const noexcept
	{
		return m_internal.isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::operator bool() const noexcept
	{
		return (not m_internal.isEmpty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::reset(const RectF& rect)
	{
		const double bigCoord = (6.0 * Max(rect.w, rect.h));
		const double rx = rect.x;
		const double ry = rect.y;

		m_internal.clear();

		m_internal.topLeft = rect.tl();
		m_internal.bottomRight = rect.br();

		const Vec2 ppA{ (rx + bigCoord), ry };
		const Vec2 ppB{ rx, (ry + bigCoord) };
		const Vec2 ppC{ (rx - bigCoord), (ry - bigCoord) };

		m_internal.vertices.emplace_back();
		m_internal.qEdges.emplace_back();

		const VertexID pA = newPoint(ppA, false);
		const VertexID pB = newPoint(ppB, false);
		const VertexID pC = newPoint(ppC, false);

		const EdgeID edgeAB = newEdge();
		const EdgeID edgeBC = newEdge();
		const EdgeID edgeCA = newEdge();

		setEdgePoints(edgeAB, pA, pB);
		setEdgePoints(edgeBC, pB, pC);
		setEdgePoints(edgeCA, pC, pA);

		splice(edgeAB, symEdge(edgeCA));
		splice(edgeBC, symEdge(edgeAB));
		splice(edgeCA, symEdge(edgeBC));

		m_internal.recentEdge = edgeAB;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPoint
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::VertexID Subdivision2D::addPoint(const Vec2& point)
	{
		// If a point with the same coordinates exists already, no new point is added.
		// returns the ID of the point.

		VertexID curr_point = 0;
		EdgeID curr_edge = 0, deleted_edge = 0;
		const PointLocation location = locate(point, curr_edge, curr_point);

		if (location == PointLocation::Error)
		{
			throw Error{ "Subdivision2D::addPoint(): Error" };
		}
		else if (location == PointLocation::OutsideRect)
		{
			throw Error{ "Subdivision2D::addPoint(): The point is outside of the triangulation specified rect" };
		}
		else if (location == PointLocation::OnVertex)
		{
			return curr_point;
		}
		else if (location == PointLocation::OnEdge)
		{
			deleted_edge = curr_edge;
			m_internal.recentEdge = curr_edge = getEdge(curr_edge, EdgeType::PreviousAroundOrigin);
			deleteEdge(deleted_edge);
		}
		else if (location == PointLocation::Inside)
		{
			;
		}
		else
		{
			throw Error{ fmt::format("Subdivision2D::addPoint(): Invalid location = {}", FromEnum(location)) };
		}

		assert(curr_edge != 0);
		m_internal.validGeometry = false;

		curr_point = newPoint(point, false);
		EdgeID base_edge = newEdge();
		const VertexID first_point = edgeBegin(curr_edge);
		setEdgePoints(base_edge, first_point, curr_point);
		splice(base_edge, curr_edge);

		do
		{
			base_edge = connectEdges(curr_edge, symEdge(base_edge));
			curr_edge = getEdge(base_edge, EdgeType::PreviousAroundOrigin);
		} while (edgeEnd(curr_edge) != first_point);

		curr_edge = getEdge(base_edge, EdgeType::PreviousAroundOrigin);

		const int32 max_edges = static_cast<int32>(m_internal.qEdges.size() * 4);

		for (int32 i = 0; i < max_edges; ++i)
		{
			VertexID temp_dst = 0, curr_org = 0, curr_dst = 0;
			const EdgeID temp_edge = getEdge(curr_edge, EdgeType::PreviousAroundOrigin);

			temp_dst = edgeEnd(temp_edge);
			curr_org = edgeBegin(curr_edge);
			curr_dst = edgeEnd(curr_edge);

			if ((0 < isRightOf(m_internal.vertices[temp_dst].pt, curr_edge)) &&
				(IsPtInCircle3(m_internal.vertices[curr_org].pt, m_internal.vertices[temp_dst].pt, m_internal.vertices[curr_dst].pt, m_internal.vertices[curr_point].pt) < 0))
			{
				swapEdges(curr_edge);
				curr_edge = getEdge(curr_edge, EdgeType::PreviousAroundOrigin);
			}
			else if (curr_org == first_point)
			{
				break;
			}
			else
			{
				curr_edge = getEdge(nextEdge(curr_edge), EdgeType::PreviousAroundLeft);
			}
		}

		++m_internal.addedPoints;

		return curr_point;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPoints
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::addPoints(const Array<Vec2>& points)
	{
		for (const Vec2& point : points)
		{
			addPoint(point);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	findNearestVertex
	//
	////////////////////////////////////////////////////////////////

	Optional<Subdivision2D::NearestVertex> Subdivision2D::findNearestVertex(const Vec2& point)
	{
		if (isEmpty() || (not m_internal.rect().contains(point)))
		{
			return none;
		}

		if (not m_internal.validGeometry)
		{
			calcVoronoi();
		}

		VertexID vertex = 0;
		EdgeID edge = 0;
		const PointLocation loc = locate(point, edge, vertex);

		if ((loc != PointLocation::OnEdge) && (loc != PointLocation::Inside))
		{
			return NearestVertex{ vertex, m_internal.vertices[vertex].pt };
		}

		vertex = 0;

		Vec2 start;
		edgeBegin(edge, &start);
		const Vec2 diff = (point - start);

		edge = rotateEdge(edge, 1);

		const int32 total = static_cast<int32>(m_internal.vertices.size());

		for (int32 i = 0; i < total; ++i)
		{
			Vec2 t;

			for (;;)
			{
				assert(0 < edgeEnd(edge, &t));

				if (0 <= IsRightOf2(t, start, diff))
				{
					break;
				}

				edge = getEdge(edge, EdgeType::NextAroundLeft);
			}

			for (;;)
			{
				assert(0 < edgeBegin(edge, &t));

				if (IsRightOf2(t, start, diff) < 0)
				{
					break;
				}

				edge = getEdge(edge, EdgeType::PreviousAroundLeft);
			}

			Vec2 tempDiff;
			edgeEnd(edge, &tempDiff);
			edgeBegin(edge, &t);
			tempDiff -= t;

			if (0 <= IsRightOf2(point, t, tempDiff))
			{
				vertex = edgeBegin(rotateEdge(edge, 3));
				break;
			}

			edge = symEdge(edge);
		}

		if (0 < vertex)
		{
			return NearestVertex{ vertex, m_internal.vertices[vertex].pt };
		}
	
		return none;
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeDelaunayEdges
	//
	////////////////////////////////////////////////////////////////

	Array<Line> Subdivision2D::computeDelaunayEdges() const
	{
		Array<Line> edges;
		computeDelaunayEdges(edges);
		return edges;
	}

	void Subdivision2D::computeDelaunayEdges(Array<Line>& edgeList) const
	{
		edgeList.clear();

		for (size_t i = 4; i < m_internal.qEdges.size(); ++i)
		{
			if (m_internal.qEdges[i].isFree())
			{
				continue;
			}

			if ((0 < m_internal.qEdges[i].pt[0]) && (0 < m_internal.qEdges[i].pt[2]))
			{
				const Vec2& org = m_internal.vertices[m_internal.qEdges[i].pt[0]].pt;
				const Vec2& dst = m_internal.vertices[m_internal.qEdges[i].pt[2]].pt;
				edgeList.emplace_back(org, dst);
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeDelaunayTriangles
	//
	////////////////////////////////////////////////////////////////

	Array<Triangle> Subdivision2D::computeDelaunayTriangles() const
	{
		Array<Triangle> triangleList;
		computeDelaunayTriangles(triangleList);
		return triangleList;
	}

	void Subdivision2D::computeDelaunayTriangles(Array<Triangle>& triangleList) const
	{
		triangleList.clear();
		
		const int32 total = static_cast<int32>(m_internal.qEdges.size() * 4);
		std::vector<bool> edgemask(total);

		for (int32 i = 4; i < total; i += 2)
		{
			if (edgemask[i])
			{
				continue;
			}

			const RectF rect = m_internal.rect();
			Vec2 a, b, c;

			const EdgeID edge_a = i;
			edgeBegin(edge_a, &a);
			if (not rect.contains(a))
			{
				continue;
			}
			
			const EdgeID edge_b = getEdge(edge_a, EdgeType::NextAroundLeft);
			edgeBegin(edge_b, &b);
			if (not rect.contains(b))
			{
				continue;
			}
			
			const EdgeID edge_c = getEdge(edge_b, EdgeType::NextAroundLeft);
			edgeBegin(edge_c, &c);
			if (not rect.contains(c))
			{
				continue;
			}
			
			edgemask[edge_a] = true;
			edgemask[edge_b] = true;
			edgemask[edge_c] = true;
			triangleList.emplace_back(a, b, c);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeVoronoiCells
	//
	////////////////////////////////////////////////////////////////

	Array<VoronoiCell> Subdivision2D::computeVoronoiCells()
	{
		Array<VoronoiCell> cellList;
		computeVoronoiCells({}, cellList);
		return cellList;
	}

	void Subdivision2D::computeVoronoiCells(Array<VoronoiCell>& cellList)
	{
		computeVoronoiCells({}, cellList);
	}

	void Subdivision2D::computeVoronoiCells(const Array<VertexID>& vertexIDs, Array<VoronoiCell>& cellList)
	{
		calcVoronoi();
		cellList.clear();

		size_t i, total;
		if (vertexIDs.isEmpty())
		{
			i = 4;
			total = m_internal.vertices.size();
		}
		else
		{
			i = 0;
			total = vertexIDs.size();
		}

		Array<Vec2> buf;

		for (; i < total; i++)
		{
			const int32 k = (vertexIDs.empty() ? static_cast<int32>(i) : vertexIDs[i]);

			if (m_internal.vertices[k].isFree() || m_internal.vertices[k].isVirtual())
			{
				continue;
			}

			const EdgeID edge = rotateEdge(m_internal.vertices[k].firstEdge, 1);
			EdgeID t = edge;

			// gather points
			buf.clear();
			do
			{
				buf.push_back(m_internal.vertices[edgeBegin(t)].pt);
				t = getEdge(t, EdgeType::NextAroundLeft);
			} while (t != edge);

			cellList.emplace_back(std::move(buf), m_internal.vertices[k].pt);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeLeadingEdges
	//
	////////////////////////////////////////////////////////////////

	Array<Subdivision2D::EdgeID> Subdivision2D::computeLeadingEdges() const
	{
		Array<EdgeID> leadingEdgeList;
		computeLeadingEdges(leadingEdgeList);
		return leadingEdgeList;
	}

	void Subdivision2D::computeLeadingEdges(Array<EdgeID>& leadingEdgeList) const
	{
		leadingEdgeList.clear();

		const int32 total = static_cast<int32>(m_internal.qEdges.size() * 4);
		std::vector<bool> edgemask(total);

		for (int32 i = 4; i < total; i += 2)
		{
			if (edgemask[i])
			{
				continue;
			}

			EdgeID edge = i;
			edgemask[edge] = true;
			edge = getEdge(edge, EdgeType::NextAroundLeft);
			edgemask[edge] = true;
			edge = getEdge(edge, EdgeType::NextAroundLeft);
			edgemask[edge] = true;
			leadingEdgeList.push_back(i);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getVertex
	//
	////////////////////////////////////////////////////////////////

	Vec2 Subdivision2D::getVertex(const VertexID vertex, EdgeID* firstEdge) const
	{
		assert((size_t)vertex < m_internal.vertices.size());

		if (firstEdge)
		{
			*firstEdge = m_internal.vertices[vertex].firstEdge;
		}

		return m_internal.vertices[vertex].pt;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getEdge
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::EdgeID Subdivision2D::getEdge(EdgeID edge, const EdgeType nextEdgeType) const
	{
		assert((size_t)(edge >> 2) < m_internal.qEdges.size());
		edge = m_internal.qEdges[edge >> 2].next[(edge + FromEnum(nextEdgeType)) & 3];
		return (edge & ~3) + ((edge + (FromEnum(nextEdgeType) >> 4)) & 3);
	}

	////////////////////////////////////////////////////////////////
	//
	//	nextEdge
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::EdgeID Subdivision2D::nextEdge(const EdgeID edge) const
	{
		assert((size_t)(edge >> 2) < m_internal.qEdges.size());
		return m_internal.qEdges[edge >> 2].next[edge & 3];
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateEdge
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::EdgeID Subdivision2D::rotateEdge(const EdgeID edge, const int32 rotate) const
	{
		return ((edge & ~3) + ((edge + rotate) & 3));
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotateEdge
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::EdgeID Subdivision2D::symEdge(const EdgeID edge) const
	{
		return (edge ^ 2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	edgeBegin
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::VertexID Subdivision2D::edgeBegin(const EdgeID edge, Vec2* beginPos) const
	{
		assert((size_t)(edge >> 2) < m_internal.qEdges.size());
		const VertexID vidx = m_internal.qEdges[edge >> 2].pt[edge & 3];

		if (beginPos)
		{
			assert((size_t)vidx < m_internal.vertices.size());
			*beginPos = m_internal.vertices[vidx].pt;
		}

		return vidx;
	}

	////////////////////////////////////////////////////////////////
	//
	//	edgeEnd
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::VertexID Subdivision2D::edgeEnd(const EdgeID edge, Vec2* endPos) const
	{
		assert((size_t)(edge >> 2) < m_internal.qEdges.size());
		const VertexID vidx = m_internal.qEdges[edge >> 2].pt[(edge + 2) & 3];

		if (endPos)
		{
			assert((size_t)vidx < m_internal.vertices.size());
			*endPos = m_internal.vertices[vidx].pt;
		}

		return vidx;
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::Internal::clear() noexcept
	{
		vertices.clear();
		qEdges.clear();

		recentEdge = 0;
		validGeometry = false;

		freeQEdge = 0;
		freePoint = 0;
		addedPoints = 0;
		
		topLeft = Vec2::Zero();
		bottomRight = Vec2::Zero();
	}

	////////////////////////////////////////////////////////////////
	//
	//	locate
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::PointLocation Subdivision2D::locate(const Vec2& pt, EdgeID& _edge, VertexID& _vertex)
	{
		VertexID vertex = 0;

		int32 maxEdges = static_cast<int32>(m_internal.qEdges.size() * 4);

		if (m_internal.qEdges.size() < 4)
		{
			throw Error{ "Subdivision is empty" };
		}

		if ((pt.x < m_internal.topLeft.x) || (pt.y < m_internal.topLeft.y) || (m_internal.bottomRight.x <= pt.x) || (m_internal.bottomRight.y <= pt.y))
		{
			throw Error{ "Subdivision: point is out of bounds" };
		}

		EdgeID edge = m_internal.recentEdge;
		assert(0 < edge);

		PointLocation location = PointLocation::Error;

		int32 right_of_curr = isRightOf(pt, edge);
		if (0 < right_of_curr)
		{
			edge = symEdge(edge);
			right_of_curr = -right_of_curr;
		}

		for (int32 i = 0; i < maxEdges; ++i)
		{
			const EdgeID onext_edge = nextEdge(edge);
			const EdgeID dprev_edge = getEdge(edge, EdgeType::PreviousAroundDst);

			const int32 right_of_onext = isRightOf(pt, onext_edge);
			const int32 right_of_dprev = isRightOf(pt, dprev_edge);

			if (0 < right_of_dprev)
			{
				if ((0 < right_of_onext) || ((right_of_onext == 0) && (right_of_curr == 0)))
				{
					location = PointLocation::Inside;
					break;
				}
				else
				{
					right_of_curr = right_of_onext;
					edge = onext_edge;
				}
			}
			else
			{
				if (0 < right_of_onext)
				{
					if ((right_of_dprev == 0) && (right_of_curr == 0))
					{
						location = PointLocation::Inside;
						break;
					}
					else
					{
						right_of_curr = right_of_dprev;
						edge = dprev_edge;
					}
				}
				else if ((right_of_curr == 0) && (0 <= isRightOf(m_internal.vertices[edgeEnd(onext_edge)].pt, edge)))
				{
					edge = symEdge(edge);
				}
				else
				{
					right_of_curr = right_of_onext;
					edge = onext_edge;
				}
			}
		}

		m_internal.recentEdge = edge;

		if (location == PointLocation::Inside)
		{
			Vec2 org_pt, dst_pt;
			edgeBegin(edge, &org_pt);
			edgeEnd(edge, &dst_pt);

			double t1 = Abs(pt.x - org_pt.x);
			t1 += Abs(pt.y - org_pt.y);
			double t2 = Abs(pt.x - dst_pt.x);
			t2 += Abs(pt.y - dst_pt.y);
			double t3 = Abs(org_pt.x - dst_pt.x);
			t3 += Abs(org_pt.y - dst_pt.y);

			if (t1 < DBL_EPSILON)
			{
				location = PointLocation::OnVertex;
				vertex = edgeBegin(edge);
				edge = 0;
			}
			else if (t2 < DBL_EPSILON)
			{
				location = PointLocation::OnVertex;
				vertex = edgeEnd(edge);
				edge = 0;
			}
			else if (((t1 < t3) || (t2 < t3)) && (Abs(TriangleArea(pt, org_pt, dst_pt)) < DBL_EPSILON))
			{
				location = PointLocation::OnEdge;
				vertex = 0;
			}
		}

		if (location == PointLocation::Error)
		{
			edge = 0;
			vertex = 0;
		}

		_edge = edge;
		_vertex = vertex;

		return location;
	}

	////////////////////////////////////////////////////////////////
	//
	//	newEdge
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::EdgeID Subdivision2D::newEdge()
	{
		if (m_internal.freeQEdge <= 0)
		{
			m_internal.qEdges.emplace_back();
			m_internal.freeQEdge = static_cast<int32>(m_internal.qEdges.size() - 1);
		}
		
		const EdgeID edge = (m_internal.freeQEdge * 4);
		m_internal.freeQEdge = m_internal.qEdges[edge >> 2].next[1];
		m_internal.qEdges[edge >> 2] = QuadEdge{ edge };
		return edge;
	}

	////////////////////////////////////////////////////////////////
	//
	//	deleteEdge
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::deleteEdge(EdgeID edge)
	{
		assert((size_t)(edge >> 2) < (size_t)m_internal.qEdges.size());
		splice(edge, getEdge(edge, EdgeType::PreviousAroundOrigin));

		const EdgeID sedge = symEdge(edge);
		splice(sedge, getEdge(sedge, EdgeType::PreviousAroundOrigin));

		edge >>= 2;
		m_internal.qEdges[edge].next[0] = 0;
		m_internal.qEdges[edge].next[1] = m_internal.freeQEdge;
		m_internal.freeQEdge = edge;
	}

	////////////////////////////////////////////////////////////////
	//
	//	newPoint
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::VertexID Subdivision2D::newPoint(const Vec2& pt, bool isVirtual, EdgeID firstEdge)
	{
		if (m_internal.freePoint == 0)
		{
			m_internal.vertices.emplace_back();
			m_internal.freePoint = static_cast<int32>(m_internal.vertices.size() - 1);
		}

		const VertexID vidx = m_internal.freePoint;
		m_internal.freePoint = m_internal.vertices[vidx].firstEdge;
		m_internal.vertices[vidx] = Vertex{ pt, isVirtual, firstEdge };

		return vidx;
	}

	////////////////////////////////////////////////////////////////
	//
	//	deletePoint
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::deletePoint(const VertexID vidx)
	{
		assert((size_t)vidx < m_internal.vertices.size());
		m_internal.vertices[vidx].firstEdge = m_internal.freePoint;
		m_internal.vertices[vidx].type = -1;
		m_internal.freePoint = vidx;
	}

	////////////////////////////////////////////////////////////////
	//
	//	setEdgePoints
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::setEdgePoints(const EdgeID edge, const VertexID orgPt, const VertexID dstPt)
	{
		m_internal.qEdges[edge >> 2].pt[edge & 3] = orgPt;
		m_internal.qEdges[edge >> 2].pt[(edge + 2) & 3] = dstPt;
		m_internal.vertices[orgPt].firstEdge = edge;
		m_internal.vertices[dstPt].firstEdge = (edge ^ 2);
	}

	////////////////////////////////////////////////////////////////
	//
	//	splice
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::splice(const EdgeID edgeA, const EdgeID edgeB)
	{
		EdgeID& a_next = m_internal.qEdges[edgeA >> 2].next[edgeA & 3];
		EdgeID& b_next = m_internal.qEdges[edgeB >> 2].next[edgeB & 3];
		const EdgeID a_rot = rotateEdge(a_next, 1);
		const EdgeID b_rot = rotateEdge(b_next, 1);
		EdgeID& a_rot_next = m_internal.qEdges[a_rot >> 2].next[a_rot & 3];
		EdgeID& b_rot_next = m_internal.qEdges[b_rot >> 2].next[b_rot & 3];
		std::swap(a_next, b_next);
		std::swap(a_rot_next, b_rot_next);
	}

	////////////////////////////////////////////////////////////////
	//
	//	connectEdges
	//
	////////////////////////////////////////////////////////////////

	Subdivision2D::EdgeID Subdivision2D::connectEdges(const EdgeID edgeA, const EdgeID edgeB)
	{
		const EdgeID edge = newEdge();

		splice(edge, getEdge(edgeA, EdgeType::NextAroundLeft));
		splice(symEdge(edge), edgeB);

		setEdgePoints(edge, edgeEnd(edgeA), edgeBegin(edgeB));
		return edge;
	}

	////////////////////////////////////////////////////////////////
	//
	//	swapEdges
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::swapEdges(const EdgeID edge)
	{
		const EdgeID sedge = symEdge(edge);
		const EdgeID a = getEdge(edge, EdgeType::PreviousAroundOrigin);
		const EdgeID b = getEdge(sedge, EdgeType::PreviousAroundOrigin);

		splice(edge, a);
		splice(sedge, b);

		setEdgePoints(edge, edgeEnd(a), edgeEnd(b));

		splice(edge, getEdge(a, EdgeType::NextAroundLeft));
		splice(sedge, getEdge(b, EdgeType::NextAroundLeft));
	}

	////////////////////////////////////////////////////////////////
	//
	//	isRightOf
	//
	////////////////////////////////////////////////////////////////

	int32 Subdivision2D::isRightOf(const Vec2& pt, const EdgeID edge) const
	{
		Vec2 org, dst;
		edgeBegin(edge, &org);
		edgeEnd(edge, &dst);
		const double cw_area = TriangleArea(pt, dst, org);
		return ((0 < cw_area) - (cw_area < 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	clearVoronoi
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::clearVoronoi()
	{
		size_t total = m_internal.qEdges.size();

		for (size_t i = 0; i < total; ++i)
		{
			m_internal.qEdges[i].pt[1] = m_internal.qEdges[i].pt[3] = 0;
		}

		total = m_internal.vertices.size();

		for (size_t i = 0; i < total; ++i)
		{
			if (m_internal.vertices[i].isVirtual())
			{
				deletePoint(static_cast<VertexID>(i));
			}
		}

		m_internal.validGeometry = false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	calcVoronoi
	//
	////////////////////////////////////////////////////////////////

	void Subdivision2D::calcVoronoi()
	{
		// check if it is already calculated
		if (m_internal.validGeometry)
		{
			return;
		}

		clearVoronoi();
		const int32 total = static_cast<int32>(m_internal.qEdges.size());

		// loop through all quad-edges, except for the first 3 (#1, #2, #3 - 0 is reserved for "NULL" pointer)
		for (int32 i = 4; i < total; ++i)
		{
			QuadEdge& quadedge = m_internal.qEdges[i];

			if (quadedge.isFree())
			{
				continue;
			}

			const EdgeID edge0 = (i * 4);
			Vec2 org0, dst0, org1, dst1;

			if (not quadedge.pt[3])
			{
				const EdgeID edge1 = getEdge(edge0, EdgeType::NextAroundLeft);
				const EdgeID edge2 = getEdge(edge1, EdgeType::NextAroundLeft);

				edgeBegin(edge0, &org0);
				edgeEnd(edge0, &dst0);
				edgeBegin(edge1, &org1);
				edgeEnd(edge1, &dst1);

				const Vec2 virtPoint = ComputeVoronoiPoint(org0, dst0, org1, dst1);

				if ((Abs(virtPoint.x) < (Largest<double> * 0.5)) &&
					(Abs(virtPoint.y) < (Largest<double> * 0.5)))
				{
					quadedge.pt[3] = m_internal.qEdges[edge1 >> 2].pt[3 - (edge1 & 2)] =
						m_internal.qEdges[edge2 >> 2].pt[3 - (edge2 & 2)] = newPoint(virtPoint, true);
				}
			}

			if (not quadedge.pt[1])
			{
				const EdgeID edge1 = getEdge(edge0, EdgeType::NextAroundRight);
				const EdgeID edge2 = getEdge(edge1, EdgeType::NextAroundRight);

				edgeBegin(edge0, &org0);
				edgeEnd(edge0, &dst0);
				edgeBegin(edge1, &org1);
				edgeEnd(edge1, &dst1);

				const Vec2 virtPoint = ComputeVoronoiPoint(org0, dst0, org1, dst1);

				if ((Abs(virtPoint.x) < (Largest<double> * 0.5)) &&
					(Abs(virtPoint.y) < (Largest<double> * 0.5)))
				{
					quadedge.pt[1] = m_internal.qEdges[edge1 >> 2].pt[1 + (edge1 & 2)] =
						m_internal.qEdges[edge2 >> 2].pt[1 + (edge2 & 2)] = newPoint(virtPoint, true);
				}
			}
		}

		m_internal.validGeometry = true;
	}
}
