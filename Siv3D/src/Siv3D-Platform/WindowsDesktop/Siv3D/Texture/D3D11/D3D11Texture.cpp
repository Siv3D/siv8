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
# include <Siv3D/ImageProcessing.hpp>
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

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	D3D11Texture::D3D11Texture(NoMipmap, ID3D11Device* device, const Image& image, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			1,
			1, 0,
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm),
			false
		}
	{
		// [メインテクスチャ] を作成
		{
			const Array<D3D11_SUBRESOURCE_DATA> initData = MakeSubResourceData(image.data(), image.width(), m_desc.format.pixelSize(), m_desc.mipLevels);

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

	D3D11Texture::D3D11Texture(GenerateMipmap, ID3D11Device* device, ID3D11DeviceContext* context, const Image& image, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			ImageProcessing::CalculateMipmapLevel(image.width(), image.height()),
			1, 0,
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm),
			false
		}
	{
		// [メインテクスチャ] を作成
		{
			const Array<D3D11_SUBRESOURCE_DATA> initData = MakeSubResourceData(image.data(), image.width(), m_desc.format.pixelSize(), m_desc.mipLevels);
			const D3D11_TEXTURE2D_DESC d3d11Desc = m_desc.makeD3D11_TEXTURE2D_DESC((D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET),
				D3D11_USAGE_DEFAULT, 0, { 1, 0 }, D3D11_RESOURCE_MISC_GENERATE_MIPS);
			
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

		generateMipmaps(context);
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

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool D3D11Texture::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDesc
	//
	////////////////////////////////////////////////////////////////

	const D3D11Texture2DDesc& D3D11Texture::getDesc() const noexcept
	{
		return m_desc;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasDepth
	//
	////////////////////////////////////////////////////////////////

	bool D3D11Texture::hasDepth() const noexcept
	{
		return m_desc.hasDepth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateMipmaps
	//
	////////////////////////////////////////////////////////////////

	void D3D11Texture::generateMipmaps(ID3D11DeviceContext* context)
	{
		if (m_desc.mipLevels <= 1)
		{
			return;
		}

		context->GenerateMips(m_shaderResourceView.Get());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSRVPtr
	//
	////////////////////////////////////////////////////////////////

	ID3D11ShaderResourceView** D3D11Texture::getSRVPtr()
	{
		return m_shaderResourceView.GetAddressOf();
	}
}
