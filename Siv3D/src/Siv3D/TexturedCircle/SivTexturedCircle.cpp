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

# include <Siv3D/TexturedCircle.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	TexturedCircle::TexturedCircle(const Texture& _texture, const float l, const float t, const float r, const float b, const Circle& _circle)
		: circle{ _circle }
		, texture{ _texture }
		, uvRect{ l, t, r, b } {}

	TexturedCircle::TexturedCircle(const Texture& _texture, const FloatRect& _uvRect, const Circle& _circle)
		: circle{ _circle }
		, texture{ _texture }
		, uvRect{ _uvRect } {}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Circle& TexturedCircle::draw(const ColorF& diffuse) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, circle, uvRect, diffuse.toFloat4());
		return circle;
	}

	const Circle& TexturedCircle::draw(const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, circle, uvRect, innerColor.toFloat4(), outerColor.toFloat4(), ColorFillDirection::InOut);
		return circle;
	}

	const Circle& TexturedCircle::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, circle, uvRect, topColor->toFloat4(), bottomColor->toFloat4(), ColorFillDirection::TopBottom);
		return circle;
	}

	const Circle& TexturedCircle::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, circle, uvRect, leftColor->toFloat4(), rightColor->toFloat4(), ColorFillDirection::LeftRight);
		return circle;
	}

	Circle TexturedCircle::draw(const double x, const double y, const ColorF& diffuse) const
	{
		const Circle c{ x, y, circle.r };
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, c, uvRect, diffuse.toFloat4());
		return c;
	}

	Circle TexturedCircle::draw(const double x, const double y, const ColorF& innerColor, const ColorF& outerColor) const
	{
		const Circle c{ x, y, circle.r };
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, c, uvRect, innerColor.toFloat4(), outerColor.toFloat4(), ColorFillDirection::InOut);
		return c;
	}

	Circle TexturedCircle::draw(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const Circle c{ x, y, circle.r };
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, c, uvRect, topColor->toFloat4(), bottomColor->toFloat4(), ColorFillDirection::TopBottom);
		return c;
	}

	Circle TexturedCircle::draw(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const Circle c{ x, y, circle.r };
		SIV3D_ENGINE(Renderer2D)->addTexturedCircle(texture, c, uvRect, leftColor->toFloat4(), rightColor->toFloat4(), ColorFillDirection::LeftRight);
		return c;
	}

	Circle TexturedCircle::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	Circle TexturedCircle::draw(const Vec2& pos, const ColorF& innerColor, const ColorF& outerColor) const
	{
		return draw(pos.x, pos.y, innerColor, outerColor);
	}

	Circle TexturedCircle::draw(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(pos.x, pos.y, topColor, bottomColor);
	}

	Circle TexturedCircle::draw(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(pos.x, pos.y, leftColor, rightColor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawPie
	//
	////////////////////////////////////////////////////////////////

	const Circle& TexturedCircle::drawPie(const double startAngle, const double angle, const ColorF& diffuse) const
	{
		const Float4 color = diffuse.toFloat4();
		SIV3D_ENGINE(Renderer2D)->addTexturedCirclePie(texture, circle.center, Abs(static_cast<float>(circle.r)),
			static_cast<float>(startAngle), Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF), uvRect, color, color);
		return circle;
	}

	const Circle& TexturedCircle::drawPie(const double startAngle, const double angle, const ColorF& innerColor, const ColorF& outerColor) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedCirclePie(texture, circle.center, Abs(static_cast<float>(circle.r)),
			static_cast<float>(startAngle), Clamp(static_cast<float>(angle), -Math::TwoPiF, Math::TwoPiF), uvRect, innerColor.toFloat4(), outerColor.toFloat4());
		return circle;
	}
}
