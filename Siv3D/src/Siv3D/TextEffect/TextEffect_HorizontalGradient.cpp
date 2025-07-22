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

# include <Siv3D/Math.hpp>
# include <Siv3D/TextEffect/HorizontalGradient.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void HorizontalGradient::draw(const TextureRegion& textureRegion, const Vec2& pos, const int32, const double, const double, const bool isColorGlyph) const
		{
			const double textureWidth = textureRegion.size.x;
			const double textureStartX = pos.x;
			const double textureEndX = (pos.x + textureWidth);

			const double t0 = Math::Saturate(Math::InvLerp(m_startX, m_endX, textureStartX));
			const double t1 = Math::Saturate(Math::InvLerp(m_startX, m_endX, textureEndX));

			const ColorF c0 = (isColorGlyph ? ColorF{ 1.0, Math::Lerp(m_startColor.a, m_endColor.a, t0) } : m_startColor.lerp(m_endColor, t0));
			const ColorF c1 = (isColorGlyph ? ColorF{ 1.0, Math::Lerp(m_startColor.a, m_endColor.a, t1) } : m_startColor.lerp(m_endColor, t1));

			textureRegion.draw(pos, Arg::left = c0, Arg::right = c1);
		}
	}
}
