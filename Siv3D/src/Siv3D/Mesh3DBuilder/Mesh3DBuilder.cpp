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
}
