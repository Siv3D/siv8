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
# include <Siv3D/TextEffect/BasicTextEffect.hpp>
# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	DrawableText::DrawableText(const Font& _font, const String& _text, const EnableLigatures enableLigatures)
		: DrawableText{ _font, String{ _text }, ReadingDirection::LeftToRight, enableLigatures } {}

	DrawableText::DrawableText(const Font& _font, String&& _text, const EnableLigatures enableLigatures)
		: DrawableText{ _font, std::move(_text), ReadingDirection::LeftToRight, enableLigatures } {}

	DrawableText::DrawableText(const Font& _font, const String& _text, const ReadingDirection _readingDirection, const EnableLigatures enableLigatures)
		: DrawableText{ _font, String{ _text }, _readingDirection, enableLigatures } {}

	DrawableText::DrawableText(const Font& _font, String&& _text, const ReadingDirection _readingDirection, const EnableLigatures enableLigatures)
		: font{ _font }
		, text{ std::move(_text) }
		, resolvedGlyphs{ font.getResolvedGlyphs(text, _readingDirection, EnableFallback::Yes, enableLigatures) }
		, readingDirection{ _readingDirection } {}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvances
	//
	////////////////////////////////////////////////////////////////

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
	//	region
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::region(const double x, const double y) const
	{
		return region(TextStyle::Default(), font.baseSize(), Vec2{ x, y });
	}

	RectF DrawableText::region(const Vec2 pos) const
	{
		return region(TextStyle::Default(), font.baseSize(), pos);
	}

	RectF DrawableText::region(const double size, const double x, const double y) const
	{
		return region(TextStyle::Default(), size, Vec2{ x, y });
	}

	RectF DrawableText::region(const Arg::topLeft_<Vec2> topLeft) const
	{
		return region(TextStyle::Default(), font.baseSize(), topLeft);
	}

	RectF DrawableText::region(const Arg::topCenter_<Vec2> topCenter) const
	{
		return region(TextStyle::Default(), font.baseSize(), topCenter);
	}

	RectF DrawableText::region(const Arg::topRight_<Vec2> topRight) const
	{
		return region(TextStyle::Default(), font.baseSize(), topRight);
	}

	RectF DrawableText::region(const Arg::rightCenter_<Vec2> rightCenter) const
	{
		return region(TextStyle::Default(), font.baseSize(), rightCenter);
	}

	RectF DrawableText::region(const Arg::bottomRight_<Vec2> bottomRight) const
	{
		return region(TextStyle::Default(), font.baseSize(), bottomRight);
	}

	RectF DrawableText::region(const Arg::bottomCenter_<Vec2> bottomCenter) const
	{
		return region(TextStyle::Default(), font.baseSize(), bottomCenter);
	}

	RectF DrawableText::region(const Arg::bottomLeft_<Vec2> bottomLeft) const
	{
		return region(TextStyle::Default(), font.baseSize(), bottomLeft);
	}

	RectF DrawableText::region(const Arg::leftCenter_<Vec2> leftCenter) const
	{
		return region(TextStyle::Default(), font.baseSize(), leftCenter);
	}

	RectF DrawableText::region(const Arg::center_<Vec2> center) const
	{
		return region(TextStyle::Default(), font.baseSize(), center);
	}

	RectF DrawableText::region(const double size, const Vec2 pos) const
	{
		return region(TextStyle::Default(), size, pos);
	}

	RectF DrawableText::region(const double size, const Arg::topLeft_<Vec2> topLeft) const
	{
		return region(TextStyle::Default(), size, topLeft);
	}

	RectF DrawableText::region(const double size, const Arg::topCenter_<Vec2> topCenter) const
	{
		return region(TextStyle::Default(), size, topCenter);
	}

	RectF DrawableText::region(const double size, const Arg::topRight_<Vec2> topRight) const
	{
		return region(TextStyle::Default(), size, topRight);
	}

	RectF DrawableText::region(const double size, const Arg::rightCenter_<Vec2> rightCenter) const
	{
		return region(TextStyle::Default(), size, rightCenter);
	}

	RectF DrawableText::region(const double size, const Arg::bottomRight_<Vec2> bottomRight) const
	{
		return region(TextStyle::Default(), size, bottomRight);
	}

	RectF DrawableText::region(const double size, const Arg::bottomCenter_<Vec2> bottomCenter) const
	{
		return region(TextStyle::Default(), size, bottomCenter);
	}

	RectF DrawableText::region(const double size, const Arg::bottomLeft_<Vec2> bottomLeft) const
	{
		return region(TextStyle::Default(), size, bottomLeft);
	}

	RectF DrawableText::region(const double size, const Arg::leftCenter_<Vec2> leftCenter) const
	{
		return region(TextStyle::Default(), size, leftCenter);
	}

	RectF DrawableText::region(const double size, const Arg::center_<Vec2> center) const
	{
		return regionAt(TextStyle::Default(), size, *center);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const double x, const double y) const
	{
		return region(textStyle, size, Vec2{ x, y });
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Vec2 pos) const
	{
		return SIV3D_ENGINE(Font)->region(font.id(), text, resolvedGlyphs, pos, size, textStyle, readingDirection);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::topLeft_<Vec2> topLeft) const
	{
		return region(textStyle, size, *topLeft);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::topCenter_<Vec2> topCenter) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy((topCenter->x - (textRegion.w * 0.5)), 0);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::topRight_<Vec2> topRight) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy((topRight->x - textRegion.w), 0);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::rightCenter_<Vec2> rightCenter) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy((rightCenter->x - textRegion.w), (rightCenter->y - (textRegion.h * 0.5)));
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::bottomRight_<Vec2> bottomRight) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy((bottomRight->x - textRegion.w), (bottomRight->y - textRegion.h));
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::bottomCenter_<Vec2> bottomCenter) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy((bottomCenter->x - (textRegion.w * 0.5)), (bottomCenter->y - textRegion.h));
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::bottomLeft_<Vec2> bottomLeft) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy(bottomLeft->x, (bottomLeft->y - textRegion.h));
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::leftCenter_<Vec2> leftCenter) const
	{
		const RectF textRegion = region(textStyle, size);
		return textRegion.movedBy(leftCenter->x, (leftCenter->y - (textRegion.h * 0.5)));
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center) const
	{
		return regionAt(textStyle, size, *center);
	}

	////////////////////////////////////////////////////////////////
	//
	//	regionBase
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::regionBase(const double x, const double y) const
	{
		return regionBase(font.baseSize(), Vec2{ x, y });
	}

	RectF DrawableText::regionBase(const Vec2 pos) const
	{
		return regionBase(TextStyle::Default(), font.baseSize(), pos);
	}

	RectF DrawableText::regionBase(const Arg::left_<Vec2> left) const
	{
		return regionBase(TextStyle::Default(), font.baseSize(), left);
	}

	RectF DrawableText::regionBase(const Arg::center_<Vec2> center) const
	{
		return regionBase(TextStyle::Default(), font.baseSize(), center);
	}

	RectF DrawableText::regionBase(const Arg::right_<Vec2> right) const
	{
		return regionBase(TextStyle::Default(), font.baseSize(), right);
	}

	RectF DrawableText::regionBase(const double size, const double x, const double y) const
	{
		return regionBase(TextStyle::Default(), size, Vec2{ x, y });
	}

	RectF DrawableText::regionBase(const double size, const Vec2 pos) const
	{
		return regionBase(TextStyle::Default(), size, pos);
	}

	RectF DrawableText::regionBase(const double size, const Arg::left_<Vec2> left) const
	{
		return regionBase(TextStyle::Default(), size, *left);
	}

	RectF DrawableText::regionBase(const double size, const Arg::center_<Vec2> center) const
	{
		return regionBase(TextStyle::Default(), size, center);
	}

	RectF DrawableText::regionBase(const double size, const Arg::right_<Vec2> right) const
	{
		return regionBase(TextStyle::Default(), size, right);
	}

	RectF DrawableText::regionBase(const TextStyle& textStyle, const double size, const double x, const double y) const
	{
		return regionBase(textStyle, size, Vec2{ x, y });
	}

	RectF DrawableText::regionBase(const TextStyle& textStyle, const double size, const Vec2 pos) const
	{
		return SIV3D_ENGINE(Font)->regionBase(font.id(), text, resolvedGlyphs, pos, size, textStyle, readingDirection);
	}

	RectF DrawableText::regionBase(const TextStyle& textStyle, const double size, const Arg::left_<Vec2> left) const
	{
		return regionBase(textStyle, size, *left);
	}

	RectF DrawableText::regionBase(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center) const
	{
		return regionBase(textStyle, size, center->movedBy((-0.5 * region(textStyle, size).w), 0));
	}

	RectF DrawableText::regionBase(const TextStyle& textStyle, const double size, const Arg::right_<Vec2> right) const
	{
		return regionBase(textStyle, size, right->movedBy(-region(textStyle, size).w, 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	regionAt
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::regionAt(const double x, const double y) const
	{
		return regionAt(TextStyle::Default(), font.baseSize(), Vec2{ x, y });
	}

	RectF DrawableText::regionAt(const Vec2 pos) const
	{
		return regionAt(TextStyle::Default(), font.baseSize(), pos);
	}

	RectF DrawableText::regionAt(const double size, const double x, const double y) const
	{
		return regionAt(TextStyle::Default(), size, Vec2{ x, y });
	}

	RectF DrawableText::regionAt(const double size, const Vec2 pos) const
	{
		return regionAt(TextStyle::Default(), size, pos);
	}

	RectF DrawableText::regionAt(const TextStyle& textStyle, const double size, const double x, const double y) const
	{
		return regionAt(textStyle, size, Vec2{ x, y });
	}

	RectF DrawableText::regionAt(const TextStyle& textStyle, const double size, const Vec2 pos) const
	{
		const RectF textRegion = region(textStyle, size);

		return textRegion.movedBy(pos - textRegion.center());
	}

	////////////////////////////////////////////////////////////////
	//
	//	regionBaseAt
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::regionBaseAt(const double x, const double y) const
	{
		return regionBaseAt(font.baseSize(), Vec2{ x, y });
	}

	RectF DrawableText::regionBaseAt(const Vec2 pos) const
	{
		return regionBaseAt(TextStyle::Default(), font.baseSize(), pos);
	}

	RectF DrawableText::regionBaseAt(const double size, const double x, const double y) const
	{
		return regionBaseAt(TextStyle::Default(), size, Vec2{ x, y });
	}

	RectF DrawableText::regionBaseAt(const double size, const Vec2 pos) const
	{
		return regionBaseAt(TextStyle::Default(), size, pos);
	}

	RectF DrawableText::regionBaseAt(const TextStyle& textStyle, const double size, const double x, const double y) const
	{
		return regionBaseAt(textStyle, size, Vec2{ x, y });
	}

	RectF DrawableText::regionBaseAt(const TextStyle& textStyle, const double size, const Vec2 pos) const
	{
		const RectF textRegion = regionBase(textStyle, size);
		
		return textRegion.movedBy((pos.x - (textRegion.w * 0.5)), pos.y);
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

	RectF DrawableText::draw(const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), topCenter, color);
	}

	RectF DrawableText::draw(const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), topRight, color);
	}

	RectF DrawableText::draw(const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), rightCenter, color);
	}

	RectF DrawableText::draw(const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), bottomRight, color);
	}

	RectF DrawableText::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), bottomCenter, color);
	}

	RectF DrawableText::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), bottomLeft, color);
	}

	RectF DrawableText::draw(const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), leftCenter, color);
	}

	RectF DrawableText::draw(const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), center, color);
	}

	bool DrawableText::draw(const RectF& area, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), area, color);
	}

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

	RectF DrawableText::draw(const double size, const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, topCenter, color);
	}

	RectF DrawableText::draw(const double size, const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, topRight, color);
	}

	RectF DrawableText::draw(const double size, const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, rightCenter, color);
	}

	RectF DrawableText::draw(const double size, const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, bottomRight, color);
	}

	RectF DrawableText::draw(const double size, const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, bottomCenter, color);
	}

	RectF DrawableText::draw(const double size, const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, bottomLeft, color);
	}

	RectF DrawableText::draw(const double size, const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, leftCenter, color);
	}

	RectF DrawableText::draw(const double size, const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, center, color);
	}

	bool DrawableText::draw(const double size, const RectF& area, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, area, color);
	}

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

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), topCenter, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), topRight, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), rightCenter, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), bottomRight, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), bottomCenter, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), bottomLeft, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), leftCenter, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), center, color);
	}

	bool DrawableText::draw(const TextStyle& textStyle, const RectF& area, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), area, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const double x, const double y, const ColorF& color) const
	{
		return draw(textStyle, size, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Vec2& pos, const ColorF& color) const
	{
		return SIV3D_ENGINE(Font)->draw(font.id(), text, resolvedGlyphs, pos, size, textStyle, TextEffect::BasicTextEffect{ color }, readingDirection);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topLeft_<Vec2> topLeft, const ColorF& color) const
	{
		return draw(textStyle, size, *topLeft, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topCenter_<Vec2> topCenter, const ColorF& color) const
	{
		return draw(textStyle, size, topCenter->movedBy((-0.5 * region(textStyle, size).w), 0), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topRight_<Vec2> topRight, const ColorF& color) const
	{
		return draw(textStyle, size, topRight->movedBy(-region(textStyle, size).w, 0), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::rightCenter_<Vec2> rightCenter, const ColorF& color) const
	{
		const RectF textRegion = region(textStyle, size);

		return draw(textStyle, size, rightCenter->movedBy(-textRegion.w, (-0.5 * textRegion.h)), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomRight_<Vec2> bottomRight, const ColorF& color) const
	{
		return draw(textStyle, size, bottomRight->movedBy(-region(textStyle, size).size), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& color) const
	{
		const RectF textRegion = region(textStyle, size);

		return draw(textStyle, size, bottomCenter->movedBy((-0.5 * textRegion.w), -textRegion.h), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& color) const
	{
		return draw(textStyle, size, bottomLeft->movedBy(0, -region(textStyle, size).h), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::leftCenter_<Vec2> leftCenter, const ColorF& color) const
	{
		return draw(textStyle, size, leftCenter->movedBy(0, (-0.5 * region(textStyle, size).h)), color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return drawAt(textStyle, size, *center, color);
	}

	bool DrawableText::draw(const TextStyle& textStyle, const double size, const RectF& area, const ColorF& color) const
	{
		return SIV3D_ENGINE(Font)->drawRect(font.id(), text, resolvedGlyphs, area, size, textStyle, TextEffect::BasicTextEffect{ color }, readingDirection);
	}

	RectF DrawableText::draw(const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const Vec2& pos, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::draw(const Arg::topLeft_<Vec2> topLeft, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), topLeft, textEffect);
	}

	RectF DrawableText::draw(const Arg::topCenter_<Vec2> topCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), topCenter, textEffect);
	}

	RectF DrawableText::draw(const Arg::topRight_<Vec2> topRight, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), topRight, textEffect);
	}

	RectF DrawableText::draw(const Arg::rightCenter_<Vec2> rightCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), rightCenter, textEffect);
	}

	RectF DrawableText::draw(const Arg::bottomRight_<Vec2> bottomRight, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), bottomRight, textEffect);
	}

	RectF DrawableText::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), bottomCenter, textEffect);
	}

	RectF DrawableText::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), bottomLeft, textEffect);
	}

	RectF DrawableText::draw(const Arg::leftCenter_<Vec2> leftCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), leftCenter, textEffect);
	}

	RectF DrawableText::draw(const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), center, textEffect);
	}

	bool DrawableText::draw(const RectF& area, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), area, textEffect);
	}

	RectF DrawableText::draw(const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, pos, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::topLeft_<Vec2> topLeft, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, topLeft, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::topCenter_<Vec2> topCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, topCenter, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::topRight_<Vec2> topRight, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, topRight, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::rightCenter_<Vec2> rightCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, rightCenter, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::bottomRight_<Vec2> bottomRight, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, bottomRight, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::bottomCenter_<Vec2> bottomCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, bottomCenter, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::bottomLeft_<Vec2> bottomLeft, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, bottomLeft, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::leftCenter_<Vec2> leftCenter, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, leftCenter, textEffect);
	}

	RectF DrawableText::draw(const double size, const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, center, textEffect);
	}

	bool DrawableText::draw(const double size, const RectF& area, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, area, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topLeft_<Vec2> topLeft, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), topLeft, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topCenter_<Vec2> topCenter, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), topCenter, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::topRight_<Vec2> topRight, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), topRight, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::rightCenter_<Vec2> rightCenter, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), rightCenter, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomRight_<Vec2> bottomRight, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), bottomRight, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomCenter_<Vec2> bottomCenter, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), bottomCenter, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::bottomLeft_<Vec2> bottomLeft, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), bottomLeft, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::leftCenter_<Vec2> leftCenter, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), leftCenter, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), center, textEffect);
	}

	bool DrawableText::draw(const TextStyle& textStyle, const RectF& area, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), area, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return SIV3D_ENGINE(Font)->draw(font.id(), text, resolvedGlyphs, pos, size, textStyle, textEffect, readingDirection);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topLeft_<Vec2> topLeft, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, *topLeft, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topCenter_<Vec2> topCenter, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, topCenter->movedBy((-0.5 * region(textStyle, size).w), 0), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::topRight_<Vec2> topRight, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, topRight->movedBy(-region(textStyle, size).w, 0), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::rightCenter_<Vec2> rightCenter, const ITextEffect& textEffect) const
	{
		const RectF textRegion = region(textStyle, size);

		return draw(textStyle, size, rightCenter->movedBy(-textRegion.w, (-0.5 * textRegion.h)), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomRight_<Vec2> bottomRight, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, bottomRight->movedBy(-region(textStyle, size).size), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomCenter_<Vec2> bottomCenter, const ITextEffect& textEffect) const
	{
		const RectF textRegion = region(textStyle, size);

		return draw(textStyle, size, bottomCenter->movedBy((-0.5 * textRegion.w), -textRegion.h), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::bottomLeft_<Vec2> bottomLeft, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, bottomLeft->movedBy(0, -region(textStyle, size).h), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::leftCenter_<Vec2> leftCenter, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, leftCenter->movedBy(0, (-0.5 * region(textStyle, size).h)), textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return drawAt(textStyle, size, *center, textEffect);
	}

	bool DrawableText::draw(const TextStyle& textStyle, const double size, const RectF& area, const ITextEffect& textEffect) const
	{
		return SIV3D_ENGINE(Font)->drawRect(font.id(), text, resolvedGlyphs, area, size, textStyle, textEffect, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawBase
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::drawBase(const double x, const double y, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBase(const Vec2& pos, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), pos, color);
	}

	RectF DrawableText::drawBase(const Arg::left_<Vec2> left, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), left, color);
	}

	RectF DrawableText::drawBase(const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), center, color);
	}

	RectF DrawableText::drawBase(const Arg::right_<Vec2> right, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), right, color);
	}

	RectF DrawableText::drawBase(const double size, const double x, const double y, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), size, Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBase(const double size, const Vec2& pos, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), size, pos, color);
	}

	RectF DrawableText::drawBase(const double size, const Arg::left_<Vec2> left, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), size, left, color);
	}

	RectF DrawableText::drawBase(const double size, const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), size, center, color);
	}

	RectF DrawableText::drawBase(const double size, const Arg::right_<Vec2> right, const ColorF& color) const
	{
		return drawBase(TextStyle::Default(), size, right, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double x, const double y, const ColorF& color) const
	{
		return drawBase(textStyle, font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Vec2& pos, const ColorF& color) const
	{
		return drawBase(textStyle, font.baseSize(), pos, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Arg::left_<Vec2> left, const ColorF& color) const
	{
		return drawBase(textStyle, font.baseSize(), left, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return drawBase(textStyle, font.baseSize(), center, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Arg::right_<Vec2> right, const ColorF& color) const
	{
		return drawBase(textStyle, font.baseSize(), right, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const double x, const double y, const ColorF& color) const
	{
		return drawBase(textStyle, size, Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Vec2& pos, const ColorF& color) const
	{
		return SIV3D_ENGINE(Font)->drawBase(font.id(), text, resolvedGlyphs, pos, size, textStyle, TextEffect::BasicTextEffect{ color }, readingDirection);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Arg::left_<Vec2> left, const ColorF& color) const
	{
		return drawBase(textStyle, size, *left, color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center, const ColorF& color) const
	{
		return drawBase(textStyle, size, center->movedBy((-0.5 * region(textStyle, size).w), 0), color);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Arg::right_<Vec2> right, const ColorF& color) const
	{
		return drawBase(textStyle, size, right->movedBy(-region(textStyle, size).w, 0), color);
	}

	RectF DrawableText::drawBase(const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBase(const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::drawBase(const Arg::left_<Vec2> left, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), left, textEffect);
	}

	RectF DrawableText::drawBase(const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), center, textEffect);
	}

	RectF DrawableText::drawBase(const Arg::right_<Vec2> right, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), font.baseSize(), right, textEffect);
	}

	RectF DrawableText::drawBase(const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBase(const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), size, pos, textEffect);
	}

	RectF DrawableText::drawBase(const double size, const Arg::left_<Vec2> left, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), size, left, textEffect);
	}

	RectF DrawableText::drawBase(const double size, const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), size, center, textEffect);
	}

	RectF DrawableText::drawBase(const double size, const Arg::right_<Vec2> right, const ITextEffect& textEffect) const
	{
		return drawBase(TextStyle::Default(), size, right, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Arg::left_<Vec2> left, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, font.baseSize(), left, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, font.baseSize(), center, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const Arg::right_<Vec2> right, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, font.baseSize(), right, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return SIV3D_ENGINE(Font)->drawBase(font.id(), text, resolvedGlyphs, pos, size, textStyle, textEffect, readingDirection);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Arg::left_<Vec2> left, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, size, *left, textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Arg::center_<Vec2> center, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, size, center->movedBy((-0.5 * region(textStyle, size).w), 0), textEffect);
	}

	RectF DrawableText::drawBase(const TextStyle& textStyle, const double size, const Arg::right_<Vec2> right, const ITextEffect& textEffect) const
	{
		return drawBase(textStyle, size, right->movedBy(-region(textStyle, size).w, 0), textEffect);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawAt
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::drawAt(const double x, const double y, const ColorF& color) const
	{
		return drawAt(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::drawAt(const Vec2& pos, const ColorF& color) const
	{
		return drawAt(TextStyle::Default(), font.baseSize(), pos, color);
	}

	RectF DrawableText::drawAt(const double size, const double x, const double y, const ColorF& color) const
	{
		return drawAt(TextStyle::Default(), size, Vec2{ x, y }, color);
	}

	RectF DrawableText::drawAt(const double size, const Vec2& pos, const ColorF& color) const
	{
		return drawAt(TextStyle::Default(), size, pos, color);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const double x, const double y, const ColorF& color) const
	{
		return drawAt(textStyle, font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const Vec2& pos, const ColorF& color) const
	{
		return drawAt(textStyle, font.baseSize(), pos, color);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const double size, const double x, const double y, const ColorF& color) const
	{
		return drawAt(textStyle, size, Vec2{ x, y }, color);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const double size, const Vec2& pos, const ColorF& color) const
	{
		const RectF textRegion = region(textStyle, size);

		return SIV3D_ENGINE(Font)->draw(font.id(), text, resolvedGlyphs, (pos - textRegion.center()), size, textStyle, TextEffect::BasicTextEffect{ color }, readingDirection);
	}

	RectF DrawableText::drawAt(const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawAt(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawAt(const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawAt(TextStyle::Default(), font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::drawAt(const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawAt(TextStyle::Default(), size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawAt(const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawAt(TextStyle::Default(), size, pos, textEffect);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawAt(textStyle, font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawAt(textStyle, font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawAt(textStyle, size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawAt(const TextStyle& textStyle, const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		const RectF textRegion = region(textStyle, size);

		return SIV3D_ENGINE(Font)->draw(font.id(), text, resolvedGlyphs, (pos - textRegion.center()), size, textStyle, textEffect, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawBaseAt
	//
	////////////////////////////////////////////////////////////////

	RectF DrawableText::drawBaseAt(const double x, const double y, const ColorF& color) const
	{
		return drawBaseAt(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBaseAt(const Vec2& pos, const ColorF& color) const
	{
		return drawBaseAt(TextStyle::Default(), font.baseSize(), pos, color);
	}

	RectF DrawableText::drawBaseAt(const double size, const double x, const double y, const ColorF& color) const
	{
		return drawBaseAt(TextStyle::Default(), size, Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBaseAt(const double size, const Vec2& pos, const ColorF& color) const
	{
		return drawBaseAt(TextStyle::Default(), size, pos, color);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const double x, const double y, const ColorF& color) const
	{
		return drawBaseAt(textStyle, font.baseSize(), Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const Vec2& pos, const ColorF& color) const
	{
		return drawBaseAt(textStyle, font.baseSize(), pos, color);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const double size, const double x, const double y, const ColorF& color) const
	{
		return drawBaseAt(textStyle, size, Vec2{ x, y }, color);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const double size, const Vec2& pos, const ColorF& color) const
	{
		const RectF textRegion = regionBase(textStyle, size);

		return SIV3D_ENGINE(Font)->drawBase(font.id(), text, resolvedGlyphs, Vec2{ (pos.x - (textRegion.w * 0.5)), pos.y }, size, textStyle, TextEffect::BasicTextEffect{ color }, readingDirection);
	}

	RectF DrawableText::drawBaseAt(const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBaseAt(TextStyle::Default(), font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBaseAt(const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawBaseAt(TextStyle::Default(), font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::drawBaseAt(const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBaseAt(TextStyle::Default(), size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBaseAt(const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawBaseAt(TextStyle::Default(), size, pos, textEffect);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBaseAt(textStyle, font.baseSize(), Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return drawBaseAt(textStyle, font.baseSize(), pos, textEffect);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const double size, const double x, const double y, const ITextEffect& textEffect) const
	{
		return drawBaseAt(textStyle, size, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::drawBaseAt(const TextStyle& textStyle, const double size, const Vec2& pos, const ITextEffect& textEffect) const
	{
		const RectF textRegion = regionBase(textStyle, size);

		return SIV3D_ENGINE(Font)->drawBase(font.id(), text, resolvedGlyphs, Vec2{ (pos.x - (textRegion.w * 0.5)), pos.y }, size, textStyle, textEffect, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fits
	//
	////////////////////////////////////////////////////////////////

	bool DrawableText::fits(const RectF& area) const
	{
		return fits(TextStyle::Default(), font.baseSize(), area);
	}

	bool DrawableText::fits(const double size, const RectF& area) const
	{
		return fits(TextStyle::Default(), size, area);
	}

	bool DrawableText::fits(const TextStyle& textStyle, const RectF& area) const
	{
		return fits(textStyle, font.baseSize(), area);
	}

	bool DrawableText::fits(const TextStyle& textStyle, const double size, const RectF& area) const
	{
		return SIV3D_ENGINE(Font)->fitsRect(font.id(), text, resolvedGlyphs, area, size, textStyle, readingDirection);
	}
}
