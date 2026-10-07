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
# include <cmath>
# include "../Common.hpp"
# include "PatternParameters.hpp"

namespace s3d
{
	namespace Pattern
	{
		/// @brief 縦横の帯が交互に上を通る、平織りの模様を指定します。
		/// @remark origin の交差では縦帯が上になり、隣接する交差ごとに上下が入れ替わります。
		/// 縦横の方向は angle による回転前の方向です。模様全体の周期は各軸で 2 * pitch です。
		/// @see PatternParameters 模様の基準座標と描画変換の扱い
		struct Weave
		{
			/// @brief 帯の色です。
			ColorF primary{ 1.0, 1.0 };

			/// @brief 帯の間と交差部の隙間の色です。
			ColorF background{ 0.0, 0.0 };

			/// @brief 平行な帯の中心間隔です。描画座標の単位で指定します。
			/// @remark 0 より大きい値を指定します。
			double pitch = 24.0;

			/// @brief 縦横の帯の幅です。描画座標の単位で指定します。
			/// @remark 0 以上 pitch 以下の値を指定します。0 では背景のみになります。
			double thickness = 10.0;

			/// @brief 上側の帯の各側面と、下側の帯の端との隙間です。描画座標の単位で指定します。
			/// @remark 0 以上 (pitch - thickness) / 2 以下の値を指定します。
			/// 0 では切れ目のない格子になります。thickness が pitch なら全体が帯の色になります。
			double gap = 2.0;

			/// @brief 模様全体の回転角度です。ラジアンで指定します。
			double angle = 0.0_deg;

			/// @brief 基準となる交差の中心です。描画座標で指定します。
			Vec2 origin{ 0, 0 };

			/// @brief 模様の描画パラメータに変換します。
			/// @pre 各数値は有限で、各メンバに記載された条件を満たす必要があります。
			/// @return 帯の配置、幅、および交差部の隙間を格納した描画パラメータ。
			[[nodiscard]]
			operator PatternParameters() const noexcept
			{
				const double c = std::cos(angle);
				const double s = std::sin(angle);
				const double invPitch = (1.0 / pitch);
				return{
					.primaryColor = primary.toFloat4(),
					.backgroundColor = background.toFloat4(),
					.uvTransform = {
						static_cast<float>(c * invPitch), static_cast<float>(-s * invPitch),
						static_cast<float>(s * invPitch), static_cast<float>(c * invPitch),
						static_cast<float>(-(origin.x * c + origin.y * s) * invPitch),
						static_cast<float>(-(-origin.x * s + origin.y * c) * invPitch) },
					.param0 = static_cast<float>(thickness * invPitch),
					.param1 = static_cast<float>((thickness + 2.0 * gap) * invPitch),
					.type = PatternType::Weave,
				};
			}
		};
	}
}
