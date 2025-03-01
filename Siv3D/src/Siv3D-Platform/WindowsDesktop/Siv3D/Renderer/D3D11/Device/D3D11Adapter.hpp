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
# include <Siv3D/String.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	struct D3D11Adapter
	{
		ComPtr<IDXGIAdapter1> pAdapter;

		uint32 adapterIndex = 0;

		String name;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_9_1;

		enum class Vendor : uint8
		{
			NVIDIA,

			AMD,

			Intel,

			Microsoft,

			Unknown,

		} vendor = Vendor::Unknown;

		DXGI_ADAPTER_DESC1 desc{};
	};
}
