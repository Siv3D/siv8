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
		/// @brief 正方格子上の水玉の半径が、指定した方向に滑らかに変化する模様を指定します。
		/// @remark 各水玉の中心を start から end への直線に射影し、その位置に応じて半径を smoothstep で補間します。
		/// start より手前では minRadius、end より先では maxRadius になります。
		/// @remark 半径は水玉ごとに一定です。開始・終了位置で水玉を切り取ることはありません。
		/// @see PatternParameters 模様の基準座標と描画変換の扱い
		struct PolkaDotSizeGradient
		{
			/// @brief 水玉の色です。
			ColorF primary{ 1.0, 1.0 };

			/// @brief 背景色です。
			ColorF background{ 0.0, 0.0 };

			/// @brief 隣接する水玉の中心間隔です。描画座標の単位で指定します。
			/// @remark 0 より大きい値を指定します。
			double pitch = 36.0;

			/// @brief 変化の開始側の半径です。描画座標の単位で指定します。
			/// @remark 0 以上 maxRadius 以下の値を指定します。0 の水玉は描画されません。
			double minRadius = 0.0;

			/// @brief 変化の終了側の半径です。描画座標の単位で指定します。
			/// @remark minRadius 以上 pitch / 2 以下の値を指定します。
			double maxRadius = 12.0;

			/// @brief 正方格子の回転角度です。ラジアンで指定します。
			/// @remark 変化の方向には影響しません。
			double angle = 45.0_deg;

			/// @brief 格子の基準となる水玉の中心です。描画座標で指定します。
			/// @remark start と end は移動しません。
			Vec2 origin{ 0, 0 };

			/// @brief 半径の変化が始まる位置です。描画座標で指定します。
			/// @remark end と異なる位置を指定します。水玉の配置には影響しません。
			Vec2 start{ 0, 100 };

			/// @brief 半径の変化が終わる位置です。描画座標で指定します。
			/// @remark start と異なる位置を指定します。水玉の配置には影響しません。
			Vec2 end{ 0, 400 };

			/// @brief 模様の描画パラメータに変換します。
			/// @pre 各数値は有限で、各メンバに記載された条件を満たす必要があります。
			/// @return 水玉の配置、半径、および変化の範囲を格納した描画パラメータ。
			[[nodiscard]]
			operator PatternParameters() const noexcept
			{
				const double invPitch = (1.0 / pitch);
				const double c = std::cos(angle);
				const double s = std::sin(angle);
				const Vec2 direction = (end - start);
				const Vec2 gradient = (direction / direction.lengthSq());
				// Express the drawing-space linear field in the lattice's UV coordinates.
				const double gx = (pitch * (c * gradient.x + s * gradient.y));
				const double gy = (pitch * (-s * gradient.x + c * gradient.y));
				const double bias = ((origin - start).dot(gradient) - 0.5 * (gx + gy));

				return{
					.primaryColor = primary.toFloat4(),
					.backgroundColor = background.toFloat4(),
					.uvTransform = {
						static_cast<float>(c * invPitch), static_cast<float>(-s * invPitch),
						static_cast<float>(s * invPitch), static_cast<float>(c * invPitch),
						static_cast<float>(0.5 - (origin.x * c + origin.y * s) * invPitch),
						static_cast<float>(0.5 - (-origin.x * s + origin.y * c) * invPitch) },
					.param0 = static_cast<float>(2.0 * minRadius * invPitch),
					.param1 = static_cast<float>(2.0 * maxRadius * invPitch),
					.type = PatternType::PolkaDotSizeGradient,
					.extraParams = { static_cast<float>(gx), static_cast<float>(gy), static_cast<float>(bias), 0.0f },
				};
			}
		};
	}
}
