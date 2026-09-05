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
# include <Siv3D/Mesh3DBuilder.hpp>
# include <Siv3D/MathConstants.hpp>
# include <Siv3D/Polygon.hpp>
# include "Mesh3DCommon.hpp"
# include <algorithm>
# include <cmath>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::AddedRange;
		using Mesh3DDetail::AdditionFailed;
		using Mesh3DDetail::CapValidationResult;
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::ForEachValidCapTriangle;
		using Mesh3DDetail::IsFloatRepresentable;
		using Mesh3DDetail::RingValidationResult;
		using Mesh3DDetail::ValidateCapTriangles;
		using Mesh3DDetail::ValidateRing;

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
}

namespace s3d::Mesh3DDetail
{
	Mesh3DAddResult AppendExtrude(
		Mesh3D& mesh,
		const Polygon& polygon,
		const double _height,
		const double smoothingAngle)
	{
		if (polygon.isEmpty())
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Extrude(): The polygon is empty");
		}

		if ((not IsFloatRepresentable(_height))
			|| (not std::isfinite(smoothingAngle)))
		{
			return AdditionFailed(Mesh3DErrorCode::NumericRange, U"Mesh3D::Extrude(): A numeric parameter is non-finite or outside the float range");
		}

		if ((smoothingAngle < 0.0)
			|| (Math::Pi < smoothingAngle))
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Extrude(): smoothingAngle must be in the range [0, Pi]");
		}

		const float height = static_cast<float>(_height);
		if (height <= 0.0f)
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidArgument, U"Mesh3D::Extrude(): height must be positive after conversion to float");
		}

		const bool smoothSide = (0.0 < smoothingAngle);
		const float minNormalDot = (smoothSide
			? std::cos(static_cast<float>(smoothingAngle))
			: 1.0f);

		const auto& capVertices = polygon.vertices();
		const auto& capIndices = polygon.indices();
		size_t validCapTriangleCount;
		const CapValidationResult capValidation = ValidateCapTriangles<true>(
			capVertices, capIndices, validCapTriangleCount);
		if (capValidation != CapValidationResult::Valid)
		{
			return AdditionFailed(
				(capValidation == CapValidationResult::NumericRange
					? Mesh3DErrorCode::NumericRange
					: Mesh3DErrorCode::InvalidGeometry),
				U"Mesh3D::Extrude(): The polygon cap triangulation is invalid");
		}

		size_t edgeCount = polygon.outer().size();
		for (const auto& inner : polygon.inners())
		{
			if (not CheckedAdd(edgeCount, inner.size(), edgeCount))
			{
				return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Extrude(): The polygon edge count exceeds the supported range");
			}
		}

		double outerPerimeter;
		if (ValidateRing(std::span<const Vec2>{ polygon.outer() }, true, outerPerimeter)
			!= RingValidationResult::Valid)
		{
			return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Extrude(): The polygon outer ring is invalid");
		}

		for (const auto& inner : polygon.inners())
		{
			double perimeter;
			if (ValidateRing(std::span<const Vec2>{ inner }, false, perimeter)
				!= RingValidationResult::Valid)
			{
				return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Extrude(): A polygon inner ring is invalid");
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
			|| (not CheckedMultiply(validCapTriangleCount, 2, capTriangleTotal))
			|| (not CheckedMultiply(edgeCount, 2, sideTriangleCount))
			|| (not CheckedAdd(capTriangleTotal, sideTriangleCount, triangleCount)))
		{
			return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Extrude(): The generated mesh exceeds the supported size");
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
			return AdditionFailed(Mesh3DErrorCode::InvalidGeometry, U"Mesh3D::Extrude(): The polygon bounds must have positive width and depth");
		}

		const size_t vertexBase = mesh.vertices.size();
		const size_t triangleBase = mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if ((not CheckedAdd(vertexBase, vertexCount, newVertexCount))
			|| (Mesh3D::MaxVertexCount < newVertexCount)
			|| (not CheckedAdd(triangleBase, triangleCount, newTriangleCount)))
		{
			return AdditionFailed(Mesh3DErrorCode::SizeLimit, U"Mesh3D::Extrude(): The generated mesh exceeds the supported size");
		}

		mesh.vertices.resize(newVertexCount);
		mesh.indices.resize(newTriangleCount);
		const float halfHeight = (height * 0.5f);
		const size_t topVertexBase = vertexBase;
		const size_t bottomVertexBase = (topVertexBase + capVertices.size());
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

		TriangleIndex32* pTriangle = (mesh.indices.data() + triangleBase);
		ForEachValidCapTriangle(capVertices, capIndices, validCapTriangleCount,
			[&](const TriangleIndex& source)
		{
			const uint32 i0 = source.i0;
			const uint32 i1 = source.i1;
			const uint32 i2 = source.i2;
			*pTriangle++ = TriangleIndex32{
				static_cast<uint32>(topVertexBase + i0),
				static_cast<uint32>(topVertexBase + i1),
				static_cast<uint32>(topVertexBase + i2)
			};
			*pTriangle++ = TriangleIndex32{
				static_cast<uint32>(bottomVertexBase + i0),
				static_cast<uint32>(bottomVertexBase + i2),
				static_cast<uint32>(bottomVertexBase + i1)
			};
		});

		size_t sideVertexOffset = (vertexBase + capVertexTotal);
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

		return AddedRange(mesh, vertexBase, triangleBase);
	}
}

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Extrude
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::Extrude(const Polygon& polygon, const double height)
	{
		Mesh3DBuilder builder;
		(void)builder.addExtrude(polygon, height);
		return std::move(builder).build();
	}

	Mesh3D Mesh3D::Extrude(
		const Polygon& polygon,
		const double height,
		const double smoothingAngle)
	{
		Mesh3DBuilder builder;
		(void)builder.addExtrude(polygon, height, smoothingAngle);
		return std::move(builder).build();
	}
}
