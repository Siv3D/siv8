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
	/// @brief 円形の破線スタイル
    struct CircularDashStyle
    {
		/// @brief 破線の数
        uint32 dashCount = 12;
      
		/// @brief 破線とスペースの比率 (0.0 ～ 1.0)
        double dashRatio = 0.5;
        
		/// @brief 破線の開始位置の角度オフセット（ラジアン）
        double startAngle = 0.0;
    };
}
