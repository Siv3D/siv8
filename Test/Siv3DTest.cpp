//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
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
	Console << U"SIV3D_PLATFORM_NAME: "	SIV3D_PLATFORM_NAME;
	Console << U"SIV3D_CPU_ARCHITECTURE: " SIV3D_CPU_ARCHITECTURE;
	Console << U"SIV3D_INTRINSIC_TYPE: " SIV3D_INTRINSIC_TYPE;
	Console << U"SIV3D_COMPILER_NAME: " SIV3D_COMPILER_NAME;
	Console << U"SIV3D_BUILD_TYPE: " SIV3D_BUILD_TYPE;
	Console << U"SIV3D_USE_MIMALLOC: " << SIV3D_USE_MIMALLOC;
}

TEST_CASE("Version.hpp")
{
	Console << U"SIV3D_VERSION_MAJOR: " << SIV3D_VERSION_MAJOR;
	Console << U"SIV3D_VERSION_MINOR: " << SIV3D_VERSION_MINOR;
	Console << U"SIV3D_VERSION_REVISION: " << SIV3D_VERSION_REVISION;
	Console << U"SIV3D_VERSION_PRERELEASE_TAG: " << SIV3D_VERSION_PRERELEASE_TAG;
	Console << U"SIV3D_VERSION: " << SIV3D_VERSION;
	Console << U"SIV3D_VERSION_SHORT_STRING: "	SIV3D_VERSION_SHORT_STRING;
	Console << U"SIV3D_VERSION_STRING: " SIV3D_VERSION_STRING;
	Console << U"SIV3D_VERSION_NAME: " SIV3D_VERSION_NAME;
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

TEST_CASE("Concepts.hpp")
{
	enum EnumT {};
	enum class ScopedEnumT {};

	CHECK(Concept::Integral<bool> == true);
	CHECK(Concept::Integral<uint8> == true);
	CHECK(Concept::Integral<int32> == true);
	CHECK(Concept::Integral<float> == false);
	CHECK(Concept::Integral<std::string> == false);
	CHECK(Concept::Integral<EnumT> == false);
	CHECK(Concept::Integral<ScopedEnumT> == false);

	CHECK(Concept::SignedIntegral<bool> == false);
	CHECK(Concept::SignedIntegral<uint8> == false);
	CHECK(Concept::SignedIntegral<int32> == true);
	CHECK(Concept::SignedIntegral<float> == false);
	CHECK(Concept::SignedIntegral<std::string> == false);
	CHECK(Concept::SignedIntegral<EnumT> == false);
	CHECK(Concept::SignedIntegral<ScopedEnumT> == false);

	CHECK(Concept::UnsignedIntegral<bool> == true);
	CHECK(Concept::UnsignedIntegral<uint8> == true);
	CHECK(Concept::UnsignedIntegral<int32> == false);
	CHECK(Concept::UnsignedIntegral<float> == false);
	CHECK(Concept::UnsignedIntegral<std::string> == false);
	CHECK(Concept::UnsignedIntegral<EnumT> == false);
	CHECK(Concept::UnsignedIntegral<ScopedEnumT> == false);

	CHECK(Concept::FloatingPoint<bool> == false);
	CHECK(Concept::FloatingPoint<uint8> == false);
	CHECK(Concept::FloatingPoint<int32> == false);
	CHECK(Concept::FloatingPoint<float> == true);
	CHECK(Concept::FloatingPoint<std::string> == false);
	CHECK(Concept::FloatingPoint<EnumT> == false);
	CHECK(Concept::FloatingPoint<ScopedEnumT> == false);

	CHECK(Concept::Signed<bool> == false);
	CHECK(Concept::Signed<uint8> == false);
	CHECK(Concept::Signed<int32> == true);
	CHECK(Concept::Signed<float> == true);
	CHECK(Concept::Signed<std::string> == false);
	CHECK(Concept::Signed<EnumT> == false);
	CHECK(Concept::Signed<ScopedEnumT> == false);

	CHECK(Concept::Arithmetic<bool> == true);
	CHECK(Concept::Arithmetic<uint8> == true);
	CHECK(Concept::Arithmetic<int32> == true);
	CHECK(Concept::Arithmetic<float> == true);
	CHECK(Concept::Arithmetic<std::string> == false);
	CHECK(Concept::Arithmetic<EnumT> == false);
	CHECK(Concept::Arithmetic<ScopedEnumT> == false);

	CHECK(Concept::Scalar<bool> == true);
	CHECK(Concept::Scalar<uint8> == true);
	CHECK(Concept::Scalar<int32> == true);
	CHECK(Concept::Scalar<float> == true);
	CHECK(Concept::Scalar<std::string> == false);
	CHECK(Concept::Scalar<EnumT> == true);
	CHECK(Concept::Scalar<ScopedEnumT> == true);

	CHECK(Concept::Enum<bool> == false);
	CHECK(Concept::Enum<uint8> == false);
	CHECK(Concept::Enum<int32> == false);
	CHECK(Concept::Enum<float> == false);
	CHECK(Concept::Enum<std::string> == false);
	CHECK(Concept::Enum<EnumT> == true);
	CHECK(Concept::Enum<ScopedEnumT> == true);

	CHECK(Concept::ScopedEnum<bool> == false);
	CHECK(Concept::ScopedEnum<uint8> == false);
	CHECK(Concept::ScopedEnum<int32> == false);
	CHECK(Concept::ScopedEnum<float> == false);
	CHECK(Concept::ScopedEnum<std::string> == false);
	CHECK(Concept::ScopedEnum<EnumT> == false);
	CHECK(Concept::ScopedEnum<ScopedEnumT> == true);

	CHECK(Concept::TriviallyCopyable<bool> == true);
	CHECK(Concept::TriviallyCopyable<uint8> == true);
	CHECK(Concept::TriviallyCopyable<int32> == true);
	CHECK(Concept::TriviallyCopyable<float> == true);
	CHECK(Concept::TriviallyCopyable<std::string> == false);
	CHECK(Concept::TriviallyCopyable<EnumT> == true);
	CHECK(Concept::TriviallyCopyable<ScopedEnumT> == true);

	CHECK(Concept::UniformRandomBitGenerator<bool> == false);
	CHECK(Concept::UniformRandomBitGenerator<uint8> == false);
	CHECK(Concept::UniformRandomBitGenerator<int32> == false);
	CHECK(Concept::UniformRandomBitGenerator<float> == false);
	CHECK(Concept::UniformRandomBitGenerator<std::string> == false);
	CHECK(Concept::UniformRandomBitGenerator<EnumT> == false);
	CHECK(Concept::UniformRandomBitGenerator<ScopedEnumT> == false);
}

TEST_CASE("Utility.hpp")
{
	CHECK(Min(-5, 5) == -5);
	CHECK(Min(5, -5) == -5);
	CHECK(Min(String(U"Aaa"), String(U"Bbb")) == U"Aaa");
	CHECK(Min(String(U"Bbb"), String(U"Aaa")) == U"Aaa");
	CHECK(Min<String>(U"Aaa", U"Bbb") == U"Aaa");
	CHECK(Min<String>(U"Bbb", U"Aaa") == U"Aaa");

	CHECK(Max(-5, 5) == 5);
	CHECK(Max(5, -5) == 5);
	CHECK(Max(String(U"Aaa"), String(U"Bbb")) == U"Bbb");
	CHECK(Max(String(U"Bbb"), String(U"Aaa")) == U"Bbb");
	CHECK(Max<String>(U"Aaa", U"Bbb") == U"Bbb");
	CHECK(Max<String>(U"Bbb", U"Aaa") == U"Bbb");

	CHECK(Clamp(-5, 0, 10) == 0);
	CHECK(Clamp(0, 0, 10) == 0);
	CHECK(Clamp(5, 0, 10) == 5);
	CHECK(Clamp(10, 0, 10) == 10);
	CHECK(Clamp(15, 0, 10) == 10);
	CHECK(Clamp(String(U"A"), String(U"Aaa"), String(U"Bbb")) == U"Aaa");
	CHECK(Clamp(String(U"Abb"), String(U"Aaa"), String(U"Bbb")) == U"Abb");
	CHECK(Clamp(String(U"Ccc"), String(U"Aaa"), String(U"Bbb")) == U"Bbb");
	CHECK(Clamp<String>(U"A", U"Aaa", U"Bbb") == U"Aaa");
	CHECK(Clamp<String>(U"Abb", U"Aaa", U"Bbb") == U"Abb");
	CHECK(Clamp<String>(U"Ccc", U"Aaa", U"Bbb") == U"Bbb");

	CHECK(InRange(-5, 0, 10) == false);
	CHECK(InRange(-1, 0, 10) == false);
	CHECK(InRange(0, 0, 10) == true);
	CHECK(InRange(1, 0, 10) == true);
	CHECK(InRange(5, 0, 10) == true);
	CHECK(InRange(9, 0, 10) == true);
	CHECK(InRange(10, 0, 10) == true);
	CHECK(InRange(11, 0, 10) == false);
	CHECK(InRange(15, 0, 10) == false);

	CHECK(InOpenRange(-5, 0, 10) == false);
	CHECK(InOpenRange(-1, 0, 10) == false);
	CHECK(InOpenRange(0, 0, 10) == false);
	CHECK(InOpenRange(1, 0, 10) == true);
	CHECK(InOpenRange(5, 0, 10) == true);
	CHECK(InOpenRange(9, 0, 10) == true);
	CHECK(InOpenRange(10, 0, 10) == false);
	CHECK(InOpenRange(11, 0, 10) == false);
	CHECK(InOpenRange(15, 0, 10) == false);

	{
		int32 a = rand(), b = rand();
		Bench{}.title("Utility").run("utility - std", [&]() { doNotOptimizeAway(std::max(std::clamp(a, 0, 10), std::clamp(b, 0, 10))); });
		Bench{}.title("Utility").run("utility - s3d", [&]() { doNotOptimizeAway(Max(Clamp(a, 0, 10), Clamp(b, 0, 10))); });
	}
}


TEST_CASE("Hash.hpp")
{
	CHECK(std::hash<StringView>{}(U"") != std::hash<StringView>{}(U"S"));
	CHECK(std::hash<StringView>{}(U"S") != std::hash<StringView>{}(U"Si"));
	CHECK(std::hash<StringView>{}(U"Si") != std::hash<StringView>{}(U"Siv"));
	CHECK(std::hash<StringView>{}(U"Siv") != std::hash<StringView>{}(U"Siv3"));
	CHECK(std::hash<StringView>{}(U"Siv3") != std::hash<StringView>{}(U"Siv3D"));
	CHECK(std::hash<StringView>{}(U"Siv3D") != std::hash<StringView>{}(U""));

	CHECK(std::hash<StringView>{}(U"") == std::hash<String>{}(U""));
	CHECK(std::hash<StringView>{}(U"Siv3D") == std::hash<String>{}(U"Siv3D"));

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

	{
		const StringView sShort = U"Siv3D";
		const StringView sMedium = U"Hello, Siv3D!";
		const StringView sLong = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.title("Hash").run("std::hash - std::u32string_view short", [&]() { doNotOptimizeAway(std::hash<std::u32string_view>{}(sShort.view())); });
		Bench{}.title("Hash").run("std::hash - std::u32string_view medium", [&]() { doNotOptimizeAway(std::hash<std::u32string_view>{}(sMedium.view())); });
		Bench{}.title("Hash").run("std::hash - std::u32string_view long", [&]() { doNotOptimizeAway(std::hash<std::u32string_view>{}(sLong.view())); });

		Bench{}.title("Hash").run("wyhash::Hash - StringView short", [&]() { doNotOptimizeAway(wyhash::Hash(sShort.data(), sShort.size_bytes())); });
		Bench{}.title("Hash").run("wyhash::Hash - StringView medium", [&]() { doNotOptimizeAway(wyhash::Hash(sMedium.data(), sMedium.size_bytes())); });
		Bench{}.title("Hash").run("wyhash::Hash - StringView long", [&]() { doNotOptimizeAway(wyhash::Hash(sLong.data(), sLong.size_bytes())); });

		Bench{}.title("Hash").run("xxHash3::Hash - StringView short", [&]() { doNotOptimizeAway(xxHash3::Hash(sShort.data(), sShort.size_bytes())); });
		Bench{}.title("Hash").run("xxHash3::Hash - StringView medium", [&]() { doNotOptimizeAway(xxHash3::Hash(sMedium.data(), sMedium.size_bytes())); });
		Bench{}.title("Hash").run("xxHash3::Hash - StringView long", [&]() { doNotOptimizeAway(xxHash3::Hash(sLong.data(), sLong.size_bytes())); });

		Bench{}.title("Hash").run("Hash - StringView short", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sShort)); });
		Bench{}.title("Hash").run("Hash - StringView medium", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sMedium)); });
		Bench{}.title("Hash").run("Hash - StringView long", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sLong)); });
	}

	{
		const String sShort = U"Siv3D";
		const String sMedium = U"Hello, Siv3D!";
		const String sLong = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.title("Hash").run("Hash - String short", [&]() { doNotOptimizeAway(std::hash<String>{}(sShort)); });
		Bench{}.title("Hash").run("Hash - String medium", [&]() { doNotOptimizeAway(std::hash<String>{}(sMedium)); });
		Bench{}.title("Hash").run("Hash - String long", [&]() { doNotOptimizeAway(std::hash<String>{}(sLong)); });
	}
}

