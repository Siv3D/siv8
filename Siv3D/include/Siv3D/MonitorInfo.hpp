﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "Optional.hpp"
# include "PointVector.hpp"
# include "String.hpp"
# include "2DShapes.hpp"
# include "Array.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MonitorInfo
	//
	////////////////////////////////////////////////////////////////

	/// @brief モニターの情報
	struct MonitorInfo
	{
		/// @brief ディスプレイの名前
		String name;

		/// @brief ディスプレイ ID
		String id;

		/// @brief 内部的に使われているディスプレイの名前
		String displayDeviceName;

		/// @brief ディスプレイ全体の位置とサイズ
		Rect displayRect = Rect::Empty();

		/// @brief タスクバーなどを除いた利用可能な領域の位置とサイズ
		Rect workArea = Rect::Empty();

		/// @brief フルスクリーン時の解像度
		Size fullscreenResolution{ 0, 0 };

		/// @brief 物理的なサイズ (mm)
		Optional<Size> sizeMillimeter;

		/// @brief メインディスプレイであるか
		bool isPrimary = false;

		/// @brief UI の拡大倍率
		Optional<double> scaling;

		/// @brief リフレッシュレート
		Optional<double> refreshRate;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief モニターの情報を文字列に変換します。
		/// @return モニターの情報
		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const MonitorInfo& value);
	};

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	EnumerateMonitors
		//
		////////////////////////////////////////////////////////////////

		/// @brief 使用可能なモニターの一覧を取得します。
		/// @return 使用可能なモニターの一覧
		[[nodiscard]]
		Array<MonitorInfo> EnumerateMonitors();

		////////////////////////////////////////////////////////////////
		//
		//	GetCurrentMonitorIndex
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウが配置されているモニターのインデックスを取得します。
		/// @return ウィンドウが配置されているモニターのインデックス
		[[nodiscard]]
		size_t GetCurrentMonitorIndex();

		////////////////////////////////////////////////////////////////
		//
		//	GetCurrentMonitor
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウが配置されているモニターの情報を取得します。
		/// @return ウィンドウが配置されているモニターの情報
		/// @throw Error 情報の取得に失敗した場合
		[[nodiscard]]
		MonitorInfo GetCurrentMonitor();
	}
}
