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
# include <Siv3D/TriangleIndex32.hpp>
# include <Siv3D/Vertex3D.hpp>
# include <Siv3D/VertexNormalWeighting.hpp>

namespace s3d::Mesh3DDetail
{
	[[nodiscard]]
	bool ComputeVertexNormals(
		const TriangleIndex32* indices, size_t faceCount,
		Vertex3D* vertices, size_t vertexCount,
		VertexNormalWeighting weighting) noexcept;
}
