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
	////////////////////////////////////////////////////////////////
	//
	//	reset
	//
	////////////////////////////////////////////////////////////////

	void MetalInternalTexture2D::reset()
	{
		if (not m_texture)
		{
			return;
		}

		m_texture.reset();
		m_size = Size{ 0, 0 };
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateRenderTexture
	//
	////////////////////////////////////////////////////////////////

	MetalInternalTexture2D MetalInternalTexture2D::CreateRenderTexture(MTL::Device* device, const Size& size)
	{
		LOG_SCOPED_DEBUG("MetalInternalTexture2D::CreateRenderTexture()");
		
		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2D);
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

		MetalInternalTexture2D texture;
		texture.m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		texture.m_size = size;
		texture.m_sampleCount = 1;

		return texture;
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateMSRenderTexture
	//
	////////////////////////////////////////////////////////////////

	MetalInternalTexture2D MetalInternalTexture2D::CreateMSRenderTexture(MTL::Device* device, const Size& size, const uint32 sampleCount)
	{
		LOG_SCOPED_DEBUG("MetalInternalTexture2D::CreateMSRenderTexture()");

		NS::SharedPtr<MTL::TextureDescriptor> textureDescriptor = NS::TransferPtr(MTL::TextureDescriptor::alloc()->init());
		textureDescriptor->setTextureType(MTL::TextureType2DMultisample); // Multisample texture type
		textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
		textureDescriptor->setWidth(size.x);
		textureDescriptor->setHeight(size.y);
		textureDescriptor->setStorageMode(MTL::StorageModePrivate);
		textureDescriptor->setUsage(MTL::TextureUsageRenderTarget);
		textureDescriptor->setSampleCount(sampleCount); // Set the sample count for multisampling

		MetalInternalTexture2D texture;
		texture.m_texture = NS::TransferPtr(device->newTexture(textureDescriptor.get()));
		texture.m_size = size;
		texture.m_sampleCount = sampleCount;
		
		return texture;
	}
}
