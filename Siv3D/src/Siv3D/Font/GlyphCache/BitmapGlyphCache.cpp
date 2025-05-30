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
# include "BitmapGlyphCache.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	getTexture
	//
	////////////////////////////////////////////////////////////////

	const Texture& BitmapGlyphCache::getTexture() noexcept
	{
		return m_glyphCacheManager.getTexture();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvances
	//
	////////////////////////////////////////////////////////////////

	Array<double> BitmapGlyphCache::getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const double fontSize)
	{
		constexpr ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		if (not prerender(font, resolvedGlyphs, true, readingDirection))
		{
			return{};
		}

		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		constexpr double basePosX = 0.0;
		double penPosX = basePosX;
		Array<double> xAdvances(Arg::reserve = resolvedGlyphs.size());

		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			const char32 ch = s[resolvedGlyph.pos];

			if (IsControl(ch))
			{
				if (ch == U'\t')
				{
					const double tabAdvance = GetTabAdvance(info.spaceXAdvance, scale, basePosX, penPosX, info.tabSize);
					xAdvances << tabAdvance;
					penPosX += tabAdvance;
				}
				else if (ch == U'\n')
				{
					xAdvances << 0.0;
					penPosX = basePosX;
				}
				else
				{
					xAdvances << 0.0;
				}

				continue;
			}

			if (resolvedGlyph.fontIndex != 0)
			{
				const uint32 fallbackIndex = (resolvedGlyph.fontIndex - 1);
				const double xAdvance = SIV3D_ENGINE(Font)->xAdvanceFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, fontSize);
				xAdvances << xAdvance;
				penPosX += xAdvance;

				continue;
			}

			const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
			const double xAdvance = (cache.info.advance * scale);
			xAdvances << xAdvance;
			penPosX += xAdvance;
		}

		return xAdvances;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xAdvanceFallback
	//
	////////////////////////////////////////////////////////////////

	double BitmapGlyphCache::xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, const double fontSize)
	{
		constexpr ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		if (not prerender(font, { resolvedGlyph }, false, readingDirection))
		{
			return 0.0;
		}

		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
		return (cache.info.advance * scale);
	}

	////////////////////////////////////////////////////////////////
	//
	//	prerender
	//
	////////////////////////////////////////////////////////////////

	bool BitmapGlyphCache::prerender(FontData& font, const Array<ResolvedGlyph>& resolvedGlyphs, const bool isMainFont, const ReadingDirection readingDirection)
	{
		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			// このフォントの担当外のグリフはスキップする
			if (isMainFont && (resolvedGlyph.fontIndex != 0))
			{
				continue;
			}

			// 未キャッシュのグリフをキャッシュする
			m_glyphCacheManager.cacheGlyph(font, resolvedGlyph.glyphIndex, readingDirection);
		}

		m_glyphCacheManager.updateTexture();

		return true;
	}
}
