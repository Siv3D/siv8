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

# include <Siv3D/TexturedQuad.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	TexturedQuad::TexturedQuad(const Texture& _texture, const float l, const float t, const float r, const float b, const Quad& _quad, const Float2& _center)
		: quad{ _quad }
		, texture{ _texture }
		, uvRect{ l, t, r, b }
		, center{ _center } {}

	TexturedQuad::TexturedQuad(const Texture& _texture, const FloatRect& _uvRect, const Quad& _quad, const Float2& _center)
		: quad{ _quad }
		, texture{ _texture }
		, uvRect{ _uvRect }
		, center{ _center } {}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const Quad& TexturedQuad::draw(const ColorF& diffuse) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture, FloatQuad{ quad }, uvRect, diffuse.toFloat4());
		return quad;
	}

	const Quad& TexturedQuad::draw(const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture, FloatQuad{ quad }, uvRect, colors);
		return quad;
	}

	const Quad& TexturedQuad::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(*topColor, *topColor, *bottomColor, *bottomColor);
	}

	const Quad& TexturedQuad::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(*leftColor, *rightColor, *rightColor, *leftColor);
	}

	Quad TexturedQuad::draw(const double x, const double y, const ColorF& diffuse) const
	{
		const Quad q = quad.movedBy(x, y);
		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture, FloatQuad{ q }, uvRect, diffuse.toFloat4());
		return q;
	}

	Quad TexturedQuad::draw(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Quad q = quad.movedBy(x, y);
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture, FloatQuad{ q }, uvRect, colors);
		return q;
	}

	Quad TexturedQuad::draw(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(x, y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	Quad TexturedQuad::draw(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(x, y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	Quad TexturedQuad::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	Quad TexturedQuad::draw(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(pos.x, pos.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	Quad TexturedQuad::draw(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(pos.x, pos.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	Quad TexturedQuad::draw(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(pos.x, pos.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawAt
	//
	////////////////////////////////////////////////////////////////

	Quad TexturedQuad::drawAt(const double x, const double y, const ColorF& diffuse) const
	{
		const Quad q = quad.movedBy((x - center.x), (y - center.y));
		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture, FloatQuad{ q }, uvRect, diffuse.toFloat4());
		return q;
	}

	Quad TexturedQuad::drawAt(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Quad q = quad.movedBy((x - center.x), (y - center.y));
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };
		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(texture, FloatQuad{ q }, uvRect, colors);
		return q;
	}

	Quad TexturedQuad::drawAt(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawAt(x, y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	Quad TexturedQuad::drawAt(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawAt(x, y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	Quad TexturedQuad::drawAt(const Vec2& pos, const ColorF& diffuse) const
	{
		return drawAt(pos.x, pos.y, diffuse);
	}

	Quad TexturedQuad::drawAt(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return drawAt(pos.x, pos.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	Quad TexturedQuad::drawAt(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawAt(pos.x, pos.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	Quad TexturedQuad::drawAt(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawAt(pos.x, pos.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}
}
