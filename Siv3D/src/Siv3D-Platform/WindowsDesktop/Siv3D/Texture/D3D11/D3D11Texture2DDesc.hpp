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
# include <Siv3D/TextureDesc.hpp>
# include <Siv3D/TextureFormat.hpp>
# include <Siv3D/PointVector.hpp>
# include <Siv3D/Renderer/D3D11/D3D11.hpp>

namespace s3d
{
	enum class TextureType : uint8
	{
		// 通常テクスチャ
		// [メインテクスチャ]<-[シェーダ・リソース・ビュー]
		Default,

		// 動的テクスチャ
		// [メインテクスチャ]<-[シェーダ・リソース・ビュー], [ステージング・テクスチャ]
		Dynamic,

		// レンダーテクスチャ
		// [メインテクスチャ]<-[レンダー・ターゲット・ビュー]<-[シェーダ・リソース・ビュー]
		Render,

		// マルチサンプル・レンダーテクスチャ
		// [マルチサンプル・テクスチャ]<-[レンダー・ターゲット・ビュー], [メインテクスチャ]<-[シェーダ・リソース・ビュー]
		MSRender,
	};

	struct D3D11Texture2DDesc : TextureDesc
	{
		TextureType type = TextureType::Default;

		Size size{ 0, 0 };

		uint8 mipLevels = 1;

		// デフォルトは 1
		uint8 multiSampleCount = 1;

		uint8 multiSampleQuality = 0;

		TextureFormat format = TextureFormat::Unknown;

		bool hasDepth = false;

		[[nodiscard]]
		bool isMSAA() const noexcept
		{
			return (1 < multiSampleCount);
		}

		[[nodiscard]]
		DXGI_FORMAT DXGIFormat() const noexcept
		{
			return static_cast<DXGI_FORMAT>(format.DXGIFormat());
		}

		[[nodiscard]]
		D3D11_TEXTURE2D_DESC makeD3D11_TEXTURE2D_DESC(uint32 bindFlags, D3D11_USAGE usage, uint32 CPUAccessFlags = 0, const std::pair<uint32, uint32>& multiSample = { 1, 0 }, uint32 miscFlags = 0) const noexcept;

		[[nodiscard]]
		D3D11_SHADER_RESOURCE_VIEW_DESC makeD3D11_SHADER_RESOURCE_VIEW_DESC() const noexcept;

		[[nodiscard]]
		String toString() const;
	};
}
