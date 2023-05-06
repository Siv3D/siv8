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

# include <Siv3D/DateTime.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/IntToString.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/FormatUtility.hpp>

namespace s3d
{
	namespace detail
	{
		static constexpr StringView MonthEnglish[12]
		{
			U"January"_sv,
			U"February"_sv,
			U"March"_sv,
			U"April"_sv,
			U"May"_sv,
			U"June"_sv,
			U"July"_sv,
			U"August"_sv,
			U"September"_sv,
			U"October"_sv,
			U"November"_sv,
			U"December"_sv,
		};

		static constexpr StringView DayOfWeekEnglish[8]
		{
			U"Sunday"_sv,
			U"Monday"_sv,
			U"Tuesday"_sv,
			U"Wednesday"_sv,
			U"Thursday"_sv,
			U"Friday"_sv,
			U"Saturday"_sv,
			U"null"_sv,
		};

		static constexpr StringView AMPM[2]
		{
			U"AM"_sv,
			U"PM"_sv,
		};

		static constexpr StringView FormatPatterns[22]
		{
			U"yyyy"_sv,
			U"yy"_sv,
			U"y"_sv,
			U"MMMM"_sv,
			U"MMM"_sv,
			U"MM"_sv,
			U"M"_sv,
			U"dd"_sv,
			U"d"_sv,
			U"EEEE"_sv,
			U"EEE"_sv,
			U"a"_sv,
			U"HH"_sv,
			U"H"_sv,
			U"hh"_sv,
			U"h"_sv,
			U"mm"_sv,
			U"ss"_sv,
			U"S"_sv,
			U"SS"_sv,
			U"SSS"_sv,
		};

		static void AppendElement(String& result, const DateTime& dt, const StringView format, const bool skipTime)
		{
			constexpr size_t timeStartIndex = 11;
			const size_t formatIndex = std::distance(
				std::begin(FormatPatterns),
				std::find(std::begin(FormatPatterns),
					skipTime ? (std::begin(FormatPatterns) + timeStartIndex) : std::end(FormatPatterns),
					format)
			);

			if (skipTime && (timeStartIndex <= formatIndex))
			{
				return;
			}

			switch (formatIndex)
			{
			case 0:	// yyyy	4 桁の年 (0001-)
				result += Pad(dt.year, { 4, U'0' });
				break;
			case 1:	// yy	2 桁の年 (00-99)
				result += Pad(dt.year % 100, { 2, U'0' });
				break;
			case 2:	// y	年 (1-)
				result += ToString(dt.year);
				break;
			case 3:	// MMMM	英語の月 (January-December)
				result += MonthEnglish[(dt.month - 1)];
				break;
			case 4:	// MMM	英語の月の略称 (Jan-Dec)
				result += MonthEnglish[(dt.month - 1)].substr(0, 3);
				break;
			case 5:	// MM	2 桁の月 (01-12)
				result += Pad(dt.month, { 2, U'0' });
				break;
			case 6:	// M	1-2 桁の月 (1-12)
				result += ToString(dt.month);
				break;
			case 7:	// dd	2 桁の日 (01-31)
				result += Pad(dt.day, { 2, U'0' });
				break;
			case 8:	// d	1-2 桁の日 (1-31)
				result += ToString(dt.day);
				break;
			case 9:	// EEEE 英語の曜日 (Sunday-Saturday)
				result += DayOfWeekEnglish[FromEnum(dt.dayOfWeek())];
				break;
			case 10: // EEE	英語の曜日の略称 (Sun-Sat)
				result += DayOfWeekEnglish[FromEnum(dt.dayOfWeek())].substr(0, 3);
				break;
			case 11: // a	午前/午後 (AM/PM)
				result += AMPM[dt.hour > 11];
				break;
			case 12: // HH	24 時間表記の 2 桁の時 (00-23)
				result += Pad(dt.hour, { 2, U'0' });
				break;
			case 13: // H	24 時間表記の時 (0-23)
				result += ToString(dt.hour);
				break;
			case 14: // hh	12 時間表記の 2 桁の時 (00-11)
				result += Pad(dt.hour % 12, { 2, U'0' });
				break;
			case 15: // h	12 時間表記の時 (0-11)
				result += ToString(dt.hour % 12);
				break;
			case 16: // mm	2 桁の分 (00-59)
				result += Pad(dt.minute, { 2, U'0' });
				break;
			case 17: // ss	2 桁の秒 (00-59)
				result += Pad(dt.second, { 2, U'0' });
				break;
			case 18: // S	小数点以下 1 桁の秒 (0-9)
				result += ToString(dt.milliseconds / 100);
				break;
			case 19: // SS	小数点以下 2 桁の秒 (00-99)
				result += Pad(dt.milliseconds / 10, { 2, U'0' });
				break;
			case 20: // SSS	小数点以下 3 桁の秒 (000-999)
				result += Pad(dt.milliseconds, { 3, U'0' });
				break;
			}
		}

