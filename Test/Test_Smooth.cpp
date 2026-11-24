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
	template <class Type>
	void CheckSetStateAliases(const Type& value, const Type& target, const Type& velocity)
	{
		const Type initialStates[] = { value, target, velocity };

		for (size_t valueIndex = 0; valueIndex < 3; ++valueIndex)
		{
			for (size_t velocityIndex = 0; velocityIndex < 3; ++velocityIndex)
			{
				CAPTURE(valueIndex, velocityIndex);
				Smooth<Type> smooth{ value };
				smooth.setState(value, velocity).setTarget(target);
				const Type* const states[] = { &smooth.value(), &smooth.target(), &smooth.velocity() };

				CHECK(&smooth.setState(*states[valueIndex], *states[velocityIndex]) == &smooth);
				CHECK(smooth.value() == initialStates[valueIndex]);
				CHECK(smooth.target() == initialStates[valueIndex]);
				CHECK(smooth.velocity() == initialStates[velocityIndex]);
			}
		}
	}
}

TEMPLATE_TEST_CASE("Smooth.setState.aliased arguments", "", float, double)
{
	using Float = TestType;
	CheckSetStateAliases(Float{ 1 }, Float{ 2 }, Float{ 3 });
	CheckSetStateAliases(Vector2D<Float>{ 1, 2 }, Vector2D<Float>{ 3, 4 }, Vector2D<Float>{ 5, 6 });
	CheckSetStateAliases(Vector3D<Float>{ 1, 2, 3 }, Vector3D<Float>{ 4, 5, 6 }, Vector3D<Float>{ 7, 8, 9 });
	CheckSetStateAliases(Vector4D<Float>{ 1, 2, 3, 4 }, Vector4D<Float>{ 5, 6, 7, 8 }, Vector4D<Float>{ 9, 10, 11, 12 });
}

TEST_CASE("Smooth.setState.aliased color arguments")
{
	CheckSetStateAliases(ColorF{ 0.1, 0.2, 0.3, 0.4 }, ColorF{ 0.5, 0.6, 0.7, 0.8 }, ColorF{ 0.9, 1.0, 1.1, 1.2 });
	CheckSetStateAliases(HSV{ 120.0, 0.2, 0.3, 0.4 }, HSV{ 240.0, 0.5, 0.6, 0.7 }, HSV{ 10.0, 0.8, 0.9, 1.0 });
}

TEST_CASE("SmoothHSV.zero velocity")
{
	constexpr HSV value{ 120.0, 0.5, 0.75, 0.25 };
	constexpr HSV velocity{ 1.0, 0.1, 0.2, 0.3 };

	SmoothHSV defaultSmooth;
	CHECK(defaultSmooth.velocity() == HSV::Zero());

	SmoothHSV smooth{ value };
	CHECK(smooth.velocity() == HSV::Zero());

	smooth.update(0.2, unspecified, (1.0 / 60.0));
	CHECK(smooth.value() == value);
	CHECK(smooth.velocity() == HSV::Zero());

	smooth.setState(value);
	CHECK(smooth.velocity() == HSV::Zero());

	smooth.setState(value, velocity);
	CHECK(smooth.velocity() == velocity);

	smooth.clearTarget();
	CHECK(smooth.velocity() == HSV::Zero());

	smooth.setState(value, velocity).jumpTo(HSV{ 240.0, 0.25, 0.5, 0.75 });
	CHECK(smooth.velocity() == HSV::Zero());

	smooth.setState(value, velocity).setTarget(HSV{ 240.0, 0.25, 0.5, 0.75 }).jumpToTarget();
	CHECK(smooth.velocity() == HSV::Zero());
}

TEST_CASE("Smooth.update.non-positive deltaTime")
{
	{
		SmoothDouble smooth{ 1.0 };
		smooth.setState(1.0, 3.0);

		CHECK(smooth.update(0.2, unspecified, 0.0) == 1.0);
		CHECK(smooth.velocity() == 3.0);

		smooth.setTarget(2.0);
		CHECK(smooth.update(0.2, unspecified, -0.1) == 1.0);
		CHECK(smooth.velocity() == 3.0);
	}

	{
		constexpr HSV value{ 120.0, 0.5, 0.75, 0.25 };
		constexpr HSV velocity{ 1.0, 0.1, 0.2, 0.3 };

		SmoothHSV smooth{ value };
		smooth.setState(value, velocity);

		CHECK(smooth.update(0.2, unspecified, 0.0) == value);
		CHECK(smooth.velocity() == velocity);
	}
}

TEST_CASE("Smooth.update.lowering maxSpeed")
{
	constexpr double smoothTime = 0.2;
	constexpr double deltaTime = (1.0 / 60.0);
	SmoothDouble moving{ 0.0 };
	moving.setTarget(1000.0);

	for (int i = 0; i < 60; ++i)
	{
		moving.update(smoothTime, 100.0, deltaTime);
	}

	for (const double maxSpeed : { 1.0, 0.0 })
	{
		for (const bool useDuration : { false, true })
		{
			CAPTURE(maxSpeed, useDuration);
			SmoothDouble smooth = moving;
			const double result = useDuration
				? smooth.update(Duration{ smoothTime }, maxSpeed, deltaTime)
				: smooth.update(smoothTime, maxSpeed, deltaTime);

			CHECK((result - moving.value()) > (maxSpeed * deltaTime));
			CHECK(smooth.velocity() > maxSpeed);
			CHECK(smooth.velocity() < moving.velocity());
		}
	}
}
