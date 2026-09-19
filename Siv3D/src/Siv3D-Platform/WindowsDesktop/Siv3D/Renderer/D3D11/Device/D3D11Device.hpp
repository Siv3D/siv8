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

	private:

		ComPtr<IDXGIFactory2> m_DXGIFactory2;

		D3D11DeviceInfo m_deviceInfo;
	};
}
