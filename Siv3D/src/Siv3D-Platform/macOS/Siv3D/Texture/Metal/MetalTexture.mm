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
# include <Siv3D/BCnData.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalTexture::MetalTexture(NoMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			1,
			1,
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm),
			false
		}
	{
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(desc.sRGB ? MTL::PixelFormatRGBA8Unorm_sRGB : MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(image.width());
		textureDescriptor->setHeight(image.height());
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const NSUInteger dataSize = (image.stride() * image.height());
		auto uploadBuffer = NS::TransferPtr(device->newBuffer(image.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size size{ static_cast<NSUInteger>(image.width()), static_cast<NSUInteger>(image.height()), 1 };
			blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, image.stride(), 0, size, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

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

	MetalTexture::MetalTexture(NoMipmap, MTL::Device* device, const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			size,
			1,
			1,
			format,
			false
		}
	{
		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const MTL::Region region = MTL::Region::Make2D(0, 0, size.x, size.y);
		const uint32 rowPitch = (((size.x * m_desc.format.pixelSize()) + 3) & ~3);
		m_texture->replaceRegion(region, 0, data.data(), rowPitch);
		
		m_initialized = true;
	}

	MetalTexture::MetalTexture(MTL::Device* device, const BCnData& bcnData)
		: m_desc{ ((1 < bcnData.textures.size()) ? TextureDesc::Mipmap : TextureDesc::NoMipmap),
			TextureType::Default,
			bcnData.size,
			static_cast<uint8>(bcnData.textures.size()),
			1,
			bcnData.format,
			false
		}
	{
		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(bcnData.size.x);
		textureDescriptor->setHeight(bcnData.size.y);
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}

		for (uint32 i = 0; i < bcnData.textures.size(); ++i)
		{
			const uint32 mipWidth = Max<uint32>((bcnData.size.x >> i), 1);
			const uint32 mipHeight = Max<uint32>((bcnData.size.y >> i), 1);
			const uint32 paddedWidth = ((mipWidth + 3) & ~3);
			const uint32 xBlocks = (paddedWidth / 4);
			const uint32 rowPitch = (xBlocks * bcnData.format.blockSize());
			
			const MTL::Region region = MTL::Region::Make2D(0, 0, mipWidth, mipHeight);
			m_texture->replaceRegion(region, i, bcnData.textures[i].data(), rowPitch);
		}
		
		m_initialized = true;
	}

	MetalTexture::MetalTexture(GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			size,
			ImageProcessing::CalculateMipmapLevel(size.x, size.y),
			1,
			format,
			false
		}
	{
		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const MTL::Region region = MTL::Region::Make2D(0, 0, size.x, size.y);
		const uint32 rowPitch = (((size.x * m_desc.format.pixelSize()) + 3) & ~3);
		m_texture->replaceRegion(region, 0, data.data(), rowPitch);
		
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

	MetalTexture::MetalTexture(Dynamic, NoMipmap, MTL::Device* device, const Size& size, const void* pData, const uint32 stride, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Dynamic,
			size,
			1,
			1,
			format,
			false
		}
	{
		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setCpuCacheMode(MTL::CPUCacheModeWriteCombined);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const MTL::Region region = MTL::Region::Make2D(0, 0, size.x, size.y);
		m_texture->replaceRegion(region, 0, pData, stride);
		
		m_initialized = true;
	}

	MetalTexture::MetalTexture(Dynamic, GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const void* pData, const uint32 stride, const TextureFormat& format, const TextureDesc desc)
	{

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
