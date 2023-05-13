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

# include <Siv3D/ParseFloat.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Char.hpp>
# include <ThirdParty/double-conversion/double-conversion.h>

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		static void ThrowParseFloatError(const std::string_view s, const ParseErrorReason reason)
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
		static void ThrowParseFloatError(const StringView s, const ParseErrorReason reason)
		{
			ThrowParseFloatError(s.toUTF8(), reason);
		}

		template <class SV>
		[[nodiscard]]
		ParseErrorReason CheckParseFloat(const SV sv) noexcept
		{
			auto* start = sv.data();
			auto* end = (start + sv.size());

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
				return ParseErrorReason::EmptyInput;
			}

			return ParseErrorReason::InvalidFormat;
		}

		inline static constexpr double sNaN = std::numeric_limits<double>::signaling_NaN();

		inline static constexpr int flags =
			(double_conversion::StringToDoubleConverter::ALLOW_LEADING_SPACES
			| double_conversion::StringToDoubleConverter::ALLOW_TRAILING_SPACES
			| double_conversion::StringToDoubleConverter::ALLOW_SPACES_AFTER_SIGN
			| double_conversion::StringToDoubleConverter::ALLOW_CASE_INSENSIBILITY);

		template <class SV>
		[[nodiscard]]
		static double ParseDouble(const SV s) noexcept
		{
			const double_conversion::StringToDoubleConverter conv{ flags, sNaN, sNaN, "inf", "nan" };
			int unused;
			return conv.Siv3D_StringToIeee(s.data(), static_cast<int>(s.length()), true, &unused);
		}

		template <class SV>
		Expected<float, ParseErrorReason> ParseFloatChecked_impl(const SV s) noexcept
		{
			const double d = ParseDouble(s);

			if (std::bit_cast<uint64>(d) == std::bit_cast<uint64>(sNaN))
			{
				return Unexpected{ CheckParseFloat(s) };
			}

			return static_cast<float>(d);
		}

		template <class SV>
		Expected<double, ParseErrorReason> ParseDoubleChecked_impl(const SV s) noexcept
		{
			const double d = ParseDouble(s);

			if (std::bit_cast<uint64>(d) == std::bit_cast<uint64>(sNaN))
			{
				return Unexpected{ CheckParseFloat(s) };
			}

			return d;
		}
	}

	template <>
	float ParseFloat<float>(const std::string_view s)
	{
		if (const auto checked = ParseFloatChecked<float>(s))
		{
			return checked.value();
		}
		else
		{
			detail::ThrowParseFloatError(s, checked.error());
		}
	}

	template <>
	double ParseFloat<double>(const std::string_view s)
	{
		if (const auto checked = ParseFloatChecked<double>(s))
		{
			return checked.value();
		}
		else
		{
			detail::ThrowParseFloatError(s, checked.error());
		}
	}

	template <>
	float ParseFloat(const StringView s)
	{
		if (const auto checked = ParseFloatChecked<float>(s))
		{
			return checked.value();
		}
		else
		{
			detail::ThrowParseFloatError(s, checked.error());
		}
	}

	template <>
	double ParseFloat(const StringView s)
	{
		if (const auto checked = ParseFloatChecked<double>(s))
		{
			return checked.value();
		}
		else
		{
			detail::ThrowParseFloatError(s, checked.error());
		}
	}

	template <>
	Optional<float> ParseFloatOpt<float>(const std::string_view s) noexcept
	{
		if (const auto checked = ParseFloatChecked<float>(s))
		{
			return checked.value();
		}
		else
		{
			return none;
		}
	}

	template <>
	Optional<double> ParseFloatOpt<double>(const std::string_view s) noexcept
	{
		if (const auto checked = ParseFloatChecked<double>(s))
		{
			return checked.value();
		}
		else
		{
			return none;
		}
	}

	template <>
	Optional<float> ParseFloatOpt<float>(const StringView s) noexcept
	{
		if (const auto checked = ParseFloatChecked<float>(s))
		{
			return checked.value();
		}
		else
		{
			return none;
		}
	}

	template <>
	Optional<double> ParseFloatOpt<double>(const StringView s) noexcept
	{
		if (const auto checked = ParseFloatChecked<double>(s))
		{
			return checked.value();
		}
		else
		{
			return none;
		}
	}

	template <>
	Expected<float, ParseErrorReason> ParseFloatChecked<float>(const std::string_view s) noexcept
	{
		return detail::ParseFloatChecked_impl(s);
	}

	template <>
	Expected<float, ParseErrorReason> ParseFloatChecked<float>(const StringView s) noexcept
	{
		return detail::ParseFloatChecked_impl(s);
	}

	template <>
	Expected<double, ParseErrorReason> ParseFloatChecked<double>(const std::string_view s) noexcept
	{
		return detail::ParseDoubleChecked_impl(s);
	}

	template <>
	Expected<double, ParseErrorReason> ParseFloatChecked<double>(const StringView s) noexcept
	{
		return detail::ParseDoubleChecked_impl(s);
	}
}
