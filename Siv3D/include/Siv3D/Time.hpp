//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"

namespace s3d
{
	/// @brief 時間
	/// @remark 時間に関する機能を提供します。
	namespace Time
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetSec
		//
		////////////////////////////////////////////////////////////////

		/// @brief アプリケーションが起動してからの経過時間を秒で返します。 | Returns the number of seconds elapsed since the application started.
		/// @return アプリケーションが起動してからの経過時間（秒） | The number of seconds elapsed since the application started.
		[[nodiscard]]
		int64 GetSec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetMillisec
		//
		////////////////////////////////////////////////////////////////

		/// @brief アプリケーションが起動してからの経過時間をミリ秒で返します。 | Returns the number of milliseconds elapsed since the application started.
		/// @return アプリケーションが起動してからの経過時間（ミリ秒） | The number of milliseconds elapsed since the application started.
		[[nodiscard]]
		int64 GetMillisec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetMicrosec
		//
		////////////////////////////////////////////////////////////////

		/// @brief アプリケーションが起動してからの経過時間をマイクロ秒で返します。 | Returns the number of microseconds elapsed since the application started.
		/// @return アプリケーションが起動してからの経過時間（マイクロ秒） | The number of microseconds elapsed since the application started.
		[[nodiscard]]
		int64 GetMicrosec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetNanosec
		//
		////////////////////////////////////////////////////////////////

		/// @brief アプリケーションが起動してからの経過時間をナノ秒で返します。 | Returns the number of nanoseconds elapsed since the application started.
		/// @return アプリケーションが起動してからの経過時間（ナノ秒） | The number of nanoseconds elapsed since the application started.
		[[nodiscard]]
		int64 GetNanosec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetUptimeSec
		//
		////////////////////////////////////////////////////////////////

		/// @brief システムが起動してからの経過時間を秒で返します。 | Returns the number of seconds elapsed since the system started.
		/// @return システムが起動してからの経過時間（秒） | The number of seconds elapsed since the system started.
		[[nodiscard]]
		int64 GetUptimeSec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetUptimeMillisec
		//
		////////////////////////////////////////////////////////////////

		/// @brief システムが起動してからの経過時間をミリ秒で返します。 | Returns the number of milliseconds elapsed since the system started.
		/// @return システムが起動してからの経過時間（ミリ秒） | The number of milliseconds elapsed since the system started.
		[[nodiscard]]
		int64 GetUptimeMillisec() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetSecSinceEpoch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 協定世界時 (UTC) で 1970 年 1 月 1 日午前 0 時からの経過時間を秒で返します。 | Returns the number of seconds elapsed since 00:00:00 UTC, January 1, 1970.
		/// @return 1970 年 1 月 1 日午前 0 時からの経過時間（秒） | The number of seconds elapsed since 00:00:00 UTC, January 1, 1970.
		[[nodiscard]]
		uint64 GetSecSinceEpoch() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetMillisecSinceEpoch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 協定世界時 (UTC) で 1970 年 1 月 1 日午前 0 時からの経過時間をミリ秒で返します。 | Returns the number of milliseconds elapsed since 00:00:00 UTC, January 1, 1970.
		/// @return 1970 年 1 月 1 日午前 0 時からの経過時間（ミリ秒） | The number of milliseconds elapsed since 00:00:00 UTC, January 1, 1970.
		[[nodiscard]]
		uint64 GetMillisecSinceEpoch() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetMicrosecSinceEpoch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 協定世界時 (UTC) で 1970 年 1 月 1 日午前 0 時からの経過時間をマイクロ秒で返します。 | Returns the number of microseconds elapsed since 00:00:00 UTC, January 1, 1970.
		/// @return 1970 年 1 月 1 日午前 0 時からの経過時間（マイクロ秒） | The number of microseconds elapsed since 00:00:00 UTC, January 1, 1970.
		[[nodiscard]]
		uint64 GetMicrosecSinceEpoch() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	UTCOffsetMinutes
		//
		////////////////////////////////////////////////////////////////

		/// @brief 協定世界時 (UTC) との時差を分で返します。 | Returns the difference between local time and Coordinated Universal Time (UTC) in minutes.
		/// @return 協定世界時 (UTC) との時差（分） | The difference between local time and Coordinated Universal Time (UTC) in minutes.
		[[nodiscard]]
		int32 UTCOffsetMinutes() noexcept;
	}
}
