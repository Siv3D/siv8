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
# include "Common.hpp"
# include "Texture.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DynamicTexture
	//
	////////////////////////////////////////////////////////////////

	/// @brief 中身を更新できる、動的テクスチャクラス
	/// @remark テクスチャの中身を更新する場合、新しい Texture を毎回作成する代わりにこのクラスの `fill()` を使うのが効率的です。
	class DynamicTexture : public Texture
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		DynamicTexture() = default;

		[[nodiscard]]
		explicit DynamicTexture(const Size& size, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		[[nodiscard]]
		DynamicTexture(const Size& size, const ColorF& color, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		//[[nodiscard]]
		//DynamicTexture(const Image& image, TextureDesc desc);

		//[[nodiscard]]
		//explicit DynamicTexture(const Image& image, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		//[[nodiscard]]
		//DynamicTexture(const Grid<uint8>& image, const TextureFormat& format = TextureFormat::R8G8B8A8_Unorm, TextureDesc desc = TextureDesc::NoMipmap);

		////////////////////////////////////////////////////////////////
		//
		//	fill
		//
		////////////////////////////////////////////////////////////////

		bool fill(const ColorF& color);

		/// @brief 動的テクスチャの中身を同じ大きさの画像で更新します。
		/// @remark 動的テクスチャが空の場合は新しい動的テクスチャを画像から作成します。
		/// @param image 画像
		/// @return 動的テクスチャの更新または作成に成功した場合 true, それ以外の場合は false
		bool fill(const Image& image);

		////////////////////////////////////////////////////////////////
		//
		//	fillRegion
		//
		////////////////////////////////////////////////////////////////

		bool fillRegion(const ColorF& color, const Rect& rect);

		bool fillRegion(const Image& image, const Rect& rect);

		////////////////////////////////////////////////////////////////
		//
		//	fillIfNotBusy
		//
		////////////////////////////////////////////////////////////////

		bool fillIfNotBusy(const Image& image);

		////////////////////////////////////////////////////////////////
		//
		//	fillRegionIfNotBusy
		//
		////////////////////////////////////////////////////////////////

		bool fillRegionIfNotBusy(const Image& image, const Rect& rect);

		////////////////////////////////////////////////////////////////
		//
		//	generateMips
		//
		////////////////////////////////////////////////////////////////

		/// @brief ミップマップを生成します。
		/// @remark この関数は、テクスチャの作成時に MipMap を有効にした場合にのみ効果があります。
		void generateMips();

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の DynamicTexture と内容を交換します。
		/// @param other 別の DynamicTexture
		void swap(DynamicTexture& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの DynamicTexture を入れ替えます。
		/// @param lhs 一方の DynamicTexture
		/// @param rhs もう一方の DynamicTexture
		friend void swap(DynamicTexture& lhs, DynamicTexture& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}
