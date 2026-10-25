//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Texture.hpp"
# include "FloatRect.hpp"
# include "2DShapes.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	TexturedRoundRect
	//
	////////////////////////////////////////////////////////////////

	struct TexturedRoundRect
	{
		RoundRect rect;

		Texture texture;

		FloatRect uvRect;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedRoundRect() = default;

		[[nodiscard]]
		TexturedRoundRect(const Texture& _texture, float l, float t, float r, float b, const RoundRect& _rect);

		[[nodiscard]]
		TexturedRoundRect(const Texture& _texture, const FloatRect& _uvRect, const RoundRect& _rect);

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const RoundRect& draw(const ColorF& diffuse = Palette::White) const;

		/// @brief テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return rect への参照
		const RoundRect& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return rect への参照
		const RoundRect& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		RoundRect draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 平行移動して、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return 描画した角丸長方形
		RoundRect draw(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 平行移動して、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param x X 方向の移動量
		/// @param y Y 方向の移動量
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return 描画した角丸長方形
		RoundRect draw(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		RoundRect draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 平行移動して、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param pos 移動量
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return 描画した角丸長方形
		RoundRect draw(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 平行移動して、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param pos 移動量
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return 描画した角丸長方形
		RoundRect draw(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		RoundRect drawAt(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 中心位置を指定し、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param x rect.x を基準とする中心の X 座標
		/// @param y rect.y を基準とする中心の Y 座標
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return 描画した角丸長方形
		RoundRect drawAt(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 中心位置を指定し、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param x rect.x を基準とする中心の X 座標
		/// @param y rect.y を基準とする中心の Y 座標
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return 描画した角丸長方形
		RoundRect drawAt(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		RoundRect drawAt(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 中心位置を指定し、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param pos rect の左上を基準とする中心位置
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return 描画した角丸長方形
		RoundRect drawAt(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 中心位置を指定し、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param pos rect の左上を基準とする中心位置
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return 描画した角丸長方形
		RoundRect drawAt(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;
	};
}
