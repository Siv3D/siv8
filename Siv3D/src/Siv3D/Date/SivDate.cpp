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

# include <Siv3D/Date.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	String Date::format(const StringView format) const
	{
		return FormatDate(*this, format);
	}

	Date Date::operator +(const Days& days) const noexcept
	{
		return detail::EpochDaysToDate(detail::DateToEpochDays(*this) + days.count());
	}

	Date Date::operator -(const Days& days) const noexcept
	{
		return (*this + (-days));
	}

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

	Date& Date::operator +=(const Days& days) noexcept
	{
		return (*this = detail::EpochDaysToDate(detail::DateToEpochDays(*this) + days.count()));
	}

	Date& Date::operator -=(const Days& days) noexcept
	{
		return (*this += (-days));
	}

	Date Date::Yesterday() noexcept
	{
		return (Today() - Days{ 1 });
	}

	Date Date::Tomorrow() noexcept
	{
		return (Today() + Days{ 1 });
	}

	void Formatter(FormatData& formatData, const Date& value)
	{
		formatData.string.append(value.format());
	}
}
