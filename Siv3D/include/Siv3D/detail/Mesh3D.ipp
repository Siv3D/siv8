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

	////////////////////////////////////////////////////////////////
	//
	//	vertexCount
	//
	////////////////////////////////////////////////////////////////

	inline size_t Mesh3D::vertexCount() const noexcept
	{
		return vertices.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	triangleCount
	//
	////////////////////////////////////////////////////////////////

	inline size_t Mesh3D::triangleCount() const noexcept
	{
		return indices.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	Loft
	//
	////////////////////////////////////////////////////////////////

	template <size_t SectionCount, size_t VertexCount>
		requires ((2 <= SectionCount) && (3 <= VertexCount))
	inline Mesh3D Mesh3D::Loft(
		const std::array<std::array<Vec2, VertexCount>, SectionCount>& sections,
		const std::array<double, SectionCount>& heights,
		const Vec2 uvScale,
		const Vec2 uvOffset)
	{
		std::array<std::span<const Vec2>, SectionCount> sectionViews;
		for (size_t i = 0; i < SectionCount; ++i)
		{
			sectionViews[i] = sections[i];
		}

		return Loft(sectionViews, heights, uvScale, uvOffset);
	}
}