TEST_CASE("Unicode.hpp")
{
	const std::string utf8 = Unicode::ToUTF8(U"渋三次元");

	CHECK(Unicode::WidenAscii("") == U"");
	CHECK(Unicode::WidenAscii("abc!#@") == U"abc!#@");
	CHECK(Unicode::WidenAscii("渋三次元") == U"");

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

		const String ascii32VeryShort = U"Siv3D";
		const String ascii32Short = U"Lorem ipsum dolor sit amet";
		const String ascii32Long = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.title("Unicode").run("Unicode::WidenAscii() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiVeryShort)); });
		Bench{}.title("Unicode").run("Unicode::WidenAscii() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiShort)); });
		Bench{}.title("Unicode").run("Unicode::WidenAscii() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiLong)); });

		Bench{}.title("Unicode").run("Unicode::FromUTF8() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::FromUTF8(asciiVeryShort)); });
		Bench{}.title("Unicode").run("Unicode::FromUTF8() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::FromUTF8(asciiShort)); });
		Bench{}.title("Unicode").run("Unicode::FromUTF8() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::FromUTF8(asciiLong)); });

		Bench{}.title("Unicode").run("Unicode::NarrowAscii() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::NarrowAscii(ascii32VeryShort)); });
		Bench{}.title("Unicode").run("Unicode::NarrowAscii() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::NarrowAscii(ascii32Short)); });
		Bench{}.title("Unicode").run("Unicode::NarrowAscii() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::NarrowAscii(ascii32Long)); });

		Bench{}.title("Unicode").run("Unicode::ToUTF8() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::ToUTF8(ascii32VeryShort)); });
		Bench{}.title("Unicode").run("Unicode::ToUTF8() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::ToUTF8(ascii32Short)); });
		Bench{}.title("Unicode").run("Unicode::ToUTF8() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::ToUTF8(ascii32Long)); });
	}
}


TEST_CASE("Parse.hpp")
{
	{
		CHECK(ParseBoolChecked(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseBoolChecked(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseBoolChecked(U"0") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"1") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U" 0") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"1 ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"t") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"tru") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"tru ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"truee ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"f") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"fals") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"fals ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseBoolChecked(U"falsee ") == Unexpected{ ParseErrorReason::InvalidFormat });

		CHECK(ParseBoolChecked(U"false") == false);
		CHECK(ParseBoolChecked(U"false ") == false);
		CHECK(ParseBoolChecked(U"  false ") == false);
		CHECK(ParseBoolChecked(U"true") == true);
		CHECK(ParseBoolChecked(U"true ") == true);
		CHECK(ParseBoolChecked(U"  true ") == true);

		CHECK(ParseBoolChecked(U"False") == false);
		CHECK(ParseBoolChecked(U"False ") == false);
		CHECK(ParseBoolChecked(U"  False ") == false);
		CHECK(ParseBoolChecked(U"True") == true);
		CHECK(ParseBoolChecked(U"True ") == true);
		CHECK(ParseBoolChecked(U"  True ") == true);

		CHECK(ParseBoolChecked(U"FALSE") == false);
		CHECK(ParseBoolChecked(U"FALSE ") == false);
		CHECK(ParseBoolChecked(U"  FALSE ") == false);
		CHECK(ParseBoolChecked(U"TRUE") == true);
		CHECK(ParseBoolChecked(U"TRUE ") == true);
		CHECK(ParseBoolChecked(U"  TRUE ") == true);
	}

	{
		CHECK(ParseIntChecked<int8>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<int8>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<int8>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"0") == 0);
		CHECK(ParseIntChecked<int8>(U"-0") == 0);
		CHECK(ParseIntChecked<int8>(U"00") == 0);
		CHECK(ParseIntChecked<int8>(U"-00") == 0);
		CHECK(ParseIntChecked<int8>(U"1") == 1);
		CHECK(ParseIntChecked<int8>(U" 1 ") == 1);
		CHECK(ParseIntChecked<int8>(U"-1") == -1);
		CHECK(ParseIntChecked<int8>(U" -1 ") == -1);
		CHECK(ParseIntChecked<int8>(U" - 1 ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"-127") == -127);
		CHECK(ParseIntChecked<int8>(U" -128 ") == -128);
		CHECK(ParseIntChecked<int8>(U"128") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"-129") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"1270") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"-9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"0", 1) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<int8>(U"0", 37) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<int8>(U"7F", 16) == 0x7F);
		CHECK(ParseIntChecked<int8>(U"-7F", 16) == -127);
		CHECK(ParseIntChecked<int8>(U"81", 16) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"-81", 16) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"2", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"011", 2) == 3);
		CHECK(ParseIntChecked<int8>(U"01015", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int8>(U"011010101010101011", 2) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"0x7F", 0) == 0x7F);
		CHECK(ParseIntChecked<int8>(U"0x81", 0) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int8>(U"", 0) == Unexpected{ ParseErrorReason::EmptyInput });
	}

	{
		CHECK(ParseIntChecked<uint8>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<uint8>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<uint8>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U"0") == 0);
		CHECK(ParseIntChecked<uint8>(U"-0") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"00") == 0);
		CHECK(ParseIntChecked<uint8>(U"-00") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"1") == 1);
		CHECK(ParseIntChecked<uint8>(U" 1 ") == 1);
		CHECK(ParseIntChecked<uint8>(U"-1") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U" -1 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U" - 1 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"-127") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U" -128 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"128") == 128);
		CHECK(ParseIntChecked<uint8>(U"-129") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"255") == 255);
		CHECK(ParseIntChecked<uint8>(U"256") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint8>(U"2550") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint8>(U"9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint8>(U"-9999999999999999999999999") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"0", 1) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<uint8>(U"0", 37) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<uint8>(U"7F", 16) == 0x7F);
		CHECK(ParseIntChecked<uint8>(U"-7F", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"81", 16) == 0x81);
		CHECK(ParseIntChecked<uint8>(U"-81", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"FF", 16) == 0xFF);
		CHECK(ParseIntChecked<uint8>(U"-FF", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint8>(U"2", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U"011", 2) == 3);
		CHECK(ParseIntChecked<uint8>(U"01015", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint8>(U"011010101010101011", 2) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint8>(U"", 0) == Unexpected{ ParseErrorReason::EmptyInput });
	}

	{
		CHECK(ParseIntChecked<int32>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<int32>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<int32>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"0") == 0);
		CHECK(ParseIntChecked<int32>(U"-0") == 0);
		CHECK(ParseIntChecked<int32>(U"00") == 0);
		CHECK(ParseIntChecked<int32>(U"-00") == 0);
		CHECK(ParseIntChecked<int32>(U"1") == 1);
		CHECK(ParseIntChecked<int32>(U" 1 ") == 1);
		CHECK(ParseIntChecked<int32>(U"-1") == -1);
		CHECK(ParseIntChecked<int32>(U" -1 ") == -1);
		CHECK(ParseIntChecked<int32>(U" - 1 ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"-127") == -127);
		CHECK(ParseIntChecked<int32>(U" -128 ") == -128);
		CHECK(ParseIntChecked<int32>(U"2147483647") == 2147483647);
		CHECK(ParseIntChecked<int32>(U"-2147483648") == -2147483648);
		CHECK(ParseIntChecked<int32>(U"2147483648") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int32>(U"-2147483649") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int32>(U"9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int32>(U"-9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int32>(U"0", 1) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<int32>(U"0", 37) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<int32>(U"7F", 16) == 0x7F);
		CHECK(ParseIntChecked<int32>(U"-7F", 16) == -127);
		CHECK(ParseIntChecked<int32>(U"81", 16) == 0x81);
		CHECK(ParseIntChecked<int32>(U"-81", 16) == -0x81);
		CHECK(ParseIntChecked<int32>(U"2", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"011", 2) == 3);
		CHECK(ParseIntChecked<int32>(U"01015", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int32>(U"011010101010101011011010101010101011", 2) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int32>(U"0x7F", 0) == 0x7F);
		CHECK(ParseIntChecked<int32>(U"0x81FF11FF11", 0) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int32>(U"", 0) == Unexpected{ ParseErrorReason::EmptyInput });
	}

	{
		CHECK(ParseIntChecked<uint32>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<uint32>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<uint32>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U"0") == 0);
		CHECK(ParseIntChecked<uint32>(U"-0") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"00") == 0);
		CHECK(ParseIntChecked<uint32>(U"-00") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"1") == 1);
		CHECK(ParseIntChecked<uint32>(U" 1 ") == 1);
		CHECK(ParseIntChecked<uint32>(U"-1") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U" -1 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U" - 1 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"-127") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U" -128 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"4294967295") == 4294967295u);
		CHECK(ParseIntChecked<uint32>(U"4294967296") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint32>(U"9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint32>(U"-9999999999999999999999999") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"0", 1) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<uint32>(U"0", 37) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<uint32>(U"7F", 16) == 0x7F);
		CHECK(ParseIntChecked<uint32>(U"-7F", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"81", 16) == 0x81);
		CHECK(ParseIntChecked<uint32>(U"-81", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"FF", 16) == 0xFF);
		CHECK(ParseIntChecked<uint32>(U"-FF", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint32>(U"2", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U"011", 2) == 3);
		CHECK(ParseIntChecked<uint32>(U"01015", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint32>(U"011010101010101011011010101010101011", 2) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint32>(U"0x7F", 0) == 0x7F);
		CHECK(ParseIntChecked<uint32>(U"0x81FF11FF11", 0) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint32>(U"", 0) == Unexpected{ ParseErrorReason::EmptyInput });
	}

	{
		CHECK(ParseIntChecked<int64>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<int64>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<int64>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"0") == 0);
		CHECK(ParseIntChecked<int64>(U"-0") == 0);
		CHECK(ParseIntChecked<int64>(U"00") == 0);
		CHECK(ParseIntChecked<int64>(U"-00") == 0);
		CHECK(ParseIntChecked<int64>(U"1") == 1);
		CHECK(ParseIntChecked<int64>(U" 1 ") == 1);
		CHECK(ParseIntChecked<int64>(U"-1") == -1);
		CHECK(ParseIntChecked<int64>(U" -1 ") == -1);
		CHECK(ParseIntChecked<int64>(U" - 1 ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"-127") == -127);
		CHECK(ParseIntChecked<int64>(U" -128 ") == -128);
		CHECK(ParseIntChecked<int64>(U"9223372036854775807") == 9223372036854775807LL);
		CHECK(ParseIntChecked<int64>(U"-9223372036854775808") == INT64_MIN);
		CHECK(ParseIntChecked<int64>(U"9223372036854775808") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int64>(U"-9223372036854775809") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int64>(U"9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int64>(U"-9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int64>(U"0", 1) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<int64>(U"0", 37) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<int64>(U"7F", 16) == 0x7F);
		CHECK(ParseIntChecked<int64>(U"-7F", 16) == -127);
		CHECK(ParseIntChecked<int64>(U"81", 16) == 0x81);
		CHECK(ParseIntChecked<int64>(U"-81", 16) == -0x81);
		CHECK(ParseIntChecked<int64>(U"2", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"011", 2) == 3);
		CHECK(ParseIntChecked<int64>(U"01015", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<int64>(U"011010101010101011011010101010101011011010101010101011011010101010101011", 2) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int64>(U"0x7F", 0) == 0x7F);
		CHECK(ParseIntChecked<int64>(U"0x81FF11FF1181FF11FF11", 0) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<int64>(U"", 0) == Unexpected{ ParseErrorReason::EmptyInput });
	}

	{
		CHECK(ParseIntChecked<uint64>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<uint64>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseIntChecked<uint64>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U"0") == 0);
		CHECK(ParseIntChecked<uint64>(U"-0") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"00") == 0);
		CHECK(ParseIntChecked<uint64>(U"-00") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"1") == 1);
		CHECK(ParseIntChecked<uint64>(U" 1 ") == 1);
		CHECK(ParseIntChecked<uint64>(U"-1") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U" -1 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U" - 1 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"-127") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U" -128 ") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"18446744073709551615") == 18446744073709551615ULL);
		CHECK(ParseIntChecked<uint64>(U"18446744073709551616") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint64>(U"9999999999999999999999999") == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint64>(U"-9999999999999999999999999") == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"0", 1) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<uint64>(U"0", 37) == Unexpected{ ParseErrorReason::InvalidRadix });
		CHECK(ParseIntChecked<uint64>(U"7F", 16) == 0x7F);
		CHECK(ParseIntChecked<uint64>(U"-7F", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"81", 16) == 0x81);
		CHECK(ParseIntChecked<uint64>(U"-81", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"FF", 16) == 0xFF);
		CHECK(ParseIntChecked<uint64>(U"-FF", 16) == Unexpected{ ParseErrorReason::InvalidValueForType });
		CHECK(ParseIntChecked<uint64>(U"2", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U"011", 2) == 3);
		CHECK(ParseIntChecked<uint64>(U"01015", 2) == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseIntChecked<uint64>(U"011010101010101011011010101010101011011010101010101011011010101010101011", 2) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint64>(U"0x7F", 0) == 0x7F);
		CHECK(ParseIntChecked<uint64>(U"0x81FF11FF1181FF11FF11", 0) == Unexpected{ ParseErrorReason::NumericOverflow });
		CHECK(ParseIntChecked<uint64>(U"", 0) == Unexpected{ ParseErrorReason::EmptyInput });
	}

	{
		CHECK(ParseFloatChecked<float>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseFloatChecked<float>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseFloatChecked<float>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U"1.2.3") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U"-1.2.3") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<float>(U"0") == 0.0f);
		CHECK(ParseFloatChecked<float>(U"-0") == -0.0f);
		CHECK(ParseFloatChecked<float>(U"00") == 0.0f);
		CHECK(ParseFloatChecked<float>(U"-00") == -0.0f);
		CHECK(ParseFloatChecked<float>(U"1") == 1.0f);
		CHECK(ParseFloatChecked<float>(U" 1 ") == 1.0f);
		CHECK(ParseFloatChecked<float>(U"-1") == -1.0f);
		CHECK(ParseFloatChecked<float>(U" -1 ") == -1.0f);
		CHECK(ParseFloatChecked<float>(U" .25 ") == 0.25f);
		CHECK(ParseFloatChecked<float>(U"-.25") == -0.25f);
		CHECK(ParseFloatChecked<float>(U" -.25 ") == -0.25f);
		CHECK(ParseFloatChecked<float>(U" 3.14159265 ") == 3.14159265f);
		CHECK(ParseFloatChecked<float>(U"-3.14159265") == -3.14159265f);
		CHECK(ParseFloatChecked<float>(U" -3.14159265 ") == -3.14159265f);
		CHECK(ParseFloatChecked<float>(U" 1e5 ") == 1e5f);
		CHECK(ParseFloatChecked<float>(U"-1e5") == -1e5f);
		CHECK(ParseFloatChecked<float>(U" -1e5 ") == -1e5f);
		CHECK(ParseFloatChecked<float>(U" 0.00000000001 ") == 0.00000000001f); 
		CHECK(ParseFloatChecked<float>(U"-0.00000000001") == -0.00000000001f);
		CHECK(ParseFloatChecked<float>(U" -0.00000000001 ") == -0.00000000001f);
		CHECK(ParseFloatChecked<float>(U" 340282346638528859811704183484516925440 ") == FLT_MAX);
		CHECK(ParseFloatChecked<float>(U"-340282346638528859811704183484516925440") == -FLT_MAX);
		CHECK(ParseFloatChecked<float>(U" -340282346638528859811704183484516925440 ") == -FLT_MAX);
		CHECK(std::isinf(ParseFloatChecked<float>(U" 350282346638528859811704183484516925440 ").value()));
		CHECK(std::isinf(ParseFloatChecked<float>(U"-350282346638528859811704183484516925440").value()));
		CHECK(std::isinf(ParseFloatChecked<float>(U" -350282346638528859811704183484516925440 ").value()));
		CHECK(ParseFloatChecked<float>(U" 350282346638528859811704183484516925440 ").value() > 0.0f);
		CHECK(ParseFloatChecked<float>(U"-350282346638528859811704183484516925440").value() < 0.0f);
		CHECK(ParseFloatChecked<float>(U" -350282346638528859811704183484516925440 ").value() < 0.0f);
		CHECK(ParseFloatChecked<float>(U" 0.00000000000000000000000000000000000001175494351 ") == FLT_MIN);
		CHECK(ParseFloatChecked<float>(U"-0.00000000000000000000000000000000000001175494351") == -FLT_MIN);
		CHECK(ParseFloatChecked<float>(U" -0.00000000000000000000000000000000000001175494351 ") == -FLT_MIN);
		CHECK(ParseFloatChecked<float>(U" 0.000000000000000000000000000000000000000000001401298464 ") == FLT_TRUE_MIN);
		CHECK(ParseFloatChecked<float>(U"-0.000000000000000000000000000000000000000000001401298464") == -FLT_TRUE_MIN);
		CHECK(ParseFloatChecked<float>(U" -0.000000000000000000000000000000000000000000001401298464 ") == -FLT_TRUE_MIN);
		CHECK(ParseFloatChecked<float>(U" 0.000000000000000000000000000000000000000000000601298464 ") == 0.0f);
		CHECK(ParseFloatChecked<float>(U"-0.000000000000000000000000000000000000000000000601298464") == 0.0f);
		CHECK(ParseFloatChecked<float>(U" -0.000000000000000000000000000000000000000000000601298464 ") == 0.0f);

		CHECK(std::isinf(ParseFloatChecked<float>(U"inf").value()));
		CHECK(std::isinf(ParseFloatChecked<float>(U" inf").value()));
		CHECK(std::isinf(ParseFloatChecked<float>(U" inf ").value()));
		CHECK(ParseFloatChecked<float>(U" inf ").value() > 0.0f);
		CHECK(std::isinf(ParseFloatChecked<float>(U"-inf").value()));
		CHECK(std::isinf(ParseFloatChecked<float>(U" -inf ").value()));
		CHECK(ParseFloatChecked<float>(U" -inf ").value() < 0.0f);

		CHECK(std::isnan(ParseFloatChecked<float>(U"nan").value()));
		CHECK(std::isnan(ParseFloatChecked<float>(U" nan").value()));
		CHECK(std::isnan(ParseFloatChecked<float>(U" nan ").value()));
	}

	{
		CHECK(ParseFloatChecked<double>(U"") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseFloatChecked<double>(U" ") == Unexpected{ ParseErrorReason::EmptyInput });
		CHECK(ParseFloatChecked<double>(U"a") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U"x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U" x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U"2x ") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U"2 x") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U"1.2.3") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U"-1.2.3") == Unexpected{ ParseErrorReason::InvalidFormat });
		CHECK(ParseFloatChecked<double>(U"0") == 0.0);
		CHECK(ParseFloatChecked<double>(U"-0") == -0.0);
		CHECK(ParseFloatChecked<double>(U"00") == 0.0);
		CHECK(ParseFloatChecked<double>(U"-00") == -0.0);
		CHECK(ParseFloatChecked<double>(U"1") == 1.0);
		CHECK(ParseFloatChecked<double>(U" 1 ") == 1.0);
		CHECK(ParseFloatChecked<double>(U"-1") == -1.0);
		CHECK(ParseFloatChecked<double>(U" -1 ") == -1.0);
		CHECK(ParseFloatChecked<double>(U" .25 ") == 0.25);
		CHECK(ParseFloatChecked<double>(U"-.25") == -0.25);
		CHECK(ParseFloatChecked<double>(U" -.25 ") == -0.25);
		CHECK(ParseFloatChecked<double>(U" 3.14159265 ") == 3.14159265);
		CHECK(ParseFloatChecked<double>(U"-3.14159265") == -3.14159265);
		CHECK(ParseFloatChecked<double>(U" -3.14159265 ") == -3.14159265);
		CHECK(ParseFloatChecked<double>(U" 1e5 ") == 1e5);
		CHECK(ParseFloatChecked<double>(U"-1e5") == -1e5);
		CHECK(ParseFloatChecked<double>(U" -1e5 ") == -1e5);
		CHECK(ParseFloatChecked<double>(U" 0.00000000001 ") == 0.00000000001);
		CHECK(ParseFloatChecked<double>(U"-0.00000000001") == -0.00000000001);
		CHECK(ParseFloatChecked<double>(U" -0.00000000001 ") == -0.00000000001);
		CHECK(ParseFloatChecked<double>(U" 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368 ") == DBL_MAX);
		CHECK(ParseFloatChecked<double>(U"-179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368") == -DBL_MAX);
		CHECK(ParseFloatChecked<double>(U" -179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368 ") == -DBL_MAX);
		CHECK(std::isinf(ParseFloatChecked<double>(U" 179869313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368 ").value()));
		CHECK(std::isinf(ParseFloatChecked<double>(U"-179869313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368").value()));
		CHECK(std::isinf(ParseFloatChecked<double>(U" -179869313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368 ").value()));
		CHECK(ParseFloatChecked<double>(U" 179869313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368 ").value() > 0.0f);
		CHECK(ParseFloatChecked<double>(U"-179869313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368").value() < 0.0f);
		CHECK(ParseFloatChecked<double>(U" -179869313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368 ").value() < 0.0f);
		CHECK(ParseFloatChecked<double>(U" 0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002225073858507201383 ") == DBL_MIN);
		CHECK(ParseFloatChecked<double>(U"-0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002225073858507201383") == -DBL_MIN);
		CHECK(ParseFloatChecked<double>(U" -0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002225073858507201383 ") == -DBL_MIN);
		CHECK(ParseFloatChecked<double>(U" 0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000494065645841246544 ") == DBL_TRUE_MIN);
		CHECK(ParseFloatChecked<double>(U"-0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000494065645841246544") == -DBL_TRUE_MIN);
		CHECK(ParseFloatChecked<double>(U" -0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000494065645841246544 ") == -DBL_TRUE_MIN);
		CHECK(ParseFloatChecked<double>(U" 0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000204065645841246544 ") == 0.0);
		CHECK(ParseFloatChecked<double>(U"-0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000204065645841246544") == 0.0);
		CHECK(ParseFloatChecked<double>(U" -0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000204065645841246544 ") == 0.0);

		CHECK(std::isinf(ParseFloatChecked<double>(U"inf").value()));
		CHECK(std::isinf(ParseFloatChecked<double>(U" inf").value()));
		CHECK(std::isinf(ParseFloatChecked<double>(U" inf ").value()));
		CHECK(ParseFloatChecked<double>(U" inf ").value() > 0.0);
		CHECK(std::isinf(ParseFloatChecked<double>(U"-inf").value()));
		CHECK(std::isinf(ParseFloatChecked<double>(U" -inf ").value()));
		CHECK(ParseFloatChecked<double>(U" -inf ").value() < 0.0);

		CHECK(std::isnan(ParseFloatChecked<double>(U"nan").value()));
		CHECK(std::isnan(ParseFloatChecked<double>(U" nan").value()));
		CHECK(std::isnan(ParseFloatChecked<double>(U" nan ").value()));
	}
}


TEST_CASE("BinaryReader.hpp")
{
	FileSystem::Remove(U"test.bin");
	ScopeExit exit = [](){ FileSystem::Remove(U"test.bin"); };
	
	{
		BinaryReader reader;
		CHECK(reader.supportsLookahead());
		CHECK(not reader);
		CHECK(not reader.isOpen());
		CHECK(reader.path().isEmpty());
		CHECK(reader.size() == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.setPos(0) == 0);
		CHECK(reader.getPos() == 0);
		
		CHECK(reader.setPos(-10) == 0);
		CHECK(reader.getPos() == 0);
	
		CHECK(reader.setPos(20) == 0);
		CHECK(reader.getPos() == 0);
		
		CHECK(reader.skip(-10) == 0);
		CHECK(reader.getPos() == 0);
		
		CHECK(reader.skip(20) == 0);
		CHECK(reader.getPos() == 0);

		{
			int32 value = 123;
			CHECK(reader.read(&value, sizeof(value)) == 0);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK(reader.read(&value, 0, sizeof(value)) == 0);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK_THROWS_AS(reader.read(&value, -2, sizeof(value)), Error);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK_THROWS_AS(reader.read(&value, 2, sizeof(value)), Error);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK_THROWS_AS(reader.read(&value, 100, sizeof(value)), Error);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK(reader.read(value) == false);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);
		}

		{
			int32 value = 123;
			CHECK(reader.lookahead(&value, sizeof(value)) == 0);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK(reader.lookahead(&value, 0, sizeof(value)) == 0);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK_THROWS_AS(reader.lookahead(&value, -2, sizeof(value)), Error);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK_THROWS_AS(reader.lookahead(&value, 2, sizeof(value)), Error);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK_THROWS_AS(reader.lookahead(&value, 100, sizeof(value)), Error);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);

			CHECK(reader.lookahead(value) == false);
			CHECK(reader.getPos() == 0);
			CHECK(value == 123);
		}
	}

	{
		BinaryReader reader{ U"test.bin" };
		CHECK(not reader);
		CHECK(not reader.isOpen());
		CHECK(reader.path().isEmpty());
		CHECK(reader.size() == 0);
		CHECK(reader.getPos() == 0);
	}

	{
		const int32 Data = 123456789;
		{
			std::ofstream ofs{ "test.bin", std::ios::binary };
			ofs.write((const char*)&Data, sizeof(Data));
		}

		{
			BinaryReader reader{ U"test.bin" };
			CHECK(reader);
			CHECK(reader.isOpen());
			CHECK(reader.path() == FileSystem::FullPath(U"test.bin"));
			CHECK(reader.size() == 4);
			CHECK(reader.getPos() == 0);

			CHECK(reader.setPos(0) == 0);
			CHECK(reader.getPos() == 0);

			CHECK(reader.setPos(-10) == 0);
			CHECK(reader.getPos() == 0);

			CHECK(reader.setPos(2) == 2);
			CHECK(reader.getPos() == 2);

			CHECK(reader.setPos(4) == 4);
			CHECK(reader.getPos() == 4);

			CHECK(reader.setPos(5) == 4);
			CHECK(reader.getPos() == 4);

			CHECK(reader.setPos(20) == 4);
			CHECK(reader.getPos() == 4);

			CHECK(reader.skip(-10) == 0);
			CHECK(reader.getPos() == 0);

			CHECK(reader.skip(1) == 1);
			CHECK(reader.getPos() == 1);

			CHECK(reader.skip(2) == 3);
			CHECK(reader.getPos() == 3);

			CHECK(reader.skip(20) == 4);
			CHECK(reader.getPos() == 4);

			CHECK(reader.setPos(0) == 0);
			CHECK(reader.getPos() == 0);

			{
				int32 value = 123;
				CHECK(reader.read(&value, sizeof(value)) == 4);
				CHECK(reader.getPos() == 4);
				CHECK(value == Data);
				value = 0;

				CHECK(reader.read(&value, 0, sizeof(value)) == 4);
				CHECK(reader.getPos() == 4);
				CHECK(value == Data);
				value = 0;

				CHECK_THROWS_AS(reader.read(&value, -2, sizeof(value)), Error);
				CHECK(reader.getPos() == 4);
				CHECK(value == 0);

				CHECK(reader.read(&value, 2, sizeof(value)) == 2);
				CHECK(reader.getPos() == 4);
				CHECK(value == (Data >> 16));
				value = 0;

				CHECK_THROWS_AS(reader.read(&value, 100, sizeof(value)), Error);
				CHECK(reader.getPos() == 4);
				CHECK(value == 0);

				CHECK(reader.read(value) == false);
				CHECK(reader.getPos() == 4);
				CHECK(value == 0);

				CHECK(reader.setPos(0) == 0);
				CHECK(reader.read(value) == true);
				CHECK(reader.getPos() == 4);
				CHECK(value == Data);
			}
		}
	}
}
