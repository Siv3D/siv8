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
# include <array>
# include <cmath>

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::CheckedAdd;
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

		TransformVertexRange(
			std::span<Vertex3D>{ (m_mesh.vertices.data() + vertexOffset), 24 },
			transform);
		return true;
	}
}
