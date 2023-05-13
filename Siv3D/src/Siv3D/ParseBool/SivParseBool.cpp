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

# include <Siv3D/ParseBool.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Char.hpp>

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		static void ThrowParseBoolError(const std::string_view s, const ParseErrorReason reason)
		{
			if (reason == ParseErrorReason::EmptyInput)
			{
				throw ParseError{ "ParseBool(): Empty input" };
			}
			else
			{
				throw ParseError{ fmt::format("ParseBool(): Failed to parse `{}`", s) };
			}
		}

		[[noreturn]]
		static void ThrowParseBoolError(const StringView s, const ParseErrorReason reason)
		{
			ThrowParseBoolError(s.toUTF8(), reason);
		}

		template <class Char>
		[[nodiscard]]
		Expected<bool, ParseErrorReason> ParseBoolChecked(Char* start, Char* end) noexcept
		{
			while ((start < end) && IsSpace(start[0]))
			{
				++start;
			}

			while ((start < end) && IsSpace(end[-1]))
			{
				--end;
			}

			if (end <= start)
			{
				return Unexpected{ ParseErrorReason::EmptyInput };
			}

			if (auto length = (end - start);
				length == 4)
			{
				if ((start[0] == 'T' || start[0] == 't')
					&& (start[1] == 'R' || start[1] == 'r')
					&& (start[2] == 'U' || start[2] == 'u')
					&& (start[3] == 'E' || start[3] == 'e'))
				{
					return true;
				}
			}
			else if (length == 5)
			{
				if ((start[0] == 'F' || start[0] == 'f')
					&& (start[1] == 'A' || start[1] == 'a')
					&& (start[2] == 'L' || start[2] == 'l')
					&& (start[3] == 'S' || start[3] == 's')
					&& (start[4] == 'E' || start[4] == 'e'))
				{
					return false;
				}
			}

			return Unexpected{ ParseErrorReason::InvalidFormat };
		}
	}

	bool ParseBool(const std::string_view s)
	{
		if (const auto checked = ParseBoolChecked(s))
		{
			return *checked;
		}
		else
		{
			detail::ThrowParseBoolError(s, checked.error());
		}
	}

	bool ParseBool(const StringView s)
	{
		if (const auto checked = ParseBoolChecked(s))
		{
			return *checked;
		}
		else
		{
			detail::ThrowParseBoolError(s, checked.error());
		}
	}

	Optional<bool> ParseBoolOpt(const std::string_view s) noexcept
	{
		if (const auto checked = ParseBoolChecked(s))
		{
			return *checked;
		}
		else
		{
			return none;
		}
	}

	Optional<bool> ParseBoolOpt(const StringView s) noexcept
	{
		if (const auto checked = ParseBoolChecked(s))
		{
			return *checked;
		}
		else
		{
			return none;
		}
	}

	Expected<bool, ParseErrorReason> ParseBoolChecked(const std::string_view s) noexcept
	{
		const char8* start = s.data();
		const char8* end = (start + s.size());
		return detail::ParseBoolChecked(start, end);
	}

	Expected<bool, ParseErrorReason> ParseBoolChecked(const StringView s) noexcept
	{
		const char32* start = s.data();
		const char32* end = (start + s.size());
		return detail::ParseBoolChecked(start, end);
	}
}
