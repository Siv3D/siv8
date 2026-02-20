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

# include <Siv3D/Common.hpp>
# include <Siv3D/PowerStatus.hpp>
# include <Siv3D/Windows/MinWindows.hpp>

namespace s3d
{
	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetPowerStatus
		//
		////////////////////////////////////////////////////////////////

		PowerStatus GetPowerStatus()
		{
			PowerStatus status;

			SYSTEM_POWER_STATUS sps;
			if (::GetSystemPowerStatus(&sps) == FALSE)
			{
				return status;
			}

			// 外部電源の接続状態
			if (sps.ACLineStatus == AC_LINE_OFFLINE)
			{
				status.externalPowerStatus = ExternalPowerStatus::NotConnected;
			}
			else if (sps.ACLineStatus == AC_LINE_ONLINE)
			{
				status.externalPowerStatus = ExternalPowerStatus::Connected;
			}
			else
			{
				status.externalPowerStatus = ExternalPowerStatus::Unknown;
			}

			// バッテリー搭載の有無
			status.hasBattery = (sps.BatteryFlag != BATTERY_FLAG_NO_BATTERY);

			// 省電力モード (バッテリーセーバー) が有効か
			status.lowPowerMode = (sps.SystemStatusFlag == SYSTEM_STATUS_FLAG_POWER_SAVING_ON);

			if (status.hasBattery)
			{
				// バッテリー残量 (0 ～ 100)
				if (sps.BatteryLifePercent <= 100)
				{
					status.batteryLevel = sps.BatteryLifePercent;
				}

				// バッテリーの充電状態
				if (sps.BatteryFlag != BATTERY_FLAG_UNKNOWN) // Unknown でない場合
				{
					if (sps.BatteryFlag & BATTERY_FLAG_CHARGING) // Charging
					{
						status.chargeStatus = BatteryChargeStatus::Charging;
					}
					else if (status.externalPowerStatus == ExternalPowerStatus::NotConnected)
					{
						status.chargeStatus = BatteryChargeStatus::Discharging;
					}
					else if (status.externalPowerStatus == ExternalPowerStatus::Connected)
					{
						if (sps.BatteryLifePercent == 100)
						{
							status.chargeStatus = BatteryChargeStatus::Full;
						}
						else
						{
							// 電源接続中だが充電はされていない状態（バッテリー保護機能が働いている場合など）
							status.chargeStatus = BatteryChargeStatus::NotCharging;
						}
					}
				}

				if ((status.chargeStatus == BatteryChargeStatus::Discharging) &&
					(sps.BatteryLifeTime != BATTERY_LIFE_UNKNOWN))
				{
					status.timeToEmptySec = sps.BatteryLifeTime;
				}
			}
			else
			{
				// バッテリー非搭載時
				status.chargeStatus = BatteryChargeStatus::Unknown;
			}

			return status;
		}
	}
}
