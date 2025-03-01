//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Vertex2D.hpp>

namespace s3d
{
	struct BatchInfo2D
	{
		uint32 indexCount = 0;

		uint32 startIndexLocation = 0;

		uint32 baseVertexLocation = 0;
	};
}
