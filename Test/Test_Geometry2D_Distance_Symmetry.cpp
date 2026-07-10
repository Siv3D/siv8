//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Distance.hpp>

TEST_CASE("Geometry2D.Distance.SymmetryAndWitnessOrder")
{
	const Point point{ 7, -1 };
	const Vec2 vec2{ -3.0, 5.0 };
	const Line line{ Vec2{ -2, 0 }, Vec2{ 2, 0 } };
	const LineString lineString{ Vec2{ -4, -3 }, Vec2{ -2, -1 }, Vec2{ -4, 1 } };
	const Bezier2 bezier2{ Vec2{ 5, -2 }, Vec2{ 7, 1 }, Vec2{ 5, 4 } };
	const Bezier3 bezier3{ Vec2{ -6, 5 }, Vec2{ -5, 7 }, Vec2{ -2, 7 }, Vec2{ -1, 5 } };
	const Rect rect{ 10, 10, 4, 3 };
	const RectF rectF{ -12, 8, 3.5, 2.5 };
	const Circle circle{ Vec2{ 12, -8 }, 2.0 };
	const Ellipse ellipse{ Vec2{ -10, -8 }, 3.0, 1.5 };
	const SuperEllipse superEllipse{ Vec2{ 0, 12 }, SizeF{ 2.5, 1.5 }, 4.0 };
	const Triangle triangle{ Vec2{ 15, 2 }, Vec2{ 18, 3 }, Vec2{ 16, 6 } };
	const Quad quad{ Vec2{ -17, -1 }, Vec2{ -14, -1 }, Vec2{ -14, 2 }, Vec2{ -17, 2 } };
	const RoundRect roundRect{ RectF{ 5, 12, 5, 4 }, 1.0 };
	const Polygon polygon{ Array<Vec2>{ Vec2{ -3, -14 }, Vec2{ 1, -14 }, Vec2{ 1, -11 }, Vec2{ -3, -11 } } };
	const MultiPolygon multiPolygon{ Polygon{ Array<Vec2>{ Vec2{ 18, -14 }, Vec2{ 21, -14 }, Vec2{ 21, -11 }, Vec2{ 18, -11 } } } };

	size_t failureCount = 0;

	auto CheckPair = [&](const auto& a, const auto& b)
	{
		const double ab = Geometry2D::Distance(a, b);
		const double ba = Geometry2D::Distance(b, a);
		failureCount += (Abs(ab - ba) > 1.0e-8);

		const auto closestAB = Geometry2D::ClosestPoints(a, b);
		const auto closestBA = Geometry2D::ClosestPoints(b, a);
		failureCount += (static_cast<bool>(closestAB) != static_cast<bool>(closestBA));

		if (closestAB && closestBA)
		{
			failureCount += (Abs(closestAB->distance - closestBA->distance) > 1.0e-8);
			failureCount += (Abs(closestAB->pointA.distanceFrom(closestAB->pointB)
				- closestAB->distance) > 1.0e-8);
		}
	};

#define CHECK_ROW(A) \
	CheckPair(A, point); CheckPair(A, vec2); CheckPair(A, line); CheckPair(A, lineString); \
	CheckPair(A, bezier2); CheckPair(A, bezier3); CheckPair(A, rect); CheckPair(A, rectF); \
	CheckPair(A, circle); CheckPair(A, ellipse); CheckPair(A, superEllipse); CheckPair(A, triangle); \
	CheckPair(A, quad); CheckPair(A, roundRect); CheckPair(A, polygon); CheckPair(A, multiPolygon)

	CHECK_ROW(point);
	CHECK_ROW(vec2);
	CHECK_ROW(line);
	CHECK_ROW(lineString);
	CHECK_ROW(bezier2);
	CHECK_ROW(bezier3);
	CHECK_ROW(rect);
	CHECK_ROW(rectF);
	CHECK_ROW(circle);
	CHECK_ROW(ellipse);
	CHECK_ROW(superEllipse);
	CHECK_ROW(triangle);
	CHECK_ROW(quad);
	CHECK_ROW(roundRect);
	CHECK_ROW(polygon);
	CHECK_ROW(multiPolygon);

#undef CHECK_ROW

	CHECK(failureCount == 0);
}

TEST_CASE("Geometry2D.Distance.UniqueWitnessReversal")
{
	const Vec2 point{ 4, 3 };
	const Line line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
	const auto pointLine = Geometry2D::ClosestPoints(point, line);
	const auto linePoint = Geometry2D::ClosestPoints(line, point);
	REQUIRE(pointLine);
	REQUIRE(linePoint);
	CHECK(pointLine->pointA.distanceFrom(linePoint->pointB) <= 1.0e-12);
	CHECK(pointLine->pointB.distanceFrom(linePoint->pointA) <= 1.0e-12);
	CHECK(pointLine->distance == Approx(linePoint->distance));

	const Circle circleA{ Vec2{ 0, 0 }, 2.0 };
	const Circle circleB{ Vec2{ 7, 4 }, 1.0 };
	const auto ab = Geometry2D::ClosestPoints(circleA, circleB);
	const auto ba = Geometry2D::ClosestPoints(circleB, circleA);
	REQUIRE(ab);
	REQUIRE(ba);
	CHECK(ab->pointA.distanceFrom(ba->pointB) <= 1.0e-8);
	CHECK(ab->pointB.distanceFrom(ba->pointA) <= 1.0e-8);
	CHECK(Abs(ab->distance - ba->distance) <= 1.0e-10);
}
