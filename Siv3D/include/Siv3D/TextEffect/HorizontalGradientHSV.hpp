//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
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
		/// @brief HSV 色空間における色相の補間方式
		enum class HueInterpolation : uint8
		{
			/// @brief H 成分を数直線上で補間します。
			Linear,

			/// @brief 色相の最短経路を使って補間します。
			Shortest,
		};

		/// @brief 水平方向にグラデーションするテキスト描画エフェクト (HSV)
		class HorizontalGradientHSV : public ITextEffect
		{
		public:

			/// @brief デフォルトコンストラクタ
			[[nodiscard]]
			HorizontalGradientHSV() = default;

			/// @brief HSV 色空間で水平方向にグラデーションするテキスト描画エフェクトを作成します。
			/// @param startColor グラデーションの開始色
			/// @param endColor グラデーションの終了色
			/// @param startX グラデーション開始位置の X 座標（ピクセル）
			/// @param endX グラデーション終了位置の X 座標（ピクセル）
			/// @param hueInterpolation 色相の補間方式
			[[nodiscard]]
			constexpr HorizontalGradientHSV(
				const HSV& startColor,
				const HSV& endColor,
				double startX,
				double endX,
				HueInterpolation hueInterpolation = HueInterpolation::Shortest) noexcept;

			void draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const override;

		private:

			HSV m_startColor = Palette::White;

			HSV m_endColor = Palette::White;

			double m_startX = 0.0;

			double m_endX = 0.0;

			HueInterpolation m_hueInterpolation = HueInterpolation::Shortest;
		};
	}
}

# include "HorizontalGradientHSV.ipp"
