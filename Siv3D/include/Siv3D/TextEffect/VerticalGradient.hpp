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
		class VerticalGradient : public ITextEffect
		{
		public:

			VerticalGradient() = default;

			constexpr VerticalGradient(const ColorF& startColor, const ColorF& endColor, double startOffset = 0.0, double endOffset = 1.0) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;

		private:

			/// @brief グラデーションの開始色
			ColorF m_startColor = Palette::White;

			/// @brief グラデーションの終了色
			ColorF m_endColor = Palette::White;

			/// @brief グラデーションの開始位置のオフセット (0.0 から 1.0 の範囲)
			double m_startOffset = 0.0;

			/// @brief グラデーションの終了位置のオフセット (0.0 から 1.0 の範囲)
			double m_endOffset = 1.0;
		};
	}
}

# include "VerticalGradient.ipp"
