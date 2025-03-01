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

namespace s3d
{
	namespace detail
	{
		[[nodiscard]]
		constexpr int32 TimeToMillisecCount(const int32 hour, const int32 minute, const int32 second, const int32 milliseconds) noexcept
		{
			return (hour * (60 * 60 * 1000)
				+ minute * (60 * 1000)
				+ second * (1000)
				+ milliseconds);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr DateTime::DateTime(
		const int32 _year,
		const int32 _month,
		const int32 _day,
		const int32 _hour,
		const int32 _minute,
		const int32 _second,
		const int32 _milliseconds) noexcept
		: year{ _year }
		, month{ _month }
		, day{ _day }
		, hour{ _hour }
		, minute{ _minute }
		, second{ _second }
		, milliseconds{ _milliseconds } {}

	constexpr DateTime::DateTime(
		const Date& date,
		const int32 _hour,
		const int32 _minute,
		const int32 _second,
		const int32 _milliseconds) noexcept
		: year{ date.year }
		, month{ date.month }
		, day{ date.day }
		, hour{ _hour }
		, minute{ _minute }
		, second{ _second }
		, milliseconds{ _milliseconds } {}

	////////////////////////////////////////////////////////////////
	//
	//	isValid
	//
	////////////////////////////////////////////////////////////////

	constexpr bool DateTime::isValid() const noexcept
	{
		return (date().isValid()
			&& InRange(hour, 0, 23)
			&& InRange(minute, 0, 59)
			&& InRange(second, 0, 59)
			&& InRange(milliseconds, 0, 999));
	}

	////////////////////////////////////////////////////////////////
	//
	//	date
	//
	////////////////////////////////////////////////////////////////

	constexpr Date DateTime::date() const noexcept
	{
		return{ year, month, day };
	}

	////////////////////////////////////////////////////////////////
	//
	//	dayOfWeek
	//
	////////////////////////////////////////////////////////////////

	constexpr DayOfWeek DateTime::dayOfWeek() const noexcept
	{
		return date().dayOfWeek();
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	inline uint64 DateTime::hash() const noexcept
	{
		return Hash(*this);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Subtract
	//
	////////////////////////////////////////////////////////////////

	constexpr Duration DateTime::Subtract(const DateTime& a, const DateTime& b) noexcept
	{
		const Days diffDays = (a.date() - b.date());
		const int64 aMilliSec = detail::TimeToMillisecCount(a.hour, a.minute, a.second, a.milliseconds);
		const int64 bMilliSec = detail::TimeToMillisecCount(b.hour, b.minute, b.second, b.milliseconds);
		return Duration{ (static_cast<int64>(diffDays.count()) * (86400 * 1000) + (aMilliSec - bMilliSec)) / 1000.0 };
	}
}
