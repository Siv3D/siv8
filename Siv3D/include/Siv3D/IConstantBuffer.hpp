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
# include <memory>
# include "Common.hpp"

namespace s3d
{
	class IConstantBuffer
	{
	public:

		static std::unique_ptr<IConstantBuffer> Create(size_t size);

		virtual ~IConstantBuffer() = default;

		virtual bool _internal_init() = 0;

		virtual bool _internal_update(const void* data, size_t size) = 0;
	};
}
