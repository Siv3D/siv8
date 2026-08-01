
//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cmath>
# include <limits>
# include <Siv3D/Geometry2D/Distance.hpp>
# include <Siv3D/Geometry2D/SignedDistance.hpp>

namespace
{
	[[nodiscard]]
	bool Near(const double actual, const double expected, const double tolerance = 1.0e-9)
	{
		return (Abs(actual - expected) <= tolerance);
	}
}

TEST_CASE("Geometry2D.SignedDistance.RectF")
{
	const RectF rect{ 0, 0, 10, 10 };

	CHECK(Near(Geometry2D::SignedDistance(rect, Vec2{ 5, 5 }), -5.0));
	CHECK(Near(Geometry2D::SignedDistance(rect, Vec2{ 15, 3 }), 5.0));
	CHECK(Geometry2D::SignedDistance(rect, Vec2{ 10, 3 }) == 0.0);
	CHECK(not std::signbit(Geometry2D::SignedDistance(rect, Vec2{ 10, 3 })));

	const auto closest = Geometry2D::ClosestPointOnBoundary(rect, Vec2{ 15, 3 });
	REQUIRE(closest.has_value());
	CHECK(*closest == Vec2{ 10, 3 });
	CHECK(Near(Geometry2D::Distance(rect, Vec2{ 15, 3 }),
		Geometry2D::SignedDistance(rect, Vec2{ 15, 3 })));
	CHECK(Geometry2D::Distance(rect, Vec2{ 5, 5 }) == 0.0);
	CHECK(Near(closest->distanceFrom(Vec2{ 15, 3 }),
		Abs(Geometry2D::SignedDistance(rect, Vec2{ 15, 3 }))));

	const RectF segment{ 0, 0, 0, 10 };
	CHECK(Near(Geometry2D::SignedDistance(segment, Vec2{ 1, 5 }), 1.0));
	CHECK(Geometry2D::SignedDistance(segment, Vec2{ 0, 5 }) == 0.0);

	const RectF empty{ 0, 0, 0, 0 };
	CHECK(not Geometry2D::ClosestPointOnBoundary(empty, Vec2{ 0, 0 }));
	CHECK(std::isinf(Geometry2D::SignedDistance(empty, Vec2{ 0, 0 })));
}

TEST_CASE("Geometry2D.SignedDistance.Rect")
{
	const Rect rect{ 0, 0, 10, 10 };
	const RectF rectF{ rect };

	for (const Vec2 point : { Vec2{ 5, 5 }, Vec2{ 15, 2 }, Vec2{ 0, 7 } })
	{
		CHECK(Near(Geometry2D::SignedDistance(rect, point),
			Geometry2D::SignedDistance(rectF, point)));
	}
}

TEST_CASE("Geometry2D.SignedDistance.Circle")
{
	const Circle circle{ Vec2{ 0, 0 }, 5 };

	CHECK(Near(Geometry2D::SignedDistance(circle, Vec2{ 0, 0 }), -5.0));
	CHECK(Near(Geometry2D::SignedDistance(circle, Vec2{ 8, 0 }), 3.0));
	CHECK(Geometry2D::SignedDistance(circle, Vec2{ 3, 4 }) == 0.0);

	const auto centerClosest = Geometry2D::ClosestPointOnBoundary(circle, Vec2{ 0, 0 });
	REQUIRE(centerClosest.has_value());
	CHECK(Near(centerClosest->distanceFrom(circle.center), 5.0));

	const Circle empty{ Vec2{ 2, 3 }, 0 };
	CHECK(not Geometry2D::ClosestPointOnBoundary(empty, Vec2{ 2, 3 }));
	CHECK(std::isinf(Geometry2D::SignedDistance(empty, Vec2{ 2, 3 })));
}

TEST_CASE("Geometry2D.SignedDistance.Ellipse_SuperEllipse")
{
	const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
	CHECK(Near(Geometry2D::SignedDistance(ellipse, Vec2{ 0, 0 }), -3.0, 1.0e-8));
	CHECK(Near(Geometry2D::SignedDistance(ellipse, Vec2{ 0, 5 }), 2.0, 1.0e-8));
	CHECK(Geometry2D::SignedDistance(ellipse, Vec2{ 5, 0 }) == 0.0);

	const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };
	CHECK(Near(Geometry2D::SignedDistance(superEllipse, Vec2{ 0, 0 }), -3.0, 1.0e-8));
	CHECK(Near(Geometry2D::SignedDistance(superEllipse, Vec2{ 7, 0 }), 2.0, 1.0e-8));
	CHECK(Geometry2D::SignedDistance(superEllipse, Vec2{ 5, 0 }) == 0.0);

	const Ellipse segment{ Vec2{ 0, 0 }, 0, 5 };
	CHECK(Near(Geometry2D::SignedDistance(segment, Vec2{ 2, 0 }), 2.0));
	CHECK(Geometry2D::SignedDistance(segment, Vec2{ 0, 2 }) == 0.0);
}

