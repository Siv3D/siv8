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

	CHECK(Concept::NonScalar<bool> == false);
	CHECK(Concept::NonScalar<uint8> == false);
	CHECK(Concept::NonScalar<int32> == false);
	CHECK(Concept::NonScalar<float> == false);
	CHECK(Concept::NonScalar<std::string> == true);
	CHECK(Concept::NonScalar<EnumT> == false);
	CHECK(Concept::NonScalar<ScopedEnumT> == false);

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
