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

# include "D3D11InternalTexture2D.hpp"
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	[[nodiscard]]
	D3D11InternalTexture2D D3D11InternalTexture2D::CreateFromSwapChain(ID3D11Device* device, IDXGISwapChain1* swapChain1)
	{
		LOG_SCOPED_DEBUG("D3D11InternalTexture2D::CreateFromSwapChain()");

		D3D11InternalTexture2D texture;

		// バックバッファのサイズを取得
		{
			DXGI_SWAP_CHAIN_DESC1 desc{};

			if (FAILED(swapChain1->GetDesc1(&desc)))
			{
				throw InternalEngineError{ "IDXGISwapChain1::GetDesc1() failed" };
			}

			texture.m_size.set(desc.Width, desc.Height);
			LOG_DEBUG(fmt::format("Back buffer size = {}", texture.m_size));
		}

		if (FAILED(swapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture.m_texture)))
		{
			throw InternalEngineError{ "IDXGISwapChain1::GetBuffer() failed" };
		}

		if (FAILED(device->CreateRenderTargetView(texture.m_texture.Get(), nullptr, &texture.m_renderTargetView)))
		{
			throw InternalEngineError{ "ID3D11Device::CreateRenderTargetView() failed" };
		}

		return texture;
	}
}
