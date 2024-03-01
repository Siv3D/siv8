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

TEST_CASE("Memory.Benchmark")
{
	{
		Bench{}.title("Malloc").run("1024", [&]()
			{
				for (int32 i = 0; i < 1000; ++i)
				{
					uint8* p = static_cast<uint8*>(Malloc(1024));
					doNotOptimizeAway(p);
					Free(p);
				}
			});
	}

	{
		Bench{}.title("vector<Byte>").run("1024", [&]()
			{
				for (int32 i = 0; i < 1000; ++i)
				{
					std::vector<Byte> v(1024);
					doNotOptimizeAway(v);
				}
			});
	}

	{
		std::vector<std::string> v(1024);

		Bench{}.title("vector<string>").run("1024 * 64", [&]()
			{
				for (auto& s : v)
				{
					s = std::string(64, '0');
					doNotOptimizeAway(s);
				}
				
				doNotOptimizeAway(v);
			});
	}

	{
		std::vector<std::string> v(16384);

		Bench{}.title("vector<string>").run("16384 * 64", [&]()
			{
				for (auto& s : v)
				{
					s = std::string(64, '0');
					doNotOptimizeAway(s);
				}

				doNotOptimizeAway(v);
			});
	}

	{
		std::vector<std::string> v(65536);

		Bench{}.title("vector<string>").run("65536 * 40", [&]()
			{
				for (auto& s : v)
				{
					s = std::string(40, 'a');
					doNotOptimizeAway(s);
				}

				doNotOptimizeAway(v);
			});
	}

	{
		std::vector<int*> v(1024);

		Bench{}.title("vector<int*>").run("1024 * 1024", [&]()
			{
				for (auto& s : v)
				{
					s = new int[1024];
					doNotOptimizeAway(s);
				}

				for (auto& s : v)
				{
					delete[] s;
				}

				doNotOptimizeAway(v);
			});
	}

	{
		std::vector<int*> v(8192);

		Bench{}.title("vector<int*>").run("8192 * 256", [&]()
			{
				for (auto& s : v)
				{
					s = new int[256];
					doNotOptimizeAway(s);
				}

				for (auto& s : v)
				{
					delete[] s;
				}

				doNotOptimizeAway(v);
			});
	}
}

# endif
