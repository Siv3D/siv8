//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/BigInt.hpp>
# include "Siv3DTestFramework.hpp"
# include <array>
# include <type_traits>
# include <utility>

using namespace s3d;

static_assert(std::is_nothrow_move_constructible_v<BigInt>);
static_assert(std::is_same_v<decltype(std::declval<BigInt>() <=> 1), std::strong_ordering>);
static_assert(std::is_same_v<decltype(std::declval<BigInt>() <=> 1.0), std::partial_ordering>);
static_assert(not noexcept(std::declval<BigInt>() == 1.0));
static_assert(not noexcept(std::declval<BigInt&>().bitSet(0, true)));
static_assert(not noexcept(std::declval<BigInt&>().bitFlip(0)));

TEST_CASE("BigInt.left subtraction")
{
	CHECK((10 - BigInt{ 3 }) == 7);
	CHECK((0u - BigInt{ 3 }) == -3);
	CHECK((-10 - BigInt{ -3 }) == -7);
	CHECK((std::numeric_limits<uint64>::max() - BigInt{ std::numeric_limits<uint64>::max() }) == 0);
	CHECK((std::numeric_limits<int64>::min() - BigInt{ 1 }).to_string() == "-9223372036854775809");
}

TEST_CASE("BigInt.move and reassign")
{
	const BigInt original{ "1234567890123456789012345678901234567890" };
	auto check = [&](auto assign, const BigInt& expected)
	{
		BigInt source = original;
		BigInt destination{ std::move(source) };
		CHECK(destination == original);
		assign(source);
		CHECK(source == expected);
		destination = std::move(source);
		CHECK(destination == expected);
		assign(source);
		CHECK(source == expected);
	};
	check([](BigInt& x) { x = -2; }, BigInt{ -2 });
	check([](BigInt& x) { x = 2u; }, BigInt{ 2 });
	check([&](BigInt& x) { x = original; }, original);
	check([](BigInt& x) { x = "42"; }, BigInt{ 42 });
	check([](BigInt& x) { x = U"43"; }, BigInt{ 43 });
	check([](BigInt& x) { x = BigInt{ 44 }; }, BigInt{ 44 });
	BigInt source{ 7 };
	BigInt destination{ std::move(source) };
	source.swap(destination);
	CHECK(source == 7);
	destination = 8;
	CHECK(destination == 8);
	const BigInt& self = source;
	source = self;
	CHECK(source == 7);
}

namespace
{
	template <class Float>
	void CheckFloatingComparison()
	{
		CHECK_FALSE(BigInt{ 2 } == Float{ 2.5 });
		CHECK_FALSE(Float{ -2.5 } == BigInt{ -2 });
		CHECK(BigInt{ 2 } < Float{ 2.5 });
		CHECK(Float{ 2.5 } > BigInt{ 2 });
		CHECK(BigInt{ -2 } > Float{ -2.5 });
		CHECK(BigInt{ 2 } == Float{ 2 });
		CHECK(BigInt{ 0 } == -Float{ 0 });
		CHECK(BigInt{ 0 } < std::numeric_limits<Float>::denorm_min());
		CHECK(BigInt{ 0 } > -std::numeric_limits<Float>::denorm_min());
		const Float inf = std::numeric_limits<Float>::infinity();
		const BigInt huge = BigInt{ 1 } << 20000;
		CHECK(huge < inf);
		CHECK(-huge > -inf);
		CHECK(huge > std::numeric_limits<Float>::max());
		CHECK(-huge < -std::numeric_limits<Float>::max());
		const Float nan = std::numeric_limits<Float>::quiet_NaN();
		CHECK(BigInt{ 2 }.compare(nan) == std::partial_ordering::unordered);
		CHECK((nan <=> BigInt{ 2 }) == std::partial_ordering::unordered);
		CHECK_FALSE(BigInt{ 2 } == nan);
		CHECK_FALSE(BigInt{ 2 } < nan);
		CHECK_FALSE(BigInt{ 2 } <= nan);
		CHECK_FALSE(BigInt{ 2 } > nan);
		CHECK_FALSE(BigInt{ 2 } >= nan);
		CHECK(nan != BigInt{ 2 });
	}
}

TEST_CASE("BigInt.floating comparison")
{
	CheckFloatingComparison<float>();
	CheckFloatingComparison<double>();
	CheckFloatingComparison<long double>();
	const BigInt boundary = BigInt{ 1 } << 100;
	const double f = std::ldexp(1.0, 100);
	CHECK(boundary == f);
	CHECK(boundary - 1 < f);
	CHECK(boundary + 1 > f);
}

TEST_CASE("BigInt.unsigned modulo conversion")
{
	for (int64 value : { int64{ 0 }, int64{ 1 }, int64{ -1 }, int64{ -2 }, std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max() })
	{
		const BigInt n{ value };
		CHECK(n.asUint32() == static_cast<uint32>(value));
		CHECK(n.asUint64() == static_cast<uint64>(value));
		CHECK(static_cast<size_t>(n) == static_cast<size_t>(value));
	}
	for (const uint32 bits : { 32u, 64u, 256u, 4096u })
	{
		const BigInt n = BigInt{ 1 } << bits;
		CHECK(n.asUint32() == 0);
		CHECK((-n).asUint32() == 0);
		CHECK((n + 17).asUint32() == 17);
		CHECK((-n - 17).asUint32() == (uint32{ 0 } - 17u));
		if (bits >= 64)
		{
			CHECK(n.asUint64() == 0);
			CHECK((-n).asUint64() == 0);
			CHECK((n + 17).asUint64() == 17);
			CHECK((-n - 17).asUint64() == (uint64{ 0 } - 17u));
			CHECK(static_cast<size_t>(n - 1) == std::numeric_limits<size_t>::max());
		}
	}
}

TEST_CASE("BigInt.divmod aliases")
{
	for (const int a : { 0, 7, -7 })
	{
		for (const int b : { 3, -3 })
		{
			for (const uint32 shift : { 0u, 192u })
			{
				const BigInt dividend = (BigInt{ a } << shift);
				const BigInt divisor = (BigInt{ b } << shift);
				const BigInt quotient = dividend / divisor;
				const BigInt remainder = dividend % divisor;
				for (size_t qi = 0; qi < 4; ++qi)
				{
					for (size_t ri = 0; ri < 4; ++ri)
					{
						if (qi == ri) { continue; }
						std::array<BigInt, 4> values{ dividend, divisor, BigInt{ 91 }, BigInt{ 92 } };
						values[0].divmod(values[1], values[qi], values[ri]);
						CHECK(values[qi] == quotient);
						CHECK(values[ri] == remainder);
						CHECK(dividend == values[qi] * divisor + values[ri]);
					}
				}
			}
		}
	}
	BigInt a{ -7 }, r;
	a.divmod(a, a, r);
	CHECK(a == 1);
	CHECK(r == 0);
	BigInt q;
	CHECK_THROWS_AS(a.divmod(BigInt{ 0 }, q, r), std::overflow_error);
}

TEST_CASE("BigInt.bits and string round trip")
{
	BigInt n;
	n.bitSet(4096, true);
	CHECK(n == (BigInt{ 1 } << 4096));
	n.bitFlip(4096);
	CHECK(n == 0);
	n.bitFlip(256);
	CHECK(n.bitTest(256));
	n.bitSet(256, false);
	CHECK(n == 0);
	const BigInt value = -(BigInt{ 1 } << 512) + 7;
	CHECK(BigInt{ value.to_string() } == value);
	CHECK(BigInt{ value.str() } == value);
	CHECK_THROWS(BigInt{ "invalid" });
}
