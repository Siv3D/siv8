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

namespace s3d
{
	namespace detail
	{
		constexpr uint8 GetDayOfWeek(const int32 year, const int32 month, const int32 day) noexcept
		{
			return static_cast<uint8>(((month == 1 || month == 2) ?
				((year - 1) + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400 + (13 * (month + 12) + 8) / 5 + day) % 7
				: (year + year / 4 - year / 100 + year / 400 + (13 * month + 8) / 5 + day) % 7));
		}

		constexpr int32 YearsToDays(const int32 years)
		{
			return (365 * years + years / 4 - years / 100 + years / 400);
		}

		inline constexpr int16 DaysAtEndOfMonth[2][12]
		{
			{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }, // normal year
			{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }  // leap year
		};

		constexpr int32 DateToEpochDays(const Date& date)
		{
			return (YearsToDays(date.year - 1)
				+ DaysAtEndOfMonth[Date::IsLeapYear(date.year)][date.month - 1]
				+ (date.day - 1));
		}

		constexpr int32 DaysToYears(const int32 days)
		{
			constexpr int32 DaysIn1Year = 365;
			constexpr int32 DaysIn4Years = (DaysIn1Year * 4 + 1);
			constexpr int32 DaysIn100Years = (DaysIn4Years * 25 - 1);
			constexpr int32 DaysIn400Years = (DaysIn100Years * 4 + 1);
			const int32 days400YearsPeriod = Min(days % DaysIn400Years, DaysIn100Years * 4 - 1);
			const int32 days100YearsPeriod = days400YearsPeriod % DaysIn100Years;
			const int32 days4YearsPeriod = Min(days100YearsPeriod % DaysIn4Years, DaysIn1Year * 4 - 1);
			return ((days / DaysIn400Years * 400)
				+ (days400YearsPeriod / DaysIn100Years * 100)
				+ (days100YearsPeriod / DaysIn4Years * 4)
				+ (days4YearsPeriod / DaysIn1Year));
		}

		inline Date EpochDaysToDate(const int32 epochDays)
		{
			const int32 years = (DaysToYears(epochDays) + 1);
			const bool isLeapYear = Date::IsLeapYear(years);
			const int32 daysAYearPeriod = (epochDays - YearsToDays(years - 1));
			const auto january = std::begin(DaysAtEndOfMonth[isLeapYear]);
			const int32 months = static_cast<int32>(std::distance(january, std::upper_bound(january, january + 12, daysAYearPeriod)));
			const int32 days = daysAYearPeriod - (DaysAtEndOfMonth[isLeapYear][(months - 1)]) + 1;
			return{ years, months, days };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Date::Date(const int32 _year, const int32 _month, const int32 _day) noexcept
		: year{ _year }
		, month{ _month }
		, day{ _day } {}

	////////////////////////////////////////////////////////////////
	//
	//	dayOfWeek
	//
	////////////////////////////////////////////////////////////////

	constexpr DayOfWeek Date::dayOfWeek() const noexcept
	{
		return DayOfWeek{ detail::GetDayOfWeek(year, month, day) };
	}

	////////////////////////////////////////////////////////////////
	//
	//	isToday
	//
	////////////////////////////////////////////////////////////////

	inline bool Date::isToday() const noexcept
	{
		return (*this == Date::Today());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isLeapYear
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Date::isLeapYear() const noexcept
	{
		return IsLeapYear(year);
	}

	////////////////////////////////////////////////////////////////
	//
	//	daysInYear
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Date::daysInYear() const noexcept
	{
		return DaysInYear(year);
	}

	////////////////////////////////////////////////////////////////
	//
	//	daysInMonth
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Date::daysInMonth() const noexcept
	{
		return DaysInMonth(year, month);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isValid
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Date::isValid() const noexcept
	{
		return (InRange(month, 1, 12)
			&& InRange(day, 1, daysInMonth()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 Date::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsLeapYear
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Date::IsLeapYear(const int32 year) noexcept
	{
		return (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0));
	}

	////////////////////////////////////////////////////////////////
	//
	//	DaysInYear
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Date::DaysInYear(const int32 year) noexcept
	{
		return (IsLeapYear(year) ? 366 : 365);
	}

	////////////////////////////////////////////////////////////////
	//
	//	DaysInMonth
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 Date::DaysInMonth(const int32 year, const int32 month) noexcept
	{
		if ((month < 1) || (12 < month))
		{
			return 0;
		}

		int32 days = (28 + ((62648012u >> (month * 2)) & 3));

		if ((month == 2) && IsLeapYear(year))
		{
			++days;
		}

		return days;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Subtract
	//
	////////////////////////////////////////////////////////////////

	constexpr Days Date::Subtract(const Date& to, const Date& from) noexcept
	{
		return Days{ detail::DateToEpochDays(to) - detail::DateToEpochDays(from) };
	}
}
