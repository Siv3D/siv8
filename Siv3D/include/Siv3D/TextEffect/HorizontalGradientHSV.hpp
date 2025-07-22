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

# pragma once
# include "../ITextEffect.hpp"

namespace s3d
{
	namespace TextEffect
	{
		class HorizontalGradientHSV : public ITextEffect
		{
		public:

			HorizontalGradientHSV() = default;

			constexpr HorizontalGradientHSV(const HSV& startColor, const HSV& endColor, double startX, double endX) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;

		private:

			HSV m_startColor = Palette::White;

			HSV m_endColor = Palette::White;

			double m_startX = 0.0;

			double m_endX = 0.0;
		};
	}
}

# include "HorizontalGradientHSV.ipp"
