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
# include <Siv3D/Error.hpp>
# include "Mesh3DNormals.hpp"
# include "Mesh3DMikkTSpace.hpp"

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr CNORM_FLAGS ToCNORMFlags(const VertexNormalWeighting weighting) noexcept
		{
			uint32 flags = CNORM_DEFAULT;

			switch (weighting)
			{
			case VertexNormalWeighting::Angle:
				flags |= CNORM_DEFAULT;
				break;
			case VertexNormalWeighting::Area:
				flags |= CNORM_WEIGHT_BY_AREA;
				break;
			case VertexNormalWeighting::Uniform:
				flags |= CNORM_WEIGHT_EQUAL;
				break;
			default:
				assert(false);
				break;
			}

			return ToEnum<CNORM_FLAGS>(flags);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeNormals
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::computeNormals(const VertexNormalWeighting weighting)
	{
		if (indices.isEmpty())
		{
			for (auto& vertex : vertices)
			{
				vertex.normal = Float3{ 0.0f, 0.0f, 0.0f };
			}

			return *this;
		}

		if (vertices.isEmpty())
		{
			return *this;
		}

		const size_t vertexCount = vertices.size();
		const Array<DirectX::XMFLOAT3> positions = vertices.map([](const Vertex3D& vertex)
		{
			return DirectX::XMFLOAT3{ vertex.pos.x, vertex.pos.y, vertex.pos.z };
		});

		const size_t triangleCount = indices.size();
		Array<uint32> flatIndices(triangleCount * 3);
		for (size_t i = 0; i < triangleCount; ++i)
		{
			const TriangleIndex32& triangle = indices[i];
			flatIndices[i * 3 + 0] = triangle.i0;
			flatIndices[i * 3 + 1] = triangle.i1;
			flatIndices[i * 3 + 2] = triangle.i2;
		}

		Array<DirectX::XMFLOAT3> computedNormals(vertexCount);

		const bool result = ComputeNormals(
			flatIndices.data(),
			triangleCount,
			positions.data(),
			vertexCount,
			ToCNORMFlags(weighting),
			computedNormals.data());

		if (not result)
		{
			throw Error{ "Mesh3D::computeNormals(): DirectX::ComputeNormals() failed." };
		};

		{
			const DirectX::XMFLOAT3* pSrc = computedNormals.data();
			const DirectX::XMFLOAT3* pSrcEnd = (pSrc + vertexCount);
			Vertex3D* pDst = vertices.data();

			while (pSrc != pSrcEnd)
			{
				const DirectX::XMFLOAT3& normal = *pSrc;
				pDst->normal = Float3{ normal.x, normal.y, normal.z };
				++pSrc;
				++pDst;
			}
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeTangents
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::computeTangents()
	{
		if (not GenerateMikkTSpaceTangents(vertices, indices))
		{
			throw Error{ "Mesh3D::computeTangents(): Failed to generate MikkTSpace tangents." };
		}

		return *this;
	}
}
