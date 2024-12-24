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

# pragma once
# include <Siv3D/Texture.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "MetalTexture.hpp"

namespace s3d
{
	class CTexture_Metal final : public ISiv3DTexture
	{
	public:

		static constexpr size_t RenderTextureFormatCount = 14;

		CTexture_Metal();

		~CTexture_Metal() override;

		void init();

		[[nodiscard]]
		Texture::IDType create(const Image& image, TextureDesc desc) override;

		[[nodiscard]]
		Texture::IDType create(const Image& image, const Array<Image>& mipmaps, TextureDesc desc) override;

		void release(Texture::IDType handleID) override;

		[[nodiscard]]
		Size getSize(Texture::IDType handleID) override;

		[[nodiscard]]
		uint32 getMipLevels(Texture::IDType handleID) override;

		[[nodiscard]]
		TextureDesc getDesc(Texture::IDType handleID) override;

		[[nodiscard]]
		TextureFormat getFormat(Texture::IDType handleID) override;

	private:

		// Texture の管理
		AssetHandleManager<Texture::IDType, MetalTexture> m_textures{ "Texture" };
	};
}
