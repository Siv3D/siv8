//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/BigFloat.hpp>
# include <Siv3D/BigInt.hpp>
# include "Siv3DTestFramework.hpp"
# include <bit>
# include <cmath>
# include <limits>
# include <random>
# include <type_traits>
# include <utility>

using namespace s3d;

static_assert(std::is_nothrow_move_constructible_v<BigFloat>);
static_assert(not noexcept(static_cast<float>(std::declval<BigFloat>())));
static_assert(not noexcept(static_cast<double>(std::declval<BigFloat>())));
static_assert(std::is_same_v<decltype(std::declval<BigFloat>() <=> 1), std::partial_ordering>);
static_assert(std::is_same_v<decltype(std::declval<BigFloat>() <=> std::declval<BigFloat>()), std::partial_ordering>);
static_assert(not noexcept(std::declval<BigFloat>() == std::declval<BigInt>()));
static_assert(not noexcept(std::declval<BigFloat>() <=> std::declval<BigInt>()));

TEST_CASE("BigFloat.left subtraction")
{
	CHECK((10 - BigFloat{ 3 }) == 7);
	CHECK((0u - BigFloat{ 3 }) == -3);
	CHECK((-10 - BigFloat{ -3 }) == -7);
	CHECK((10.5 - BigFloat{ 3 }) == 7.5);
	CHECK((10.5L - BigFloat{ 3 }) == 7.5);
	CHECK((BigInt{ 10 } - BigFloat{ 3 }) == 7);
}

TEST_CASE("BigFloat.move and swap")
{
	const BigFloat original{ "12345678901234567890.123456789" };
	auto check = [&](auto assign, const BigFloat& expected)
	{
		BigFloat source = original;
		BigFloat destination{ std::move(source) };
		CHECK(destination == original);
		assign(source);
		CHECK(source == expected);
		destination = std::move(source);
		CHECK(destination == expected);
		assign(source);
		CHECK(source == expected);
	};
	check([](BigFloat& x) { x = -2; }, BigFloat{ -2 });
	check([](BigFloat& x) { x = 2u; }, BigFloat{ 2 });
	check([](BigFloat& x) { x = 2.5; }, BigFloat{ 2.5 });
	check([](BigFloat& x) { x = BigInt{ 3 }; }, BigFloat{ 3 });
	check([&](BigFloat& x) { x = original; }, original);
	check([](BigFloat& x) { x = "4.25"; }, BigFloat{ 4.25 });
	check([](BigFloat& x) { x = U"4.5"; }, BigFloat{ 4.5 });
	check([](BigFloat& x) { x = BigFloat{ 5 }; }, BigFloat{ 5 });
	BigFloat a{ 1.25 }, b{ 2.5 };
	a.swap(b);
	CHECK(a == 2.5);
	CHECK(b == 1.25);
	a.swap(a);
	CHECK(a == 2.5);
	BigFloat c{ std::move(a) };
	swap(a, b);
	CHECK(a == 1.25);
	b = 3;
	CHECK(b == 3);
	CHECK(c == 2.5);
	const BigFloat& self = a;
	a = self;
	CHECK(a == 1.25);
}

TEST_CASE("BigFloat.string assignment")
{
	for (const std::string_view initial : { "0", "-7.5", "nan", "inf", "-inf" })
	{
		for (const auto& [number, wideNumber] : {
			std::pair<std::string_view, StringView>{ "0", U"0" },
			{ "-0", U"-0" },
			{ "1.25", U"1.25" },
			{ "-2.5", U"-2.5" },
			{ "1e-1000", U"1e-1000" },
			{ "-1e1000", U"-1e1000" },
			{ "12345678901234567890.123456789", U"12345678901234567890.123456789" },
			{ "nan", U"nan" },
			{ "inf", U"inf" },
			{ "-inf", U"-inf" } })
		{
			CAPTURE(initial, number);
			const BigFloat expected{ number };
			BigFloat narrow{ initial }, wide{ initial };
			CHECK(&(narrow = number) == &narrow);
			CHECK(&(wide = wideNumber) == &wide);
			if (expected.isNaN())
			{
				CHECK(narrow.isNaN());
				CHECK(wide.isNaN());
			}
			else
			{
				CHECK(narrow == expected);
				CHECK(wide == expected);
			}
		}

		BigFloat narrow{ initial }, wide{ initial };
		CHECK_THROWS(narrow = "invalid");
		CHECK_THROWS(wide = U"invalid");
		narrow = "1.25";
		wide = U"-2.5";
		CHECK(narrow == 1.25);
		CHECK(wide == -2.5);
	}
}

