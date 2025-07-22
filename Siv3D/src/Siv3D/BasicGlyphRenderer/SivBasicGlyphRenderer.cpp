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

# include <Siv3D/BasicGlyphRenderer.hpp>

namespace s3d
{
	void BasicGlyphRenderer::draw(const TextureRegion& textureRegion, const Vec2& pos, [[maybe_unused]] const int32 index, [[maybe_unused]] const double top, [[maybe_unused]] const double bottom, const bool isColorGlyph) const
	{
		if (isColorGlyph)
		{
			textureRegion.draw(pos, ColorF{ 1.0, color.a });
		}
		else
		{
			textureRegion.draw(pos, color);
		}
	}
}
