//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Vertex2D.hpp>
# include <Siv3D/FunctionRef.hpp>
# include "Vertex2DBufferPointer.hpp"

namespace s3d
{
	using BufferCreatorFunc = FunctionRef<Vertex2DBufferPointer(Vertex2D::IndexType, Vertex2D::IndexType)>;

	namespace Vertex2DBuilder
	{
		[[nodiscard]]
		Vertex2D::IndexType BuildTriangle(const BufferCreatorFunc& bufferCreator, const Float2(&points)[3], const Float4& color);
	}
}
