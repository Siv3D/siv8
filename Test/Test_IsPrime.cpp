//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <array>
# include <limits>
# include <type_traits>
# include <vector>

static_assert(std::is_same_v<decltype(Math::IsPrime(uint64{})), bool>);
static_assert(noexcept(Math::IsPrime(uint64{})));
static_assert(not Math::IsPrime(uint64{ 0 }));
static_assert(not Math::IsPrime(uint64{ 1 }));
static_assert(Math::IsPrime(uint64{ 2 }));
static_assert(Math::IsPrime(uint64{ 65537 }));
static_assert(Math::IsPrime(uint64{ 4294967291ull }));
static_assert(Math::IsPrime(uint64{ 2305843009213693951ull }));
static_assert(Math::IsPrime(uint64{ 18446744073709551557ull }));
static_assert(not Math::IsPrime(uint64{ 341550071728321ull }));
static_assert(not Math::IsPrime(std::numeric_limits<uint64>::max()));

namespace
{
	bool TrialDivisionIsPrime(const uint64 n)
	{
		if (n < 2)
		{
			return false;
		}

		for (uint64 divisor = 2; divisor <= (n / divisor); ++divisor)
		{
			if ((n % divisor) == 0)
			{
				return false;
			}
		}

		return true;
	}
}

TEST_CASE("IsPrime.small_range")
{
	constexpr uint64 Limit = 100000;
	std::vector<bool> expected((Limit + 1), true);
	expected[0] = expected[1] = false;
	for (uint64 divisor = 2; (divisor * divisor) <= Limit; ++divisor)
	{
		if (not expected[divisor])
		{
			continue;
		}
		for (uint64 multiple = (divisor * divisor); multiple <= Limit; multiple += divisor)
		{
			expected[multiple] = false;
		}
	}

	for (uint64 n = 0; n <= Limit; ++n)
	{
		CAPTURE(n);
		CHECK(Math::IsPrime(n) == static_cast<bool>(expected[n]));
	}
}

TEST_CASE("IsPrime.boundaries")
{
	// Independently check both sides of small-table and integer-width boundaries.
	for (const uint64 center : { 1024ull, 11881ull, 39601ull, (1ull << 21), (1ull << 32) })
	{
		for (uint64 n = (center - 32); n <= (center + 32); ++n)
		{
			CAPTURE(center, n);
			CHECK(Math::IsPrime(n) == TrialDivisionIsPrime(n));
		}
	}
}

TEST_CASE("IsPrime.large_primes")
{
	// Includes primes above the signed 32-bit and signed 64-bit ranges.
	for (const uint64 n : { 2147483647ull, 4294967291ull, 4294967311ull,
		2305843009213693951ull, 9223372036854775783ull, 18446744073709551557ull })
	{
		CAPTURE(n);
		CHECK(Math::IsPrime(n));
	}
}

TEST_CASE("IsPrime.pseudoprimes")
{
	// Carmichael numbers and strong pseudoprimes that defeat incomplete base sets.
	for (const uint64 n : { 561ull, 1105ull, 1729ull, 2047ull, 1373653ull,
		25326001ull, 3215031751ull, 2152302898747ull, 3474749660383ull,
		341550071728321ull, 3825123056546413051ull, 13148563482635885461ull })
	{
		CAPTURE(n);
		CHECK_FALSE(Math::IsPrime(n));
	}
}

TEST_CASE("IsPrime.factorized_composites")
{
	constexpr std::array<uint64, 7> factors{ 199, 65537, 104729, 1000000007,
		2147483647, 4294967279ull, 4294967291ull };
	for (const uint64 a : factors)
	{
		for (const uint64 b : factors)
		{
			CAPTURE(a, b);
			CHECK_FALSE(Math::IsPrime(a * b));
		}
	}

	for (uint32 exponent = 2; exponent < 64; ++exponent)
	{
		CAPTURE(exponent);
		CHECK_FALSE(Math::IsPrime(uint64{ 1 } << exponent));
	}
	for (const uint64 n : { (1ull << 62) - 1, (1ull << 62) + 1,
		(1ull << 63) - 1, (1ull << 63) + 1, 18446744073709551614ull, 18446744073709551615ull })
	{
		CAPTURE(n);
		CHECK_FALSE(Math::IsPrime(n));
	}
}
