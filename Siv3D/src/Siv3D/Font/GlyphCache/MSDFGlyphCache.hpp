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

		MSDFGlyphCache() = default;

		[[nodiscard]]
		Array<double> getXAdvances(FontData& font, StringView s, const Array<ResolvedGlyph>& resolvedGlyphs, double fontSize) override;
	
		[[nodiscard]]
		double xAdvanceFallback(FontData& font, const ResolvedGlyph& resolvedGlyph, double fontSize) override;
	
	private:

	};
}
