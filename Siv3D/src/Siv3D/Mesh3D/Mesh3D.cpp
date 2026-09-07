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
# include <Siv3D/Mat3x2.hpp>
# include <Siv3D/Mat4x4.hpp>
# include <Siv3D/Quaternion.hpp>
# include <ThirdParty/DirectXMath/DirectXCollision.h>
# include "Mesh3DCommon.hpp"
# include "Mesh3DNormals.hpp"
# include "Mesh3DMikkTSpace.hpp"

namespace s3d
{
	namespace
	{
		using Mesh3DDetail::TransformMeshRange;

	}

	////////////////////////////////////////////////////////////////
	//
	//	computeBoundingBox
	//
	////////////////////////////////////////////////////////////////

	s3d::Box Mesh3D::computeBoundingBox() const noexcept
	{
		if (vertices.empty())
		{
			return s3d::Box{ 0.0 };
		}

		DirectX::BoundingBox boundingBox;
		DirectX::BoundingBox::CreateFromPoints(
			boundingBox,
			vertices.size(),
			static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertices.front().pos)),
			sizeof(Vertex3D));

		return s3d::Box{
			Vec3{ boundingBox.Center.x, boundingBox.Center.y, boundingBox.Center.z },
			Vec3{
				(boundingBox.Extents.x * 2.0),
				(boundingBox.Extents.y * 2.0),
				(boundingBox.Extents.z * 2.0) }
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeBoundingSphere
	//
	////////////////////////////////////////////////////////////////

	s3d::Sphere Mesh3D::computeBoundingSphere() const noexcept
	{
		if (vertices.empty())
		{
			return s3d::Sphere{ 0.0 };
		}

		DirectX::BoundingSphere boundingSphere;
		DirectX::BoundingSphere::CreateFromPoints(
			boundingSphere,
			vertices.size(),
			static_cast<const DirectX::XMFLOAT3*>(static_cast<const void*>(&vertices.front().pos)),
			sizeof(Vertex3D));

		return s3d::Sphere{
			Vec3{ boundingSphere.Center.x, boundingSphere.Center.y, boundingSphere.Center.z },
			boundingSphere.Radius
		};
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

	bool Mesh3D::append(const Mesh3D& mesh, const Mat4x4& matrix)
	{
		const size_t vertexOffset = vertices.size();
		const size_t sourceVertexCount = mesh.vertices.size();
		const size_t triangleOffset = indices.size();
		const size_t sourceTriangleCount = mesh.indices.size();

		if (not append(mesh))
		{
			return false;
		}

		if (sourceVertexCount != 0)
		{
			TransformMeshRange(*this,
				Mesh3DRange{ vertexOffset, sourceVertexCount, triangleOffset, sourceTriangleCount }, matrix);
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
		return static_cast<bool>(Mesh3DDetail::AppendMesh(*this, mesh));
	}

	Result<Mesh3DRange, Mesh3DErrorCode> Mesh3DDetail::AppendMesh(Mesh3D& destination, const Mesh3D& mesh)
	{
		if (not mesh.validate())
		{
			return Err{ Mesh3DErrorCode::InvalidGeometry };
		}

		const size_t vertexOffset = destination.vertices.size();
		const size_t sourceVertexCount = mesh.vertices.size();

		if ((Mesh3D::MaxVertexCount < vertexOffset)
			|| ((Mesh3D::MaxVertexCount - vertexOffset) < sourceVertexCount))
		{
			return Err{ Mesh3DErrorCode::SizeLimit };
		}

		const size_t triangleOffset = destination.indices.size();
		const size_t sourceTriangleCount = mesh.indices.size();

		if (&destination == &mesh)
		{
			destination.vertices.resize(vertexOffset + sourceVertexCount);

			for (size_t i = 0; i < sourceVertexCount; ++i)
			{
				destination.vertices[vertexOffset + i] = destination.vertices[i];
			}

			destination.indices.resize(triangleOffset + sourceTriangleCount);

			for (size_t i = 0; i < sourceTriangleCount; ++i)
			{
				destination.indices[triangleOffset + i] = destination.indices[i];
			}
		}
		else
		{
			destination.vertices.insert(
				destination.vertices.end(),
				mesh.vertices.begin(),
				mesh.vertices.end());

			destination.indices.insert(
				destination.indices.end(),
				mesh.indices.begin(),
				mesh.indices.end());
		}

		for (size_t i = triangleOffset; i < destination.indices.size(); ++i)
		{
			auto& triangle = destination.indices[i];

			triangle.i0 = static_cast<TriangleIndex32::value_type>(vertexOffset + triangle.i0);
			triangle.i1 = static_cast<TriangleIndex32::value_type>(vertexOffset + triangle.i1);
			triangle.i2 = static_cast<TriangleIndex32::value_type>(vertexOffset + triangle.i2);
		}

		return AddedRange(destination, vertexOffset, triangleOffset);
	}

	////////////////////////////////////////////////////////////////
	//
	//	transformUV
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::transformUV(const Mat3x2& transform) noexcept
	{
		for (auto& vertex : vertices)
		{
			vertex.tex = transform.transformPoint(vertex.tex);
		}

		return *this;
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
			return *this;
		}

		if (vertices.isEmpty())
		{
			return *this;
		}

		const bool result = Mesh3DDetail::ComputeVertexNormals(
			indices.data(),
			indices.size(),
			vertices.data(),
			vertices.size(),
			weighting);

		if (not result)
		{
			throw Error{ "Mesh3D::computeNormals(): DirectX::ComputeNormals() failed." };
		};

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	computeTangents
	//
	////////////////////////////////////////////////////////////////

	Mesh3D& Mesh3D::computeTangents()
	{
		if (not Mesh3DDetail::GenerateMikkTSpaceTangents(vertices, indices))
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
		TransformMeshRange(*this, Mesh3DRange{ 0, vertices.size(), 0, indices.size() }, matrix);
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
			reverseWinding();
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

		if (flipHandedness)
		{
			reverseWinding();
		}

		return *this;
	}
}
