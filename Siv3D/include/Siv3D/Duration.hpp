//-----------------------------------------------
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
# include <chrono>
# include "Common.hpp"
# include "StringView.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Days / DaysF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 日数 | Days
	using Days = std::chrono::duration<int32, std::ratio<86400>>;

	/// @brief 日数 | Days
	using DaysF = std::chrono::duration<double, std::ratio<86400>>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Hours / HoursF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 時間 | Hours
	using Hours = std::chrono::hours;

	/// @brief 時間 | Hours
	using HoursF = std::chrono::duration<double, std::ratio<3600>>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Minutes / MinutesF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 分 | Minutes
	using Minutes = std::chrono::minutes;

	/// @brief 分 | Minutes
	using MinutesF = std::chrono::duration<double, std::ratio<60>>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Seconds / SecondsF
	//
	////////////////////////////////////////////////////////////////

	/// @brief 秒 | Seconds
	using Seconds = std::chrono::seconds;

	/// @brief 秒 | Seconds
	using SecondsF = std::chrono::duration<double>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Milliseconds / MillisecondsF
	//
	////////////////////////////////////////////////////////////////

	/// @brief ミリ秒 | Milliseconds
	using Milliseconds = std::chrono::milliseconds;

	/// @brief ミリ秒 | Milliseconds
	using MillisecondsF = std::chrono::duration<double, std::milli>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Microseconds / MicrosecondsF
	//
	////////////////////////////////////////////////////////////////

	/// @brief マイクロ秒 | Microseconds
	using Microseconds = std::chrono::microseconds;

	/// @brief マイクロ秒 | Microseconds
	using MicrosecondsF = std::chrono::duration<double, std::micro>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Nanoseconds / NanosecondsF
	//
	////////////////////////////////////////////////////////////////

	/// @brief ナノ秒 | Nanoseconds
	using Nanoseconds = std::chrono::nanoseconds;

	/// @brief ナノ秒 | Nanoseconds
	using NanosecondsF = std::chrono::duration<double, std::nano>;
	
	////////////////////////////////////////////////////////////////
	//
	//	Duration
	//
	////////////////////////////////////////////////////////////////

	/// @brief 秒 | Seconds
	using Duration = SecondsF;
	
	////////////////////////////////////////////////////////////////
	//
	//	DurationCast
	//
	////////////////////////////////////////////////////////////////

	/// @brief 時間の単位を変換します。 | Converts the unit of time.
	/// @tparam DurationTo 変換前の時間の型 | The type of the time before conversion.
	/// @tparam DurationFrom 変換後の時間の型 | The type of the time after conversion.
	/// @param duration 変換する時間 | The time to convert.
	/// @return 変換後の時間 | The time after conversion.
	template <class DurationTo, class DurationFrom>
	[[nodiscard]]
	constexpr DurationTo DurationCast(const DurationFrom& duration) noexcept;

	inline namespace Literals
	{
		inline namespace DurationLiterals
		{	
			////////////////////////////////////////////////////////////////
			//
			//	_d
			//
			////////////////////////////////////////////////////////////////

			/// @brief Days 型の値を表現するユーザ定義リテラル | User-defined literal to represent a value of type Days.
			/// @param days 日数 | Days
			/// @return 日数 | Days
			[[nodiscard]]
			constexpr Days operator ""_d(unsigned long long days);

			/// @brief DaysF 型の値を表現するユーザ定義リテラル | User-defined literal to represent a value of type DaysF.
			/// @param days 日数 | Days
			/// @return 日数 | Days
			[[nodiscard]]
			constexpr DaysF operator ""_d(long double days);
		}
	}
	
	////////////////////////////////////////////////////////////////
	//
	//	FormatTime
	//
	////////////////////////////////////////////////////////////////

	/// @brief 時間を文字列に変換します。
	/// DD		日 (00-)
	/// D		日 (0-)
	/// dd		日 (00-)
	/// d		日 (0-)
	/// HH		時 (00-)
	/// H		時 (0-)
	/// hh		時 (00-24)
	/// h		時 (0-24)
	/// MM		分 (00-)
	/// M		分 (0-)
	/// mm		分 (00-59)
	/// m		分 (0-59)
	/// SS		秒 (00-)
	/// S		秒 (0-)
	/// ss		秒 (00-59)
	/// s		秒 (0-59)
	/// X		ミリ秒 (0-)
	/// x		小数点以下 1 桁秒 (0-9)
	/// xx		小数点以下 2 桁秒 (00-99)
	/// xxx		小数点以下 3 桁秒 (000-999)
	/// @param duration 時間
	/// @param format フォーマット指定
	/// @return フォーマットされた時間
	[[nodiscard]]
	String FormatTime(const Duration& duration, StringView format = U"HH:mm:ss.xxx"_sv);
	
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 日数をフォーマットします。 | Formats the number of days.
	/// @param formatData フォーマットデータ | Format data
	/// @param days 日数 | Days
	void Formatter(FormatData& formatData, const Days& days);

	/// @brief 日数をフォーマットします。 | Formats the number of days.
	/// @param formatData フォーマットデータ | Format data
	/// @param days 日数 | Days
	void Formatter(FormatData& formatData, const DaysF& days);

	/// @brief 時間をフォーマットします。 | Formats the number of hours.
	/// @param formatData フォーマットデータ | Format data
	/// @param hours 時間 | Hours
	void Formatter(FormatData& formatData, const Hours& hours);

	/// @brief 時間をフォーマットします。 | Formats the number of hours.
	/// @param formatData フォーマットデータ | Format data
	/// @param hours 時間 | Hours
	void Formatter(FormatData& formatData, const HoursF& hours);

	/// @brief 分をフォーマットします。 | Formats the number of minutes.
	/// @param formatData フォーマットデータ | Format data
	/// @param minutes 分 | Minutes
	void Formatter(FormatData& formatData, const Minutes& minutes);

	/// @brief 分をフォーマットします。 | Formats the number of minutes.
	/// @param formatData フォーマットデータ | Format data
	/// @param minutes 分 | Minutes
	void Formatter(FormatData& formatData, const MinutesF& minutes);

	/// @brief 秒をフォーマットします。 | Formats the number of seconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param seconds 秒 | Seconds
	void Formatter(FormatData& formatData, const Seconds& seconds);

	/// @brief 秒をフォーマットします。 | Formats the number of seconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param seconds 秒 | Seconds
	void Formatter(FormatData& formatData, const SecondsF& seconds);

	/// @brief ミリ秒をフォーマットします。 | Formats the number of milliseconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param milliseconds ミリ秒 | Milliseconds
	void Formatter(FormatData& formatData, const Milliseconds& milliseconds);

	/// @brief ミリ秒をフォーマットします。 | Formats the number of milliseconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param milliseconds ミリ秒 | Milliseconds
	void Formatter(FormatData& formatData, const MillisecondsF& milliseconds);

	/// @brief マイクロ秒をフォーマットします。 | Formats the number of microseconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param microseconds マイクロ秒 | Microseconds
	void Formatter(FormatData& formatData, const Microseconds& microseconds);

	/// @brief マイクロ秒をフォーマットします。 | Formats the number of microseconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param microseconds マイクロ秒 | Microseconds
	void Formatter(FormatData& formatData, const MicrosecondsF& microseconds);

	/// @brief ナノ秒をフォーマットします。 | Formats the number of nanoseconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param nanoseconds ナノ秒 | Nanoseconds
	void Formatter(FormatData& formatData, const Nanoseconds& nanoseconds);

	/// @brief ナノ秒をフォーマットします。 | Formats the number of nanoseconds.
	/// @param formatData フォーマットデータ | Format data
	/// @param nanoseconds ナノ秒 | Nanoseconds
	void Formatter(FormatData& formatData, const NanosecondsF& nanoseconds);
}

# include "detail/Duration.ipp"
