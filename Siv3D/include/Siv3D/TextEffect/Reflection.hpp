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
		/// @brief 反射付きテキスト描画エフェクト
		class Reflection : public ITextEffect
		{
		public:

			Reflection() = default;

			/// @brief コンストラクタ
			/// @param start 反射する領域（0.0 から 1.0 の範囲）
			/// @param startAlpha 反射の開始位置のアルファ値（0.0 から 1.0 の範囲）
			/// @param offsetScaleY 反射位置を調整する Y オフセットのスケール値（倍率）
			/// @param color 描画時に使用する色
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
