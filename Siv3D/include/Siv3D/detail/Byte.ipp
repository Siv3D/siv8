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
	//	AsUint8
	//
	////////////////////////////////////////////////////////////////

	constexpr uint8 AsUint8(const Byte value) noexcept
	{
		return static_cast<uint8>(value);
	}
}
