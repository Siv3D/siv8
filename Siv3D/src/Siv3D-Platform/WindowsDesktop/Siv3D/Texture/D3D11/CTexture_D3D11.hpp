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
# include <Siv3D/Renderer/D3D11/D3D11.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "D3D11Texture.hpp"

namespace s3d
{
	class CRenderer_D3D11;

	class CTexture_D3D11 final : public ISiv3DTexture
	{
	public:

		static constexpr size_t RenderTextureFormatCount = 14;

		CTexture_D3D11();

		~CTexture_D3D11() override;

		void init() override;

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

		CRenderer_D3D11* m_pRenderer = nullptr;

		// device のコピー
		ID3D11Device* m_device = nullptr;

		// context のコピー
		ID3D11DeviceContext* m_context = nullptr;

		// Texture の管理
		AssetHandleManager<Texture::IDType, D3D11Texture> m_textures{ "Texture" };

		std::array<bool, RenderTextureFormatCount> m_multiSampleSupportTable{};
	};
}
