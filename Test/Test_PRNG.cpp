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
	const ScopedLogSilencer logSilencer;
	
	{
		HardwareRNG hardwareRNG{};
		std::mt19937_64 stdMT;
		PRNG::SplitMix64 splitMix;
		PRNG::Xoshiro256PlusPlus xoshiro;
		PRNG::PCG64 pcg;

		Bench{}.title("rng()").run("HardwareRNG", [&]()
			{
				auto r = hardwareRNG();
				doNotOptimizeAway(r);
			});

		Bench{}.title("rng()").run("std::mt19937_64", [&]()
			{
				auto r = stdMT();
				doNotOptimizeAway(r);
			});

		Bench{}.title("rng()").run("PRNG::SplitMix64", [&]()
			{
				auto r = splitMix();
				doNotOptimizeAway(r);
			});

		Bench{}.title("rng()").run("PRNG::Xoshiro256PlusPlus", [&]()
			{
				auto r = xoshiro();
				doNotOptimizeAway(r);
			});

		Bench{}.title("rng()").run("PRNG::PCG64", [&]()
			{
				auto r = pcg();
				doNotOptimizeAway(r);
			});

		Bench{}.title("rng()").run("GetDefaultRNG()", [&]()
			{
				auto r = GetDefaultRNG()();
				doNotOptimizeAway(r);
			});
	}
}

# endif
