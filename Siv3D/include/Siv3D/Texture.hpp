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

# if SIV3D_PLATFORM(WINDOWS)

	struct ID3D11Texture2D;

# endif

namespace s3d
{
	struct TextureRegion;
	struct TexturedQuad;
	struct TexturedRoundRect;

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

		/// @brief デフォルトコンストラクタ。空のテクスチャを作成します。
		[[nodiscard]]
		Texture();

		/// @brief 画像からテクスチャを作成します。
		/// @param image 画像
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit Texture(const Image& image, TextureDesc desc = TextureDesc::Default2D);

		/// @brief 画像とミップマップからテクスチャを作成します。
		/// @param image 基本画像
		/// @param mipmaps ミップマップ
		/// @param desc テクスチャの設定
		[[nodiscard]]
		Texture(const Image& image, const Array<Image>& mipmaps, TextureDesc desc = TextureDesc::Default2D);

		/// @brief 画像ファイルを読み込んでテクスチャを作成します。
		/// @param path 画像ファイルのパス
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit Texture(FilePathView path, TextureDesc desc = TextureDesc::Default2D);

		/// @brief Reader から画像ファイルを読み込んでテクスチャを作成します。
		/// @param reader Reader オブジェクト
		/// @param desc テクスチャの設定
		[[nodiscard]]
		explicit Texture(IReader&& reader, TextureDesc desc = TextureDesc::Default2D);

		/// @brief 一方の画像ファイルから RGB チャンネルを、もう一方の画像ファイルからアルファチャンネルを読み込んでテクスチャを作成します。
		/// @param rgb RGB チャンネルの画像ファイルのパス
		/// @param alpha アルファチャンネルの画像ファイルのパス（R 成分がアルファチャンネルとして使用されます）
		/// @param desc テクスチャの設定
		[[nodiscard]]
		Texture(FilePathView rgb, FilePathView alpha, TextureDesc desc = TextureDesc::Default2D);

		/// @brief 指定した色を RGB チャンネルに、指定した画像ファイルの内容をアルファチャンネルに使用してテクスチャを作成します。
		/// @param rgb RGB チャンネルの色
		/// @param alpha アルファチャンネルの画像ファイルのパス（R 成分がアルファチャンネルとして使用されます）
		/// @param desc テクスチャの設定
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

		/// @brief デストラクタ
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

		/// @brief テクスチャのアスペクト比（幅 / 高さ）を返します。
		/// @tparam Float アスペクト比の型
		/// @return テクスチャのアスペクト比（幅 / 高さ）
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

		/// @brief テクスチャがミップマップを持つかを返します。
		/// @return テクスチャがミップマップを持つ場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasMipmaps() const;

		////////////////////////////////////////////////////////////////
		//
		//	isSRGB
		//
		////////////////////////////////////////////////////////////////

		/// @brief シェーダでのサンプリング時に sRGB → Linear への変換が行われるかを返します。
		/// @return シェーダでのサンプリング時に sRGB → Linear への変換が行われる場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isSRGB() const;

		////////////////////////////////////////////////////////////////
		//
		//	isSDF
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャが SDF (Signed Distance Field) であるかを返します。
		/// @return テクスチャが SDF (Signed Distance Field) である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isSDF() const;

		////////////////////////////////////////////////////////////////
		//
		//	mipLevels
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャのミップマップレベル数を返します。
		/// @return テクスチャのミップマップレベル数。ミップマップを持たない場合は 1
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

		/// @brief テクスチャが深度バッファを持つかを返します。
		/// @return テクスチャが深度バッファを持つ場合 true, それ以外の場合は false
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

		/// @brief 指定した位置にテクスチャを描画する際の領域を返します。
		/// @param x テクスチャの左上を配置する X 座標
		/// @param y テクスチャの左上を配置する Y 座標
		/// @return テクスチャが描画される領域
		[[nodiscard]]
		Rect region(int32 x, int32 y) const;

		/// @brief 指定した位置にテクスチャを描画する際の領域を返します。
		/// @param pos テクスチャの左上を配置する座標
		/// @return テクスチャが描画される領域
		[[nodiscard]]
		Rect region(Point pos = Point{ 0, 0 }) const;

		/// @brief 指定した位置にテクスチャを描画する際の領域を返します。
		/// @param x テクスチャの左上を配置する X 座標
		/// @param y テクスチャの左上を配置する Y 座標
		/// @return テクスチャが描画される領域
		[[nodiscard]]
		RectF region(double x, double y) const;

		/// @brief 指定した位置にテクスチャを描画する際の領域を返します。
		/// @param pos テクスチャの左上を配置する座標
		/// @return テクスチャが描画される領域
		[[nodiscard]]
		RectF region(Vec2 pos) const;

		////////////////////////////////////////////////////////////////
		//
		//	regionAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心にテクスチャを描画する際の領域を返します。
		/// @param x テクスチャの中心を配置する X 座標
		/// @param y テクスチャの中心を配置する Y 座標
		/// @return テクスチャが描画される領域
		[[nodiscard]]
		RectF regionAt(double x, double y) const;

