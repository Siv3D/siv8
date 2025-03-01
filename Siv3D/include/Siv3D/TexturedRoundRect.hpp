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

		RoundRect draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		RoundRect draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;

		RoundRect drawAt(double x, double y, const ColorF& diffuse = Palette::White) const;

		RoundRect drawAt(const Vec2& pos, const ColorF& diffuse = Palette::White) const;
	};
}
