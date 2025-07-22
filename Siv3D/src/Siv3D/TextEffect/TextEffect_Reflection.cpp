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

# include <Siv3D/TextEffect/Reflection.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void Reflection::draw(const TextureRegion& textureRegion, const Vec2& pos, const int32, const double top, const double bottom, const bool isColorGlyph) const
		{
			const ColorF color = (isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);
			textureRegion.draw(pos, color);
			textureRegion.flipped()(0, 0, textureRegion.size.x, (textureRegion.size.y * m_start))
				.draw(Vec2{ pos.x, (bottom + (bottom - top) * m_offsetScaleY) }, Arg::top(color, (color.a * m_startAlpha)), Arg::bottom(color, 0.0));
		}
	}
}
