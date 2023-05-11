//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Array.hpp"
# include "MonitorInfo.hpp"

namespace s3d
{
	namespace System
	{
		/// @brief 使用可能なモニターの一覧を取得します。
		/// @return 使用可能なモニターの一覧
		[[nodiscard]]
		Array<MonitorInfo> EnumerateMonitors();

		/// @brief ウィンドウが配置されているモニターのインデックスを取得します。
		/// @return ウィンドウが配置されているモニターのインデックス
		[[nodiscard]]
		size_t GetCurrentMonitorIndex();

		/// @brief ウィンドウが配置されているモニターの情報を取得します。
		/// @return ウィンドウが配置されているモニターの情報
		[[nodiscard]]
		MonitorInfo GetCurrentMonitor();
	}
}
