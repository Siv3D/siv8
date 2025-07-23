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
		/// @brief 水平方向にグラデーションするテキスト描画エフェクト (HSV)
		class HorizontalGradientHSV : public ITextEffect
		{
		public:

			HorizontalGradientHSV() = default;

			/// @brief コンストラクタ
			/// @param startColor グラデーションの開始色
			/// @param endColor グラデーションの終了色
			/// @param startX グラデーション開始位置の X 座標（ピクセル）
			/// @param endX グラデーション終了位置の X 座標（ピクセル）
			constexpr HorizontalGradientHSV(const HSV& startColor, const HSV& endColor, double startX, double endX) noexcept;

			void draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const override;

		private:

			HSV m_startColor = Palette::White;

			HSV m_endColor = Palette::White;

			double m_startX = 0.0;

			double m_endX = 0.0;
		};
	}
}

# include "HorizontalGradientHSV.ipp"
