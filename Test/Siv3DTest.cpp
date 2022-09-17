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
# if SIV3D_COMPILER(APPLECLANG) // Xcode では色付けを無効化
#   define DOCTEST_CONFIG_COLORS_NONE
# endif
# include <ThirdParty/doctest/doctest.h>

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
	const std::string utf8 = Unicode::ToUTF8(U"渋三次元");

	CHECK(Unicode::WidenAscii("") == U"");
	CHECK(Unicode::WidenAscii("abc!#@") == U"abc!#@");
	CHECK(Unicode::WidenAscii("渋三次元") == U"");

	CHECK(Unicode::Widen("") == U"");
	CHECK(Unicode::Widen("abc!#@") == U"abc!#@");
	CHECK(Unicode::Widen("渋三次元") == U"渋三次元");

	CHECK(Unicode::FromWstring(L"") == U"");
	CHECK(Unicode::FromWstring(L"abc!#@") == U"abc!#@");
	CHECK(Unicode::FromWstring(L"渋三次元") == U"渋三次元");

	CHECK(Unicode::FromUTF8("") == U"");
	CHECK(Unicode::FromUTF8("abc!#@") == U"abc!#@");
	CHECK(Unicode::FromUTF8(utf8) == U"渋三次元");

	CHECK(Unicode::FromUTF16(u"") == U"");
	CHECK(Unicode::FromUTF16(u"abc!#@") == U"abc!#@");
	CHECK(Unicode::FromUTF16(u"渋三次元") == U"渋三次元");

	CHECK(Unicode::FromUTF32(U"") == U"");
	CHECK(Unicode::FromUTF32(U"abc!#@") == U"abc!#@");
	CHECK(Unicode::FromUTF32(U"渋三次元") == U"渋三次元");

	CHECK(Unicode::NarrowAscii(U"") == "");
	CHECK(Unicode::NarrowAscii(U"abc!#@") == "abc!#@");
	CHECK(Unicode::NarrowAscii(U"渋三次元") == "");

	CHECK(Unicode::Narrow(U"") == "");
	CHECK(Unicode::Narrow(U"abc!#@") == "abc!#@");
	CHECK(Unicode::Narrow(U"渋三次元") == "渋三次元");

	CHECK(Unicode::ToWstring(U"") == L"");
	CHECK(Unicode::ToWstring(U"abc!#@") == L"abc!#@");
	CHECK(Unicode::ToWstring(U"渋三次元") == L"渋三次元");

	CHECK(Unicode::ToUTF8(U"") == "");
	CHECK(Unicode::ToUTF8(U"abc!#@") == "abc!#@");
	CHECK(Unicode::ToUTF8(U"渋三次元") == utf8);

	CHECK(Unicode::ToUTF16(U"") == u"");
	CHECK(Unicode::ToUTF16(U"abc!#@") == u"abc!#@");
	CHECK(Unicode::ToUTF16(U"渋三次元") == u"渋三次元");

	CHECK(Unicode::ToUTF32(U"") == U"");
	CHECK(Unicode::ToUTF32(U"abc!#@") == U"abc!#@");
	CHECK(Unicode::ToUTF32(U"渋三次元") == U"渋三次元");

	CHECK(Unicode::UTF8ToUTF16("") == u"");
	CHECK(Unicode::UTF8ToUTF16("abc!#@") == u"abc!#@");
	CHECK(Unicode::UTF8ToUTF16(utf8) == u"渋三次元");

	CHECK(Unicode::UTF8ToUTF32("") == U"");
	CHECK(Unicode::UTF8ToUTF32("abc!#@") == U"abc!#@");
	CHECK(Unicode::UTF8ToUTF32(utf8) == U"渋三次元");

	CHECK(Unicode::UTF16ToUTF8(u"") == "");
	CHECK(Unicode::UTF16ToUTF8(u"abc!#@") == "abc!#@");
	CHECK(Unicode::UTF16ToUTF8(u"渋三次元") == utf8);

	CHECK(Unicode::UTF16ToUTF32(u"") == U"");
	CHECK(Unicode::UTF16ToUTF32(u"abc!#@") == U"abc!#@");
	CHECK(Unicode::UTF16ToUTF32(u"渋三次元") == U"渋三次元");

	CHECK(Unicode::UTF32ToUTF8(U"") == "");
	CHECK(Unicode::UTF32ToUTF8(U"abc!#@") == "abc!#@");
	CHECK(Unicode::UTF32ToUTF8(U"渋三次元") == utf8);

	CHECK(Unicode::UTF32ToUTF16(U"") == u"");
	CHECK(Unicode::UTF32ToUTF16(U"abc!#@") == u"abc!#@");
	CHECK(Unicode::UTF32ToUTF16(U"渋三次元") == u"渋三次元");

	CHECK(Unicode::IsHighSurrogate(u'a') == false);
	CHECK(Unicode::IsHighSurrogate(u'あ') == false);
	CHECK(Unicode::IsHighSurrogate(u"𩸽"[0]) == true);
	CHECK(Unicode::IsHighSurrogate(u"𩸽"[1]) == false);

	CHECK(Unicode::IsLowSurrogate(u'a') == false);
	CHECK(Unicode::IsLowSurrogate(u'あ') == false);
	CHECK(Unicode::IsLowSurrogate(u"𩸽"[0]) == false);
	CHECK(Unicode::IsLowSurrogate(u"𩸽"[1]) == true);

	{
		const std::string asciiVeryShort = "Siv3D";
		const std::string asciiShort = "Lorem ipsum dolor sit amet";
		const std::string asciiLong = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("Unicode::WidenAscii() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiVeryShort)); });
		Bench{}.run("Unicode::WidenAscii() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiShort)); });
		Bench{}.run("Unicode::WidenAscii() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiLong)); });

		Bench{}.run("Unicode::Widen() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::Widen(asciiVeryShort)); });
		Bench{}.run("Unicode::Widen() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::Widen(asciiShort)); });
		Bench{}.run("Unicode::Widen() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::Widen(asciiLong)); });
	}
}

void TestPlatform()
{
	Console << U"SIV3D_PLATFORM_NAME: "	SIV3D_PLATFORM_NAME;
	Console << U"SIV3D_CPU_ARCHITECTURE: " SIV3D_CPU_ARCHITECTURE;
	Console << U"SIV3D_INTRINSIC_TYPE: " SIV3D_INTRINSIC_TYPE;
	Console << U"SIV3D_COMPILER_NAME: " SIV3D_COMPILER_NAME;
	Console << U"SIV3D_BUILD_TYPE: " SIV3D_BUILD_TYPE;
}
