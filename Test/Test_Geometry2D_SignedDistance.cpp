
//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <array>
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

TEST_CASE("Geometry2D.SignedDistance.Ellipse.AxisClosestPoints")
{
	for (const bool transpose : { false, true })
	{
		const Vec2 center{ 7, -11 };
		const Ellipse ellipse{ center, (transpose ? Vec2{ 3, 5 } : Vec2{ 5, 3 }) };
		const std::array<Vec2, 4> points{ Vec2{ 0, 0 }, Vec2{ 2, 0 }, Vec2{ 4, 0 }, Vec2{ 7, 0 } };
		const std::array<Vec2, 4> expected{ Vec2{ 0, 3 }, Vec2{ 25.0 / 8, 3 * std::sqrt(39.0) / 8 }, Vec2{ 5, 0 }, Vec2{ 5, 0 } };
		const std::array<double, 4> distances{ -3.0, -std::sqrt(6.75), -1.0, 2.0 };
		for (size_t i = 0; i < points.size(); ++i)
		{
			CAPTURE(transpose, i);
			const Vec2 point = (center + (transpose ? points[i].yx() : points[i]));
			const Vec2 boundary = (center + (transpose ? expected[i].yx() : expected[i]));
			auto Check = [&](const auto& shape)
			{
				const auto closest = Geometry2D::ClosestPointOnBoundary(shape, point);
				REQUIRE(closest);
				const Vec2 delta = (*closest - center);
				CHECK(Vec2{ Abs(delta.x), Abs(delta.y) }.distanceFrom(boundary - center) < 1.0e-12);
				CHECK(Near(Geometry2D::SignedDistance(shape, point), distances[i], 1.0e-12));
			};
			Check(ellipse);
			Check(SuperEllipse{ ellipse, 2.0 });
		}
	}
}

