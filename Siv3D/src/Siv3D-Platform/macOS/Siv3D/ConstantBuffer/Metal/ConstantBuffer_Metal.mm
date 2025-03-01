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

# include "ConstantBuffer_Metal.hpp"
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	ConstantBuffer_Metal::ConstantBuffer_Metal(const size_t) {}

	bool ConstantBuffer_Metal::_internal_init()
	{
		return true;
	}

	bool ConstantBuffer_Metal::_internal_update(const void*, const size_t)
	{
		return true;
	}
}
