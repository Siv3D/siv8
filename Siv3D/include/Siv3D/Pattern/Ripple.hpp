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
# include "../Common.hpp"
# include "PatternParameters.hpp"

namespace s3d
{
	namespace Pattern
	{
		/// @brief 等間隔の同心円による波紋模様を指定します。
		/// @remark 中心からの距離から radiusOffset を引いた値を、pitch を周期として繰り返します。
		/// 距離の値が pitch の整数倍となる位置を中心に、thickness の幅で塗ります。
		/// 中心まで帯が届いた場合は、中心も塗りつぶします。
		/// @see PatternParameters 模様の基準座標と描画変換の扱い
		struct Ripple
		{
			/// @brief 波紋の色です。
			ColorF primary{ 1.0, 1.0 };

			/// @brief 背景色です。
			ColorF background{ 0.0, 0.0 };

			/// @brief 隣接する円の中心線の半径差です。描画座標の単位で指定します。
			/// @remark 0 より大きい値を指定します。
			double pitch = 24.0;

			/// @brief 円の線幅です。描画座標の単位で指定します。
			/// @remark 0 以上 pitch 以下の値を指定します。0 では背景のみ、pitch では全体が波紋の色になります。
			double thickness = 4.0;

			/// @brief 同心円の中心です。描画座標で指定します。
			Vec2 center{ 0, 0 };

			/// @brief 半径方向のずらし量です。描画座標の単位で指定します。
			/// @remark 値を増やすと外向きに移動します。pitch の整数倍だけ異なる値は同じ模様になります。
			double radiusOffset = 0.0;

			/// @brief 模様の描画パラメータに変換します。
			/// @pre 各数値は有限で、各メンバに記載された条件を満たす必要があります。
			/// @return 同心円の配置と線幅を格納した描画パラメータ。
			[[nodiscard]]
			operator PatternParameters() const noexcept
			{
				const double invPitch = (1.0 / pitch);
				return{
					.primaryColor = primary.toFloat4(),
					.backgroundColor = background.toFloat4(),
					.uvTransform = {
						static_cast<float>(invPitch), 0.0f, 0.0f, static_cast<float>(invPitch),
						static_cast<float>(-center.x * invPitch), static_cast<float>(-center.y * invPitch) },
					.param0 = static_cast<float>(thickness * invPitch),
					.param1 = static_cast<float>(radiusOffset * invPitch),
					.type = PatternType::Ripple,
				};
			}
		};
	}
}
