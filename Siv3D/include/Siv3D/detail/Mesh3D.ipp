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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	inline Mesh3D::Mesh3D(size_t vertexCount, size_t triangleCount)
		: vertices(vertexCount)
		, indices(triangleCount) {}

	inline Mesh3D::Mesh3D(Array<Vertex3D> _vertices, Array<TriangleIndex32> _indices)
		: vertices(std::move(_vertices))
		, indices(std::move(_indices)) {}

	inline Mesh3D::Mesh3D(std::span<const Vertex3D> _vertices, std::span<const TriangleIndex32> _indices)
		: vertices(_vertices.begin(), _vertices.end())
		, indices(_indices.begin(), _indices.end()) {}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	inline bool Mesh3D::isEmpty() const noexcept
	{
		return (vertices.empty() || indices.empty());
	}
}
