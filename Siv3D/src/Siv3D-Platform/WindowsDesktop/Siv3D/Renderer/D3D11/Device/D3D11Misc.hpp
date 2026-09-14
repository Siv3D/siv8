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
# include "D3D11AdapterCache.hpp"
# include "D3D11DeviceInfo.hpp"

namespace s3d
{
	namespace D3D11Misc
	{
		struct HardwareAdapterList
		{
			Array<D3D11Adapter> adapters;

			// True only when a matching cache entry supplied a feature level and stopped enumeration.
			bool usedCache = false;
		};

		[[nodiscard]]
		Optional<D3D11AdapterCache> LoadAdapterCache();

		// Replaces the result while retaining its array capacity. An absent cache forces fresh probing.
		void EnumHardwareAdapters(HardwareAdapterList& result, IDXGIFactory6* pDXGIFactory6, IDXGIFactory2* pDXGIFactory2,
			PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, DXGI_GPU_PREFERENCE GPU_Preference, const Optional<D3D11AdapterCache>& cache);

		// The synchronous enumerator receives useCache=true first, then false only if cached candidates fail.
		// Explicit software drivers do not invoke the enumerator.
		[[nodiscard]]
		D3D11DeviceInfo CreateDevice(PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice,
			FunctionRef<void(HardwareAdapterList&, bool useCache)> enumHardwareAdapters,
			EngineOption::D3D11Driver targetDriverType, bool useDebugLayer);
	}
}
