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
# include <Siv3D/Math.hpp>
# include <Siv3D/TextEffect/HorizontalGradientHSV.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void HorizontalGradientHSV::draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const
		{
			const double textureWidth = textureRegion.size.x;
			const double textureStartX = glyphContext.pos.x;
			const double textureEndX = (glyphContext.pos.x + textureWidth);

			const double t0 = Math::Saturate(Math::InvLerp(m_startX, m_endX, textureStartX));
			const double t1 = Math::Saturate(Math::InvLerp(m_startX, m_endX, textureEndX));

			const ColorF c0 = (glyphContext.isColorGlyph ? HSV{ ColorF{ 1.0, Math::Lerp(m_startColor.a, m_endColor.a, t0) } } : m_startColor.lerp(m_endColor, t0));
			const ColorF c1 = (glyphContext.isColorGlyph ? HSV{ ColorF{ 1.0, Math::Lerp(m_startColor.a, m_endColor.a, t1) } } : m_startColor.lerp(m_endColor, t1));

			textureRegion.draw(glyphContext.pos, Arg::left = c0, Arg::right = c1);
		}
	}
}