TEST_CASE("Geometry2D.SignedDistance.Ellipse.NormalOffsets")
{
	const Vec2 center{ 7, -11 };
	for (const Vec2 axes : { Vec2{ 5, 3 }, Vec2{ 3, 5 }, Vec2{ 5, 5 }, Vec2{ 1000, 1 }, Vec2{ 1, 1000 }, Vec2{ 5, 4.999999999 } })
	{
		const Ellipse ellipse{ center, axes };
		for (const double angle : { 0.0, 1.0e-8, 0.01, 0.4, 0.9, (Math::HalfPi - 1.0e-8), Math::HalfPi })
		{
			const Vec2 unit{ std::cos(angle), std::sin(angle) };
			const double tangentLength = std::hypot((axes.x * unit.y), (axes.y * unit.x));
			const double curvatureRadius = (tangentLength * tangentLength * tangentLength / (axes.x * axes.y));
			const double gap = (0.01 * Min({ axes.x, axes.y, curvatureRadius }));
			const double tolerance = (1.0e-11 * Max(axes.x, axes.y));
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				const Vec2 boundary = (center + sign * axes * unit);
				const Vec2 normal = (sign * unit / axes).normalized();
				for (const double offset : { -gap, 0.0, gap })
				{
					CAPTURE(axes, angle, sign, offset);
					const Vec2 point = (boundary + normal * offset);
					auto Check = [&](const auto& shape)
					{
						const auto closest = Geometry2D::ClosestPointOnBoundary(shape, point);
						REQUIRE(closest);
						CHECK(closest->distanceFrom(boundary) <= tolerance);
						const double distance = Geometry2D::SignedDistance(shape, point);
						CHECK(Near(distance, offset, tolerance));
						if (offset == 0.0)
						{
							CHECK(distance == 0.0);
							CHECK(not std::signbit(distance));
						}
						if (0.0 < offset)
						{
							const auto pair = Geometry2D::ClosestPoints(shape, point);
							REQUIRE(pair);
							CHECK(pair->pointA.distanceFrom(*closest) <= tolerance);
							CHECK(Near(pair->distance, offset, tolerance));
						}
					};
					Check(ellipse);
					Check(SuperEllipse{ ellipse, 2.0 });
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.SignedDistance.Diamond")
{
	const Vec2 center{ 7, -11 };
	const SuperEllipse diamond{ center, SizeF{ 4, 3 }, 1.0 };
	for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
	{
		const Vec2 boundary = (center + sign * Vec2{ 2, 1.5 });
		const Vec2 normal = (sign * Vec2{ 0.6, 0.8 });
		for (const double offset : { -0.5, 0.0, 0.5 })
		{
			const Vec2 point = (boundary + normal * offset);
			const auto closest = Geometry2D::ClosestPointOnBoundary(diamond, point);
			REQUIRE(closest);
			CHECK(closest->distanceFrom(boundary) < 1.0e-12);
			CHECK(Near(Geometry2D::SignedDistance(diamond, point), offset, 1.0e-12));
		}
	}
	CHECK(Near(Geometry2D::SignedDistance(diamond, center), -2.4, 1.0e-12));
	const auto centerClosest = Geometry2D::ClosestPointOnBoundary(diamond, center);
	REQUIRE(centerClosest);
	CHECK(Near(centerClosest->distanceFrom(center), 2.4, 1.0e-12));
	CHECK(Geometry2D::SignedDistance(diamond, center + Vec2{ 4, 0 }) == 0.0);
	CHECK(Geometry2D::ClosestPointOnBoundary(diamond, center + Vec2{ 5, 0 }) == (center + Vec2{ 4, 0 }));
}

TEST_CASE("Geometry2D.SignedDistance.SuperEllipse.AxisAndNeighborhood")
{
	const Vec2 center{ 7, -11 };
	for (const double n : { 2.01, 4.0, 8.0, 16.0, 64.0 })
	{
		for (const Vec2 axes : { Vec2{ 100, 100 }, Vec2{ 80, 5 }, Vec2{ 3, 200 } })
		{
			const SuperEllipse shape{ center, axes, n };
			const double gap = (1.0e-5 * Min(axes.x, axes.y));
			const double tolerance = (1.0e-11 * Max(axes.x, axes.y));
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				for (const double coordinate : { 0.0, 1.0e-8, 1.0e-5, 0.001, 0.1, 0.6 })
				{
					const double other = std::pow((1.0 - std::pow(coordinate, n)), (1.0 / n));
					for (const Vec2 normalized : { Vec2{ coordinate, other }, Vec2{ other, coordinate } })
					{
						CAPTURE(n, axes, sign, normalized);
						const Vec2 boundary = (center + sign * axes * normalized);
						const Vec2 normal = (sign * Vec2{ (std::pow(normalized.x, (n - 1.0)) / axes.x),
							(std::pow(normalized.y, (n - 1.0)) / axes.y) }).normalized();
						const double onBoundary = Geometry2D::SignedDistance(shape, boundary);
						CHECK(onBoundary == 0.0);
						CHECK(not std::signbit(onBoundary));
						for (const double offset : { -gap, gap })
						{
							const Vec2 point = (boundary + normal * offset);
							const auto closest = Geometry2D::ClosestPointOnBoundary(shape, point);
							REQUIRE(closest);
							CHECK(Near(closest->distanceFrom(point), Abs(offset), tolerance));
							CHECK(closest->distanceFrom(boundary) <= (1.0e-8 * Max(axes.x, axes.y)));
							CHECK(Near(Geometry2D::SignedDistance(shape, point), offset, tolerance));
							CHECK(Near(Geometry2D::Distance(shape, point), Max(offset, 0.0), tolerance));
							const auto pair = Geometry2D::ClosestPoints(shape, point);
							const auto reversed = Geometry2D::ClosestPoints(point, shape);
							REQUIRE(pair);
							REQUIRE(reversed);
							CHECK(pair->pointB == point);
							CHECK(reversed->pointA == point);
							CHECK(pair->pointA == reversed->pointB);
							CHECK(Near(pair->distance, Max(offset, 0.0), tolerance));
							CHECK(Near(pair->pointA.distanceFrom(pair->pointB), pair->distance, tolerance));
							if (0.0 < offset)
							{
								CHECK(pair->pointA == *closest);
							}
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.SignedDistance.SuperEllipse.Center")
{
	for (const double n : { 2.01, 4.0, 8.0, 16.0, 64.0 })
	{
		for (const Vec2 axes : { Vec2{ 100, 100 }, Vec2{ 80, 5 }, Vec2{ 3, 200 } })
		{
			const SuperEllipse shape{ Vec2{ 7, -11 }, axes, n };
			CHECK(Near(Geometry2D::SignedDistance(shape, shape.center), -Min(axes.x, axes.y)));
			const auto closest = Geometry2D::ClosestPointOnBoundary(shape, shape.center);
			REQUIRE(closest);
			CHECK(Near(closest->distanceFrom(shape.center), Min(axes.x, axes.y)));
		}
	}
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

TEST_CASE("Geometry2D.SignedDistance.Polygon.point_collapse")
{
	const Polygon source{
		Array<Vec2>{ { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } },
		Array<Array<Vec2>>{ { { 3, 3 }, { 3, 7 }, { 7, 7 }, { 7, 3 } } }
	};
	const Vec2 position{ 2, 3 };
	const Polygon point = source.scaledFrom(position, 0.0);
	REQUIRE(not point.isEmpty());

	auto CheckPointBoundary = [&](const auto& shape)
	{
		CHECK(Near(Geometry2D::SignedDistance(shape, Vec2{ 5, 7 }), 5.0));
		CHECK(Near(Geometry2D::SignedDistance(shape, Point{ 5, 7 }), 5.0));
		const auto closest = Geometry2D::ClosestPointOnBoundary(shape, Vec2{ 5, 7 });
		REQUIRE(closest.has_value());
		CHECK(*closest == position);
		CHECK(Geometry2D::ClosestPointOnBoundary(shape, Point{ 5, 7 }) == closest);
		CHECK(Geometry2D::ClosestPointOnBoundary(shape, position) == closest);
		const double onBoundary = Geometry2D::SignedDistance(shape, position);
		CHECK(onBoundary == 0.0);
		CHECK(not std::signbit(onBoundary));
	};
	CheckPointBoundary(point);
	CheckPointBoundary(MultiPolygon{ point });
	CheckPointBoundary(MultiPolygon{ Polygon{}, point, point.movedBy(30, 0) });
	CheckPointBoundary(MultiPolygon{ point.movedBy(30, 0), point, Polygon{} });

	const Polygon area = source.movedBy(30, 0);
	for (const auto& multi : { MultiPolygon{ point, Polygon{}, area }, MultiPolygon{ area, point } })
	{
		CheckPointBoundary(multi);
		CHECK(Near(Geometry2D::SignedDistance(multi, Vec2{ 31, 5 }), -1.0));
		const auto closest = Geometry2D::ClosestPointOnBoundary(multi, Vec2{ 31, 5 });
		REQUIRE(closest.has_value());
		CHECK(*closest == Vec2{ 30, 5 });
	}

	const Polygon segment = source.scaledFrom(position, Vec2{ 0, 1 });
	CHECK(Near(Geometry2D::SignedDistance(segment, Vec2{ 5, 7 }), 3.0));
	CHECK(Geometry2D::SignedDistance(segment, Vec2{ 2, 7 }) == 0.0);
	const auto segmentClosest = Geometry2D::ClosestPointOnBoundary(segment, Vec2{ 5, 7 });
	REQUIRE(segmentClosest.has_value());
	CHECK(*segmentClosest == Vec2{ 2, 7 });

	CHECK(std::isinf(Geometry2D::SignedDistance(Polygon{}, position)));
	CHECK(not Geometry2D::ClosestPointOnBoundary(Polygon{}, position));
	CHECK(std::isinf(Geometry2D::SignedDistance(MultiPolygon{ Polygon{} }, position)));
	CHECK(not Geometry2D::ClosestPointOnBoundary(MultiPolygon{ Polygon{} }, position));
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
