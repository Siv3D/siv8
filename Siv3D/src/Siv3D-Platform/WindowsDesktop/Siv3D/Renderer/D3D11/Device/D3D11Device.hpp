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
# include "D3D11DeviceInfo.hpp"

namespace s3d
{
	class D3D11Device
	{
	public:

		void init();

	private:

		PFN_D3D11_CREATE_DEVICE m_pD3D11CreateDevice = nullptr;

		decltype(CreateDXGIFactory1)* m_pCreateDXGIFactory1 = nullptr;

		ComPtr<IDXGIFactory2> m_DXGIFactory2;

		ComPtr<IDXGIFactory6> m_DXGIFactory6;

		Array<D3D11Adapter> m_hardwareAdapters;

		D3D11DeviceInfo m_deviceInfo;

		bool m_hasDebugLayer = false;
	};
}
