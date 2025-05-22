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
# include <Siv3D/OutlineGlyph.hpp>
# include "OutlineGlyphRenderer.hpp"
# include "../FontUtility.hpp"

namespace s3d
{
	[[nodiscard]]
	OutlineGlyph RenderOutlineGlyph(::FT_Face face, GlyphIndex glyphIndex, CloseRing closeRing, const FontFaceInfo& info, ReadingDirection readingDirection);
}
