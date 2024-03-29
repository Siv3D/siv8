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
# include "Time.hpp"

namespace s3d
{
	/// @brief ミリ秒カウンター
	/// @remark 経過時間をミリ秒で計測します。
	class MillisecClock
	{
	public:

		/// @brief 経過時間（ミリ秒）を返します。
		/// @return 経過時間（ミリ秒）
		[[nodiscard]]
		int64 ms() const noexcept;

		/// @brief ログに経過時間を出力します。
		void log() const;

		/// @brief コンソールに経過時間を出力します。
		void console() const;

		/// @brief 経過時間をデバッグ表示します。
		void print() const;

	private:

		const int64 m_start = Time::GetMillisec();
	};
}

# include "detail/MillisecClock.ipp"
