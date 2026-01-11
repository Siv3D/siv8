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

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/2DShapes.hpp>
# include <Siv3D/Texture.hpp>
# include <Siv3D/Font.hpp>
# include "../FontData.hpp"

namespace s3d
{
	struct ITextEffect;

	class IGlyphCache
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	TextOperation
		//
		////////////////////////////////////////////////////////////////

		enum class TextOperation
		{
			Draw,
			Region,
		};

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		virtual ~IGlyphCache() = default;

		////////////////////////////////////////////////////////////////
		//
		//	preload
		//
		////////////////////////////////////////////////////////////////

		virtual bool preload(FontData& font, const Array<ResolvedGlyph>& resolvedGlyphs, const ReadingDirection readingDirection) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getTexture
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual const Texture& getTexture() noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontal
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual RectF processHorizontal(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontalFallback
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual std::pair<double, double> processHorizontalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, int32 index, int32 totalGlyphCount, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	processVertical
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual RectF processVertical(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	processVerticalFallback
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual std::pair<double, double> processVerticalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, int32 index, int32 totalGlyphCount, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontalRect
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual bool processHorizontalRect(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getXAdvances
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual Array<double> getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, double fontSize) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	xAdvanceFallback
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual double xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, double fontSize) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	setBufferThickness
		//
		////////////////////////////////////////////////////////////////

		virtual void setBufferThickness(int32 thickness) = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getBufferThickness
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual int32 getBufferThickness() const noexcept = 0;

		////////////////////////////////////////////////////////////////
		//
		//	getGlyph
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		virtual std::pair<GlyphInfo, TextureRegion> getGlyph(GlyphIndex glyphIndex, ReadingDirection readingDirection) = 0;
	};
}
