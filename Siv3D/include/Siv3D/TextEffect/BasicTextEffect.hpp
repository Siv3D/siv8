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
		/// @brief デフォルトのテキスト描画エフェクト
		class BasicTextEffect : public ITextEffect
		{
		public:

			BasicTextEffect() = default;

			/// @brief コンストラクタ
			/// @param color 描画時に使用する色
			explicit constexpr BasicTextEffect(const ColorF& color) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, int32 index, double top, double bottom, bool isColorGlyph) const override;

		protected:

			/// @brief 描画時に使用する色
			ColorF m_color = Palette::White;
		};
	}
}

# include "BasicTextEffect.ipp"
