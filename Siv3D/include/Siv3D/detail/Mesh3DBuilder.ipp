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
	//	reserve
	//
	////////////////////////////////////////////////////////////////

	inline void Mesh3DBuilder::reserve(const size_t vertexCapacity, const size_t triangleCapacity)
	{
		m_mesh.reserve(vertexCapacity, triangleCapacity);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	inline void Mesh3DBuilder::clear() noexcept
	{
		m_mesh.clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMesh
	//
	////////////////////////////////////////////////////////////////

	inline const Mesh3D& Mesh3DBuilder::getMesh() const noexcept
	{
		return m_mesh;
	}

	////////////////////////////////////////////////////////////////
	//
	//	obtainMesh
	//
	////////////////////////////////////////////////////////////////

	inline Mesh3D Mesh3DBuilder::obtainMesh() noexcept
	{
		Mesh3D mesh;
		mesh.vertices.swap(m_mesh.vertices);
		mesh.indices.swap(m_mesh.indices);
		return mesh;
	}
}