TEST_CASE("Geometry2D.SignedDistance.Triangle_Quad")
{
	const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
	CHECK(Geometry2D::SignedDistance(triangle, Vec2{ 1, 1 }) < 0.0);
	CHECK(Near(Geometry2D::SignedDistance(triangle, Vec2{ 5, 5 }), 0.0));
	CHECK(Geometry2D::SignedDistance(triangle, Vec2{ 9, 9 }) > 0.0);

	const Triangle pointTriangle{ Vec2{ 2, 3 }, Vec2{ 2, 3 }, Vec2{ 2, 3 } };
	CHECK(Near(Geometry2D::SignedDistance(pointTriangle, Vec2{ 5, 7 }), 5.0));
	const auto triangleClosest = Geometry2D::ClosestPointOnBoundary(pointTriangle, Vec2{ 5, 7 });
	REQUIRE(triangleClosest.has_value());
	CHECK(*triangleClosest == Vec2{ 2, 3 });

	const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
	CHECK(Near(Geometry2D::SignedDistance(quad, Vec2{ 5, 5 }), -5.0));
	CHECK(Near(Geometry2D::SignedDistance(quad, Vec2{ 12, 5 }), 2.0));
}

TEST_CASE("Geometry2D.SignedDistance.RoundRect")
{
	const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

	CHECK(Near(Geometry2D::SignedDistance(roundRect, Vec2{ 5, 5 }), -5.0));
	CHECK(Near(Geometry2D::SignedDistance(roundRect, Vec2{ 12, 5 }), 2.0));
	CHECK(Geometry2D::SignedDistance(roundRect, Vec2{ 5, 0 }) == 0.0);
	CHECK(Near(Geometry2D::SignedDistance(roundRect, Vec2{ 0, 0 }),
		std::sqrt(8.0) - 2.0, 1.0e-9));
}

TEST_CASE("Geometry2D.SignedDistance.Polygon_Hole")
{
	const Polygon polygon{
		Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } },
		Array<Array<Vec2>>{ Array<Vec2>{ Vec2{ 3, 3 }, Vec2{ 3, 7 }, Vec2{ 7, 7 }, Vec2{ 7, 3 } } }
	};

	CHECK(Near(Geometry2D::SignedDistance(polygon, Vec2{ 1, 5 }), -1.0));
	CHECK(Near(Geometry2D::SignedDistance(polygon, Vec2{ 5, 5 }), 2.0));
	CHECK(Geometry2D::SignedDistance(polygon, Vec2{ 3, 5 }) == 0.0);
	CHECK(Near(Geometry2D::SignedDistance(polygon, Vec2{ 12, 5 }), 2.0));

	const auto holeClosest = Geometry2D::ClosestPointOnBoundary(polygon, Vec2{ 5, 5 });
	REQUIRE(holeClosest.has_value());
	CHECK(Near(holeClosest->distanceFrom(Vec2{ 5, 5 }), 2.0));
}

TEST_CASE("Geometry2D.SignedDistance.MultiPolygon")
{
	const Polygon a{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } } };
	const Polygon b{ Array<Vec2>{ Vec2{ 6, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 4 }, Vec2{ 6, 4 } } };
	const MultiPolygon multi{ a, b };

	CHECK(Near(Geometry2D::SignedDistance(multi, Vec2{ 2, 2 }), -2.0));
	CHECK(Near(Geometry2D::SignedDistance(multi, Vec2{ 5, 2 }), 1.0));
	CHECK(Geometry2D::SignedDistance(multi, Vec2{ 6, 2 }) == 0.0);

	const Polygon touchingA{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 2, 0 }, Vec2{ 2, 2 }, Vec2{ 0, 2 } } };
	const Polygon touchingB{ Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const MultiPolygon pointTouch{ touchingA, touchingB };
	CHECK(Geometry2D::SignedDistance(pointTouch, Vec2{ 2, 2 }) == 0.0);
	CHECK(Near(Geometry2D::SignedDistance(pointTouch, Vec2{ 3, 1 }), 1.0));

	const MultiPolygon empty;
	CHECK(not Geometry2D::ClosestPointOnBoundary(empty, Vec2{ 0, 0 }));
	CHECK(std::isinf(Geometry2D::SignedDistance(empty, Vec2{ 0, 0 })));
}

TEST_CASE("Geometry2D.SignedDistance.CurvedScale")
{
	const Ellipse large{ Vec2{ 1.0e9, -1.0e9 }, 5.0e5, 3.0e5 };
	const Vec2 outside{ 1.0e9 + 7.0e5, -1.0e9 };
	const auto closest = Geometry2D::ClosestPointOnBoundary(large, outside);
	REQUIRE(closest.has_value());
	CHECK(Near(Geometry2D::SignedDistance(large, outside), 2.0e5, 1.0e-5));
	CHECK(Near(closest->distanceFrom(outside), 2.0e5, 1.0e-5));
}

TEST_CASE("Geometry2D.SignedDistance.Point_Overloads")
{
	const Circle circle{ Vec2{ 0, 0 }, 5 };
	CHECK(Near(Geometry2D::SignedDistance(circle, Point{ 8, 0 }), 3.0));
	const auto closest = Geometry2D::ClosestPointOnBoundary(circle, Point{ 8, 0 });
	REQUIRE(closest.has_value());
	CHECK(*closest == Vec2{ 5, 0 });
}
