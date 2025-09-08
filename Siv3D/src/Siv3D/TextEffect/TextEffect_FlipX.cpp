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
# include <Siv3D/TextEffect/FlipX.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void FlipX::draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const
		{
			const ColorF color = (glyphContext.isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);

			textureRegion.mirrored(m_flipX).draw(glyphContext.pos, color);
		}
	}
}