		/// @brief 指定した位置を中心にテクスチャを描画する際の領域を返します。
		/// @param pos テクスチャの中心を配置する座標
		/// @return テクスチャが描画される領域
		[[nodiscard]]
		RectF regionAt(Vec2 pos) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置 (0, 0) にテクスチャを描画します。
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(const ColorF& diffuse = Palette::White) const;

		/// @brief 位置 (0, 0) にテクスチャを描画します。
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 位置 (0, 0) にテクスチャを描画します。
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 位置 (0, 0) にテクスチャを描画します。
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param x テクスチャの左上を配置する X 座標
		/// @param y テクスチャの左上を配置する Y 座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param x テクスチャの左上を配置する X 座標
		/// @param y テクスチャの左上を配置する Y 座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(double x, double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param x テクスチャの左上を配置する X 座標
		/// @param y テクスチャの左上を配置する Y 座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param x テクスチャの左上を配置する X 座標
		/// @param y テクスチャの左上を配置する Y 座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param pos テクスチャの左上を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param pos テクスチャの左上を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param pos テクスチャの左上を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param pos テクスチャの左上を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topLeft テクスチャの左上を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topLeft テクスチャの左上を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topLeft テクスチャの左上を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topLeft テクスチャの左上を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topRight テクスチャの右上を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topRight テクスチャの右上を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topRight テクスチャの右上を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topRight テクスチャの右上を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomLeft テクスチャの左下を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomLeft テクスチャの左下を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomLeft テクスチャの左下を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomLeft テクスチャの左下を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomRight テクスチャの右下を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomRight テクスチャの右下を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomRight テクスチャの右下を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomRight テクスチャの右下を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topCenter テクスチャの上辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topCenter テクスチャの上辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topCenter テクスチャの上辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param topCenter テクスチャの上辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomCenter テクスチャの下辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomCenter テクスチャの下辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomCenter テクスチャの下辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param bottomCenter テクスチャの下辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param leftCenter テクスチャの左辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param leftCenter テクスチャの左辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param leftCenter テクスチャの左辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param leftCenter テクスチャの左辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param rightCenter テクスチャの右辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param rightCenter テクスチャの右辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param rightCenter テクスチャの右辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置にテクスチャを描画します。
		/// @param rightCenter テクスチャの右辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param center テクスチャの中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param center テクスチャの中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param center テクスチャの中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param center テクスチャの中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param x テクスチャの中心を配置する X 座標
		/// @param y テクスチャの中心を配置する Y 座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param x テクスチャの中心を配置する X 座標
		/// @param y テクスチャの中心を配置する Y 座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(double x, double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param x テクスチャの中心を配置する X 座標
		/// @param y テクスチャの中心を配置する Y 座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param x テクスチャの中心を配置する X 座標
		/// @param y テクスチャの中心を配置する Y 座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param pos テクスチャの中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param pos テクスチャの中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param pos テクスチャの中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置を中心にテクスチャを描画します。
		/// @param pos テクスチャの中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawClipped
		//
		////////////////////////////////////////////////////////////////

		//RectF drawClipped(double x, double y, const RectF& clipRect, const ColorF& diffuse = Palette::White) const;

		//RectF drawClipped(const Vec2& pos, const RectF& clipRect, const ColorF& diffuse = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAtClipped
		//
		////////////////////////////////////////////////////////////////

		//RectF drawAtClipped(double x, double y, const RectF& clipRect, const ColorF& diffuse = Palette::White) const;

		//RectF drawAtClipped(const Vec2& pos, const RectF& clipRect, const ColorF& diffuse = Palette::White) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawQuadWarp
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した四角形にパース変形してテクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param diffuse 描画時に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した四角形にパース変形してテクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した四角形にパース変形してテクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した四角形にパース変形してテクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ()
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分テクスチャを作成します。
		/// @param x 範囲の左上の X 座標
		/// @param y 範囲の左上の Y 座標
		/// @param size 範囲の幅と高さ（ピクセル）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(double x, double y, double size) const;

		/// @brief 部分テクスチャを作成します。
		/// @param x 範囲の左上の X 座標
		/// @param y 範囲の左上の Y 座標
		/// @param w 範囲の幅（ピクセル）
		/// @param h 範囲の高さ（ピクセル）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(double x, double y, double w, double h) const;

		/// @brief 部分テクスチャを作成します。
		/// @param xy 範囲の左上の座標
		/// @param w 範囲の幅（ピクセル）
		/// @param h 範囲の高さ（ピクセル）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(const Vec2& xy, double w, double h) const;

		/// @brief 部分テクスチャを作成します。
		/// @param x 範囲の左上の X 座標
		/// @param y 範囲の左上の Y 座標
		/// @param size 範囲の幅と高さ（ピクセル）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(double x, double y, const SizeF& size) const;

