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

# include "MetalInternalTexture2D.hpp"
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	void MetalInternalTexture2D::reset()
	{
		if (not m_texture)
		{
			return;
		}

		m_texture->release();
		m_size = Size{ 0, 0 };
	}

	MetalInternalTexture2D MetalInternalTexture2D::CreateRenderTexture(MTL::Device* device, const Size& size)
	{
		LOG_SCOPED_DEBUG("MetalInternalTexture2D::CreateRenderTexture()");
		
		MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);  // GPU-private storage
		textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

		MetalInternalTexture2D texture;
		texture.m_texture = device->newTexture(textureDescriptor);
		texture.m_size = size;
		textureDescriptor->release();

		return texture;
	}

	MetalInternalTexture2D MetalInternalTexture2D::CreateMSRenderTexture(const Size& size, const uint32 sampleCount)
	{
		LOG_SCOPED_DEBUG("MetalInternalTexture2D::CreateMSRenderTexture()");
		return{};
	}
}
