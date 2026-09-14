//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/EngineOption.hpp>
# include <Siv3D/FunctionRef.hpp>
# include "D3D11Adapter.hpp"
# include "D3D11DeviceInfo.hpp"

namespace s3d
{
	namespace D3D11Misc
	{
		// Collects hardware candidates without creating devices, retaining the result's array capacity.
		void EnumHardwareAdapters(Array<D3D11Adapter>& result, IDXGIFactory6* pDXGIFactory6, IDXGIFactory2* pDXGIFactory2,
			DXGI_GPU_PREFERENCE GPU_Preference);

		// The synchronous enumerator runs once before hardware creation attempts.
		// Explicit software drivers do not invoke the enumerator.
		[[nodiscard]]
		D3D11DeviceInfo CreateDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			FunctionRef<void(Array<D3D11Adapter>&)> enumHardwareAdapters,
			EngineOption::D3D11Driver targetDriverType, bool useDebugLayer);
	}
}
