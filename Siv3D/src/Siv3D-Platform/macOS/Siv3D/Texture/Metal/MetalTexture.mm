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

# include "MetalTexture.hpp"
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalTexture::MetalTexture(NoMipmap, MTL::Device* device, const Image& image, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			1,
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
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const MTL::Region region = MTL::Region::Make2D(0, 0, image.width(), image.height());
		m_texture->replaceRegion(region, 0, image.data(), (image.width() * m_desc.format.pixelSize()));
		
		m_initialized = true;
	}

	MetalTexture::MetalTexture(GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			ImageProcessing::CalculateMipmapLevel(image.width(), image.height()),
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
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const MTL::Region region = MTL::Region::Make2D(0, 0, image.width(), image.height());
		m_texture->replaceRegion(region, 0, image.data(), (image.width() * m_desc.format.pixelSize()));
		
		//NS::SharedPtr<MTL::CommandQueue> commandQueue = NS::TransferPtr(device->newCommandQueue());
		NS::SharedPtr<MTL::CommandBuffer> commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		NS::SharedPtr<MTL::BlitCommandEncoder> blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			blitCommandEncoder->generateMipmaps(m_texture.get());
			blitCommandEncoder->endEncoding();
		}

		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
		
		m_initialized = true;
	}

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
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const MTL::Region region = MTL::Region::Make2D(0, 0, image.width(), image.height());
		m_texture->replaceRegion(region, 0, image.data(), (image.width() * m_desc.format.pixelSize()));
		
		for (uint32 i = 0; i < mipmaps.size(); ++i)
		{
			const Image& mipmap = mipmaps[i];
			const MTL::Region region = MTL::Region::Make2D(0, 0, mipmap.width(), mipmap.height());
			m_texture->replaceRegion(region, (i + 1), mipmap.data(), (mipmap.width() * m_desc.format.pixelSize()));
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

	////////////////////////////////////////////////////////////////
	//
	//	hasDepth
	//
	////////////////////////////////////////////////////////////////

	bool MetalTexture::hasDepth() const noexcept
	{
		return m_desc.hasDepth;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	MTL::Texture* MetalTexture::getTexture() const noexcept
	{
		return m_texture.get();
	}
}
