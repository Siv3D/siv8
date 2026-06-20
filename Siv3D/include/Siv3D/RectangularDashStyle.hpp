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
	/// @brief 長方形の破線スタイル
	struct RectangularDashStyle
	{
		/// @brief 破線の数
		uint32 dashCount = 12;

		/// @brief 1 周期における破線部分の比率
		/// @remark 0.0 のとき描画なし、1.0 のとき実線
		double dashRatio = 0.5;

		/// @brief 破線の開始位置のオフセット
		/// @remark 左上角から時計回りの距離
		double offset = 0.0;
	};
}
