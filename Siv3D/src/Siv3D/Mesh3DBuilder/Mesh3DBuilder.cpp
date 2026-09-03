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
# include <limits>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::CheckedAdd;
		using Mesh3DDetail::CheckedMultiply;
		using Mesh3DDetail::GenerationFailed;
		using Mesh3DDetail::IsFloatRepresentable;
		using Mesh3DDetail::TransformVertexRange;
		using CircleSample = Mesh3DDetail::CircleSample<float>;

		struct BoxFaceData
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

	bool Mesh3DBuilder::addBox(const Vec3 size, const BoxFace faces)
	{
		return addBox(size, BoxUVMapping{}, faces);
	}

	bool Mesh3DBuilder::addBox(const Vec3 size, const Vec3 offset, const BoxFace faces)
	{
		return addBox(size, BoxUVMapping{}, offset, faces);
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 size,
		const Vec3 offset,
		const Quaternion& rotation,
		const BoxFace faces)
	{
		return addBox(size, BoxUVMapping{}, offset, rotation, faces);
	}

	bool Mesh3DBuilder::addBox(const Vec3 size, const Mat4x4& transform, const BoxFace faces)
	{
		return addBox(size, BoxUVMapping{}, transform, faces);
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 _size,
		const BoxUVMapping& uvMapping,
		const BoxFace selectedFaces)
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

		constexpr uint8 ValidFaceBits = static_cast<uint8>(BoxFace::All);
		const uint8 selectedFaceBits = static_cast<uint8>(selectedFaces);
		if (selectedFaceBits & ~ValidFaceBits)
		{
			return GenerationFailed<bool>("Mesh3D::Box(): faces contains unsupported bits");
		}

		const Float3 halfSize = (size * 0.5f);
		const std::array<BoxFaceData, 6> faces =
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
		constexpr std::array<BoxFace, 6> faceMasks =
		{{
			BoxFace::NegativeZ,
			BoxFace::PositiveZ,
			BoxFace::PositiveX,
			BoxFace::NegativeX,
			BoxFace::PositiveY,
			BoxFace::NegativeY,
		}};

		size_t selectedFaceCount = 0;
		for (size_t faceIndex = 0; faceIndex < faceMasks.size(); ++faceIndex)
		{
			if (not static_cast<bool>(selectedFaces & faceMasks[faceIndex]))
			{
				continue;
			}

			if (not IsFinite(uvRects[faceIndex]))
			{
				return GenerationFailed<bool>("Mesh3D::Box(): Every selected UV rectangle must be finite");
			}

			++selectedFaceCount;
		}

		const size_t addedVertexCount = (selectedFaceCount * 4);
		const size_t addedTriangleCount = (selectedFaceCount * 2);
		const size_t vertexBase = m_mesh.vertices.size();
		const size_t triangleBase = m_mesh.indices.size();
		size_t newVertexCount;
		size_t newTriangleCount;
		if ((not CheckedAdd(vertexBase, addedVertexCount, newVertexCount))
			|| (Mesh3D::MaxVertexCount < newVertexCount)
			|| (not CheckedAdd(triangleBase, addedTriangleCount, newTriangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::Box(): The generated mesh exceeds the supported size");
		}

		m_mesh.vertices.resize(newVertexCount);
		m_mesh.indices.resize(newTriangleCount);

		size_t outputFaceIndex = 0;
		for (size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex)
		{
			if (not static_cast<bool>(selectedFaces & faceMasks[faceIndex]))
			{
				continue;
			}

			const BoxFaceData& face = faces[faceIndex];
			const Float3 halfU = (face.u * 0.5f);
			const Float3 halfV = (face.v * 0.5f);
			const FloatRect uvRect = uvRects[faceIndex];
			const float uSign = ((uvRect.right < uvRect.left) ? -1.0f : 1.0f);
			const float vSign = ((uvRect.bottom < uvRect.top) ? -1.0f : 1.0f);
			const Float3 tangent = (face.u.normalized() * uSign);
			const Float4 tangentFrame{ tangent, (uSign * vSign) };
			const size_t vertexOffset = (vertexBase + outputFaceIndex * 4);

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
			const size_t triangleOffset = (triangleBase + outputFaceIndex * 2);
			m_mesh.indices[triangleOffset + 0] = TriangleIndex32{ i0, (i0 + 1), (i0 + 2) };
			m_mesh.indices[triangleOffset + 1] = TriangleIndex32{ (i0 + 2), (i0 + 1), (i0 + 3) };
			++outputFaceIndex;
		}

		return true;
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const BoxFace faces)
	{
		return addBox(size, uvMapping, Mat4x4::Translate(Float3{ offset }), faces);
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Vec3 offset,
		const Quaternion& rotation,
		const BoxFace faces)
	{
		return addBox(size, uvMapping,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }), faces);
	}

	bool Mesh3DBuilder::addBox(
		const Vec3 size,
		const BoxUVMapping& uvMapping,
		const Mat4x4& transform,
		const BoxFace faces)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addBox(size, uvMapping, faces))
		{
			return false;
		}

		if (vertexOffset < m_mesh.vertices.size())
		{
			TransformAddedVertices(m_mesh, vertexOffset, transform);
		}
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
	//	addExtrude
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addExtrude(const Polygon& polygon, const double height)
	{
		return Mesh3DDetail::AppendExtrude(m_mesh, polygon, height, 0.0);
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const Vec3 offset)
	{
		return addExtrude(polygon, height, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addExtrude(polygon, height,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not Mesh3DDetail::AppendExtrude(m_mesh, polygon, height, 0.0))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const double smoothingAngle)
	{
		return Mesh3DDetail::AppendExtrude(m_mesh, polygon, height, smoothingAngle);
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const double smoothingAngle,
		const Vec3 offset)
	{
		return addExtrude(
			polygon, height, smoothingAngle, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const double smoothingAngle,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addExtrude(polygon, height, smoothingAngle,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addExtrude(
		const Polygon& polygon,
		const double height,
		const double smoothingAngle,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not Mesh3DDetail::AppendExtrude(m_mesh, polygon, height, smoothingAngle))
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

	////////////////////////////////////////////////////////////////
	//
	//	addPlane
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		return addGrid(sizeXZ, 1, 1, uvScale, uvOffset);
	}

	bool Mesh3DBuilder::addPlane(const SizeF sizeXZ, const Vec3 offset)
	{
		return addPlane(sizeXZ, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, offset);
	}

	bool Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addPlane(
			sizeXZ, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, offset, rotation);
	}

	bool Mesh3DBuilder::addPlane(const SizeF sizeXZ, const Mat4x4& transform)
	{
		return addPlane(sizeXZ, Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, transform);
	}

	bool Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Vec3 offset)
	{
		return addPlane(sizeXZ, uvScale, uvOffset, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addPlane(sizeXZ, uvScale, uvOffset,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addPlane(
		const SizeF sizeXZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Mat4x4& transform)
	{
		return addGrid(sizeXZ, 1, 1, uvScale, uvOffset, transform);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addGrid
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addGrid(
		const SizeF _sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 _uvScale,
		const Vec2 _uvOffset)
	{
		if ((not IsFloatRepresentable(_sizeXZ))
			|| (not IsFloatRepresentable(_uvScale))
			|| (not IsFloatRepresentable(_uvOffset)))
		{
			return GenerationFailed<bool>("Mesh3D::Grid()/Plane(): sizeXZ and the UV transform must be finite and float-representable");
		}

		const Float2 sizeXZ = _sizeXZ;
		const Float2 uvScale = _uvScale;
		const Float2 uvOffset = _uvOffset;
		if ((sizeXZ.x <= 0.0f)
			|| (sizeXZ.y <= 0.0f)
			|| (segmentsX == 0)
			|| (segmentsZ == 0))
		{
			return GenerationFailed<bool>("Mesh3D::Grid()/Plane(): Every size component and segment count must be positive");
		}

		size_t columnCount;
		size_t rowCount;
		size_t vertexCount;
		size_t cellCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(segmentsX), 1, columnCount))
			|| (not CheckedAdd(static_cast<size_t>(segmentsZ), 1, rowCount))
			|| (not CheckedMultiply(columnCount, rowCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(segmentsX), static_cast<size_t>(segmentsZ), cellCount))
			|| (not CheckedMultiply(cellCount, 2, triangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::Grid()/Plane(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::Grid()/Plane(): The generated mesh exceeds the supported size");
		}

		const Float2 halfSize = (sizeXZ * 0.5f);
		const float invSegmentsX = (1.0f / static_cast<float>(segmentsX));
		const float invSegmentsZ = (1.0f / static_cast<float>(segmentsZ));
		const Float3 normal = Float3::UnitY();
		const Float4 tangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		for (uint32 z = 0; z <= segmentsZ; ++z)
		{
			const float v = (z * invSegmentsZ);
			const float positionZ = (halfSize.y - (sizeXZ.y * v));

			for (uint32 x = 0; x <= segmentsX; ++x)
			{
				const float u = (x * invSegmentsX);
				const size_t vertexIndex = (vertexOffset
					+ (static_cast<size_t>(z) * columnCount) + x);

				m_mesh.vertices[vertexIndex] = Vertex3D{
					.pos = Float3{ (-halfSize.x + (sizeXZ.x * u)), 0.0f, positionZ },
					.normal = normal,
					.tex = Float2{ (uvOffset.x + (uvScale.x * u)), (uvOffset.y + (uvScale.y * v)) },
					.tangent = tangent
				};
			}
		}

		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
		for (uint32 z = 0; z < segmentsZ; ++z)
		{
			const size_t rowOffset = (vertexOffset + (static_cast<size_t>(z) * columnCount));

			for (uint32 x = 0; x < segmentsX; ++x)
			{
				const uint32 i0 = static_cast<uint32>(rowOffset + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(rowOffset + columnCount + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		return true;
	}

	bool Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec3 offset)
	{
		return addGrid(sizeXZ, segmentsX, segmentsZ,
			Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, offset);
	}

	bool Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addGrid(sizeXZ, segmentsX, segmentsZ,
			Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, offset, rotation);
	}

	bool Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Mat4x4& transform)
	{
		return addGrid(sizeXZ, segmentsX, segmentsZ,
			Vec2{ 1.0, 1.0 }, Vec2{ 0.0, 0.0 }, transform);
	}

	bool Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Vec3 offset)
	{
		return addGrid(sizeXZ, segmentsX, segmentsZ,
			uvScale, uvOffset, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addGrid(sizeXZ, segmentsX, segmentsZ, uvScale, uvOffset,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addGrid(
		const SizeF sizeXZ,
		const uint32 segmentsX,
		const uint32 segmentsZ,
		const Vec2 uvScale,
		const Vec2 uvOffset,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addGrid(sizeXZ, segmentsX, segmentsZ, uvScale, uvOffset))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addDisc
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addDisc(const double radius, const uint32 segments)
	{
		return addAnnulus(0.0, radius, segments);
	}

	bool Mesh3DBuilder::addDisc(
		const double radius,
		const uint32 segments,
		const Vec3 offset)
	{
		return addAnnulus(0.0, radius, segments, offset);
	}

	bool Mesh3DBuilder::addDisc(
		const double radius,
		const uint32 segments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addAnnulus(0.0, radius, segments, offset, rotation);
	}

	bool Mesh3DBuilder::addDisc(
		const double radius,
		const uint32 segments,
		const Mat4x4& transform)
	{
		return addAnnulus(0.0, radius, segments, transform);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addAnnulus
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addAnnulus(
		const double _innerRadius,
		const double _outerRadius,
		const uint32 segments)
	{
		if ((not IsFloatRepresentable(_innerRadius))
			|| (not IsFloatRepresentable(_outerRadius)))
		{
			return GenerationFailed<bool>("Mesh3D::Annulus()/Disc(): The radii must be finite and float-representable");
		}

		const float innerRadius = static_cast<float>(_innerRadius);
		const float outerRadius = static_cast<float>(_outerRadius);
		if ((innerRadius < 0.0f)
			|| (outerRadius <= innerRadius)
			|| (segments < 3))
		{
			return GenerationFailed<bool>("Mesh3D::Annulus()/Disc(): The radii or segment count is invalid");
		}

		const bool isDisc = (innerRadius == 0.0f);
		size_t vertexCount;
		size_t triangleCount;

		if (isDisc)
		{
			if (not CheckedAdd(static_cast<size_t>(segments), 1, vertexCount))
			{
				return GenerationFailed<bool>("Mesh3D::Disc(): The generated vertex count exceeds the supported range");
			}

			triangleCount = segments;
		}
		else
		{
			if ((not CheckedMultiply(static_cast<size_t>(segments), 2, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 2, triangleCount)))
			{
				return GenerationFailed<bool>("Mesh3D::Annulus(): The generated mesh exceeds the supported size");
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return GenerationFailed<bool>("Mesh3D::Annulus()/Disc(): The generated vertex count exceeds the supported range");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::Annulus()/Disc(): The generated mesh exceeds the supported size");
		}

		const Array<CircleSample> circle = Mesh3DDetail::MakeCircleSamples<float>(segments);
		const Float3 normal = Float3::UnitY();
		const Float4 tangent{ 1.0f, 0.0f, 0.0f, 1.0f };

		if (isDisc)
		{
			m_mesh.vertices[vertexOffset] = Vertex3D{
				.pos = Float3::Zero(),
				.normal = normal,
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = tangent
			};

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				m_mesh.vertices[vertexOffset + static_cast<size_t>(i) + 1] = Vertex3D{
					.pos = Float3{ (outerRadius * sample.cos), 0.0f, (outerRadius * sample.sin) },
					.normal = normal,
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) },
					.tangent = tangent
				};

				const uint32 center = static_cast<uint32>(vertexOffset);
				const uint32 current = static_cast<uint32>(vertexOffset + i + 1);
				const uint32 next = static_cast<uint32>(
					vertexOffset + ((i + 1) % segments) + 1);
				m_mesh.indices[triangleOffset + i] = TriangleIndex32{ center, next, current };
			}
		}
		else
		{
			const size_t outerRingBase = vertexOffset;
			const size_t innerRingBase = (vertexOffset + segments);
			const float innerUVScale = (0.5f * innerRadius / outerRadius);

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				m_mesh.vertices[outerRingBase + i] = Vertex3D{
					.pos = Float3{ (outerRadius * sample.cos), 0.0f, (outerRadius * sample.sin) },
					.normal = normal,
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) },
					.tangent = tangent
				};
				m_mesh.vertices[innerRingBase + i] = Vertex3D{
					.pos = Float3{ (innerRadius * sample.cos), 0.0f, (innerRadius * sample.sin) },
					.normal = normal,
					.tex = Float2{ (0.5f + (innerUVScale * sample.cos)), (0.5f - (innerUVScale * sample.sin)) },
					.tangent = tangent
				};
			}

			TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 next = ((i + 1) % segments);
				const uint32 outerCurrent = static_cast<uint32>(outerRingBase + i);
				const uint32 outerNext = static_cast<uint32>(outerRingBase + next);
				const uint32 innerCurrent = static_cast<uint32>(innerRingBase + i);
				const uint32 innerNext = static_cast<uint32>(innerRingBase + next);

				*pTriangle++ = TriangleIndex32{ outerCurrent, innerCurrent, outerNext };
				*pTriangle++ = TriangleIndex32{ innerCurrent, innerNext, outerNext };
			}
		}

		return true;
	}

	bool Mesh3DBuilder::addAnnulus(
		const double innerRadius,
		const double outerRadius,
		const uint32 segments,
		const Vec3 offset)
	{
		return addAnnulus(
			innerRadius, outerRadius, segments, Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addAnnulus(
		const double innerRadius,
		const double outerRadius,
		const uint32 segments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addAnnulus(innerRadius, outerRadius, segments,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addAnnulus(
		const double innerRadius,
		const double outerRadius,
		const uint32 segments,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addAnnulus(innerRadius, outerRadius, segments))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addHollowCylinder
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addHollowCylinder(
		const double _innerRadius,
		const double _outerRadius,
		const double _height,
		const uint32 segments)
	{
		if ((not IsFloatRepresentable(_innerRadius))
			|| (not IsFloatRepresentable(_outerRadius))
			|| (not IsFloatRepresentable(_height)))
		{
			return GenerationFailed<bool>("Mesh3D::HollowCylinder(): The radii and height must be finite and float-representable");
		}

		const float innerRadius = static_cast<float>(_innerRadius);
		const float outerRadius = static_cast<float>(_outerRadius);
		const float height = static_cast<float>(_height);
		if ((innerRadius <= 0.0f)
			|| (outerRadius <= innerRadius)
			|| (height <= 0.0f)
			|| (segments < 3))
		{
			return GenerationFailed<bool>("Mesh3D::HollowCylinder(): The radii, height, or segment count is invalid");
		}

		size_t ringStride;
		size_t vertexCount;
		size_t triangleCount;
		if ((not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
			|| (not CheckedMultiply(static_cast<size_t>(segments), 8, vertexCount))
			|| (not CheckedAdd(vertexCount, 4, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(segments), 8, triangleCount)))
		{
			return GenerationFailed<bool>("Mesh3D::HollowCylinder(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::HollowCylinder(): The generated mesh exceeds the supported size");
		}

		const Array<CircleSample> circle = Mesh3DDetail::MakeCircleSamples<float>(segments);
		const float halfHeight = (height * 0.5f);
		const float inverseSegments = (1.0f / static_cast<float>(segments));
		const float innerUVScale = (0.5f * innerRadius / outerRadius);
		const size_t outerTopBase = vertexOffset;
		const size_t outerBottomBase = (outerTopBase + ringStride);
		const size_t innerTopBase = (outerTopBase + (ringStride * 2));
		const size_t innerBottomBase = (outerTopBase + (ringStride * 3));
		const size_t topOuterBase = (outerTopBase + (ringStride * 4));
		const size_t topInnerBase = (topOuterBase + segments);
		const size_t bottomOuterBase = (topInnerBase + segments);
		const size_t bottomInnerBase = (bottomOuterBase + segments);

		for (uint32 i = 0; i <= segments; ++i)
		{
			const float u = (i * inverseSegments);
			const CircleSample outerSample = circle[i];
			const Float3 outerNormal{ outerSample.cos, 0.0f, outerSample.sin };
			const Float4 outerTangent{ -outerSample.sin, 0.0f, outerSample.cos, 1.0f };
			m_mesh.vertices[outerTopBase + i] = Vertex3D{
				.pos = Float3{ (outerRadius * outerSample.cos), halfHeight, (outerRadius * outerSample.sin) },
				.normal = outerNormal,
				.tex = Float2{ u, 0.0f },
				.tangent = outerTangent
			};
			m_mesh.vertices[outerBottomBase + i] = Vertex3D{
				.pos = Float3{ (outerRadius * outerSample.cos), -halfHeight, (outerRadius * outerSample.sin) },
				.normal = outerNormal,
				.tex = Float2{ u, 1.0f },
				.tangent = outerTangent
			};

			const CircleSample innerSample = circle[segments - i];
			const Float3 innerNormal{ -innerSample.cos, 0.0f, -innerSample.sin };
			const Float4 innerTangent{ innerSample.sin, 0.0f, -innerSample.cos, 1.0f };
			m_mesh.vertices[innerTopBase + i] = Vertex3D{
				.pos = Float3{ (innerRadius * innerSample.cos), halfHeight, (innerRadius * innerSample.sin) },
				.normal = innerNormal,
				.tex = Float2{ u, 0.0f },
				.tangent = innerTangent
			};
			m_mesh.vertices[innerBottomBase + i] = Vertex3D{
				.pos = Float3{ (innerRadius * innerSample.cos), -halfHeight, (innerRadius * innerSample.sin) },
				.normal = innerNormal,
				.tex = Float2{ u, 1.0f },
				.tangent = innerTangent
			};
		}

		for (uint32 i = 0; i < segments; ++i)
		{
			const CircleSample sample = circle[i];
			const Float3 outerPosition{ (outerRadius * sample.cos), 0.0f, (outerRadius * sample.sin) };
			const Float3 innerPosition{ (innerRadius * sample.cos), 0.0f, (innerRadius * sample.sin) };
			const Float2 topOuterUV{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) };
			const Float2 topInnerUV{ (0.5f + (innerUVScale * sample.cos)), (0.5f - (innerUVScale * sample.sin)) };
			const Float2 bottomOuterUV{ (0.5f + (0.5f * sample.cos)), (0.5f + (0.5f * sample.sin)) };
			const Float2 bottomInnerUV{ (0.5f + (innerUVScale * sample.cos)), (0.5f + (innerUVScale * sample.sin)) };

			m_mesh.vertices[topOuterBase + i] = Vertex3D{
				.pos = Float3{ outerPosition.x, halfHeight, outerPosition.z },
				.normal = Float3::UnitY(),
				.tex = topOuterUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			m_mesh.vertices[topInnerBase + i] = Vertex3D{
				.pos = Float3{ innerPosition.x, halfHeight, innerPosition.z },
				.normal = Float3::UnitY(),
				.tex = topInnerUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			m_mesh.vertices[bottomOuterBase + i] = Vertex3D{
				.pos = Float3{ outerPosition.x, -halfHeight, outerPosition.z },
				.normal = -Float3::UnitY(),
				.tex = bottomOuterUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			m_mesh.vertices[bottomInnerBase + i] = Vertex3D{
				.pos = Float3{ innerPosition.x, -halfHeight, innerPosition.z },
				.normal = -Float3::UnitY(),
				.tex = bottomInnerUV,
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
		}

		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
		for (uint32 i = 0; i < segments; ++i)
		{
			const uint32 outerTopLeft = static_cast<uint32>(outerTopBase + i);
			const uint32 outerTopRight = (outerTopLeft + 1);
			const uint32 outerBottomLeft = static_cast<uint32>(outerBottomBase + i);
			const uint32 outerBottomRight = (outerBottomLeft + 1);
			*pTriangle++ = TriangleIndex32{ outerTopLeft, outerTopRight, outerBottomLeft };
			*pTriangle++ = TriangleIndex32{ outerBottomLeft, outerTopRight, outerBottomRight };

			const uint32 innerTopLeft = static_cast<uint32>(innerTopBase + i);
			const uint32 innerTopRight = (innerTopLeft + 1);
			const uint32 innerBottomLeft = static_cast<uint32>(innerBottomBase + i);
			const uint32 innerBottomRight = (innerBottomLeft + 1);
			*pTriangle++ = TriangleIndex32{ innerTopLeft, innerTopRight, innerBottomLeft };
			*pTriangle++ = TriangleIndex32{ innerBottomLeft, innerTopRight, innerBottomRight };

			const uint32 next = ((i + 1) % segments);
			const uint32 topOuterCurrent = static_cast<uint32>(topOuterBase + i);
			const uint32 topOuterNext = static_cast<uint32>(topOuterBase + next);
			const uint32 topInnerCurrent = static_cast<uint32>(topInnerBase + i);
			const uint32 topInnerNext = static_cast<uint32>(topInnerBase + next);
			*pTriangle++ = TriangleIndex32{ topOuterCurrent, topInnerCurrent, topOuterNext };
			*pTriangle++ = TriangleIndex32{ topInnerCurrent, topInnerNext, topOuterNext };

			const uint32 bottomOuterCurrent = static_cast<uint32>(bottomOuterBase + i);
			const uint32 bottomOuterNext = static_cast<uint32>(bottomOuterBase + next);
			const uint32 bottomInnerCurrent = static_cast<uint32>(bottomInnerBase + i);
			const uint32 bottomInnerNext = static_cast<uint32>(bottomInnerBase + next);
			*pTriangle++ = TriangleIndex32{ bottomOuterCurrent, bottomOuterNext, bottomInnerCurrent };
			*pTriangle++ = TriangleIndex32{ bottomInnerCurrent, bottomOuterNext, bottomInnerNext };
		}

		return true;
	}

	bool Mesh3DBuilder::addHollowCylinder(
		const double innerRadius,
		const double outerRadius,
		const double height,
		const uint32 segments,
		const Vec3 offset)
	{
		return addHollowCylinder(innerRadius, outerRadius, height, segments,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addHollowCylinder(
		const double innerRadius,
		const double outerRadius,
		const double height,
		const uint32 segments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addHollowCylinder(innerRadius, outerRadius, height, segments,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addHollowCylinder(
		const double innerRadius,
		const double outerRadius,
		const double height,
		const uint32 segments,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addHollowCylinder(innerRadius, outerRadius, height, segments))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addConicalFrustum
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addConicalFrustum(
		const double _bottomRadius,
		const double _topRadius,
		const double _height,
		const uint32 segments)
	{
		if ((not IsFloatRepresentable(_bottomRadius))
			|| (not IsFloatRepresentable(_topRadius))
			|| (not IsFloatRepresentable(_height)))
		{
			return GenerationFailed<bool>("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The radii and height must be finite and float-representable");
		}

		const float bottomRadius = static_cast<float>(_bottomRadius);
		const float topRadius = static_cast<float>(_topRadius);
		const float height = static_cast<float>(_height);
		if ((bottomRadius <= 0.0f)
			|| (topRadius < 0.0f)
			|| (height <= 0.0f)
			|| (segments < 3))
		{
			return GenerationFailed<bool>("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The radii, height, or segment count is invalid");
		}

		const bool isCone = (topRadius == 0.0f);
		size_t ringStride;
		size_t vertexCount;
		size_t triangleCount;

		if (not CheckedAdd(static_cast<size_t>(segments), 1, ringStride))
		{
			return GenerationFailed<bool>("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): segments exceed the supported range");
		}

		if (isCone)
		{
			size_t scaledSegments;
			if ((not CheckedMultiply(static_cast<size_t>(segments), 3, scaledSegments))
				|| (not CheckedAdd(scaledSegments, 2, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 2, triangleCount)))
			{
				return GenerationFailed<bool>("Mesh3D::Cone(): The generated mesh exceeds the supported size");
			}
		}
		else
		{
			if ((not CheckedMultiply(ringStride, 4, vertexCount))
				|| (not CheckedMultiply(static_cast<size_t>(segments), 4, triangleCount)))
			{
				return GenerationFailed<bool>("Mesh3D::ConicalFrustum()/Cylinder(): The generated mesh exceeds the supported size");
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return GenerationFailed<bool>("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The generated vertex count exceeds the supported range");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3D::ConicalFrustum()/Cylinder()/Cone(): The generated mesh exceeds the supported size");
		}

		const Array<CircleSample> circle = Mesh3DDetail::MakeCircleSamples<float>(segments);
		const float halfHeight = (height * 0.5f);
		const float radiusDelta = (bottomRadius - topRadius);
		const float inverseSideLength = (1.0f / std::sqrt((height * height) + (radiusDelta * radiusDelta)));
		const float angleStep = (Math::TwoPiF / static_cast<float>(segments));
		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);

		if (isCone)
		{
			const size_t apexBase = vertexOffset;
			const size_t bottomSideBase = (apexBase + segments);
			const size_t bottomCapCenter = (bottomSideBase + ringStride);
			const size_t bottomCapRingBase = (bottomCapCenter + 1);

			for (uint32 i = 0; i < segments; ++i)
			{
				const float middleAngle = ((i + 0.5f) * angleStep);
				const float middleSin = std::sin(middleAngle);
				const float middleCos = std::cos(middleAngle);
				const Float3 sideNormal = Float3{
					(height * middleCos),
					radiusDelta,
					(height * middleSin)
				} * inverseSideLength;

				m_mesh.vertices[apexBase + i] = Vertex3D{
					.pos = Float3{ 0.0f, halfHeight, 0.0f },
					.normal = sideNormal,
					.tex = Float2{ ((i + 0.5f) / static_cast<float>(segments)), 0.0f },
					.tangent = Float4{ -middleSin, 0.0f, middleCos, 1.0f }
				};
			}

			for (uint32 i = 0; i <= segments; ++i)
			{
				const CircleSample sample = circle[i];
				const Float3 sideNormal = Float3{
					(height * sample.cos),
					radiusDelta,
					(height * sample.sin)
				} * inverseSideLength;

				m_mesh.vertices[bottomSideBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = sideNormal,
					.tex = Float2{ (static_cast<float>(i) / static_cast<float>(segments)), 1.0f },
					.tangent = Float4{ -sample.sin, 0.0f, sample.cos, 1.0f }
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 apex = static_cast<uint32>(apexBase + i);
				const uint32 bottomLeft = static_cast<uint32>(bottomSideBase + i);
				const uint32 bottomRight = (bottomLeft + 1);
				*pTriangle++ = TriangleIndex32{ apex, bottomRight, bottomLeft };
			}

			m_mesh.vertices[bottomCapCenter] = Vertex3D{
				.pos = Float3{ 0.0f, -halfHeight, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				m_mesh.vertices[bottomCapRingBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = -Float3::UnitY(),
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f + (0.5f * sample.sin)) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 current = static_cast<uint32>(bottomCapRingBase + i);
				const uint32 next = static_cast<uint32>(bottomCapRingBase + ((i + 1) % segments));
				*pTriangle++ = TriangleIndex32{
					static_cast<uint32>(bottomCapCenter), current, next };
			}
		}
		else
		{
			const size_t topSideBase = vertexOffset;
			const size_t bottomSideBase = (topSideBase + ringStride);
			const size_t bottomCapCenter = (topSideBase + (ringStride * 2));
			const size_t bottomCapRingBase = (bottomCapCenter + 1);
			const size_t topCapCenter = (bottomCapRingBase + segments);
			const size_t topCapRingBase = (topCapCenter + 1);

			for (uint32 i = 0; i <= segments; ++i)
			{
				const CircleSample sample = circle[i];
				const Float3 sideNormal = Float3{
					(height * sample.cos),
					radiusDelta,
					(height * sample.sin)
				} * inverseSideLength;
				const Float4 sideTangent{ -sample.sin, 0.0f, sample.cos, 1.0f };
				const float u = (static_cast<float>(i) / static_cast<float>(segments));

				m_mesh.vertices[topSideBase + i] = Vertex3D{
					.pos = Float3{ (topRadius * sample.cos), halfHeight, (topRadius * sample.sin) },
					.normal = sideNormal,
					.tex = Float2{ u, 0.0f },
					.tangent = sideTangent
				};
				m_mesh.vertices[bottomSideBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = sideNormal,
					.tex = Float2{ u, 1.0f },
					.tangent = sideTangent
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 topLeft = static_cast<uint32>(topSideBase + i);
				const uint32 topRight = (topLeft + 1);
				const uint32 bottomLeft = static_cast<uint32>(bottomSideBase + i);
				const uint32 bottomRight = (bottomLeft + 1);

				*pTriangle++ = TriangleIndex32{ topLeft, topRight, bottomLeft };
				*pTriangle++ = TriangleIndex32{ bottomLeft, topRight, bottomRight };
			}

			m_mesh.vertices[bottomCapCenter] = Vertex3D{
				.pos = Float3{ 0.0f, -halfHeight, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};
			m_mesh.vertices[topCapCenter] = Vertex3D{
				.pos = Float3{ 0.0f, halfHeight, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};

			for (uint32 i = 0; i < segments; ++i)
			{
				const CircleSample sample = circle[i];
				m_mesh.vertices[bottomCapRingBase + i] = Vertex3D{
					.pos = Float3{ (bottomRadius * sample.cos), -halfHeight, (bottomRadius * sample.sin) },
					.normal = -Float3::UnitY(),
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f + (0.5f * sample.sin)) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
				m_mesh.vertices[topCapRingBase + i] = Vertex3D{
					.pos = Float3{ (topRadius * sample.cos), halfHeight, (topRadius * sample.sin) },
					.normal = Float3::UnitY(),
					.tex = Float2{ (0.5f + (0.5f * sample.cos)), (0.5f - (0.5f * sample.sin)) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
			}

			for (uint32 i = 0; i < segments; ++i)
			{
				const uint32 bottomCurrent = static_cast<uint32>(bottomCapRingBase + i);
				const uint32 bottomNext = static_cast<uint32>(bottomCapRingBase + ((i + 1) % segments));
				const uint32 topCurrent = static_cast<uint32>(topCapRingBase + i);
				const uint32 topNext = static_cast<uint32>(topCapRingBase + ((i + 1) % segments));

				*pTriangle++ = TriangleIndex32{
					static_cast<uint32>(bottomCapCenter), bottomCurrent, bottomNext };
				*pTriangle++ = TriangleIndex32{
					static_cast<uint32>(topCapCenter), topNext, topCurrent };
			}
		}

		return true;
	}

	bool Mesh3DBuilder::addConicalFrustum(
		const double bottomRadius,
		const double topRadius,
		const double height,
		const uint32 segments,
		const Vec3 offset)
	{
		return addConicalFrustum(bottomRadius, topRadius, height, segments,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addConicalFrustum(
		const double bottomRadius,
		const double topRadius,
		const double height,
		const uint32 segments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addConicalFrustum(bottomRadius, topRadius, height, segments,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addConicalFrustum(
		const double bottomRadius,
		const double topRadius,
		const double height,
		const uint32 segments,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addConicalFrustum(bottomRadius, topRadius, height, segments))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCylinder
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addCylinder(
		const double radius,
		const double height,
		const uint32 segments)
	{
		return addConicalFrustum(radius, radius, height, segments);
	}

	bool Mesh3DBuilder::addCylinder(
		const double radius,
		const double height,
		const uint32 segments,
		const Vec3 offset)
	{
		return addConicalFrustum(radius, radius, height, segments, offset);
	}

	bool Mesh3DBuilder::addCylinder(
		const double radius,
		const double height,
		const uint32 segments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addConicalFrustum(radius, radius, height, segments, offset, rotation);
	}

	bool Mesh3DBuilder::addCylinder(
		const double radius,
		const double height,
		const uint32 segments,
		const Mat4x4& transform)
	{
		return addConicalFrustum(radius, radius, height, segments, transform);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCone
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addCone(
		const double radius,
		const double height,
		const uint32 segments)
	{
		return addConicalFrustum(radius, 0.0, height, segments);
	}

	bool Mesh3DBuilder::addCone(
		const double radius,
		const double height,
		const uint32 segments,
		const Vec3 offset)
	{
		return addConicalFrustum(radius, 0.0, height, segments, offset);
	}

	bool Mesh3DBuilder::addCone(
		const double radius,
		const double height,
		const uint32 segments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addConicalFrustum(radius, 0.0, height, segments, offset, rotation);
	}

	bool Mesh3DBuilder::addCone(
		const double radius,
		const double height,
		const uint32 segments,
		const Mat4x4& transform)
	{
		return addConicalFrustum(radius, 0.0, height, segments, transform);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addTorus
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addTorus(
		const double _majorRadius,
		const double _tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments)
	{
		if ((not IsFloatRepresentable(_majorRadius))
			|| (not IsFloatRepresentable(_tubeRadius)))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addTorus(): majorRadius and tubeRadius must be finite and float-representable");
		}

		const float majorRadius = static_cast<float>(_majorRadius);
		const float tubeRadius = static_cast<float>(_tubeRadius);
		if ((majorRadius <= 0.0f)
			|| (tubeRadius <= 0.0f)
			|| (majorRadius <= tubeRadius)
			|| (ringSegments < 3)
			|| (tubeSegments < 3))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addTorus(): The radii or segment counts are invalid");
		}

		size_t ringStride;
		size_t tubeRowCount;
		size_t vertexCount;
		size_t quadCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(ringSegments), 1, ringStride))
			|| (not CheckedAdd(static_cast<size_t>(tubeSegments), 1, tubeRowCount))
			|| (not CheckedMultiply(ringStride, tubeRowCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(ringSegments), static_cast<size_t>(tubeSegments), quadCount))
			|| (not CheckedMultiply(quadCount, 2, triangleCount)))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addTorus(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addTorus(): The generated mesh exceeds the supported size");
		}

		const Array<CircleSample> ringSinCos = Mesh3DDetail::MakeCircleSamples<float>(ringSegments);
		const Array<CircleSample> tubeSinCos = Mesh3DDetail::MakeCircleSamples<float>(tubeSegments);
		const float invRingSegments = (1.0f / static_cast<float>(ringSegments));
		const float invTubeSegments = (1.0f / static_cast<float>(tubeSegments));

		for (uint32 tubeIndex = 0; tubeIndex <= tubeSegments; ++tubeIndex)
		{
			const float tubeSin = tubeSinCos[tubeIndex].sin;
			const float tubeCos = tubeSinCos[tubeIndex].cos;
			const float ringRadius = (majorRadius + (tubeRadius * tubeSin));
			const float positionY = (tubeRadius * tubeCos);
			const float v = (static_cast<float>(tubeIndex) * invTubeSegments);
			const size_t rowBase = (vertexOffset + (static_cast<size_t>(tubeIndex) * ringStride));

			for (uint32 ringIndex = 0; ringIndex <= ringSegments; ++ringIndex)
			{
				const float ringSin = ringSinCos[ringIndex].sin;
				const float ringCos = ringSinCos[ringIndex].cos;
				const Float3 normal{ (ringCos * tubeSin), tubeCos, (ringSin * tubeSin) };

				m_mesh.vertices[rowBase + ringIndex] = Vertex3D{
					.pos = Float3{ (ringRadius * ringCos), positionY, (ringRadius * ringSin) },
					.normal = normal,
					.tex = Float2{ (static_cast<float>(ringIndex) * invRingSegments), v },
					.tangent = Float4{ -ringSin, 0.0f, ringCos, 1.0f }
				};
			}
		}

		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
		for (uint32 tubeIndex = 0; tubeIndex < tubeSegments; ++tubeIndex)
		{
			const size_t rowBase = (vertexOffset + (static_cast<size_t>(tubeIndex) * ringStride));
			const size_t nextRowBase = (rowBase + ringStride);

			for (uint32 ringIndex = 0; ringIndex < ringSegments; ++ringIndex)
			{
				const uint32 i0 = static_cast<uint32>(rowBase + ringIndex);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(nextRowBase + ringIndex);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		return true;
	}

	bool Mesh3DBuilder::addTorus(
		const double majorRadius,
		const double tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments,
		const Vec3 offset)
	{
		return addTorus(majorRadius, tubeRadius, ringSegments, tubeSegments,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addTorus(
		const double majorRadius,
		const double tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addTorus(majorRadius, tubeRadius, ringSegments, tubeSegments,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addTorus(
		const double majorRadius,
		const double tubeRadius,
		const uint32 ringSegments,
		const uint32 tubeSegments,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addTorus(majorRadius, tubeRadius, ringSegments, tubeSegments))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSphere
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addSphere(
		const double _radius,
		const uint32 slices,
		const uint32 stacks)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addSphere(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if ((radius <= 0.0f)
			|| (slices < 3)
			|| (stacks < 2))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addSphere(): The radius, slice count, or stack count is invalid");
		}

		const size_t interiorRingCount = (static_cast<size_t>(stacks) - 1);
		size_t ringStride;
		size_t ringVertexCount;
		size_t poleVertexCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(interiorRingCount, ringStride, ringVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(slices), 2, poleVertexCount))
			|| (not CheckedAdd(ringVertexCount, poleVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(slices), interiorRingCount, triangleCount))
			|| (not CheckedMultiply(triangleCount, 2, triangleCount)))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addSphere(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addSphere(): The generated mesh exceeds the supported size");
		}

		const size_t firstRingBase = (vertexOffset + slices);
		const size_t bottomPoleBase = (firstRingBase + ringVertexCount);
		const float longitudeStep = (Math::TwoPiF / static_cast<float>(slices));
		const float latitudeStep = (Math::PiF / static_cast<float>(stacks));
		const Array<CircleSample> longitudeSinCos = Mesh3DDetail::MakeCircleSamples<float>(slices);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float longitude = (longitudeStep * x);
			const float middleLongitude = (longitude + (longitudeStep * 0.5f));
			const float middleSin = std::sin(middleLongitude);
			const float middleCos = std::cos(middleLongitude);
			const float u = ((x + 0.5f) / static_cast<float>(slices));
			const Float4 tangent{ -middleSin, 0.0f, middleCos, 1.0f };

			m_mesh.vertices[vertexOffset + x] = Vertex3D{
				.pos = Float3{ 0.0f, radius, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ u, 0.0f },
				.tangent = tangent
			};
			m_mesh.vertices[bottomPoleBase + x] = Vertex3D{
				.pos = Float3{ 0.0f, -radius, 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ u, 1.0f },
				.tangent = tangent
			};
		}

		for (uint32 y = 1; y < stacks; ++y)
		{
			const float v = (static_cast<float>(y) / static_cast<float>(stacks));
			const float latitude = (latitudeStep * y);
			const float latitudeSin = std::sin(latitude);
			const float latitudeCos = std::cos(latitude);
			const size_t ringBase = (firstRingBase + (static_cast<size_t>(y - 1) * ringStride));

			for (uint32 x = 0; x <= slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;
				const Float3 normal{
					(latitudeSin * longitudeCos),
					latitudeCos,
					(latitudeSin * longitudeSin)
				};

				m_mesh.vertices[ringBase + x] = Vertex3D{
					.pos = (normal * radius),
					.normal = normal,
					.tex = Float2{ (static_cast<float>(x) / static_cast<float>(slices)), v },
					.tangent = Float4{ -longitudeSin, 0.0f, longitudeCos, 1.0f }
				};
			}
		}

		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 pole = static_cast<uint32>(vertexOffset + x);
			const uint32 ringLeft = static_cast<uint32>(firstRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			*pTriangle++ = TriangleIndex32{ pole, ringRight, ringLeft };
		}

		for (size_t ring = 0; (ring + 1) < interiorRingCount; ++ring)
		{
			const size_t upperRingBase = (firstRingBase + (ring * ringStride));
			const size_t lowerRingBase = (upperRingBase + ringStride);

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 i0 = static_cast<uint32>(upperRingBase + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(lowerRingBase + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		const size_t lastRingBase = (firstRingBase + ((interiorRingCount - 1) * ringStride));
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 ringLeft = static_cast<uint32>(lastRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			const uint32 pole = static_cast<uint32>(bottomPoleBase + x);
			*pTriangle++ = TriangleIndex32{ ringLeft, ringRight, pole };
		}

		return true;
	}

	bool Mesh3DBuilder::addSphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Vec3 offset)
	{
		return addSphere(radius, slices, stacks,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addSphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addSphere(radius, slices, stacks,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addSphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addSphere(radius, slices, stacks))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addHemisphere
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks)
	{
		return addHemisphere(radius, CloseBottom::No, slices, stacks);
	}

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Vec3 offset)
	{
		return addHemisphere(radius, CloseBottom::No, slices, stacks, offset);
	}

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addHemisphere(radius, CloseBottom::No, slices, stacks, offset, rotation);
	}

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const uint32 slices,
		const uint32 stacks,
		const Mat4x4& transform)
	{
		return addHemisphere(radius, CloseBottom::No, slices, stacks, transform);
	}

	bool Mesh3DBuilder::addHemisphere(
		const double _radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks)
	{
		if (not IsFloatRepresentable(_radius))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): radius must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		if ((radius <= 0.0f)
			|| (slices < 3)
			|| (stacks < 1))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): The radius, slice count, or stack count is invalid");
		}

		size_t ringStride;
		size_t ringVertexCount;
		size_t surfaceVertexCount;
		size_t twiceStackCount;
		size_t triangleFactor;
		size_t surfaceTriangleCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(static_cast<size_t>(stacks), ringStride, ringVertexCount))
			|| (not CheckedAdd(static_cast<size_t>(slices), ringVertexCount, surfaceVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(stacks), 2, twiceStackCount)))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): The surface dimensions exceed the supported range");
		}

		triangleFactor = (twiceStackCount - 1);
		if (not CheckedMultiply(static_cast<size_t>(slices), triangleFactor, surfaceTriangleCount))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): The surface triangle count exceeds the supported range");
		}

		vertexCount = surfaceVertexCount;
		triangleCount = surfaceTriangleCount;
		if (closeBottom)
		{
			size_t bottomVertexCount;
			if ((not CheckedAdd(static_cast<size_t>(slices), 1, bottomVertexCount))
				|| (not CheckedAdd(vertexCount, bottomVertexCount, vertexCount))
				|| (not CheckedAdd(triangleCount, static_cast<size_t>(slices), triangleCount)))
			{
				return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): The closed-bottom mesh exceeds the supported size");
			}
		}

		if (Mesh3D::MaxVertexCount < vertexCount)
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): The generated vertex count exceeds the supported range");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addHemisphere(): The generated mesh exceeds the supported size");
		}

		const size_t firstRingBase = (vertexOffset + slices);
		const float invSlices = (1.0f / static_cast<float>(slices));
		const float invStacks = (1.0f / static_cast<float>(stacks));
		const float longitudeStep = (Math::TwoPiF * invSlices);
		const float latitudeStep = (Math::HalfPiF * invStacks);
		const Array<CircleSample> longitudeSinCos = Mesh3DDetail::MakeCircleSamples<float>(slices);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float middleLongitude = (longitudeStep * (static_cast<float>(x) + 0.5f));
			const Float4 tangent{ -std::sin(middleLongitude), 0.0f, std::cos(middleLongitude), 1.0f };

			m_mesh.vertices[vertexOffset + x] = Vertex3D{
				.pos = Float3{ 0.0f, radius, 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ ((static_cast<float>(x) + 0.5f) * invSlices), 0.0f },
				.tangent = tangent
			};
		}

		for (uint32 stack = 1; stack <= stacks; ++stack)
		{
			const bool isEquator = (stack == stacks);
			const float latitude = (latitudeStep * static_cast<float>(stack));
			const float latitudeSin = (isEquator ? 1.0f : std::sin(latitude));
			const float latitudeCos = (isEquator ? 0.0f : std::cos(latitude));
			const float v = (static_cast<float>(stack) * invStacks);
			const size_t ringBase = (firstRingBase + (static_cast<size_t>(stack - 1) * ringStride));

			for (uint32 x = 0; x <= slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;
				const Float3 normal{
					(latitudeSin * longitudeCos),
					latitudeCos,
					(latitudeSin * longitudeSin)
				};

				m_mesh.vertices[ringBase + x] = Vertex3D{
					.pos = (normal * radius),
					.normal = normal,
					.tex = Float2{ (static_cast<float>(x) * invSlices), v },
					.tangent = Float4{ -longitudeSin, 0.0f, longitudeCos, 1.0f }
				};
			}
		}

		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 pole = static_cast<uint32>(vertexOffset + x);
			const uint32 ringLeft = static_cast<uint32>(firstRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			*pTriangle++ = TriangleIndex32{ pole, ringRight, ringLeft };
		}

		for (size_t ring = 0; (ring + 1) < stacks; ++ring)
		{
			const size_t upperRingBase = (firstRingBase + (ring * ringStride));
			const size_t lowerRingBase = (upperRingBase + ringStride);

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 i0 = static_cast<uint32>(upperRingBase + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(lowerRingBase + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		if (closeBottom)
		{
			const size_t bottomCenterIndex = (vertexOffset + surfaceVertexCount);
			const size_t bottomRingBase = (bottomCenterIndex + 1);
			m_mesh.vertices[bottomCenterIndex] = Vertex3D{
				.pos = Float3::Zero(),
				.normal = -Float3::UnitY(),
				.tex = Float2{ 0.5f, 0.5f },
				.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
			};

			for (uint32 x = 0; x < slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;
				m_mesh.vertices[bottomRingBase + x] = Vertex3D{
					.pos = Float3{ (radius * longitudeCos), 0.0f, (radius * longitudeSin) },
					.normal = -Float3::UnitY(),
					.tex = Float2{ (0.5f + 0.5f * longitudeCos), (0.5f + 0.5f * longitudeSin) },
					.tangent = Float4{ 1.0f, 0.0f, 0.0f, 1.0f }
				};
			}

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 center = static_cast<uint32>(bottomCenterIndex);
				const uint32 current = static_cast<uint32>(bottomRingBase + x);
				const uint32 next = static_cast<uint32>(bottomRingBase + ((x + 1) % slices));
				*pTriangle++ = TriangleIndex32{ center, current, next };
			}
		}

		return true;
	}

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks,
		const Vec3 offset)
	{
		return addHemisphere(radius, closeBottom, slices, stacks,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addHemisphere(radius, closeBottom, slices, stacks,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addHemisphere(
		const double radius,
		const CloseBottom closeBottom,
		const uint32 slices,
		const uint32 stacks,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addHemisphere(radius, closeBottom, slices, stacks))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addCapsule
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3DBuilder::addCapsule(
		const double _radius,
		const double _cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks)
	{
		if ((not IsFloatRepresentable(_radius))
			|| (not IsFloatRepresentable(_cylinderHeight)))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addCapsule(): radius and cylinderHeight must be finite and float-representable");
		}

		const float radius = static_cast<float>(_radius);
		const float cylinderHeight = static_cast<float>(_cylinderHeight);
		if ((radius <= 0.0f)
			|| (cylinderHeight < 0.0f)
			|| (slices < 3)
			|| (hemisphereStacks < 1))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addCapsule(): The radius, cylinder height, slice count, or hemisphere stack count is invalid");
		}

		if (cylinderHeight == 0.0f)
		{
			if ((std::numeric_limits<uint32>::max() / 2) < hemisphereStacks)
			{
				return GenerationFailed<bool>("Mesh3DBuilder::addCapsule(): hemisphereStacks exceed the supported range for a zero-length cylinder");
			}

			return addSphere(radius, slices, (hemisphereStacks * 2));
		}

		size_t interiorRingCount;
		size_t ringStride;
		size_t ringVertexCount;
		size_t poleVertexCount;
		size_t vertexCount;
		size_t triangleCount;

		if ((not CheckedMultiply(static_cast<size_t>(hemisphereStacks), 2, interiorRingCount))
			|| (not CheckedAdd(static_cast<size_t>(slices), 1, ringStride))
			|| (not CheckedMultiply(interiorRingCount, ringStride, ringVertexCount))
			|| (not CheckedMultiply(static_cast<size_t>(slices), 2, poleVertexCount))
			|| (not CheckedAdd(ringVertexCount, poleVertexCount, vertexCount))
			|| (Mesh3D::MaxVertexCount < vertexCount)
			|| (not CheckedMultiply(static_cast<size_t>(slices), interiorRingCount, triangleCount))
			|| (not CheckedMultiply(triangleCount, 2, triangleCount)))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addCapsule(): The generated mesh exceeds the supported size");
		}

		size_t vertexOffset;
		size_t triangleOffset;
		if (not ResizeForAddition(
			m_mesh, vertexCount, triangleCount, vertexOffset, triangleOffset))
		{
			return GenerationFailed<bool>("Mesh3DBuilder::addCapsule(): The generated mesh exceeds the supported size");
		}

		const size_t firstRingBase = (vertexOffset + slices);
		const size_t bottomPoleBase = (firstRingBase + ringVertexCount);
		const float halfCylinderHeight = (cylinderHeight * 0.5f);
		const float hemisphereAngleStep = (Math::HalfPiF / static_cast<float>(hemisphereStacks));
		const float invSlices = (1.0f / static_cast<float>(slices));
		const float longitudeStep = (Math::TwoPiF * invSlices);
		const float profileLength = ((Math::PiF * radius) + cylinderHeight);
		const float invProfileLength = (1.0f / profileLength);
		const Array<CircleSample> longitudeSinCos = Mesh3DDetail::MakeCircleSamples<float>(slices);

		for (uint32 x = 0; x < slices; ++x)
		{
			const float middleLongitude = ((longitudeStep * x) + (longitudeStep * 0.5f));
			const float middleSin = std::sin(middleLongitude);
			const float middleCos = std::cos(middleLongitude);
			const float u = ((x + 0.5f) * invSlices);
			const Float4 tangent{ -middleSin, 0.0f, middleCos, 1.0f };

			m_mesh.vertices[vertexOffset + x] = Vertex3D{
				.pos = Float3{ 0.0f, (halfCylinderHeight + radius), 0.0f },
				.normal = Float3::UnitY(),
				.tex = Float2{ u, 0.0f },
				.tangent = tangent
			};
			m_mesh.vertices[bottomPoleBase + x] = Vertex3D{
				.pos = Float3{ 0.0f, -(halfCylinderHeight + radius), 0.0f },
				.normal = -Float3::UnitY(),
				.tex = Float2{ u, 1.0f },
				.tangent = tangent
			};
		}

		const auto writeRing = [&](const size_t ringIndex, const float ringRadius, const float positionY,
			const float normalRadial, const float normalY, const float v)
		{
			const size_t ringBase = (firstRingBase + (ringIndex * ringStride));

			for (uint32 x = 0; x <= slices; ++x)
			{
				const float longitudeSin = longitudeSinCos[x].sin;
				const float longitudeCos = longitudeSinCos[x].cos;

				m_mesh.vertices[ringBase + x] = Vertex3D{
					.pos = Float3{ (ringRadius * longitudeCos), positionY, (ringRadius * longitudeSin) },
					.normal = Float3{ (normalRadial * longitudeCos), normalY, (normalRadial * longitudeSin) },
					.tex = Float2{ (static_cast<float>(x) * invSlices), v },
					.tangent = Float4{ -longitudeSin, 0.0f, longitudeCos, 1.0f }
				};
			}
		};

		for (uint32 stack = 1; stack <= hemisphereStacks; ++stack)
		{
			const float angle = (hemisphereAngleStep * stack);
			const float angleSin = std::sin(angle);
			const float angleCos = std::cos(angle);
			writeRing(
				(stack - 1),
				(radius * angleSin),
				(halfCylinderHeight + (radius * angleCos)),
				angleSin,
				angleCos,
				((radius * angle) * invProfileLength));
		}

		writeRing(
			hemisphereStacks,
			radius,
			-halfCylinderHeight,
			1.0f,
			0.0f,
			(((Math::HalfPiF * radius) + cylinderHeight) * invProfileLength));

		for (uint32 stack = 1; stack < hemisphereStacks; ++stack)
		{
			const float angle = (hemisphereAngleStep * stack);
			const float angleSin = std::sin(angle);
			const float angleCos = std::cos(angle);
			writeRing(
				(static_cast<size_t>(hemisphereStacks) + stack),
				(radius * angleCos),
				(-halfCylinderHeight - (radius * angleSin)),
				angleCos,
				-angleSin,
				(((Math::HalfPiF * radius) + cylinderHeight + (radius * angle)) * invProfileLength));
		}

		TriangleIndex32* pTriangle = (m_mesh.indices.data() + triangleOffset);
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 pole = static_cast<uint32>(vertexOffset + x);
			const uint32 ringLeft = static_cast<uint32>(firstRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			*pTriangle++ = TriangleIndex32{ pole, ringRight, ringLeft };
		}

		for (size_t ring = 0; (ring + 1) < interiorRingCount; ++ring)
		{
			const size_t upperRingBase = (firstRingBase + (ring * ringStride));
			const size_t lowerRingBase = (upperRingBase + ringStride);

			for (uint32 x = 0; x < slices; ++x)
			{
				const uint32 i0 = static_cast<uint32>(upperRingBase + x);
				const uint32 i1 = (i0 + 1);
				const uint32 i2 = static_cast<uint32>(lowerRingBase + x);
				const uint32 i3 = (i2 + 1);

				*pTriangle++ = TriangleIndex32{ i0, i1, i2 };
				*pTriangle++ = TriangleIndex32{ i2, i1, i3 };
			}
		}

		const size_t lastRingBase = (firstRingBase + ((interiorRingCount - 1) * ringStride));
		for (uint32 x = 0; x < slices; ++x)
		{
			const uint32 ringLeft = static_cast<uint32>(lastRingBase + x);
			const uint32 ringRight = (ringLeft + 1);
			const uint32 pole = static_cast<uint32>(bottomPoleBase + x);
			*pTriangle++ = TriangleIndex32{ ringLeft, ringRight, pole };
		}

		return true;
	}

	bool Mesh3DBuilder::addCapsule(
		const double radius,
		const double cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks,
		const Vec3 offset)
	{
		return addCapsule(radius, cylinderHeight, slices, hemisphereStacks,
			Mat4x4::Translate(Float3{ offset }));
	}

	bool Mesh3DBuilder::addCapsule(
		const double radius,
		const double cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks,
		const Vec3 offset,
		const Quaternion& rotation)
	{
		return addCapsule(radius, cylinderHeight, slices, hemisphereStacks,
			Mat4x4::AffineTransform(Float3::One(), rotation, Float3{ offset }));
	}

	bool Mesh3DBuilder::addCapsule(
		const double radius,
		const double cylinderHeight,
		const uint32 slices,
		const uint32 hemisphereStacks,
		const Mat4x4& transform)
	{
		const size_t vertexOffset = m_mesh.vertices.size();
		if (not addCapsule(radius, cylinderHeight, slices, hemisphereStacks))
		{
			return false;
		}

		TransformAddedVertices(m_mesh, vertexOffset, transform);
		return true;
	}
}
