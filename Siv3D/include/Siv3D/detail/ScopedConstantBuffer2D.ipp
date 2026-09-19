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
	template <ShaderStage Stage>
	template <class Type>
	ScopedConstantBuffer2D<Stage>::ScopedConstantBuffer2D(const uint32 slot, const ConstantBuffer<Type>& buffer)
		: ScopedConstantBuffer2D{ slot, buffer._data(), buffer.size() } {}
}
