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
	//	TexturedCircle
	//
	////////////////////////////////////////////////////////////////

	struct TexturedCircle
	{
		Circle circle;

		Texture texture;

		FloatRect uvRect;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedCircle() = default;

		[[nodiscard]]
		TexturedCircle(const Texture& _texture, float l, float t, float r, float b, const Circle& _circle);

		[[nodiscard]]
		TexturedCircle(const Texture& _texture, const FloatRect& _uvRect, const Circle& _circle);

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		const Circle& draw(const ColorF& diffuse = Palette::White) const;

		/// @brief テクスチャに中心から外周へのグラデーションを乗算して描きます。
		/// @param innerColor 中心の乗算色（アルファを含む）
		/// @param outerColor 外周の乗算色（アルファを含む）
		/// @return circle への参照
		const Circle& draw(const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param topColor 上端の乗算色（アルファを含む）
		/// @param bottomColor 下端の乗算色（アルファを含む）
		/// @return circle への参照
		const Circle& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param leftColor 左端の乗算色（アルファを含む）
		/// @param rightColor 右端の乗算色（アルファを含む）
		/// @return circle への参照
		const Circle& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Circle draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 中心位置を指定し、テクスチャに中心から外周へのグラデーションを乗算して描きます。
		/// @param x 中心の X 座標
		/// @param y 中心の Y 座標
		/// @param innerColor 中心の乗算色（アルファを含む）
		/// @param outerColor 外周の乗算色（アルファを含む）
		/// @return 描画した円
		Circle draw(double x, double y, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 中心位置を指定し、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param x 中心の X 座標
		/// @param y 中心の Y 座標
		/// @param topColor 上端の乗算色（アルファを含む）
		/// @param bottomColor 下端の乗算色（アルファを含む）
		/// @return 描画した円
		Circle draw(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 中心位置を指定し、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param x 中心の X 座標
		/// @param y 中心の Y 座標
		/// @param leftColor 左端の乗算色（アルファを含む）
		/// @param rightColor 右端の乗算色（アルファを含む）
		/// @return 描画した円
		Circle draw(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Circle draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 中心位置を指定し、テクスチャに中心から外周へのグラデーションを乗算して描きます。
		/// @param pos 中心座標
		/// @param innerColor 中心の乗算色（アルファを含む）
		/// @param outerColor 外周の乗算色（アルファを含む）
		/// @return 描画した円
		Circle draw(const Vec2& pos, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 中心位置を指定し、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param pos 中心座標
		/// @param topColor 上端の乗算色（アルファを含む）
		/// @param bottomColor 下端の乗算色（アルファを含む）
		/// @return 描画した円
		Circle draw(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 中心位置を指定し、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param pos 中心座標
		/// @param leftColor 左端の乗算色（アルファを含む）
		/// @param rightColor 右端の乗算色（アルファを含む）
		/// @return 描画した円
		Circle draw(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;
	};
}
