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

# include <Siv3D/IConstantBuffer.hpp>
# include "Metal/ConstantBuffer_Metal.hpp"

namespace s3d
{
	std::unique_ptr<IConstantBuffer> IConstantBuffer::Create(const size_t size)
	{
		return std::make_unique<ConstantBuffer_Metal>(size);
	}
}
