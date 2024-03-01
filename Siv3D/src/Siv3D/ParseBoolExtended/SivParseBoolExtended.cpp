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

# include <Siv3D/ParseBoolExtended.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error/ParseError.hpp>

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		static void ThrowParseBoolExtendedError(const std::string_view s, const ParseErrorReason reason)
		{
			if (reason == ParseErrorReason::EmptyInput)
			{
				throw ParseError{ "ParseBoolExtended(): Empty input" };
			}
			else
			{
				throw ParseError{ fmt::format("ParseBoolExtended(): Failed to parse `{}`", s) };
			}
		}

		[[noreturn]]
		static void ThrowParseBoolExtendedError(const StringView s, const ParseErrorReason reason)
		{
			ThrowParseBoolExtendedError(Unicode::ToUTF8(s), reason);
		}

		template <class Char>
		[[nodiscard]]
		Result<bool, ParseErrorReason> ParseBoolExtendedWithReason(Char* start, Char* end) noexcept
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
				return Err{ ParseErrorReason::EmptyInput };
			}

			if (auto length = (end - start);
				length == 1)
			{
				// 0
				if (start[0] == '0')
				{
					return false;
				}

				// 1
				if (start[0] == '1')
				{
					return true;
				}
			}
			else if (length == 2)
			{
				// No
				if (((start[0] == 'N') || (start[0] == 'n'))
					&& ((start[1] == 'O') || (start[1] == 'o')))
				{
					return false;
				}

				// On
				if (((start[0] == 'O') || (start[0] == 'o'))
					&& ((start[1] == 'N') || (start[1] == 'n')))
				{
					return true;
				}
			}
			else if (length == 3)
			{
				// Yes
				if (((start[0] == 'Y') || (start[0] == 'y'))
					&& ((start[1] == 'E') || (start[1] == 'e'))
					&& ((start[2] == 'S') || (start[2] == 's')))
				{
					return true;
				}

				// Off
				if (((start[0] == 'O') || (start[0] == 'o'))
					&& ((start[1] == 'F') || (start[1] == 'f'))
					&& ((start[2] == 'F') || (start[2] == 'f')))
				{
					return false;
				}
			}
			else if (length == 4)
			{
				// True
				if (((start[0] == 'T') || (start[0] == 't'))
					&& ((start[1] == 'R') || (start[1] == 'r'))
					&& ((start[2] == 'U') || (start[2] == 'u'))
					&& ((start[3] == 'E') || (start[3] == 'e')))
				{
					return true;
				}
			}
			else if (length == 5)
			{
				// False
				if (((start[0] == 'F') || (start[0] == 'f'))
					&& ((start[1] == 'A') || (start[1] == 'a'))
					&& ((start[2] == 'L') || (start[2] == 'l'))
					&& ((start[3] == 'S') || (start[3] == 's'))
					&& ((start[4] == 'E') || (start[4] == 'e')))
				{
					return false;
				}
			}

			return Err{ ParseErrorReason::InvalidFormat };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolExtended
	//
	////////////////////////////////////////////////////////////////

	bool ParseBoolExtended(const std::string_view s)
	{
		if (const auto checked = ParseBoolExtendedWithReason(s))
		{
			return *checked;
		}
		else
		{
			detail::ThrowParseBoolExtendedError(s, checked.error());
		}
	}

	bool ParseBoolExtended(const StringView s)
	{
		if (const auto checked = ParseBoolExtendedWithReason(s))
		{
			return *checked;
		}
		else
		{
			detail::ThrowParseBoolExtendedError(s, checked.error());
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolExtendedOpt
	//
	////////////////////////////////////////////////////////////////

	Optional<bool> ParseBoolExtendedOpt(const std::string_view s) noexcept
	{
		if (const auto checked = ParseBoolExtendedWithReason(s))
		{
			return *checked;
		}
		else
		{
			return none;
		}
	}

	Optional<bool> ParseBoolExtendedOpt(const StringView s) noexcept
	{
		if (const auto checked = ParseBoolExtendedWithReason(s))
		{
			return *checked;
		}
		else
		{
			return none;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolExtendedWithReason
	//
	////////////////////////////////////////////////////////////////

	Result<bool, ParseErrorReason> ParseBoolExtendedWithReason(const std::string_view s) noexcept
	{
		const char8* start = s.data();
		const char8* end = (start + s.size());
		return detail::ParseBoolExtendedWithReason(start, end);
	}

	Result<bool, ParseErrorReason> ParseBoolExtendedWithReason(const StringView s) noexcept
	{
		const char32* start = s.data();
		const char32* end = (start + s.size());
		return detail::ParseBoolExtendedWithReason(start, end);
	}
}
