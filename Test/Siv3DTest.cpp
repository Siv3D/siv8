//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# define ANKERL_NANOBENCH_IMPLEMENT
# include <ThirdParty/nanobench/nanobench.h>
using ankerl::nanobench::Bench;
using ankerl::nanobench::doNotOptimizeAway;

# define DOCTEST_CONFIG_IMPLEMENT
# define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
# if SIV3D_COMPILER(APPLECLANG) // Xcode では色付けを無効化
#   define DOCTEST_CONFIG_COLORS_NONE
# endif
# include <ThirdParty/doctest/doctest.h>

void RunTest()
{
	Console.open();

	doctest::Context context;

	context.run();
}


TEST_CASE("Platform.hpp")
{

}

TEST_CASE("Version.hpp")
{

}

TEST_CASE("Types.hpp")
{

}

TEST_CASE("Hash.hpp")
{
	{
		const std::string sShort = "Siv3D";
		const std::string sMedium = "Hello, Siv3D!";
		const std::string sLong = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.title("Hash").run("std::hash - std::string short", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sShort)); });
		Bench{}.title("Hash").run("std::hash - std::string medium", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sMedium)); });
		Bench{}.title("Hash").run("std::hash - std::string long", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sLong)); });
		Bench{}.title("Hash").run("wyhash::Hash - std::string short", [&]() { doNotOptimizeAway(wyhash::Hash(sShort.data(), sShort.size())); });
		Bench{}.title("Hash").run("wyhash::Hash - std::string medium", [&]() { doNotOptimizeAway(wyhash::Hash(sMedium.data(), sMedium.size())); });
		Bench{}.title("Hash").run("wyhash::Hash - std::string long", [&]() { doNotOptimizeAway(wyhash::Hash(sLong.data(), sLong.size())); });
		Bench{}.title("Hash").run("xxHash3::Hash - std::string short", [&]() { doNotOptimizeAway(xxHash3::Hash(sShort.data(), sShort.size())); });
		Bench{}.title("Hash").run("xxHash3::Hash - std::string medium", [&]() { doNotOptimizeAway(xxHash3::Hash(sMedium.data(), sMedium.size())); });
		Bench{}.title("Hash").run("xxHash3::Hash - std::string long", [&]() { doNotOptimizeAway(xxHash3::Hash(sLong.data(), sLong.size())); });
	}
}