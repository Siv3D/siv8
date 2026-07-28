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

namespace
{
	struct ConstLerpValue
	{
		double value;

		[[nodiscard]]
		constexpr ConstLerpValue lerp(const ConstLerpValue& other, const double f) const noexcept
		{
			return{ Math::Lerp(value, other.value, f) };
		}
	};

	struct MutableOnlyLerpValue
	{
		[[nodiscard]]
		MutableOnlyLerpValue lerp(const MutableOnlyLerpValue&, double)
		{
			return{};
		}
	};

	struct ThrowingLerpValue
	{
		[[nodiscard]]
		ThrowingLerpValue lerp(const ThrowingLerpValue&, double) const
		{
			return{};
		}
	};

	static_assert(HasLerp<ConstLerpValue>);
	static_assert(not HasLerp<MutableOnlyLerpValue>);
	static_assert(not HasLerp<ThrowingLerpValue>);
	static_assert(noexcept(Math::Lerp(ConstLerpValue{}, ConstLerpValue{}, 0.5)));
	static_assert(noexcept(Math::Map(0.5, 0.0, 1.0, ConstLerpValue{}, ConstLerpValue{})));
}

TEST_CASE("Math::Lerp")
{
	CHECK(Math::Lerp(0.0, 10.0, 0.25) == 2.5);
	CHECK(Math::Lerp(10.0, 0.0, 0.25) == 7.5);
	CHECK(Math::Lerp(uint32{ 10 }, uint32{ 0 }, 0.5) == 5.0);

	const ConstLerpValue result = Math::Lerp(ConstLerpValue{ 2.0 }, ConstLerpValue{ 6.0 }, 0.25);
	CHECK(result.value == 3.0);
}

TEST_CASE("Math::InvLerp")
{
	CHECK(Math::InvLerp(0.0, 10.0, 2.5) == 0.25);
	CHECK(Math::InvLerp(10.0, 0.0, 2.5) == 0.75);
	CHECK(Math::InvLerp(0.0, 10.0, 15.0) == 1.5);
}

TEST_CASE("Math::LerpAngle")
{
	CHECK(Math::LerpAngle(0.0, Math::Pi, 0.5) == doctest::Approx(-Math::HalfPi));
	CHECK(Math::LerpAngle(uint32{ 10 }, uint32{ 0 }, 0.5)
		== doctest::Approx(Math::LerpAngle(10.0, 0.0, 0.5)));
}

TEST_CASE("Math::Eerp")
{
	CHECK(Math::Eerp(2.0, 8.0, 0.0) == 2.0);
	CHECK(Math::Eerp(2.0, 8.0, 0.5) == doctest::Approx(4.0));
	CHECK(Math::Eerp(2.0, 8.0, 1.0) == 8.0);
	CHECK(Math::Eerp(-2, -8, 0.5) == doctest::Approx(-4.0));
}

TEST_CASE("Math::Map")
{
	CHECK(Math::Map(2.5, 0.0, 10.0, 0.0, 100.0) == 25.0);
	CHECK(Math::Map(15.0, 0.0, 10.0, 0.0, 100.0) == 150.0);
	CHECK(Math::Map(0.5, 0.0, 1.0, uint32{ 10 }, uint32{ 0 }) == 5.0);

	const ConstLerpValue result = Math::Map(0.25, 0.0, 1.0, ConstLerpValue{ 2.0 }, ConstLerpValue{ 6.0 });
	CHECK(result.value == 3.0);
}

