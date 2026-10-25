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
		/// @param innerColor 中心の乗算色
		/// @param outerColor 外周の乗算色
		/// @return circle への参照
		const Circle& draw(const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return circle への参照
		const Circle& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return circle への参照
		const Circle& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Circle draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		/// @brief 中心位置を指定し、テクスチャに中心から外周へのグラデーションを乗算して描きます。
		/// @param x 中心の X 座標
		/// @param y 中心の Y 座標
		/// @param innerColor 中心の乗算色
		/// @param outerColor 外周の乗算色
		/// @return 描画した円
		Circle draw(double x, double y, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 中心位置を指定し、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param x 中心の X 座標
		/// @param y 中心の Y 座標
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return 描画した円
		Circle draw(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 中心位置を指定し、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param x 中心の X 座標
		/// @param y 中心の Y 座標
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return 描画した円
		Circle draw(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Circle draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		/// @brief 中心位置を指定し、テクスチャに中心から外周へのグラデーションを乗算して描きます。
		/// @param pos 中心座標
		/// @param innerColor 中心の乗算色
		/// @param outerColor 外周の乗算色
		/// @return 描画した円
		Circle draw(const Vec2& pos, const ColorF& innerColor, const ColorF& outerColor) const;

		/// @brief 中心位置を指定し、テクスチャに上下方向のグラデーションを乗算して描きます。
		/// @param pos 中心座標
		/// @param topColor 上端の乗算色
		/// @param bottomColor 下端の乗算色
		/// @return 描画した円
		Circle draw(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		/// @brief 中心位置を指定し、テクスチャに左右方向のグラデーションを乗算して描きます。
		/// @param pos 中心座標
		/// @param leftColor 左端の乗算色
		/// @param rightColor 右端の乗算色
		/// @return 描画した円
		Circle draw(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawPie
		//
		////////////////////////////////////////////////////////////////

		/// @brief テクスチャ付きの扇形を描きます。
		/// @remark UV は円全体への貼り付け位置を維持します。半径には絶対値を使用します。
		/// @param startAngle 開始角度（ラジアン、上方向から時計回り）
		/// @param angle 扇形の角度（ラジアン、負は反時計回り）。[-2π, 2π] に制限され、0 では描画しません。
		/// @param diffuse 乗算色
		/// @return circle への参照
		const Circle& drawPie(double startAngle, double angle, const ColorF& diffuse = Palette::White) const;

		/// @brief テクスチャに中心から外周へのグラデーションを乗算した扇形を描きます。
		/// @remark UV と半径の扱いは単色の drawPie() と同じです。
		/// @param startAngle 開始角度（ラジアン、上方向から時計回り）
		/// @param angle 扇形の角度（ラジアン、負は反時計回り）。[-2π, 2π] に制限され、0 では描画しません。
		/// @param innerColor 中心の乗算色
		/// @param outerColor 外周の乗算色
		/// @return circle への参照
		const Circle& drawPie(double startAngle, double angle, const ColorF& innerColor, const ColorF& outerColor) const;
	};
}
