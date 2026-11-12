//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "MetalTexture.hpp"
# include <Siv3D/Renderer/Metal/MetalFrameContext.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Texture/TextureUtility.hpp>
# include <Siv3D/ImageProcessing.hpp>
# include <Siv3D/Blob.hpp>
# include <Siv3D/EngineLog.hpp>
# include <cstring>
# include <Siv3D/Error/InternalEngineError.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	MetalTexture::MetalTexture(Render, MTL::Device* device, MTL::CommandBuffer* commands,
		const Size& size, const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc, const ColorF& color, const uint32 sampleCount)
		: m_desc{ desc, ((sampleCount == 1) ? TextureType::Render : TextureType::MSRender), size,
			(desc.hasMipmap ? ImageProcessing::CalculateMipmapLevel(size.x, size.y) : uint8{ 1 }), static_cast<uint8>(sampleCount), format, false }
	{
		if (sampleCount > 1)
		{
			if (((sampleCount != 2) && (sampleCount != 4)) || not device->supportsTextureSampleCount(sampleCount))
			{
				return;
			}
			// Metal exposes the additional resolve capability separately from the sample count.
			// https://developer.apple.com/documentation/metal/mtldevice/supports32bitmsaa
			const bool float32 = format.isFloat32();
			if (float32 && (not device->supports32BitMSAA()
				|| ((1 < m_desc.mipLevels) && not device->supports32BitFloatFiltering())))
			{
				return;
			}
		}
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		auto descriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		descriptor->setTextureType(MTL::TextureType2D);
		descriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(format.MTLPixelFormat()));
		descriptor->setWidth(size.x);
		descriptor->setHeight(size.y);
		descriptor->setMipmapLevelCount(m_desc.mipLevels);
		descriptor->setStorageMode(MTL::StorageModePrivate);
		descriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
		m_texture = NS::TransferPtr(device->newTexture(descriptor.get()));
		if (not m_texture)
		{
			return;
		}
		if (sampleCount > 1)
		{
			descriptor->setTextureType(MTL::TextureType2DMultisample);
			descriptor->setSampleCount(sampleCount);
			descriptor->setMipmapLevelCount(1);
			descriptor->setUsage(MTL::TextureUsageRenderTarget);
			m_multiSampleTexture = NS::TransferPtr(device->newTexture(descriptor.get()));
			if (not m_multiSampleTexture)
			{
				return;
			}
		}
		if (not data.empty())
		{
			const uint32 rowBytes = (size.x * format.pixelSize());
			const uint32 rowPitch = ((rowBytes + 255) & ~255u);
			auto upload = NS::TransferPtr(device->newBuffer(static_cast<size_t>(rowPitch) * size.y,
				MTL::ResourceStorageModeShared | MTL::ResourceCPUCacheModeWriteCombined));
			if (not upload)
			{
				return;
			}
			auto* destination = static_cast<Byte*>(upload->contents());
			for (int32 y = 0; y < size.y; ++y)
			{
				std::memcpy((destination + static_cast<size_t>(y) * rowPitch), (data.data() + static_cast<size_t>(y) * rowBytes), rowBytes);
			}
			auto* encoder = commands->blitCommandEncoder();
			if (not encoder)
			{
				return;
			}
			encoder->copyFromBuffer(upload.get(), 0, rowPitch, 0,
				MTL::Size{ static_cast<NS::UInteger>(size.x), static_cast<NS::UInteger>(size.y), 1 },
				m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
			if (1 < m_desc.mipLevels)
			{
				encoder->generateMipmaps(m_texture.get());
			}
			encoder->endEncoding();
		}
		else
		{
			clearRT(commands, color);
			if (sampleCount > 1)
			{
				resolveMSRT(commands);
			}
			if (1 < m_desc.mipLevels)
			{
				generateMips(commands);
			}
		}
		m_initialized = true;
	}

	void MetalTexture::clearRT(MTL::CommandBuffer* commands, const ColorF& color)
	{
		if ((m_desc.type != TextureType::Render) && (m_desc.type != TextureType::MSRender))
		{
			return;
		}
		auto pass = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		auto* attachment = pass->colorAttachments()->object(0);
		attachment->setTexture(m_desc.isMSAA() ? m_multiSampleTexture.get() : m_texture.get());
		attachment->setLoadAction(MTL::LoadActionClear);
		attachment->setStoreAction(MTL::StoreActionStore);
		attachment->setClearColor(MTL::ClearColor{ color.r, color.g, color.b, color.a });
		auto* encoder = commands->renderCommandEncoder(pass.get());
		if (not encoder)
		{
			throw InternalEngineError{ "RenderTexture::clear(): renderCommandEncoder() failed" };
		}
		encoder->endEncoding();
	}

	void MetalTexture::resolveMSRT(MTL::CommandBuffer* commands)
	{
		if (m_desc.type != TextureType::MSRender)
		{
			return;
		}
		auto pass = NS::TransferPtr(MTL::RenderPassDescriptor::alloc()->init());
		auto* attachment = pass->colorAttachments()->object(0);
		attachment->setTexture(m_multiSampleTexture.get());
		attachment->setResolveTexture(m_texture.get());
		attachment->setLoadAction(MTL::LoadActionLoad);
		attachment->setStoreAction(MTL::StoreActionStoreAndMultisampleResolve);
		auto* encoder = commands->renderCommandEncoder(pass.get());
		if (not encoder)
		{
			throw InternalEngineError{ "MSRenderTexture::resolve(): renderCommandEncoder() failed" };
		}
		encoder->endEncoding();
	}

	void MetalTexture::generateMips(MTL::CommandBuffer* commands)
	{
		auto* encoder = commands->blitCommandEncoder();
		if (not encoder)
		{
			throw InternalEngineError{ "Texture::generateMips(): blitCommandEncoder() failed" };
		}
		encoder->generateMipmaps(m_texture.get());
		encoder->endEncoding();
	}

	MTL::Buffer* MetalTexture::encodeReadRT(MTL::Device* device, MTL::CommandBuffer* commands,
		const uint32 mipLevel, const uint32 bytesPerRow)
	{
		const Size levelSize = ImageProcessing::CalculateMipmapSize(m_desc.size, mipLevel);
		const uint32 width = levelSize.x;
		const uint32 height = levelSize.y;
		const size_t bytes = (static_cast<size_t>(bytesPerRow) * height);
		if (not m_readbackBuffer || (m_readbackBuffer->length() < bytes))
		{
			auto buffer = NS::TransferPtr(device->newBuffer(bytes, MTL::ResourceStorageModeShared));
			if (not buffer)
			{
				return nullptr;
			}
			m_readbackBuffer = std::move(buffer);
		}
		auto* encoder = commands->blitCommandEncoder();
		if (not encoder)
		{
			return nullptr;
		}
		encoder->copyFromTexture(m_texture.get(), 0, mipLevel, MTL::Origin{ 0, 0, 0 }, MTL::Size{ width, height, 1 },
			m_readbackBuffer.get(), 0, bytesPerRow, bytes);
		encoder->endEncoding();
		return m_readbackBuffer.get();
	}

	namespace
	{
		struct MipSource
		{
			Size size;
			size_t rowPitch;
			std::span<const Byte> bytes;
		};

		constexpr size_t UploadSize(const uint32 rowPitch, const uint32 height) noexcept
		{
			return (static_cast<size_t>(rowPitch) * height);
		}

		static_assert(UploadSize(16384 * 16, 16384) == (uint64{ 1 } << 32));

		template <class GetMipSource>
		NS::SharedPtr<MTL::Texture> CreateImmutableTexture(MTL::Device* device, MTL::CommandQueue* queue,
			const MetalTexture2DDesc& desc, const uint32 sourceLevels, GetMipSource getSource)
		{
			const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
			const bool generateMips = (sourceLevels < desc.mipLevels);
			auto descriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
			descriptor->setTextureType(MTL::TextureType2D);
			descriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(desc.format.MTLPixelFormat()));
			descriptor->setWidth(desc.size.x);
			descriptor->setHeight(desc.size.y);
			descriptor->setStorageMode(MTL::StorageModePrivate);
			descriptor->setUsage(generateMips ? (MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget) : MTL::TextureUsageShaderRead);
			descriptor->setMipmapLevelCount(desc.mipLevels);
			auto texture = NS::TransferPtr(device->newTexture(descriptor.get()));
			if (not texture)
			{
				return{};
			}

			auto* commands = queue->commandBuffer();
			if (not commands)
			{
				return{};
			}
			auto* encoder = commands->blitCommandEncoder();
			if (not encoder)
			{
				return{};
			}

			for (uint32 level = 0; level < sourceLevels; ++level)
			{
				const MipSource mip = getSource(level);
				auto upload = NS::TransferPtr(device->newBuffer(mip.bytes.data(), mip.bytes.size_bytes(), MTL::ResourceOptionCPUCacheModeDefault));
				if (not upload)
				{
					encoder->endEncoding();
					return{};
				}
				const MTL::Size extent{ static_cast<NS::UInteger>(mip.size.x), static_cast<NS::UInteger>(mip.size.y), 1 };
				encoder->copyFromBuffer(upload.get(), 0, mip.rowPitch, 0, extent, texture.get(), 0, level, MTL::Origin{ 0, 0, 0 });
			}
			if (generateMips)
			{
				encoder->generateMipmaps(texture.get());
			}
			encoder->endEncoding();
			commands->commit();
			commands->waitUntilCompleted();
			if (commands->status() != MTL::CommandBufferStatusCompleted)
			{
				return{};
			}
			return texture;
		}
	}

	MetalTexture::MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, const std::span<const Image> mipmaps, const TextureDesc desc)
		: m_desc{ desc, TextureType::Default, image.size(), static_cast<uint8>(mipmaps.size() + 1), 1,
			(desc.sRGB ? TextureFormat::R8G8B8A8_Unorm_SRGB : TextureFormat::R8G8B8A8_Unorm), false }
	{
		m_texture = CreateImmutableTexture(device, commandQueue, m_desc, m_desc.mipLevels, [&](const uint32 level)
		{
			const Image& mip = (level == 0) ? image : mipmaps[level - 1];
			return MipSource{ mip.size(), mip.bytesPerRow(), std::as_bytes(std::span{ mip }) };
		});
		m_initialized = static_cast<bool>(m_texture);
	}

	MetalTexture::MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size,
		const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc, TextureType::Default, size,
			(desc.hasMipmap ? ImageProcessing::CalculateMipmapLevel(size.x, size.y) : uint8{ 1 }), 1, format, false }
	{
		const uint32 rowPitch = format.bytesPerRow(size.x);
		const size_t bytes = UploadSize(rowPitch, size.y);
		if (data.size_bytes() < bytes)
		{
			return;
		}
		m_texture = CreateImmutableTexture(device, commandQueue, m_desc, 1, [&](uint32)
		{
			return MipSource{ size, rowPitch, data.first(bytes) };
		});
		m_initialized = static_cast<bool>(m_texture);
	}

	MetalTexture::MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const TextureFormat& format, const std::span<const Blob> mipmaps)
		: m_desc{ ((1 < mipmaps.size()) ? TextureDesc::Mipmap : TextureDesc::NoMipmap),
			TextureType::Default, size, static_cast<uint8>(mipmaps.size()), 1, format, false }
	{
		m_texture = CreateImmutableTexture(device, commandQueue, m_desc, m_desc.mipLevels, [&](const uint32 level)
		{
			const Size mipSize = ImageProcessing::CalculateMipmapSize(size, level);
			return MipSource{ mipSize, format.packedRowBytes(mipSize.x), mipmaps[level] };
		});
		m_initialized = static_cast<bool>(m_texture);
	}

	MetalTexture::MetalTexture(Dynamic, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size,
		const std::span<const Byte> data, const TextureFormat& format, const TextureDesc desc)
		: m_desc{ desc, TextureType::Dynamic, size,
			(desc.hasMipmap ? ImageProcessing::CalculateMipmapLevel(size.x, size.y) : uint8{ 1 }), 1, format, false }
	{
		const auto pool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());
		const bool generateMips = (1 < m_desc.mipLevels);

		auto descriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		descriptor->setTextureType(MTL::TextureType2D);
		descriptor->setPixelFormat(ToEnum<MTL::PixelFormat>(format.MTLPixelFormat()));
		descriptor->setWidth(size.x);
		descriptor->setHeight(size.y);
		descriptor->setStorageMode(MTL::StorageModeShared);
		descriptor->setCpuCacheMode(MTL::CPUCacheModeWriteCombined);
		descriptor->setUsage(generateMips ? (MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget) : MTL::TextureUsageShaderRead);
		descriptor->setMipmapLevelCount(m_desc.mipLevels);

		m_texture = NS::TransferPtr(device->newTexture(descriptor.get()));
		if (not m_texture)
		{
			return;
		}

		const uint32 rowPitch = format.bytesPerRow(size.x);
		const NS::UInteger dataSize = (static_cast<NS::UInteger>(rowPitch) * size.y);
		if (data.empty())
		{
			m_uploadBuffer = NS::TransferPtr(device->newBuffer(dataSize, MTL::ResourceOptionCPUCacheModeWriteCombined));
		}
		else
		{
			m_uploadBuffer = NS::TransferPtr(device->newBuffer(data.data(), dataSize, MTL::ResourceOptionCPUCacheModeWriteCombined));
		}
		if (not m_uploadBuffer)
		{
			return;
		}

		auto* commands = commandQueue->commandBuffer();
		if (not commands)
		{
			return;
		}
		auto* encoder = commands->blitCommandEncoder();
		if (not encoder)
		{
			return;
		}
		const MTL::Size extent{ static_cast<NS::UInteger>(size.x), static_cast<NS::UInteger>(size.y), 1 };
		encoder->copyFromBuffer(m_uploadBuffer.get(), 0, rowPitch, 0, extent, m_texture.get(), 0, 0, MTL::Origin{ 0, 0, 0 });
		if (generateMips)
		{
			encoder->generateMipmaps(m_texture.get());
		}
		encoder->endEncoding();
		commands->commit();
		commands->waitUntilCompleted();
		m_initialized = (commands->status() == MTL::CommandBufferStatusCompleted);
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

	bool MetalTexture::fill(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const ColorF& color, const bool wait)
	{
		if (m_desc.type != TextureType::Dynamic)
		{
			return false;
		}

		if (not prepareUpload(wait))
		{
			return false;
		}

		const uint32 bytesPerRow = m_desc.format.bytesPerRow(m_desc.size.x);
		void* ptr = m_uploadBuffer->contents();
		
		if (not ptr)
		{
			return false;
		}

		FillWithColor(ptr, m_desc.size, bytesPerRow, color, m_desc.format);

		return uploadRegion(commandQueue, frame, bytesPerRow, Rect{ m_desc.size });
	}

	bool MetalTexture::fill(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const std::span<const Byte> data, const uint32 srcBytesPerRow, const bool wait)
	{
		if (m_desc.type != TextureType::Dynamic)
		{
			return false;
		}
		const size_t rowBytes = GetTextureUploadRowBytes(m_desc.size, m_desc.format, data, srcBytesPerRow);
		if ((rowBytes == 0) || not prepareUpload(wait))
		{
			return false;
		}
		const uint32 bytesPerRow = m_desc.format.bytesPerRow(m_desc.size.x);
		void* const ptr = m_uploadBuffer->contents();
		if (not ptr)
		{
			return false;
		}
		FillWithImage(ptr, m_desc.size, bytesPerRow, data, srcBytesPerRow, rowBytes);
		return uploadRegion(commandQueue, frame, bytesPerRow, Rect{ m_desc.size });
	}

	bool MetalTexture::fillRegion(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const ColorF& color, const Rect& rect, const bool wait)
	{
		if ((not isValidRegion(rect))
			|| (not prepareUpload(wait)))
		{
			return false;
		}

		const uint32 bytesPerRow = m_desc.format.bytesPerRow(rect.w);
		void* const ptr = m_uploadBuffer->contents();

		if (not ptr)
		{
			return false;
		}

		FillWithColor(ptr, rect.size, bytesPerRow, color, m_desc.format);

		return uploadRegion(commandQueue, frame, bytesPerRow, rect);
	}

	bool MetalTexture::fillRegion(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const std::span<const Byte> data,
		const uint32 srcBytesPerRow, const Rect& rect, const bool wait)
	{
		if (not isValidRegion(rect))
		{
			return false;
		}
		const size_t rowBytes = GetTextureUploadRowBytes(rect.size, m_desc.format, data, srcBytesPerRow);
		if ((rowBytes == 0) || not prepareUpload(wait))
		{
			return false;
		}
		const uint32 bytesPerRow = m_desc.format.bytesPerRow(rect.w);
		void* const ptr = m_uploadBuffer->contents();
		if (not ptr)
		{
			return false;
		}
		FillWithImage(ptr, rect.size, bytesPerRow, data, srcBytesPerRow, rowBytes);
		return uploadRegion(commandQueue, frame, bytesPerRow, rect);
	}

	bool MetalTexture::prepareUpload(const bool wait)
	{
		if (not m_uploadCommandBuffer)
		{
			return true;
		}

		MTL::CommandBufferStatus status = m_uploadCommandBuffer->status();

		if (status == MTL::CommandBufferStatusCompleted)
		{
			m_uploadCommandBuffer.reset();
			return true;
		}

		if (status == MTL::CommandBufferStatusError)
		{
			m_uploadCommandBuffer.reset();
			return false;
		}

		if (not wait)
		{
			return false;
		}

		m_uploadCommandBuffer->waitUntilCompleted();
		status = m_uploadCommandBuffer->status();
		m_uploadCommandBuffer.reset();

		return (status == MTL::CommandBufferStatusCompleted);
	}

	bool MetalTexture::isValidRegion(const Rect& rect) const noexcept
	{
		return ((m_desc.type == TextureType::Dynamic)
			&& (0 <= rect.x)
			&& (0 <= rect.y)
			&& (0 < rect.w)
			&& (0 < rect.h)
			&& (rect.w <= m_desc.size.x)
			&& (rect.h <= m_desc.size.y)
			&& (rect.x <= (m_desc.size.x - rect.w))
			&& (rect.y <= (m_desc.size.y - rect.h)));
	}

	bool MetalTexture::uploadRegion(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const uint32 bytesPerRow, const Rect& rect)
	{
		const auto autoreleasePool = NS::TransferPtr(NS::AutoreleasePool::alloc()->init());

		// Reserve the continuation before recording a copy, so allocation failure
		// leaves the frame and upload tracking unchanged.
		auto next = NS::RetainPtr(commandQueue->commandBuffer());
		if (not next)
		{
			return false;
		}
		MTL::CommandBuffer* commandBuffer = frame.getCommandBuffer();
		if (not commandBuffer)
		{
			commandBuffer = next.get();
		}

		MTL::BlitCommandEncoder* blitCommandEncoder = commandBuffer->blitCommandEncoder();

		if (not blitCommandEncoder)
		{
			return false;
		}

		const MTL::Size size
		{
			static_cast<NSUInteger>(rect.w),
			static_cast<NSUInteger>(rect.h),
			1
		};

		const MTL::Origin origin
		{
			static_cast<NSUInteger>(rect.x),
			static_cast<NSUInteger>(rect.y),
			0
		};

		blitCommandEncoder->copyFromBuffer(m_uploadBuffer.get(), 0, bytesPerRow, 0, size, m_texture.get(), 0, 0, origin);
		blitCommandEncoder->endEncoding();

		m_uploadCommandBuffer = NS::RetainPtr(commandBuffer);
		if (frame.getCommandBuffer())
		{
			frame.submitAndContinue(std::move(next));
		}
		else
		{
			commandBuffer->commit();
		}

		return true;
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

	MTL::Texture* MetalTexture::getRenderTargetTexture() const noexcept
	{
		return m_multiSampleTexture ? m_multiSampleTexture.get() : m_texture.get();
	}
}
