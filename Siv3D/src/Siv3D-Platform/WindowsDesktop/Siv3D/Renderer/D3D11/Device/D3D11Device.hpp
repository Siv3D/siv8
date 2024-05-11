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

		[[nodiscard]]
		IDXGIFactory2* getDXGIFactory2() const noexcept
		{
			return m_DXGIFactory2.Get();
		}

		[[nodiscard]]
		IDXGIFactory6* getDXGIFactory6() const noexcept
		{
			return m_DXGIFactory6.Get();
		}

		[[nodiscard]]
		ID3D11Device* getDevice() const noexcept
		{
			return m_deviceInfo.device.Get();
		}

		[[nodiscard]]
		IDXGIDevice1* getDXGIDevice1() const noexcept
		{
			return m_deviceInfo.dxgiDevice.Get();
		}

		[[nodiscard]]
		ID3D11DeviceContext* getContext() const noexcept
		{
			return m_deviceInfo.context.Get();
		}

		[[nodiscard]]
		bool supportsDXGI1_4() const noexcept
		{
			return (m_DXGIFactory6 != nullptr);
		}

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
