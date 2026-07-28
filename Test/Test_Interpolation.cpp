//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("Math::SmoothDamp")
{
	SUBCASE("normal")
	{
		double velocity = 0.0;
		const double result = Math::SmoothDamp(0.0, 1.0, velocity, 0.2, unspecified, (1.0 / 60.0));

		CHECK(0.0 < result);
		CHECK(result < 1.0);
		CHECK(0.0 < velocity);
	}

	SUBCASE("zero deltaTime")
	{
		double velocity = 3.0;
		const double result = Math::SmoothDamp(1.0, 1.0, velocity, 0.2, unspecified, 0.0);

		CHECK(result == 1.0);
		CHECK(velocity == 3.0);
	}

	SUBCASE("negative deltaTime")
	{
		double velocity = 3.0;
		const double result = Math::SmoothDamp(1.0, 2.0, velocity, 0.2, unspecified, -0.1);

		CHECK(result == 1.0);
		CHECK(velocity == 3.0);
	}

	SUBCASE("HSV with zero deltaTime")
	{
		constexpr HSV value{ 120.0, 0.5, 0.75, 0.25 };
		HSV velocity{ 1.0, 0.1, 0.2, 0.3 };
		const HSV result = Math::SmoothDamp(value, value, velocity, 0.2, unspecified, 0.0);

		CHECK(result == value);
		CHECK(velocity == HSV{ 1.0, 0.1, 0.2, 0.3 });
	}

	SUBCASE("overshoot")
	{
		double velocity = 100.0;
		const double result = Math::SmoothDamp(0.0, 1.0, velocity, 0.2, unspecified, 0.1);

		CHECK(result == 1.0);
		CHECK(velocity == 0.0);
	}
}
