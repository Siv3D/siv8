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
# include "BasicTextEffect.hpp"

namespace s3d
{
	namespace TextEffect
	{
		/// @brief 文字を四角形にワープさせるテキスト描画エフェクト
		/// @remark Local と Camera の適用後に射影変換を行い、外側の ScopedQuadWarp2D と合成します。
		/// @remark 変換先が非凸または退化している場合や、Local と Camera によってグリフが潰れる場合は描画しません。
		class QuadWarp : public BasicTextEffect
		{
		public:

			[[nodiscard]]
			QuadWarp() = default;

			/// @brief コンストラクタ
			/// @param leftOffset 左側のオフセット
			/// @param rightOffset 右側のオフセット
			/// @param color 描画時に使用する色
			[[nodiscard]]
			constexpr QuadWarp(double leftOffset, double rightOffset, const ColorF& color = Palette::White) noexcept;

			void draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const override;

		private:

			double m_leftOffset = 0.0;

			double m_rightOffset = 0.0;
		};
	}
}

# include "QuadWarp.ipp"