		/// @brief 部分テクスチャを作成します。
		/// @param xy 範囲の左上の座標
		/// @param size 範囲の幅と高さ（ピクセル）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(const Vec2& xy, double size) const;

		/// @brief 部分テクスチャを作成します。
		/// @param xy 範囲の左上の座標
		/// @param size 範囲の幅と高さ（ピクセル）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(const Vec2& xy, const SizeF& size) const;

		/// @brief 部分テクスチャを作成します。
		/// @param rect 範囲
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion operator ()(const RectF& rect) const;

		////////////////////////////////////////////////////////////////
		//
		//	uv
		//
		////////////////////////////////////////////////////////////////

		/// @brief 部分テクスチャを作成します。
		/// @param u 範囲の左上の U 座標
		/// @param v 範囲の左上の V 座標
		/// @param w 範囲の幅（UV 座標）
		/// @param h 範囲の高さ（UV 座標）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion uv(double u, double v, double w, double h) const;

		/// @brief 部分テクスチャを作成します。
		/// @param u 範囲の左上の U 座標
		/// @param v 範囲の左上の V 座標
		/// @param uvSize 範囲の幅と高さ（UV 座標）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion uv(double u, double v, double uvSize) const;

		/// @brief 部分テクスチャを作成します。
		/// @param rect 範囲（UV 座標）
		/// @return 部分テクスチャ
		[[nodiscard]]
		TextureRegion uv(const RectF& rect) const;

		////////////////////////////////////////////////////////////////
		//
		//	mirrored
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャを左右反転した部分テクスチャを作成します。
		/// @return 左右反転した部分テクスチャ
		[[nodiscard]]
		TextureRegion mirrored() const;

		/// @brief テクスチャを左右反転した部分テクスチャを作成します。
		/// @param doMirror 左右反転する場合 true, それ以外の場合は false
		/// @return 左右反転またはそのままの部分テクスチャ
		[[nodiscard]]
		TextureRegion mirrored(bool doMirror) const;

		////////////////////////////////////////////////////////////////
		//
		//	flipped
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャを上下反転した部分テクスチャを作成します。
		/// @return 上下反転した部分テクスチャ
		[[nodiscard]]
		TextureRegion flipped() const;

		/// @brief テクスチャを上下反転した部分テクスチャを作成します。
		/// @param doFlip 上下反転する場合 true, それ以外の場合は false
		/// @return 上下反転またはそのままの部分テクスチャ
		[[nodiscard]]
		TextureRegion flipped(bool doFlip) const;

		////////////////////////////////////////////////////////////////
		//
		//	scaled
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion scaled(double s) const;

		[[nodiscard]]
		TextureRegion scaled(double xs, double ys) const;

		[[nodiscard]]
		TextureRegion scaled(SizeF s) const;

		////////////////////////////////////////////////////////////////
		//
		//	resized
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion resized(double size) const;

		[[nodiscard]]
		TextureRegion resized(double width, double height) const;

		[[nodiscard]]
		TextureRegion resized(SizeF size) const;

		////////////////////////////////////////////////////////////////
		//
		//	repeated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion repeated(double repeat) const;

		[[nodiscard]]
		TextureRegion repeated(double xRepeat, double yRepeat) const;

		[[nodiscard]]
		TextureRegion repeated(SizeF repeat) const;

		////////////////////////////////////////////////////////////////
		//
		//	mapped
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion mapped(double size) const;

		[[nodiscard]]
		TextureRegion mapped(double width, double height) const;

		[[nodiscard]]
		TextureRegion mapped(SizeF size) const;

		////////////////////////////////////////////////////////////////
		//
		//	fitted
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion fitted(double size, AllowUpscale allowUpscale = AllowUpscale::Yes) const;

		[[nodiscard]]
		TextureRegion fitted(double width, double height, AllowUpscale allowUpscale = AllowUpscale::Yes) const;

		[[nodiscard]]
		TextureRegion fitted(const SizeF& size, AllowUpscale allowUpscale = AllowUpscale::Yes) const;

		////////////////////////////////////////////////////////////////
		//
		//	rotated
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedQuad rotated(double angle) const;

		////////////////////////////////////////////////////////////////
		//
		//	rotatedAt
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedQuad rotatedAt(double x, double y, double angle) const;

		[[nodiscard]]
		TexturedQuad rotatedAt(const Vec2& pos, double angle) const;

		////////////////////////////////////////////////////////////////
		//
		//	rounded
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedRoundRect rounded(double r) const;

		[[nodiscard]]
		TexturedRoundRect rounded(double x, double y, double w, double h, double r) const;

		////////////////////////////////////////////////////////////////
		//
		//	getD3D11Texture2D
		//
		////////////////////////////////////////////////////////////////

	# if SIV3D_PLATFORM(WINDOWS)

		[[nodiscard]]
		ID3D11Texture2D* getD3D11Texture2D();

	# endif

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
