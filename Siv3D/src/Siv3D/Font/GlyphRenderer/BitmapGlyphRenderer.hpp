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
# include <Siv3D/BitmapGlyph.hpp>
# include "../FontUtility.hpp"

class SkFont;

namespace s3d
{
	[[nodiscard]]
	BitmapGlyph RenderBitmapGlyph(::FT_Face face, GlyphIndex glyphIndex, const FontFaceInfo& info, ReadingDirection readingDirection, SkFont* skFont);
}
