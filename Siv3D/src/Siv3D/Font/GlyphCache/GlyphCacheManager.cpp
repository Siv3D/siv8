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

# include "GlyphCacheManager.hpp"
# include "../FontData.hpp"

namespace s3d
{
	namespace
	{
		inline constexpr InternalGlyphIndex VerticalGlyphFlag = (1u << 31);

		[[nodiscard]]
		constexpr InternalGlyphIndex AsBaseGlyphIndex(const InternalGlyphIndex glyphIndex) noexcept
		{
			return (glyphIndex & ~VerticalGlyphFlag);
		}

		[[nodiscard]]
		constexpr InternalGlyphIndex AsVerticalGlyphIndex(const InternalGlyphIndex glyphIndex) noexcept
		{
			return (glyphIndex | VerticalGlyphFlag);
		}

		[[nodiscard]]
		constexpr InternalGlyphIndex AsInternalGlyphIndex(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) noexcept
		{
			if (readingDirection == ReadingDirection::TopToBottom)
			{
				return AsVerticalGlyphIndex(glyphIndex);
			}
			else
			{
				return glyphIndex;
			}
		}

		[[nodiscard]]
		static constexpr Size CalculateInitialBitmapSize(const int32 fontBaseSize) noexcept
		{
			const int32 width =
				(fontBaseSize <= 16) ? 512 :
				(fontBaseSize <= 32) ? 768 :
				(fontBaseSize <= 48) ? 1024 :
				(fontBaseSize <= 64) ? 1536 :
				(fontBaseSize <= 256) ? 2048 : 4096;

			const int32 height = (fontBaseSize <= 256) ? 256 : 512;

			return{ width, height };
		}
	}

	const GlyphCache& GlyphCacheManager::get(const GlyphIndex glyphIndex, const ReadingDirection readingDirection) const
	{
		return m_glyphTable.find(AsInternalGlyphIndex(glyphIndex, readingDirection))->second;
	}

	bool GlyphCacheManager::isEmpty() const noexcept
	{
		return m_glyphTable.empty();
	}

	const Texture& GlyphCacheManager::getTexture() const noexcept
	{
		return m_texture;
	}

	bool GlyphCacheManager::cacheGlyph(FontData& font, const GlyphIndex glyphIndex, const ReadingDirection readingDirection)
	{
		// バッファ画像が作成されていない場合、作成する
		if (m_image.isEmpty())
		{
			const Size initialBitmapSize = CalculateInitialBitmapSize(font.getInfo().baseSize);
			m_image.resize(initialBitmapSize, BackgroundColor);
			m_isDirty = true;
			
			// Notdef グリフをキャッシュする
			cacheGlyph(font, GlyphIndexNotdef, ReadingDirection::LeftToRight);
			cacheGlyph(font, GlyphIndexNotdef, ReadingDirection::TopToBottom);
		}

		const InternalGlyphIndex internalGlyphIndex = AsInternalGlyphIndex(glyphIndex, readingDirection);

		if (m_glyphTable.contains(internalGlyphIndex))
		{
			// すでにキャッシュされている場合
			return true;
		}

		const BitmapGlyph bitmapGlyph = font.renderBitmapByGlyphIndex(glyphIndex, readingDirection);

		// グリフのレンダリングに失敗した場合
		if (bitmapGlyph.glyphIndex != glyphIndex)
		{
			return false;
		}

		if (internalGlyphIndex & VerticalGlyphFlag)
		{
			// 横書きグリフがすでにキャッシュされている場合
			if (auto it = m_glyphTable.find(AsBaseGlyphIndex(internalGlyphIndex));
				it != m_glyphTable.end())
			{
				const auto& horiCache = it->second;

				// すでにキャッシュされているグリフと同サイズの場合、それを再利用する
				if (Size{ horiCache.textureRegionWidth, horiCache.textureRegionHeight } == bitmapGlyph.image.size())
				{
					GlyphCache cache;
					cache.info = bitmapGlyph;
					cache.textureRegionLeft		= horiCache.textureRegionLeft;
					cache.textureRegionTop		= horiCache.textureRegionTop;
					cache.textureRegionWidth	= horiCache.textureRegionWidth;
					cache.textureRegionHeight	= horiCache.textureRegionHeight;
					m_glyphTable.emplace(internalGlyphIndex, cache);
					return true;
				}
			}
		}
		else
		{
			// 縦書きグリフがすでにキャッシュされている場合
			if (auto it = m_glyphTable.find(AsVerticalGlyphIndex(internalGlyphIndex));
				it != m_glyphTable.end())
			{
				const auto& vertCache = it->second;

				// すでにキャッシュされているグリフと同サイズの場合、それを再利用する
				if (Size{ vertCache.textureRegionWidth, vertCache.textureRegionHeight } == bitmapGlyph.image.size())
				{
					GlyphCache cache;
					cache.info = bitmapGlyph;
					cache.textureRegionLeft		= vertCache.textureRegionLeft;
					cache.textureRegionTop		= vertCache.textureRegionTop;
					cache.textureRegionWidth	= vertCache.textureRegionWidth;
					cache.textureRegionHeight	= vertCache.textureRegionHeight;
					m_glyphTable.emplace(internalGlyphIndex, cache);
					return true;
				}
			}
		}

		const int32 glyphWidth	= bitmapGlyph.image.width();
		const int32 glyphHeight	= bitmapGlyph.image.height();

		// キャッシュ画像の更新
		{
			m_penPos.x += m_padding;

			// 右に余白が足りない場合、次の行へ進む
			if (m_image.width() < (m_penPos.x + (glyphWidth + m_padding)))
			{
				m_penPos.x = m_padding;
				m_penPos.y += (m_currentMaxHeight + (m_padding * 2));
				m_currentMaxHeight = 0;
			}

			// 下に余白が足りない場合、バッファ画像の高さを拡張する
			if (m_image.height() < (m_penPos.y + (glyphHeight + m_padding)))
			{
				const int32 newBitmapHeight = (((m_penPos.y + (glyphHeight + m_padding)) + 255) / 256 * 256);

				if (MaxHeight < newBitmapHeight)
				{
					return false;
				}

				m_image.resizeHeight(newBitmapHeight, BackgroundColor);
				m_isDirty = true;
			}

			// キャッシュ画像にグリフ画像を書き込む
			{
				bitmapGlyph.image.overwrite(m_image, m_penPos);
				m_isDirty = true;
			}

			m_currentMaxHeight = Max(m_currentMaxHeight, glyphHeight);
		}

		// キャッシュテーブルの更新
		{
			GlyphCache cache;
			cache.info = bitmapGlyph;
			cache.textureRegionLeft		= static_cast<int16>(m_penPos.x);
			cache.textureRegionTop		= static_cast<int16>(m_penPos.y);
			cache.textureRegionWidth	= static_cast<int16>(glyphWidth);
			cache.textureRegionHeight	= static_cast<int16>(glyphHeight);
			m_glyphTable.emplace(internalGlyphIndex, cache);
		}

		m_penPos.x += (glyphWidth + m_padding);

		return true;
	}

	void GlyphCacheManager::updateTexture()
	{
		if (not m_isDirty)
		{
			return;
		}

		if (m_texture.size() == m_image.size())
		{
			m_texture.fill(m_image);
		}
		else
		{
			m_texture = DynamicTexture{ m_image };
		}

		m_isDirty = false;
	}
}
