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
# include <Siv3D/Texture/TextureUtility.hpp>
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
		
		const NSUInteger dataSize = (image.bytesPerRow() * image.height());
		auto uploadBuffer = NS::TransferPtr(device->newBuffer(image.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size sourceSize{ static_cast<NSUInteger>(image.width()), static_cast<NSUInteger>(image.height()), 1 };
			blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, image.bytesPerRow(), 0, sourceSize, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
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
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(desc.sRGB ? MTL::PixelFormatRGBA8Unorm_sRGB : MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(image.width());
		textureDescriptor->setHeight(image.height());
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const NSUInteger dataSize = (image.bytesPerRow() * image.height());
		auto uploadBuffer = NS::TransferPtr(device->newBuffer(image.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size sourceSize{ static_cast<NSUInteger>(image.width()), static_cast<NSUInteger>(image.height()), 1 };
			blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, image.bytesPerRow(), 0, sourceSize, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			
			blitCommandEncoder->generateMipmaps(m_texture.get());
			blitCommandEncoder->endEncoding();
		}

		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
		
		m_initialized = true;
	}

	MetalTexture::MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			image.size(),
			static_cast<uint8>(mipmaps.size() + 1),
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
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			// base
			{
				const NSUInteger dataSize = (image.bytesPerRow() * image.height());
				auto uploadBuffer = NS::TransferPtr(device->newBuffer(image.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
				
				const MTL::Size size{ static_cast<NSUInteger>(image.width()), static_cast<NSUInteger>(image.height()), 1 };
				blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, image.bytesPerRow(), 0, size, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			}
			
			// mipmaps
			for (uint32 i = 0; i < mipmaps.size(); ++i)
			{
				const Image& mipmap = mipmaps[i];
				const NSUInteger dataSize = (mipmap.bytesPerRow() * mipmap.height());
				auto uploadBuffer = NS::TransferPtr(device->newBuffer(mipmap.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
				
				const MTL::Size sourceSize{ static_cast<NSUInteger>(mipmap.width()), static_cast<NSUInteger>(mipmap.height()), 1 };
				blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, mipmap.bytesPerRow(), 0, sourceSize, m_texture.get(), 0, (i + 1), MTL::Origin{ 0, 0, 0 });
			}
			
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		m_initialized = true;
	}

	MetalTexture::MetalTexture(NoMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Default,
			size,
			1,
			1,
			format,
			false
		}
	{
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		const uint32 rowPitch = (((size.x * m_desc.format.pixelSize()) + 3) & ~3);
		const NSUInteger dataSize = (rowPitch * size.y);
		auto uploadBuffer = NS::TransferPtr(device->newBuffer(data.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size sourceSize{ static_cast<NSUInteger>(size.x), static_cast<NSUInteger>(size.y), 1 };
			blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, rowPitch, 0, sourceSize, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();
		
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
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
	
		const uint32 rowPitch = (((size.x * m_desc.format.pixelSize()) + 3) & ~3);
		const NSUInteger dataSize = (rowPitch * size.y);
		auto uploadBuffer = NS::TransferPtr(device->newBuffer(data.data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
	
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size sourceSize{ static_cast<NSUInteger>(size.x), static_cast<NSUInteger>(size.y), 1 };
			blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, rowPitch, 0, sourceSize, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			
			blitCommandEncoder->generateMipmaps(m_texture.get());
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		m_initialized = true;
	}

	MetalTexture::MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const BCnData& bcnData)
		: m_desc{ ((1 < bcnData.textures.size()) ? TextureDesc::Mipmap : TextureDesc::NoMipmap),
			TextureType::Default,
			bcnData.size,
			static_cast<uint8>(bcnData.textures.size()),
			1,
			bcnData.format,
			false
		}
	{
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(bcnData.size.x);
		textureDescriptor->setHeight(bcnData.size.y);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			for (uint32 i = 0; i < bcnData.textures.size(); ++i)
			{
				const uint32 mipWidth = Max<uint32>((bcnData.size.x >> i), 1);
				const uint32 mipHeight = Max<uint32>((bcnData.size.y >> i), 1);
				const uint32 paddedWidth = ((mipWidth + 3) & ~3);
				const uint32 xBlocks = (paddedWidth / 4);
				const uint32 rowPitch = (xBlocks * bcnData.format.blockSize());
				
				const NSUInteger dataSize = (rowPitch * mipHeight);
				auto uploadBuffer = NS::TransferPtr(device->newBuffer(bcnData.textures[i].data(), dataSize, MTL::ResourceOptionCPUCacheModeDefault));
				
				const MTL::Size sourceSize{ static_cast<NSUInteger>(mipWidth), static_cast<NSUInteger>(mipHeight), 1 };
				blitCommandEncoder->copyFromBuffer(uploadBuffer.get(), 0, rowPitch, 0, sourceSize, m_texture.get(), 0, i, MTL::Origin{ 0, 0, 0 });
			}
			
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		m_initialized = true;
	}

	MetalTexture::MetalTexture(Dynamic, NoMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Dynamic,
			size,
			1,
			1,
			format,
			false
		}
	{
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
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

		const uint32 bytesPerRow = format.bytesPerRow(size.x);
		const NSUInteger dataSize = (bytesPerRow * size.y);

		if (data.empty())
		{
			m_uploadBuffer = NS::TransferPtr(device->newBuffer(dataSize, MTL::ResourceOptionCPUCacheModeWriteCombined));
		}
		else
		{
			m_uploadBuffer = NS::TransferPtr(device->newBuffer(data.data(), dataSize, MTL::ResourceOptionCPUCacheModeWriteCombined));
		}
		
		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size sourceSize{ static_cast<NSUInteger>(size.x), static_cast<NSUInteger>(size.y), 1 };
			blitCommandEncoder->copyFromBuffer(m_uploadBuffer.get(), 0, bytesPerRow, 0, sourceSize, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

		m_initialized = true;
	}

	MetalTexture::MetalTexture(Dynamic, GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc,
			TextureType::Dynamic,
			size,
			ImageProcessing::CalculateMipmapLevel(size.x, size.y),
			1,
			format,
			false
		}
	{
		auto textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(m_desc.format.MTLPixelFormat()));
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModeShared);
		textureDescriptor->setCpuCacheMode(MTL::CPUCacheModeWriteCombined);
		textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
		textureDescriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		
		if (not m_texture)
		{
			return;
		}
	
		const uint32 bytesPerRow = format.bytesPerRow(size.x);
		const NSUInteger dataSize = (bytesPerRow * size.y);
		
		if (data.empty())
		{
			m_uploadBuffer = NS::TransferPtr(device->newBuffer(dataSize, MTL::ResourceOptionCPUCacheModeWriteCombined));
		}
		else
		{
			m_uploadBuffer = NS::TransferPtr(device->newBuffer(data.data(), dataSize, MTL::ResourceOptionCPUCacheModeWriteCombined));
		}

		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size sourceSize{ static_cast<NSUInteger>(size.x), static_cast<NSUInteger>(size.y), 1 };
			blitCommandEncoder->copyFromBuffer(m_uploadBuffer.get(), 0, bytesPerRow, 0, sourceSize, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			
			blitCommandEncoder->generateMipmaps(m_texture.get());
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();
		commandBuffer->waitUntilCompleted();

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
	//	fill
	//
	////////////////////////////////////////////////////////////////

	bool MetalTexture::fill(MTL::CommandQueue* commandQueue, const ColorF& color, bool)
	{
		if (m_desc.type != TextureType::Dynamic)
		{
			return false;
		}

		const uint32 bytesPerRow = m_desc.format.bytesPerRow(m_desc.size.x);
		const uint32 totalBytes = (bytesPerRow * m_desc.size.y);
		void* ptr = m_uploadBuffer->contents();
		
		if (not ptr)
		{
			return false;
		}

		FillWithColor(ptr, totalBytes, color, m_desc.format);

		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size size { static_cast<NSUInteger>(m_desc.size.x), static_cast<NSUInteger>(m_desc.size.y), 1 };
			blitCommandEncoder->copyFromBuffer(m_uploadBuffer.get(), 0, bytesPerRow, 0, size, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();

		return true;
	}

	bool MetalTexture::fill(MTL::CommandQueue* commandQueue, const std::span<const Byte> data, const uint32 srcBytesPerRow, bool)
	{
		if (m_desc.type != TextureType::Dynamic)
		{
			return false;
		}

		const uint32 bytesPerRow = m_desc.format.bytesPerRow(m_desc.size.x);
		void* ptr = m_uploadBuffer->contents();
		
		if (not ptr)
		{
			return false;
		}
		
		FillWithImage(ptr, m_desc.size, bytesPerRow, data, srcBytesPerRow);

		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			const MTL::Size size { static_cast<NSUInteger>(m_desc.size.x), static_cast<NSUInteger>(m_desc.size.y), 1 };
			blitCommandEncoder->copyFromBuffer(m_uploadBuffer.get(), 0, bytesPerRow, 0, size, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			blitCommandEncoder->endEncoding();
		}
		
		commandBuffer->commit();

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	generateMips
	//
	////////////////////////////////////////////////////////////////

	void MetalTexture::generateMips(MTL::CommandQueue* commandQueue)
	{
		if (m_desc.mipLevels <= 1)
		{
			return;
		}

		auto commandBuffer = NS::TransferPtr(commandQueue->commandBuffer());
		auto blitCommandEncoder = NS::TransferPtr(commandBuffer->blitCommandEncoder());
		{
			blitCommandEncoder->generateMipmaps(m_texture.get());
			blitCommandEncoder->endEncoding();
		}

		commandBuffer->commit();
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
