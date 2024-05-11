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
# include <Siv3D/Common.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	struct D3D11AdapterCache
	{
		LUID luid = {};

		UINT vendorId = 0;

		UINT deviceId = 0;

		UINT subSysId = 0;

		UINT revision = 0;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_9_1;

		uint8 unused[4] = {};
	};
	static_assert(sizeof(D3D11AdapterCache) == 32);
}
