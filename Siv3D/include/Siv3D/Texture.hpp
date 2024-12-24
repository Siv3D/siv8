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
# include "Common.hpp"
# include "Image.hpp"
# include "TextureDesc.hpp"
# include "TextureFormat.hpp"
# include "AssetHandle.hpp"
# include "2DShapesFwd.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Texture
	//
	////////////////////////////////////////////////////////////////

	/// @brief テクスチャ
	class Texture : public AssetHandle<Texture>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Texture();

		[[nodiscard]]
		explicit Texture(const Image& image, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		Texture(const Image& image, const Array<Image>& mipmaps, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		explicit Texture(FilePathView path, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		explicit Texture(IReader&& reader, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		Texture(FilePathView rgb, FilePathView alpha, TextureDesc desc = TextureDesc::Default2D);

		[[nodiscard]]
		Texture(const Color& rgb, FilePathView alpha, TextureDesc desc = TextureDesc::Default2D);

		/// @brief 絵文字からテクスチャを作成します。
		/// @param emoji 絵文字
		/// @remark テクスチャのサイズは `Emoji::ImageSize` です。
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit Texture(const Emoji& emoji, TextureDesc desc = TextureDesc::Default2D);

		/// @brief 絵文字からテクスチャを作成します。
		/// @param emoji 絵文字
		/// @param size テクスチャのサイズ
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit Texture(const Emoji& emoji, int32 size, TextureDesc desc = TextureDesc::Default2D);

		///// @brief アイコンからテクスチャを作成します。
		///// @param icon アイコン
		///// @param size アイコンのサイズ
		///// @param desc テクスチャの設定
		//[[nodiscard]]
		//explicit Texture(const Icon& icon, int32 size, TextureDesc desc = TextureDesc::Default2D);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~Texture();

		////////////////////////////////////////////////////////////////
		//
		//	width, height
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャの幅（ピクセル）を返します。
		/// @return  テクスチャの幅（ピクセル）
		[[nodiscard]]
		int32 width() const;

		/// @brief テクスチャの高さ（ピクセル）を返します。
		/// @return  テクスチャの高さ（ピクセル）
		[[nodiscard]]
		int32 height() const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャの幅と高さ（ピクセル）を返します。
		/// @return  テクスチャの幅と高さ（ピクセル）
		[[nodiscard]]
		Size size() const;

		////////////////////////////////////////////////////////////////
		//
		//	horizontalAspectRatio
		//
		////////////////////////////////////////////////////////////////

		template <Concept::FloatingPoint Float = double>
		[[nodiscard]]
		Float horizontalAspectRatio() const;

		////////////////////////////////////////////////////////////////
		//
		//	getDesc
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャの設定を返します。
		/// @return テクスチャの設定
		[[nodiscard]]
		TextureDesc getDesc() const;

		////////////////////////////////////////////////////////////////
		//
		//	getDesc
		//
		////////////////////////////////////////////////////////////////

		/// @brief ミップマップを持つかを返します。
		/// @return ミップマップを持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasMipmaps() const;

		////////////////////////////////////////////////////////////////
		//
		//	isSRGB
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isSRGB() const;

		////////////////////////////////////////////////////////////////
		//
		//	isSDF
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isSDF() const;

		////////////////////////////////////////////////////////////////
		//
		//	mipLevels
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 mipLevels() const;

		////////////////////////////////////////////////////////////////
		//
		//	getFormat
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャのフォーマットを返します。
		/// @return テクスチャのフォーマット
		[[nodiscard]]
		TextureFormat getFormat() const;

		////////////////////////////////////////////////////////////////
		//
		//	hasDepth
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool hasDepth() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の Texture と内容を交換します。
		/// @param other 別の Texture
		void swap(Texture& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	region
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Rect region(int32 x, int32 y) const;

		[[nodiscard]]
		Rect region(Point pos = Point{ 0, 0 }) const;

		[[nodiscard]]
		RectF region(double x, double y) const;

		[[nodiscard]]
		RectF region(Vec2 pos) const;

		////////////////////////////////////////////////////////////////
		//
		//	regionAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF regionAt(double x, double y) const;

		[[nodiscard]]
		RectF regionAt(Vec2 pos) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		RectF draw(const ColorF& diffuse = Palette::White) const;

		/// @brief 左上位置を指定してテクスチャを描画します。
		/// @param x 描画を開始する左上の X 座標
		/// @param y 描画を開始する左上の Y 座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(double x, double y, const ColorF& diffuse = Palette::White) const;


















		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Texture を入れ替えます。
		/// @param lhs 一方の Texture
		/// @param rhs もう一方の Texture
		friend void swap(Texture& lhs, Texture& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}

# include "detail/Texture.ipp"
