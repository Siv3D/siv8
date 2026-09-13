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
# include "../MathConstants.hpp"
# include "PatternParameters.hpp"

namespace s3d
{
	namespace Pattern
	{
		/// @brief 正弦波の線を等間隔に並べた模様を指定します。
		/// @remark angle が 0 のとき、中心線は origin を基準として
		/// y = amplitude * sin(2π * x / wavelength) + n * pitch（n は整数）になります。
		/// @remark 線幅は中心線の傾きに応じて近似的に補正されます。
		/// 急な曲がりや太い線では、中心線から一定距離の輪郭とは異なります。隣接する線は重なることがあります。
		/// @see PatternParameters 模様の基準座標と描画変換の扱い
		struct Wave
		{
			/// @brief 波線の色です。
			ColorF primary{ 1.0, 1.0 };

			/// @brief 背景色です。
			ColorF background{ 0.0, 0.0 };

			/// @brief 波線の中心線を並べる間隔です。描画座標の単位で指定します。
			/// @remark 0 より大きい値を指定します。曲線間の最短距離ではありません。
			double pitch = 24.0;

			/// @brief 波線の太さの目安です。描画座標の単位で指定します。
			/// @remark 0 以上 pitch 以下の値を指定します。0 では背景のみ、pitch では全体が波線の色になります。
			double thickness = 4.0;

			/// @brief 波の振幅です。描画座標の単位で指定します。
			/// @remark 0 以上の値を指定します。0 では直線の縞模様になります。
			double amplitude = 6.0;

			/// @brief 波の 1 周期の長さです。描画座標の単位で指定します。
			/// @remark 0 より大きい値を指定します。
			double wavelength = 80.0;

			/// @brief 模様全体の回転角度です。ラジアンで指定します。
			double angle = 0.0_deg;

			/// @brief 波の変位が 0 を通過する中心線上の基準位置です。描画座標で指定します。
			Vec2 origin{ 0, 0 };

			/// @brief 模様の描画パラメータに変換します。
			/// @pre 各数値は有限で、各メンバに記載された条件を満たす必要があります。
			/// @return 波線の配置、太さ、および波形を格納した描画パラメータ。
			[[nodiscard]]
			operator PatternParameters() const noexcept
			{
				const double c = std::cos(angle);
				const double s = std::sin(angle);
				const double invPitch = (1.0 / pitch);
				const double invWavelength = (1.0 / wavelength);

				return{
					.primaryColor = primary.toFloat4(),
					.backgroundColor = background.toFloat4(),
					.uvTransform = {
						static_cast<float>(c * invWavelength), static_cast<float>(-s * invPitch),
						static_cast<float>(s * invWavelength), static_cast<float>(c * invPitch),
						static_cast<float>(-(origin.x * c + origin.y * s) * invWavelength),
						static_cast<float>(-(-origin.x * s + origin.y * c) * invPitch) },
					.param0 = static_cast<float>(thickness * invPitch),
					.param1 = static_cast<float>(amplitude * invPitch),
					.type = PatternType::Wave,
					.extraParams = { static_cast<float>(Math::TwoPi * amplitude * invWavelength), 0.0f, 0.0f, 0.0f },
				};
			}
		};
	}
}
