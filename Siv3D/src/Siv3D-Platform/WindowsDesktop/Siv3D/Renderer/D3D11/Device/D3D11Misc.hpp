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
# include "D3D11Adapter.hpp"

namespace s3d
{
	namespace D3D11Misc
	{
		[[nodiscard]]
		Array<D3D11Adapter> GetAdapters(IDXGIFactory2* pDXGIFactory2, PFN_D3D11_CREATE_DEVICE pD3D11CreateDevice, bool GPU_Preference_HighPerformance);
	}
}
