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
# include "2DShapes.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	TexturedQuad
	//
	////////////////////////////////////////////////////////////////

	struct TexturedQuad
	{
		Quad quad;

		Texture texture;

		FloatRect uvRect;

		Float2 center;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TexturedQuad() = default;

		[[nodiscard]]
		TexturedQuad(const Texture& _texture, float l, float t, float r, float b, const Quad& _quad, const Float2& _center);

		[[nodiscard]]
		TexturedQuad(const Texture& _texture, const FloatRect& _uvRect, const Quad& _quad, const Float2& _center);

		////////////////////////////////////////////////////////////////
		//
		//	(draw)
		//
		////////////////////////////////////////////////////////////////

		const Quad& draw(const ColorF& diffuse = Palette::White) const;

		const Quad& draw(const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		const Quad& draw(Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		const Quad& draw(Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Quad draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		Quad draw(double x, double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		Quad draw(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		Quad draw(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Quad draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		Quad draw(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		Quad draw(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		Quad draw(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		////////////////////////////////////////////////////////////////
		//
		//	drawAt
		//
		////////////////////////////////////////////////////////////////

		Quad drawAt(double x, double y, const ColorF& diffuse = Palette::White) const;

		Quad drawAt(double x, double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		Quad drawAt(double x, double y, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		Quad drawAt(double x, double y, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;

		Quad drawAt(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		Quad drawAt(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const;

		Quad drawAt(const Vec2& pos, Arg::top_<ColorF> topColor, Arg::bottom_<ColorF> bottomColor) const;

		Quad drawAt(const Vec2& pos, Arg::left_<ColorF> leftColor, Arg::right_<ColorF> rightColor) const;
	};
}
