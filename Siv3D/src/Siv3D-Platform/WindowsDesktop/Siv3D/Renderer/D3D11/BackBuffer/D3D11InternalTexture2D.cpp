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
	void D3D11InternalTexture2D::clear(ID3D11DeviceContext* context, const ColorF& color)
	{
		if (isEmpty())
		{
			return;
		}

		const Float4 rgba = color.toFloat4();

		context->ClearRenderTargetView(m_renderTargetView.Get(), rgba.getPointer());
	}

	void D3D11InternalTexture2D::reset()
	{
		m_texture.Reset();
		m_renderTargetView.Reset();
		m_shaderResourceView.Reset();
		m_size = Size{ 0, 0 };
		m_sampleDesc = { 1, 0 };
	}

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

	D3D11InternalTexture2D D3D11InternalTexture2D::CreateRenderTexture(ID3D11Device* device, const Size& size)
	{
		LOG_SCOPED_DEBUG("D3D11InternalTexture2D::CreateRenderTexture()");

		const CD3D11_TEXTURE2D_DESC desc{
			DXGI_FORMAT_R8G8B8A8_UNORM,
			static_cast<uint32>(size.x), static_cast<uint32>(size.y),
			1, 1,
			(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			D3D11_USAGE_DEFAULT,
			0,
			1, 0,
			0 };

		D3D11InternalTexture2D texture;
		{
			{
				if (FAILED(device->CreateTexture2D(&desc, nullptr, &texture.m_texture)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateTexture2D() failed" };
				}
			}

			{
				const CD3D11_RENDER_TARGET_VIEW_DESC rtDesc{ D3D11_RTV_DIMENSION_TEXTURE2D, desc.Format };

				if (FAILED(device->CreateRenderTargetView(texture.m_texture.Get(), &rtDesc, &texture.m_renderTargetView)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateRenderTargetView() failed" };
				}
			}

			{
				const CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ texture.m_texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2D, desc.Format };

				if (FAILED(device->CreateShaderResourceView(texture.m_texture.Get(), &srvDesc, &texture.m_shaderResourceView)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateShaderResourceView() failed" };
				}
			}
		
			texture.m_size = size;
		}

		return texture;
	}

	D3D11InternalTexture2D D3D11InternalTexture2D::CreateMSRenderTexture(ID3D11Device* device, const Size& size, const uint32 sampleCount)
	{
		LOG_SCOPED_DEBUG("D3D11InternalTexture2D::CreateMSRenderTexture()");

		const CD3D11_TEXTURE2D_DESC desc{
			DXGI_FORMAT_R8G8B8A8_UNORM,
			static_cast<uint32>(size.x), static_cast<uint32>(size.y),
			1, 1,
			(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			D3D11_USAGE_DEFAULT,
			0,
			sampleCount, 0,
			0 };

		D3D11InternalTexture2D texture;
		{
			{
				if (FAILED(device->CreateTexture2D(&desc, nullptr, &texture.m_texture)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateTexture2D() failed" };
				}
			}

			{
				const CD3D11_RENDER_TARGET_VIEW_DESC rtDesc{ D3D11_RTV_DIMENSION_TEXTURE2DMS, desc.Format };

				if (FAILED(device->CreateRenderTargetView(texture.m_texture.Get(), &rtDesc, &texture.m_renderTargetView)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateRenderTargetView() failed" };
				}
			}

			{
				const CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{ texture.m_texture.Get(), D3D11_SRV_DIMENSION_TEXTURE2DMS, desc.Format };

				if (FAILED(device->CreateShaderResourceView(texture.m_texture.Get(), &srvDesc, &texture.m_shaderResourceView)))
				{
					throw InternalEngineError{ "ID3D11Device::CreateShaderResourceView() failed" };
				}
			}

			texture.m_size			= size;
			texture.m_sampleDesc	= desc.SampleDesc;
		}

		return texture;
	}
}
