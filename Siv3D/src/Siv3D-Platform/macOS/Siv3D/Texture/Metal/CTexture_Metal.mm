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
			auto nullTexture = std::make_unique<MetalTexture>(m_device, image, mipmaps, TextureDesc::Mipmap);

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

	Texture::IDType CTexture_Metal::create(const Image& image, const TextureDesc desc)
	{
		if (not image)
		{
			return Texture::IDType::Null();
		}

		std::unique_ptr<MetalTexture> texture;

		if ((not desc.hasMipmap) || (image.size() == Size{ 1, 1 }))
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::NoMipmap{}, m_device, image, desc);
		}
		else
		{
			texture = std::make_unique<MetalTexture>(MetalTexture::GenerateMipmap{}, m_device, m_commandQueue, image, desc);
		}

		if (not texture->isInitialized())
		{
			return Texture::IDType::Null();
		}

		const String info = texture->getDesc().toString();
		return m_textures.add(std::move(texture), info);
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
			texture = std::make_unique<MetalTexture>(MetalTexture::NoMipmap{}, m_device, image, desc);
		}
		else
		{
			texture = std::make_unique<MetalTexture>(m_device, image, mipmaps, desc);
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
		return m_textures[handleID]->getDesc();
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
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	MTL::Texture* CTexture_Metal::getTexture(const Texture::IDType handleID)
	{
		return m_textures[handleID]->getTexture();
	}
}
