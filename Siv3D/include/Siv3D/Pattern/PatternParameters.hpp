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
# include <array>
# include "../Common.hpp"
# include "../PointVector.hpp"
# include "../ColorHSV.hpp"
# include "../Mat3x2.hpp"
# include "PatternType.hpp"

namespace s3d
{
	/// @brief 図形の塗りつぶしや枠に使用する模様の描画パラメータです。
	/// @remark ローカル・カメラ変換前の共通の描画座標を基準とし、模様も図形と同じ変換を受けます。
	/// 図形ごとの原点は設定されません。図形の座標自体を変更する操作は、模様の原点を移動しません。
	/// @remark 組み込み Pattern ピクセルシェーダとカスタム頂点シェーダを組み合わせる場合、
	/// 頂点シェーダは出力の float2 uv に変換前の描画座標を渡す必要があります。
	/// Pattern 描画でピクセルシェーダのみを置き換えた場合も、uv にはこの座標が渡されます。
	struct PatternParameters
	{
		Float4 primaryColor{ 1.0f, 1.0f, 1.0f, 1.0f };

		Float4 backgroundColor{ 0.0f, 0.0f, 0.0f, 0.0f };

		/// @brief 模様の基準座標を、模様を評価する UV 座標に変換する行列です。
		Mat3x2 uvTransform = Mat3x2::Identity();

		float param0 = 0.0f;

		float param1 = 0.0f;

		PatternType type = PatternType::PolkaDot;

		/// @brief UV 変換・模様固有のパラメータ・背景色をシェーダ定数用に格納します。
		/// @return { m11, m12, m31, m32 }, { m21, m22, param0, param1 }, backgroundColor の 3 要素。primaryColor と type は含みません。
		[[nodiscard]]
		constexpr std::array<Float4, 3> toFloat4Array() const noexcept;

		/// @brief UV 変換に倍率補正を加え、シェーダ定数用に格納します。
		/// @param rmsScalingInv UV 変換の線形部分と平行移動部分に乗じる倍率
		/// @return 補正後の行列を格納した toFloat4Array() と同じ形式の 3 要素
		[[nodiscard]]
		constexpr std::array<Float4, 3> toFloat4Array(float rmsScalingInv) const noexcept;
	};
}

# include "PatternParameters.ipp"
