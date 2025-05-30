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
	//	draw
	//
	////////////////////////////////////////////////////////////////

	RectF MSDFGlyphCache::draw(FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ColorF& color, const double lineHeightScale)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawFallback
	//
	////////////////////////////////////////////////////////////////

	RectF MSDFGlyphCache::drawFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const ColorF& color, const double lineHeightScale)
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
