//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/BigFloat.hpp>
# include <Siv3D/BigInt.hpp>
# include "Siv3DTestFramework.hpp"
# include <type_traits>
# include <utility>

using namespace s3d;

static_assert(std::is_nothrow_move_constructible_v<BigFloat>);
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
