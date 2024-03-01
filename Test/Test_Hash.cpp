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

TEST_CASE("Hash.wyhash")
{
	{
		const int32 n = 123456789;
		CHECK_EQ(wyhash::Hash(&n, sizeof(n)), wyhash::Hash(&n, sizeof(n), 0));
		CHECK_EQ(wyhash::Hash(&n, sizeof(n)), wyhash::Hash(&n, sizeof(n), 0, wyhash::DefaultHashSecret));
		CHECK_EQ(wyhash::Hash(&n, sizeof(n)), wyhash::Hash(n));
	}

	{
		const std::string s = "Hello, Siv3D!";
		CHECK_EQ(wyhash::Hash(s.data(), s.size()), wyhash::Hash(s.data(), s.size(), 0));
		CHECK_EQ(wyhash::Hash(s.data(), s.size()), wyhash::Hash(s.data(), s.size(), 0, wyhash::DefaultHashSecret));
	}
}

TEST_CASE("Hash.xxHash")
{
	{
		const int32 n = 123456789;
		CHECK_EQ(xxHash3::Hash(&n, sizeof(n)), xxHash3::Hash(&n, sizeof(n), 0));
		CHECK_EQ(xxHash3::Hash(&n, sizeof(n)), xxHash3::Hash(n));
	}

	{
		const std::string s = "Hello, Siv3D!";
		CHECK_EQ(xxHash3::Hash(s.data(), s.size()), xxHash3::Hash(s.data(), s.size(), 0));
	}
}

TEST_CASE("MixHash")
{
	constexpr uint64 a = 1234567890123456789ull;
	constexpr uint64 b = 9876543210987654321ull;

	CHECK_NE(MixHash(a, 0), a);
	CHECK_NE(MixHash(0, a), a);

	CHECK_NE(MixHash(a, b), a);
	CHECK_NE(MixHash(b, a), a);
	
	CHECK_NE(MixHash(a, b), b);
	CHECK_NE(MixHash(b, a), b);

	CHECK_NE(MixHash(a, 0), MixHash(0, a));
	CHECK_NE(MixHash(a, 0), MixHash(0, b));
	CHECK_NE(MixHash(a, 0), MixHash(a, b));
	CHECK_NE(MixHash(a, 0), MixHash(b, a));
	CHECK_NE(MixHash(a, b), MixHash(b, a));
}

# if SIV3D_RUN_BENCHMARK

TEST_CASE("Hash.Benchmark")
{
	{
		const uint8 u8 = 123;
		const uint32 u32 = 123456789u;
		const uint64 u64 = 1234567890123456789ull;

		Bench{}.title("Hash uint8").run("std::hash", [&]() { doNotOptimizeAway(std::hash<uint8>{}(u8)); });
		Bench{}.title("Hash uint8").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(u8)); });
		Bench{}.title("Hash uint8").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(u8)); });

		Bench{}.title("Hash uint32").run("std::hash", [&]() { doNotOptimizeAway(std::hash<uint32>{}(u32)); });
		Bench{}.title("Hash uint32").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(u32)); });
		Bench{}.title("Hash uint32").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(u32)); });

		Bench{}.title("Hash uint64").run("std::hash", [&]() { doNotOptimizeAway(std::hash<uint64>{}(u64)); });
		Bench{}.title("Hash uint64").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(u64)); });
		Bench{}.title("Hash uint64").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(u64)); });
	}

	{
		const std::string s5 = "Siv3D";
		const std::string s20 = "Abcdefghijklmnopqrst";
		const std::string s445 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.title("Hash std::string(5)").run("std::hash", [&]() { doNotOptimizeAway(std::hash<std::string>{}(s5)); });
		Bench{}.title("Hash std::string(5)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(s5.data(), s5.size())); });
		Bench{}.title("Hash std::string(5)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(s5.data(), s5.size())); });

		Bench{}.title("Hash std::string(20)").run("std::hash", [&]() { doNotOptimizeAway(std::hash<std::string>{}(s20)); });
		Bench{}.title("Hash std::string(20)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(s20.data(), s20.size())); });
		Bench{}.title("Hash std::string(20)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(s20.data(), s20.size())); });

		Bench{}.title("Hash std::string(445)").run("std::hash", [&]() { doNotOptimizeAway(std::hash<std::string>{}(s445)); });
		Bench{}.title("Hash std::string(445)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(s445.data(), s445.size())); });
		Bench{}.title("Hash std::string(445)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(s445.data(), s445.size())); });
	}

	{
		const std::u32string s5 = U"Siv3D";
		const std::u32string s20 = U"Abcdefghijklmnopqrst";
		const std::u32string s445 = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.title("Hash std::u32string(5)").run("std::hash", [&]() { doNotOptimizeAway(std::hash<std::u32string>{}(s5)); });
		Bench{}.title("Hash std::u32string(5)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(s5.data(), (sizeof(char32) * s5.size()))); });
		Bench{}.title("Hash std::u32string(5)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(s5.data(), (sizeof(char32) * s5.size()))); });

		Bench{}.title("Hash std::u32string(20)").run("std::hash", [&]() { doNotOptimizeAway(std::hash<std::u32string>{}(s20)); });
		Bench{}.title("Hash std::u32string(20)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(s20.data(), (sizeof(char32) * s20.size()))); });
		Bench{}.title("Hash std::u32string(20)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(s20.data(), (sizeof(char32) * s20.size()))); });

		Bench{}.title("Hash std::u32string(445)").run("std::hash", [&]() { doNotOptimizeAway(std::hash<std::u32string>{}(s445)); });
		Bench{}.title("Hash std::u32string(445)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(s445.data(), (sizeof(char32) * s445.size()))); });
		Bench{}.title("Hash std::u32string(445)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(s445.data(), (sizeof(char32) * s445.size()))); });
	}

	{
		const std::vector<uint8> v1K(1024);
		const std::vector<uint8> v4K(4096);
		const std::vector<uint8> v16K(16384);

		Bench{}.title("Hash std::vector<uint8>(1K)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(v1K.data(), v1K.size())); });
		Bench{}.title("Hash std::vector<uint8>(1K)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(v1K.data(), v1K.size())); });

		Bench{}.title("Hash std::vector<uint8>(4K)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(v4K.data(), v4K.size())); });
		Bench{}.title("Hash std::vector<uint8>(4K)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(v4K.data(), v4K.size())); });

		Bench{}.title("Hash std::vector<uint8>(16K)").run("wyhash::Hash", [&]() { doNotOptimizeAway(wyhash::Hash(v16K.data(), v16K.size())); });
		Bench{}.title("Hash std::vector<uint8>(16K)").run("xxHash3::Hash", [&]() { doNotOptimizeAway(xxHash3::Hash(v16K.data(), v16K.size())); });
	}
}

# endif