TEST_CASE("BigFloat.partial comparison")
{
	const BigFloat nan{ "nan" };
	auto checkUnordered = [&](const auto& rhs)
	{
		CHECK(nan.compare(rhs) == std::partial_ordering::unordered);
		CHECK((nan <=> rhs) == std::partial_ordering::unordered);
		CHECK((rhs <=> nan) == std::partial_ordering::unordered);
		CHECK_FALSE(nan == rhs);
		CHECK_FALSE(rhs == nan);
		CHECK_FALSE(nan < rhs);
		CHECK_FALSE(nan <= rhs);
		CHECK_FALSE(nan > rhs);
		CHECK_FALSE(nan >= rhs);
		CHECK_FALSE(rhs < nan);
		CHECK_FALSE(rhs <= nan);
		CHECK_FALSE(rhs > nan);
		CHECK_FALSE(rhs >= nan);
		CHECK(nan != rhs);
	};
	checkUnordered(nan);
	checkUnordered(BigFloat{ 0 });
	checkUnordered(BigFloat{ "inf" });
	checkUnordered(BigInt{ 0 });
	checkUnordered(0);
	checkUnordered(0u);
	checkUnordered(0.0);
	checkUnordered(std::numeric_limits<long double>::quiet_NaN());
	const double nativeNaN = std::numeric_limits<double>::quiet_NaN();
	CHECK(BigFloat{ 1 }.compare(nativeNaN) == std::partial_ordering::unordered);
	CHECK_FALSE(BigFloat{ 1 } == nativeNaN);
	CHECK_FALSE(BigFloat{ 1 } > nativeNaN);
	const BigFloat inf{ "inf" }, negativeInf{ "-inf" };
	CHECK(inf == std::numeric_limits<double>::infinity());
	CHECK(negativeInf == -std::numeric_limits<double>::infinity());
	CHECK(inf == inf);
	CHECK(negativeInf < inf);
	CHECK(inf > BigInt{ 1 });
	CHECK(negativeInf < BigInt{ -1 });
	CHECK(BigFloat{ 0 } == -0.0);
	CHECK(BigFloat{ "2.5" } > 2);
	CHECK(BigFloat{ "-2.5" } < -2);
	CHECK(BigFloat{ 3 } > BigFloat{ 2 });
}

TEST_CASE("BigFloat.rounded BigInt comparison")
{
	const BigInt n = BigInt{ 10 }.pow(200);
	const BigFloat f{ n };
	CHECK(f == n);
	CHECK(f == n + 1);
	CHECK(n + 1 == f);
	CHECK(f > -n);
	CHECK(-f < n);
	CHECK(BigFloat{ 0 } < n);
	CHECK(BigFloat{ 0 } > -n);
	CHECK(BigFloat{ 0 } == BigInt{ 0 });
}

namespace
{
	void CheckBigIntConversion(const BigInt& integer, const BigFloat& expected)
	{
		const BigFloat converted{ integer };
		CHECK(converted == expected);
		CHECK(static_cast<BigFloat>(integer) == expected);
		CHECK(integer.operator BigFloat() == expected);
		BigFloat assigned{ "nan" };
		CHECK(&(assigned = integer) == &assigned);
		CHECK(assigned == expected);
		assigned = std::numeric_limits<double>::infinity();
		assigned = integer;
		CHECK(assigned == expected);
		assigned = -std::numeric_limits<double>::infinity();
		assigned = integer;
		CHECK(assigned == expected);
		CHECK(expected == integer);
		CHECK(integer == expected);
		CHECK(expected.compare(integer) == std::partial_ordering::equivalent);
		CHECK((expected <=> integer) == std::partial_ordering::equivalent);
		CHECK((BigFloat{ 0 } + integer) == expected);
		CHECK((BigFloat{ 0 } - integer) == -expected);
		CHECK((BigFloat{ 1 } * integer) == expected);
		CHECK((integer + BigFloat{ 0 }) == expected);
		CHECK((integer - BigFloat{ 0 }) == expected);
		CHECK((integer * BigFloat{ 1 }) == expected);
		CHECK((integer / BigFloat{ 1 }) == expected);
		assigned = 0;
		CHECK((assigned += integer) == expected);
		assigned = 0;
		CHECK((assigned -= integer) == -expected);
		assigned = 1;
		CHECK((assigned *= integer) == expected);
		if (integer != 0)
		{
			CHECK((BigFloat{ 1 } / integer) == (BigFloat{ 1 } / expected));
			assigned = 1;
			CHECK((assigned /= integer) == (BigFloat{ 1 } / expected));
		}
		CHECK(BigFloat{ converted.to_string(0, std::ios_base::fmtflags{}) } == converted);
	}
}

