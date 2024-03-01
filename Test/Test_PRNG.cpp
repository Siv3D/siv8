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

# include "Siv3DTest.hpp"

# if SIV3D_RUN_BENCHMARK

TEST_CASE("PRNG.Benchmark")
{
	{
		HardwareRNG rng{};

		Bench{}.title("HardwareRNG").run("rng()", [&]()
			{
				auto r = rng();
				doNotOptimizeAway(r);
			});
	}

	{
		std::mt19937_64 rng;

		Bench{}.title("std::mt19937_64").run("rng()", [&]()
			{
				auto r = rng();
				doNotOptimizeAway(r);
			});
	}

	{
		PRNG::SplitMix64 rng;

		Bench{}.title("PRNG::SplitMix64").run("rng()", [&]()
			{
				auto r = rng();
				doNotOptimizeAway(r);
			});
	}

	{
		PRNG::Xoshiro256PlusPlus rng;

		Bench{}.title("PRNG::Xoshiro256PlusPlus").run("rng()", [&]()
			{
				auto r = rng();
				doNotOptimizeAway(r);
			});
	}

	{
		PRNG::PCG64 rng;

		Bench{}.title("PRNG::PCG64").run("rng()", [&]()
			{
				auto r = rng();
				doNotOptimizeAway(r);
			});
	}

	{
		Bench{}.title("GetDefaultRNG()").run("rng()", [&]()
			{
				auto r = GetDefaultRNG()();
				doNotOptimizeAway(r);
			});
	}
}

# endif
