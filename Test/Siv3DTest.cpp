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

# define ANKERL_NANOBENCH_IMPLEMENT
# include <ThirdParty/nanobench/nanobench.h>

# define DOCTEST_CONFIG_IMPLEMENT
# include <ThirdParty/doctest/doctest.h>

# include "Siv3DTest.hpp"

void TestPlatform();

void RunTest()
{
	Console.open();

	TestPlatform();

	doctest::Context context;

	context.run();
}

TEST_CASE("Types.hpp")
{
	CHECK(sizeof(int8) == 1);
	CHECK(sizeof(uint8) == 1);
	CHECK(sizeof(int16) == 2);
	CHECK(sizeof(uint16) == 2);
	CHECK(sizeof(int32) == 4);
	CHECK(sizeof(uint32) == 4);
	CHECK(sizeof(int64) == 8);
	CHECK(sizeof(uint64) == 8);
	CHECK(sizeof(char8) == 1);
	CHECK(sizeof(char16) == 2);
	CHECK(sizeof(char32) == 4);
}

TEST_CASE("Unicode.hpp")
{
	CHECK(Unicode::WidenAscii("") == U"");
	CHECK(Unicode::WidenAscii("abc!#@") == U"abc!#@");

	CHECK(Unicode::Widen("") == U"");
	CHECK(Unicode::Widen("abc!#@") == U"abc!#@");
	CHECK(Unicode::Widen("渋三次元") == U"渋三次元");

	CHECK(Unicode::FromWstring(L"") == U"");
	CHECK(Unicode::FromWstring(L"abc!#@") == U"abc!#@");
	CHECK(Unicode::FromWstring(L"渋三次元") == U"渋三次元");
}

void TestPlatform()
{
	Console << U"SIV3D_PLATFORM_NAME: "	SIV3D_PLATFORM_NAME;
	Console << U"SIV3D_INTRINSIC_TYPE: " SIV3D_INTRINSIC_TYPE;
	Console << U"SIV3D_BUILD_TYPE: " SIV3D_BUILD_TYPE;
	Console << U"SIV3D_COMPILER_NAME: " SIV3D_COMPILER_NAME;
	Console << U"SIV3D_CPU_TYPE: " SIV3D_CPU_TYPE;
}