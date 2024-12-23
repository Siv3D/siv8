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
# include <Siv3D/Common.hpp>
# include <Siv3D/Image.hpp>
# include "D3D11TextureDesc.hpp"

namespace s3d
{
	class D3D11Texture
	{
	public:

		[[nodiscard]]
		D3D11Texture(ID3D11Device* device, const Image& image, const Array<Image>& mipmaps, TextureDesc desc);

		[[nodiscard]]
		bool isInitialized() const noexcept;

	private:

		D3D11TextureDesc m_desc;

		bool m_initialized = false;

		// [メインテクスチャ]
		ComPtr<ID3D11Texture2D> m_texture;

		// [マルチサンプル・テクスチャ]
		ComPtr<ID3D11Texture2D> m_multiSampledTexture;

		// [ステージング・テクスチャ]
		ComPtr<ID3D11Texture2D> m_stagingTexture;

		// [デプス・ステンシル・テクスチャ]
		ComPtr<ID3D11Texture2D> m_depthStencilTexture;

		// [レンダー・ターゲット・ビュー]
		ComPtr<ID3D11RenderTargetView> m_renderTargetView;

		// [シェーダ・リソース・ビュー]
		ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

		// [デプス・ステンシル・ビュー]
		ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	};
}
