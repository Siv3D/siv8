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

# include "CTexture_Metal.hpp"
# include <Siv3D/Renderer/Metal/CRenderer_Metal.hpp>
# include <Siv3D/Error/InternalEngineError.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/ImageFormat/BCnDecoder.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CTexture_Metal::CTexture_Metal() {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CTexture_Metal::~CTexture_Metal()
	{
		LOG_SCOPED_DEBUG("CTexture_Metal::~CTexture_Metal()");

		m_textures.destroy();
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::init()
	{
		LOG_SCOPED_DEBUG("CTexture_Metal::init()");

		m_pRenderer		= static_cast<CRenderer_Metal*>(SIV3D_ENGINE(Renderer));
		m_device		= m_pRenderer->getDevice();
		m_commandQueue	= m_pRenderer->getCommandQueue();

		// null Texture を管理に登録
		{
			const Image image{ 16, Palette::Yellow };
			const Array<Image> mipmaps = {
				Image{ 8, Palette::Yellow }, Image{ 4, Palette::Yellow },
				Image{ 2, Palette::Yellow }, Image{ 1, Palette::Yellow }
			};

			// null Texture を作成
			auto nullTexture = std::make_unique<MetalTexture>(m_device, m_commandQueue, image, mipmaps, TextureDesc::Mipmap);

			if (not nullTexture->isInitialized()) // もし作成に失敗していたら
			{
				throw InternalEngineError{ "Failed to create a null texture" };
			}

			// 管理に登録
			m_textures.setNullData(std::move(nullTexture));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	create
	//
	////////////////////////////////////////////////////////////////

	Texture::IDType CTexture_Metal::create(IReader&& reader, FilePathView pathHint, const TextureDesc desc)
	{
		if (not reader.isOpen())
		{
			return Texture::IDType::Null();
		}

		const BCnDecoder bcnDecoder{};
		const bool isBCn = bcnDecoder.getImageInfo(reader).has_value();

		if (isBCn)
		{
			return create(bcnDecoder.decodeNative(reader, desc.sRGB, pathHint));
		}
		else
		{
			const Image image{ std::move(reader) };
			return create(image.size(), std::as_bytes(std::span{ image }), (desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm), desc);
		}
	}

	Texture::IDType CTexture_Metal::create(const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
	{
		if (not image)
		{
			return Texture::IDType::Null();
		}

		std::unique_ptr<MetalTexture> texture;

		if ((not desc.hasMipmap) || (image.size() == Size{ 1, 1 }) || mipmaps.isEmpty())
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::NoMipmap{}, m_device, m_commandQueue, image, desc);
		}
		else
		{
			texture = std::make_unique<MetalTexture>(m_device, m_commandQueue, image, mipmaps, desc);
		}

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::create(const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return Texture::IDType::Null();
		}
		
		std::unique_ptr<MetalTexture> texture;
		
		if ((not desc.hasMipmap) || (size == Size{ 1, 1 }))
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::NoMipmap{}, m_device, m_commandQueue, size, data, format, desc);
		}
		else
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::GenerateMipmap{}, m_device, m_commandQueue, size, data, format, desc);
		}
		
		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::create(const BCnData& bcnData)
	{
		if (not bcnData)
		{
			return Texture::IDType::Null();
		}
		
		std::unique_ptr<MetalTexture> texture = std::make_unique<MetalTexture>(m_device, m_commandQueue, bcnData);
		
		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	Texture::IDType CTexture_Metal::createDynamic(const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
	{
		if ((size.x <= 0) || (size.y <= 0))
		{
			return Texture::IDType::Null();
		}
		
		std::unique_ptr<MetalTexture> texture;
		
		if ((not desc.hasMipmap) || (size == Size{ 1, 1 }))
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::Dynamic{}, MetalTexture::NoMipmap{}, m_device, m_commandQueue, size, data, format, desc);
		}
		else
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::Dynamic{}, MetalTexture::GenerateMipmap{}, m_device, m_commandQueue, size, data, format, desc);
		}
		
		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}
			
		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
	}

	////////////////////////////////////////////////////////////////
	//
	//	release
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::release(const Texture::IDType handleID)
	{
		m_textures.erase(handleID);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSize
	//
	////////////////////////////////////////////////////////////////

	Size CTexture_Metal::getSize(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().size;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getMipLevels
	//
	////////////////////////////////////////////////////////////////

	uint32 CTexture_Metal::getMipLevels(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().mipLevels;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDesc
	//
	////////////////////////////////////////////////////////////////

	TextureDesc CTexture_Metal::getDesc(const Texture::IDType handleID)
	{
		const auto& desc = m_textures[handleID]->getDesc();
		return{ desc.hasMipmap, desc.sRGB, desc.isSDF };
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFormat
	//
	////////////////////////////////////////////////////////////////

	TextureFormat CTexture_Metal::getFormat(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getDesc().format;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasDepth
	//
	////////////////////////////////////////////////////////////////

	bool CTexture_Metal::hasDepth(const Texture::IDType handleID)
	{
		return m_textures[handleID]->hasDepth();
	}

	////////////////////////////////////////////////////////////////
	//
	//	fill
	//
	////////////////////////////////////////////////////////////////

	bool CTexture_Metal::fill(const Texture::IDType handleID, const ColorF& color, const bool wait)
	{
		return m_textures[handleID]->fill(m_commandQueue, color, wait);
	}

	bool CTexture_Metal::fill(const Texture::IDType handleID, const std::span<const Byte> src, const uint32 srcBytesPerRow, const bool wait)
	{
		return m_textures[handleID]->fill(m_commandQueue, src, srcBytesPerRow, wait);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fillRegion
	//
	////////////////////////////////////////////////////////////////

	bool CTexture_Metal::fillRegion(const Texture::IDType handleID, const ColorF& color, const Rect& rect)
	{
		return(false);
	}

	bool CTexture_Metal::fillRegion(const Texture::IDType handleID, const std::span<const Byte> src, const uint32 srcBytesPerRow, const Rect& rect, const bool wait)
	{
		return(false);
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateMips
	//
	////////////////////////////////////////////////////////////////

	void CTexture_Metal::generateMips(const Texture::IDType handleID)
	{
		return m_textures[handleID]->generateMips(m_commandQueue);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	MTL::Texture* CTexture_Metal::getTexture(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getTexture();
	}
}
