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

# include "MetalTexture.hpp"
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	MetalTexture::MetalTexture(MTL::Device* device, const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			static_cast<uint8>(mipmaps.size() + 1),
			1,
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm),
			false
		}
	{
		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(desc.sRGB ? MTL::PixelFormatRGBA8Unorm_sRGB : MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(image.width());
		textureDescriptor->setHeight(image.height());
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		m_initialized = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInitialized
	//
	////////////////////////////////////////////////////////////////

	bool MetalTexture::isInitialized() const noexcept
	{
		return m_initialized;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDesc
	//
	////////////////////////////////////////////////////////////////

	const MetalTexture2DDesc& MetalTexture::getDesc() const noexcept
	{
		return m_desc;
	}

}
