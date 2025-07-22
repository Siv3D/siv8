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
# include "BasicTextEffect.hpp"

namespace s3d
{
	namespace TextEffect
	{
		class VerticalScale : public BasicTextEffect
		{
		public:

			VerticalScale() = default;

			constexpr VerticalScale(double scale, const ColorF& color = Palette::White) noexcept;

			constexpr VerticalScale(double scale, double center, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;

		private:

			double m_scale = 1.0;

			double m_center = 0.5;
		};
	}
}

# include "VerticalScale.ipp"
