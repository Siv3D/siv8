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

# include <Siv3D/GlyphContext.hpp>
# include <Siv3D/TextEffect/QuadWarp.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void QuadWarp::draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const
		{
			const ColorF color = (glyphContext.isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);

			Quad quad = RectF{ glyphContext.pos, textureRegion.size }.asQuad();

			quad.p0.y -= (textureRegion.size.x * m_leftOffset);
			quad.p1.y -= (textureRegion.size.x * m_rightOffset);
			
			textureRegion.drawQuadWarp(quad, color);
		}
	}
}
