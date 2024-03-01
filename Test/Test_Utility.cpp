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

struct MyInt
{
	int value = 0;
	constexpr auto operator <=>(const MyInt&) const = default;
};

enum Number : int32
{
	One = 1,
	Two = 2,
	Three = 3,
};

enum class Color : uint8
{
	Red,
	Green,
	Blue,
};

TEST_CASE("Utility")
{
	SUBCASE("Min")
	{
		const std::string a;

		static_assert(Min(1, 2) == 1);
		static_assert(Min(2, 1) == 1);
		static_assert(Min(1.0, 2.0) == 1.0);
		static_assert(Min(2.0, 1.0) == 1.0);
		static_assert(Min(MyInt{ 1 }, MyInt{ 2 }).value == 1);
		static_assert(Min(MyInt{ 2 }, MyInt{ 1 }).value == 1);

		static_assert(not std::is_reference_v<decltype(Min(1, 2))>);
		static_assert(not std::is_reference_v<decltype(Min(1.0, 2.0))>);
		static_assert(std::is_reference_v<decltype(Min(MyInt{ 1 }, MyInt{ 2 }))>);
		static_assert(std::is_reference_v<decltype(Min(a, a))>);
	}

	SUBCASE("Max")
	{
		const std::string a;

		static_assert(Max(1, 2) == 2);
		static_assert(Max(2, 1) == 2);
		static_assert(Max(1.0, 2.0) == 2.0);
		static_assert(Max(2.0, 1.0) == 2.0);
		static_assert(Max(MyInt{ 1 }, MyInt{ 2 }).value == 2);
		static_assert(Max(MyInt{ 2 }, MyInt{ 1 }).value == 2);

		static_assert(not std::is_reference_v<decltype(Max(1, 2))>);
		static_assert(not std::is_reference_v<decltype(Max(1.0, 2.0))>);
		static_assert(std::is_reference_v<decltype(Max(MyInt{ 1 }, MyInt{ 2 }))>);
		static_assert(std::is_reference_v<decltype(Max(a, a))>);
	}

	SUBCASE("Clamp")
	{
		const std::string a;

		static_assert(Clamp(1, 2, 3) == 2);
		static_assert(Clamp(2, 1, 3) == 2);
		static_assert(Clamp(3, 1, 2) == 2);
		static_assert(Clamp(1.0, 2.0, 3.0) == 2.0);
		static_assert(Clamp(2.0, 1.0, 3.0) == 2.0);
		static_assert(Clamp(3.0, 1.0, 2.0) == 2.0);
		static_assert(Clamp(MyInt{ 1 }, MyInt{ 2 }, MyInt{ 3 }).value == 2);
		static_assert(Clamp(MyInt{ 2 }, MyInt{ 1 }, MyInt{ 3 }).value == 2);
		static_assert(Clamp(MyInt{ 3 }, MyInt{ 1 }, MyInt{ 2 }).value == 2);

		static_assert(not std::is_reference_v<decltype(Clamp(1, 2, 3))>);
		static_assert(not std::is_reference_v<decltype(Clamp(1.0, 2.0, 3.0))>);
		static_assert(std::is_reference_v<decltype(Clamp(MyInt{ 1 }, MyInt{ 2 }, MyInt{ 3 }))>);
		static_assert(std::is_reference_v<decltype(Clamp(a, a, a))>);	
	}

	SUBCASE("InRange")
	{
		static_assert(InRange(1, 2, 3) == false);
		static_assert(InRange(2, 1, 3) == true);
		static_assert(InRange(3, 1, 2) == false);
		static_assert(InRange(1.0, 2.0, 3.0) == false);
		static_assert(InRange(2.0, 1.0, 3.0) == true);
		static_assert(InRange(3.0, 1.0, 2.0) == false);
		static_assert(InRange(MyInt{ 1 }, MyInt{ 2 }, MyInt{ 3 }) == false);
		static_assert(InRange(MyInt{ 2 }, MyInt{ 1 }, MyInt{ 3 }) == true);
		static_assert(InRange(MyInt{ 3 }, MyInt{ 1 }, MyInt{ 2 }) == false);
	}

	SUBCASE("InOpenRange")
	{
		static_assert(InOpenRange(1, 3, 5) == false);
		static_assert(InOpenRange(3, 1, 5) == true);
		static_assert(InOpenRange(5, 1, 3) == false);
		static_assert(InOpenRange(1.0, 3.0, 5.0) == false);
		static_assert(InOpenRange(3.0, 1.0, 5.0) == true);
		static_assert(InOpenRange(5.0, 1.0, 3.0) == false);
		static_assert(InOpenRange(MyInt{ 1 }, MyInt{ 3 }, MyInt{ 5 }) == false);
		static_assert(InOpenRange(MyInt{ 3 }, MyInt{ 1 }, MyInt{ 5 }) == true);
		static_assert(InOpenRange(MyInt{ 5 }, MyInt{ 1 }, MyInt{ 3 }) == false);
	}

	SUBCASE("Abs")
	{
		static_assert(Abs(1) == 1);
		static_assert(Abs(-1) == 1);
		static_assert(Abs(1LL) == 1LL);
		static_assert(Abs(-1LL) == 1LL);
		static_assert(Abs(1.0) == 1.0);
		static_assert(Abs(-1.0) == 1.0);

		static_assert(Abs(std::numeric_limits<int32>::max()) == std::numeric_limits<int32>::max());
		static_assert(Abs(std::numeric_limits<int32>::min() + 1) == std::numeric_limits<int32>::max());
		static_assert(Abs(std::numeric_limits<int64>::max()) == std::numeric_limits<int64>::max());
		static_assert(Abs(std::numeric_limits<int64>::min() + 1) == std::numeric_limits<int64>::max());
		static_assert(Abs(std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		static_assert(Abs(-std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
	}

	SUBCASE("AbsDiff")
	{
		static_assert(AbsDiff(1, 2) == 1);
		static_assert(AbsDiff(2, 1) == 1);
		static_assert(AbsDiff(1u, 2u) == 1u);
		static_assert(AbsDiff(2u, 1u) == 1u);
		static_assert(AbsDiff(1LL, 2LL) == 1LL);
		static_assert(AbsDiff(2LL, 1LL) == 1LL);
		static_assert(AbsDiff(1.0, 2.0) == 1.0);
		static_assert(AbsDiff(2.0, 1.0) == 1.0);

		static_assert(AbsDiff(std::numeric_limits<int32>::max(), std::numeric_limits<int32>::min()) == std::numeric_limits<uint32>::max());
		static_assert(AbsDiff(std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max()) == std::numeric_limits<uint32>::max());
		static_assert(AbsDiff(std::numeric_limits<int32>::min(), 0) == 2147483648u);
		static_assert(AbsDiff(0, std::numeric_limits<int32>::max()) == 2147483647u);

		static_assert(AbsDiff(std::numeric_limits<uint32>::max(), 0u) == std::numeric_limits<uint32>::max());
		static_assert(AbsDiff(0u, std::numeric_limits<uint32>::max()) == std::numeric_limits<uint32>::max());

		static_assert(AbsDiff(std::numeric_limits<int64>::max(), std::numeric_limits<int64>::min()) == std::numeric_limits<uint64>::max());
		static_assert(AbsDiff(std::numeric_limits<int64>::min(), std::numeric_limits<int64>::max()) == std::numeric_limits<uint64>::max());
		static_assert(AbsDiff(std::numeric_limits<int64>::min(), 0LL) == 9223372036854775808ull);
		static_assert(AbsDiff(0LL, std::numeric_limits<int64>::max()) == 9223372036854775807ull);

		static_assert(AbsDiff(std::numeric_limits<double>::max(), 0.0) == std::numeric_limits<double>::max());
		static_assert(AbsDiff(0.0, std::numeric_limits<double>::max()) == std::numeric_limits<double>::max());
		static_assert(AbsDiff(std::numeric_limits<double>::max() / 2, -std::numeric_limits<double>::max() / 2) == std::numeric_limits<double>::max());
	}

	SUBCASE("FromEnum")
	{
		static_assert(FromEnum(One) == 1);
		static_assert(FromEnum(Two) == 2);
		static_assert(FromEnum(Three) == 3);
		static_assert(FromEnum(Color::Red) == 0);
		static_assert(FromEnum(Color::Green) == 1);
		static_assert(FromEnum(Color::Blue) == 2);

		static_assert(std::is_same_v<decltype(FromEnum(One)), int32>);
		static_assert(std::is_same_v<decltype(FromEnum(Color::Red)), uint8>);
	}

	SUBCASE("ToEnum")
	{
		static_assert(ToEnum<Number>(1) == One);
		static_assert(ToEnum<Number>(2) == Two);
		static_assert(ToEnum<Number>(3) == Three);
		static_assert(ToEnum<Color>(0) == Color::Red);
		static_assert(ToEnum<Color>(1) == Color::Green);
		static_assert(ToEnum<Color>(2) == Color::Blue);

		static_assert(std::is_same_v<decltype(ToEnum<Number>(1)), Number>);
		static_assert(std::is_same_v<decltype(ToEnum<Color>(0)), Color>);
	}

	SUBCASE("IsOdd")
	{
		static_assert(IsOdd(std::numeric_limits<int32>::min()) == false);
		static_assert(IsOdd(-2) == false);
		static_assert(IsOdd(-1) == true);
		static_assert(IsOdd(0) == false);
		static_assert(IsOdd(1) == true);
		static_assert(IsOdd(2) == false);
		static_assert(IsOdd(std::numeric_limits<int32>::max()) == true);

		static_assert(IsOdd(0u) == false);
		static_assert(IsOdd(1u) == true);
		static_assert(IsOdd(2u) == false);
		static_assert(IsOdd(std::numeric_limits<uint32>::max()) == true);

		static_assert(IsOdd(std::numeric_limits<int64>::min()) == false);
		static_assert(IsOdd(-2LL) == false);
		static_assert(IsOdd(-1LL) == true);
		static_assert(IsOdd(0LL) == false);
		static_assert(IsOdd(1LL) == true);
		static_assert(IsOdd(2LL) == false);
		static_assert(IsOdd(std::numeric_limits<int64>::max()) == true);
	}

	SUBCASE("IsEven")
	{
		static_assert(IsEven(std::numeric_limits<int32>::min()) == true);
		static_assert(IsEven(-2) == true);
		static_assert(IsEven(-1) == false);
		static_assert(IsEven(0) == true);
		static_assert(IsEven(1) == false);
		static_assert(IsEven(2) == true);
		static_assert(IsEven(std::numeric_limits<int32>::max()) == false);

		static_assert(IsEven(0u) == true);
		static_assert(IsEven(1u) == false);
		static_assert(IsEven(2u) == true);
		static_assert(IsEven(std::numeric_limits<uint32>::max()) == false);

		static_assert(IsEven(std::numeric_limits<int64>::min()) == true);
		static_assert(IsEven(-2LL) == true);
		static_assert(IsEven(-1LL) == false);
		static_assert(IsEven(0LL) == true);
		static_assert(IsEven(1LL) == false);
		static_assert(IsEven(2LL) == true);
		static_assert(IsEven(std::numeric_limits<int64>::max()) == false);
	}

	SUBCASE("Identity")
	{
		static_assert(Identity(1) == 1);
		static_assert(Identity(1.0) == 1.0);
		static_assert(Identity(MyInt{ 1 }).value == 1);

		static_assert(std::is_same_v<decltype(Identity(1)), int&&>);
		static_assert(std::is_same_v<decltype(Identity(1.0)), double&&>);
		static_assert(std::is_same_v<decltype(Identity(MyInt{ 1 })), MyInt&&>);
		static_assert(std::is_same_v<decltype(Identity(std::string{ "a" })), std::string&&>);
	}
}
