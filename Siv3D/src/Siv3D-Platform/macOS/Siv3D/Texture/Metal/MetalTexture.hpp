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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>
# include "MetalTexture2DDesc.hpp"

namespace s3d
{
	class Blob;

	class MetalFrameContext;

	class MetalTexture
	{
	public:
		
		struct Dynamic {};

		struct Render {};

		[[nodiscard]]
		MetalTexture(Render, MTL::Device* device, MTL::CommandBuffer* commands, const Size& size,
			std::span<const Byte> data, const TextureFormat& format, TextureDesc desc, const ColorF& color, uint32 sampleCount = 1);

		void clearRT(MTL::CommandBuffer* commands, const ColorF& color);

		void resolveMSRT(MTL::CommandBuffer* commands);

		[[nodiscard]]
		MTL::Buffer* encodeReadRT(MTL::Device* device, MTL::CommandBuffer* commands, uint32 mipLevel, uint32 bytesPerRow);

		struct NoMipmap {};

		struct GenerateMipmap {};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		MetalTexture(NoMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, std::span<const Image> mipmaps, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(NoMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, const TextureFormat& format, std::span<const Blob> mipmaps);

		[[nodiscard]]
		MetalTexture(Dynamic, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		////////////////////////////////////////////////////////////////
		//
		//	isInitialized
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isInitialized() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getDesc
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const MetalTexture2DDesc& getDesc() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasDepth
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool hasDepth() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		bool fill(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const ColorF& color, bool wait);

		bool fill(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, std::span<const Byte> data, uint32 srcBytesPerRow, bool wait);

		bool fillRegion(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, const ColorF& color, const Rect& rect, bool wait);

		bool fillRegion(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, std::span<const Byte> data, uint32 srcBytesPerRow, const Rect& rect, bool wait);

		////////////////////////////////////////////////////////////////
		//
		//	generateMips
		//
		////////////////////////////////////////////////////////////////

		void generateMips(MTL::CommandBuffer* commands);

		////////////////////////////////////////////////////////////////
		//
		//	getTexture
		//
		////////////////////////////////////////////////////////////////
		
		[[nodiscard]]
		MTL::Texture* getTexture() const noexcept;

		[[nodiscard]]
		MTL::Texture* getRenderTargetTexture() const noexcept;

	private:

		NS::SharedPtr<MTL::Buffer> m_readbackBuffer;

		[[nodiscard]]
		bool prepareUpload(bool wait);

		[[nodiscard]]
		bool isValidRegion(const Rect& rect) const noexcept;

		bool uploadRegion(MTL::CommandQueue* commandQueue, MetalFrameContext& frame, uint32 bytesPerRow, const Rect& rect);

		MetalTexture2DDesc m_desc;

		bool m_initialized = false;

		NS::SharedPtr<MTL::Texture> m_texture;

		NS::SharedPtr<MTL::Texture> m_multiSampleTexture;
		
		NS::SharedPtr<MTL::Buffer> m_uploadBuffer;

		NS::SharedPtr<MTL::CommandBuffer> m_uploadCommandBuffer;
	};
}
