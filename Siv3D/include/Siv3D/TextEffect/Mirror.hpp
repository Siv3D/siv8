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
		/// @brief 鏡文字テキスト描画エフェクト
		class Mirror : public BasicTextEffect
		{
		public:

			Mirror() = default;

			/// @brief コンストラクタ
			/// @param color 描画時に使用する色
			explicit constexpr Mirror(const ColorF& color) noexcept;

			/// @brief コンストラクタ
			/// @param miror 鏡文字にするかどうか
			/// @param color 描画時に使用する色
			explicit constexpr Mirror(bool miror, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const Vec2& pos, const int32, double, double, bool) const override;

		private:

			bool m_mirror = true;
		};
	}
}

# include "Mirror.ipp"
