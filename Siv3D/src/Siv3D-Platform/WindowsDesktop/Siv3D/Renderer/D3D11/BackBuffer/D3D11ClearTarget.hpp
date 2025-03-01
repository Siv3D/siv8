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
# include <ThirdParty/EnumBitmask/EnumBitmask.hpp>

namespace s3d
{
	enum class D3D11ClearTarget : uint8
	{
		Scene		= (1 << 0),

		BackBuffer	= (1 << 1),
	};
	DEFINE_BITMASK_OPERATORS(D3D11ClearTarget);
}
