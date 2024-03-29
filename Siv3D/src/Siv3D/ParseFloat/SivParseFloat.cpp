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

# include <ThirdParty/fast_float/fast_float.h>
# include <Siv3D/ParseFloat.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error/ParseError.hpp>

namespace s3d
{
	namespace
	{
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
	float ParseFloat<float>(const std::string_view s)
	{
		if (const auto result = ParseFloatWithReason<float>(s))
		{
			return *result;
		}
		else
		{
			ThrowParseFloatError(s, result.error());
		}
	}

	template <>
	double ParseFloat<double>(const std::string_view s)
	{
		if (const auto result = ParseFloatWithReason<double>(s))
		{
			return *result;
		}
		else
		{
			ThrowParseFloatError(s, result.error());
		}
	}

	template <>
	float ParseFloat(const StringView s)
	{
		if (const auto result = ParseFloatWithReason<float>(s))
		{
			return *result;
		}
		else
		{
			ThrowParseFloatError(s, result.error());
		}
	}

	template <>
	double ParseFloat(const StringView s)
	{
		if (const auto result = ParseFloatWithReason<double>(s))
		{
			return *result;
		}
		else
		{
			ThrowParseFloatError(s, result.error());
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
