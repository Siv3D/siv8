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

# pragma once
# include <Siv3D/Font.hpp>
# include "IGlyphCache.hpp"
# include "GlyphCacheManager.hpp"

namespace s3d
{
	class BitmapGlyphCache : public IGlyphCache
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		BitmapGlyphCache() = default;

		////////////////////////////////////////////////////////////////
		//
		//	getTexture
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const Texture& getTexture() noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontal
		//
		////////////////////////////////////////////////////////////////

		RectF processHorizontal(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontalFallback
		//
		////////////////////////////////////////////////////////////////

		std::pair<double, double> processHorizontalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, int32 index, int32 totalGlyphCount, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processVertical
		//
		////////////////////////////////////////////////////////////////

		RectF processVertical(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processVerticalFallback
		//
		////////////////////////////////////////////////////////////////

		std::pair<double, double> processVerticalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, int32 index, int32 totalGlyphCount, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontalRect
		//
		////////////////////////////////////////////////////////////////

		bool processHorizontalRect(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& area, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvances
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Array<double> getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, double fontSize) override;

		////////////////////////////////////////////////////////////////
		//
		//	xAdvanceFallback
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		double xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, double fontSize) override;

		////////////////////////////////////////////////////////////////
		//
		//	setBufferThickness
		//
		////////////////////////////////////////////////////////////////

		void setBufferThickness(int32 thickness) override;

		////////////////////////////////////////////////////////////////
		//
		//	getBufferThickness
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 getBufferThickness() const noexcept override;

	private:

		static constexpr int32 DefaultBufferThickness = 0;

		GlyphCacheManager m_glyphCacheManager{ DefaultBufferThickness };

		[[nodiscard]]
		bool prerender(FontData& font, const Array<ResolvedGlyph>& resolvedGlyphs, bool isMainFont, ReadingDirection readingDirection);
	};
}
