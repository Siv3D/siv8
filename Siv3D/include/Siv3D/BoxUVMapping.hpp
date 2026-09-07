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
# include "FloatRect.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BoxUVMapping
	//
	////////////////////////////////////////////////////////////////

	/// @brief 直方体の各面に割り当てる UV 矩形
	/// @remark 各矩形の成分は有限値である必要があります。
	/// @remark `right < left` または `bottom < top` を指定すると、対応する方向の UV 座標が反転します。
	/// @remark `left == right` または `top == bottom` を指定すると、対応する方向の UV 座標が一定になります。
	/// @remark 矩形は `[0, 1]` の範囲外でも構いません。
	struct BoxUVMapping
	{
		/// @brief 法線が Z 軸の負方向を向く面の UV 矩形
		FloatRect negativeZ{ 0.0f, 0.0f, 1.0f, 1.0f };

		/// @brief 法線が Z 軸の正方向を向く面の UV 矩形
		FloatRect positiveZ{ 0.0f, 0.0f, 1.0f, 1.0f };

		/// @brief 法線が X 軸の正方向を向く面の UV 矩形
		FloatRect positiveX{ 0.0f, 0.0f, 1.0f, 1.0f };

		/// @brief 法線が X 軸の負方向を向く面の UV 矩形
		FloatRect negativeX{ 0.0f, 0.0f, 1.0f, 1.0f };

		/// @brief 法線が Y 軸の正方向を向く面の UV 矩形
		FloatRect positiveY{ 0.0f, 0.0f, 1.0f, 1.0f };

		/// @brief 法線が Y 軸の負方向を向く面の UV 矩形
		FloatRect negativeY{ 0.0f, 0.0f, 1.0f, 1.0f };
	};
}
