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

# include "Metal/CRenderer_Metal.hpp"

namespace s3d
{
	ISiv3DRenderer* ISiv3DRenderer::Create()
	{
		return new CRenderer_Metal;
	}
}
