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
	class BitmapGlyphCache : public IGlyphCache
	{
	public:

		BitmapGlyphCache() = default;

		[[nodiscard]]
		Array<double> getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, double fontSize) override;
	
		[[nodiscard]]
		double xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, double fontSize) override;
	
	private:

		HashMap<GlyphIndex, GlyphCache> m_glyphTable;

		DynamicTexture m_texture;

		bool m_hasDirty = false;

		BufferImage m_buffer = {};

		[[nodiscard]]
		bool prerender(FontData& font, const Array<ResolvedGlyph>& resolvedGlyphs, bool isMainFont, ReadingDirection readingDirection);
		
		void updateTexture();
	};
}
