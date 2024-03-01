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
# include "Common.hpp"
# include "DayOfWeek.hpp"
# include "String.hpp"
# include "Duration.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Date
	//
	////////////////////////////////////////////////////////////////

	/// @brief 日付 | Date
	struct Date
	{
		/// @brief 西暦 | Year
		int32 year;

		/// @brief 月 [1-12] | Month [1-12]
		int32 month;

		/// @brief 日 [1-31] | Day [1-31]
		int32 day;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ | Default constructor
		[[nodiscard]]
		Date() noexcept = default;

		/// @brief 日付を作成します。 | Creates a date.
		/// @param _year 西暦 | Year
		/// @param _month 月 [1-12] | Month [1-12]
		/// @param _day 日 [1-31] | Day [1-31]
		/// @remark 日付の妥当性検証は行われません。 | No validation is performed.
		[[nodiscard]] 
		constexpr Date(int32 _year, int32 _month, int32 _day) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	daysOfWeek
		//
		////////////////////////////////////////////////////////////////

		/// @brief 曜日を返します。 | Returns the day of the week.
		/// @return 曜日 | Day of the week
		[[nodiscard]]
		constexpr DayOfWeek dayOfWeek() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isToday
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のローカルの年月日と一致するかを返します。 | Returns true if the date matches the current local date.
		/// @return 現在のローカルの年月日と一致する場合 true, それ以外の場合は false | Returns true if the date matches the current local date.
		[[nodiscard]]
		bool isToday() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isLeapYear
		//
		////////////////////////////////////////////////////////////////

		/// @brief うるう年であるかを返します。 | Returns true if the year is a leap year.
		/// @return うるう年である場合 true, それ以外の場合は false | Returns true if the year is a leap year.
		[[nodiscard]]
		constexpr bool isLeapYear() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	daysInYear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 年の日数を返します。 | Returns the number of days in a year.
		/// @return 1 年の日数 | The number of days in a year.
		[[nodiscard]]
		constexpr int32 daysInYear() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	daysInMonth
		//
		////////////////////////////////////////////////////////////////

		/// @brief 月の日数を返します。 | Returns the number of days in a month.
		/// @return 月の日数 | The number of days in a month.
		[[nodiscard]]
		constexpr int32 daysInMonth() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isValid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付の妥当性を返します。 | Returns the validity of the date.
		/// @return 日付が正しい範囲の値であれば true, それ以外の場合は false | Returns true if the date is valid.
		[[nodiscard]]
		constexpr bool isValid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付を指定したフォーマットの文字列で返します。 | Returns the date as a string in the specified format.
		/// yyyy	4 桁の年 (0001-) | 4-digit year (0001-)
		/// yy		2 桁の年 (00-99) | 2-digit year (00-99)
		/// y		年 (1-) | Year (1-)
		/// MMMM	英語の月 (January-December) | English month (January-December)
		/// MMM		英語の月の略称 (Jan-Dec) | English month abbreviation (Jan-Dec)
		/// MM		2 桁の月 (01-12) | 2-digit month (01-12)
		/// M		1-2 桁の月 (1-12) | 1-2 digit month (1-12)
		/// dd		2 桁の日 (01-31) | 2-digit day (01-31)
		/// d		1-2 桁の日 (1-31) | 1-2 digit day (1-31)
		/// EEEE	英語の曜日 (Sunday-Saturday) | English day of the week (Sunday-Saturday)
		/// EEE		英語の曜日の略称 (Sun-Sat) | English day of the week abbreviation (Sun-Sat)
		/// @param format フォーマット指定 | Format
		/// @return フォーマットされた日付 | Formatted date
		[[nodiscard]]
		String format(StringView format = U"yyyy-MM-dd"_sv) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した日数だけ進めた日付を返します。 | Returns the date advanced by the specified number of days.
		/// @param days 日数 | Days
		/// @return 指定した日数だけ進めた日付 | Date advanced by the specified number of days
		Date operator +(const Days& days) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した日数だけ戻した日付を返します。 | Returns the date moved back by the specified number of days.
		/// @param days 日数 | Days
		/// @return 指定した日数だけ戻した日付 | Date moved back by the specified number of days
		Date operator -(const Days& days) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ++
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付を進めます。 | Advances the date.
		/// @return *this
		Date& operator ++() noexcept;

		/// @brief 日付を進めます。 | Advances the date.
		/// @return 進めた日付 | Advanced date
		Date operator ++(int) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator --
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付を戻します。 | Moves the date back.
		/// @return *this
		Date& operator --() noexcept;

		/// @brief 日付を戻します。 | Moves the date back.
		/// @return 戻した日付 | Moved-back date
		Date operator --(int) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの日付の差を計算します。 | Calculates the difference between two dates.
		/// @param to 開始日 | Start date
		/// @param from 終了日 | End date
		/// @return `from` から `to` までの日数 | The number of days from `from` to `to`
		[[nodiscard]]
		friend constexpr Days operator -(const Date& to, const Date& from) noexcept
		{
			return Date::Subtract(to, from);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付を進めます | Advances the date
		/// @param days 進める日数 | Days to advance
		/// @return *this
		Date& operator +=(const Days& days) noexcept;

		/// @brief 日付を戻します | Moves the date back
		/// @param days 戻す日数 | Days to move back
		/// @return *this
		Date& operator -=(const Days& days) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==, operator <=>
		//
		////////////////////////////////////////////////////////////////

		friend constexpr bool operator ==(const Date&, const Date&) = default;

		/// @brief 二つの Date オブジェクトを比較して順序を判断します。 | Compares two Date objects for ordering and equality.
		/// @param lhs 比較する最初の Date オブジェクト | First Date object to compare
		/// @param rhs 比較する二番目の Date オブジェクト | Second Date object to compare
		/// @return 三方向比較結果 (std::strong_ordering) | A three-way comparison result (std::strong_ordering)
		friend constexpr auto operator <=>(const Date&, const Date&) = default;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付のハッシュ値を返します | Returns the hash value of the date
		/// @return 日付のハッシュ値 | Hash value of the date
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Yesterday
		//
		////////////////////////////////////////////////////////////////

		/// @brief 昨日の日付（ローカルタイム）を返します。 | Returns yesterday's date (local time).
		/// @return 昨日の日付（ローカルタイム） | Yesterday's date (local time)
		[[nodiscard]]
		static Date Yesterday() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Today
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の日付（ローカルタイム）を返します。 | Returns the current date (local time).
		/// @return 現在の日付（ローカルタイム） | Current date (local time)
		[[nodiscard]]
		static Date Today() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Tomorrow
		//
		////////////////////////////////////////////////////////////////

		/// @brief 明日の日付（ローカルタイム）を返します。 | Returns tomorrow's date (local time).
		/// @return 明日の日付（ローカルタイム） | Tomorrow's date (local time)
		[[nodiscard]]
		static Date Tomorrow() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	IsLeapYear
		//
		////////////////////////////////////////////////////////////////

		/// @brief うるう年かどうかを返します。 | Returns whether it is a leap year.
		/// @param year 年 | Year
		/// @return うるう年の場合 true, それ以外の場合は false | Returns true if it is a leap year, false otherwise
		[[nodiscard]]
		static constexpr bool IsLeapYear(int32 year) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	DaysInYear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 年の日数を返します。 | Returns the number of days in a year.
		/// @param year 年 | Year
		/// @return 1 年の日数 | Number of days in a year
		[[nodiscard]]
		static constexpr int32 DaysInYear(int32 year) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	DaysInMonth
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 か月の日数を返します。 | Returns the number of days in a month.
		/// @param year 年 | Year
		/// @param month 月 | Month
		/// @return 1 か月の日数 | Number of days in a month
		[[nodiscard]]
		static constexpr int32 DaysInMonth(int32 year, int32 month) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Subtract
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの日付の差を計算します。 | Calculates the difference between two dates.
		/// @param to 開始日 | Start date
		/// @param from 終了日 | End date
		/// @return `from` から `to` までの日数 | The number of days from `from` to `to`
		[[nodiscard]]
		static constexpr Days Subtract(const Date& to, const Date& from) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付をストリームに出力します。 | Outputs the date to a stream.
		/// @tparam CharType 文字型 | Character type
		/// @param output 出力先のストリーム | Output stream
		/// @param value 日付 | Date
		/// @return 出力ストリーム | Output stream
		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const Date& value)
		{
			return output << value.format();
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief 日付をフォーマットします。 | Formats the date.
		/// @param formatData フォーマットデータ | Format data
		/// @param value 日付 | Date
		friend void Formatter(FormatData& formatData, const Date& value);
	};

	////////////////////////////////////////////////////////////////
	//
	//	FormatDate
	//
	////////////////////////////////////////////////////////////////

	/// @brief 日付を文字列に変換します。 | Converts the date to a string.
	/// yyyy	4 桁の年 (0001-) | 4-digit year (0001-)
	/// yy		2 桁の年 (00-99) | 2-digit year (00-99)
	/// y		年 (1-) | Year (1-)
	/// MMMM	英語の月 (January-December) | English month (January-December)
	/// MMM		英語の月の略称 (Jan-Dec) | English month abbreviation (Jan-Dec)
	/// MM		2 桁の月 (01-12) | 2-digit month (01-12)
	/// M		1-2 桁の月 (1-12) | 1-2 digit month (1-12)
	/// dd		2 桁の日 (01-31) | 2-digit day (01-31)
	/// d		1-2 桁の日 (1-31) | 1-2 digit day (1-31)
	/// EEEE	英語の曜日 (Sunday-Saturday) | English day of the week (Sunday-Saturday)
	/// EEE		英語の曜日の略称 (Sun-Sat) | English day of the week abbreviation (Sun-Sat)
	/// @param date 日付 | Date
	/// @param format フォーマット指定 | Format specifier
	/// @return フォーマットされた日付 | Formatted date
	[[nodiscard]]
	String FormatDate(const Date& date, StringView format = U"yyyy-MM-dd"_sv);
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::Date>
{
	[[nodiscard]]
	size_t operator ()(const s3d::Date& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/Date.ipp"
