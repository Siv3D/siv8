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

# include <Siv3D/TextEffect/VerticalGradient.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void VerticalGradient::draw(const TextureRegion& textureRegion, const Vec2& pos, [[maybe_unused]] const int32 index, const double top, const double bottom, const bool isColorGlyph) const
		{
			const ColorF c0 = (isColorGlyph ? ColorF{ 1.0, m_startColor.a } : m_startColor);
			const ColorF c1 = (isColorGlyph ? ColorF{ 1.0, m_endColor.a } : m_endColor);

			const double textureWidth = textureRegion.size.x;
			const double textureHeight = textureRegion.size.y;
			const double textureStartY = pos.y;
			const double textureEndY = (pos.y + textureHeight);

			const double gradientStartY = Math::Lerp(top, bottom, m_startOffset);
			const double gradientEndY = Math::Lerp(top, bottom, m_endOffset);

			const double textureTopHeight = Clamp((gradientStartY - textureStartY), 0.0, textureHeight);
			const double textureBottomHeight = Clamp((textureEndY - gradientEndY), 0.0, textureHeight);

			// 上部（startOffset より上）は単色 c0
			if (textureTopHeight)
			{
				textureRegion(0, 0, textureWidth, textureTopHeight).draw(pos.x, textureStartY, c0);
			}

			// グラデーション領域
			if (const double textureMiddleHeight = (textureHeight - textureTopHeight - textureBottomHeight))
			{
				const double y = Max(gradientStartY, textureStartY);
				const double t0 = Math::InvLerp(gradientStartY, gradientEndY, y);
				const double t1 = Math::InvLerp(gradientStartY, gradientEndY, (y + textureMiddleHeight));
				textureRegion(0, textureTopHeight, textureWidth, textureMiddleHeight).draw(pos.x, y, Arg::top = c0.lerp(c1, t0), Arg::bottom = c0.lerp(c1, t1));
			}

			// 下部（endOffset より下）は単色 c1
			if (textureBottomHeight)
			{
				textureRegion(0, (textureHeight - textureBottomHeight), textureWidth, textureBottomHeight).draw(Arg::bottomLeft(pos.x, textureEndY), c1);
			}
		}
	}
}
