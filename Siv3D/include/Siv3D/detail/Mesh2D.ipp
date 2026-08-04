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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	inline Mesh2D::Mesh2D(const size_t vertexCount, const size_t triangleCount)
	{
		if (MaxVertexCount < vertexCount)
		{
			return;
		}

		vertices.resize(vertexCount);
		indices.resize(triangleCount);
	}

	inline Mesh2D::Mesh2D(Array<Vertex2D> _vertices, Array<TriangleIndex> _indices)
	{
		if (MaxVertexCount < _vertices.size())
		{
			return;
		}

		vertices = std::move(_vertices);
		indices = std::move(_indices);
	}

	inline Mesh2D::Mesh2D(const std::span<const Vertex2D> _vertices, const std::span<const TriangleIndex> _indices)
	{
		if (MaxVertexCount < _vertices.size())
		{
			return;
		}

		vertices.assign(_vertices.begin(), _vertices.end());
		indices.assign(_indices.begin(), _indices.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	inline bool Mesh2D::isEmpty() const noexcept
	{
		return (vertices.empty() || indices.empty());
	}
}
