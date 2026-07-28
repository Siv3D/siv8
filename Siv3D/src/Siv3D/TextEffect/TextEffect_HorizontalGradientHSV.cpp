//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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

			ColorF c0;
			ColorF c1;

			if (glyphContext.isColorGlyph)
			{
				c0 = ColorF{ 1.0, Math::Lerp(m_startColor.a, m_endColor.a, t0) };
				c1 = ColorF{ 1.0, Math::Lerp(m_startColor.a, m_endColor.a, t1) };
			}
			else
			{
				HSV interpolationEndColor = m_endColor;

				if (m_hueInterpolation == HueInterpolation::Shortest)
				{
					interpolationEndColor.h
						= m_startColor.lerpShortestHue(m_endColor, 1.0).h;
				}

				c0 = m_startColor.lerp(interpolationEndColor, t0).toColorF();
				c1 = m_startColor.lerp(interpolationEndColor, t1).toColorF();
			}

			textureRegion.draw(glyphContext.pos, Arg::left = c0, Arg::right = c1);
		}
	}
}
