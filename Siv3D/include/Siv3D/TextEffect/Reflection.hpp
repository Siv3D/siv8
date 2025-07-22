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
		class Reflection : public ITextEffect
		{
		public:

			explicit constexpr Reflection(double start = 0.5, double startAlpha = 0.5, double offsetScaleY = 0.0, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;

		private:

			ColorF m_color = Palette::White;

			double m_start = 0.5;

			double m_startAlpha = 0.5;

			double m_offsetScaleY = 0.0;
		};
	}
}

# include "Reflection.ipp"
