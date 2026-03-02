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
# import <Foundation/Foundation.h>
# import <IOKit/ps/IOPowerSources.h>
# import <IOKit/ps/IOPSKeys.h>
# include <mutex>
# include <chrono>
# include <atomic>
# include <algorithm>

namespace s3d
{
	namespace System
	{
		// IOKit を叩く重い処理を分離
		static PowerStatus UpdatePowerStatusInternal()
		{
			PowerStatus status;

			status.lowPowerMode = [[NSProcessInfo processInfo] isLowPowerModeEnabled];
			
			CFTypeRef blob = IOPSCopyPowerSourcesInfo();
			if (!blob) return status;

			CFStringRef providingSource = IOPSGetProvidingPowerSourceType(blob);
			if (providingSource)
			{
				if (CFEqual(providingSource, CFSTR(kIOPSACPowerValue)))
					status.externalPowerStatus = ExternalPowerStatus::Connected;
				else if (CFEqual(providingSource, CFSTR(kIOPSBatteryPowerValue)))
					status.externalPowerStatus = ExternalPowerStatus::NotConnected;
			}

			CFArrayRef list = IOPSCopyPowerSourcesList(blob);
			if (!list)
			{
				CFRelease(blob);
				return status;
			}

			status.hasBattery = false;
			status.chargeStatus = BatteryChargeStatus::Unknown;

			bool foundBattery = false;
			const CFIndex count = CFArrayGetCount(list);

			for (CFIndex i = 0; i < count; ++i)
			{
				CFTypeRef ps = CFArrayGetValueAtIndex(list, i);
				CFDictionaryRef dict = IOPSGetPowerSourceDescription(blob, ps);
				if (!dict) continue;

				CFStringRef type = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(kIOPSTypeKey));
				if (type && CFEqual(type, CFSTR(kIOPSInternalBatteryType)))
				{
					status.hasBattery = true;
				}
				else
				{
					CFBooleanRef isPresent = (CFBooleanRef)CFDictionaryGetValue(dict, CFSTR(kIOPSIsPresentKey));
					if (isPresent == kCFBooleanTrue) status.hasBattery = true;
				}

				if (status.hasBattery && !foundBattery)
				{
					foundBattery = true;

					int currentCapacity = 0, maxCapacity = 0, batteryPercentage = 0;
					CFNumberRef currentCapNum = (CFNumberRef)CFDictionaryGetValue(dict, CFSTR(kIOPSCurrentCapacityKey));
					if (currentCapNum) CFNumberGetValue(currentCapNum, kCFNumberIntType, &currentCapacity);

					CFNumberRef maxCapNum = (CFNumberRef)CFDictionaryGetValue(dict, CFSTR(kIOPSMaxCapacityKey));
					if (maxCapNum) CFNumberGetValue(maxCapNum, kCFNumberIntType, &maxCapacity);

					if (maxCapacity > 0)
					{
						batteryPercentage = std::clamp((currentCapacity * 100) / maxCapacity, 0, 100);
						status.batteryLevel = batteryPercentage;
					}
					else if (currentCapNum)
					{
						batteryPercentage = std::clamp(currentCapacity, 0, 100);
						status.batteryLevel = batteryPercentage;
					}

					bool isCharging = false, isCharged = false;
					CFBooleanRef isChargingBool = (CFBooleanRef)CFDictionaryGetValue(dict, CFSTR(kIOPSIsChargingKey));
					if (isChargingBool == kCFBooleanTrue) isCharging = true;

					CFBooleanRef isChargedBool = (CFBooleanRef)CFDictionaryGetValue(dict, CFSTR(kIOPSIsChargedKey));
					if (isChargedBool == kCFBooleanTrue) isCharged = true;

					if (isCharging)
					{
						status.chargeStatus = BatteryChargeStatus::Charging;
					}
					else if (status.externalPowerStatus == ExternalPowerStatus::NotConnected)
					{
						status.chargeStatus = BatteryChargeStatus::Discharging;
					}
					else if (status.externalPowerStatus == ExternalPowerStatus::Connected)
					{
						if (isCharged || (batteryPercentage == 100))
							status.chargeStatus = BatteryChargeStatus::Full;
						else
							status.chargeStatus = BatteryChargeStatus::NotCharging;
					}

					if (status.chargeStatus == BatteryChargeStatus::Discharging)
					{
						CFTimeInterval timeRemaining = IOPSGetTimeRemainingEstimate();
						if ((timeRemaining != kIOPSTimeRemainingUnknown) && (timeRemaining != kIOPSTimeRemainingUnlimited))
						{
							status.timeToEmptySec = static_cast<int32>(timeRemaining);
						}
						else
						{
							int timeToEmptyMin = -1;
							CFNumberRef tteNum = (CFNumberRef)CFDictionaryGetValue(dict, CFSTR(kIOPSTimeToEmptyKey));
							if (tteNum) CFNumberGetValue(tteNum, kCFNumberIntType, &timeToEmptyMin);
							if (timeToEmptyMin > 0) status.timeToEmptySec = (timeToEmptyMin * 60);
						}
					}
					else if (status.chargeStatus == BatteryChargeStatus::Charging)
					{
						int timeToFullMin = -1;
						CFNumberRef ttfNum = (CFNumberRef)CFDictionaryGetValue(dict, CFSTR(kIOPSTimeToFullChargeKey));
						if (ttfNum) CFNumberGetValue(ttfNum, kCFNumberIntType, &timeToFullMin);
						if (timeToFullMin > 0) status.timeToFullChargeSec = (timeToFullMin * 60);
					}
				}
			}

			CFRelease(list);
			CFRelease(blob);

			return status;
		}

		////////////////////////////////////////////////////////////////
		//
		//	GetPowerStatus
		//
		////////////////////////////////////////////////////////////////

		PowerStatus GetPowerStatus()
		{
			static std::mutex s_mutex;
			static PowerStatus s_cachedStatus;
			static std::chrono::steady_clock::time_point s_lastUpdateTime;
			static std::atomic<bool> s_isUpdating{ false };
			static bool s_initialized = false;

			std::lock_guard<std::mutex> lock(s_mutex);

			auto now = std::chrono::steady_clock::now();

			// 初回呼び出し時は即座に（同期的に）取得して返す
			if (not s_initialized)
			{
				s_cachedStatus = UpdatePowerStatusInternal();
				s_lastUpdateTime = now;
				s_initialized = true;
				return s_cachedStatus;
			}

			// キャッシュの有効期限（1秒）が切れているか
			if (now - s_lastUpdateTime >= std::chrono::milliseconds(1000))
			{
				// すでにバックグラウンドで更新中でなければ、更新タスクをディスパッチ
				bool expected = false;
				if (s_isUpdating.compare_exchange_strong(expected, true))
				{
					// QoS: Utility (ゲームループの邪魔にならない優先度) で非同期実行
					dispatch_async(dispatch_get_global_queue(QOS_CLASS_UTILITY, 0), ^{
						
						PowerStatus newStatus = UpdatePowerStatusInternal();

						// 取得し終わったらキャッシュを更新
						std::lock_guard<std::mutex> innerLock(s_mutex);
						s_cachedStatus = newStatus;
						s_lastUpdateTime = std::chrono::steady_clock::now();
						s_isUpdating = false;
					});
				}
			}

			// キャッシュの期限が切れていてバックグラウンド更新中であっても、
			// 待機はせずに即座に前回のキャッシュを返す (スパイク防止)
			return s_cachedStatus;
		}
	}
}
