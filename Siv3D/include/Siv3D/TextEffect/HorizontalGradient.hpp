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
		/// @brief 水平方向にグラデーションするテキスト描画エフェクト
		class HorizontalGradient : public ITextEffect
		{
		public:

			[[nodiscard]]
			HorizontalGradient() = default;

			/// @brief コンストラクタ
			/// @param startColor グラデーションの開始色
			/// @param endColor グラデーションの終了色
			/// @param startX グラデーション開始位置の X 座標（ピクセル）
			/// @param endX グラデーション終了位置の X 座標（ピクセル）
			[[nodiscard]]
			constexpr HorizontalGradient(const ColorF& startColor, const ColorF& endColor, double startX, double endX) noexcept;

			void draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const override;

		private:

			ColorF m_startColor = Palette::White;

			ColorF m_endColor = Palette::White;

			double m_startX = 0.0;

			double m_endX = 0.0;
		};
	}
}

# include "HorizontalGradient.ipp"
