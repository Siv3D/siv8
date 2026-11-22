//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <bit>
# include <cfenv>
# include <charconv>
# include <cmath>
# include <limits>
# include <random>

namespace
{
	template <class Float, class Text>
	void CheckParsed(const Text text, const Float expected)
	{
		const auto result = ParseFloatResult<Float>(text);
		const auto optional = ParseFloatOpt<Float>(text);
		REQUIRE(result);
		REQUIRE(optional);
		for (const Float actual : { *result, *optional, ParseFloat<Float>(text) })
		{
			if (std::isnan(expected))
			{
				CHECK(std::isnan(actual));
			}
			else
			{
				using Bits = std::conditional_t<std::is_same_v<Float, float>, uint32, uint64>;
				CHECK(std::bit_cast<Bits>(actual) == std::bit_cast<Bits>(expected));
			}
		}
	}

	template <class Float>
	void CheckBothEncodings(const std::string_view text, const Float expected)
	{
		CAPTURE(text, sizeof(Float));
		CheckParsed(text, expected);
		const String wide = Unicode::FromUTF8(text);
		CheckParsed(StringView{ wide }, expected);
	}

	template <class Float, class Text>
	void CheckRejected(const Text text, const ParseErrorReason reason)
	{
		const auto result = ParseFloatResult<Float>(text);
		REQUIRE_FALSE(result);
		CHECK(result.error() == reason);
		CHECK_FALSE(ParseFloatOpt<Float>(text));
		CHECK_THROWS_AS(ParseFloat<Float>(text), ParseError);
	}

	template <class Float>
	void CheckFormsAndErrors()
	{
		for (const std::string_view text : { "12.5", "1.25e1", "125E-1", "00012.5" })
		{
			CheckBothEncodings(text, Float{ 12.5 });
		}
		CheckBothEncodings(".5", Float{ 0.5 });
		CheckBothEncodings("1.", Float{ 1 });
		CheckBothEncodings("-0", -Float{ 0 });
		CheckBothEncodings("1234567890123456.25", static_cast<Float>(1234567890123456.25));
		CheckBothEncodings("iNf", std::numeric_limits<Float>::infinity());
		CheckBothEncodings("-INFINITY", -std::numeric_limits<Float>::infinity());
		CheckBothEncodings("NaN", std::numeric_limits<Float>::quiet_NaN());
		for (const std::string_view text : { "", "invalid", "+1", " 1", ".", "1e10000", "-1e-10000" })
		{
			CAPTURE(text, sizeof(Float));
			const auto reason = text.empty() ? ParseErrorReason::EmptyInput
				: ((text == "1e10000" || text == "-1e-10000")
					? ParseErrorReason::NumericOverflow : ParseErrorReason::InvalidFormat);
			CheckRejected<Float>(text, reason);
			const String wide = Unicode::FromUTF8(text);
			CheckRejected<Float>(StringView{ wide }, reason);
		}
		const char32 nonDigit[] = { char32{ 0xFFFFFFFFu }, U'1' };
		CheckRejected<Float>(StringView{ nonDigit, 2 }, ParseErrorReason::InvalidFormat);
	}

	template <class Float>
	void CheckRoundTrips()
	{
		const auto check = [](const Float value)
		{
			char buffer[64];
			const auto [end, error] = std::to_chars(std::begin(buffer), std::end(buffer), value);
			REQUIRE(error == std::errc{});
			CheckBothEncodings(std::string_view{ buffer, static_cast<size_t>(end - buffer) }, value);
		};
		for (const Float value : { Float{ 0 }, -Float{ 0 }, std::numeric_limits<Float>::denorm_min(),
			std::numeric_limits<Float>::min(), std::nextafter(std::numeric_limits<Float>::min(), Float{ 0 }),
			std::numeric_limits<Float>::max() })
		{
			check(value);
			check(-value);
		}
		using Bits = std::conditional_t<std::is_same_v<Float, float>, uint32, uint64>;
		std::mt19937_64 random{ 0xFA57F10A7 };
		for (size_t i = 0; i < 256; ++i)
		{
			const Float value = std::bit_cast<Float>(static_cast<Bits>(random()));
			if (std::isfinite(value))
			{
				check(value);
			}
		}
	}
}

TEST_CASE("ParseFloat.forms_and_errors")
{
	CheckFormsAndErrors<float>();
	CheckFormsAndErrors<double>();
}

TEST_CASE("ParseFloat.boundary_and_random_round_trips")
{
	CheckRoundTrips<float>();
	CheckRoundTrips<double>();
}

TEST_CASE("ParseFloat.rounding_modes")
{
	std::fenv_t environment;
	REQUIRE(std::fegetenv(&environment) == 0);
	const ScopeExit restore{ [&] { std::fesetenv(&environment); } };
	for (const int mode : { FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO })
	{
		CAPTURE(mode);
		REQUIRE(std::fesetround(mode) == 0);
		// Parsing rounds to nearest-even independently of the ambient rounding mode.
		CheckBothEncodings("0.1", 0.1f);
		CheckBothEncodings("0.1", 0.1);
		CheckBothEncodings("1.000000059604644775390625", 1.0f);
		CheckBothEncodings("1.000000059604644775390626", std::bit_cast<float>(uint32{ 0x3F800001 }));
		CheckBothEncodings("1.00000000000000011102230246251565404236316680908203125", 1.0);
		CheckBothEncodings("1.00000000000000011102230246251565404236316680908203126",
			std::bit_cast<double>(uint64{ 0x3FF0000000000001 }));
	}
}
