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
# include "Texture.hpp"
# include "FloatRect.hpp"

namespace s3d
{
	/// @brief 部分テクスチャ（テクスチャ内の長方形領域と、その描画サイズの情報）
	struct TextureRegion
	{
		/// @brief 基本テクスチャ
		Texture texture;

		/// @brief 領域の UV 座標
		FloatRect uvRect{ 0.0f, 0.0f, 0.0f, 0.0f };

		/// @brief 描画サイズ（ピクセル）
		Float2 size{ 0.0f, 0.0f };

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ。空の部分テクスチャを作成します。
		[[nodiscard]]
		TextureRegion() = default;

		[[nodiscard]]
		TextureRegion(const Texture& _texture);

		[[nodiscard]]
		TextureRegion(const Texture& _texture, double width, double height);

		[[nodiscard]]
		TextureRegion(const Texture& _texture, const SizeF& _size);

		[[nodiscard]]
		TextureRegion(const Texture& _texture, float l, float t, float r, float b, double width, double height);

		[[nodiscard]]
		TextureRegion(const Texture& _texture, float l, float t, float r, float b, const SizeF& _size);

		[[nodiscard]]
		TextureRegion(const Texture& _texture, const FloatRect& _uvRect, double width, double height);

		[[nodiscard]]
		TextureRegion(const Texture& _texture, const FloatRect& _uvRect, const SizeF& _size);

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 別の TextureRegion と内容を交換します。
		/// @param other 別の TextureRegion
		void swap(TextureRegion& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	region
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		RectF region(double x, double y) const noexcept;

		[[nodiscard]]
		RectF region(Vec2 pos = Vec2{ 0, 0 }) const noexcept;

		[[nodiscard]]
		RectF regionAt(double x, double y) const noexcept;

		[[nodiscard]]
		RectF regionAt(Vec2 pos) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	stretched
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion stretched(double x, double y) const;

		[[nodiscard]]
		TextureRegion stretched(Vec2 _size) const;

		[[nodiscard]]
		TextureRegion stretched(double top, double right, double bottom, double left) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		/// @brief 位置 (0, 0) に部分テクスチャを描画します。
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(const ColorF& diffuse = Palette::White) const;

		/// @brief 位置 (0, 0) に部分テクスチャを描画します。
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 位置 (0, 0) に部分テクスチャを描画します。
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 位置 (0, 0) に部分テクスチャを描画します。
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param x 部分テクスチャの左上を配置する X 座標
		/// @param y 部分テクスチャの左上を配置する Y 座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param x 部分テクスチャの左上を配置する X 座標
		/// @param y 部分テクスチャの左上を配置する Y 座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(double x, double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param x 部分テクスチャの左上を配置する X 座標
		/// @param y 部分テクスチャの左上を配置する Y 座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param x 部分テクスチャの左上を配置する X 座標
		/// @param y 部分テクスチャの左上を配置する Y 座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの左上を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの左上を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの左上を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの左上を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topLeft 部分テクスチャの左上を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topLeft 部分テクスチャの左上を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topLeft 部分テクスチャの左上を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topLeft 部分テクスチャの左上を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topLeft_<Vec2> topLeft, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topRight 部分テクスチャの右上を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topRight 部分テクスチャの右上を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topRight 部分テクスチャの右上を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topRight 部分テクスチャの右上を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topRight_<Vec2> topRight, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomLeft 部分テクスチャの左下を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomLeft 部分テクスチャの左下を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomLeft 部分テクスチャの左下を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomLeft 部分テクスチャの左下を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomRight 部分テクスチャの右下を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomRight 部分テクスチャの右下を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomRight 部分テクスチャの右下を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomRight 部分テクスチャの右下を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomRight_<Vec2> bottomRight, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topCenter 部分テクスチャの上辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topCenter 部分テクスチャの上辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topCenter 部分テクスチャの上辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param topCenter 部分テクスチャの上辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::topCenter_<Vec2> topCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomCenter 部分テクスチャの下辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomCenter 部分テクスチャの下辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomCenter 部分テクスチャの下辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param bottomCenter 部分テクスチャの下辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param leftCenter 部分テクスチャの左辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param leftCenter 部分テクスチャの左辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param leftCenter 部分テクスチャの左辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param leftCenter 部分テクスチャの左辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::leftCenter_<Vec2> leftCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param rightCenter 部分テクスチャの右辺の中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param rightCenter 部分テクスチャの右辺の中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param rightCenter 部分テクスチャの右辺の中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置に部分テクスチャを描画します。
		/// @param rightCenter 部分テクスチャの右辺の中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::rightCenter_<Vec2>rightCenter, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param center 部分テクスチャの中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param center 部分テクスチャの中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param center 部分テクスチャの中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param center 部分テクスチャの中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF draw(Arg::center_<Vec2> center, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param x 部分テクスチャの中心を配置する X 座標
		/// @param y 部分テクスチャの中心を配置する Y 座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param x 部分テクスチャの中心を配置する X 座標
		/// @param y 部分テクスチャの中心を配置する Y 座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(double x, double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param x 部分テクスチャの中心を配置する X 座標
		/// @param y 部分テクスチャの中心を配置する Y 座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param x 部分テクスチャの中心を配置する X 座標
		/// @param y 部分テクスチャの中心を配置する Y 座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの中心を配置する座標
		/// @param diffuse 描画時に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの中心を配置する座標
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの中心を配置する座標
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
		RectF drawAt(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した位置を中心に部分テクスチャを描画します。
		/// @param pos 部分テクスチャの中心を配置する座標
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 部分テクスチャが描画された領域
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

		/// @brief 指定した四角形にパース変形して部分テクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param diffuse 描画時に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, const ColorF& diffuse = Palette::White) const;

		/// @brief 指定した四角形にパース変形して部分テクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param topLeftColor 描画時に左上の頂点に乗算する色
		/// @param topRightColor 描画時に右上の頂点に乗算する色
		/// @param bottomRightColor 描画時に右下の頂点に乗算する色
		/// @param bottomLeftColor 描画時に左下の頂点に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		/// @brief 指定した四角形にパース変形して部分テクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param topColor 描画時に上側の頂点に乗算する色
		/// @param bottomColor 描画時に下側の頂点に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 指定した四角形にパース変形して部分テクスチャを描画します。
		/// @param quad パース変形の形状
		/// @param leftColor 描画時に左側の頂点に乗算する色
		/// @param rightColor 描画時に右側の頂点に乗算する色
		/// @return 不正な（凸でない）四角形の場合 false, それ以外の場合 true
		bool drawQuadWarp(const Quad& quad, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	mirrored
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion mirrored() const;

		[[nodiscard]]
		TextureRegion mirrored(bool doMirror) const;

		////////////////////////////////////////////////////////////////
		//
		//	flipped
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion flipped() const;

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
		TextureRegion scaled(double sx, double sy) const;

		[[nodiscard]]
		TextureRegion scaled(SizeF s) const;

		////////////////////////////////////////////////////////////////
		//
		//	resized
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureRegion resized(double _size) const;

		[[nodiscard]]
		TextureRegion resized(double width, double height) const;

		[[nodiscard]]
		TextureRegion resized(SizeF _size) const;

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

		//[[nodiscard]]
		//TexturedRoundRect rounded(double r) const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの TextureRegion を入れ替えます。
		/// @param lhs 一方の TextureRegion
		/// @param rhs もう一方の TextureRegion
		friend void swap(TextureRegion& lhs, TextureRegion& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}
