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

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(4566)

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
}

TEST_CASE("Version.hpp")
{
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
	CHECK(Concept::Integral<String> == false);
	CHECK(Concept::Integral<EnumT> == false);
	CHECK(Concept::Integral<ScopedEnumT> == false);

	CHECK(Concept::SignedIntegral<bool> == false);
	CHECK(Concept::SignedIntegral<uint8> == false);
	CHECK(Concept::SignedIntegral<int32> == true);
	CHECK(Concept::SignedIntegral<float> == false);
	CHECK(Concept::SignedIntegral<String> == false);
	CHECK(Concept::SignedIntegral<EnumT> == false);
	CHECK(Concept::SignedIntegral<ScopedEnumT> == false);

	CHECK(Concept::UnsignedIntegral<bool> == true);
	CHECK(Concept::UnsignedIntegral<uint8> == true);
	CHECK(Concept::UnsignedIntegral<int32> == false);
	CHECK(Concept::UnsignedIntegral<float> == false);
	CHECK(Concept::UnsignedIntegral<String> == false);
	CHECK(Concept::UnsignedIntegral<EnumT> == false);
	CHECK(Concept::UnsignedIntegral<ScopedEnumT> == false);

	CHECK(Concept::FloatingPoint<bool> == false);
	CHECK(Concept::FloatingPoint<uint8> == false);
	CHECK(Concept::FloatingPoint<int32> == false);
	CHECK(Concept::FloatingPoint<float> == true);
	CHECK(Concept::FloatingPoint<String> == false);
	CHECK(Concept::FloatingPoint<EnumT> == false);
	CHECK(Concept::FloatingPoint<ScopedEnumT> == false);

	CHECK(Concept::Signed<bool> == false);
	CHECK(Concept::Signed<uint8> == false);
	CHECK(Concept::Signed<int32> == true);
	CHECK(Concept::Signed<float> == true);
	CHECK(Concept::Signed<String> == false);
	CHECK(Concept::Signed<EnumT> == false);
	CHECK(Concept::Signed<ScopedEnumT> == false);

	CHECK(Concept::Arithmetic<bool> == true);
	CHECK(Concept::Arithmetic<uint8> == true);
	CHECK(Concept::Arithmetic<int32> == true);
	CHECK(Concept::Arithmetic<float> == true);
	CHECK(Concept::Arithmetic<String> == false);
	CHECK(Concept::Arithmetic<EnumT> == false);
	CHECK(Concept::Arithmetic<ScopedEnumT> == false);

	CHECK(Concept::Scalar<bool> == true);
	CHECK(Concept::Scalar<uint8> == true);
	CHECK(Concept::Scalar<int32> == true);
	CHECK(Concept::Scalar<float> == true);
	CHECK(Concept::Scalar<String> == false);
	CHECK(Concept::Scalar<EnumT> == true);
	CHECK(Concept::Scalar<ScopedEnumT> == true);

	CHECK(Concept::NonScalar<bool> == false);
	CHECK(Concept::NonScalar<uint8> == false);
	CHECK(Concept::NonScalar<int32> == false);
	CHECK(Concept::NonScalar<float> == false);
	CHECK(Concept::NonScalar<String> == true);
	CHECK(Concept::NonScalar<EnumT> == false);
	CHECK(Concept::NonScalar<ScopedEnumT> == false);

	CHECK(Concept::Enum<bool> == false);
	CHECK(Concept::Enum<uint8> == false);
	CHECK(Concept::Enum<int32> == false);
	CHECK(Concept::Enum<float> == false);
	CHECK(Concept::Enum<String> == false);
	CHECK(Concept::Enum<EnumT> == true);
	CHECK(Concept::Enum<ScopedEnumT> == true);

	CHECK(Concept::ScopedEnum<bool> == false);
	CHECK(Concept::ScopedEnum<uint8> == false);
	CHECK(Concept::ScopedEnum<int32> == false);
	CHECK(Concept::ScopedEnum<float> == false);
	CHECK(Concept::ScopedEnum<String> == false);
	CHECK(Concept::ScopedEnum<EnumT> == false);
	CHECK(Concept::ScopedEnum<ScopedEnumT> == true);

	CHECK(Concept::TriviallyCopyable<bool> == true);
	CHECK(Concept::TriviallyCopyable<uint8> == true);
	CHECK(Concept::TriviallyCopyable<int32> == true);
	CHECK(Concept::TriviallyCopyable<float> == true);
	CHECK(Concept::TriviallyCopyable<String> == false);
	CHECK(Concept::TriviallyCopyable<EnumT> == true);
	CHECK(Concept::TriviallyCopyable<ScopedEnumT> == true);

	CHECK(Concept::UniformRandomBitGenerator<bool> == false);
	CHECK(Concept::UniformRandomBitGenerator<uint8> == false);
	CHECK(Concept::UniformRandomBitGenerator<int32> == false);
	CHECK(Concept::UniformRandomBitGenerator<float> == false);
	CHECK(Concept::UniformRandomBitGenerator<String> == false);
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
		int a = rand(), b = rand();
		Bench{}.run("utility - std", [&]() { doNotOptimizeAway(std::max(std::clamp(a, 0, 10), std::clamp(b, 0, 10))); });
		Bench{}.run("utility - s3d", [&]() { doNotOptimizeAway(Max(Clamp(a, 0, 10), Clamp(b, 0, 10))); });
	}
}

