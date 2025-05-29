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

	Array<double> BitmapGlyphCache::getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const double fontSize)
	{
		if (not prerender(font, resolvedGlyphs, true, ReadingDirection::LeftToRight))
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
				const size_t fallbackIndex = (resolvedGlyph.fontIndex - 1);

				const double xAdvance = SIV3D_ENGINE(Font)->xAdvanceFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, fontSize);
				xAdvances << xAdvance;
				penPosX += xAdvance;

				continue;
			}

			const auto& cache = m_glyphTable.find(resolvedGlyph.glyphIndex)->second;
			const double xAdvance = (cache.info.advance * scale);
			xAdvances << xAdvance;
			penPosX += xAdvance;
		}

		return xAdvances;
	}

	double BitmapGlyphCache::xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, const double fontSize)
	{
		if (not prerender(font, { resolvedGlyph }, false, ReadingDirection::LeftToRight))
		{
			return 0.0;
		}

		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		const auto& cache = m_glyphTable.find(resolvedGlyph.glyphIndex)->second;
		return (cache.info.advance * scale);
	}

	bool BitmapGlyphCache::prerender(FontData& font, const Array<ResolvedGlyph>& resolvedGlyphs, const bool isMainFont, const ReadingDirection readingDirection)
	{
		// Notdef グリフを最初に作成しておく
		if (m_glyphTable.empty())
		{
			const BitmapGlyph bitmapGlyph = font.renderBitmapByGlyphIndex(GlyphIndexNotdef, readingDirection);

			if (not CacheGlyph(font, bitmapGlyph.image, bitmapGlyph, m_buffer, m_glyphTable))
			{
				return false;
			}

			m_hasDirty = true;
		}

		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			if (isMainFont && (resolvedGlyph.fontIndex != 0))
			{
				continue;
			}

			if (m_glyphTable.contains(resolvedGlyph.glyphIndex))
			{
				continue;
			}

			{
				const BitmapGlyph bitmapGlyph = font.renderBitmapByGlyphIndex(resolvedGlyph.glyphIndex, readingDirection);

				if (bitmapGlyph.glyphIndex == GlyphIndexNotdef)
				{
					continue;
				}

				if (not CacheGlyph(font, bitmapGlyph.image, bitmapGlyph, m_buffer, m_glyphTable))
				{
					return false;
				}
			}

			m_hasDirty = true;
		}

		// texture content can be updated in a different thread
		//if (System::GetRendererType() == EngineOption::Renderer::Direct3D11)
		//{
			updateTexture();
		//}

		return true;
	}

	void BitmapGlyphCache::updateTexture()
	{
		if (not m_hasDirty)
		{
			return;
		}

		if (m_texture.size() == m_buffer.image.size())
		{
			m_texture.fill(m_buffer.image);
		}
		else
		{
			m_texture = DynamicTexture{ m_buffer.image };
		}

		m_hasDirty = false;
	}
}
