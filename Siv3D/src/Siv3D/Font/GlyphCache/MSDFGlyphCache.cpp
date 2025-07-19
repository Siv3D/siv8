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

# include <Siv3D/Font/IFont.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include "MSDFGlyphCache.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	const Texture& MSDFGlyphCache::getTexture() noexcept
	{
		return(_unused);
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontal
	//
	////////////////////////////////////////////////////////////////

	RectF MSDFGlyphCache::processHorizontal(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontalFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> MSDFGlyphCache::processHorizontalFallback(const TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	processVertical
	//
	////////////////////////////////////////////////////////////////

	RectF MSDFGlyphCache::processVertical(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	processVerticalFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> MSDFGlyphCache::processVerticalFallback(const TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontalRect
	//
	////////////////////////////////////////////////////////////////

	bool MSDFGlyphCache::processHorizontalRect(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& area, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvances
	//
	////////////////////////////////////////////////////////////////

	Array<double> MSDFGlyphCache::getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const double fontSize)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	xAdvanceFallback
	//
	////////////////////////////////////////////////////////////////

	double MSDFGlyphCache::xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, const double fontSize)
	{
		return(0.0);
	}
}
