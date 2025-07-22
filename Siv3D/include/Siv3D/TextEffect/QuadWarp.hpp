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
		/// @brief 文字を四角形にワープさせるテキスト描画エフェクト
		class QuadWarp : public BasicTextEffect
		{
		public:

			QuadWarp() = default;

			/// @brief コンストラクタ
			/// @param scaleOffset ワープ効果のスケール値
			/// @param color 描画時に使用する色
			constexpr QuadWarp(double scaleOffset, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;

		private:

			double m_scaleOffset = 0.0;
		};
	}
}

# include "QuadWarp.ipp"
