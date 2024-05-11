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
# include <Siv3D/Array.hpp>
# include <Siv3D/EngineOption.hpp>
# include "D3D11Adapter.hpp"
# include "D3D11DeviceInfo.hpp"

namespace s3d
{
	namespace D3D11Misc
	{
		[[nodiscard]]
		Array<D3D11Adapter> EnumHardwareAdapters(IDXGIFactory6* pDXGIFactory6, IDXGIFactory2* pDXGIFactory2, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, DXGI_GPU_PREFERENCE GPU_Preference);

		[[nodiscard]]
		D3D11DeviceInfo CreateDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, const Array<D3D11Adapter>& hardwareAdapters,
			EngineOption::D3D11Driver targetDriverType, bool useDebugLayer);
	}
}
