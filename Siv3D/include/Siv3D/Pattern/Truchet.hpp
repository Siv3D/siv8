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
		/// @brief 正方形タイル内の 2 本の円弧が、隣接するタイルの円弧につながる模様を指定します。
		/// @remark 円弧の中心線は半径 pitch / 2 の四分円です。タイルの各辺の中点で滑らかにつながります。
		/// @see PatternParameters 模様の基準座標と描画変換の扱い
		struct Truchet
		{
			/// @brief タイル内の円弧の向きの配置方法です。
			enum class Layout : uint8
			{
				/// @brief タイル座標と seed で向きを決定します。同じ値から同じ配置を得られます。
				Random,

				/// @brief 全タイルで、左上と右下の頂点を中心とする円弧を描きます。方向は angle による回転前のものです。
				Uniform,

				/// @brief 隣接するタイルごとに向きを入れ替えます。タイル (0, 0) は Uniform と同じ向きです。
				Alternating,
			};

			/// @brief 円弧の色です。
			ColorF primary{ 1.0, 1.0 };

			/// @brief 背景色です。
			ColorF background{ 0.0, 0.0 };

			/// @brief 正方形タイルの一辺の長さです。描画座標の単位で指定します。
			/// @remark 0 より大きい値を指定します。
			double pitch = 40.0;

			/// @brief 円弧の線幅です。描画座標の単位で指定します。
			/// @remark 0 以上 pitch 以下の値を指定します。0 では背景のみ、pitch では全体が円弧の色になります。
			/// 太い円弧同士は重なります。
			double thickness = 6.0;

			/// @brief 模様全体の回転角度です。ラジアンで指定します。
			double angle = 0.0_deg;

			/// @brief タイル (0, 0) の左上の頂点です。描画座標で指定します。
			Vec2 origin{ 0, 0 };

			/// @brief タイル内の円弧の向きの配置方法です。
			Layout layout = Layout::Random;

			/// @brief ランダム配置の seed です。すべての uint32 値を使用できます。
			/// @remark layout が Random の場合に使用します。ほかの配置には影響しません。
			uint32 seed = 0;

			/// @brief 模様の描画パラメータに変換します。
			/// @pre 各数値は有限で、各メンバに記載された条件を満たす必要があります。
			/// layout は Layout に定義された値を指定します。
			/// @return 円弧の配置、線幅、およびタイルの向きを格納した描画パラメータ。
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
					.param1 = 0.0f,
					.type = PatternType::Truchet,
					// Keep every seed bit in finite, exactly representable floats.
					.extraParams = { static_cast<float>(seed & 0xFFFFu), static_cast<float>(seed >> 16),
						static_cast<float>(layout), 0.0f },
				};
			}
		};
	}
}
