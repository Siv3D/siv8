//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	Glyph GetSingleGlyph(const Font& font, const char32 ch, const ReadingDirection direction, const int32 api)
	{
		switch (api)
		{
		case 0:
			return font.getGlyph(ch, direction);
		case 1:
			return font.getGlyph(StringView{ &ch, 1 }, direction);
		default:
			return font.getGlyphByGlyphIndex(font.getGlyphIndex(ch, direction), direction);
		}
	}

	void CheckSameGlyph(const Glyph& actual, const Glyph& expected)
	{
		CHECK(actual.glyphIndex == expected.glyphIndex);
		CHECK(actual.bufferThickness == expected.bufferThickness);
		CHECK(actual.left == expected.left);
		CHECK(actual.top == expected.top);
		CHECK(actual.width == expected.width);
		CHECK(actual.height == expected.height);
		CHECK(actual.ascender == expected.ascender);
		CHECK(actual.descender == expected.descender);
		CHECK(actual.advance == expected.advance);
		CHECK(actual.texture.texture == expected.texture.texture);
		CHECK(actual.texture.size == expected.texture.size);
		CHECK(actual.texture.uvRect.left == expected.texture.uvRect.left);
		CHECK(actual.texture.uvRect.top == expected.texture.uvRect.top);
		CHECK(actual.texture.uvRect.right == expected.texture.uvRect.right);
		CHECK(actual.texture.uvRect.bottom == expected.texture.uvRect.bottom);
	}

	void CheckSingleGlyph(const FontMethod method, const ReadingDirection direction, const char32 ch, const int32 api)
	{
		INFO("method = " << static_cast<int32>(method) << ", direction = " << static_cast<int32>(direction)
			<< ", character = " << static_cast<uint32>(ch) << ", API = " << api);
		// Each entry point must populate its own fresh cache, without preload or prior drawing.
		const Font font{ method, 24, 4 };
		REQUIRE(font);
		REQUIRE_FALSE(font.getTexture());
		const GlyphIndex index = font.getGlyphIndex(ch, direction);
		CHECK((index == 0) == (ch == U'\U0010FFFF'));
		const Glyph actual = GetSingleGlyph(font, ch, direction, api);
		REQUIRE(actual.glyphIndex == index);
		REQUIRE(actual.texture.texture);
		CHECK(actual.texture.size.x >= 0);
		CHECK(actual.texture.size.y >= 0);
		CHECK(actual.texture.uvRect.left >= 0);
		CHECK(actual.texture.uvRect.top >= 0);
		CHECK(actual.texture.uvRect.left <= actual.texture.uvRect.right);
		CHECK(actual.texture.uvRect.top <= actual.texture.uvRect.bottom);
		CHECK(actual.texture.uvRect.right <= 1);
		CHECK(actual.texture.uvRect.bottom <= 1);
		if (ch == U'A')
		{
			CHECK(actual.texture.size.x > 0);
			CHECK(actual.texture.size.y > 0);
		}
		if (ch == U' ')
		{
			CHECK(actual.advance > 0);
		}

		const Glyph repeated = GetSingleGlyph(font, ch, direction, api);
		CheckSameGlyph(repeated, actual);
		// This fresh Font has no fallback fonts, including for the notdef case.
		const auto glyphs = font.getGlyphs(StringView{ &ch, 1 }, EnableLigatures::No, direction);
		REQUIRE(glyphs.size() == 1);
		CheckSameGlyph(actual, glyphs.front());
	}
}

TEST_CASE("Font.glyph_uncached")
{
	for (const FontMethod method : { FontMethod::Bitmap, FontMethod::MSDF })
	{
		for (const ReadingDirection direction : { ReadingDirection::LeftToRight, ReadingDirection::TopToBottom, ReadingDirection::RightToLeft })
		{
			for (const int32 api : { 0, 1, 2 })
			{
				CheckSingleGlyph(method, direction, U'A', api);
			}
		}
	}
}

TEST_CASE("Font.glyph_space_and_notdef")
{
	for (const FontMethod method : { FontMethod::Bitmap, FontMethod::MSDF })
	{
		for (const char32 ch : { U' ', U'\U0010FFFF' })
		{
			for (const int32 api : { 0, 1, 2 })
			{
				CheckSingleGlyph(method, ReadingDirection::LeftToRight, ch, api);
			}
		}
	}
}
