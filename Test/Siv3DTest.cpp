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
	{
		const std::string sShort = "Siv3D";
		const std::string sMedium = "Hello, Siv3D!";
		const std::string sLong = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("std::hash - std::string short", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sShort)); });
		Bench{}.run("std::hash - std::string medium", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sMedium)); });
		Bench{}.run("std::hash - std::string long", [&]() { doNotOptimizeAway(std::hash<std::string>{}(sLong)); });
		Bench{}.run("s3d::Hash - std::string short", [&]() { doNotOptimizeAway(Hash(sShort.data(), sShort.size())); });
		Bench{}.run("s3d::Hash - std::string medium", [&]() { doNotOptimizeAway(Hash(sMedium.data(), sMedium.size())); });
		Bench{}.run("s3d::Hash - std::string long", [&]() { doNotOptimizeAway(Hash(sLong.data(), sLong.size())); });
	}

	{
		const StringView sShort = U"Siv3D";
		const StringView sMedium = U"Hello, Siv3D!";
		const StringView sLong = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("std::hash - StringView short", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sShort)); });
		Bench{}.run("std::hash - StringView medium", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sMedium)); });
		Bench{}.run("std::hash - StringView long", [&]() { doNotOptimizeAway(std::hash<StringView>{}(sLong)); });
		Bench{}.run("s3d::Hash - StringView short", [&]() { doNotOptimizeAway(Hash(sShort.data(), sShort.size() * sizeof(char32))); });
		Bench{}.run("s3d::Hash - StringView medium", [&]() { doNotOptimizeAway(Hash(sMedium.data(), sMedium.size() * sizeof(char32))); });
		Bench{}.run("s3d::Hash - StringView long", [&]() { doNotOptimizeAway(Hash(sLong.data(), sLong.size() * sizeof(char32))); });
	}

	{
		const String sShort = U"Siv3D";
		const String sMedium = U"Hello, Siv3D!";
		const String sLong = U"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

		Bench{}.run("std::hash - String short", [&]() { doNotOptimizeAway(std::hash<String>{}(sShort)); });
		Bench{}.run("std::hash - String medium", [&]() { doNotOptimizeAway(std::hash<String>{}(sMedium)); });
		Bench{}.run("std::hash - String long", [&]() { doNotOptimizeAway(std::hash<String>{}(sLong)); });
		Bench{}.run("s3d::Hash - String short", [&]() { doNotOptimizeAway(Hash(sShort.data(), sShort.size() * sizeof(char32))); });
		Bench{}.run("s3d::Hash - String medium", [&]() { doNotOptimizeAway(Hash(sMedium.data(), sMedium.size() * sizeof(char32))); });
		Bench{}.run("s3d::Hash - String long", [&]() { doNotOptimizeAway(Hash(sLong.data(), sLong.size() * sizeof(char32))); });
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



SIV3D_DISABLE_MSVC_WARNINGS_POP()