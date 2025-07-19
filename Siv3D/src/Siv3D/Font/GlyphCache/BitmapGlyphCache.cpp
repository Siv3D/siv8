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
# include <Siv3D/Math.hpp>
# include <Siv3D/TextStyle.hpp>
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
	//	processHorizontal
	//
	////////////////////////////////////////////////////////////////

	RectF BitmapGlyphCache::processHorizontal(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
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

		for (const auto& resolvedGlyph : resolvedGlyphs)
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

				auto [w, adv] = (isDraw ? SIV3D_ENGINE(Font)->drawBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, nextPos, fontSize, textStyle, drawFunc, readingDirection)
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
					const Vec2 drawPosRounded = Math::Round(drawPos);

					if (isDraw)
					{
						drawFunc(textureRegion, drawPosRounded, top, bottom, isColorGlyph);
					}

					w = textureRegion.region(drawPosRounded).w;
				}
				else
				{
					if (isDraw)
					{
						drawFunc(textureRegion.scaled(scale), drawPos, top, bottom, isColorGlyph);
					}

					w = textureRegion.scaled(scale).region(drawPos).w;
				}

				w += (posOffset.x * scale);
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

	std::pair<double, double> BitmapGlyphCache::processHorizontalFallback(const TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
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
					const Vec2 drawPosRounded = Math::Round(drawPos);

					if (isDraw)
					{
						drawFunc(textureRegion, drawPosRounded, top, bottom, isColorGlyph);
					}

					w = textureRegion.region(drawPosRounded).w;
				}
				else
				{
					if (isDraw)
					{
						drawFunc(textureRegion.scaled(scale), drawPos, top, bottom, isColorGlyph);
					}

					w = textureRegion.scaled(scale).region(drawPos).w;
				}

				w += (posOffset.x * scale);
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

	RectF BitmapGlyphCache::processVertical(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		if (not prerender(font, resolvedGlyphs, true, readingDirection))
		{
			return{};
		}

		const bool isDraw = (textOperation == TextOperation::Draw);
		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		const bool pixelPerfect = (fontSize == info.baseSize);

		const Vec2 basePos{ pos };
		Vec2 penPos{ basePos };
		double yMax = basePos.y;
		double advance = 0.0;

		int32 lineCount = 1;

		for (const auto& resolvedGlyph : resolvedGlyphs)
		{
			// タブ, 空白, 制御文字
			if (const char32 ch = s[resolvedGlyph.pos];
				IsControl(ch))
			{
				if (ConsumeControlCharacterVertical(s[resolvedGlyph.pos], penPos, lineCount, basePos, scale, textStyle.lineSpacing, info))
				{
					yMax = Max(yMax, penPos.y);
					advance = 0.0;
					continue;
				}
			}

			penPos.y += std::exchange(advance, 0.0);

			// フォールバックフォント
			if (resolvedGlyph.fontIndex != 0)
			{
				const size_t fallbackIndex = (resolvedGlyph.fontIndex - 1);

				Vec2 nextPos = penPos;
				auto [h, adv] = (isDraw? SIV3D_ENGINE(Font)->drawBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, nextPos, fontSize, textStyle, drawFunc, readingDirection)
					: SIV3D_ENGINE(Font)->regionBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, nextPos, fontSize, textStyle, readingDirection));

				yMax = Max(yMax, (penPos.y + h));
				advance = adv;
				penPos.y += h;
				continue;
			}

			const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
			double h = 0.0;
			{
				const TextureRegion textureRegion	= m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);		
				const Vec2 posOffset{ cache.info.left, cache.info.top };
				const Vec2 drawPos = (penPos + posOffset);
				const double top = penPos.y;
				const double bottom = (top + ((cache.info.top + textureRegion.size.y) * scale));

				if (pixelPerfect)
				{
					const Vec2 drawPosRounded = Math::Round(drawPos);
					
					if (isDraw)
					{
						drawFunc(textureRegion, drawPosRounded, top, bottom, isColorGlyph);
					}

					h = textureRegion.region(drawPosRounded).h;
				}
				else
				{
					if (isDraw)
					{
						drawFunc(textureRegion.scaled(scale), drawPos, top, bottom, isColorGlyph);
					}

					h = textureRegion.scaled(scale).region(drawPos).h;
				}

				h += (posOffset.y * scale);
			}

			yMax = Max(yMax, (penPos.y + h));
			advance = (Max((cache.info.advance * scale + textStyle.characterSpacing), 0.0) - h);
			penPos.y += h;
		}

		const double right = (basePos.x + (info.height() * scale * 0.5));
		const double left = (right - (info.height() * scale * textStyle.lineSpacing * lineCount));
		const double top = pos.y;
		const double bottom = yMax;
		return{ left, top, (right - left), (bottom - top) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	processVerticalFallback
	//
	////////////////////////////////////////////////////////////////

	std::pair<double, double> BitmapGlyphCache::processVerticalFallback(const TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, const bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
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
		double h = 0.0;

		{
			const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
			{
				const TextureRegion textureRegion = m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);
				const Vec2 posOffset{ cache.info.left, cache.info.top };
				const Vec2 drawPos = (penPos + posOffset * scale);
				const double top = penPos.y;
				const double bottom = (top + ((cache.info.top + textureRegion.size.y) * scale));

				if (pixelPerfect)
				{
					const Vec2 drawPosRounded = Math::Round(drawPos);
					
					if (isDraw)
					{
						drawFunc(textureRegion, drawPosRounded, top, bottom, isColorGlyph);
					}
					
					h = textureRegion.region(drawPosRounded).h;
				}
				else
				{
					if (isDraw)
					{
						drawFunc(textureRegion.scaled(scale), drawPos, top, bottom, isColorGlyph);
					}

					h = textureRegion.scaled(scale).region(drawPos).h;
				}

				h += (posOffset.y * scale);
			}

			advance = (Max((cache.info.advance * scale + textStyle.characterSpacing), 0.0) - h);
		}

		return{ h, advance };
	}

	////////////////////////////////////////////////////////////////
	//
	//	processHorizontalRect
	//
	////////////////////////////////////////////////////////////////

	bool BitmapGlyphCache::processHorizontalRect(const TextOperation textOperation, FontData& font, const StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& area, const double fontSize, const TextStyle& textStyle, const Font::DrawFunc drawFunc, const bool isColorGlyph, const ReadingDirection readingDirection)
	{
		// 「...」用のグリフ
		const ResolvedGlyph periodGlyph = font.getResolvedGlyphs(U".", readingDirection, EnableFallback::No, EnableLigatures::No).fetch(0, ResolvedGlyph{});
		{
			Array<ResolvedGlyph> resolvedGlyphsWithPeriod(Arg::reserve = resolvedGlyphs.size() + 1);
			resolvedGlyphsWithPeriod.append(resolvedGlyphs);
			resolvedGlyphsWithPeriod.push_back(periodGlyph);

			if (not prerender(font, resolvedGlyphsWithPeriod, true, readingDirection))
			{
				return{};
			}
		}

		const auto& info = font.getInfo();
		const double scale = (fontSize / info.baseSize);
		const bool pixelPerfect = (fontSize == info.baseSize);

		const Vec2 areaBottomRight = area.br();
		const double periodXAdvance = (m_glyphCacheManager.get(periodGlyph.glyphIndex, readingDirection).info.advance);

		const double lineHeight = (info.height() * scale * textStyle.lineSpacing);
		const int32 maxLines = Max(static_cast<int32>(area.h / (lineHeight ? lineHeight : 1)), 1);
		const Vec2 basePos{ area.pos };
		
		// 描画がオーバーフローするかどうか
		bool isOverflow = false;
		
		// 各グリフの描画位置を先に求める
		Array<PenPosInfo> penPositions;
		Array<PenPosInfo> periodPositions;
		{
			Vec2 penPos{ basePos };
			double advance = 0.0;

			int32 lineCount = 1;

			for (const auto& resolvedGlyph : resolvedGlyphs)
			{
				// タブ, 空白, 制御文字
				if (const char32 ch = s[resolvedGlyph.pos];
					IsControl(ch))
				{
					if (ch == U'\t')
					{
						const double tabAdvance = GetTabAdvance(info.spaceXAdvance, scale, basePos.x, penPos.x, info.tabSize);

						if (areaBottomRight.x < (penPos.x + tabAdvance))
						{
							penPos.x = basePos.x;
							penPos.y += lineHeight;
							const double w = GetTabAdvance(info.spaceXAdvance, scale, basePos.x, penPos.x, info.tabSize);
							
							if ((maxLines < ++lineCount) || (areaBottomRight.x < (penPos.x + w)))
							{
								isOverflow = true;
								break;
							}

							penPositions.emplace_back(penPos, penPos);
							penPos.x += w;
						}
						else
						{
							penPositions.emplace_back(penPos, penPos);
							penPos.x += tabAdvance;
						}
					}
					else if (ch == U'\n')
					{
						penPositions.emplace_back(penPos, penPos);
						penPos.x = basePos.x;
						penPos.y += lineHeight;

						if (maxLines < ++lineCount)
						{
							isOverflow = true;
							break;
						}
					}
					else
					{
						penPositions.emplace_back(penPos, penPos);
					}

					advance = 0.0;
					continue;
				}

				penPos.x += std::exchange(advance, 0.0);

				// フォールバックフォント
				if (resolvedGlyph.fontIndex != 0)
				{
					const size_t fallbackIndex = (resolvedGlyph.fontIndex - 1);
					Vec2 nextPos = (penPos + Vec2{ 0, (info.ascender * scale) });

					auto [w, adv] = SIV3D_ENGINE(Font)->regionBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, nextPos, fontSize, textStyle, readingDirection);

					if (areaBottomRight.x < (penPos.x + w))
					{
						penPos.x = basePos.x;
						penPos.y += lineHeight;
						nextPos = (penPos + Vec2{ 0, (info.ascender * scale) });
						advance = 0.0;

						if ((maxLines < ++lineCount) || (areaBottomRight.x < (penPos.x + w)))
						{
							isOverflow = true;
							break;
						}
					}

					penPositions.emplace_back(penPos, nextPos);

					advance = adv;
					penPos.x += w;
					continue;
				}

				const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
				double w = 0.0;
				{
					const TextureRegion textureRegion = m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);
					const Vec2 posOffset = cache.info.getOffset(scale);
					Vec2 drawPos = (penPos + posOffset);

					if (pixelPerfect)
					{
						drawPos = Math::Round(drawPos);
						w = textureRegion.region(drawPos).w;
						w += (posOffset.x * scale);

						if (areaBottomRight.x < (penPos.x + w))
						{
							penPos.x = basePos.x;
							penPos.y += lineHeight;
							advance = 0.0;

							if ((maxLines < ++lineCount) || (areaBottomRight.x < (penPos.x + w)))
							{
								isOverflow = true;
								break;
							}

							drawPos = (penPos + posOffset);
							drawPos = Math::Round(drawPos);
							w = textureRegion.region(drawPos).w;
							w += (posOffset.x * scale);
						}
					}
					else
					{
						w = textureRegion.scaled(scale).region(drawPos).w;
						w += (posOffset.x * scale);

						if (areaBottomRight.x < (penPos.x + w))
						{
							penPos.x = basePos.x;
							penPos.y += lineHeight;
							advance = 0.0;

							if ((maxLines < ++lineCount) || (areaBottomRight.x < (penPos.x + w)))
							{
								isOverflow = true;
								break;
							}
							
							drawPos = (penPos + posOffset);
							w = textureRegion.scaled(scale).region(drawPos).w;
							w += (posOffset.x * scale);
						}
					}

					penPositions.emplace_back(penPos, drawPos);
				}

				advance = (Max((cache.info.advance * scale + textStyle.characterSpacing), 0.0) - w);
				penPos.x += w;
			}

			if (textOperation == TextOperation::Region)
			{
				return (not isOverflow);
			}

			// ... グリフの追加
			if (isOverflow)
			{
				penPos = basePos;

				while (penPositions)
				{
					penPos = penPositions.back().penPos;

					penPositions.pop_back();

					if (not penPositions)
					{
						break;
					}

					if ((penPositions.back().penPos.x + periodXAdvance * 3) <= areaBottomRight.x)
					{
						break;
					}
				}

				const auto& cache = m_glyphCacheManager.get(periodGlyph.glyphIndex, readingDirection);
				const Vec2 posOffset = cache.info.getOffset(scale);
				const TextureRegion textureRegion = m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);

				for (int32 i = 0; i < 3; ++i)
				{
					double w = 0.0;
					{
						Vec2 drawPos = (penPos + posOffset);

						if (pixelPerfect)
						{
							drawPos = Math::Round(drawPos);
							w = textureRegion.region(drawPos).w;
							w += (posOffset.x * scale);
						}
						else
						{
							w = textureRegion.scaled(scale).region(drawPos).w;
							w += (posOffset.x * scale);
						}

						periodPositions.emplace_back(penPos, drawPos);
					}

					advance = (Max((cache.info.advance * scale + textStyle.characterSpacing), 0.0) - w);
					penPos.x += w;
				}
			}
		}

		{
			const double baseLineHeight = (info.height() * scale);

			for (size_t i = 0; i < penPositions.size(); ++i)
			{
				const ResolvedGlyph& resolvedGlyph = resolvedGlyphs[i];
				const PenPosInfo& penPosInfo = penPositions[i];

				if (const char32 ch = s[resolvedGlyph.pos];
					IsControl(ch))
				{
					continue;
				}

				if (resolvedGlyph.fontIndex != 0)
				{
					const size_t fallbackIndex = (resolvedGlyph.fontIndex - 1);
					SIV3D_ENGINE(Font)->drawBaseFallback(font.getFallbackFontID(fallbackIndex), resolvedGlyph, penPosInfo.drawPos, fontSize, textStyle, drawFunc, readingDirection);
				}
				else
				{
					const auto& cache = m_glyphCacheManager.get(resolvedGlyph.glyphIndex, readingDirection);
					const TextureRegion textureRegion = m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);
					const double top = penPosInfo.penPos.y;
					const double bottom = (top + baseLineHeight);

					if (pixelPerfect)
					{
						drawFunc(textureRegion, penPosInfo.drawPos, top, bottom, isColorGlyph);
					}
					else
					{
						drawFunc(textureRegion.scaled(scale), penPosInfo.drawPos, top, bottom, isColorGlyph);
					}
				}
			}

			for (const auto& periodPosition : periodPositions)
			{
				const auto& cache = m_glyphCacheManager.get(periodGlyph.glyphIndex, readingDirection);
				const TextureRegion textureRegion = m_glyphCacheManager.getTexture()(cache.textureRegionLeft, cache.textureRegionTop, cache.textureRegionWidth, cache.textureRegionHeight);
				const Vec2 drawPos = periodPosition.drawPos;
				const double top = periodPosition.penPos.y;
				const double bottom = (top + baseLineHeight);

				if (pixelPerfect)
				{
					drawFunc(textureRegion, Math::Round(drawPos), top, bottom, isColorGlyph);
				}
				else
				{
					drawFunc(textureRegion.scaled(scale), drawPos, top, bottom, isColorGlyph);
				}
			}
		}

		return (not isOverflow);
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
