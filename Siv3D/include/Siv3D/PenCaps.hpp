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
# include "Types.hpp"

namespace s3d
{
	/// @brief ペンタブレットの機能サポート（取得可能な項目）
	struct PenCaps
	{
		/// @brief 法線方向（ペン先）の筆圧を取得可能か
		bool normalPressure	= false;
		
		/// @brief 接線方向（バレル／タンジェント）の筆圧を取得可能か
		/// @remark 一般的なペンでは未対応です（例：エアブラシ等の特殊入力で提供されることがあります）
		bool tangentialPressure = false;
		
		/// @brief ペンの向き（方位・仰角・ねじれ）を取得可能か
		bool orientation = false;

		/// @brief ペンの回転（pitch, roll, yaw）を取得可能か
		bool rotation = false;

		/// @brief 法線方向（ペン先）の筆圧の段階数
		int32 normalPressureLevels = 0;

		/// @brief 接線方向（バレル／タンジェント）の筆圧の段階数
		int32 tangentialPressureLevels = 0;
	};
}
