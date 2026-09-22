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

TEST_CASE("Geometry2D.Contains.Circle_Ellipse.circular")
{
	struct TestCase
	{
		Vec2 offset;
		double radius;
		bool expected;
	};
	const TestCase cases[] = {
		{ { 0, 0 }, 10, true },        // Same region
		{ { 0, 0 }, 9.999, true },     // Small clearance
		{ { 0, 0 }, 11, false },
		{ { 3, 4 }, 5, true },        // Internal tangency
		{ { 3, 4 }, 4, true },
		{ { 3, 4 }, 5.0001, false },  // Just beyond internal tangency
		{ { 10, 0 }, 1, false },
		{ { 20, 0 }, 10, false },     // External tangency
		{ { 30, 0 }, 1, false },
		{ { 0, 0 }, 0, false },       // Empty target
	};

	for (const Vec2 center : { Vec2{ 0, 0 }, Vec2{ 32, -64 } })
	{
		const Circle circle{ center, 10 };
		const Ellipse ellipse{ center, 10, 10 };

		for (const auto& test : cases)
		{
			CAPTURE(center, test.offset, test.radius);
			const Circle innerCircle{ center + test.offset, test.radius };
			const Ellipse innerEllipse{ innerCircle.center, test.radius, test.radius };
			CHECK(Geometry2D::Contains(circle, innerEllipse) == test.expected);
			CHECK(Geometry2D::Contains(ellipse, innerCircle) == test.expected);
			CHECK(circle.contains(innerEllipse) == test.expected);
			CHECK(ellipse.contains(innerCircle) == test.expected);
		}
	}
}

TEST_CASE("Geometry2D.Contains.Circle_Ellipse.empty_and_non_circular")
{
	const Circle circle{ 0, 0, 10 };
	const Ellipse ellipse{ 0, 0, 10, 10 };
	const Circle emptyCircle{ 0, 0, -0.0 };
	const Ellipse emptyEllipse{ 0, 0, -0.0, 0.0 };
	CHECK_FALSE(emptyCircle.contains(ellipse));
	CHECK_FALSE(emptyEllipse.contains(circle));
	CHECK_FALSE(circle.contains(emptyEllipse));
	CHECK_FALSE(ellipse.contains(emptyCircle));
	CHECK_FALSE(emptyCircle.contains(emptyEllipse));
	CHECK_FALSE(emptyEllipse.contains(emptyCircle));

	CHECK(circle.contains(Ellipse{ 0, 0, 5, 3 }));
	CHECK_FALSE(circle.contains(Ellipse{ 0, 0, 5, 11 }));
	CHECK(Ellipse{ 0, 0, 12, 10 }.contains(Circle{ 0, 0, 5 }));
	CHECK_FALSE(Ellipse{ 0, 0, 12, 10 }.contains(Circle{ 0, 0, 11 }));
	CHECK(circle.contains(Ellipse{ 0, 0, 0, 10 }));
	CHECK(circle.contains(Ellipse{ 0, 0, 10, 0 }));
	CHECK_FALSE(Ellipse{ 0, 0, 0, 10 }.contains(Circle{ 0, 0, 1 }));
	CHECK_FALSE(Ellipse{ 0, 0, 10, 0 }.contains(Circle{ 0, 0, 1 }));
}
