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

# include <Siv3D/TextEffect/Mirror.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void Mirror::draw(const TextureRegion& textureRegion, const Vec2& pos, const int32, const double, const double, const bool isColorGlyph) const
		{
			const ColorF color = (isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);

			textureRegion.mirrored(m_mirror).draw(pos, color);
		}
	}
}
