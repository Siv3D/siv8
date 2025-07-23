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
# include <Siv3D/MSDFGlyph.hpp>
# include "../FontUtility.hpp"

namespace s3d
{
	[[nodiscard]]
	MSDFGlyph RenderMSDFGlyph(::FT_Face face, GlyphIndex glyphIndex, const FontFaceInfo& info, int32 bufferThickness, ReadingDirection readingDirection);
}
