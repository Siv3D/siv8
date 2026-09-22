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

TEST_CASE("Geometry2D.Contains.SuperEllipse.concave_in_convex_containers")
{
	for (const double n : { 0.125, 0.5, 0.9, 0.999 })
	{
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 32, -64 }, Vec2{ 134217728, -134217728 } })
		{
			for (const double gap : { -1.0e-5, 0.0, 1.0e-5 })
			{
				CAPTURE(n, offset, gap);
				const SuperEllipse shape{ offset, (4 + gap), 2, n };
				auto Check = [&](const auto& container)
				{
					CHECK(Geometry2D::Contains(container, shape) == (gap <= 0));
					CHECK(container.contains(shape) == (gap <= 0));
				};
				// The horizontal tips touch each container when gap == 0.
				Check(Circle{ offset, 4 });
				Check(Ellipse{ offset, 4, 3 });
				Check(SuperEllipse{ offset, 4, 3, 4 });
				Check(Triangle{ offset + Vec2{ 0, -4 }, offset + Vec2{ 8, 4 }, offset + Vec2{ -8, 4 } });
				Check(Quad{ offset + Vec2{ 0, -2 }, offset + Vec2{ 4, 0 }, offset + Vec2{ 0, 2 }, offset + Vec2{ -4, 0 } });
				Check(RoundRect{ RectF{ offset - Vec2{ 4, 3 }, 8, 6 }, 1 });
				Check(RectF{ offset - Vec2{ 4, 3 }, 8, 6 });
			}
		}
	}
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.support_scale")
{
	for (const double n : { 1.0, 1.000001, 1.001, 1.1, 2.0, 4.0 })
	{
		for (const double scale : { 0.01, 1.0, 1000.0 })
		{
			for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 32, -64 }, Vec2{ 134217728, -134217728 } })
			{
				CAPTURE(n, scale, offset);
				const SuperEllipse shape{ offset, (4 * scale), (4 * scale), n };
				auto Check = [&](const auto& container, const bool expected)
				{
					CHECK(Geometry2D::Contains(container, shape) == expected);
					CHECK(container.contains(shape) == expected);
				};
				// For 1 <= n <= 4 the circumradius is at most 4*2^(1/4) < 5.
				Check(Circle{ offset, (5 * scale) }, true);
				if (n == 1.001)
				{
					// The bounding diamond protrudes, so the support calculation is needed.
					Check(Circle{ offset, (4.001 * scale) }, true);
				}
				Check(Circle{ offset, (3 * scale) }, false);
				Check(Ellipse{ offset, (5 * scale), (6 * scale) }, true);
				Check(Ellipse{ offset, (3 * scale), (6 * scale) }, false);
				Check(SuperEllipse{ offset, (5 * scale), (6 * scale), 4 }, true);
				Check(SuperEllipse{ offset, (3 * scale), (6 * scale), 4 }, false);
				Check(Triangle{ offset + Vec2{ 0, -10 } * scale, offset + Vec2{ 10, 5 } * scale, offset + Vec2{ -10, 5 } * scale }, true);
				Check(RoundRect{ RectF{ offset - Vec2{ 5, 5 } * scale, SizeF{ 10, 10 } * scale }, (2 * scale) }, true);
				Check(RoundRect{ RectF{ offset - Vec2{ 3, 3 } * scale, SizeF{ 6, 6 } * scale }, scale }, false);
				Check(Polygon{ RectF{ offset - Vec2{ 5, 5 } * scale, SizeF{ 10, 10 } * scale }.asPolygon() }, true);
				Check(Polygon{ RectF{ offset - Vec2{ 3, 3 } * scale, SizeF{ 6, 6 } * scale }.asPolygon() }, false);
			}
		}
	}
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.holes_and_empty_shapes")
{
	const SuperEllipse shape{ 0, 0, 4, 4, 0.5 };
	const Array<Vec2> outer{ { 0, -5 }, { 5, 0 }, { 0, 5 }, { -5, 0 } };
	const Polygon diamond{ outer };
	CHECK(diamond.contains(shape));
	// All four tips are inside, but the central hole excludes part of the shape.
	const Polygon centralHole{ outer, { { { -0.1, -0.1 }, { -0.1, 0.1 }, { 0.1, 0.1 }, { 0.1, -0.1 } } } };
	CHECK_FALSE(centralHole.contains(shape));
	// This hole lies in the bounding box, outside the convex hull of the shape.
	const Polygon outsideHole{ outer, { { { 2.9, 1.4 }, { 2.9, 1.6 }, { 3.1, 1.6 }, { 3.1, 1.4 } } } };
	CHECK(outsideHole.contains(shape));
	CHECK(MultiPolygon{ Polygon{}, diamond }.contains(shape));
	CHECK_FALSE(MultiPolygon{ centralHole }.contains(shape));
	CHECK_FALSE(MultiPolygon{ RectF{ -5, -5, 5, 10 }.asPolygon(), RectF{ 0, -5, 5, 10 }.asPolygon() }.contains(shape));
	CHECK(shape.contains(shape));
	CHECK(shape.contains(SuperEllipse{ 0, 0, 0.5, 0.5, 0.25 }));
	CHECK_FALSE(Circle{ 0, 0, 0 }.contains(shape));
	CHECK_FALSE(Ellipse{ 0, 0, 0, 4 }.contains(shape));
	CHECK_FALSE(RoundRect{ 0, 0, 0, 0, 1 }.contains(shape));
	CHECK_FALSE(Polygon{}.contains(shape));
	CHECK_FALSE(MultiPolygon{}.contains(shape));
	for (const double n : { 0.5, 1.0, 1.001, 4.0 })
	{
		CAPTURE(n);
		const Circle circle{ 0, 0, 4 };
		CHECK_FALSE(circle.contains(SuperEllipse{ 0, 0, 0, 0, n }));
		CHECK(circle.contains(SuperEllipse{ 0, 0, 4, 0, n }));
		CHECK(circle.contains(SuperEllipse{ 0, 0, 0, 4, n }));
		CHECK_FALSE(circle.contains(SuperEllipse{ 0, 0, 0, 5, n }));
	}
}
