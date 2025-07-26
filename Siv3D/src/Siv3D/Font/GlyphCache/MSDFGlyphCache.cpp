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

# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/Indexed.hpp>
# include <Siv3D/TextStyle.hpp>
# include <Siv3D/ITextEffect.hpp>
# include <Siv3D/GlyphContext.hpp>
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
		return m_glyphCacheManager.getTexture();
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontal
	//
	////////////////////////////////////////////////////////////////

	RectF MSDFGlyphCache::processHorizontal(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		if (not prerender(font, resolvedGlyphs, true, readingDirection))
		{
			return{};
		}

		const bool isDraw = (textOperation == TextOperation::Draw);
		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		const double baseLineHeight = (info.height() * scale);
		const bool pixelPerfect = (fontSize == info.baseSize);

		const Vec2 basePos{ pos };
		Vec2 penPos{ basePos };
		double xMax = basePos.x;
		double advance = 0.0;

		int32 lineCount = 1;
		const int32 totalGlyphCount = static_cast<int32>(resolvedGlyphs.size());

		for (const auto& [index, resolvedGlyph] : Indexed(resolvedGlyphs))
		{
			// タブ, 空白, 制御文字
			if (const char32 ch = s[resolvedGlyph.pos];
				IsControl(ch))
			{
				if (ConsumeControlCharacterHorizontal(s[resolvedGlyph.pos], penPos, lineCount, basePos, scale, textStyle.lineSpacing, info))
				{
					xMax = Max(xMax, penPos.x);
					advance = 0.0;
					continue;
				}
			}

			penPos.x += std::exchange(advance, 0.0);

			// フォールバックフォント
			if (resolvedGlyph.fontIndex != 0)
			{
				const size_t fallbackIndex = (resolvedGlyph.fontIndex - 1);

				Vec2 nextPos = penPos;

				if (not useBasePos)
				{
					nextPos.y += (info.ascender * scale);
				}

				auto [w, adv] = (isDraw ? SIV3D_ENGINE(Font)->drawBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, nextPos, fontSize, textStyle, textEffect, index, totalGlyphCount, readingDirection)
					: SIV3D_ENGINE(Font)->regionBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, nextPos, fontSize, textStyle, readingDirection));

				xMax = Max(xMax, (penPos.x + w));
				advance = adv;
				penPos.x += w;
				continue;
			}

			const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
			double w = 0.0;
			{
				const TextureRegion textureRegion	= m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);		
				const Vec2 posOffset				= (useBasePos ? cache.info.getBase(scale) : cache.info.getOffset(scale));
				const Vec2 drawPos					= (penPos + posOffset);
				const double top = (useBasePos ? (penPos.y - (cache.info.ascender * scale)) : penPos.y);
				const double bottom = (top + baseLineHeight);

				if (pixelPerfect)
				{
					if (isDraw)
					{
						textEffect.draw(textureRegion, GlyphContext{ drawPos, index, totalGlyphCount, top, bottom, readingDirection, isColorGlyph });
					}

					w = textureRegion.region(drawPos).w;
				}
				else
				{
					if (isDraw)
					{
						textEffect.draw(textureRegion.scaled(scale), GlyphContext{ drawPos, index, totalGlyphCount, top, bottom, readingDirection, isColorGlyph });
					}

					w = textureRegion.scaled(scale).region(drawPos).w;
				}

				w += posOffset.x;
			}

			xMax = Max(xMax, (penPos.x + w));
			advance = (Max((cache.info.advance * scale + textStyle.characterSpacing), 0.0) - w);
			penPos.x += w;
		}

		const Vec2 topLeft = (useBasePos ? pos.movedBy(0, -info.ascender * scale) : pos);
		const double width = (xMax - basePos.x);
		const double height = ((info.height() * scale * textStyle.lineSpacing) * lineCount);
		return{ topLeft, width, height };
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontalFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> MSDFGlyphCache::processHorizontalFallback(const TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const int32 index, const int32 totalGlyphCount, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		if (not prerender(font, { resolvedGlyph }, false, readingDirection))
		{
			return{};
		}

		const bool isDraw = (textOperation == TextOperation::Draw);
		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		const bool pixelPerfect = (fontSize == info.baseSize);

		const Vec2 basePos{ pos };
		Vec2 penPos{ basePos };
		double advance = 0.0;
		double w = 0.0;

		{
			const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
			{
				const TextureRegion textureRegion = m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);
				const Vec2 posOffset = (useBasePos ? cache.info.getBase(scale) : cache.info.getOffset(scale));
				const Vec2 drawPos = (penPos + posOffset);
				const double top = (penPos.y - (cache.info.ascender * scale));
				const double bottom = (top + (font.getInfo().height() * scale));

				if (pixelPerfect)
				{
					if (isDraw)
					{
						textEffect.draw(textureRegion, GlyphContext{ drawPos, index, totalGlyphCount, top, bottom, readingDirection, isColorGlyph });
					}

					w = textureRegion.region(drawPos).w;
				}
				else
				{
					if (isDraw)
					{
						textEffect.draw(textureRegion.scaled(scale), GlyphContext{ drawPos, index, totalGlyphCount, top, bottom, readingDirection, isColorGlyph });
					}

					w = textureRegion.scaled(scale).region(drawPos).w;
				}

				w += posOffset.x;
			}

			advance = (Max((cache.info.advance * scale + textStyle.characterSpacing), 0.0) - w);
		}

		return{ w, advance };
	}

	////////////////////////////////////////////////////////////////
	//
	//	processVertical
	//
	////////////////////////////////////////////////////////////////

	RectF MSDFGlyphCache::processVertical(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	processVerticalFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> MSDFGlyphCache::processVerticalFallback(const TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const int32 index, const int32 totalGlyphCount, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontalRect
	//
	////////////////////////////////////////////////////////////////

	bool MSDFGlyphCache::processHorizontalRect(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& area, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getXAdvances
	//
	////////////////////////////////////////////////////////////////

	Array<double> MSDFGlyphCache::getXAdvances(FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const double fontSize)
	{
		constexpr ReadingDirection readingDirection = ReadingDirection::LeftToRight;

		if (not prerender(font, resolvedGlyphs, true, readingDirection))
		{
			return{};
		}

		return m_glyphCacheManager.getXAdvances(font, s, resolvedGlyphs, fontSize, readingDirection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	xAdvanceFallback
	//
	////////////////////////////////////////////////////////////////

	double MSDFGlyphCache::xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, const double fontSize)
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
	//	setBufferThickness
	//
	////////////////////////////////////////////////////////////////

	void MSDFGlyphCache::setBufferThickness(const int32 thickness)
	{
		m_glyphCacheManager.setBufferThickness(thickness);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBufferThickness
	//
	////////////////////////////////////////////////////////////////

	int32 MSDFGlyphCache::getBufferThickness() const noexcept
	{
		return m_glyphCacheManager.getBufferThickness();
	}

	////////////////////////////////////////////////////////////////
	//
	//	prerender
	//
	////////////////////////////////////////////////////////////////

	bool MSDFGlyphCache::prerender(FontData& font, const Array<ResolvedGlyph>& resolvedGlyphs, const bool isMainFont, const ReadingDirection readingDirection)
	{
		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			// このフォントの担当外のグリフはスキップする
			if (isMainFont && (resolvedGlyph.fontIndex != 0))
			{
				continue;
			}

			// 未キャッシュのグリフをキャッシュする
			if (not m_glyphCacheManager.cacheMSDFGlyph(font, resolvedGlyph.glyphIndex, readingDirection))
			{
				m_glyphCacheManager.updateTexture();
				return false;
			}
		}

		m_glyphCacheManager.updateTexture();

		return true;
	}
}
