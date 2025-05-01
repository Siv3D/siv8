//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "D3D11Texture.hpp"
# include <Siv3D/Texture/TextureUtility.hpp>
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/BCnData.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<D3D11_SUBRESOURCE_DATA> MakeSubResourceData(const void* imageData, const uint32 width, const uint32 bytesPerPixel, const uint32 mipLevel)
		{
			Array<D3D11_SUBRESOURCE_DATA> subResourceData(mipLevel);

			for (uint32 i = 0; i < mipLevel; ++i)
			{
				const uint32 mipmapWidth = Max((width >> i), 1u);
				const uint32 bytesPerRow = (((mipmapWidth * bytesPerPixel) + 3) & ~3);
				subResourceData[i] = { imageData, bytesPerRow }; // 0 < i における imageData は仮のデータ。あとで generateMips() で上書きする
			}

			return subResourceData;
		}

		static void Error_Texture2D(const HRESULT hr)
		{
			LOG_FAIL(fmt::format("❌ D3D11Texture::D3D11Texture(): Failed to create Texture2D. [Error {:#X}]", static_cast<uint32>(hr)));
		}

		static void Error_ShaderResourceView(const HRESULT hr)
		{
			LOG_FAIL(fmt::format("❌ D3D11Texture::D3D11Texture(): Failed to create ShaderResourceView. [Error {:#X}]", static_cast<uint32>(hr)));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

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
			initData[0] = { image.data(), image.bytesPerRow(), 0 };
			
			for (uint32 i = 0; i < mipmaps.size(); ++i)
			{
				initData[i + 1] = { mipmaps[i].data(), mipmaps[i].bytesPerRow() };
			}
		}

		// [メインテクスチャ] を作成
		{
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);
			
			if (HRESULT hr = device->CreateTexture2D(&textureDesc, initData.data(), &m_texture);
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

	D3D11Texture::D3D11Texture(NoMipmap, ID3D11Device* device, const Size& size, std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			size,
			1,
			1, 0,
			format,
			false
		}
	{
		const uint32 bytesPerRow = format.bytesPerRow(size.x);
		
		if (const size_t expectedSize = (bytesPerRow * size.y); 
			data.size_bytes() != expectedSize)
		{
			LOG_FAIL(fmt::format("❌ D3D11Texture::D3D11Texture(): size_bytes[{}] != expectedSize[{}]", data.size_bytes(), expectedSize));
			return;
		}

		// [メインテクスチャ] を作成
		{
			const D3D11_SUBRESOURCE_DATA initData{ data.data(), bytesPerRow, 0 };
			
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, &initData, &m_texture);
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

	D3D11Texture::D3D11Texture(GenerateMipmap, ID3D11Device* device, ID3D11DeviceContext* context, const Size& size, std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			size,
			ImageProcessing::CalculateMipmapLevel(size.x, size.y),
			1, 0,
			format,
			false
		}
	{
		const uint32 bytesPerRow = format.bytesPerRow(size.x);
		
		if (const size_t expectedSize = (bytesPerRow * size.y); 
			data.size_bytes() != expectedSize)
		{
			LOG_FAIL(fmt::format("❌ D3D11Texture::D3D11Texture(): size_bytes[{}] != expectedSize[{}]", data.size_bytes(), expectedSize));
			return;
		}

		// [メインテクスチャ] を作成
		{
			const Array<D3D11_SUBRESOURCE_DATA> initData = MakeSubResourceData(data.data(), size.x, m_desc.format.pixelSize(), m_desc.mipLevels);
			
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC((D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET),
				D3D11_USAGE_DEFAULT, 0, { 1, 0 }, D3D11_RESOURCE_MISC_GENERATE_MIPS);

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, initData.data(), &m_texture);
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

	D3D11Texture::D3D11Texture(ID3D11Device* device, const BCnData& bcnData)
		: m_desc{ ((1 < bcnData.textures.size()) ? TextureDesc::Mipmap : TextureDesc::NoMipmap),
			TextureType::Default,
			bcnData.size,
			static_cast<uint8>(bcnData.textures.size()),
			1, 0,
			bcnData.format,
			false
		}
	{
		const uint32 blockSize = bcnData.format.blockSize();

		Array<D3D11_SUBRESOURCE_DATA> initDataList(m_desc.mipLevels);

		for (uint32 i = 0; i < bcnData.textures.size(); ++i)
		{
			const uint32 mipWidth = Max<uint32>((bcnData.size.x >> i), 1);
			const uint32 paddedWidth = ((mipWidth + 3) & ~3);
			const uint32 xBlocks = (paddedWidth / 4);

			initDataList[i] =
			{
				.pSysMem = bcnData.textures[i].data(),
				.SysMemPitch = (xBlocks * blockSize),
				.SysMemSlicePitch = 0
			};
		}

		// [メインテクスチャ] を作成
		{
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, initDataList.data(), &m_texture);
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

	D3D11Texture::D3D11Texture(Dynamic, NoMipmap, ID3D11Device* device, const Size& size, std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Dynamic,
			size,
			1,
			1, 0,
			format,
			false
		}
	{
		const uint32 bytesPerRow = format.bytesPerRow(size.x);
		const D3D11_SUBRESOURCE_DATA initData = { data.data(), bytesPerRow, 0};
		
		// [メインテクスチャ] を作成
		{
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC(D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DEFAULT);

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, (data.empty() ? nullptr : &initData), &m_texture);
				FAILED(hr))
			{
				Error_Texture2D(hr);
				return;
			}
		}

		// [ステージング・テクスチャ] を作成
		{
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC(0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_WRITE);

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, &m_stagingTexture);
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

	D3D11Texture::D3D11Texture(Dynamic, GenerateMipmap, ID3D11Device* device, ID3D11DeviceContext* context, const Size& size, std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Dynamic,
			size,
			ImageProcessing::CalculateMipmapLevel(size.x, size.y),
			1, 0,
			format,
			false
		}
	{
		// [メインテクスチャ] を作成
		{
			const Array<D3D11_SUBRESOURCE_DATA> initData = MakeSubResourceData(data.data(), size.x, m_desc.format.pixelSize(), m_desc.mipLevels);
			const D3D11_TEXTURE2D_DESC textureDesc = m_desc.makeD3D11_TEXTURE2D_DESC((D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET),
				D3D11_USAGE_DEFAULT, 0, { 1, 0 }, D3D11_RESOURCE_MISC_GENERATE_MIPS);

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, (data.empty() ? nullptr : initData.data()), &m_texture);
				FAILED(hr))
			{
				Error_Texture2D(hr);
				return;
			}
		}

		// [ステージング・テクスチャ] を作成
		{
			const D3D11Texture2DDesc stagingDesc
			{
				desc,
				TextureType::Default,
				size,
				1u,
				1, 0,
				format,
				false
			};

			const D3D11_TEXTURE2D_DESC textureDesc = stagingDesc.makeD3D11_TEXTURE2D_DESC(0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_WRITE);

			const uint32 bytesPerRow = format.bytesPerRow(size.x);
			const D3D11_SUBRESOURCE_DATA initData = { data.data(), bytesPerRow, 0};

			if (HRESULT hr = device->CreateTexture2D(&textureDesc, (data.empty() ? nullptr : &initData), &m_stagingTexture);
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

		if (not data.empty())
		{
			generateMipmaps(context);
		}
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
	//	fill
	//
	////////////////////////////////////////////////////////////////

	bool D3D11Texture::fill(ID3D11DeviceContext* context, const ColorF& color, const bool wait)
	{
		if (m_desc.type != TextureType::Dynamic)
		{
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE mapped;
		{
			const UINT flag = (wait ? 0 : D3D11_MAP_FLAG_DO_NOT_WAIT);

			if (FAILED(context->Map(m_stagingTexture.Get(), 0, D3D11_MAP_WRITE, flag, &mapped)))
			{
				return false;
			}

			if (mapped.pData == nullptr)
			{
				context->Unmap(m_stagingTexture.Get(), 0);
				return false;
			}

			FillWithColor(mapped.pData, (mapped.RowPitch * m_desc.size.x), color, m_desc.format);

			context->Unmap(m_stagingTexture.Get(), 0);
		}

		if (m_desc.hasMipmap)
		{
			context->CopySubresourceRegion(m_texture.Get(), 0, 0, 0, 0, m_stagingTexture.Get(), 0, nullptr);
		}
		else
		{
			context->CopyResource(m_texture.Get(), m_stagingTexture.Get());
		}

		return true;
	}

	bool D3D11Texture::fill(ID3D11DeviceContext* context, const std::span<const Byte> data, const uint32 srcBytesPerRow, const bool wait)
	{
		if (m_desc.type != TextureType::Dynamic)
		{
			return false;
		}

		return(false);
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
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	ID3D11Texture2D* D3D11Texture::getTexture()
	{
		return m_texture.Get();
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
