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
# include "Common.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PolyhedronUVMapping
	//
	////////////////////////////////////////////////////////////////

	/// @brief 正多面体に適用する UV マッピング方式
	enum class PolyhedronUVMapping : uint8
	{
		/// @brief 各面に独立した UV 座標を割り当てます。
		PerFace,

		/// @brief 正距円筒図法の球面 UV 座標を割り当てます。
		/// @remark Y 軸の正方向が V = 0、Y 軸の負方向が V = 1、X 軸の正方向が U = 0、Z 軸の正方向が U = 0.25 に対応します。
		/// @remark 経度の境界をまたぐ面では、連続した補間のため U 座標が 1.0 を超えることがあります。U 方向を Repeat に設定したサンプラーを使用してください。
		Spherical,
	};
}
