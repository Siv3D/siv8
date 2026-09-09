//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include <Siv3D/BigInt.hpp>
# include <Siv3D/BigFloat.hpp>
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
	template <class Int>
	concept HasConvertTo = requires(const BigInt& value)
	{
		{ value.template convertTo<Int>() } -> std::same_as<Optional<Int>>;
	};

	enum class IntegerLike : int32 {};

	static_assert(not HasConvertTo<bool>);
	static_assert(not HasConvertTo<const bool>);
	static_assert(not HasConvertTo<volatile bool>);
	static_assert(not HasConvertTo<float>);
	static_assert(not HasConvertTo<double>);
	static_assert(not HasConvertTo<long double>);
	static_assert(not HasConvertTo<IntegerLike>);
	static_assert(not HasConvertTo<BigInt>);
	static_assert(not HasConvertTo<void>);
	static_assert(not HasConvertTo<int32&>);
	static_assert(not HasConvertTo<int32*>);

# if defined(__SIZEOF_INT128__)
	static_assert(not HasConvertTo<__int128>);
	static_assert(not HasConvertTo<unsigned __int128>);
# endif

	template <Concept::Integral Int>
	void CheckIntegerConversion()
	{
		static_assert(HasConvertTo<Int>);
		static_assert(noexcept(std::declval<const BigInt&>().convertTo<Int>()));
		const Int min = std::numeric_limits<Int>::min();
		const Int max = std::numeric_limits<Int>::max();
		const BigInt lower{ min }, upper{ max };

		for (const Int expected : { Int{ 0 }, Int{ 1 }, Int{ 42 }, min, max,
			static_cast<Int>(min + 1), static_cast<Int>(max - 1) })
		{
			const BigInt original{ expected };
			const auto result = original.convertTo<Int>();
			REQUIRE(result.has_value());
			CHECK(*result == expected);
			CHECK(BigInt{ *result } == original);
		}

		CHECK((lower - 1).convertTo<Int>() == none);
		CHECK((upper + 1).convertTo<Int>() == none);
		const BigInt huge = (BigInt{ 1 } << 4096) + 7;
		CHECK(huge.convertTo<Int>() == none);
		CHECK((-huge).convertTo<Int>() == none);

		if constexpr (Concept::SignedIntegral<Int>)
		{
			CHECK(BigInt{ -1 }.convertTo<Int>() == Optional<Int>{ -1 });
			CHECK(BigInt{ -42 }.convertTo<Int>() == Optional<Int>{ -42 });
		}
		else
		{
			CHECK(BigInt{ -1 }.convertTo<Int>() == none);
			CHECK(BigInt{ -42 }.convertTo<Int>() == none);
		}
	}

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

TEST_CASE("BigInt.convertTo integer boundaries and round trip")
{
	CheckIntegerConversion<int8>();
	CheckIntegerConversion<uint8>();
	CheckIntegerConversion<int16>();
	CheckIntegerConversion<uint16>();
	CheckIntegerConversion<int32>();
	CheckIntegerConversion<uint32>();
	CheckIntegerConversion<int64>();
	CheckIntegerConversion<uint64>();
	CheckIntegerConversion<size_t>();
	CheckIntegerConversion<long>();
	CheckIntegerConversion<unsigned long>();
	CheckIntegerConversion<long long>();
	CheckIntegerConversion<unsigned long long>();
	CheckIntegerConversion<char>();
	CheckIntegerConversion<wchar_t>();
	CheckIntegerConversion<char8_t>();
	CheckIntegerConversion<char16_t>();
	CheckIntegerConversion<char32_t>();

	static_assert(HasConvertTo<const int32>);
	static_assert(HasConvertTo<volatile uint64>);
	CHECK(BigInt{ -42 }.convertTo<const int32>().value() == -42);
	CHECK(BigInt{ 42 }.convertTo<volatile uint64>().value() == 42);
}

TEST_CASE("BigInt.size_t modulo conversion")
{
	for (int64 value : { int64{ 0 }, int64{ 1 }, int64{ -1 }, int64{ -2 }, std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max() })
	{
		const BigInt n{ value };
		CHECK(static_cast<size_t>(n) == static_cast<size_t>(value));
	}
	for (const uint32 bits : { uint32{ std::numeric_limits<size_t>::digits }, 256u, 4096u })
	{
		const BigInt n = BigInt{ 1 } << bits;
		CHECK(static_cast<size_t>(n) == 0);
		CHECK(static_cast<size_t>(-n) == 0);
		CHECK(static_cast<size_t>(n + 17) == 17);
		CHECK(static_cast<size_t>(-n - 17) == (size_t{ 0 } - 17u));
		CHECK(static_cast<size_t>(n - 1) == std::numeric_limits<size_t>::max());
	}
}

TEST_CASE("BigInt.explicit floating conversions")
{
	static_assert(noexcept(static_cast<float>(std::declval<const BigInt&>())));
	static_assert(noexcept(static_cast<double>(std::declval<const BigInt&>())));
	static_assert(not std::is_convertible_v<BigInt, float>);
	static_assert(not std::is_convertible_v<BigInt, double>);

	for (const int64 value : { int64{ 0 }, int64{ 42 }, int64{ -42 },
		int64{ 16777217 }, int64{ -16777217 },
		int64{ 9007199254740993 }, int64{ -9007199254740993 } })
	{
		const BigInt n{ value };
		CHECK(static_cast<float>(n) == static_cast<float>(value));
		CHECK(static_cast<double>(n) == static_cast<double>(value));
		CHECK(static_cast<BigFloat>(n) == BigFloat{ value });
		CHECK(n.operator BigFloat() == BigFloat{ value });
	}

	const BigInt huge = BigInt{ 1 } << 4096;
	CHECK(static_cast<float>(huge) == std::numeric_limits<float>::infinity());
	CHECK(static_cast<float>(-huge) == -std::numeric_limits<float>::infinity());
	CHECK(static_cast<double>(huge) == std::numeric_limits<double>::infinity());
	CHECK(static_cast<double>(-huge) == -std::numeric_limits<double>::infinity());

	const BigInt power = BigInt{ 10 }.pow(200);
	const BigFloat converted = static_cast<BigFloat>(power);
	CHECK(converted == BigFloat{ power });
	CHECK(converted == BigFloat{ "1e200" });
	CHECK(static_cast<BigFloat>(power + 1) == converted);
	CHECK((power + 1).operator BigFloat() == converted);
	CHECK(static_cast<BigFloat>(-power) == -converted);
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