		[[nodiscard]]
		static String FormatDateTime(const DateTime& dateTime, const StringView format, const bool skipTime)
		{
			String formattedDateTime, keyPattern;

			bool inQuot = false;

			char32 previousChar = U'\0';

			for (size_t i = 0; i < format.length(); ++i)
			{
				const char32 ch = format[i];

				if (IsAlpha(ch))
				{
					if (inQuot)
					{
						formattedDateTime << ch;
					}
					else
					{
						if (keyPattern.isEmpty() || ch == previousChar)
						{
							keyPattern.push_back(ch);
						}
						else
						{
							detail::AppendElement(formattedDateTime, dateTime, keyPattern, skipTime);
							keyPattern.clear();
							keyPattern.push_back(ch);
						}
					}
				}
				else
				{
					if (keyPattern)
					{
						detail::AppendElement(formattedDateTime, dateTime, keyPattern, skipTime);
						keyPattern.clear();
					}

					if (ch == U'\'')
					{
						if (((i + 1) < format.length()) &&
							(format[i + 1] == U'\''))
						{
							formattedDateTime << U'\'';
							++i;
							continue;
						}

						inQuot = (not inQuot);
					}
					else
					{
						formattedDateTime << ch;
					}
				}

				previousChar = ch;
			}

			if (keyPattern)
			{
				detail::AppendElement(formattedDateTime, dateTime, keyPattern, skipTime);
			}

			return formattedDateTime;
		}
	}

	String FormatDate(const Date& date, const StringView format)
	{
		return detail::FormatDateTime(date, format, true);
	}

	String DateTime::format(const StringView format) const
	{
		return FormatDateTime(*this, format);
	}

	DateTime DateTime::operator +(const Days& days) const noexcept
	{
		return (DateTime{ *this } += days);
	}

	DateTime DateTime::operator +(const Milliseconds& _milliseconds) const noexcept
	{
		return (DateTime{ *this } += _milliseconds);
	}

	DateTime DateTime::operator -(const Days& days) const noexcept
	{
		return (DateTime{ *this } -= days);
	}

	DateTime DateTime::operator -(const Milliseconds& _milliseconds) const noexcept
	{
		return (DateTime{ *this } -= _milliseconds);
	}

	DateTime& DateTime::operator +=(const Days& days) noexcept
	{
		Date d = (date() + days);
		year = d.year;
		month = d.month;
		day = d.day;
		return *this;
	}

	DateTime& DateTime::operator +=(const Milliseconds& _milliseconds) noexcept
	{
		constexpr int64 millisecIn1Day = (86400 * 1000);

		int64 count = _milliseconds.count();

		if (const int64 days = (count / millisecIn1Day))
		{
			(operator += (Days{ days }));
			count -= (days * millisecIn1Day);
		}

		int64 newCount = (detail::TimeToMillisecCount(hour, minute, second, milliseconds) + count);

		if (millisecIn1Day <= newCount)
		{
			(operator += (Days{ 1 }));
			newCount -= millisecIn1Day;
		}
		else if (newCount < 0)
		{
			(operator += (Days{ -1 }));
			newCount += millisecIn1Day;
		}

		hour = static_cast<int32>(newCount / (60 * 60 * 1000));
		minute = static_cast<int32>(newCount / (60 * 1000) % 60);
		second = static_cast<int32>(newCount / (1000) % 60);
		milliseconds = static_cast<int32>(newCount % 1000);
		return *this;
	}

	DateTime& DateTime::operator -=(const Days& days) noexcept
	{
		return (operator += (-days));
	}

	DateTime& DateTime::operator -=(const Milliseconds& _milliseconds) noexcept
	{
		return (operator += (-_milliseconds));
	}

	DateTime DateTime::Yesterday() noexcept
	{
		return Date::Yesterday();
	}

	DateTime DateTime::Today() noexcept
	{
		return Date::Today();
	}

	DateTime DateTime::Tomorrow() noexcept
	{
		return Date::Tomorrow();
	}

	void Formatter(FormatData& formatData, const DateTime& value)
	{
		formatData.string.append(value.format());
	}

	String FormatDateTime(const DateTime& dateTime, const StringView format)
	{
		return detail::FormatDateTime(dateTime, format, false);
	}
}
