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
		/// @brief 垂直方向に拡大縮小するテキスト描画エフェクト
		class VerticalScale : public BasicTextEffect
		{
		public:

			[[nodiscard]]
			VerticalScale() = default;

			/// @brief コンストラクタ
			/// @param scale 拡大縮小の倍率
			/// @param color 描画時に使用する色
			[[nodiscard]]
			constexpr VerticalScale(double scale, const ColorF& color = Palette::White) noexcept;

			/// @brief コンストラクタ
			/// @param scale 拡大縮小の倍率
			/// @param center 拡大縮小の中心位置 (0.0 から 1.0 の範囲)
			/// @param color 描画時に使用する色
			[[nodiscard]]
			constexpr VerticalScale(double scale, double center, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const override;

		private:

			double m_scale = 1.0;

			double m_center = 0.5;
		};
	}
}

# include "VerticalScale.ipp"
