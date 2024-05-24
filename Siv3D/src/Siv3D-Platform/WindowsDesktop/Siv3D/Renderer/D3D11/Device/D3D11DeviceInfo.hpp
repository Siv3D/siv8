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
# include <Siv3D/Optional.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	struct D3D11DeviceInfo
	{
		ComPtr<ID3D11Device> device;

		ComPtr<IDXGIDevice1> dxgiDevice;

		ComPtr<ID3D11DeviceContext> context;

		Optional<uint32> adapterIndex;

		D3D_DRIVER_TYPE deviceType = D3D_DRIVER_TYPE_UNKNOWN;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_9_1;
	};
}
