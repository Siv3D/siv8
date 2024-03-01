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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	SwapEndian
	//
	////////////////////////////////////////////////////////////////

	constexpr uint16 SwapEndian(const uint16 value) noexcept
	{
		return std::byteswap(value);
	}

	constexpr uint32 SwapEndian(const uint32 value) noexcept
	{
		return std::byteswap(value);
	}

	constexpr uint64 SwapEndian(const uint64 value) noexcept
	{
		return std::byteswap(value);
	}
}
