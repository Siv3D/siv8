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
# include <Siv3D/Mat4x4.hpp>
# include <Siv3D/Quaternion.hpp>
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
	//	validate
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3D::validate() const noexcept
	{
		if (MaxVertexCount < vertices.size())
		{
			return false;
		}

		const size_t vertexCount = vertices.size();

		for (const auto& triangle : indices)
		{
			if ((vertexCount <= triangle.i0)
				|| (vertexCount <= triangle.i1)
				|| (vertexCount <= triangle.i2))
			{
				return false;
			}
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	append
	//
	////////////////////////////////////////////////////////////////

	bool Mesh3D::append(const Mesh3D& mesh)
	{
		if (not mesh.validate())
		{
			return false;
		}

		const size_t vertexOffset = vertices.size();
		const size_t sourceVertexCount = mesh.vertices.size();

		if ((MaxVertexCount < vertexOffset)
			|| ((MaxVertexCount - vertexOffset) < sourceVertexCount))
		{
			return false;
		}

		const size_t triangleOffset = indices.size();
		const size_t sourceTriangleCount = mesh.indices.size();

		if (this == &mesh)
		{
			vertices.resize(vertexOffset + sourceVertexCount);

			for (size_t i = 0; i < sourceVertexCount; ++i)
			{
				vertices[vertexOffset + i] = vertices[i];
			}

			indices.resize(triangleOffset + sourceTriangleCount);

			for (size_t i = 0; i < sourceTriangleCount; ++i)
			{
				indices[triangleOffset + i] = indices[i];
			}
		}
		else
		{
			vertices.insert(
				vertices.end(),
				mesh.vertices.begin(),
				mesh.vertices.end());

			indices.insert(
				indices.end(),
				mesh.indices.begin(),
				mesh.indices.end());
		}

		for (size_t i = triangleOffset; i < indices.size(); ++i)
		{
			auto& triangle = indices[i];

			triangle.i0 = static_cast<TriangleIndex32::value_type>(vertexOffset + triangle.i0);
			triangle.i1 = static_cast<TriangleIndex32::value_type>(vertexOffset + triangle.i1);
			triangle.i2 = static_cast<TriangleIndex32::value_type>(vertexOffset + triangle.i2);
		}

		return true;
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

	////////////////////////////////////////////////////////////////
	//
	//	reverseWinding
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::reverseWinding() noexcept
	{
		for (auto& triangle : indices)
		{
			triangle.flip();
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	invertNormals
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::invertNormals() noexcept
	{
		for (auto& vertex : vertices)
		{
			vertex.normal = -vertex.normal;
			vertex.tangent.w = -vertex.tangent.w;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	invert
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::invert() noexcept
	{
		reverseWinding();
		invertNormals();

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformed
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::transformed(const Mat4x4& matrix) const&
	{
		Mesh3D result{ *this };
		result.transform(matrix);
		return result;
	}

	Mesh3D Mesh3D::transformed(const Mat4x4& matrix) && noexcept
	{
		transform(matrix);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	transform
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::transform(const Mat4x4& matrix) noexcept
	{
		if (vertices.isEmpty())
		{
			return *this;
		}

		DirectX::XMVECTOR determinantVector;
		const DirectX::XMMATRIX inverseMatrix = DirectX::XMMatrixInverse(&determinantVector, matrix.value);
		const float determinant = DirectX::XMVectorGetX(determinantVector);

		if (determinant == 0.0f)
		{
			for (auto& vertex : vertices)
			{
				const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
					static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
				DirectX::XMStoreFloat3(
					static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
					DirectX::XMVector3Transform(position, matrix.value));
			}

			return *this;
		}

		const DirectX::XMMATRIX normalMatrix = DirectX::XMMatrixTranspose(inverseMatrix);
		const bool flipHandedness = (determinant < 0.0f);

		for (auto& vertex : vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
				DirectX::XMVector3Transform(position, matrix.value));

			const DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.normal)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.normal)),
				DirectX::XMVector3Normalize(DirectX::XMVector3TransformNormal(normal, normalMatrix)));

			const DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(
				DirectX::XMVector3TransformNormal(
					DirectX::XMLoadFloat4(
						static_cast<const DirectX::XMFLOAT4*>(static_cast<const void*>(&vertex.tangent))),
					matrix.value));
			const float tangentW = (flipHandedness ? -vertex.tangent.w : vertex.tangent.w);
			DirectX::XMStoreFloat4(
				static_cast<DirectX::XMFLOAT4*>(static_cast<void*>(&vertex.tangent)),
				DirectX::XMVectorSetW(tangent, tangentW));
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	translated
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::translated(const Float3 offset) const&
	{
		Mesh3D result{ *this };
		result.translate(offset);
		return result;
	}

	Mesh3D Mesh3D::translated(const Float3 offset) && noexcept
	{
		translate(offset);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	translate
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::translate(const Float3 offset) noexcept
	{
		for (auto& vertex : vertices)
		{
			vertex.pos += offset;
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::rotated(const Quaternion& rotation) const&
	{
		Mesh3D result{ *this };
		result.rotate(rotation);
		return result;
	}

	Mesh3D Mesh3D::rotated(const Quaternion& rotation) && noexcept
	{
		rotate(rotation);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotate
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::rotate(const Quaternion& rotation) noexcept
	{
		const DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(rotation.value.vec);

		for (auto& vertex : vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
				DirectX::XMVector3TransformNormal(position, rotationMatrix));

			const DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.normal)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.normal)),
				DirectX::XMVector3TransformNormal(normal, rotationMatrix));

			const DirectX::XMVECTOR tangent = DirectX::XMVector3TransformNormal(
				DirectX::XMLoadFloat4(
					static_cast<const DirectX::XMFLOAT4*>(static_cast<const void*>(&vertex.tangent))),
				rotationMatrix);
			const float tangentW = vertex.tangent.w;
			DirectX::XMStoreFloat4(
				static_cast<DirectX::XMFLOAT4*>(static_cast<void*>(&vertex.tangent)),
				DirectX::XMVectorSetW(tangent, tangentW));
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaled
	//
	////////////////////////////////////////////////////////////////

	Mesh3D Mesh3D::scaled(const float scale) const&
	{
		Mesh3D result{ *this };
		result.scale(scale);
		return result;
	}

	Mesh3D Mesh3D::scaled(const float scale) && noexcept
	{
		this->scale(scale);
		return std::move(*this);
	}

	Mesh3D Mesh3D::scaled(const Float3 scale) const&
	{
		Mesh3D result{ *this };
		result.scale(scale);
		return result;
	}

	Mesh3D Mesh3D::scaled(const Float3 scale) && noexcept
	{
		this->scale(scale);
		return std::move(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	scale
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::scale(const float scale) noexcept
	{
		if (scale < 0.0f)
		{
			for (auto& vertex : vertices)
			{
				vertex.pos *= scale;
				vertex.normal = -vertex.normal;
				vertex.tangent = -vertex.tangent;
			}
		}
		else
		{
			for (auto& vertex : vertices)
			{
				vertex.pos *= scale;
			}
		}

		return *this;
	}

	Mesh3D& Mesh3D::scale(const Float3 scale) noexcept
	{
		const DirectX::XMVECTOR scaleVector = DirectX::XMVectorSetW(
			DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&scale))), 1.0f);

		if ((scale.x == 0.0f)
			|| (scale.y == 0.0f)
			|| (scale.z == 0.0f))
		{
			for (auto& vertex : vertices)
			{
				const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
					static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
				DirectX::XMStoreFloat3(
					static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
					DirectX::XMVectorMultiply(position, scaleVector));
			}

			return *this;
		}

		const DirectX::XMVECTOR inverseScaleVector = DirectX::XMVectorReciprocal(scaleVector);
		const bool flipHandedness = (((scale.x < 0.0f) != (scale.y < 0.0f)) != (scale.z < 0.0f));

		for (auto& vertex : vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.pos)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.pos)),
				DirectX::XMVectorMultiply(position, scaleVector));

			const DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(
				static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertex.normal)));
			DirectX::XMStoreFloat3(
				static_cast<DirectX::XMFLOAT3*>(static_cast<void*>(&vertex.normal)),
				DirectX::XMVector3Normalize(DirectX::XMVectorMultiply(normal, inverseScaleVector)));

			const DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(
				DirectX::XMVectorMultiply(
					DirectX::XMLoadFloat4(
						static_cast<const DirectX::XMFLOAT4*>(static_cast<const void*>(&vertex.tangent))),
					scaleVector));
			const float tangentW = (flipHandedness ? -vertex.tangent.w : vertex.tangent.w);
			DirectX::XMStoreFloat4(
				static_cast<DirectX::XMFLOAT4*>(static_cast<void*>(&vertex.tangent)),
				DirectX::XMVectorSetW(tangent, tangentW));
		}

		return *this;
	}
}