TEST_CASE("BigFloat.BigInt exact conversion")
{
	for (const auto text : { "0", "1", "-1", "9223372036854775807", "9223372036854775808",
		"-9223372036854775808", "-9223372036854775809", "18446744073709551615",
		"-18446744073709551615", "18446744073709551616", "-18446744073709551616" })
	{
		CheckBigIntConversion(BigInt{ text }, BigFloat{ text });
	}
	for (const uint32 exponent : { 38u, 100u, 120u, 128u, 200u, 201u, 207u, 208u, 1000u })
	{
		const BigInt integer = BigInt{ 10 }.pow(exponent);
		const BigFloat expected{ "1e" + std::to_string(exponent) };
		CheckBigIntConversion(integer, expected);
		CheckBigIntConversion(-integer, -expected);
	}
	const std::string significant = ("1234567890" + std::string(89, '9') + "1");
	const BigInt exact{ significant + std::string(200, '0') };
	const BigFloat expected{ significant + "e200" };
	CheckBigIntConversion(exact, expected);
	CheckBigIntConversion(-exact, -expected);

	BigFloat source{ 1 };
	const BigFloat destination{ std::move(source) };
	source = BigInt{ 10 }.pow(200);
	CHECK(source == BigFloat{ "1e200" });
	CHECK(destination == 1);
}

TEST_CASE("BigFloat.BigInt decimal input agreement")
{
	std::mt19937_64 random{ 0xB16F10A7 };
	for (const size_t digits : { 19u, 20u, 21u, 38u, 39u, 100u, 120u, 121u, 127u, 128u, 129u,
		150u, 151u, 152u, 153u, 159u, 160u, 161u, 199u, 200u, 201u, 999u, 1000u, 10000u })
	{
		std::string text(digits, '0');
		text[0] = static_cast<char>('1' + (random() % 9));
		for (size_t index = 1; index < digits; ++index)
		{
			text[index] = static_cast<char>('0' + (random() % 10));
		}
		const BigInt integer{ text };
		const BigFloat expected{ text };
		CheckBigIntConversion(integer, expected);
		CheckBigIntConversion(-integer, -expected);
	}
}

TEST_CASE("BigFloat.BigInt truncation toward zero")
{
	// At this decimal alignment the backend retains 128 digits, including its guard digits.
	const std::string prefix(128, '9');
	const BigFloat expected{ prefix + "e32" };
	for (const char discardedDigit : { '0', '4', '5', '9' })
	{
		const BigInt integer{ prefix + std::string(32, discardedDigit) };
		CheckBigIntConversion(integer, expected);
		CheckBigIntConversion(-integer, -expected);
	}
	const BigInt power = BigInt{ 10 }.pow(200);
	CHECK(BigFloat{ power + 1 } == BigFloat{ "1e200" });
	CHECK(BigFloat{ power - 1 } < BigFloat{ "1e200" });
	CHECK(BigFloat{ -power - 1 } == BigFloat{ "-1e200" });
	CHECK(BigFloat{ -power + 1 } > BigFloat{ "-1e200" });
}

