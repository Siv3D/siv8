//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cmath>
# include <Siv3D/Geometry2D/Distance.hpp>

namespace
{
	void CheckWitnessConsistency(const ClosestPoints2D& result, const double tolerance = 1.0e-8)
	{
		CHECK(result.distance >= 0.0);
		CHECK(Abs(result.pointA.distanceFrom(result.pointB) - result.distance) <= tolerance);
	}
}

TEST_CASE("Geometry2D.Distance.Empty")
{
	const LineString emptyLineString;
	const Polygon emptyPolygon;
	const MultiPolygon emptyMultiPolygon;
	const Circle emptyCircle{ Vec2{ 0, 0 }, 0.0 };
	const RectF emptyRect{ 0, 0, 0, 0 };
	const Vec2 point{ 1, 2 };

	CHECK(std::isinf(Geometry2D::Distance(emptyLineString, point)));
	CHECK(std::isinf(Geometry2D::Distance(point, emptyPolygon)));
	CHECK(std::isinf(Geometry2D::Distance(emptyMultiPolygon, emptyCircle)));
	CHECK(std::isinf(Geometry2D::Distance(emptyRect, point)));
	CHECK(not Geometry2D::ClosestPoints(emptyLineString, point));
	CHECK(not Geometry2D::ClosestPoints(point, emptyPolygon));
	CHECK(not Geometry2D::ClosestPoints(emptyCircle, point));
}

TEST_CASE("Geometry2D.Distance.PointAndLine")
{
	{
		const auto result = Geometry2D::ClosestPoints(Point{ 0, 0 }, Vec2{ 3, 4 });
		REQUIRE(result);
		CHECK((result->pointA == Vec2{ 0, 0 }));
		CHECK((result->pointB == Vec2{ 3, 4 }));
		CHECK(result->distance == Approx(5.0));
		CHECK(Geometry2D::Distance(Point{ 0, 0 }, Vec2{ 3, 4 }) == Approx(5.0));
		CheckWitnessConsistency(*result);
	}

	{
		const Line line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
		const auto result = Geometry2D::ClosestPoints(Vec2{ 4, 3 }, line);
		REQUIRE(result);
		CHECK((result->pointA == Vec2{ 4, 3 }));
		CHECK((result->pointB == Vec2{ 4, 0 }));
		CHECK(result->distance == Approx(3.0));
		CheckWitnessConsistency(*result);
	}

	{
		const Line a{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
		const Line b{ Vec2{ 4, 3 }, Vec2{ 7, 3 } };
		const auto result = Geometry2D::ClosestPoints(a, b);
		REQUIRE(result);
		CHECK(result->distance == Approx(3.0));
		CHECK(Geometry2D::Intersects(result->pointA, a));
		CHECK(Geometry2D::Intersects(result->pointB, b));
		CheckWitnessConsistency(*result);
	}
}

TEST_CASE("Geometry2D.Distance.IntersectionAndContainment")
{
	{
		const Line horizontal{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
		const Line vertical{ Vec2{ 5, -5 }, Vec2{ 5, 5 } };
		const auto result = Geometry2D::ClosestPoints(horizontal, vertical);
		REQUIRE(result);
		CHECK(result->distance == 0.0);
		CHECK(result->pointA == result->pointB);
		CHECK(Geometry2D::Intersects(result->pointA, horizontal));
		CHECK(Geometry2D::Intersects(result->pointB, vertical));
	}

	{
		const RectF outer{ 0, 0, 20, 20 };
		const Circle inner{ Vec2{ 10, 10 }, 2.0 };
		const auto result = Geometry2D::ClosestPoints(outer, inner);
		REQUIRE(result);
		CHECK(result->distance == 0.0);
		CHECK(result->pointA == result->pointB);
		CHECK(Geometry2D::Intersects(result->pointA, outer));
		CHECK(Geometry2D::Intersects(result->pointB, inner));
	}

	{
		const Circle a{ Vec2{ 0, 0 }, 5.0 };
		const Circle tangent{ Vec2{ 10, 0 }, 5.0 };
		CHECK(Geometry2D::Distance(a, tangent) == 0.0);
	}
}

TEST_CASE("Geometry2D.Distance.AnalyticAreaCases")
{
	{
		const Circle a{ Vec2{ 0, 0 }, 3.0 };
		const Circle b{ Vec2{ 10, 0 }, 2.0 };
		const auto result = Geometry2D::ClosestPoints(a, b);
		REQUIRE(result);
		CHECK(Abs(result->distance - 5.0) <= 1.0e-10);
		CHECK(Abs(result->pointA.x - 3.0) <= 1.0e-9);
		CHECK(Abs(result->pointB.x - 8.0) <= 1.0e-9);
		CheckWitnessConsistency(*result);
	}

	{
		const RectF rect{ 0, 0, 4, 4 };
		const Circle circle{ Vec2{ 10, 2 }, 2.0 };
		const auto result = Geometry2D::ClosestPoints(rect, circle);
		REQUIRE(result);
		CHECK(Abs(result->distance - 4.0) <= 1.0e-9);
		CheckWitnessConsistency(*result);
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5.0, 3.0 };
		const Vec2 point{ 9, 0 };
		const auto result = Geometry2D::ClosestPoints(ellipse, point);
		REQUIRE(result);
		CHECK(Abs(result->distance - 4.0) <= 1.0e-9);
		CHECK(Abs(result->pointA.x - 5.0) <= 1.0e-9);
		CheckWitnessConsistency(*result);
	}
}

TEST_CASE("Geometry2D.Distance.DegenerateGeometry")
{
	{
		const RectF verticalSegment{ 2, 0, 0, 10 };
		const Vec2 point{ 5, 4 };
		const auto result = Geometry2D::ClosestPoints(verticalSegment, point);
		REQUIRE(result);
		CHECK(result->distance == Approx(3.0));
		CHECK((result->pointA == Vec2{ 2, 4 }));
		CheckWitnessConsistency(*result);
	}

	{
		const Triangle segment{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } };
		CHECK(Geometry2D::Distance(segment, Vec2{ 4, 3 }) == Approx(3.0));
	}

	{
		const Bezier2 pointCurve{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };
		CHECK(Geometry2D::Distance(pointCurve, Vec2{ 4, 5 }) == Approx(5.0));
	}
}

