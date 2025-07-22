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

		enum class TextOperation
		{
			Draw,
			Region,
		};

		virtual ~IGlyphCache() = default;

		[[nodiscard]]
		virtual const Texture& getTexture() noexcept = 0;

		[[nodiscard]]
		virtual RectF processHorizontal(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		[[nodiscard]]
		virtual std::pair<double, double> processHorizontalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		[[nodiscard]]
		virtual RectF processVertical(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		[[nodiscard]]
		virtual std::pair<double, double> processVerticalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, const double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		[[nodiscard]]
		virtual bool processHorizontalRect(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& pos, double fontSize, const TextStyle& textStyle, const ITextEffect& textEffect, bool isColorGlyph, ReadingDirection readingDirection) = 0;

		[[nodiscard]]
		virtual Array<double> getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, double fontSize) = 0;

		[[nodiscard]]
		virtual double xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, double fontSize) = 0;
	
		/*
		virtual void setBufferWidth(int32 width) = 0;

		virtual int32 getBufferWidth() const noexcept = 0;

		virtual bool preload(const FontData& font, StringView s) = 0;

		[[nodiscard]]
		virtual TextureRegion getTextureRegion(const FontData& font, GlyphIndex glyphIndex) = 0;

		[[nodiscard]]
		virtual int32 getBufferThickness(GlyphIndex glyphIndex) = 0*/;
	};
}