TEST_CASE("Memory.hpp")
{
	const auto StressTest1 = []()
	{
		std::vector<std::string> vs;

		for (size_t i = 0; i < 4096; ++i)
		{
			vs.push_back(std::string(32, 'a'));
		}

		return vs;
	};

	const auto StressTest2 = []()
	{
		std::vector<std::string> vs;

		for (size_t i = 0; i < 4096; ++i)
		{
			vs.push_back(std::string(512, 'a'));
		}

		return vs;
	};

	{
		void* p = Malloc(256);
		CHECK(IsAligned(p, MinAlignment));
		Free(p);
	}

	{
		constexpr size_t Alignment = (MinAlignment * 4);
		void* p = AlignedAlloc(256, Alignment);
		CHECK(IsAligned(p, Alignment));
		AlignedFree(p, Alignment);
	}

	{
		Bench{}.run("Malloc(16)", [&]() {
			void* p = Malloc(16);
			doNotOptimizeAway(p);
			Free(p);
			});

		Bench{}.run("Calloc(16, 1)", [&]() {
			void* p = Calloc(16, 1);
			doNotOptimizeAway(p);
			Free(p);
			});

		Bench{}.run("AlignedAlloc(16, 16)", [&]() {
			void* p = AlignedAlloc(16, 16);
			doNotOptimizeAway(p);
			AlignedFree(p, 16);
			});

		Bench{}.run("AlignedAlloc(16, 4096)", [&]() {
			void* p = AlignedAlloc(16, 4096);
			doNotOptimizeAway(p);
			AlignedFree(p, 4096);
			});
	}
	
	{
		Bench{}.run("Malloc(16384)", [&]() {
			void* p = Malloc(16384);
			doNotOptimizeAway(p);
			Free(p);
			});

		Bench{}.run("Calloc(16384, 1)", [&]() {
			void* p = Calloc(16384, 1);
			doNotOptimizeAway(p);
			Free(p);
			});

		Bench{}.run("AlignedAlloc(16384, 16)", [&]() {
			void* p = AlignedAlloc(16384, 16);
			doNotOptimizeAway(p);
			AlignedFree(p, 16);
			});

		Bench{}.run("AlignedAlloc(16384, 4096)", [&]() {
			void* p = AlignedAlloc(16384, 4096);
			doNotOptimizeAway(p);
			AlignedFree(p, 4096);
			});

		Bench{}.run("StressTest1", [&]() {
			doNotOptimizeAway(StressTest1());
			});

		Bench{}.run("StressTest2", [&]() {
			doNotOptimizeAway(StressTest2());
			});
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

	{
		const std::string sShort = "Siv3D";
		const std::string sMedium = "Hello, Siv3D!";
		const std::string sLong = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("std::hash - std::string short", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sShort)); });
		Bench{}.run("std::hash - std::string medium", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sMedium)); });
		Bench{}.run("std::hash - std::string long", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sLong)); });
		Bench{}.run("Hash - std::string short", [&]() { doNotOptimizeAway(Hash(sShort.data(), sShort.size())); });
		Bench{}.run("Hash - std::string medium", [&]() { doNotOptimizeAway(Hash(sMedium.data(), sMedium.size())); });
		Bench{}.run("Hash - std::string long", [&]() { doNotOptimizeAway(Hash(sLong.data(), sLong.size())); });
	}

	{
		const StringView sShort = U"Siv3D";
		const StringView sMedium = U"Hello, Siv3D!";
		const StringView sLong = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("std::hash - u32string_view short", [&]() { doNotOptimizeAway(std::hash<std::u32string_view>{}(sShort.view())); });
		Bench{}.run("std::hash - u32string_view medium", [&]() { doNotOptimizeAway(std::hash<std::u32string_view>{}(sMedium.view())); });
		Bench{}.run("std::hash - u32string_view long", [&]() { doNotOptimizeAway(std::hash<std::u32string_view>{}(sLong.view())); });
		Bench{}.run("Hash - StringView short", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sShort)); });
		Bench{}.run("Hash - StringView medium", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sMedium)); });
		Bench{}.run("Hash - StringView long", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sLong)); });
	}

	{
		const String sShort = U"Siv3D";
		const String sMedium = U"Hello, Siv3D!";
		const String sLong = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("Hash - StringView short", [&]() { doNotOptimizeAway(std::hash<String>{}(sShort)); });
		Bench{}.run("Hash - StringView medium", [&]() { doNotOptimizeAway(std::hash<String>{}(sMedium)); });
		Bench{}.run("Hash - StringView long", [&]() { doNotOptimizeAway(std::hash<String>{}(sLong)); });
	}
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

		const String ascii32VeryShort = U"Siv3D";
		const String ascii32Short = U"Lorem ipsum dolor sit amet";
		const String ascii32Long = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";


		Bench{}.run("Unicode::WidenAscii() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiVeryShort)); });
		Bench{}.run("Unicode::WidenAscii() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiShort)); });
		Bench{}.run("Unicode::WidenAscii() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::WidenAscii(asciiLong)); });

		Bench{}.run("Unicode::Widen() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::Widen(asciiVeryShort)); });
		Bench{}.run("Unicode::Widen() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::Widen(asciiShort)); });
		Bench{}.run("Unicode::Widen() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::Widen(asciiLong)); });

		Bench{}.run("Unicode::NarrowAscii() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::NarrowAscii(ascii32VeryShort)); });
		Bench{}.run("Unicode::NarrowAscii() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::NarrowAscii(ascii32Short)); });
		Bench{}.run("Unicode::NarrowAscii() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::NarrowAscii(ascii32Long)); });

		Bench{}.run("Unicode::Narrow() - very short (ASCII)", [&]() { doNotOptimizeAway(Unicode::Narrow(ascii32VeryShort)); });
		Bench{}.run("Unicode::Narrow() - short (ASCII)", [&]() { doNotOptimizeAway(Unicode::Narrow(ascii32Short)); });
		Bench{}.run("Unicode::Narrow() - long (ASCII)", [&]() { doNotOptimizeAway(Unicode::Narrow(ascii32Long)); });
	}
}

struct MyFormattableData
{
	int32 x;
	friend void Formatter(FormatData&, const MyFormattableData&) {}
};

struct MyUnformattableData
{
	int32 x;
};

TEST_CASE("Formatter.hpp")
{
	CHECK(Concept::Formattable<bool>);
	CHECK(Concept::Formattable<int32>);
	CHECK(Concept::Formattable<StringView>);
	CHECK(Concept::Formattable<String>);
	CHECK(Concept::Formattable<MyFormattableData>);
	CHECK(not Concept::Formattable<MyUnformattableData>);
}


TEST_CASE("Format.hpp")
{
	CHECK(Format(Smallest<int32>) == U"-2147483648");
	CHECK(Format(Largest<int32>) == U"2147483647");

	CHECK(Format(Smallest<uint32>) == U"0");
	CHECK(Format(Largest<uint32>) == U"4294967295");


	CHECK(Format(0.0f) == U"0");
	CHECK(Format(1.0f) == U"1");
	CHECK(Format(-1.0f) == U"-1");
	CHECK(Format(0.125f) == U"0.125");
	CHECK(Format(Inf<float>) == U"inf");
	CHECK(Format(-Inf<float>) == U"-inf");
	CHECK(Format(SNaN<float>) == U"nan");
	CHECK(Format(QNaN<float>) == U"nan");

	CHECK(Format(0.0) == U"0");
	CHECK(Format(1.0) == U"1");
	CHECK(Format(-1.0) == U"-1");
	CHECK(Format(0.125) == U"0.125");
	CHECK(Format(Inf<double>) == U"inf");
	CHECK(Format(-Inf<double>) == U"-inf");
	CHECK(Format(SNaN<double>) == U"nan");
	CHECK(Format(QNaN<double>) == U"nan");

	CHECK(Format(0.0L) == U"0");
	CHECK(Format(1.0L) == U"1");
	CHECK(Format(-1.0L) == U"-1");
	CHECK(Format(0.125L) == U"0.125");
	CHECK(Format(Inf<long double>) == U"inf");
	CHECK(Format(-Inf<long double>) == U"-inf");
	CHECK(Format(SNaN<long double>) == U"nan");
	CHECK(Format(QNaN<long double>) == U"nan");
}

# pragma once
# include <cstdint>
# include <array>
# include <limits>
# include <type_traits>
# if __has_cpp_attribute(nodiscard) >= 201907L
#	define XOSHIROCPP_NODISCARD_CXX20 [[nodiscard]]
# else
#	define XOSHIROCPP_NODISCARD_CXX20
# endif

namespace XoshiroCpp
{
	// A default seed value for the generators
	inline constexpr std::uint64_t DefaultSeed = 1234567890ULL;

	// Converts given uint32 value `i` into a 32-bit floating
	// point value in the range of [0.0f, 1.0f)
	template <class Uint32, std::enable_if_t<std::is_same_v<Uint32, std::uint32_t>>* = nullptr>
	[[nodiscard]]
	inline constexpr float FloatFromBits(Uint32 i) noexcept;

	// Converts given uint64 value `i` into a 64-bit floating
	// point value in the range of [0.0, 1.0)
	template <class Uint64, std::enable_if_t<std::is_same_v<Uint64, std::uint64_t>>* = nullptr>
	[[nodiscard]]
	inline constexpr double DoubleFromBits(Uint64 i) noexcept;

	// SplitMix64
	// Output: 64 bits
	// Period: 2^64
	// Footprint: 8 bytes
	// Original implementation: http://prng.di.unimi.it/splitmix64.c
	class SplitMix64
	{
	public:

		using state_type = std::uint64_t;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr SplitMix64(state_type state = DefaultSeed) noexcept;

		constexpr result_type operator()() noexcept;

		template <std::size_t N>
		[[nodiscard]]
		constexpr std::array<std::uint64_t, N> generateSeedSequence() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const SplitMix64& lhs, const SplitMix64& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const SplitMix64& lhs, const SplitMix64& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoshiro256+
	// Output: 64 bits
	// Period: 2^256 - 1
	// Footprint: 32 bytes
	// Original implementation: http://prng.di.unimi.it/xoshiro256plus.c
	// Version: 1.0
	class Xoshiro256Plus
	{
	public:

		using state_type = std::array<std::uint64_t, 4>;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro256Plus(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro256Plus(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^128 calls to operator(); it can be used to generate 2^128
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^192 calls to next(); it can be used to generate 2^64 starting points,
		// from each of which jump() will generate 2^64 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoshiro256Plus& lhs, const Xoshiro256Plus& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoshiro256Plus& lhs, const Xoshiro256Plus& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoshiro256++
	// Output: 64 bits
	// Period: 2^256 - 1
	// Footprint: 32 bytes
	// Original implementation: http://prng.di.unimi.it/xoshiro256plusplus.c
	// Version: 1.0
	class Xoshiro256PlusPlus
	{
	public:

		using state_type = std::array<std::uint64_t, 4>;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro256PlusPlus(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro256PlusPlus(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^128 calls to next(); it can be used to generate 2^128
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^192 calls to next(); it can be used to generate 2^64 starting points,
		// from each of which jump() will generate 2^64 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoshiro256PlusPlus& lhs, const Xoshiro256PlusPlus& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoshiro256PlusPlus& lhs, const Xoshiro256PlusPlus& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoshiro256**
	// Output: 64 bits
	// Period: 2^256 - 1
	// Footprint: 32 bytes
	// Original implementation: http://prng.di.unimi.it/xoshiro256starstar.c
	// Version: 1.0
	class Xoshiro256StarStar
	{
	public:

		using state_type = std::array<std::uint64_t, 4>;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro256StarStar(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro256StarStar(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^128 calls to next(); it can be used to generate 2^128
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^192 calls to next(); it can be used to generate 2^64 starting points,
		// from each of which jump() will generate 2^64 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoshiro256StarStar& lhs, const Xoshiro256StarStar& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoshiro256StarStar& lhs, const Xoshiro256StarStar& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoroshiro128+
	// Output: 64 bits
	// Period: 2^128 - 1
	// Footprint: 16 bytes
	// Original implementation: http://prng.di.unimi.it/xoroshiro128plus.c
	// Version: 1.0
	class Xoroshiro128Plus
	{
	public:

		using state_type = std::array<std::uint64_t, 2>;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoroshiro128Plus(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoroshiro128Plus(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^64 calls to next(); it can be used to generate 2^64
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^96 calls to next(); it can be used to generate 2^32 starting points,
		// from each of which jump() will generate 2^32 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoroshiro128Plus& lhs, const Xoroshiro128Plus& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoroshiro128Plus& lhs, const Xoroshiro128Plus& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoroshiro128++
	// Output: 64 bits
	// Period: 2^128 - 1
	// Footprint: 16 bytes
	// Original implementation: http://prng.di.unimi.it/xoroshiro128plusplus.c
	// Version: 1.0
	class Xoroshiro128PlusPlus
	{
	public:

		using state_type = std::array<std::uint64_t, 2>;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoroshiro128PlusPlus(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoroshiro128PlusPlus(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^64 calls to next(); it can be used to generate 2^64
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^96 calls to next(); it can be used to generate 2^32 starting points,
		// from each of which jump() will generate 2^32 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoroshiro128PlusPlus& lhs, const Xoroshiro128PlusPlus& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoroshiro128PlusPlus& lhs, const Xoroshiro128PlusPlus& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoroshiro128**
	// Output: 64 bits
	// Period: 2^128 - 1
	// Footprint: 16 bytes
	// Original implementation: http://prng.di.unimi.it/xoroshiro128starstar.c
	// Version: 1.0
	class Xoroshiro128StarStar
	{
	public:

		using state_type = std::array<std::uint64_t, 2>;
		using result_type = std::uint64_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoroshiro128StarStar(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoroshiro128StarStar(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^64 calls to next(); it can be used to generate 2^64
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^96 calls to next(); it can be used to generate 2^32 starting points,
		// from each of which jump() will generate 2^32 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoroshiro128StarStar& lhs, const Xoroshiro128StarStar& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoroshiro128StarStar& lhs, const Xoroshiro128StarStar& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoshiro128+
	// Output: 32 bits
	// Period: 2^128 - 1
	// Footprint: 16 bytes
	// Original implementation: http://prng.di.unimi.it/xoshiro128plus.c
	// Version: 1.0
	class Xoshiro128Plus
	{
	public:

		using state_type = std::array<std::uint32_t, 4>;
		using result_type = std::uint32_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro128Plus(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro128Plus(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^64 calls to next(); it can be used to generate 2^64
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^96 calls to next(); it can be used to generate 2^32 starting points,
		// from each of which jump() will generate 2^32 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoshiro128Plus& lhs, const Xoshiro128Plus& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoshiro128Plus& lhs, const Xoshiro128Plus& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoshiro128++
	// Output: 32 bits
	// Period: 2^128 - 1
	// Footprint: 16 bytes
	// Original implementation: http://prng.di.unimi.it/xoshiro128plusplus.c
	// Version: 1.0
	class Xoshiro128PlusPlus
	{
	public:

		using state_type = std::array<std::uint32_t, 4>;
		using result_type = std::uint32_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro128PlusPlus(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro128PlusPlus(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^64 calls to next(); it can be used to generate 2^64
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^96 calls to next(); it can be used to generate 2^32 starting points,
		// from each of which jump() will generate 2^32 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoshiro128PlusPlus& lhs, const Xoshiro128PlusPlus& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoshiro128PlusPlus& lhs, const Xoshiro128PlusPlus& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};

	// xoshiro128**
	// Output: 32 bits
	// Period: 2^128 - 1
	// Footprint: 16 bytes
	// Original implementation: http://prng.di.unimi.it/xoshiro128starstar.c
	// Version: 1.1
	class Xoshiro128StarStar
	{
	public:

		using state_type = std::array<std::uint32_t, 4>;
		using result_type = std::uint32_t;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro128StarStar(std::uint64_t seed = DefaultSeed) noexcept;

		XOSHIROCPP_NODISCARD_CXX20
			explicit constexpr Xoshiro128StarStar(state_type state) noexcept;

		constexpr result_type operator()() noexcept;

		// This is the jump function for the generator. It is equivalent
		// to 2^64 calls to next(); it can be used to generate 2^64
		// non-overlapping subsequences for parallel computations.
		constexpr void jump() noexcept;

		// This is the long-jump function for the generator. It is equivalent to
		// 2^96 calls to next(); it can be used to generate 2^32 starting points,
		// from each of which jump() will generate 2^32 non-overlapping
		// subsequences for parallel distributed computations.
		constexpr void longJump() noexcept;

		[[nodiscard]]
		static constexpr result_type min() noexcept;

		[[nodiscard]]
		static constexpr result_type max() noexcept;

		[[nodiscard]]
		constexpr state_type serialize() const noexcept;

		constexpr void deserialize(state_type state) noexcept;

		[[nodiscard]]
		friend bool operator ==(const Xoshiro128StarStar& lhs, const Xoshiro128StarStar& rhs) noexcept
		{
			return (lhs.m_state == rhs.m_state);
		}

		[[nodiscard]]
		friend bool operator !=(const Xoshiro128StarStar& lhs, const Xoshiro128StarStar& rhs) noexcept
		{
			return (lhs.m_state != rhs.m_state);
		}

	private:

		state_type m_state;
	};
}

////////////////////////////////////////////////////////////////

namespace XoshiroCpp
{
	template <class Uint32, std::enable_if_t<std::is_same_v<Uint32, std::uint32_t>>*>
	inline constexpr float FloatFromBits(const Uint32 i) noexcept
	{
		return (i >> 8) * 0x1.0p-24f;
	}

	template <class Uint64, std::enable_if_t<std::is_same_v<Uint64, std::uint64_t>>*>
	inline constexpr double DoubleFromBits(const Uint64 i) noexcept
	{
		return (i >> 11) * 0x1.0p-53;
	}

	namespace detail
	{
		[[nodiscard]]
		static constexpr std::uint64_t RotL(const std::uint64_t x, const int s) noexcept
		{
			return (x << s) | (x >> (64 - s));
		}

		[[nodiscard]]
		static constexpr std::uint32_t RotL(const std::uint32_t x, const int s) noexcept
		{
			return (x << s) | (x >> (32 - s));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	SplitMix64
	//
	inline constexpr SplitMix64::SplitMix64(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr SplitMix64::result_type SplitMix64::operator()() noexcept
	{
		std::uint64_t z = (m_state += 0x9e3779b97f4a7c15);
		z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
		z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
		return z ^ (z >> 31);
	}

	template <std::size_t N>
	inline constexpr std::array<std::uint64_t, N> SplitMix64::generateSeedSequence() noexcept
	{
		std::array<std::uint64_t, N> seeds = {};

		for (auto& seed : seeds)
		{
			seed = operator()();
		}

		return seeds;
	}

	inline constexpr SplitMix64::result_type SplitMix64::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr SplitMix64::result_type SplitMix64::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr SplitMix64::state_type SplitMix64::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void SplitMix64::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoshiro256+
	//
	inline constexpr Xoshiro256Plus::Xoshiro256Plus(const std::uint64_t seed) noexcept
		: m_state(SplitMix64{ seed }.generateSeedSequence<4>()) {}

	inline constexpr Xoshiro256Plus::Xoshiro256Plus(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoshiro256Plus::result_type Xoshiro256Plus::operator()() noexcept
	{
		const std::uint64_t result = m_state[0] + m_state[3];
		const std::uint64_t t = m_state[1] << 17;
		m_state[2] ^= m_state[0];
		m_state[3] ^= m_state[1];
		m_state[1] ^= m_state[2];
		m_state[0] ^= m_state[3];
		m_state[2] ^= t;
		m_state[3] = detail::RotL(m_state[3], 45);
		return result;
	}

	inline constexpr void Xoshiro256Plus::jump() noexcept
	{
		constexpr std::uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		std::uint64_t s2 = 0;
		std::uint64_t s3 = 0;

		for (std::uint64_t jump : JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr void Xoshiro256Plus::longJump() noexcept
	{
		constexpr std::uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		std::uint64_t s2 = 0;
		std::uint64_t s3 = 0;

		for (std::uint64_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr Xoshiro256Plus::result_type Xoshiro256Plus::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoshiro256Plus::result_type Xoshiro256Plus::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoshiro256Plus::state_type Xoshiro256Plus::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoshiro256Plus::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoshiro256++
	//
	inline constexpr Xoshiro256PlusPlus::Xoshiro256PlusPlus(const std::uint64_t seed) noexcept
		: m_state(SplitMix64{ seed }.generateSeedSequence<4>()) {}

	inline constexpr Xoshiro256PlusPlus::Xoshiro256PlusPlus(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoshiro256PlusPlus::result_type Xoshiro256PlusPlus::operator()() noexcept
	{
		const std::uint64_t result = detail::RotL(m_state[0] + m_state[3], 23) + m_state[0];
		const std::uint64_t t = m_state[1] << 17;
		m_state[2] ^= m_state[0];
		m_state[3] ^= m_state[1];
		m_state[1] ^= m_state[2];
		m_state[0] ^= m_state[3];
		m_state[2] ^= t;
		m_state[3] = detail::RotL(m_state[3], 45);
		return result;
	}

	inline constexpr void Xoshiro256PlusPlus::jump() noexcept
	{
		constexpr std::uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		std::uint64_t s2 = 0;
		std::uint64_t s3 = 0;

		for (std::uint64_t jump : JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr void Xoshiro256PlusPlus::longJump() noexcept
	{
		constexpr std::uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		std::uint64_t s2 = 0;
		std::uint64_t s3 = 0;

		for (std::uint64_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr Xoshiro256PlusPlus::result_type Xoshiro256PlusPlus::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoshiro256PlusPlus::result_type Xoshiro256PlusPlus::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoshiro256PlusPlus::state_type Xoshiro256PlusPlus::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoshiro256PlusPlus::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoshiro256**
	//
	inline constexpr Xoshiro256StarStar::Xoshiro256StarStar(const std::uint64_t seed) noexcept
		: m_state(SplitMix64{ seed }.generateSeedSequence<4>()) {}

	inline constexpr Xoshiro256StarStar::Xoshiro256StarStar(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoshiro256StarStar::result_type Xoshiro256StarStar::operator()() noexcept
	{
		const std::uint64_t result = detail::RotL(m_state[1] * 5, 7) * 9;
		const std::uint64_t t = m_state[1] << 17;
		m_state[2] ^= m_state[0];
		m_state[3] ^= m_state[1];
		m_state[1] ^= m_state[2];
		m_state[0] ^= m_state[3];
		m_state[2] ^= t;
		m_state[3] = detail::RotL(m_state[3], 45);
		return result;
	}

	inline constexpr void Xoshiro256StarStar::jump() noexcept
	{
		constexpr std::uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		std::uint64_t s2 = 0;
		std::uint64_t s3 = 0;

		for (std::uint64_t jump : JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr void Xoshiro256StarStar::longJump() noexcept
	{
		constexpr std::uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;
		std::uint64_t s2 = 0;
		std::uint64_t s3 = 0;

		for (std::uint64_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr Xoshiro256StarStar::result_type Xoshiro256StarStar::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoshiro256StarStar::result_type Xoshiro256StarStar::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoshiro256StarStar::state_type Xoshiro256StarStar::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoshiro256StarStar::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoroshiro128+
	//
	inline constexpr Xoroshiro128Plus::Xoroshiro128Plus(const std::uint64_t seed) noexcept
		: m_state(SplitMix64{ seed }.generateSeedSequence<2>()) {}

	inline constexpr Xoroshiro128Plus::Xoroshiro128Plus(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoroshiro128Plus::result_type Xoroshiro128Plus::operator()() noexcept
	{
		const std::uint64_t s0 = m_state[0];
		std::uint64_t s1 = m_state[1];
		const std::uint64_t result = s0 + s1;
		s1 ^= s0;
		m_state[0] = detail::RotL(s0, 24) ^ s1 ^ (s1 << 16);
		m_state[1] = detail::RotL(s1, 37);
		return result;
	}

	inline constexpr void Xoroshiro128Plus::jump() noexcept
	{
		constexpr std::uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;

		for (std::uint64_t jump : JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
	}

	inline constexpr void Xoroshiro128Plus::longJump() noexcept
	{
		constexpr std::uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;

		for (std::uint64_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
	}

	inline constexpr Xoroshiro128Plus::result_type Xoroshiro128Plus::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoroshiro128Plus::result_type Xoroshiro128Plus::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoroshiro128Plus::state_type Xoroshiro128Plus::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoroshiro128Plus::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoroshiro128++
	//
	inline constexpr Xoroshiro128PlusPlus::Xoroshiro128PlusPlus(const std::uint64_t seed) noexcept
		: m_state(SplitMix64{ seed }.generateSeedSequence<2>()) {}

	inline constexpr Xoroshiro128PlusPlus::Xoroshiro128PlusPlus(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoroshiro128PlusPlus::result_type Xoroshiro128PlusPlus::operator()() noexcept
	{
		const std::uint64_t s0 = m_state[0];
		std::uint64_t s1 = m_state[1];
		const std::uint64_t result = detail::RotL(s0 + s1, 17) + s0;
		s1 ^= s0;
		m_state[0] = detail::RotL(s0, 49) ^ s1 ^ (s1 << 21);
		m_state[1] = detail::RotL(s1, 28);
		return result;
	}

	inline constexpr void Xoroshiro128PlusPlus::jump() noexcept
	{
		constexpr std::uint64_t JUMP[] = { 0x2bd7a6a6e99c2ddc, 0x0992ccaf6a6fca05 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;

		for (std::uint64_t jump : JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
	}

	inline constexpr void Xoroshiro128PlusPlus::longJump() noexcept
	{
		constexpr std::uint64_t LONG_JUMP[] = { 0x360fd5f2cf8d5d99, 0x9c6e6877736c46e3 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;

		for (std::uint64_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
	}

	inline constexpr Xoroshiro128PlusPlus::result_type Xoroshiro128PlusPlus::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoroshiro128PlusPlus::result_type Xoroshiro128PlusPlus::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoroshiro128PlusPlus::state_type Xoroshiro128PlusPlus::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoroshiro128PlusPlus::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoroshiro128**
	//
	inline constexpr Xoroshiro128StarStar::Xoroshiro128StarStar(const std::uint64_t seed) noexcept
		: m_state(SplitMix64{ seed }.generateSeedSequence<2>()) {}

	inline constexpr Xoroshiro128StarStar::Xoroshiro128StarStar(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoroshiro128StarStar::result_type Xoroshiro128StarStar::operator()() noexcept
	{
		const std::uint64_t s0 = m_state[0];
		std::uint64_t s1 = m_state[1];
		const std::uint64_t result = detail::RotL(s0 * 5, 7) * 9;
		s1 ^= s0;
		m_state[0] = detail::RotL(s0, 24) ^ s1 ^ (s1 << 16);
		m_state[1] = detail::RotL(s1, 37);
		return result;
	}

	inline constexpr void Xoroshiro128StarStar::jump() noexcept
	{
		constexpr std::uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;

		for (std::uint64_t jump : JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
	}

	inline constexpr void Xoroshiro128StarStar::longJump() noexcept
	{
		constexpr std::uint64_t LONG_JUMP[] = { 0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1 };

		std::uint64_t s0 = 0;
		std::uint64_t s1 = 0;

		for (std::uint64_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 64; ++b)
			{
				if (jump & UINT64_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
	}

	inline constexpr Xoroshiro128StarStar::result_type Xoroshiro128StarStar::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoroshiro128StarStar::result_type Xoroshiro128StarStar::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoroshiro128StarStar::state_type Xoroshiro128StarStar::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoroshiro128StarStar::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoshiro128+
	//
	inline constexpr Xoshiro128Plus::Xoshiro128Plus(const std::uint64_t seed) noexcept
		: m_state()
	{
		SplitMix64 splitmix{ seed };

		for (auto& state : m_state)
		{
			state = static_cast<std::uint32_t>(splitmix());
		}
	}

	inline constexpr Xoshiro128Plus::Xoshiro128Plus(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoshiro128Plus::result_type Xoshiro128Plus::operator()() noexcept
	{
		const std::uint32_t result = m_state[0] + m_state[3];
		const std::uint32_t t = m_state[1] << 9;
		m_state[2] ^= m_state[0];
		m_state[3] ^= m_state[1];
		m_state[1] ^= m_state[2];
		m_state[0] ^= m_state[3];
		m_state[2] ^= t;
		m_state[3] = detail::RotL(m_state[3], 11);
		return result;
	}

	inline constexpr void Xoshiro128Plus::jump() noexcept
	{
		constexpr std::uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

		std::uint32_t s0 = 0;
		std::uint32_t s1 = 0;
		std::uint32_t s2 = 0;
		std::uint32_t s3 = 0;

		for (std::uint32_t jump : JUMP)
		{
			for (int b = 0; b < 32; ++b)
			{
				if (jump & UINT32_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr void Xoshiro128Plus::longJump() noexcept
	{
		constexpr std::uint32_t LONG_JUMP[] = { 0xb523952e, 0x0b6f099f, 0xccf5a0ef, 0x1c580662 };

		std::uint32_t s0 = 0;
		std::uint32_t s1 = 0;
		std::uint32_t s2 = 0;
		std::uint32_t s3 = 0;

		for (std::uint32_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 32; ++b)
			{
				if (jump & UINT32_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr Xoshiro128Plus::result_type Xoshiro128Plus::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoshiro128Plus::result_type Xoshiro128Plus::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoshiro128Plus::state_type Xoshiro128Plus::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoshiro128Plus::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoshiro128++
	//
	inline constexpr Xoshiro128PlusPlus::Xoshiro128PlusPlus(const std::uint64_t seed) noexcept
		: m_state()
	{
		SplitMix64 splitmix{ seed };

		for (auto& state : m_state)
		{
			state = static_cast<std::uint32_t>(splitmix());
		}
	}

	inline constexpr Xoshiro128PlusPlus::Xoshiro128PlusPlus(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoshiro128PlusPlus::result_type Xoshiro128PlusPlus::operator()() noexcept
	{
		const std::uint32_t result = detail::RotL(m_state[0] + m_state[3], 7) + m_state[0];
		const std::uint32_t t = m_state[1] << 9;
		m_state[2] ^= m_state[0];
		m_state[3] ^= m_state[1];
		m_state[1] ^= m_state[2];
		m_state[0] ^= m_state[3];
		m_state[2] ^= t;
		m_state[3] = detail::RotL(m_state[3], 11);
		return result;
	}

	inline constexpr void Xoshiro128PlusPlus::jump() noexcept
	{
		constexpr std::uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

		std::uint32_t s0 = 0;
		std::uint32_t s1 = 0;
		std::uint32_t s2 = 0;
		std::uint32_t s3 = 0;

		for (std::uint32_t jump : JUMP)
		{
			for (int b = 0; b < 32; ++b)
			{
				if (jump & UINT32_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr void Xoshiro128PlusPlus::longJump() noexcept
	{
		constexpr std::uint32_t LONG_JUMP[] = { 0xb523952e, 0x0b6f099f, 0xccf5a0ef, 0x1c580662 };

		std::uint32_t s0 = 0;
		std::uint32_t s1 = 0;
		std::uint32_t s2 = 0;
		std::uint32_t s3 = 0;

		for (std::uint32_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 32; ++b)
			{
				if (jump & UINT32_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr Xoshiro128PlusPlus::result_type Xoshiro128PlusPlus::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoshiro128PlusPlus::result_type Xoshiro128PlusPlus::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoshiro128PlusPlus::state_type Xoshiro128PlusPlus::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoshiro128PlusPlus::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}

	////////////////////////////////////////////////////////////////
	//
	//	xoshiro128**
	//
	inline constexpr Xoshiro128StarStar::Xoshiro128StarStar(const std::uint64_t seed) noexcept
		: m_state()
	{
		SplitMix64 splitmix{ seed };

		for (auto& state : m_state)
		{
			state = static_cast<std::uint32_t>(splitmix());
		}
	}

	inline constexpr Xoshiro128StarStar::Xoshiro128StarStar(const state_type state) noexcept
		: m_state(state) {}

	inline constexpr Xoshiro128StarStar::result_type Xoshiro128StarStar::operator()() noexcept
	{
		const std::uint32_t result = detail::RotL(m_state[1] * 5, 7) * 9;
		const std::uint32_t t = m_state[1] << 9;
		m_state[2] ^= m_state[0];
		m_state[3] ^= m_state[1];
		m_state[1] ^= m_state[2];
		m_state[0] ^= m_state[3];
		m_state[2] ^= t;
		m_state[3] = detail::RotL(m_state[3], 11);
		return result;
	}

	inline constexpr void Xoshiro128StarStar::jump() noexcept
	{
		constexpr std::uint32_t JUMP[] = { 0x8764000b, 0xf542d2d3, 0x6fa035c3, 0x77f2db5b };

		std::uint32_t s0 = 0;
		std::uint32_t s1 = 0;
		std::uint32_t s2 = 0;
		std::uint32_t s3 = 0;

		for (std::uint32_t jump : JUMP)
		{
			for (int b = 0; b < 32; ++b)
			{
				if (jump & UINT32_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr void Xoshiro128StarStar::longJump() noexcept
	{
		constexpr std::uint32_t LONG_JUMP[] = { 0xb523952e, 0x0b6f099f, 0xccf5a0ef, 0x1c580662 };

		std::uint32_t s0 = 0;
		std::uint32_t s1 = 0;
		std::uint32_t s2 = 0;
		std::uint32_t s3 = 0;

		for (std::uint32_t jump : LONG_JUMP)
		{
			for (int b = 0; b < 32; ++b)
			{
				if (jump & UINT32_C(1) << b)
				{
					s0 ^= m_state[0];
					s1 ^= m_state[1];
					s2 ^= m_state[2];
					s3 ^= m_state[3];
				}
				operator()();
			}
		}

		m_state[0] = s0;
		m_state[1] = s1;
		m_state[2] = s2;
		m_state[3] = s3;
	}

	inline constexpr Xoshiro128StarStar::result_type Xoshiro128StarStar::min() noexcept
	{
		return std::numeric_limits<result_type>::lowest();
	}

	inline constexpr Xoshiro128StarStar::result_type Xoshiro128StarStar::max() noexcept
	{
		return std::numeric_limits<result_type>::max();
	}

	inline constexpr Xoshiro128StarStar::state_type Xoshiro128StarStar::serialize() const noexcept
	{
		return m_state;
	}

	inline constexpr void Xoshiro128StarStar::deserialize(const state_type state) noexcept
	{
		m_state = state;
	}
}


TEST_CASE("PCG.hpp")
{
	{
		uint64 buffer[1024]{};

		Bench{}.run("PCG32", [&]() {
			pcg32 rng;

			for (size_t i = 0; i < std::size(buffer); ++i)
			{
				buffer[i] = rng();
			}

			doNotOptimizeAway(buffer);
			});

		Bench{}.run("PCG64", [&]() {
			PRNG::PCG64 rng;

			for (size_t i = 0; i < std::size(buffer); ++i)
			{
				buffer[i] = rng();
			}

			doNotOptimizeAway(buffer);
			});

		Bench{}.run("xoshiro256++", [&]() {
			XoshiroCpp::Xoshiro256PlusPlus rng(12345);

			for (size_t i = 0; i < std::size(buffer); ++i)
			{
				buffer[i] = rng();
			}

			doNotOptimizeAway(buffer);
			});

		Bench{}.run("std::mt19937_64", [&]() {
			std::mt19937_64 rng;

			for (size_t i = 0; i < std::size(buffer); ++i)
			{
				buffer[i] = rng();
			}

			doNotOptimizeAway(buffer);
			});
	}
}

SIV3D_DISABLE_MSVC_WARNINGS_POP()