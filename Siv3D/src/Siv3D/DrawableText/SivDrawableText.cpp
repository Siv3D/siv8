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

# include <Siv3D/DrawableText.hpp>
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/TextLayoutResult.hpp>
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
		, resolvedGlyphs{ font.getResolvedGlyphs(text, _readingDirection, EnableFontFallback::Yes, enableLigatures) }
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

	RectF DrawableText::region(const Anchor anchor, const double x, const double y) const
	{
		return region(anchor, Vec2{ x, y });
	}

	RectF DrawableText::region(const Anchor anchor, const Vec2 pos) const
	{
		return region(TextStyle::Default(), font.baseSize(), anchor, pos);
	}

	RectF DrawableText::region(const double size, const Vec2 pos) const
	{
		return region(TextStyle::Default(), size, pos);
	}

	RectF DrawableText::region(const double size, const Anchor anchor, const double x, const double y) const
	{
		return region(size, anchor, Vec2{ x, y });
	}

	RectF DrawableText::region(const double size, const Anchor anchor, const Vec2 pos) const
	{
		return region(TextStyle::Default(), size, anchor, pos);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const double x, const double y) const
	{
		return region(textStyle, size, Vec2{ x, y });
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Vec2 pos) const
	{
		return SIV3D_ENGINE(Font)->region(font.id(), text, resolvedGlyphs, pos, size, textStyle, readingDirection);
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Anchor anchor, const double x, const double y) const
	{
		return region(textStyle, size, anchor, Vec2{ x, y });
	}

	RectF DrawableText::region(const TextStyle& textStyle, const double size, const Anchor anchor, const Vec2 pos) const
	{
		return{ anchor, pos, region(textStyle, size).size };
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

	RectF DrawableText::draw(const Anchor anchor, const double x, const double y, const ColorF& color) const
	{
		return draw(anchor, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const Anchor anchor, const Vec2& pos, const ColorF& color) const
	{
		return draw(TextStyle::Default(), font.baseSize(), anchor, pos, color);
	}

	TextLayoutResult DrawableText::draw(const RectF& area, const ColorF& color) const
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

	RectF DrawableText::draw(const double size, const Anchor anchor, const double x, const double y, const ColorF& color) const
	{
		return draw(size, anchor, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const double size, const Anchor anchor, const Vec2& pos, const ColorF& color) const
	{
		return draw(TextStyle::Default(), size, anchor, pos, color);
	}

	TextLayoutResult DrawableText::draw(const double size, const RectF& area, const ColorF& color) const
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

	RectF DrawableText::draw(const TextStyle& textStyle, const Anchor anchor, const double x, const double y, const ColorF& color) const
	{
		return draw(textStyle, anchor, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Anchor anchor, const Vec2& pos, const ColorF& color) const
	{
		return draw(textStyle, font.baseSize(), anchor, pos, color);
	}

	TextLayoutResult DrawableText::draw(const TextStyle& textStyle, const RectF& area, const ColorF& color) const
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

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Anchor anchor, const double x, const double y, const ColorF& color) const
	{
		return draw(textStyle, size, anchor, Vec2{ x, y }, color);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Anchor anchor, const Vec2& pos, const ColorF& color) const
	{
		if (anchor == Anchor::TopLeft)
		{
			return draw(textStyle, size, pos, color);
		}

		const RectF textRegion = region(textStyle, size);
		return draw(textStyle, size, RectF{ anchor, pos, textRegion.size }.pos, color);
	}

	TextLayoutResult DrawableText::draw(const TextStyle& textStyle, const double size, const RectF& area, const ColorF& color) const
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

	RectF DrawableText::draw(const Anchor anchor, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(anchor, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), font.baseSize(), anchor, pos, textEffect);
	}

	TextLayoutResult DrawableText::draw(const RectF& area, const ITextEffect& textEffect) const
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

	RectF DrawableText::draw(const double size, const Anchor anchor, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(size, anchor, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const double size, const Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return draw(TextStyle::Default(), size, anchor, pos, textEffect);
	}

	TextLayoutResult DrawableText::draw(const double size, const RectF& area, const ITextEffect& textEffect) const
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

	RectF DrawableText::draw(const TextStyle& textStyle, const Anchor anchor, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(textStyle, anchor, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const
	{
		return draw(textStyle, font.baseSize(), anchor, pos, textEffect);
	}

	TextLayoutResult DrawableText::draw(const TextStyle& textStyle, const RectF& area, const ITextEffect& textEffect) const
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

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Anchor anchor, const double x, const double y, const ITextEffect& textEffect) const
	{
		return draw(textStyle, size, anchor, Vec2{ x, y }, textEffect);
	}

	RectF DrawableText::draw(const TextStyle& textStyle, const double size, const Anchor anchor, const Vec2& pos, const ITextEffect& textEffect) const
	{
		if (anchor == Anchor::TopLeft)
		{
			return draw(textStyle, size, pos, textEffect);
		}

		const RectF textRegion = region(textStyle, size);
		return draw(textStyle, size, RectF{ anchor, pos, textRegion.size }.pos, textEffect);
	}

	TextLayoutResult DrawableText::draw(const TextStyle& textStyle, const double size, const RectF& area, const ITextEffect& textEffect) const
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

	TextLayoutResult DrawableText::fits(const RectF& area) const
	{
		return fits(TextStyle::Default(), font.baseSize(), area);
	}

	TextLayoutResult DrawableText::fits(const double size, const RectF& area) const
	{
		return fits(TextStyle::Default(), size, area);
	}

	TextLayoutResult DrawableText::fits(const TextStyle& textStyle, const RectF& area) const
	{
		return fits(textStyle, font.baseSize(), area);
	}

	TextLayoutResult DrawableText::fits(const TextStyle& textStyle, const double size, const RectF& area) const
	{
		return SIV3D_ENGINE(Font)->fitsRect(font.id(), text, resolvedGlyphs, area, size, textStyle, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderOutlines
	//
	////////////////////////////////////////////////////////////////

	Array<Array<LineString>> DrawableText::renderOutlines(const Vec2& pos, const CloseRing closeRing) const
	{
		return renderOutlines(font.baseSize(), pos, closeRing);
	}

	Array<Array<LineString>> DrawableText::renderOutlines(const double fontSize, const Vec2& pos, const CloseRing closeRing) const
	{
		const auto id = font.id();
		const double baseSize = font.baseSize();
		const double scale = (fontSize / baseSize);
		const double baseLineHeight = (font.height() * scale);
		const Vec2 basePos{ pos };
		Vec2 penPos{ basePos };

		Array<Array<LineString>> outlines;

		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			if (const char32 ch = text[resolvedGlyph.pos];
				ch == U'\n')
			{
				penPos.x = basePos.x;
				penPos.y += baseLineHeight;
				continue;
			}

			const OutlineGlyph outlineGlyph = SIV3D_ENGINE(Font)->generateOutlineGlyphByGlyphIndex(id, resolvedGlyph.glyphIndex, closeRing, readingDirection);

			Array<LineString> glyphOutlines;

			for (const LineString& ring : outlineGlyph.rings)
			{
				const Vec2 posOffset = outlineGlyph.getOffset(scale);
				const Vec2 drawPos = (penPos + posOffset);
				glyphOutlines.push_back(ring.scaledFromOrigin(scale).movedBy(drawPos));
			}

			penPos.x += (outlineGlyph.advance * scale);
			outlines.push_back(std::move(glyphOutlines));
		}

		return outlines;
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderOutlinesAt
	//
	////////////////////////////////////////////////////////////////

	Array<Array<LineString>> DrawableText::renderOutlinesAt(const Vec2& center, const CloseRing closeRing) const
	{
		return renderOutlinesAt(font.baseSize(), center, closeRing);
	}

	Array<Array<LineString>> DrawableText::renderOutlinesAt(const double fontSize, const Vec2& center, const CloseRing closeRing) const
	{
		const RectF textRegion = region(fontSize);
		return renderOutlines(fontSize, (center - textRegion.center()), closeRing);
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderPolygons
	//
	////////////////////////////////////////////////////////////////

	Array<MultiPolygon> DrawableText::renderPolygons(const Vec2& pos) const
	{
		return renderPolygons(font.baseSize(), pos);
	}

	Array<MultiPolygon> DrawableText::renderPolygons(const double fontSize, const Vec2& pos) const
	{
		const auto id = font.id();
		const double baseSize = font.baseSize();
		const double scale = (fontSize / baseSize);
		const double baseLineHeight = (font.height() * scale);
		const Vec2 basePos{ pos };
		Vec2 penPos{ basePos };
		
		Array<MultiPolygon> polygons;
		
		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			if (const char32 ch = text[resolvedGlyph.pos];
				ch == U'\n')
			{
				penPos.x = basePos.x;
				penPos.y += baseLineHeight;
				continue;
			}
			
			const PolygonGlyph polygonGlyph = SIV3D_ENGINE(Font)->generatePolygonGlyphByGlyphIndex(id, resolvedGlyph.glyphIndex, readingDirection);
			const Vec2 posOffset = polygonGlyph.getOffset(scale);
			const Vec2 drawPos = (penPos + posOffset);
			polygons.push_back(polygonGlyph.polygons.scaledFromOrigin(scale).movedBy(drawPos));

			penPos.x += (polygonGlyph.advance * scale);
		}

		return polygons;
	}

	////////////////////////////////////////////////////////////////
	//
	//	renderPolygonsAt
	//
	////////////////////////////////////////////////////////////////

	Array<MultiPolygon> DrawableText::renderPolygonsAt(const Vec2& center) const
	{
		return renderPolygonsAt(font.baseSize(), center);
	}

	Array<MultiPolygon> DrawableText::renderPolygonsAt(const double fontSize, const Vec2& center) const
	{
		const RectF textRegion = region(fontSize);
		return renderPolygons(fontSize, (center - textRegion.center()));
	}
}
