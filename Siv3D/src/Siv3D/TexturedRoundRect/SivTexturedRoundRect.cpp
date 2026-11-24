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

# include <Siv3D/TexturedRoundRect.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		void DrawGradient(const Texture& texture, const RoundRect& rect, const FloatRect& uvRect,
			const ColorF& color0, const ColorF& color1, const ColorFillDirection colorType)
		{
			SIV3D_ENGINE(Renderer2D)->addTexturedRoundRect(texture,
				FloatRect{ rect.x, rect.y, (rect.x + rect.w), (rect.y + rect.h) },
				static_cast<float>(rect.w),
				static_cast<float>(rect.h),
				Abs(static_cast<float>(rect.r)),
				uvRect, color0.toFloat4(), color1.toFloat4(), colorType);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	TexturedRoundRect::TexturedRoundRect(const Texture& _texture, const float l, const float t, const float r, const float b, const RoundRect& _rect)
		: rect{ _rect }
		, texture{ _texture }
		, uvRect{ l, t, r, b } {}

	TexturedRoundRect::TexturedRoundRect(const Texture& _texture, const FloatRect& _uvRect, const RoundRect& _rect)
		: rect{ _rect }
		, texture{ _texture }
		, uvRect{ _uvRect } {}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	const RoundRect& TexturedRoundRect::draw(const ColorF& diffuse) const
	{
		SIV3D_ENGINE(Renderer2D)->addTexturedRoundRect(texture,
			FloatRect{ rect.x, rect.y, (rect.x + rect.w), (rect.y + rect.h) },
			static_cast<float>(rect.w),
			static_cast<float>(rect.h),
			Abs(static_cast<float>(rect.r)),
			uvRect, diffuse.toFloat4());
		return rect;
	}

	const RoundRect& TexturedRoundRect::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		DrawGradient(texture, rect, uvRect, *topColor, *bottomColor, ColorFillDirection::TopBottom);
		return rect;
	}

	const RoundRect& TexturedRoundRect::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		DrawGradient(texture, rect, uvRect, *leftColor, *rightColor, ColorFillDirection::LeftRight);
		return rect;
	}

	RoundRect TexturedRoundRect::draw(const double x, const double y, const ColorF& diffuse) const
	{
		const RoundRect rr = rect.movedBy(x, y);
		SIV3D_ENGINE(Renderer2D)->addTexturedRoundRect(texture,
			FloatRect{ rr.x, rr.y, (rr.x + rect.w), (rr.y + rect.h) },
			static_cast<float>(rect.w),
			static_cast<float>(rect.h),
			Abs(static_cast<float>(rect.r)),
			uvRect, diffuse.toFloat4());
		return rr;
	}

	RoundRect TexturedRoundRect::draw(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const RoundRect rr = rect.movedBy(x, y);
		DrawGradient(texture, rr, uvRect, *topColor, *bottomColor, ColorFillDirection::TopBottom);
		return rr;
	}

	RoundRect TexturedRoundRect::draw(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const RoundRect rr = rect.movedBy(x, y);
		DrawGradient(texture, rr, uvRect, *leftColor, *rightColor, ColorFillDirection::LeftRight);
		return rr;
	}

	RoundRect TexturedRoundRect::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RoundRect TexturedRoundRect::draw(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(pos.x, pos.y, topColor, bottomColor);
	}

	RoundRect TexturedRoundRect::draw(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(pos.x, pos.y, leftColor, rightColor);
	}

	RoundRect TexturedRoundRect::drawAt(const double x, const double y, const ColorF& diffuse) const
	{
		return draw((x - rect.w * 0.5), (y - rect.h * 0.5), diffuse);
	}

	RoundRect TexturedRoundRect::drawAt(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((x - rect.w * 0.5), (y - rect.h * 0.5), topColor, bottomColor);
	}

	RoundRect TexturedRoundRect::drawAt(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((x - rect.w * 0.5), (y - rect.h * 0.5), leftColor, rightColor);
	}

	RoundRect TexturedRoundRect::drawAt(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw((pos.x - rect.w * 0.5), (pos.y - rect.h * 0.5), diffuse);
	}

	RoundRect TexturedRoundRect::drawAt(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawAt(pos.x, pos.y, topColor, bottomColor);
	}

	RoundRect TexturedRoundRect::drawAt(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawAt(pos.x, pos.y, leftColor, rightColor);
	}
}
