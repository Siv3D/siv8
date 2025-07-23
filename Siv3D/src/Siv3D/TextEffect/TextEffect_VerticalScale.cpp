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
# include <Siv3D/TexturedQuad.hpp>
# include <Siv3D/TextEffect/VerticalScale.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void VerticalScale::draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const
		{
			const ColorF color = (glyphContext.isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);

			const double textureHeight = textureRegion.size.y;
			const double textureStartY = glyphContext.pos.y;
			const double textureEndY = (glyphContext.pos.y + textureHeight);
			const double centerY = Math::Lerp(glyphContext.top, glyphContext.bottom, m_center);

			const double compressedY0 = Math::Lerp(centerY, textureStartY, m_scale);
			const double compressedY1 = Math::Lerp(centerY, textureEndY, m_scale);

			RectF{ glyphContext.pos.x, compressedY0, textureRegion.size.x, (compressedY1 - compressedY0) }(textureRegion).draw(color);
		}
	}
}
