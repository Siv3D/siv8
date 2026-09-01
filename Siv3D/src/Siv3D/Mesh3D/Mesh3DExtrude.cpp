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
# include <Siv3D/MathConstants.hpp>
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
		static bool ValidateRing(
			const std::span<const Vec2> ring,
			const bool expectPositiveArea,
			double& perimeter) noexcept
		{
			if (ring.size() < 3)
			{
				return false;
			}

			double twiceArea = 0.0;
			perimeter = 0.0;

			for (size_t i = 0; i < ring.size(); ++i)
			{
				const Vec2 current = ring[i];
				const Vec2 next = ring[(i + 1) % ring.size()];
				if ((not IsFloatRepresentable(current.x))
					|| (not IsFloatRepresentable(current.y))
					|| (not IsFloatRepresentable(next.x))
					|| (not IsFloatRepresentable(next.y)))
				{
					return false;
				}

				const Float2 currentF = current;
				const Float2 nextF = next;
				const double dx = (static_cast<double>(nextF.x) - currentF.x);
				const double dy = (static_cast<double>(nextF.y) - currentF.y);
				const double edgeLength = std::hypot(dx, dy);
				if ((not std::isfinite(edgeLength))
					|| (edgeLength == 0.0))
				{
					return false;
				}

				perimeter += edgeLength;
				twiceArea += ((static_cast<double>(currentF.x) * nextF.y)
					- (static_cast<double>(nextF.x) * currentF.y));
			}

			if ((not std::isfinite(perimeter))
				|| (not std::isfinite(twiceArea)))
			{
				return false;
			}

			return (expectPositiveArea ? (0.0 < twiceArea) : (twiceArea < 0.0));
		}

		[[nodiscard]]
		static double ComputeRingPerimeter(const std::span<const Vec2> ring) noexcept
		{
			double perimeter = 0.0;

			for (size_t i = 0; i < ring.size(); ++i)
			{
				const Float2 current = ring[i];
				const Float2 next = ring[(i + 1) % ring.size()];
				perimeter += std::hypot(
					(static_cast<double>(next.x) - current.x),
					(static_cast<double>(next.y) - current.y));
			}

			return perimeter;
		}

		[[nodiscard]]
		static bool ValidateCapTriangles(
			const std::span<const Float2> vertices,
			const std::span<const TriangleIndex> indices) noexcept
		{
			if ((vertices.size() < 3)
				|| indices.empty())
			{
				return false;
			}

			for (const Float2 vertex : vertices)
			{
				if ((not std::isfinite(vertex.x))
					|| (not std::isfinite(vertex.y)))
				{
					return false;
				}
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

		struct SideEdge
		{
			double length;
			Float3 normal;
		};

		[[nodiscard]]
		static SideEdge MakeSideEdge(const Float2 current, const Float2 next) noexcept
		{
			const double dx = (static_cast<double>(next.x) - current.x);
			const double sourceDY = (static_cast<double>(next.y) - current.y);
			const double length = std::hypot(dx, sourceDY);
			const float tangentX = static_cast<float>(dx / length);
			const float tangentZ = static_cast<float>(-sourceDY / length);
			return SideEdge{ length, Float3{ -tangentZ, 0.0f, tangentX } };
		}

		[[nodiscard]]
		static Float3 SmoothSideNormal(
			const Float3& normal0,
			const Float3& normal1,
			const Float3& fallback,
			const float minNormalDot) noexcept
		{
			if (normal0.dot(normal1) < minNormalDot)
			{
				return fallback;
			}

			const Float3 sum = (normal0 + normal1);
			const float lengthSq = sum.lengthSq();
			if (lengthSq == 0.0f)
			{
				return fallback;
			}

			return (sum / std::sqrt(lengthSq));
		}

		[[nodiscard]]
		static Float4 MakeSideTangent(const Float3& normal) noexcept
		{
			return Float4{ normal.z, 0.0f, -normal.x, -1.0f };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Extrude
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Extrude(const Polygon& polygon, const double _height)
	{
		return Extrude(polygon, _height, 0.0);
	}

	Mesh3D Mesh3D::Extrude(
		const Polygon& polygon,
		const double _height,
		const double smoothingAngle)
	{
		if ((polygon.isEmpty())
			|| (not IsFloatRepresentable(_height))
			|| (not std::isfinite(smoothingAngle))
			|| (smoothingAngle < 0.0)
			|| (Math::Pi < smoothingAngle))
		{
			return{};
		}

		const float height = static_cast<float>(_height);
		if (height <= 0.0f)
		{
			return{};
		}

		const bool smoothSide = (0.0 < smoothingAngle);
		const float minNormalDot = (smoothSide
			? std::cos(static_cast<float>(smoothingAngle))
			: 1.0f);

		const auto& capVertices = polygon.vertices();
		const auto& capIndices = polygon.indices();
		if (not ValidateCapTriangles(capVertices, capIndices))
		{
			return{};
		}

		size_t edgeCount = polygon.outer().size();
		for (const auto& inner : polygon.inners())
		{
			if (not CheckedAdd(edgeCount, inner.size(), edgeCount))
			{
				return{};
			}
		}

		double outerPerimeter;
		if (not ValidateRing(polygon.outer(), true, outerPerimeter))
		{
			return{};
		}

		for (const auto& inner : polygon.inners())
		{
			double perimeter;
			if (not ValidateRing(inner, false, perimeter))
			{
				return{};
			}
		}

		size_t capVertexTotal;
		size_t sideVertexCount;
		size_t vertexCount;
		size_t capTriangleTotal;
		size_t sideTriangleCount;
		size_t triangleCount;
		if ((not CheckedMultiply(capVertices.size(), 2, capVertexTotal))
			|| (not CheckedMultiply(edgeCount, 4, sideVertexCount))
			|| (not CheckedAdd(capVertexTotal, sideVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(capIndices.size(), 2, capTriangleTotal))
			|| (not CheckedMultiply(edgeCount, 2, sideTriangleCount))
			|| (not CheckedAdd(capTriangleTotal, sideTriangleCount, triangleCount)))
		{
			return{};
		}

		float minX = capVertices.front().x;
		float maxX = minX;
		float minY = capVertices.front().y;
		float maxY = minY;
		for (const Float2 vertex : capVertices)
		{
			minX = std::min(minX, vertex.x);
			maxX = std::max(maxX, vertex.x);
			minY = std::min(minY, vertex.y);
			maxY = std::max(maxY, vertex.y);
		}

		const double width = (static_cast<double>(maxX) - minX);
		const double depth = (static_cast<double>(maxY) - minY);
		if ((width <= 0.0)
			|| (depth <= 0.0))
		{
			return{};
		}

		Mesh3D mesh{ vertexCount, triangleCount };
		const float halfHeight = (height * 0.5f);
		const size_t topVertexBase = 0;
		const size_t bottomVertexBase = capVertices.size();
		const Float4 capTangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		for (size_t i = 0; i < capVertices.size(); ++i)
		{
			const Float2 source = capVertices[i];
			const float u = static_cast<float>((static_cast<double>(source.x) - minX) / width);
			const float v = static_cast<float>((static_cast<double>(source.y) - minY) / depth);
			const Float3 topPosition{ source.x, halfHeight, -source.y };
			const Float3 bottomPosition{ source.x, -halfHeight, -source.y };

			mesh.vertices[topVertexBase + i] = Vertex3D{
				.pos = topPosition,
				.normal = Float3::UnitY(),
				.tex = Float2{ u, v },
				.tangent = capTangent
			};
			mesh.vertices[bottomVertexBase + i] = Vertex3D{
				.pos = bottomPosition,
				.normal = -Float3::UnitY(),
				.tex = Float2{ u, (1.0f - v) },
				.tangent = capTangent
			};
		}

		TriangleIndex32* pTriangle = mesh.indices.data();
		for (const TriangleIndex& source : capIndices)
		{
			const uint32 i0 = source.i0;
			const uint32 i1 = source.i1;
			const uint32 i2 = source.i2;
			*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
			*pTriangle++ = TriangleIndex32{
				static_cast<uint32>(bottomVertexBase + i0),
				static_cast<uint32>(bottomVertexBase + i2),
				static_cast<uint32>(bottomVertexBase + i1)
			};
		}

		size_t sideVertexOffset = capVertexTotal;
		const auto writeRing = [&](const std::span<const Vec2> ring, const double perimeter)
		{
			double accumulatedLength = 0.0;
			const SideEdge firstEdge = MakeSideEdge(ring[0], ring[1]);
			const SideEdge lastEdge = MakeSideEdge(ring.back(), ring.front());
			SideEdge previousEdge = lastEdge;
			SideEdge currentEdge = firstEdge;

			for (size_t i = 0; i < ring.size(); ++i)
			{
				const Float2 current = ring[i];
				const Float2 next = ring[(i + 1) % ring.size()];
				const SideEdge nextEdge = ((i + 1) == ring.size()
					? firstEdge
					: (((i + 2) == ring.size())
						? lastEdge
						: MakeSideEdge(ring[i + 1], ring[i + 2])));
				Float3 startNormal = currentEdge.normal;
				Float3 endNormal = currentEdge.normal;
				if (smoothSide)
				{
					startNormal = SmoothSideNormal(
						previousEdge.normal, currentEdge.normal, currentEdge.normal, minNormalDot);
					endNormal = SmoothSideNormal(
						currentEdge.normal, nextEdge.normal, currentEdge.normal, minNormalDot);
				}
				const Float4 startTangent = MakeSideTangent(startNormal);
				const Float4 endTangent = (smoothSide
					? MakeSideTangent(endNormal)
					: startTangent);
				const float u0 = static_cast<float>(accumulatedLength / perimeter);
				accumulatedLength += currentEdge.length;
				const float u1 = ((i + 1 == ring.size())
					? 1.0f
					: static_cast<float>(accumulatedLength / perimeter));
				const Float3 topCurrent{ current.x, halfHeight, -current.y };
				const Float3 bottomCurrent{ current.x, -halfHeight, -current.y };
				const Float3 topNext{ next.x, halfHeight, -next.y };
				const Float3 bottomNext{ next.x, -halfHeight, -next.y };

				mesh.vertices[sideVertexOffset + 0] = Vertex3D{
					.pos = topCurrent, .normal = startNormal, .tex = Float2{ u0, 0.0f }, .tangent = startTangent
				};
				mesh.vertices[sideVertexOffset + 1] = Vertex3D{
					.pos = bottomCurrent, .normal = startNormal, .tex = Float2{ u0, 1.0f }, .tangent = startTangent
				};
				mesh.vertices[sideVertexOffset + 2] = Vertex3D{
					.pos = topNext, .normal = endNormal, .tex = Float2{ u1, 0.0f }, .tangent = endTangent
				};
				mesh.vertices[sideVertexOffset + 3] = Vertex3D{
					.pos = bottomNext, .normal = endNormal, .tex = Float2{ u1, 1.0f }, .tangent = endTangent
				};

				const uint32 i0 = static_cast<uint32>(sideVertexOffset);
				*pTriangle++ = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
				*pTriangle++ = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
				sideVertexOffset += 4;
				previousEdge = currentEdge;
				currentEdge = nextEdge;
			}
		};

		writeRing(polygon.outer(), outerPerimeter);
		for (const auto& inner : polygon.inners())
		{
			writeRing(inner, ComputeRingPerimeter(inner));
		}

		return mesh;
	}
}
