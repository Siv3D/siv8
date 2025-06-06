﻿//-----------------------------------------------
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
# include "D3D11Texture2DDesc.hpp"

namespace s3d
{
	struct BCnData;

	class D3D11Texture
	{
	public:

		struct Dynamic {};

		struct NoMipmap {};

		struct GenerateMipmap {};

		[[nodiscard]]
		D3D11Texture(ID3D11Device* device, const Image& image, const Array<Image>& mipmaps, TextureDesc desc);

		[[nodiscard]]
		D3D11Texture(NoMipmap, ID3D11Device* device, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		D3D11Texture(GenerateMipmap, ID3D11Device* device, ID3D11DeviceContext* context, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		D3D11Texture(ID3D11Device* device, const BCnData& bcnData);

		[[nodiscard]]
		D3D11Texture(Dynamic, NoMipmap, ID3D11Device* device, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		D3D11Texture(Dynamic, GenerateMipmap, ID3D11Device* device, ID3D11DeviceContext* context, const Size& size, std::span<const Byte> data, const TextureFormat& format, TextureDesc desc);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		const D3D11Texture2DDesc& getDesc() const noexcept;

		[[nodiscard]]
		bool hasDepth() const noexcept;

		bool fill(ID3D11DeviceContext* context, const ColorF& color, bool wait);

		bool fill(ID3D11DeviceContext* context, std::span<const Byte> data, uint32 srcBytesPerRow, bool wait);

		void generateMipmaps(ID3D11DeviceContext* context);

		[[nodiscard]]
		ID3D11Texture2D* getTexture();

		[[nodiscard]]
		ID3D11ShaderResourceView** getSRVPtr();

	private:

		D3D11Texture2DDesc m_desc;

		bool m_initialized = false;

		// [メインテクスチャ]
		ComPtr<ID3D11Texture2D> m_texture;

		// [マルチサンプル・テクスチャ]
		ComPtr<ID3D11Texture2D> m_multiSampleTexture;

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
