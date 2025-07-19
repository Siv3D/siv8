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
# include <Siv3D/HashMap.hpp>
# include <Siv3D/DynamicTexture.hpp>
# include "IGlyphCache.hpp"
# include "GlyphCacheCommon.hpp"

namespace s3d
{
	class MSDFGlyphCache : public IGlyphCache
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		MSDFGlyphCache() = default;

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

		RectF processHorizontal(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, Font::DrawFunc drawFunc, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontalFallback
		//
		////////////////////////////////////////////////////////////////

		std::pair<double, double> processHorizontalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, Font::DrawFunc drawFunc, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processVertical
		//
		////////////////////////////////////////////////////////////////

		RectF processVertical(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, Font::DrawFunc drawFunc, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processVerticalFallback
		//
		////////////////////////////////////////////////////////////////

		std::pair<double, double> processVerticalFallback(TextOperation textOperation, FontData& font, const ResolvedGlyph& resolvedGlyph, bool useBasePos, const Vec2& pos, double fontSize, const TextStyle& textStyle, Font::DrawFunc drawFunc, bool isColorGlyph, ReadingDirection readingDirection) override;

		////////////////////////////////////////////////////////////////
		//
		//	processHorizontalRect
		//
		////////////////////////////////////////////////////////////////

		bool processHorizontalRect(TextOperation textOperation, FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, const RectF& area, double fontSize, const TextStyle& textStyle, Font::DrawFunc drawFunc, bool isColorGlyph, ReadingDirection readingDirection) override;

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
	
	private:

		Texture _unused;
	};
}
