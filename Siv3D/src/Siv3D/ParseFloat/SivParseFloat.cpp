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

# include <ThirdParty/fast_float/fast_float.h>
# include <Siv3D/ParseFloat.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error/ParseError.hpp>

namespace s3d
{
	namespace
	{
	# if SIV3D_BUILD(DEBUG)

		[[noreturn]]
		static void ThrowParseFloatError(const std::string_view s, const ParseErrorReason reason, const std::source_location& location)
		{
			if (reason == ParseErrorReason::EmptyInput)
			{
				throw ParseError{ "ParseFloat(): Empty input", location };
			}
			else
			{
				throw ParseError{ fmt::format("ParseFloat(): Failed to parse `{}`", s), location };
			}
		}
		
		[[noreturn]]
		static void ThrowParseFloatError(const StringView s, const ParseErrorReason reason, const std::source_location& location)
		{
			ThrowParseFloatError(Unicode::ToUTF8(s), reason, location);
		}

	# else

		[[noreturn]]
		static void ThrowParseFloatError(const std::string_view s, const ParseErrorReason reason)
		{
			if (reason == ParseErrorReason::EmptyInput)
			{
				throw ParseError{ "ParseFloat(): Empty input" };
			}
			else
			{
				throw ParseError{ fmt::format("ParseFloat(): Failed to parse `{}`", s) };
			}
		}

		[[noreturn]]
		static void ThrowParseFloatError(const StringView s, const ParseErrorReason reason)
		{
			ThrowParseFloatError(Unicode::ToUTF8(s), reason);
		}

	# endif

		template <Concept::FloatingPoint Float, class SV>
		Result<Float, ParseErrorReason> ParseFloatWithReason_impl(const SV s) noexcept
		{
			Float value;
			auto [p, ec] = fast_float::from_chars(s.data(), (s.data() + s.size()), value);

			if (ec == std::errc{})
			{
				return value;
			}
			else if (ec == std::errc::result_out_of_range)
			{
				return Err{ ParseErrorReason::NumericOverflow };
			}
			else if (s.empty())
			{
				return Err{ ParseErrorReason::EmptyInput };
			}
			else
			{
				return Err{ ParseErrorReason::InvalidFormat };
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloat
	//
	////////////////////////////////////////////////////////////////

	template <>
	float ParseFloat<float>(const std::string_view s, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto result = ParseFloatWithReason<float>(s))
		{
			return *result;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			ThrowParseFloatError(s, result.error(), location);
		# else
			ThrowParseFloatError(s, result.error());
		# endif
		}
	}

	template <>
	double ParseFloat<double>(const std::string_view s, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto result = ParseFloatWithReason<double>(s))
		{
			return *result;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			ThrowParseFloatError(s, result.error(), location);
		# else
			ThrowParseFloatError(s, result.error());
		# endif
		}
	}

	template <>
	float ParseFloat(const StringView s, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto result = ParseFloatWithReason<float>(s))
		{
			return *result;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			ThrowParseFloatError(s, result.error(), location);
		# else
			ThrowParseFloatError(s, result.error());
		# endif
		}
	}

	template <>
	double ParseFloat(const StringView s, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto result = ParseFloatWithReason<double>(s))
		{
			return *result;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			ThrowParseFloatError(s, result.error(), location);
		# else
			ThrowParseFloatError(s, result.error());
		# endif
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloatOpt
	//
	////////////////////////////////////////////////////////////////

	template <>
	Optional<float> ParseFloatOpt<float>(const std::string_view s) noexcept
	{
		if (const auto result = ParseFloatWithReason<float>(s))
		{
			return *result;
		}
		else
		{
			return none;
		}
	}

	template <>
	Optional<double> ParseFloatOpt<double>(const std::string_view s) noexcept
	{
		if (const auto result = ParseFloatWithReason<double>(s))
		{
			return *result;
		}
		else
		{
			return none;
		}
	}

	template <>
	Optional<float> ParseFloatOpt<float>(const StringView s) noexcept
	{
		if (const auto result = ParseFloatWithReason<float>(s))
		{
			return *result;
		}
		else
		{
			return none;
		}
	}

	template <>
	Optional<double> ParseFloatOpt<double>(const StringView s) noexcept
	{
		if (const auto result = ParseFloatWithReason<double>(s))
		{
			return *result;
		}
		else
		{
			return none;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseFloatWithReason
	//
	////////////////////////////////////////////////////////////////

	template <>
	Result<float, ParseErrorReason> ParseFloatWithReason<float>(const std::string_view s) noexcept
	{
		return ParseFloatWithReason_impl<float>(s);
	}

	template <>
	Result<float, ParseErrorReason> ParseFloatWithReason<float>(const StringView s) noexcept
	{
		return ParseFloatWithReason_impl<float>(s);
	}

	template <>
	Result<double, ParseErrorReason> ParseFloatWithReason<double>(const std::string_view s) noexcept
	{
		return ParseFloatWithReason_impl<double>(s);
	}

	template <>
	Result<double, ParseErrorReason> ParseFloatWithReason<double>(const StringView s) noexcept
	{
		return ParseFloatWithReason_impl<double>(s);
	}
}
