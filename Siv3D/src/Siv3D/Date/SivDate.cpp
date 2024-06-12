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

# include <Siv3D/Date.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{		
	namespace
	{
		static Date EpochDaysToDate(const int32 epochDays)
		{
			const int32 years = (detail::DaysToYears(epochDays) + 1);
			const bool isLeapYear = Date::IsLeapYear(years);
			const int32 daysAYearPeriod = (epochDays - detail::YearsToDays(years - 1));
			const auto january = std::begin(detail::DaysAtEndOfMonth[isLeapYear]);
			const int32 months = static_cast<int32>(std::distance(january, std::upper_bound(january, january + 12, daysAYearPeriod)));
			const int32 days = daysAYearPeriod - (detail::DaysAtEndOfMonth[isLeapYear][(months - 1)]) + 1;
			return{ years, months, days };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String Date::format(const StringView format) const
	{
		return FormatDate(*this, format);
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	Date Date::operator +(const Days& days) const noexcept
	{
		return EpochDaysToDate(detail::DateToEpochDays(*this) + days.count());
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	Date Date::operator -(const Days& days) const noexcept
	{
		return (*this + (-days));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator ++
	//
	////////////////////////////////////////////////////////////////

	Date& Date::operator ++() noexcept
	{
		return (*this += Days{ 1 });
	}

	Date Date::operator ++(int) noexcept
	{
		const Date result = *this;
		++(*this);
		return result;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator --
	//
	////////////////////////////////////////////////////////////////

	Date& Date::operator --() noexcept
	{
		return (*this -= Days{ 1 });
	}

	Date Date::operator --(int) noexcept
	{
		const Date result = *this;
		--(*this);
		return result;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	Date& Date::operator +=(const Days& days) noexcept
	{
		return (*this = EpochDaysToDate(detail::DateToEpochDays(*this) + days.count()));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	Date& Date::operator -=(const Days& days) noexcept
	{
		return (*this += (-days));
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Yesterday
	//
	////////////////////////////////////////////////////////////////

	Date Date::Yesterday() noexcept
	{
		return (Today() - Days{ 1 });
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Tomorrow
	//
	////////////////////////////////////////////////////////////////

	Date Date::Tomorrow() noexcept
	{
		return (Today() + Days{ 1 });
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const Date& value)
	{
		formatData.string.append(value.format());
	}
}
