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
# include "Optional.hpp"
# include "ExternalPowerStatus.hpp"
# include "BatteryChargeStatus.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	PowerStatus
	//
	////////////////////////////////////////////////////////////////

	/// @brief システムの電源の状態
	struct PowerStatus
	{
		/// @brief 外部電源の接続状態
		ExternalPowerStatus externalPowerStatus = ExternalPowerStatus::Unknown;

		/// @brief 充電状態
		BatteryChargeStatus chargeStatus = BatteryChargeStatus::Unknown;

		/// @brief バッテリーを搭載しているか（デスクトップPC判定などに使用）
		bool hasBattery = true;

		/// @brief 省電力モード（低電力モード・バッテリーセーバー）が有効か
		bool lowPowerMode = false;

		/// @brief バッテリー残量 (0 ～ 100)。取得できない場合やバッテリー非搭載の場合は none
		Optional<int32> batteryLevel;

		/// @brief バッテリーが空になるまでの残り時間（秒）。計算中やAC駆動時、不明な場合は none
		Optional<int32> timeToEmptySec;

		/// @brief フル充電されるまでの所要時間（秒）。計算中や放電時、不明な場合は none
		Optional<int32> timeToFullChargeSec;
	};

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetPowerStatus
		//
		////////////////////////////////////////////////////////////////

		/// @brief システムの電源の状態を返します。
		/// @return システムの電源の状態
		[[nodiscard]]
		PowerStatus GetPowerStatus();
	}
}
