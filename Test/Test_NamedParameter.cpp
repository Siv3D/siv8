//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace test
{
	SIV3D_NAMED_PARAMETER(month);
	SIV3D_NAMED_PARAMETER(day);
	SIV3D_NAMED_PARAMETER(center);
	SIV3D_NAMED_PARAMETER(name);

	struct Point
	{
		int32 x, y;

		Point() = default;

		constexpr Point(int32 _x, int32 _y) noexcept
			: x{ _x }
			, y{ _y } {}
	};
}

static constexpr int32 F1(test::month_<int32> month, test::day_<int32> day)
{
	return (month.value() * 100 + day.value());
}

static constexpr int32 F1(test::day_<int32> day, test::month_<int32> month)
{
	return (month.value() * 100 + day.value());
}

static constexpr int32 F1(int32 month, int32 day)
{
	return (month * 100 + day);
}

static constexpr int32 F2(test::center_<test::Point> center)
{
	return (center.value().x * 100 + center.value().y);
}

static constexpr int32 F2(test::Point center)
{
	return (center.x * 100 + center.y);
}

static size_t F3(const test::name_<std::string>& name)
{
	return name->size();
}

TEST_CASE("NamedParameter")
{
	static_assert(F1(1, 2) == 102);
	
	static_assert(F1(test::month = 1, test::day = 2) == 102);
	static_assert(F1(test::day = 2, test::month = 1) == 102);
	static_assert(F1(test::month(1), test::day(2)) == 102);
	static_assert(F1(test::day(2), test::month(1)) == 102);

	static_assert(F1(test::month = 1u, test::day = 2u) == 102);
	static_assert(F1(test::day = 2u, test::month = 1u) == 102);
	static_assert(F1(test::month(1u), test::day(2u)) == 102);
	static_assert(F1(test::day(2u), test::month(1u)) == 102);

	static_assert(F2(test::center = test::Point{ 1, 2 }) == 102);
	static_assert(F2(test::center(test::Point{ 1, 2 })) == 102);
	static_assert(F2(test::center(1, 2)) == 102);
	static_assert(F2(test::Point{ 1, 2 }) == 102);

	CHECK(F3(test::name = "Siv3D") == 5);
	CHECK(F3(test::name("Siv3D")) == 5);
	CHECK(F3(test::name(5, 'a')) == 5);
}
