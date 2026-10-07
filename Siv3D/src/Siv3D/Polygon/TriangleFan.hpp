//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# pragma once
# include <Siv3D/Array.hpp>
# include <Siv3D/TriangleIndex.hpp>

namespace s3d::detail
{
	// 頂点 0 を共有する、時計回りの凸多角形の三角形分割。vertexCount は 3 以上。
	[[nodiscard]]
	inline Array<TriangleIndex> MakeTriangleFan(const size_t vertexCount)
	{
		Array<TriangleIndex> indices(vertexCount - 2);

		for (size_t i = 0; i < indices.size(); ++i)
		{
			indices[i] = { 0, static_cast<Vertex2D::IndexType>(i + 1), static_cast<Vertex2D::IndexType>(i + 2) };
		}

		return indices;
	}
}
