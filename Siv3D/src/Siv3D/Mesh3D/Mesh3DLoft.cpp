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

# include <Siv3D/Mesh3D.hpp>
# include <Siv3D/Polygon.hpp>
# include <algorithm>
# include <cmath>
# include <limits>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool IsFloatRepresentable(const double value) noexcept
		{
			constexpr double MaxFloat = std::numeric_limits<float>::max();
			return (std::isfinite(value)
				&& (-MaxFloat <= value)
				&& (value <= MaxFloat));
		}

		[[nodiscard]]
		static bool CheckedAdd(const size_t a, const size_t b, size_t& result) noexcept
		{
			if ((std::numeric_limits<size_t>::max() - a) < b)
			{
				return false;
			}

			result = (a + b);
			return true;
		}

		[[nodiscard]]
		static bool CheckedMultiply(const size_t a, const size_t b, size_t& result) noexcept
		{
			if ((a != 0)
				&& ((std::numeric_limits<size_t>::max() / a) < b))
			{
				return false;
			}

			result = (a * b);
			return true;
		}

		[[nodiscard]]
		static bool ValidateCap(
			const std::span<const Float2> vertices,
			const std::span<const TriangleIndex> indices) noexcept
		{
			if ((vertices.size() < 3)
				|| indices.empty())
			{
				return false;
			}

			for (const TriangleIndex& index : indices)
			{
				if ((vertices.size() <= index.i0)
					|| (vertices.size() <= index.i1)
					|| (vertices.size() <= index.i2))
				{
					return false;
				}

				const Float2 p0 = vertices[index.i0];
				const Float2 p1 = vertices[index.i1];
				const Float2 p2 = vertices[index.i2];
				const double twiceArea = (
					((static_cast<double>(p1.x) - p0.x) * (static_cast<double>(p2.y) - p0.y))
					- ((static_cast<double>(p1.y) - p0.y) * (static_cast<double>(p2.x) - p0.x)));
				if ((not std::isfinite(twiceArea))
					|| (twiceArea <= 0.0))
				{
					return false;
				}
			}

			return true;
		}

		struct Bounds2D
		{
			float minX;
			float maxX;
			float minY;
			float maxY;
		};

		[[nodiscard]]
		static bool ComputeBounds(
			const std::span<const Float2> vertices,
			Bounds2D& bounds) noexcept
		{
			bounds = Bounds2D{
				.minX = vertices.front().x,
				.maxX = vertices.front().x,
				.minY = vertices.front().y,
				.maxY = vertices.front().y
			};

			for (const Float2 vertex : vertices)
			{
				bounds.minX = std::min(bounds.minX, vertex.x);
				bounds.maxX = std::max(bounds.maxX, vertex.x);
				bounds.minY = std::min(bounds.minY, vertex.y);
				bounds.maxY = std::max(bounds.maxY, vertex.y);
			}

			return ((bounds.minX < bounds.maxX)
				&& (bounds.minY < bounds.maxY));
		}

		[[nodiscard]]
		static Vec3 ToVec3(const Float2 point, const float height) noexcept
		{
			return{ point.x, height, -point.y };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	LoftImpl
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::LoftImpl(
		const std::span<const std::span<const Vec2>> sections,
		const std::span<const double> heights,
		const Vec2 _uvScale,
		const Vec2 _uvOffset)
	{
		if ((sections.size() < 2)
			|| (sections.size() != heights.size())
			|| (sections.front().size() < 3)
			|| (not IsFloatRepresentable(_uvScale.x))
			|| (not IsFloatRepresentable(_uvScale.y))
			|| (not IsFloatRepresentable(_uvOffset.x))
			|| (not IsFloatRepresentable(_uvOffset.y))
			|| (not IsFloatRepresentable(_uvOffset.x + _uvScale.x))
			|| (not IsFloatRepresentable(_uvOffset.y + _uvScale.y)))
		{
			return{};
		}

		const size_t sectionCount = sections.size();
		const size_t ringVertexCount = sections.front().size();
		size_t sectionVertexCount;
		if ((not CheckedMultiply(sectionCount, ringVertexCount, sectionVertexCount))
			|| (Mesh3D::MaxVertexCount < sectionVertexCount))
		{
			return{};
		}

		Array<float> sectionHeights(sectionCount);
		for (size_t sectionIndex = 0; sectionIndex < sectionCount; ++sectionIndex)
		{
			if ((sections[sectionIndex].size() != ringVertexCount)
				|| (not IsFloatRepresentable(heights[sectionIndex])))
			{
				return{};
			}

			sectionHeights[sectionIndex] = static_cast<float>(heights[sectionIndex]);
			if ((0 < sectionIndex)
				&& (not (sectionHeights[sectionIndex - 1] < sectionHeights[sectionIndex])))
			{
				return{};
			}
		}

		const double totalHeight = (static_cast<double>(sectionHeights.back()) - sectionHeights.front());
		if (not IsFloatRepresentable(_uvOffset.y + (_uvScale.y * totalHeight)))
		{
			return{};
		}

		Array<Float2> points(sectionVertexCount);
		double firstPerimeter = 0.0;
		for (size_t sectionIndex = 0; sectionIndex < sectionCount; ++sectionIndex)
		{
			const auto section = sections[sectionIndex];
			const size_t sectionBase = (sectionIndex * ringVertexCount);
			for (size_t vertexIndex = 0; vertexIndex < ringVertexCount; ++vertexIndex)
			{
				const Vec2 point = section[vertexIndex];
				if ((not IsFloatRepresentable(point.x))
					|| (not IsFloatRepresentable(point.y)))
				{
					return{};
				}

				points[sectionBase + vertexIndex] = point;
			}

			double twiceArea = 0.0;
			double perimeter = 0.0;
			for (size_t vertexIndex = 0; vertexIndex < ringVertexCount; ++vertexIndex)
			{
				const Float2 current = points[sectionBase + vertexIndex];
				const Float2 next = points[sectionBase + ((vertexIndex + 1) % ringVertexCount)];
				const double dx = (static_cast<double>(next.x) - current.x);
				const double dy = (static_cast<double>(next.y) - current.y);
				const double edgeLength = std::hypot(dx, dy);
				if ((not std::isfinite(edgeLength))
					|| (edgeLength == 0.0))
				{
					return{};
				}

				perimeter += edgeLength;
				twiceArea += ((static_cast<double>(current.x) * next.y)
					- (static_cast<double>(next.x) * current.y));
			}

			if ((not std::isfinite(perimeter))
				|| (not std::isfinite(twiceArea))
				|| (twiceArea <= 0.0))
			{
				return{};
			}

			if (sectionIndex == 0)
			{
				firstPerimeter = perimeter;
			}
		}

		const Polygon bottomPolygon{ sections.front() };
		const Polygon topPolygon{ sections.back() };
		if (bottomPolygon.isEmpty()
			|| topPolygon.isEmpty()
			|| (not ValidateCap(bottomPolygon.vertices(), bottomPolygon.indices()))
			|| (not ValidateCap(topPolygon.vertices(), topPolygon.indices())))
		{
			return{};
		}

		Bounds2D bottomBounds;
		Bounds2D topBounds;
		if ((not ComputeBounds(bottomPolygon.vertices(), bottomBounds))
			|| (not ComputeBounds(topPolygon.vertices(), topBounds)))
		{
			return{};
		}

		size_t capVertexCount;
		size_t sideVertexCount;
		size_t vertexCount;
		size_t capTriangleCount;
		size_t sideQuadCount;
		size_t sideTriangleCount;
		size_t triangleCount;
		if ((not CheckedAdd(bottomPolygon.vertices().size(), topPolygon.vertices().size(), capVertexCount))
			|| (not CheckedMultiply(sectionVertexCount, 2, sideVertexCount))
			|| (not CheckedAdd(capVertexCount, sideVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedAdd(bottomPolygon.indices().size(), topPolygon.indices().size(), capTriangleCount))
			|| (not CheckedMultiply((sectionCount - 1), ringVertexCount, sideQuadCount))
			|| (not CheckedMultiply(sideQuadCount, 2, sideTriangleCount))
			|| (not CheckedAdd(capTriangleCount, sideTriangleCount, triangleCount)))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const size_t bottomCapBase = 0;
		const size_t topCapBase = bottomPolygon.vertices().size();
		const Float4 capTangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		const auto writeCap = [&](const Polygon& polygon, const Bounds2D& bounds,
			const size_t capBase, const float height, const bool bottom)
		{
			const double width = (static_cast<double>(bounds.maxX) - bounds.minX);
			const double depth = (static_cast<double>(bounds.maxY) - bounds.minY);
			for (size_t vertexIndex = 0; vertexIndex < polygon.vertices().size(); ++vertexIndex)
			{
				const Float2 source = polygon.vertices()[vertexIndex];
				const double u = ((static_cast<double>(source.x) - bounds.minX) / width);
				const double sourceV = ((static_cast<double>(source.y) - bounds.minY) / depth);
				const double v = (bottom ? (1.0 - sourceV) : sourceV);
				mesh.vertices[capBase + vertexIndex] = Vertex3D{
					.pos = Float3{ source.x, height, -source.y },
					.normal = (bottom ? -Float3::UnitY() : Float3::UnitY()),
					.tex = Float2{
						static_cast<float>(_uvOffset.x + (_uvScale.x * u)),
						static_cast<float>(_uvOffset.y + (_uvScale.y * v))
					},
					.tangent = capTangent
				};
			}
		};

		writeCap(bottomPolygon, bottomBounds, bottomCapBase, sectionHeights.front(), true);
		writeCap(topPolygon, topBounds, topCapBase, sectionHeights.back(), false);

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (const TriangleIndex& source : bottomPolygon.indices())
		{
			*pTriangle++ = TriangleIndex32{
				static_cast<uint32>(bottomCapBase + source.i0),
				static_cast<uint32>(bottomCapBase + source.i2),
				static_cast<uint32>(bottomCapBase + source.i1)
			};
		}

		for (const TriangleIndex& source : topPolygon.indices())
		{
			*pTriangle++ = TriangleIndex32{
				static_cast<uint32>(topCapBase + source.i0),
				static_cast<uint32>(topCapBase + source.i1),
				static_cast<uint32>(topCapBase + source.i2)
			};
		}

		const size_t sideBase = capVertexCount;
		double accumulatedLength = 0.0;
		for (size_t edgeIndex = 0; edgeIndex < ringVertexCount; ++edgeIndex)
		{
			const size_t nextEdgeIndex = ((edgeIndex + 1) % ringVertexCount);
			const Float2 firstCurrent = points[edgeIndex];
			const Float2 firstNext = points[nextEdgeIndex];
			const double firstEdgeLength = std::hypot(
				(static_cast<double>(firstNext.x) - firstCurrent.x),
				(static_cast<double>(firstNext.y) - firstCurrent.y));
			const double u0 = (accumulatedLength / firstPerimeter);
			accumulatedLength += firstEdgeLength;
			const double u1 = (((edgeIndex + 1) == ringVertexCount)
				? 1.0
				: (accumulatedLength / firstPerimeter));
			const size_t edgeBase = (sideBase + (edgeIndex * sectionCount * 2));

			for (size_t sectionIndex = 0; sectionIndex < sectionCount; ++sectionIndex)
			{
				const size_t sectionBase = (sectionIndex * ringVertexCount);
				const size_t previousSection = ((sectionIndex == 0) ? 0 : (sectionIndex - 1));
				const size_t nextSection = (((sectionIndex + 1) == sectionCount)
					? sectionIndex
					: (sectionIndex + 1));
				const Float2 current = points[sectionBase + edgeIndex];
				const Float2 next = points[sectionBase + nextEdgeIndex];
				const Vec3 currentPosition = ToVec3(current, sectionHeights[sectionIndex]);
				const Vec3 nextPosition = ToVec3(next, sectionHeights[sectionIndex]);
				const Vec3 edgeDirection = (nextPosition - currentPosition);
				const Vec3 currentSectionDirection = (
					ToVec3(points[nextSection * ringVertexCount + edgeIndex], sectionHeights[nextSection])
					- ToVec3(points[previousSection * ringVertexCount + edgeIndex], sectionHeights[previousSection]));
				const Vec3 nextSectionDirection = (
					ToVec3(points[nextSection * ringVertexCount + nextEdgeIndex], sectionHeights[nextSection])
					- ToVec3(points[previousSection * ringVertexCount + nextEdgeIndex], sectionHeights[previousSection]));
				const Vec3 currentNormal = edgeDirection.cross(currentSectionDirection).normalized();
				const Vec3 nextNormal = edgeDirection.cross(nextSectionDirection).normalized();
				const Vec3 tangent = edgeDirection.normalized();
				const float currentHandedness = ((0.0 <= currentNormal.cross(tangent).dot(currentSectionDirection))
					? 1.0f : -1.0f);
				const float nextHandedness = ((0.0 <= nextNormal.cross(tangent).dot(nextSectionDirection))
					? 1.0f : -1.0f);
				const double heightDistance = (static_cast<double>(sectionHeights[sectionIndex]) - sectionHeights.front());
				const float v = static_cast<float>(_uvOffset.y + (_uvScale.y * heightDistance));
				const size_t vertexBase = (edgeBase + sectionIndex * 2);

				mesh.vertices[vertexBase + 0] = Vertex3D{
					.pos = currentPosition,
					.normal = currentNormal,
					.tex = Float2{
						static_cast<float>(_uvOffset.x + (_uvScale.x * u0)), v
					},
					.tangent = Float4{
						static_cast<float>(tangent.x),
						static_cast<float>(tangent.y),
						static_cast<float>(tangent.z),
						currentHandedness
					}
				};
				mesh.vertices[vertexBase + 1] = Vertex3D{
					.pos = nextPosition,
					.normal = nextNormal,
					.tex = Float2{
						static_cast<float>(_uvOffset.x + (_uvScale.x * u1)), v
					},
					.tangent = Float4{
						static_cast<float>(tangent.x),
						static_cast<float>(tangent.y),
						static_cast<float>(tangent.z),
						nextHandedness
					}
				};
			}

			for (size_t sectionIndex = 0; (sectionIndex + 1) < sectionCount; ++sectionIndex)
			{
				const uint32 lowerCurrent = static_cast<uint32>(edgeBase + sectionIndex * 2);
				const uint32 lowerNext = (lowerCurrent + 1);
				const uint32 upperCurrent = (lowerCurrent + 2);
				const uint32 upperNext = (lowerCurrent + 3);
				*pTriangle++ = TriangleIndex32{ lowerCurrent, lowerNext, upperCurrent };
				*pTriangle++ = TriangleIndex32{ upperCurrent, lowerNext, upperNext };
			}
		}

		return mesh;
	}
}
