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

# include "D3D11ScreenCapture.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static bool CreateStagingTexture(ID3D11Device* device, ComPtr<ID3D11Texture2D>& stagingTexture, const Size& size)
		{
			if (stagingTexture)
			{
				stagingTexture.Reset();
			}

			const CD3D11_TEXTURE2D_DESC desc{
				DXGI_FORMAT_R8G8B8A8_UNORM,
				static_cast<uint32>(size.x), static_cast<uint32>(size.y),
				1, 1,
				0,
				D3D11_USAGE_STAGING,
				D3D11_CPU_ACCESS_READ,
				1, 0,
				0 };

			return SUCCEEDED(device->CreateTexture2D(&desc, nullptr, &stagingTexture));
		}

		[[nodiscard]]
		static bool CopyToImage(ID3D11DeviceContext* context, const ComPtr<ID3D11Texture2D>& stagingTexture, Image& image)
		{
			D3D11_MAPPED_SUBRESOURCE mapped;
			{
				if (FAILED(context->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
				{
					return false;
				}

				if (not mapped.pData)
				{
					context->Unmap(stagingTexture.Get(), 0);
					return false;
				}

				const size_t srcBytesPerRow = mapped.RowPitch;
				const size_t dstBytesPerRow = image.bytesPerRow();

				if (srcBytesPerRow == dstBytesPerRow)
				{
					std::memcpy(image.data(), mapped.pData, image.size_bytes());
				}
				else
				{
					const size_t width = image.width();
					const size_t height = image.height();

					const uint8* pSrc = static_cast<const uint8*>(mapped.pData);
					Color* pDst = image.data();

					for (size_t y = 0; y < height; ++y)
					{
						std::memcpy(pDst, pSrc, dstBytesPerRow);
						pDst += width;
						pSrc += srcBytesPerRow;
					}
				}

				context->Unmap(stagingTexture.Get(), 0);
			}

			for (auto& pixel : image)
			{
				pixel.a = 255;
			}

			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void D3D11ScreenCapture::init(const D3D11Device& device)
	{
		LOG_SCOPED_DEBUG("D3D11ScreenCapture::init()");
		m_device = device.getDevice();
		m_context = device.getContext();
	}

	////////////////////////////////////////////////////////////////
	//
	//	capture
	//
	////////////////////////////////////////////////////////////////

	bool D3D11ScreenCapture::capture(ID3D11Texture2D* texture, const Size& size)
	{
		LOG_TRACE("D3D11ScreenCapture::capture()");

		// [ステージング・テクスチャ] が未作成か異なるサイズの場合、再作成
		if (size != m_image.size())
		{
			if (not CreateStagingTexture(m_device, m_stagingTexture, size))
			{
				return false;
			}
			
			// Image をテクスチャのサイズにリサイズ
			m_image.resize(size);
		}

		// [ステージング・テクスチャ] へコピー
		{
			m_context->CopyResource(m_stagingTexture.Get(), texture);
		}

		// Image へコピー
		if (not CopyToImage(m_context, m_stagingTexture, m_image))
		{
			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getImage
	//
	////////////////////////////////////////////////////////////////

	const Image& D3D11ScreenCapture::getImage() const noexcept
	{
		return m_image;
	}
}