TEST_CASE("Geometry2D.Distance.PolygonHoleAndMultiPolygon")
{
	const Polygon polygon{
		Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 20 }, Vec2{ 0, 20 } },
		Array<Array<Vec2>>{ Array<Vec2>{ Vec2{ 6, 6 }, Vec2{ 6, 14 }, Vec2{ 14, 14 }, Vec2{ 14, 6 } } }
	};

	{
		const auto result = Geometry2D::ClosestPoints(polygon, Vec2{ 10, 10 });
		REQUIRE(result);
		CHECK(result->distance == Approx(4.0));
		CHECK(Geometry2D::Intersects(result->pointA, polygon));
		CHECK((result->pointB == Vec2{ 10, 10 }));
		CheckWitnessConsistency(*result);
	}

	{
		const Polygon left{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } } };
		const Polygon right{ Array<Vec2>{ Vec2{ 10, 0 }, Vec2{ 14, 0 }, Vec2{ 14, 4 }, Vec2{ 10, 4 } } };
		const MultiPolygon multi{ left, right };
		CHECK(Geometry2D::Distance(multi, Vec2{ 7, 2 }) == Approx(3.0));
	}
}

TEST_CASE("Geometry2D.Distance.Curves")
{
	{
		const Bezier2 curve{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } };
		const Vec2 point{ 5, 8 };
		const auto result = Geometry2D::ClosestPoints(curve, point);
		REQUIRE(result);
		CHECK(Abs(result->distance - 3.0) <= 1.0e-7);
		CheckWitnessConsistency(*result, 1.0e-7);
	}

	{
		const SuperEllipse shape{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };
		const Vec2 point{ 9, 0 };
		const auto result = Geometry2D::ClosestPoints(shape, point);
		REQUIRE(result);
		CHECK(Abs(result->distance - 4.0) <= 1.0e-7);
		CheckWitnessConsistency(*result, 1.0e-7);
	}
}
