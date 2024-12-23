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

# include "D3D11Texture.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<D3D11_SUBRESOURCE_DATA> MakeSubResourceData(const void* imageData, size_t width, const size_t pixelSize, const uint32 mipLevel)
		{
			Array<D3D11_SUBRESOURCE_DATA> subResourceData(mipLevel);

			for (uint32 i = 0; i < mipLevel; ++i)
			{
				// ミップマップにおける imageData は仮のデータ。あとで generateMips() で上書きする
				subResourceData[i] = { imageData, static_cast<uint32>(width * pixelSize) };

				width = Max<size_t>((width / 2), 1);
			}

			return subResourceData;
		}

		static void Error_Texture2D(const HRESULT hr)
		{
			LOG_FAIL(U"❌ D3D11Texture::D3D11Texture() : Failed to create Texture2D. [Error {:#X}]"_fmt(hr));
		}

		static void Error_ShaderResourceView(const HRESULT hr)
		{
			LOG_FAIL(U"❌ D3D11Texture::D3D11Texture() : Failed to create ShaderResourceView. [Error {:#X}]"_fmt(hr));
		}
	}

	D3D11Texture::D3D11Texture(ID3D11Device* device, const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			static_cast<uint8>(mipmaps.size() + 1),
			1, 0,
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm),
			false
		}
	{
		Array<D3D11_SUBRESOURCE_DATA> initData(m_desc.mipLevels);
		{
			initData[0] = { image.data(), image.stride(), 0 };
			
			for (uint32 i = 0; i < mipmaps.size(); ++i)
			{
				initData[i + 1] = { mipmaps[i].data(), mipmaps[i].stride() };
			}
		}

		// [メインテクスチャ] を作成
		{
			const D3D11_TEXTURE2D_DESC d3d11Desc = m_desc.makeD3D11_TEXTURE2D_DESC(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);
			
			if (HRESULT hr = device->CreateTexture2D(&d3d11Desc, initData.data(), &m_texture);
				FAILED(hr))
			{
				Error_Texture2D(hr);
				return;
			}
		}

		// [シェーダ・リソース・ビュー] を作成
		{
			const D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = m_desc.makeD3D11_SHADER_RESOURCE_VIEW_DESC();
			
			if (HRESULT hr = device->CreateShaderResourceView(m_texture.Get(), &srvDesc, &m_shaderResourceView);
				FAILED(hr))
			{
				Error_ShaderResourceView(hr);
				return;
			}
		}

		m_initialized = true;
	}

	bool D3D11Texture::isInitialized() const noexcept
	{
		return m_initialized;
	}

}
