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
	/// @brief バッテリーの充電状態
	enum class BatteryChargeStatus : uint8
	{
		/// @brief 不明
		Unknown,
		
		/// @brief 放電中
		Discharging,

		/// @brief 充電中
		Charging,
		
		/// @brief 電源接続中だが充電はされていない（バッテリー保護等）
		NotCharging,
		
		/// @brief 充電完了
		Full,
	};
}
