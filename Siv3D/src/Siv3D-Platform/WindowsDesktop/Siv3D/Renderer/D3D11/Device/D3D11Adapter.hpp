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
# include <Siv3D/String.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>
# include <Siv3D/Windows/ComPtr.hpp>

namespace s3d
{
	struct D3D11Adapter
	{
		ComPtr<IDXGIAdapter> pAdapter;

		String name;

		D3D_FEATURE_LEVEL maxLevel = D3D_FEATURE_LEVEL_9_1;

		D3D_FEATURE_LEVEL selectedLevel = D3D_FEATURE_LEVEL_9_1;

		uint32 adapterIndex = 0;

		enum class Vendor : uint8
		{
			NVIDIA,

			AMD,

			Intel,

			Microsoft,

			Unknown,

		} vendor = Vendor::Unknown;

		bool d3d11_1_runtimeSupport = false;

		bool computeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x = false;
	};
}
