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

# include <Siv3D/ParseBool.hpp>
# include <Siv3D/Char.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error/ParseError.hpp>

namespace s3d
{
	namespace
	{
	# if SIV3D_BUILD(DEBUG)

		[[noreturn]]
		static void ThrowParseBoolError(const std::string_view s, const ParseErrorReason reason, const std::source_location& location)
		{
			if (reason == ParseErrorReason::EmptyInput)
			{
				throw ParseError{ "ParseBool(): Empty input", location };
			}
			else
			{
				throw ParseError{ fmt::format("ParseBool(): Failed to parse `{}`", s), location };
			}
		}

		[[noreturn]]
		static void ThrowParseBoolError(const StringView s, const ParseErrorReason reason, const std::source_location& location)
		{
			ThrowParseBoolError(Unicode::ToUTF8(s), reason, location);
		}

	# else

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
			ThrowParseBoolError(Unicode::ToUTF8(s), reason);
		}

	# endif

		template <class Char>
		[[nodiscard]]
		Result<bool, ParseErrorReason> ParseBoolWithReason(Char* start, Char* end) noexcept
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
				length == 4)
			{
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
	//	ParseBool
	//
	////////////////////////////////////////////////////////////////

	bool ParseBool(const std::string_view s, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto reason = ParseBoolWithReason(s))
		{
			return *reason;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			ThrowParseBoolError(s, reason.error(), location);
		# else
			ThrowParseBoolError(s, reason.error());
		# endif
		}
	}

	bool ParseBool(const StringView s, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto reason = ParseBoolWithReason(s))
		{
			return *reason;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			ThrowParseBoolError(s, reason.error(), location);
		# else
			ThrowParseBoolError(s, reason.error());
		# endif
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolOpt
	//
	////////////////////////////////////////////////////////////////

	Optional<bool> ParseBoolOpt(const std::string_view s) noexcept
	{
		if (const auto reason = ParseBoolWithReason(s))
		{
			return *reason;
		}
		else
		{
			return none;
		}
	}

	Optional<bool> ParseBoolOpt(const StringView s) noexcept
	{
		if (const auto reason = ParseBoolWithReason(s))
		{
			return *reason;
		}
		else
		{
			return none;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseBoolWithReason
	//
	////////////////////////////////////////////////////////////////

	Result<bool, ParseErrorReason> ParseBoolWithReason(const std::string_view s) noexcept
	{
		const char8* start = s.data();
		const char8* end = (start + s.size());
		return ParseBoolWithReason(start, end);
	}

	Result<bool, ParseErrorReason> ParseBoolWithReason(const StringView s) noexcept
	{
		const char32* start = s.data();
		const char32* end = (start + s.size());
		return ParseBoolWithReason(start, end);
	}
}
