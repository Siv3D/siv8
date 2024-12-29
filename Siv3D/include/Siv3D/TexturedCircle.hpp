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

		Circle draw(double x, double y, const ColorF& diffuse = Palette::White) const;

		Circle draw(const Vec2& pos, const ColorF& diffuse = Palette::White) const;
	};
}
