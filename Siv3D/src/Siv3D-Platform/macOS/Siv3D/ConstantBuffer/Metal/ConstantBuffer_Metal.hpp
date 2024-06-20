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
# include <Siv3D/IConstantBuffer.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>

namespace s3d
{
	class ConstantBuffer_Metal final : public IConstantBuffer
	{
	public:

		explicit ConstantBuffer_Metal(size_t size);

		bool _internal_init() override;

		bool _internal_update(const void* data, size_t size) override;

	private:

	};
}