TEST_CASE("Math::MoveTowards")
{
	CHECK(Math::MoveTowards(0.0, 10.0, 2.0) == 2.0);
	CHECK(Math::MoveTowards(9.0, 10.0, 2.0) == 10.0);
	CHECK(Math::MoveTowards(1.0, 2.0, 0.0) == 1.0);
	CHECK(Math::MoveTowards(1.0, 2.0, -1.0) == 1.0);
	CHECK(Math::MoveTowards(1.0, 1.0, -1.0) == 1.0);

	CHECK(Math::MoveTowards(Vec2{ 0.0, 0.0 }, Vec2{ 3.0, 4.0 }, 2.5) == Vec2{ 1.5, 2.0 });
	CHECK(Math::MoveTowards(Vec2{ 1.0, 1.0 }, Vec2{ 2.0, 2.0 }, -1.0) == Vec2{ 1.0, 1.0 });
	CHECK(Math::MoveTowards(Vec2{ 1.0, 1.0 }, Vec2{ 1.0, 1.0 }, -1.0) == Vec2{ 1.0, 1.0 });
	CHECK(Math::MoveTowards(ColorF{ 0.1, 0.2, 0.3, 0.4 }, ColorF{ 0.5, 0.6, 0.7, 0.8 }, -1.0)
		== ColorF{ 0.1, 0.2, 0.3, 0.4 });
	CHECK(Math::MoveTowards(HSV{ 120.0, 0.5, 0.75, 0.25 }, HSV{ 240.0, 0.25, 0.5, 0.75 }, -1.0)
		== HSV{ 120.0, 0.5, 0.75, 0.25 });
}

TEST_CASE("Math::Damp")
{
	CHECK(Math::Damp(0.0, 1.0, 1.0, 0.0) == 0.0);
	CHECK(Math::Damp(0.0, 1.0, 1.0, 1.0) == doctest::Approx(1.0 - std::exp(-1.0)));

	const Vec2 result = Math::Damp(Vec2{ 0.0, 0.0 }, Vec2{ 1.0, 2.0 }, 1.0, 1.0);
	CHECK(result.x == doctest::Approx(1.0 - std::exp(-1.0)));
	CHECK(result.y == doctest::Approx(2.0 * (1.0 - std::exp(-1.0))));
}

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

	SUBCASE("zero smoothTime")
	{
		double velocity = 0.0;
		const double result = Math::SmoothDamp(1.0, 2.0, velocity, 0.0, unspecified, 0.1);

		CHECK(std::isfinite(result));
		CHECK(std::isfinite(velocity));
		CHECK(1.0 < result);
		CHECK(result <= 2.0);
	}

	SUBCASE("negative smoothTime")
	{
		double velocity = 0.0;
		const double result = Math::SmoothDamp(1.0, 2.0, velocity, -1.0, unspecified, 0.1);

		CHECK(std::isfinite(result));
		CHECK(std::isfinite(velocity));
		CHECK(1.0 < result);
		CHECK(result <= 2.0);
	}

	SUBCASE("negative maxSpeed")
	{
		{
			double velocity = 0.0;
			const double result = Math::SmoothDamp(1.0, 2.0, velocity, 0.2, -1.0, 0.1);

			CHECK(result == 1.0);
			CHECK(velocity == 0.0);
		}

		{
			Vec2 velocity{ 0.0, 0.0 };
			const Vec2 result = Math::SmoothDamp(Vec2{ 1.0, 1.0 }, Vec2{ 2.0, 2.0 }, velocity, 0.2, -1.0, 0.1);

			CHECK(result == Vec2{ 1.0, 1.0 });
			CHECK(velocity == Vec2{ 0.0, 0.0 });
		}
	}

	SUBCASE("current equals target")
	{
		{
			double velocity = -1.0;
			const double result = Math::SmoothDamp(1.0, 1.0, velocity, 0.2, unspecified, 0.1);

			CHECK(result == 1.0);
			CHECK(velocity == 0.0);
		}

		{
			double velocity = 1.0;
			const double result = Math::SmoothDamp(1.0, 1.0, velocity, 0.2, unspecified, 0.1);

			CHECK(result == 1.0);
			CHECK(velocity == 0.0);
		}

		{
			Vec2 velocity{ -1.0, 0.0 };
			const Vec2 result = Math::SmoothDamp(Vec2{ 1.0, 1.0 }, Vec2{ 1.0, 1.0 }, velocity, 0.2, unspecified, 0.1);

			CHECK(result == Vec2{ 1.0, 1.0 });
			CHECK(velocity == Vec2{ 0.0, 0.0 });
		}

		{
			constexpr HSV value{ 120.0, 0.5, 0.75, 0.25 };
			HSV velocity{ 1.0, 0.1, 0.2, 0.3 };
			const HSV result = Math::SmoothDamp(value, value, velocity, 0.2, unspecified, 0.1);

			CHECK(result == value);
			CHECK(velocity == HSV::Zero());
		}
	}
}
