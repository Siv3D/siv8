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

TEST_CASE("String.operator ==.Benchmark")
{
	const String s5 = U"Siv3D";
	const String s8 = U"C++Siv3D";
	const String s10 = U"OpenSiv3D!";
	const String s20 = U"Abcdefghijklmnopqrst";
	const String s445 = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

	{
		Bench{}.title("String::operator ==").run("s5", [&]() { doNotOptimizeAway(s5 == s5); });
		Bench{}.title("String::operator ==").run("s8", [&]() { doNotOptimizeAway(s8 == s8); });
		Bench{}.title("String::operator ==").run("s10", [&]() { doNotOptimizeAway(s10 == s10); });
		Bench{}.title("String::operator ==").run("s20", [&]() { doNotOptimizeAway(s20 == s20); });
		Bench{}.title("String::operator ==").run("s445", [&]() { doNotOptimizeAway(s445 == s445); });
	}

	{
		Bench{}.title("String::Eq").run("s5", [&]() { doNotOptimizeAway(String::Eq(s5, s5)); });
		Bench{}.title("String::Eq").run("s8", [&]() { doNotOptimizeAway(String::Eq(s8, s8)); });
		Bench{}.title("String::Eq").run("s10", [&]() { doNotOptimizeAway(String::Eq(s10, s10)); });
		Bench{}.title("String::Eq").run("s20", [&]() { doNotOptimizeAway(String::Eq(s20, s20)); });
		Bench{}.title("String::Eq").run("s445", [&]() { doNotOptimizeAway(String::Eq(s445, s445)); });
	}

	{
		Bench{}.title("String::Eq2").run("s5", [&]() { doNotOptimizeAway(String::Eq2(s5, s5)); });
		Bench{}.title("String::Eq2").run("s8", [&]() { doNotOptimizeAway(String::Eq2(s8, s8)); });
		Bench{}.title("String::Eq2").run("s10", [&]() { doNotOptimizeAway(String::Eq2(s10, s10)); });
		Bench{}.title("String::Eq2").run("s20", [&]() { doNotOptimizeAway(String::Eq2(s20, s20)); });
		Bench{}.title("String::Eq2").run("s445", [&]() { doNotOptimizeAway(String::Eq2(s445, s445)); });
	}
}

# endif