namespace
{
	template <class Float>
	void CheckNativeFloatConversion()
	{
		auto convert = [](const BigFloat& value)
		{
			const Float result = static_cast<Float>(value);
			if constexpr (std::is_same_v<Float, float>)
			{
				CHECK(value.asFloat() == result);
			}
			else
			{
				CHECK(value.asDouble() == result);
			}
			return result;
		};
		const Float infinity = std::numeric_limits<Float>::infinity();
		for (const Float value : { Float{ 0 }, Float{ 1.25 }, Float{ -2.5 },
			std::numeric_limits<Float>::denorm_min(), std::numeric_limits<Float>::min(),
			std::nextafter(std::numeric_limits<Float>::min(), Float{ 0 }),
			std::numeric_limits<Float>::max() })
		{
			CAPTURE(value);
			CHECK(convert(BigFloat{ value }) == value);
			CHECK(convert(BigFloat{ -value }) == -value);
		}
		CHECK(convert(BigFloat{ "inf" }) == infinity);
		CHECK(convert(BigFloat{ "-inf" }) == -infinity);
		CHECK(convert(BigFloat{ "1e10000" }) == infinity);
		CHECK(convert(BigFloat{ "-1e10000" }) == -infinity);
		CHECK(convert(BigFloat{ "1e-10000" }) == 0);
		const Float negativeUnderflow = convert(BigFloat{ "-1e-10000" });
		CHECK(negativeUnderflow == 0);
		CHECK(std::signbit(negativeUnderflow));
		CHECK(std::isnan(static_cast<Float>(BigFloat{ "nan" })));

		const BigFloat halfway{ std::is_same_v<Float, float>
			? "1.000000059604644775390625"
			: "1.00000000000000011102230246251565404236316680908203125" };
		const BigFloat epsilon{ "1e-100" };
		CHECK(convert(halfway) == Float{ 1 });
		CHECK(convert(halfway - epsilon) == Float{ 1 });
		CHECK(convert(halfway + epsilon) == std::nextafter(Float{ 1 }, infinity));
		CHECK(convert(-halfway - epsilon) == std::nextafter(Float{ -1 }, -infinity));

		using Bits = std::conditional_t<std::is_same_v<Float, float>, uint32, uint64>;
		std::mt19937_64 random{ 0xB16F10A7 };
		for (size_t index = 0; index < 512; ++index)
		{
			const Float value = std::bit_cast<Float>(static_cast<Bits>(random()));
			if (std::isfinite(value))
			{
				CAPTURE(value);
				CHECK(convert(BigFloat{ value }) == value);
			}
		}
	}
}

TEST_CASE("BigFloat.native floating conversion")
{
	CheckNativeFloatConversion<float>();
	CheckNativeFloatConversion<double>();
	CHECK(BigFloat{ "1e-308" }.asDouble() == 1e-308);
	CHECK(BigFloat{ "-1e-308" }.asDouble() == -1e-308);
	CHECK(std::isnan(BigFloat{ "nan" }.asFloat()));
	CHECK(std::isnan(BigFloat{ "nan" }.asDouble()));
}

TEST_CASE("BigFloat.format precision and round trip")
{
	CHECK(BigFloat{ "1e-101" }.to_string() == "0");
	CHECK(BigFloat{ "1.2300" }.to_string() == "1.23");
	CHECK(BigFloat{ "1.23456" }.to_string(3) == "1.235");
	CHECK(BigFloat{ "1.23456" }.to_string(3, std::ios_base::scientific) == "1.235e+00");
	CHECK(BigFloat{ "1.23456" }.to_string(3, std::ios_base::fmtflags{}) == "1.23");
	for (const auto text : { "0", "-0", "1e-101", "-1e-1000", "1e10000", "inf", "-inf",
		"1.2345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789" })
	{
		const BigFloat value{ text };
		const std::string encoded = value.to_string(0, std::ios_base::fmtflags{});
		CHECK(encoded.size() < 160);
		CHECK(BigFloat{ encoded } == value);
		CHECK(BigFloat{ value.str(0, std::ios_base::fmtflags{}) } == value);
	}
	const BigFloat fraction = BigFloat{ 1 } / 7;
	CHECK(BigFloat{ fraction.to_string(0, std::ios_base::fmtflags{}) } == fraction);
	CHECK(BigFloat{ BigFloat{ "nan" }.to_string(0, std::ios_base::fmtflags{}) }.isNaN());
	CHECK_THROWS(BigFloat{ "invalid" });
}
