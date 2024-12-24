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
# include <Siv3D/TextureDesc.hpp>
# include <Siv3D/TextureFormat.hpp>
# include <Siv3D/PointVector.hpp>

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

	struct MetalTexture2DDesc : TextureDesc
	{
		TextureType type = TextureType::Default;

		Size size{ 0, 0 };

		uint8 mipLevels = 1;

		// デフォルトは 1
		uint8 multiSampleCount = 1;

		TextureFormat format = TextureFormat::Unknown;

		bool hasDepth = false;

		[[nodiscard]]
		bool isMSAA() const noexcept
		{
			return (1 < multiSampleCount);
		}
	};
}
