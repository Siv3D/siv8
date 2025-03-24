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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/Renderer/Metal/Metal.hpp>
# include "MetalTexture2DDesc.hpp"

namespace s3d
{
	class MetalTexture
	{
	public:

		struct NoMipmap {};

		struct GenerateMipmap {};

		[[nodiscard]]
		MetalTexture(NoMipmap, MTL::Device* device, const Image& image, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Image& image, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(MTL::Device* device, const Image& image, const Array<Image>& mipmaps, TextureDesc desc);

		[[nodiscard]]
		MetalTexture(NoMipmap, MTL::Device* device, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);
		
		[[nodiscard]]
		MetalTexture(GenerateMipmap, MTL::Device* device, MTL::CommandQueue* commandQueue, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		const MetalTexture2DDesc& getDesc() const noexcept;

		[[nodiscard]]
		bool hasDepth() const noexcept;

		[[nodiscard]]
		MTL::Texture* getTexture() const noexcept;

	private:

		MetalTexture2DDesc m_desc;

		bool m_initialized = false;
		
		NS::SharedPtr<MTL::Texture> m_texture;
	};
}
