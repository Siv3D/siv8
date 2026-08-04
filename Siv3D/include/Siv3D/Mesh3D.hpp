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


# include "Common.hpp"
# include "Array.hpp"
# include "Vertex3D.hpp"
# include "TriangleIndex32.hpp"


namespace s3d
{
	struct Mesh3D
	{
		Array<Vertex3D> vertices;

		Array<TriangleIndex32> indices;

		[[nodiscard]]
		Mesh3D() = default;

		[[nodiscard]]
		Mesh3D(size_t vertexCount, size_t triangleCount);

		[[nodiscard]]
		Mesh3D(Array<Vertex3D> _vertices, Array<TriangleIndex32> _indices);
	};
}
