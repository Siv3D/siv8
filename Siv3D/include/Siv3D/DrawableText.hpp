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
# include "String.hpp"
# include "Font.hpp"
# include "Format.hpp"

namespace s3d
{
	struct TextStyle;

	struct DrawableText
	{
		Font font;

		String text;

		Array<ResolvedGlyph> resolvedGlyphs;

		[[nodiscard]]
		DrawableText() = default;

		[[nodiscard]]
		DrawableText(const Font& _font, const String& _text);

		[[nodiscard]]
		DrawableText(const Font& _font, String&& _text);

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvances
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<double> getXAdvances() const;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvances
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<double> getXAdvances(double fontSize) const;

		////////////////////////////////////////////////////////////////
		//
		//	draw
		//
		////////////////////////////////////////////////////////////////

		RectF draw(double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF draw(Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(double size, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(double size, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, const Vec2& pos = Vec2{ 0, 0 }, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(const TextStyle& textStyle, const RectF& area, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, double x, double y, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, const Vec2& pos, const ColorF& color = Palette::White) const;

		RectF draw(const TextStyle& textStyle, double size, Arg::topLeft_<Vec2> topLeft, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::topCenter_<Vec2> topCenter, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::topRight_<Vec2> topRight, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::rightCenter_<Vec2> rightCenter, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::bottomRight_<Vec2> bottomRight, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::leftCenter_<Vec2> leftCenter, const ColorF& color = Palette::White) const;

		//RectF draw(const TextStyle& textStyle, double size, Arg::center_<Vec2> center, const ColorF& color = Palette::White) const;

		//bool draw(const TextStyle& textStyle, double size, const RectF& area, const ColorF& color = Palette::White) const;

	};
}

# include "detail/DrawableText.ipp"
