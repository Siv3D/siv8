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
# include <Siv3D/TextEffect/BasicTextEffect.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void BasicTextEffect::draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const
		{
			const ColorF color = (glyphContext.isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);

			textureRegion.draw(glyphContext.pos, color);
		}
	}
}
