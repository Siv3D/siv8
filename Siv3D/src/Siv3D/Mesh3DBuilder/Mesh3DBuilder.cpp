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

# include <Siv3D/Mesh3DBuilder.hpp>
# include "../Mesh3D/Mesh3DCommon.hpp"
# include <algorithm>
# include <array>
# include <cmath>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::GenerationFailed;
		using Mesh3DDetail::IsFloatRepresentable;
		using Mesh3DDetail::TransformVertexRange;

		struct BoxFace
		{
			Float3 center;
			Float3 u;
			Float3 v;
			Float3 normal;
		};

		[[nodiscard]]
		static bool IsFinite(const FloatRect& rect) noexcept
		{
			return (std::isfinite(rect.left)
				&& std::isfinite(rect.top)
				&& std::isfinite(rect.right)
				&& std::isfinite(rect.bottom));
		}

		[[nodiscard]]
		static bool IsFinite(const BoxUVMapping& uvMapping) noexcept
		{
			return (IsFinite(uvMapping.negativeZ)
				&& IsFinite(uvMapping.positiveZ)
				&& IsFinite(uvMapping.positiveX)
				&& IsFinite(uvMapping.negativeX)
				&& IsFinite(uvMapping.positiveY)
				&& IsFinite(uvMapping.negativeY));
		}

		[[nodiscard]]
		static Float2 MapProjectedUV(const FloatRect& uvRect, const Float2 uv) noexcept
		{
			return{
				(uvRect.left + ((uvRect.right - uvRect.left) * uv.x)),
				(uvRect.top + ((uvRect.bottom - uvRect.top) * uv.y))
			};
		}

		[[nodiscard]]
		static Float4 MakeProjectedTangent(const Float3 baseTangent, const FloatRect& uvRect) noexcept
		{
			const float uSign = ((uvRect.right < uvRect.left) ? -1.0f : 1.0f);
			const float vSign = ((uvRect.bottom < uvRect.top) ? -1.0f : 1.0f);
			return{ (baseTangent * uSign), (uSign * vSign) };
		}

		static void TransformAddedVertices(
			Mesh3D& mesh,
			const size_t vertexOffset,
			const Mat4x4& transform) noexcept
		{
			TransformVertexRange(
				std::span<Vertex3D>{ (mesh.vertices.data() + vertexOffset), (mesh.vertices.size() - vertexOffset) },
				transform);
		}

		[[nodiscard]]
		static bool ResizeForAddition(
			Mesh3D& mesh,
			const size_t addedVertexCount,
			const size_t addedTriangleCount,
			size_t& vertexOffset,
			size_t& triangleOffset)
		{
			vertexOffset = mesh.vertices.size();
			triangleOffset = mesh.indices.size();
			size_t newVertexCount;
			size_t newTriangleCount;
			if ((not CheckedAdd(vertexOffset, addedVertexCount, newVertexCount))
				|| (Mesh3D::MaxVertexCount < newVertexCount)
				|| (not CheckedAdd(triangleOffset, addedTriangleCount, newTriangleCount)))
			{
				return false;
			}

			mesh.vertices.resize(newVertexCount);
			mesh.indices.resize(newTriangleCount);
			return true;
		}

		static void WriteProjectedTriangle(
			Mesh3D& mesh,
			size_t& vertexOffset,
			size_t& triangleOffset,
			const std::array<Float3, 3>& positions,
			const std::array<Float2, 3>& projectedUVs,
			const Float3 normal,
			const Float3 baseTangent,
			const FloatRect& uvRect)
		{
			const Float4 tangent = MakeProjectedTangent(baseTangent, uvRect);

			for (size_t i = 0; i < positions.size(); ++i)
			{
				mesh.vertices[vertexOffset + i] = Vertex3D{
					.pos = positions[i],
					.normal = normal,
					.tex = MapProjectedUV(uvRect, projectedUVs[i]),
					.tangent = tangent
				};
			}

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			mesh.indices[triangleOffset] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			vertexOffset += 3;
			++triangleOffset;
		}

		static void WriteProjectedQuad(
			Mesh3D& mesh,
			size_t& vertexOffset,
			size_t& triangleOffset,
			const std::array<Float3, 4>& positions,
			const std::array<Float2, 4>& projectedUVs,
			const Float3 normal,
			const Float3 baseTangent,
			const FloatRect& uvRect)
		{
			const Float4 tangent = MakeProjectedTangent(baseTangent, uvRect);

			for (size_t i = 0; i < positions.size(); ++i)
			{
				mesh.vertices[vertexOffset + i] = Vertex3D{
					.pos = positions[i],
					.normal = normal,
					.tex = MapProjectedUV(uvRect, projectedUVs[i]),
					.tangent = tangent
				};
			}

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
			vertexOffset += 4;
			triangleOffset += 2;
		}

		constexpr float GoldenRatio = 1.6180339887498948482f;

		constexpr std::array<Float3, 12> IcosahedronBaseVertices =
		{{
			{ -1.0f, GoldenRatio, 0.0f },
			{ 1.0f, GoldenRatio, 0.0f },
			{ -1.0f, -GoldenRatio, 0.0f },
			{ 1.0f, -GoldenRatio, 0.0f },
			{ 0.0f, -1.0f, GoldenRatio },
			{ 0.0f, 1.0f, GoldenRatio },
			{ 0.0f, -1.0f, -GoldenRatio },
			{ 0.0f, 1.0f, -GoldenRatio },
			{ GoldenRatio, 0.0f, -1.0f },
			{ GoldenRatio, 0.0f, 1.0f },
			{ -GoldenRatio, 0.0f, -1.0f },
			{ -GoldenRatio, 0.0f, 1.0f },
		}};

		constexpr std::array<std::array<uint32, 3>, 20> IcosahedronFaces =
		{{
			{ 0, 11, 5 }, { 0, 5, 1 }, { 0, 1, 7 }, { 0, 7, 10 }, { 0, 10, 11 },
			{ 1, 5, 9 }, { 5, 11, 4 }, { 11, 10, 2 }, { 10, 7, 6 }, { 7, 1, 8 },
			{ 3, 9, 4 }, { 3, 4, 2 }, { 3, 2, 6 }, { 3, 6, 8 }, { 3, 8, 9 },
			{ 4, 9, 5 }, { 2, 4, 11 }, { 6, 2, 10 }, { 8, 6, 7 }, { 9, 8, 1 },
		}};

		template <size_t VertexCount, size_t FaceCount>
		[[nodiscard]]
		static bool AddTriangleFacedPolyhedron(
			Mesh3D& mesh,
			const float radius,
			const std::array<Float3, VertexCount>& baseVertices,
			const float baseRadius,
			const std::array<std::array<uint32, 3>, FaceCount>& faces)
		{
			size_t vertexOffset;
			size_t triangleOffset;
			if (not ResizeForAddition(mesh, (FaceCount * 3), FaceCount, vertexOffset, triangleOffset))
			{
				return false;
			}

			const float scale = (radius / baseRadius);
			for (size_t faceIndex = 0; faceIndex < FaceCount; ++faceIndex)
			{
				Float3 p0 = baseVertices[faces[faceIndex][0]];
				Float3 p1 = baseVertices[faces[faceIndex][1]];
				Float3 p2 = baseVertices[faces[faceIndex][2]];
				Float3 normal = (p1 - p0).cross(p2 - p0).normalized();

				if (normal.dot(p0 + p1 + p2) < 0.0f)
				{
					std::swap(p1, p2);
					normal = -normal;
				}

				const Float4 faceTangent{ (p0 - p1).normalized(), 1.0f };
				const size_t vertexBase = (vertexOffset + (faceIndex * 3));
				mesh.vertices[vertexBase + 0] = Vertex3D{
					.pos = (p0 * scale),
					.normal = normal,
					.tex = Float2{ 1.0f, 1.0f },
					.tangent = faceTangent
				};
				mesh.vertices[vertexBase + 1] = Vertex3D{
					.pos = (p1 * scale),
					.normal = normal,
					.tex = Float2{ 0.0f, 1.0f },
					.tangent = faceTangent
				};
				mesh.vertices[vertexBase + 2] = Vertex3D{
					.pos = (p2 * scale),
					.normal = normal,
					.tex = Float2{ 0.5f, 0.0f },
					.tangent = faceTangent
				};

				const uint32 i0 = static_cast<uint32>(vertexBase);
				mesh.indices[triangleOffset + faceIndex] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			}

			return true;
		}

		struct DodecahedronData
		{
			std::array<Float3, 20> vertices;
			std::array<std::array<uint32, 5>, 12> faces;
		};

		[[nodiscard]]
		static const DodecahedronData& GetDodecahedronData()
		{
			static const DodecahedronData data = []
			{
				DodecahedronData result;

				for (size_t faceIndex = 0; faceIndex < IcosahedronFaces.size(); ++faceIndex)
				{
					const auto& face = IcosahedronFaces[faceIndex];
					result.vertices[faceIndex] = (IcosahedronBaseVertices[face[0]]
						+ IcosahedronBaseVertices[face[1]]
						+ IcosahedronBaseVertices[face[2]]).normalized();
				}

				for (uint32 vertexIndex = 0; vertexIndex < IcosahedronBaseVertices.size(); ++vertexIndex)
				{
					const Float3 axis = IcosahedronBaseVertices[vertexIndex].normalized();
					const Float3 helper = ((std::abs(axis.y) < 0.9f) ? Float3::UnitY() : Float3::UnitX());
					const Float3 u = helper.cross(axis).normalized();
					const Float3 v = axis.cross(u);
					std::array<std::pair<float, uint32>, 5> adjacentFaces;
					size_t adjacentCount = 0;

					for (uint32 faceIndex = 0; faceIndex < IcosahedronFaces.size(); ++faceIndex)
					{
						const auto& face = IcosahedronFaces[faceIndex];
						if ((face[0] == vertexIndex)
							|| (face[1] == vertexIndex)
							|| (face[2] == vertexIndex))
						{
							const Float3 center = result.vertices[faceIndex];
							adjacentFaces[adjacentCount++] = {
								std::atan2(center.dot(v), center.dot(u)), faceIndex
							};
						}
					}

					std::sort(adjacentFaces.begin(), adjacentFaces.end());
					for (size_t i = 0; i < adjacentFaces.size(); ++i)
					{
						result.faces[vertexIndex][i] = adjacentFaces[i].second;
					}
				}

				return result;
			}();

			return data;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	addBox
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addBox(const Vec3 size)
	{
		return addBox(size, BoxUVMapping{});
	}

	bool Mesh3DBuilder::addBox(const Vec3 size, const Vec3 offset)
	{
		return addBox(size, BoxUVMapping{}, offset);
	}

	bool Mesh3DBuilder::addBox(const Vec3 size, const Vec3 offset, const Quaternion& rotation)
	{
		return addBox(size, BoxUVMapping{}, offset, rotation);
	}

	bool Mesh3DBuilder::addBox(const Vec3 size, const Mat4x4& transform)
	{
		return addBox(size, BoxUVMapping{}, transform);
	}

	bool Mesh3DBuilder::addBox(const Vec3 _size, const BoxUVMapping& uvMapping)
	{
		if (not IsFloatRepresentable(_size))
		{
			return GenerationFailed<bool>("Mesh3D::Box(): size must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return GenerationFailed<bool>("Mesh3D::Box(): Every size component must be positive after conversion to float");
		}

		const Float3 halfSize = (size * 0.5f);
		const std::array<BoxFace, 6> faces =
		{{
			{ { 0.0f, 0.0f, -halfSize.z }, { size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, -1.0f } },
			{ { 0.0f, 0.0f,  halfSize.z }, { -size.x, 0.0f, 0.0f }, { 0.0f, -size.y, 0.0f }, { 0.0f, 0.0f, 1.0f } },
			{ {  halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -size.y, 0.0f }, { 1.0f, 0.0f, 0.0f } },
			{ { -halfSize.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, -size.y, 0.0f }, { -1.0f, 0.0f, 0.0f } },
			{ { 0.0f,  halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, -size.z }, { 0.0f, 1.0f, 0.0f } },
			{ { 0.0f, -halfSize.y, 0.0f }, { size.x, 0.0f, 0.0f }, { 0.0f, 0.0f, size.z }, { 0.0f, -1.0f, 0.0f } },
		}};
		const std::array<FloatRect, 6> uvRects =
		{{
			uvMapping.negativeZ,
			uvMapping.positiveZ,
			uvMapping.positiveX,
			uvMapping.negativeX,
			uvMapping.positiveY,
			uvMapping.negativeY,
		}};

		for (const auto& uvRect : uvRects)
		{
			if (not IsFinite(uvRect))
			{
				return GenerationFailed<bool>("Mesh3D::Box(): Every UV rectangle must be finite");
			}
		}

		constexpr size_t AddedVertexCount = 24;
		constexpr size_t AddedTriangleCount = 12;
		const size_t vertexBase = m_mesh.vertices.size();
		const size_t triangleBase = m_mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if ((not CheckedAdd(vertexBase, AddedVertexCount, newVertexCount))
			|| (Mesh3D::MaxVertexCount < newVertexCount)
			|| (not CheckedAdd(triangleBase, AddedTriangleCount, newTriangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::Box(): The generated mesh exceeds the supported size");
		}

		m_mesh.vertices.resize(newVertexCount);
		m_mesh.indices.resize(newTriangleCount);

		for (size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
		{
			const BoxFace& face = faces[faceIndex];
			const Float3 halfU = (face.u * 0.5f);
			const Float3 halfV = (face.v * 0.5f);
			const FloatRect uvRect = uvRects[faceIndex];
			const float uSign = ((uvRect.right < uvRect.left) ? -1.0f : 1.0f);
			const float vSign = ((uvRect.bottom < uvRect.top) ? -1.0f : 1.0f);
			const Float3 tangent = (face.u.normalized() * uSign);
			const Float4 tangentFrame{ tangent, (uSign * vSign) };
			const size_t vertexOffset = (vertexBase + faceIndex * 4);

			m_mesh.vertices[vertexOffset + 0] = Vertex3D{
				.pos = (face.center - halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.left, uvRect.top },
				.tangent = tangentFrame
			};
			m_mesh.vertices[vertexOffset + 1] = Vertex3D{
				.pos = (face.center + halfU - halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.right, uvRect.top },
				.tangent = tangentFrame
			};
			m_mesh.vertices[vertexOffset + 2] = Vertex3D{
				.pos = (face.center - halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.left, uvRect.bottom },
				.tangent = tangentFrame
			};
			m_mesh.vertices[vertexOffset + 3] = Vertex3D{
				.pos = (face.center + halfU + halfV),
				.normal = face.normal,
				.tex = Float2{ uvRect.right, uvRect.bottom },
				.tangent = tangentFrame
			};

			const uint32 i0 = static_cast<uint32>(vertexOffset);
			const size_t triangleOffset = (triangleBase + faceIndex * 2);
			m_mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			m_mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
		}

		return true;
	}

	bool Mesh3DBuilder::addBox(const Vec3 size, const BoxUVMapping& uvMapping, const Vec3 offset)
	{
		return addBox(size, uvMapping, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addBox(size, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addBox(size, uvMapping))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRoundedBox
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions)
	{
		return addRoundedBox(size, radius, subdivisions, BoxUVMapping{});
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const Vec3 offset)
	{
		return addRoundedBox(size, radius, subdivisions, BoxUVMapping{}, offset);
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addRoundedBox(size, radius, subdivisions, BoxUVMapping{}, offset, rotation);
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const Mat4x4& transform)
	{
		return addRoundedBox(size, radius, subdivisions, BoxUVMapping{}, transform);
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 _size,
		const double _radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFloatRepresentable(_radius))
			|| (not IsFinite(uvMapping))
			|| (_radius < 0.0)
			|| (subdivisions == 0))
		{
			return GenerationFailed<bool>("Mesh3D::RoundedBox(): The size, radius, subdivisions, or UV mapping is invalid");
		}

		const Float3 size = _size;
		const float radius = static_cast<float>(_radius);
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return GenerationFailed<bool>("Mesh3D::RoundedBox(): Every size component must be positive after conversion to float");
		}

		const double maxRadius = (std::min({ _size.x, _size.y, _size.z }) * 0.5);
		if (maxRadius < _radius)
		{
			return GenerationFailed<bool>("Mesh3D::RoundedBox(): radius must not exceed half of the smallest size component");
		}

		if (radius == 0.0f)
		{
			return addBox(size, uvMapping);
		}

		const Float3 halfSize = (size * 0.5f);
		const Float3 innerHalfSize{
			std::max((halfSize.x - radius), 0.0f),
			std::max((halfSize.y - radius), 0.0f),
			std::max((halfSize.z - radius), 0.0f)
		};
		size_t twiceSubdivisions;
		if (not CheckedMultiply(static_cast<size_t>(subdivisions), 2, twiceSubdivisions))
		{
			return GenerationFailed<bool>("Mesh3D::RoundedBox(): subdivisions exceed the supported range");
		}

		const auto getAxisPointCount = [twiceSubdivisions](const float innerHalfExtent) -> size_t
		{
			return (twiceSubdivisions + ((innerHalfExtent == 0.0f) ? 1 : 2));
		};
		const size_t xPointCount = getAxisPointCount(innerHalfSize.x);
		const size_t yPointCount = getAxisPointCount(innerHalfSize.y);
		const size_t zPointCount = getAxisPointCount(innerHalfSize.z);
		size_t xyPointCount;
		size_t xzPointCount;
		size_t yzPointCount;
		size_t pointCountSum;
		size_t addedVertexCount;
		size_t xyCellCount;
		size_t xzCellCount;
		size_t yzCellCount;
		size_t cellCountSum;
		size_t addedTriangleCount;
		if ((not CheckedMultiply(xPointCount, yPointCount, xyPointCount))
			|| (not CheckedMultiply(xPointCount, zPointCount, xzPointCount))
			|| (not CheckedMultiply(yPointCount, zPointCount, yzPointCount))
			|| (not CheckedAdd(xyPointCount, xzPointCount, pointCountSum))
			|| (not CheckedAdd(pointCountSum, yzPointCount, pointCountSum))
			|| (not CheckedMultiply(pointCountSum, 2, addedVertexCount))
			|| (not CheckedMultiply((xPointCount - 1), (yPointCount - 1), xyCellCount))
			|| (not CheckedMultiply((xPointCount - 1), (zPointCount - 1), xzCellCount))
			|| (not CheckedMultiply((yPointCount - 1), (zPointCount - 1), yzCellCount))
			|| (not CheckedAdd(xyCellCount, xzCellCount, cellCountSum))
			|| (not CheckedAdd(cellCountSum, yzCellCount, cellCountSum))
			|| (not CheckedMultiply(cellCountSum, 4, addedTriangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::RoundedBox(): The generated mesh exceeds the supported size");
		}

		const size_t vertexBase = m_mesh.vertices.size();
		const size_t triangleBase = m_mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if ((not CheckedAdd(vertexBase, addedVertexCount, newVertexCount))
			|| (Mesh3D::MaxVertexCount < newVertexCount)
			|| (not CheckedAdd(triangleBase, addedTriangleCount, newTriangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::RoundedBox(): The generated mesh exceeds the supported size");
		}

		const size_t roundedSegmentCount = static_cast<size_t>(subdivisions);
		Array<float> roundedOffsets(roundedSegmentCount + 1);
		const float angleStep = (Math::QuarterPiF / static_cast<float>(subdivisions));
		roundedOffsets[0] = 0.0f;
		for (size_t i = 1; i < roundedSegmentCount; ++i)
		{
			roundedOffsets[i] = (radius * std::tan(angleStep * static_cast<float>(i)));
		}
		roundedOffsets[roundedSegmentCount] = radius;

		const auto makeAxisSamples = [&roundedOffsets, roundedSegmentCount](
			const float halfExtent,
			const float innerHalfExtent)
		{
			const size_t pointCount = ((roundedSegmentCount * 2)
				+ ((innerHalfExtent == 0.0f) ? 1 : 2));
			Array<float> samples(pointCount);

			for (size_t i = 0; i <= roundedSegmentCount; ++i)
			{
				samples[i] = (-innerHalfExtent - roundedOffsets[roundedSegmentCount - i]);
			}
			samples[0] = -halfExtent;

			if (innerHalfExtent == 0.0f)
			{
				for (size_t i = 1; i <= roundedSegmentCount; ++i)
				{
					const size_t index = (roundedSegmentCount + i);
					samples[index] = ((i == roundedSegmentCount)
						? halfExtent
						: roundedOffsets[i]);
				}
			}
			else
			{
				samples[roundedSegmentCount + 1] = innerHalfExtent;
				for (size_t i = 1; i <= roundedSegmentCount; ++i)
				{
					const size_t index = (roundedSegmentCount + 1 + i);
					samples[index] = ((i == roundedSegmentCount)
						? halfExtent
						: (innerHalfExtent + roundedOffsets[i]));
				}
			}

			return samples;
		};

		const Array<float> xSamples = makeAxisSamples(halfSize.x, innerHalfSize.x);
		const Array<float> ySamples = makeAxisSamples(halfSize.y, innerHalfSize.y);
		const Array<float> zSamples = makeAxisSamples(halfSize.z, innerHalfSize.z);
		m_mesh.vertices.resize(newVertexCount);
		m_mesh.indices.resize(newTriangleCount);
		size_t vertexOffset = vertexBase;
		size_t triangleOffset = triangleBase;

		const auto writeFace = [
			this, &vertexOffset, &triangleOffset, &innerHalfSize, radius](
			const Float3 center,
			const Float3 uAxis,
			const Float3 vAxis,
			const Array<float>& uSamples,
			const Array<float>& vSamples,
			const float uSize,
			const float vSize,
			const FloatRect& uvRect)
		{
			const size_t rowStride = uSamples.size();

			for (size_t v = 0; v < vSamples.size(); ++v)
			{
				for (size_t u = 0; u < uSamples.size(); ++u)
				{
					const Float3 cubePoint = (center
						+ (uAxis * uSamples[u])
						+ (vAxis * vSamples[v]));
					const Float3 innerPoint{
						std::clamp(cubePoint.x, -innerHalfSize.x, innerHalfSize.x),
						std::clamp(cubePoint.y, -innerHalfSize.y, innerHalfSize.y),
						std::clamp(cubePoint.z, -innerHalfSize.z, innerHalfSize.z)
					};
					const Float3 normal = (cubePoint - innerPoint).normalized();
					const Float3 position = (innerPoint + (normal * radius));
					const Float3 baseTangent = (uAxis - (normal * normal.dot(uAxis))).normalized();
					const Float2 projectedUV{
						(0.5f + (position.dot(uAxis) / uSize)),
						(0.5f + (position.dot(vAxis) / vSize))
					};
					m_mesh.vertices[vertexOffset + (v * rowStride) + u] = Vertex3D{
						.pos = position,
						.normal = normal,
						.tex = MapProjectedUV(uvRect, projectedUV),
						.tangent = MakeProjectedTangent(baseTangent, uvRect)
					};
				}
			}

			for (size_t v = 0; (v + 1) < vSamples.size(); ++v)
			{
				for (size_t u = 0; (u + 1) < uSamples.size(); ++u)
				{
					const uint32 i0 = static_cast<uint32>(vertexOffset + (v * rowStride) + u);
					const uint32 nextRow = (i0 + static_cast<uint32>(rowStride));
					m_mesh.indices[triangleOffset++] = TriangleIndex32{ i0, (i0 + 1), nextRow };
					m_mesh.indices[triangleOffset++] = TriangleIndex32{ nextRow, (i0 + 1), (nextRow + 1) };
				}
			}

			vertexOffset += (uSamples.size() * vSamples.size());
		};

		writeFace({ 0.0f, 0.0f, -halfSize.z },
			Float3::UnitX(), -Float3::UnitY(), xSamples, ySamples,
			size.x, size.y, uvMapping.negativeZ);
		writeFace({ 0.0f, 0.0f, halfSize.z },
			-Float3::UnitX(), -Float3::UnitY(), xSamples, ySamples,
			size.x, size.y, uvMapping.positiveZ);
		writeFace({ halfSize.x, 0.0f, 0.0f },
			Float3::UnitZ(), -Float3::UnitY(), zSamples, ySamples,
			size.z, size.y, uvMapping.positiveX);
		writeFace({ -halfSize.x, 0.0f, 0.0f },
			-Float3::UnitZ(), -Float3::UnitY(), zSamples, ySamples,
			size.z, size.y, uvMapping.negativeX);
		writeFace({ 0.0f, halfSize.y, 0.0f },
			Float3::UnitX(), -Float3::UnitZ(), xSamples, zSamples,
			size.x, size.z, uvMapping.positiveY);
		writeFace({ 0.0f, -halfSize.y, 0.0f },
			Float3::UnitX(), Float3::UnitZ(), xSamples, zSamples,
			size.x, size.z, uvMapping.negativeY);

		return true;
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping,
		const Vec3 offset)
	{
		return addRoundedBox(size, radius, subdivisions, uvMapping,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addRoundedBox(size, radius, subdivisions, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addRoundedBox(
		const Vec3 size,
		const double radius,
		const uint32 subdivisions,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addRoundedBox(size, radius, subdivisions, uvMapping))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addWedge
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addWedge(const Vec3 size)
	{
		return addWedge(size, BoxUVMapping{});
	}

	bool Mesh3DBuilder::addWedge(const Vec3 size, const Vec3 offset)
	{
		return addWedge(size, BoxUVMapping{}, offset);
	}

	bool Mesh3DBuilder::addWedge(const Vec3 size, const Vec3 offset, const Quaternion& rotation)
	{
		return addWedge(size, BoxUVMapping{}, offset, rotation);
	}

	bool Mesh3DBuilder::addWedge(const Vec3 size, const Mat4x4& transform)
	{
		return addWedge(size, BoxUVMapping{}, transform);
	}

	bool Mesh3DBuilder::addWedge(const Vec3 _size, const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFinite(uvMapping)))
		{
			return GenerationFailed<bool>("Mesh3D::Wedge(): size and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return GenerationFailed<bool>("Mesh3D::Wedge(): Every size component must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(m_mesh, 18, 8, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::Wedge(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const float left = -halfSize.x;
		const float right = halfSize.x;
		const float bottom = -halfSize.y;
		const float top = halfSize.y;
		const float front = -halfSize.z;
		const float back = halfSize.z;
		const double inverseSlopeLength = (1.0 / std::hypot(_size.y, _size.z));
		const Float3 slopeNormal{
			0.0f,
			static_cast<float>(_size.z * inverseSlopeLength),
			static_cast<float>(-_size.y * inverseSlopeLength)
		};

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ right, top, back }, { left, top, back },
				{ right, bottom, back }, { left, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			Float3::UnitZ(), -Float3::UnitX(), uvMapping.positiveZ);

		WriteProjectedTriangle(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ right, bottom, front },
				{ right, top, back },
				{ right, bottom, back }
			}},
			{{ { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }},
			Float3::UnitX(), Float3::UnitZ(), uvMapping.positiveX);

		WriteProjectedTriangle(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front },
				{ left, bottom, back },
				{ left, top, back }
			}},
			{{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f } }},
			-Float3::UnitX(), -Float3::UnitZ(), uvMapping.negativeX);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, top, back }, { right, top, back },
				{ left, bottom, front }, { right, bottom, front }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			slopeNormal, Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front }, { right, bottom, front },
				{ left, bottom, back }, { right, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		return true;
	}

	bool Mesh3DBuilder::addWedge(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset)
	{
		return addWedge(size, uvMapping, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addWedge(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addWedge(size, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addWedge(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addWedge(size, uvMapping))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTriangularPrism
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addTriangularPrism(const Vec3 size)
	{
		return addTriangularPrism(size, BoxUVMapping{});
	}

	bool Mesh3DBuilder::addTriangularPrism(const Vec3 size, const Vec3 offset)
	{
		return addTriangularPrism(size, BoxUVMapping{}, offset);
	}

	bool Mesh3DBuilder::addTriangularPrism(
		const Vec3 size,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addTriangularPrism(size, BoxUVMapping{}, offset, rotation);
	}

	bool Mesh3DBuilder::addTriangularPrism(const Vec3 size, const Mat4x4& transform)
	{
		return addTriangularPrism(size, BoxUVMapping{}, transform);
	}

	bool Mesh3DBuilder::addTriangularPrism(const Vec3 _size, const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFinite(uvMapping)))
		{
			return GenerationFailed<bool>("Mesh3D::TriangularPrism(): size and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f))
		{
			return GenerationFailed<bool>("Mesh3D::TriangularPrism(): Every size component must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(m_mesh, 18, 8, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::TriangularPrism(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const float left = -halfSize.x;
		const float right = halfSize.x;
		const float bottom = -halfSize.y;
		const float top = halfSize.y;
		const float front = -halfSize.z;
		const float back = halfSize.z;
		const float inverseSlopeLength = static_cast<float>(1.0 / std::hypot(_size.y, (_size.z * 0.5)));
		const Float3 frontSlopeNormal{ 0.0f, (halfSize.z * inverseSlopeLength), (-size.y * inverseSlopeLength) };
		const Float3 backSlopeNormal{ 0.0f, (halfSize.z * inverseSlopeLength), (size.y * inverseSlopeLength) };

		WriteProjectedTriangle(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ right, bottom, back },
				{ right, bottom, front },
				{ right, top, 0.0f }
			}},
			{{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.5f, 0.0f } }},
			Float3::UnitX(), Float3::UnitZ(), uvMapping.positiveX);

		WriteProjectedTriangle(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front },
				{ left, bottom, back },
				{ left, top, 0.0f }
			}},
			{{ { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.5f, 0.0f } }},
			-Float3::UnitX(), -Float3::UnitZ(), uvMapping.negativeX);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, top, 0.0f }, { right, top, 0.0f },
				{ left, bottom, front }, { right, bottom, front }
			}},
			{{ { 0.0f, 0.5f }, { 1.0f, 0.5f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			frontSlopeNormal, Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ right, top, 0.0f }, { left, top, 0.0f },
				{ right, bottom, back }, { left, bottom, back }
			}},
			{{ { 1.0f, 0.5f }, { 0.0f, 0.5f }, { 1.0f, 0.0f }, { 0.0f, 0.0f } }},
			backSlopeNormal, Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front }, { right, bottom, front },
				{ left, bottom, back }, { right, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		return true;
	}

	bool Mesh3DBuilder::addTriangularPrism(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset)
	{
		return addTriangularPrism(size, uvMapping, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addTriangularPrism(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addTriangularPrism(size, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addTriangularPrism(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addTriangularPrism(size, uvMapping))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addStairs
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addStairs(const Vec3 size, const uint32 steps)
	{
		return addStairs(size, steps, BoxUVMapping{});
	}

	bool Mesh3DBuilder::addStairs(const Vec3 size, const uint32 steps, const Vec3 offset)
	{
		return addStairs(size, steps, BoxUVMapping{}, offset);
	}

	bool Mesh3DBuilder::addStairs(
		const Vec3 size,
		const uint32 steps,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addStairs(size, steps, BoxUVMapping{}, offset, rotation);
	}

	bool Mesh3DBuilder::addStairs(const Vec3 size, const uint32 steps, const Mat4x4& transform)
	{
		return addStairs(size, steps, BoxUVMapping{}, transform);
	}

	bool Mesh3DBuilder::addStairs(
		const Vec3 _size,
		const uint32 steps,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_size))
			|| (not IsFinite(uvMapping)))
		{
			return GenerationFailed<bool>("Mesh3D::Stairs(): size and UV mapping must be finite and float-representable");
		}

		const Float3 size = _size;
		if ((size.x <= 0.0f)
			|| (size.y <= 0.0f)
			|| (size.z <= 0.0f)
			|| (steps == 0))
		{
			return GenerationFailed<bool>("Mesh3D::Stairs(): Every size component and the step count must be positive");
		}

		size_t addedVertexCount;
		size_t addedTriangleCount;
		if ((not CheckedMultiply(static_cast<size_t>(steps), 16, addedVertexCount))
			|| (not CheckedAdd(addedVertexCount, 8, addedVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(steps), 8, addedTriangleCount))
			|| (not CheckedAdd(addedTriangleCount, 4, addedTriangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::Stairs(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, addedVertexCount, addedTriangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::Stairs(): The generated mesh exceeds the supported size");
		}

		const Float3 halfSize = (size * 0.5f);
		const float left = -halfSize.x;
		const float right = halfSize.x;
		const float bottom = -halfSize.y;
		const float top = halfSize.y;
		const float front = -halfSize.z;
		const float back = halfSize.z;
		const float inverseSteps = (1.0f / static_cast<float>(steps));

		for (uint32 i = 0; i < steps; ++i)
		{
			const float t0 = (i * inverseSteps);
			const float t1 = ((i + 1) * inverseSteps);
			const float y0 = (bottom + (size.y * t0));
			const float y1 = ((i + 1 == steps) ? top : (bottom + (size.y * t1)));
			const float z0 = (front + (size.z * t0));
			const float z1 = ((i + 1 == steps) ? back : (front + (size.z * t1)));

			WriteProjectedQuad(
				m_mesh, vertexOffset, triangleOffset,
				{{
					{ left, y1, z1 }, { right, y1, z1 },
					{ left, y1, z0 }, { right, y1, z0 }
				}},
				{{ { 0.0f, (1.0f - t1) }, { 1.0f, (1.0f - t1) }, { 0.0f, (1.0f - t0) }, { 1.0f, (1.0f - t0) } }},
				Float3::UnitY(), Float3::UnitX(), uvMapping.positiveY);

			WriteProjectedQuad(
				m_mesh, vertexOffset, triangleOffset,
				{{
					{ left, y1, z0 }, { right, y1, z0 },
					{ left, y0, z0 }, { right, y0, z0 }
				}},
				{{ { 0.0f, (1.0f - t1) }, { 1.0f, (1.0f - t1) }, { 0.0f, (1.0f - t0) }, { 1.0f, (1.0f - t0) } }},
				-Float3::UnitZ(), Float3::UnitX(), uvMapping.negativeZ);
		}

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ right, top, back }, { left, top, back },
				{ right, bottom, back }, { left, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			Float3::UnitZ(), -Float3::UnitX(), uvMapping.positiveZ);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ left, bottom, front }, { right, bottom, front },
				{ left, bottom, back }, { right, bottom, back }
			}},
			{{ { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		for (uint32 i = 0; i < steps; ++i)
		{
			const float t0 = (i * inverseSteps);
			const float t1 = ((i + 1) * inverseSteps);
			const float y1 = ((i + 1 == steps) ? top : (bottom + (size.y * t1)));
			const float z0 = (front + (size.z * t0));
			const float z1 = ((i + 1 == steps) ? back : (front + (size.z * t1)));

			WriteProjectedQuad(
				m_mesh, vertexOffset, triangleOffset,
				{{
					{ right, y1, z0 }, { right, y1, z1 },
					{ right, bottom, z0 }, { right, bottom, z1 }
				}},
				{{ { t0, (1.0f - t1) }, { t1, (1.0f - t1) }, { t0, 1.0f }, { t1, 1.0f } }},
				Float3::UnitX(), Float3::UnitZ(), uvMapping.positiveX);

			WriteProjectedQuad(
				m_mesh, vertexOffset, triangleOffset,
				{{
					{ left, y1, z1 }, { left, y1, z0 },
					{ left, bottom, z1 }, { left, bottom, z0 }
				}},
				{{ { (1.0f - t1), (1.0f - t1) }, { (1.0f - t0), (1.0f - t1) }, { (1.0f - t1), 1.0f }, { (1.0f - t0), 1.0f } }},
				-Float3::UnitX(), -Float3::UnitZ(), uvMapping.negativeX);
		}

		return true;
	}

	bool Mesh3DBuilder::addStairs(
		const Vec3 size,
		const uint32 steps,
		const BoxUVMapping& uvMapping,
		const Vec3 offset)
	{
		return addStairs(size, steps, uvMapping, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addStairs(
		const Vec3 size,
		const uint32 steps,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addStairs(size, steps, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addStairs(
		const Vec3 size,
		const uint32 steps,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addStairs(size, steps, uvMapping))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addPyramid
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addPyramid(const double baseSize, const double height)
	{
		return addPyramid(SizeF{ baseSize, baseSize }, height);
	}

	bool Mesh3DBuilder::addPyramid(
		const double baseSize,
		const double height,
		const Vec3 offset)
	{
		return addPyramid(SizeF{ baseSize, baseSize }, height, offset);
	}

	bool Mesh3DBuilder::addPyramid(
		const double baseSize,
		const double height,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addPyramid(SizeF{ baseSize, baseSize }, height, offset, rotation);
	}

	bool Mesh3DBuilder::addPyramid(
		const double baseSize,
		const double height,
		const Mat4x4& transform)
	{
		return addPyramid(SizeF{ baseSize, baseSize }, height, transform);
	}

	bool Mesh3DBuilder::addPyramid(const SizeF _baseSizeXZ, const double _height)
	{
		if ((not IsFloatRepresentable(_baseSizeXZ))
			|| (not IsFloatRepresentable(_height)))
		{
			return GenerationFailed<bool>("Mesh3D::Pyramid(): baseSizeXZ and height must be finite and float-representable");
		}

		const Float2 baseSizeXZ = _baseSizeXZ;
		const float height = static_cast<float>(_height);
		if ((baseSizeXZ.x <= 0.0f)
			|| (baseSizeXZ.y <= 0.0f)
			|| (height <= 0.0f))
		{
			return GenerationFailed<bool>("Mesh3D::Pyramid(): Every base size component and height must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(m_mesh, 16, 6, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::Pyramid(): The generated mesh exceeds the supported size");
		}

		const float halfX = (baseSizeXZ.x * 0.5f);
		const float halfZ = (baseSizeXZ.y * 0.5f);
		const float halfHeight = (height * 0.5f);
		const Float3 apex{ 0.0f, halfHeight, 0.0f };
		const float inverseZSideLength = (1.0f / std::hypot(height, halfZ));
		const float inverseXSideLength = (1.0f / std::hypot(height, halfX));

		struct Side
		{
			Float3 firstBase;
			Float3 secondBase;
			Float3 normal;
			Float3 tangent;
		};

		const std::array<Side, 4> sides =
		{{
			{
				{ halfX, -halfHeight, -halfZ },
				{ -halfX, -halfHeight, -halfZ },
				(Float3{ 0.0f, halfZ, -height } * inverseZSideLength),
				Float3::UnitX()
			},
			{
				{ -halfX, -halfHeight, halfZ },
				{ halfX, -halfHeight, halfZ },
				(Float3{ 0.0f, halfZ, height } * inverseZSideLength),
				-Float3::UnitX()
			},
			{
				{ halfX, -halfHeight, halfZ },
				{ halfX, -halfHeight, -halfZ },
				(Float3{ height, halfX, 0.0f } * inverseXSideLength),
				Float3::UnitZ()
			},
			{
				{ -halfX, -halfHeight, -halfZ },
				{ -halfX, -halfHeight, halfZ },
				(Float3{ -height, halfX, 0.0f } * inverseXSideLength),
				-Float3::UnitZ()
			},
		}};

		for (size_t faceIndex = 0; faceIndex < sides.size(); ++faceIndex)
		{
			const Side& side = sides[faceIndex];
			const Float4 tangent{ side.tangent, 1.0f };
			const size_t vertexBase = (vertexOffset + (faceIndex * 3));
			m_mesh.vertices[vertexBase + 0] = Vertex3D{
				.pos = side.firstBase,
				.normal = side.normal,
				.tex = Float2{ 1.0f, 1.0f },
				.tangent = tangent
			};
			m_mesh.vertices[vertexBase + 1] = Vertex3D{
				.pos = side.secondBase,
				.normal = side.normal,
				.tex = Float2{ 0.0f, 1.0f },
				.tangent = tangent
			};
			m_mesh.vertices[vertexBase + 2] = Vertex3D{
				.pos = apex,
				.normal = side.normal,
				.tex = Float2{ 0.5f, 0.0f },
				.tangent = tangent
			};

			const uint32 i0 = static_cast<uint32>(vertexBase);
			m_mesh.indices[triangleOffset + faceIndex] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
		}

		const size_t bottomVertexBase = (vertexOffset + 12);
		const Float3 bottomNormal = -Float3::UnitY();
		const Float4 bottomTangent{ 1.0f, 0.0f, 0.0f, 1.0f };
		m_mesh.vertices[bottomVertexBase + 0] = Vertex3D{
			.pos = Float3{ -halfX, -halfHeight, -halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 0.0f, 0.0f },
			.tangent = bottomTangent
		};
		m_mesh.vertices[bottomVertexBase + 1] = Vertex3D{
			.pos = Float3{ halfX, -halfHeight, -halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 1.0f, 0.0f },
			.tangent = bottomTangent
		};
		m_mesh.vertices[bottomVertexBase + 2] = Vertex3D{
			.pos = Float3{ -halfX, -halfHeight, halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 0.0f, 1.0f },
			.tangent = bottomTangent
		};
		m_mesh.vertices[bottomVertexBase + 3] = Vertex3D{
			.pos = Float3{ halfX, -halfHeight, halfZ },
			.normal = bottomNormal,
			.tex = Float2{ 1.0f, 1.0f },
			.tangent = bottomTangent
		};

		const uint32 bottomIndexBase = static_cast<uint32>(bottomVertexBase);
		m_mesh.indices[triangleOffset + 4] = TriangleIndex32{
			bottomIndexBase, (bottomIndexBase + 1), (bottomIndexBase + 2) };
		m_mesh.indices[triangleOffset + 5] = TriangleIndex32{
			(bottomIndexBase + 2), (bottomIndexBase + 1), (bottomIndexBase + 3) };
		return true;
	}

	bool Mesh3DBuilder::addPyramid(
		const SizeF baseSizeXZ,
		const double height,
		const Vec3 offset)
	{
		return addPyramid(baseSizeXZ, height, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addPyramid(
		const SizeF baseSizeXZ,
		const double height,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addPyramid(baseSizeXZ, height,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addPyramid(
		const SizeF baseSizeXZ,
		const double height,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addPyramid(baseSizeXZ, height))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRectangularFrustum
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height)
	{
		return addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, BoxUVMapping{});
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const Vec3 offset)
	{
		return addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, BoxUVMapping{}, offset);
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addRectangularFrustum(
			bottomSizeXZ, topSizeXZ, height, BoxUVMapping{}, offset, rotation);
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const Mat4x4& transform)
	{
		return addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, BoxUVMapping{}, transform);
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF _bottomSizeXZ,
		const SizeF _topSizeXZ,
		const double _height,
		const BoxUVMapping& uvMapping)
	{
		if ((not IsFloatRepresentable(_bottomSizeXZ))
			|| (not IsFloatRepresentable(_topSizeXZ))
			|| (not IsFloatRepresentable(_height))
			|| (not IsFinite(uvMapping)))
		{
			return GenerationFailed<bool>("Mesh3D::RectangularFrustum(): The sizes, height, and UV mapping must be finite and float-representable");
		}

		const Float2 bottomSizeXZ = _bottomSizeXZ;
		const Float2 topSizeXZ = _topSizeXZ;
		const float height = static_cast<float>(_height);
		if ((bottomSizeXZ.x <= 0.0f)
			|| (bottomSizeXZ.y <= 0.0f)
			|| (topSizeXZ.x <= 0.0f)
			|| (topSizeXZ.y <= 0.0f)
			|| (height <= 0.0f))
		{
			return GenerationFailed<bool>("Mesh3D::RectangularFrustum(): Every size component and height must be positive after conversion to float");
		}

		if (bottomSizeXZ == topSizeXZ)
		{
			return addBox(Vec3{ bottomSizeXZ.x, height, bottomSizeXZ.y }, uvMapping);
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(m_mesh, 24, 12, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::RectangularFrustum(): The generated mesh exceeds the supported size");
		}

		const Float2 bottomHalf = (bottomSizeXZ * 0.5f);
		const Float2 topHalf = (topSizeXZ * 0.5f);
		const Float2 boundsHalf{
			std::max(bottomHalf.x, topHalf.x),
			std::max(bottomHalf.y, topHalf.y)
		};
		const float halfHeight = (height * 0.5f);
		const float xDelta = (bottomHalf.x - topHalf.x);
		const float zDelta = (bottomHalf.y - topHalf.y);
		const float inverseXSideLength = static_cast<float>(1.0 / std::hypot(_height, static_cast<double>(xDelta)));
		const float inverseZSideLength = static_cast<float>(1.0 / std::hypot(_height, static_cast<double>(zDelta)));
		const Float3 negativeZNormal{ 0.0f, (zDelta * inverseZSideLength), (-height * inverseZSideLength) };
		const Float3 positiveZNormal{ 0.0f, (zDelta * inverseZSideLength), (height * inverseZSideLength) };
		const Float3 positiveXNormal{ (height * inverseXSideLength), (xDelta * inverseXSideLength), 0.0f };
		const Float3 negativeXNormal{ (-height * inverseXSideLength), (xDelta * inverseXSideLength), 0.0f };
		const auto mapX = [boundsHalf](const float x) { return (0.5f + (x / (2.0f * boundsHalf.x))); };
		const auto mapZ = [boundsHalf](const float z) { return (0.5f + (z / (2.0f * boundsHalf.y))); };

		const float bottomLeft = -bottomHalf.x;
		const float bottomRight = bottomHalf.x;
		const float bottomFront = -bottomHalf.y;
		const float bottomBack = bottomHalf.y;
		const float topLeft = -topHalf.x;
		const float topRight = topHalf.x;
		const float topFront = -topHalf.y;
		const float topBack = topHalf.y;

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ topLeft, halfHeight, topFront }, { topRight, halfHeight, topFront },
				{ bottomLeft, -halfHeight, bottomFront }, { bottomRight, -halfHeight, bottomFront }
			}},
			{{
				{ mapX(topLeft), 0.0f }, { mapX(topRight), 0.0f },
				{ mapX(bottomLeft), 1.0f }, { mapX(bottomRight), 1.0f }
			}},
			negativeZNormal, Float3::UnitX(), uvMapping.negativeZ);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ topRight, halfHeight, topBack }, { topLeft, halfHeight, topBack },
				{ bottomRight, -halfHeight, bottomBack }, { bottomLeft, -halfHeight, bottomBack }
			}},
			{{
				{ (1.0f - mapX(topRight)), 0.0f }, { (1.0f - mapX(topLeft)), 0.0f },
				{ (1.0f - mapX(bottomRight)), 1.0f }, { (1.0f - mapX(bottomLeft)), 1.0f }
			}},
			positiveZNormal, -Float3::UnitX(), uvMapping.positiveZ);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ topRight, halfHeight, topFront }, { topRight, halfHeight, topBack },
				{ bottomRight, -halfHeight, bottomFront }, { bottomRight, -halfHeight, bottomBack }
			}},
			{{
				{ mapZ(topFront), 0.0f }, { mapZ(topBack), 0.0f },
				{ mapZ(bottomFront), 1.0f }, { mapZ(bottomBack), 1.0f }
			}},
			positiveXNormal, Float3::UnitZ(), uvMapping.positiveX);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ topLeft, halfHeight, topBack }, { topLeft, halfHeight, topFront },
				{ bottomLeft, -halfHeight, bottomBack }, { bottomLeft, -halfHeight, bottomFront }
			}},
			{{
				{ (1.0f - mapZ(topBack)), 0.0f }, { (1.0f - mapZ(topFront)), 0.0f },
				{ (1.0f - mapZ(bottomBack)), 1.0f }, { (1.0f - mapZ(bottomFront)), 1.0f }
			}},
			negativeXNormal, -Float3::UnitZ(), uvMapping.negativeX);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ topLeft, halfHeight, topBack }, { topRight, halfHeight, topBack },
				{ topLeft, halfHeight, topFront }, { topRight, halfHeight, topFront }
			}},
			{{
				{ mapX(topLeft), (1.0f - mapZ(topBack)) }, { mapX(topRight), (1.0f - mapZ(topBack)) },
				{ mapX(topLeft), (1.0f - mapZ(topFront)) }, { mapX(topRight), (1.0f - mapZ(topFront)) }
			}},
			Float3::UnitY(), Float3::UnitX(), uvMapping.positiveY);

		WriteProjectedQuad(
			m_mesh, vertexOffset, triangleOffset,
			{{
				{ bottomLeft, -halfHeight, bottomFront }, { bottomRight, -halfHeight, bottomFront },
				{ bottomLeft, -halfHeight, bottomBack }, { bottomRight, -halfHeight, bottomBack }
			}},
			{{
				{ mapX(bottomLeft), mapZ(bottomFront) }, { mapX(bottomRight), mapZ(bottomFront) },
				{ mapX(bottomLeft), mapZ(bottomBack) }, { mapX(bottomRight), mapZ(bottomBack) }
			}},
			-Float3::UnitY(), Float3::UnitX(), uvMapping.negativeY);

		return true;
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const BoxUVMapping& uvMapping,
		const Vec3 offset)
	{
		return addRectangularFrustum(
			bottomSizeXZ, topSizeXZ, height, uvMapping, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addRectangularFrustum(
		const SizeF bottomSizeXZ,
		const SizeF topSizeXZ,
		const double height,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addRectangularFrustum(bottomSizeXZ, topSizeXZ, height, uvMapping))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTetrahedron
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addTetrahedron(const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed<bool>("Mesh3D::Tetrahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return GenerationFailed<bool>("Mesh3D::Tetrahedron(): radius must be positive after conversion to float");
		}

		constexpr std::array<Float3, 4> Vertices =
		{{
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ -1.0f, -1.0f, 1.0f },
		}};
		constexpr std::array<std::array<uint32, 3>, 4> Faces =
		{{
			{ 0, 1, 2 }, { 0, 3, 1 }, { 0, 2, 3 }, { 1, 3, 2 },
		}};

		if (not AddTriangleFacedPolyhedron(
			m_mesh, radius, Vertices, 1.7320508075688772935f, Faces))
		{
			return GenerationFailed<bool>("Mesh3D::Tetrahedron(): The generated mesh exceeds the supported size");
		}

		return true;
	}

	bool Mesh3DBuilder::addTetrahedron(const double radius, const Vec3 offset)
	{
		return addTetrahedron(radius, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addTetrahedron(
		const double radius,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addTetrahedron(radius,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addTetrahedron(const double radius, const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addTetrahedron(radius))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addOctahedron
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addOctahedron(const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed<bool>("Mesh3D::Octahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return GenerationFailed<bool>("Mesh3D::Octahedron(): radius must be positive after conversion to float");
		}

		constexpr std::array<Float3, 6> Vertices =
		{{
			{ 1.0f, 0.0f, 0.0f },
			{ -1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, -1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, -1.0f },
		}};
		constexpr std::array<std::array<uint32, 3>, 8> Faces =
		{{
			{ 2, 4, 0 }, { 2, 1, 4 }, { 2, 5, 1 }, { 2, 0, 5 },
			{ 3, 0, 4 }, { 3, 4, 1 }, { 3, 1, 5 }, { 3, 5, 0 },
		}};

		if (not AddTriangleFacedPolyhedron(m_mesh, radius, Vertices, 1.0f, Faces))
		{
			return GenerationFailed<bool>("Mesh3D::Octahedron(): The generated mesh exceeds the supported size");
		}

		return true;
	}

	bool Mesh3DBuilder::addOctahedron(const double radius, const Vec3 offset)
	{
		return addOctahedron(radius, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addOctahedron(
		const double radius,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addOctahedron(radius,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addOctahedron(const double radius, const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addOctahedron(radius))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addIcosahedron
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addIcosahedron(const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed<bool>("Mesh3D::Icosahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return GenerationFailed<bool>("Mesh3D::Icosahedron(): radius must be positive after conversion to float");
		}

		if (not AddTriangleFacedPolyhedron(
			m_mesh,
			radius,
			IcosahedronBaseVertices,
			1.9021130325903071442f,
			IcosahedronFaces))
		{
			return GenerationFailed<bool>("Mesh3D::Icosahedron(): The generated mesh exceeds the supported size");
		}

		return true;
	}

	bool Mesh3DBuilder::addIcosahedron(const double radius, const Vec3 offset)
	{
		return addIcosahedron(radius, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addIcosahedron(
		const double radius,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addIcosahedron(radius,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addIcosahedron(const double radius, const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addIcosahedron(radius))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addDodecahedron
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addDodecahedron(const double _radius)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed<bool>("Mesh3D::Dodecahedron(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if (radius <= 0.0f)
		{
			return GenerationFailed<bool>("Mesh3D::Dodecahedron(): radius must be positive after conversion to float");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(m_mesh, 60, 36, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::Dodecahedron(): The generated mesh exceeds the supported size");
		}

		const DodecahedronData& data = GetDodecahedronData();
		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);

		for (size_t faceIndex = 0; faceIndex < data.faces.size(); ++faceIndex)
		{
			std::array<uint32, 5> face = data.faces[faceIndex];
			Float3 faceCenter = Float3::Zero();
			for (const uint32 vertexIndex : face)
			{
				faceCenter += data.vertices[vertexIndex];
			}
			faceCenter /= static_cast<float>(face.size());

			Float3 normal = (data.vertices[face[1]] - data.vertices[face[0]])
				.cross(data.vertices[face[2]] - data.vertices[face[0]]).normalized();
			if (normal.dot(faceCenter) < 0.0f)
			{
				std::reverse(face.begin(), face.end());
				normal = -normal;
			}

			const size_t vertexBase = (vertexOffset + (faceIndex * face.size()));
			std::array<Float3, 5> positions;
			for (size_t i = 0; i < positions.size(); ++i)
			{
				positions[i] = data.vertices[face[i]];
			}

			const Float3 tangent = (positions[0] - faceCenter).normalized();
			const Float3 bitangent = normal.cross(tangent);
			const float inverseFaceRadius = (1.0f / positions[0].distanceFrom(faceCenter));

			for (size_t i = 0; i < positions.size(); ++i)
			{
				const Float3 vertexOffsetFromCenter = (positions[i] - faceCenter);
				m_mesh.vertices[vertexBase + i] = Vertex3D{
					.pos = (positions[i] * radius),
					.normal = normal,
					.tex = Float2{
						std::clamp((0.5f + (0.5f * vertexOffsetFromCenter.dot(tangent) * inverseFaceRadius)), 0.0f, 1.0f),
						std::clamp((0.5f + (0.5f * vertexOffsetFromCenter.dot(bitangent) * inverseFaceRadius)), 0.0f, 1.0f)
					},
					.tangent = Float4{ tangent, 1.0f }
				};
			}

			const uint32 i0 = static_cast<uint32>(vertexBase);
			*pTriangle++ = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			*pTriangle++ = TriangleIndex32{ i0, (i0 + 2), (i0 + 3) };
			*pTriangle++ = TriangleIndex32{ i0, (i0 + 3), (i0 + 4) };
		}

		return true;
	}

	bool Mesh3DBuilder::addDodecahedron(const double radius, const Vec3 offset)
	{
		return addDodecahedron(radius, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addDodecahedron(
		const double radius,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addDodecahedron(radius,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addDodecahedron(const double radius, const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addDodecahedron(radius))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}
}
