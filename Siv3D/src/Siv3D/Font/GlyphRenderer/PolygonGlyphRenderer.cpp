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

# include <Siv3D/Geometry2D/Misc.hpp>
# include "PolygonGlyphRenderer.hpp"
# include "OutlineGlyphRenderer.hpp"

namespace s3d
{
	PolygonGlyph RenderPolygonGlyph(const ::FT_Face face, const GlyphIndex glyphIndex, const FontFaceInfo& info, const ReadingDirection readingDirection)
	{
		const OutlineGlyph outlineGlyph = RenderOutlineGlyph(face, glyphIndex, CloseRing::No, info, readingDirection);

		PolygonGlyph polygonGlyph;
		polygonGlyph.glyphIndex			= outlineGlyph.glyphIndex;
		polygonGlyph.bufferThickness	= outlineGlyph.bufferThickness;
		polygonGlyph.left				= outlineGlyph.left;
		polygonGlyph.top				= outlineGlyph.top;
		polygonGlyph.width				= outlineGlyph.width;
		polygonGlyph.height				= outlineGlyph.height;
		polygonGlyph.ascender			= outlineGlyph.ascender;
		polygonGlyph.descender			= outlineGlyph.descender;
		polygonGlyph.advance			= outlineGlyph.advance;
		polygonGlyph.polygons			= Geometry2D::ComposePolygons(outlineGlyph.rings);
		return polygonGlyph;
	}
}
