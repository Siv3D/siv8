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
