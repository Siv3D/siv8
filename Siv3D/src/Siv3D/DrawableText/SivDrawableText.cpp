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

# include <Siv3D/DrawableText.hpp>
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	DrawableText::DrawableText(const Font& _font, const String& _text, const ReadingDirection _readingDirection)
		: DrawableText{ _font, String{ _text }, _readingDirection } {}

	DrawableText::DrawableText(const Font& _font, String&& _text, const ReadingDirection _readingDirection)
		: font{ _font }
		, text{ std::move(_text) }
		, resolvedGlyphs{ font.getResolvedGlyphs(text, _readingDirection) }
		, readingDirection{ _readingDirection } {}
	
	Array<double> DrawableText::getXAdvances() const
	{
		return getXAdvances(font.baseSize());
	}

	Array<double> DrawableText::getXAdvances(const double fontSize) const
	{
		return SIV3D_ENGINE(Font)->getXAdvances(font.id(), text, resolvedGlyphs, fontSize);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::draw(const double x, const double y, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const Vec2& pos, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), pos, color);
	}

	RectF DrawableText::draw(const Arg::topLeft_<Vec2> topLeft, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), topLeft, color);
	}

	//RectF DrawableText::draw(const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), topCenter, color);
	//}

	//RectF DrawableText::draw(const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), topRight, color);
	//}

	//RectF DrawableText::draw(const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), rightCenter, color);
	//}

	//RectF DrawableText::draw(const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), bottomRight, color);
	//}

	//RectF DrawableText::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), bottomCenter, color);
	//}

	//RectF DrawableText::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), bottomLeft, color);
	//}

	//RectF DrawableText::draw(const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), leftCenter, color);
	//}

	//RectF DrawableText::draw(const Arg::center_<Vec2> center, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), center, color);
	//}

	//bool DrawableText::draw(const RectF& area, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), font.baseSize(), area, color);
	//}

	RectF DrawableText::draw(const double size, const double x, const double y, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const double size, const Vec2& pos, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, pos, color);
	}

	RectF DrawableText::draw(const double size, const Arg::topLeft_<Vec2> topLeft, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, topLeft, color);
	}

	//RectF DrawableText::draw(const double size, const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, topCenter, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, topRight, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, rightCenter, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, bottomRight, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, bottomCenter, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, bottomLeft, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, leftCenter, color);
	//}

	//RectF DrawableText::draw(const double size, const Arg::center_<Vec2> center, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, center, color);
	//}

	//bool DrawableText::draw(const double size, const RectF& area, const ColorF& color) const
	//{
	//	return draw(TextStyle::Default(), size, area, color);
	//}

	RectF DrawableText::draw(const TextStyle& textStyle, const double x, const double y, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Vec2& pos, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), pos, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topLeft_<Vec2> topLeft, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), topLeft, color);
	}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), topCenter, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), topRight, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), rightCenter, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), bottomRight, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), bottomCenter, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), bottomLeft, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), leftCenter, color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const Arg::center_<Vec2> center, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), center, color);
	//}

	//bool DrawableText::draw(const TextStyle& textStyle, const RectF& area, const ColorF& color) const
	//{
	//	return draw(textStyle, font.baseSize(), area, color);
	//}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const double x, const double y, const ColorF& color) const
	{
		return draw(textStyle, size, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Vec2& pos, const ColorF& color) const
	{
		return SIV3D_ENGINE(Font)->draw(font.id(), text, resolvedGlyphs, pos, size, textStyle, color, readingDirection);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topLeft_<Vec2> topLeft, const ColorF& color) const
	{
		return draw(textStyle, size, *topLeft, color);
	}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	//{
	//	return draw(textStyle, size, topCenter->movedBy(-region(size).w * 0.5, 0), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	//{
	//	return draw(textStyle, size, topRight->movedBy(-region(size).w, 0), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	//{
	//	const RectF textRegion = region(size);

	//	return draw(textStyle, size, rightCenter->movedBy(-textRegion.w, -textRegion.h * 0.5), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	//{
	//	return draw(textStyle, size, bottomRight->movedBy(-region(size).size), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	//{
	//	const RectF textRegion = region(size);

	//	return draw(textStyle, size, bottomCenter->movedBy(-textRegion.w * 0.5, -textRegion.h), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	//{
	//	return draw(textStyle, size, bottomLeft->movedBy(0, -region(size).h), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	//{
	//	return draw(textStyle, size, leftCenter->movedBy(0, -region(size).h * 0.5), color);
	//}

	//RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center, const ColorF& color) const
	//{
	//	return drawAt(textStyle, size, *center, color);
	//}

	//bool DrawableText::draw(const TextStyle& textStyle, const double size, const RectF& area, const ColorF& color) const
	//{
	//	return SIV3D_ENGINE(Font)->draw(font.id(), text, clusters, area, size, textStyle, color, 1.0);
	//}

}
