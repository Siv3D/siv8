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

	template <class Bezier>
	void CheckBezierPointDistance(const Bezier& curve, const Vec2& query, const Vec2& expected, const double tolerance = 1e-8)
	{
		const double distance = query.distanceFrom(expected);
		const auto CheckTarget = [&](const auto& target)
		{
			const auto ab = Geometry2D::ClosestPoints(curve, target);
			const auto ba = Geometry2D::ClosestPoints(target, curve);
			REQUIRE(ab);
			REQUIRE(ba);
			CHECK(ab->pointA.distanceFrom(expected) <= tolerance);
			CHECK(ab->pointB.distanceFrom(query) <= tolerance);
			CHECK(ba->pointA == ab->pointB);
			CHECK(ba->pointB == ab->pointA);
			CHECK(Abs(ab->distance - distance) <= tolerance);
			CHECK(Abs(Geometry2D::Distance(curve, target) - distance) <= tolerance);
			CHECK(Geometry2D::Distance(curve, target) == Geometry2D::Distance(target, curve));
			CheckWitnessConsistency(*ab, tolerance);
		};
		CheckTarget(query);
		CheckTarget(Line{ query, query });
		CHECK(curve.computeClosestPoint(query).distanceFrom(expected) <= tolerance);
		CHECK(curve.pointAt(curve.computeClosestT(query)).distanceFrom(expected) <= tolerance);
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
		CHECK(result->distance == Test::Approx(5.0));
		CHECK(Geometry2D::Distance(Point{ 0, 0 }, Vec2{ 3, 4 }) == Test::Approx(5.0));
		CheckWitnessConsistency(*result);
	}

	{
		const Line line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
		const auto result = Geometry2D::ClosestPoints(Vec2{ 4, 3 }, line);
		REQUIRE(result);
		CHECK((result->pointA == Vec2{ 4, 3 }));
		CHECK((result->pointB == Vec2{ 4, 0 }));
		CHECK(result->distance == Test::Approx(3.0));
		CheckWitnessConsistency(*result);
	}

	{
		const Line a{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
		const Line b{ Vec2{ 4, 3 }, Vec2{ 7, 3 } };
		const auto result = Geometry2D::ClosestPoints(a, b);
		REQUIRE(result);
		CHECK(result->distance == Test::Approx(3.0));
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

TEST_CASE("Geometry2D.Distance.Ellipse.IntersectionWitnesses")
{
	const auto CheckPair = [](const auto& a, const auto& b)
	{
		CHECK(Geometry2D::Distance(a, b) == 0.0);
		const auto result = Geometry2D::ClosestPoints(a, b);
		const auto reversed = Geometry2D::ClosestPoints(b, a);
		REQUIRE(result.has_value());
		REQUIRE(reversed.has_value());
		CHECK(result->distance == 0.0);
		CHECK(result->pointA == result->pointB);
		CHECK(reversed->distance == 0.0);
		CHECK(reversed->pointA == reversed->pointB);
		const auto CheckInside = [](const Vec2& point, const auto& shape)
		{
			Vec2 axes;
			if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Circle>)
			{
				axes = { shape.r, shape.r };
			}
			else
			{
				axes = shape.axes;
			}
			CHECK(((point - shape.center) / axes).lengthSq() <= (1.0 + 1.0e-12));
		};
		CheckInside(result->pointA, a);
		CheckInside(result->pointB, b);
		CheckInside(reversed->pointA, b);
		CheckInside(reversed->pointB, a);
	};
	const Vec2 direction{ 0.6, 0.8 };
	for (const double distance : { 129.9999, 130.0, 70.0, 70.0001, 60.0, 0.0 })
	{
		const Ellipse b{ (direction * distance), 30, 30 };
		CheckPair(Ellipse{ 0, 0, 100, 100 }, b);
		CheckPair(Circle{ 0, 0, 100 }, b);
		CheckPair(Ellipse{ 0, 0, 100, 30 }, Ellipse{ (b.center * Vec2{ 1, 0.3 }), 30, 9 });
	}
	CheckPair(Ellipse{ 0, 0, 100, 30 }, Ellipse{ 0, 0, 100, 30 });
	for (const double gap : { -1.0e-12, 0.0, 1.0e-12 })
	{
		const Ellipse b{ (direction * (130.0 + gap)), 30, 30 };
		CheckPair(Ellipse{ 0, 0, 100, 100 }, b);
		CheckPair(Circle{ 0, 0, 100 }, b);
	}
}

TEST_CASE("Geometry2D.Distance.Ellipse.SeparatedWitnesses")
{
	const auto Support = [](const Vec2& axes, const Vec2& normal)
	{
		const Vec2 scaled = (axes * normal);
		return (axes * scaled.normalized());
	};
	const auto CheckPair = [](const auto& a, const Ellipse& b,
		const Vec2& expectedA, const Vec2& expectedB, const double gap, const double tolerance)
	{
		CHECK(Abs(Geometry2D::Distance(a, b) - gap) <= tolerance);
		CHECK(Abs(Geometry2D::Distance(b, a) - gap) <= tolerance);
		const auto pair = Geometry2D::ClosestPoints(a, b);
		const auto reversed = Geometry2D::ClosestPoints(b, a);
		REQUIRE(pair);
		REQUIRE(reversed);
		CHECK(Abs(pair->distance - gap) <= tolerance);
		CHECK(Abs(reversed->distance - gap) <= tolerance);
		CHECK(pair->pointA.distanceFrom(expectedA) <= tolerance);
		CHECK(pair->pointB.distanceFrom(expectedB) <= tolerance);
		CHECK(reversed->pointA.distanceFrom(expectedB) <= tolerance);
		CHECK(reversed->pointB.distanceFrom(expectedA) <= tolerance);
		CheckWitnessConsistency(*pair, tolerance);
		CheckWitnessConsistency(*reversed, tolerance);
	};

	for (const auto& [axesA, axesB] : {
		std::pair{ Vec2{ 100, 100 }, Vec2{ 30, 30 } },
		std::pair{ Vec2{ 100, 100 }, Vec2{ 30, 70 } },
		std::pair{ Vec2{ 100, 30 }, Vec2{ 20, 70 } },
		std::pair{ Vec2{ 100, 1 }, Vec2{ 0.5, 70 } } })
	{
		for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 1.0e-8 },
			Vec2{ 1.0e-8, 1 }, Vec2{ 0.6, 0.8 }, Vec2{ std::cos(0.37), std::sin(0.37) } })
		{
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				const Vec2 normal = (direction.normalized() * sign);
				for (const double scale : { 0.01, 1.0, 100.0 })
				{
					for (const double unscaledGap : { 0.0001, 0.01, 10.0 })
					{
						CAPTURE(axesA, axesB, normal, scale, unscaledGap);
						const double gap = (unscaledGap * scale);
						const double tolerance = (1.0e-9 * scale);
						const Ellipse a{ (Vec2{ 7, -11 } * scale), (axesA * scale) };
						const Vec2 expectedA = (a.center + Support(a.axes, normal));
						const Vec2 expectedB = (expectedA + normal * gap);
						const Ellipse b{ (expectedB + Support((axesB * scale), normal)), (axesB * scale) };
						CheckPair(a, b, expectedA, expectedB, gap, tolerance);
						if (axesA.x == axesA.y)
						{
							CheckPair(Circle{ a.center, a.a }, b, expectedA, expectedB, gap, tolerance);
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.Ellipse.PointAndLineWitnesses")
{
	const auto CheckPair = [](const auto& a, const Ellipse& b,
		const Vec2& expectedA, const Vec2& expectedB, const double gap, const double tolerance)
	{
		CHECK(Abs(Geometry2D::Distance(a, b) - gap) <= tolerance);
		CHECK(Abs(Geometry2D::Distance(b, a) - gap) <= tolerance);
		const auto pair = Geometry2D::ClosestPoints(a, b);
		const auto reversed = Geometry2D::ClosestPoints(b, a);
		REQUIRE(pair);
		REQUIRE(reversed);
		CHECK(Abs(pair->distance - gap) <= tolerance);
		CHECK(Abs(reversed->distance - gap) <= tolerance);
		CHECK(pair->pointA.distanceFrom(expectedA) <= tolerance);
		CHECK(pair->pointB.distanceFrom(expectedB) <= tolerance);
		CHECK(reversed->pointA.distanceFrom(expectedB) <= tolerance);
		CHECK(reversed->pointB.distanceFrom(expectedA) <= tolerance);
		CHECK(Abs(((pair->pointB - b.center) / b.axes).lengthSq() - 1.0) <= 1.0e-10);
		CheckWitnessConsistency(*pair, tolerance);
		CheckWitnessConsistency(*reversed, tolerance);
	};
	for (const Vec2 axes : { Vec2{ 100, 30 }, Vec2{ 1, 100 }, Vec2{ 100, 100 } })
	{
		for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 1.0e-8 },
			Vec2{ 1.0e-8, 1 }, Vec2{ 0.6, 0.8 } })
		{
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				const Vec2 normal = (direction.normalized() * sign);
				const Vec2 tangent{ -normal.y, normal.x };
				for (const double scale : { 0.01, 1.0, 100.0 })
				{
					const Ellipse ellipse{ (Vec2{ 7, -11 } * scale), (axes * scale) };
					const Vec2 onEllipse = (ellipse.center + ellipse.axes * (ellipse.axes * normal).normalized());
					for (const double unscaledGap : { 0.0001, 0.01, 10.0 })
					{
						CAPTURE(axes, normal, scale, unscaledGap);
						const double gap = (unscaledGap * scale);
						const double tolerance = (1.0e-9 * scale);
						const Vec2 point = (onEllipse + normal * gap);
						CheckPair(point, ellipse, point, onEllipse, gap, tolerance);
						for (const Line line : {
							Line{ (point - tangent * (50 * scale)), (point + tangent * (100 * scale)) },
							Line{ point, (point + (normal * 70 + tangent * 30) * scale) },
							Line{ point, point } })
						{
							CheckPair(line, ellipse, point, onEllipse, gap, tolerance);
							CheckPair(Line{ line.end, line.start }, ellipse, point, onEllipse, gap, tolerance);
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.Ellipse.LinearBoundaries")
{
	const Ellipse ellipse{ 0, 0, 100, 30 };
	const Vec2 normal{ 0.6, 0.8 }, tangent{ -0.8, 0.6 };
	const Vec2 onEllipse = (ellipse.axes * (ellipse.axes * normal).normalized());
	const Vec2 point = (onEllipse + normal * 0.0001);
	const Line edge{ (point - tangent * 50), (point + tangent * 100) };
	const Quad quad{ edge.end, edge.start, (edge.start + normal * 100), (edge.end + normal * 100) };
	const auto CheckShape = [&](const auto& shape, const Vec2& expectedA, const Vec2& expectedB, const double gap)
	{
		CHECK(Abs(Geometry2D::Distance(shape, ellipse) - gap) <= 1.0e-9);
		CHECK(Abs(Geometry2D::Distance(ellipse, shape) - gap) <= 1.0e-9);
		const auto pair = Geometry2D::ClosestPoints(shape, ellipse);
		const auto reversed = Geometry2D::ClosestPoints(ellipse, shape);
		REQUIRE(pair);
		REQUIRE(reversed);
		CHECK(pair->pointA.distanceFrom(expectedA) <= 1.0e-9);
		CHECK(pair->pointB.distanceFrom(expectedB) <= 1.0e-9);
		CHECK(reversed->pointA.distanceFrom(expectedB) <= 1.0e-9);
		CHECK(reversed->pointB.distanceFrom(expectedA) <= 1.0e-9);
		CheckWitnessConsistency(*pair);
	};
	CheckShape(Triangle{ edge.start, edge.end, (point + normal * 100) }, point, onEllipse, 0.0001);
	CheckShape(quad, point, onEllipse, 0.0001);
	const Polygon polygon{ { quad.p0, quad.p1, quad.p2, quad.p3 } };
	REQUIRE(not polygon.isEmpty());
	CheckShape(polygon, point, onEllipse, 0.0001);
	CheckShape(MultiPolygon{ polygon, Rect{ -500, -500, 10, 10 }.asPolygon() },
		point, onEllipse, 0.0001);
	CheckShape(LineString{ edge.start, edge.end, quad.p2 }, point, onEllipse, 0.0001);
	CheckShape(RectF{ 100.01, -20, 100, 40 }, Vec2{ 100.01, 0 }, Vec2{ 100, 0 }, 0.01);
	CheckShape(Rect{ 101, -20, 100, 40 }, Vec2{ 101, 0 }, Vec2{ 100, 0 }, 1.0);

	for (const Line line : { Line{ { 100, -20 }, { 100, 20 } }, Line{ { 0, 0 }, { 150, 0 } },
		Line{ { 0, 0 }, { 1, 0 } }, Line{ { 0, 0 }, { 0, 0 } } })
	{
		CHECK(Geometry2D::Distance(line, ellipse) == 0.0);
		const auto pair = Geometry2D::ClosestPoints(line, ellipse);
		REQUIRE(pair);
		CHECK(pair->pointA == pair->pointB);
		CHECK(Geometry2D::Intersects(pair->pointA, line));
		CHECK(Geometry2D::Intersects(pair->pointB, ellipse));
	}
	const Ellipse collapsed{ 0, 0, 100, 0 };
	const Line above{ { -50, 3 }, { 50, 3 } };
	CHECK(Geometry2D::Distance(above, collapsed) == Test::Approx(3.0));
	const auto pair = Geometry2D::ClosestPoints(above, collapsed);
	REQUIRE(pair);
	CHECK(Geometry2D::Intersects(pair->pointA, above));
	CHECK(Geometry2D::Intersects(pair->pointB, collapsed));
	CheckWitnessConsistency(*pair);
	CHECK(std::isinf(Geometry2D::Distance(above, Ellipse{ 0, 0, 0, 0 })));
	CHECK(not Geometry2D::ClosestPoints(above, Ellipse{ 0, 0, 0, 0 }));
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConvexPairs")
{
	// Construct separated supporting lines with a prescribed normal and gap.
	const auto Support = [](const Vec2& axes, const double n, const Vec2& normal)
	{
		const double q = (n / (n - 1.0));
		const double x = Abs(axes.x * normal.x), y = Abs(axes.y * normal.y);
		const double scale = Max(x, y);
		const double denominator = std::pow((std::pow((x / scale), q) + std::pow((y / scale), q)), (1.0 / n));
		return Vec2{ std::copysign((axes.x * std::pow((x / scale), (q - 1.0)) / denominator), normal.x),
			std::copysign((axes.y * std::pow((y / scale), (q - 1.0)) / denominator), normal.y) };
	};
	const auto CheckInside = [](const Vec2& point, const SuperEllipse& shape)
	{
		const Vec2 p = ((point - shape.center) / shape.axes);
		CHECK((std::pow(Abs(p.x), shape.n) + std::pow(Abs(p.y), shape.n)) <= (1.0 + 1.0e-9));
	};
	for (const auto [nA, nB] : { std::pair{ 2.0, 1.25 }, { 2.0, 2.0 }, { 2.0, 4.0 },
		{ 2.0, 16.0 }, { 2.0, 64.0 }, { 4.0, 1.25 }, { 4.0, 16.0 }, { 64.0, 4.0 }, { 1.01, 1.25 } })
	{
		for (const auto& [axesA, axesB] : { std::pair{ Vec2{ 100, 100 }, Vec2{ 30, 70 } },
			std::pair{ Vec2{ 100, 30 }, Vec2{ 30, 70 } }, std::pair{ Vec2{ 8, 0.15 }, Vec2{ 34, 0.5 } },
			std::pair{ Vec2{ 13, 90 }, Vec2{ 13, 69 } } })
		{
			for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 1.0e-20 }, Vec2{ 1.0e-20, 1 },
				Vec2{ 0.6, 0.8 }, Vec2{ 0.026, 1 }, Vec2{ 1, 0.15 } })
			{
				for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
				{
					const Vec2 normal = (direction.normalized() * sign);
					for (const double scale : { 0.01, 1.0, 100.0 })
					{
						for (const double unscaledGap : { 0.0001, 0.01, 500.0 })
						{
							CAPTURE(nA, nB, axesA, axesB, normal, scale, unscaledGap);
							const double gap = (unscaledGap * scale), tolerance = (1.0e-9 * scale);
							const SuperEllipse a{ (Vec2{ 7, -11 } * scale), (axesA * scale), nA };
							const Vec2 expectedA = (a.center + Support(a.axes, nA, normal));
							const Vec2 expectedB = (expectedA + normal * gap);
							const SuperEllipse b{ (expectedB + Support((axesB * scale), nB, normal)), (axesB * scale), nB };
							const auto CheckPair = [&](const auto& first, const SuperEllipse& second)
							{
								CHECK(Abs(Geometry2D::Distance(first, second) - gap) <= tolerance);
								CHECK(Abs(Geometry2D::Distance(second, first) - gap) <= tolerance);
								const auto ab = Geometry2D::ClosestPoints(first, second);
								const auto ba = Geometry2D::ClosestPoints(second, first);
								REQUIRE(ab);
								REQUIRE(ba);
								CHECK(Abs(ab->distance - gap) <= tolerance);
								CHECK(Abs(ba->distance - gap) <= tolerance);
								CheckInside(ab->pointA, a);
								CheckInside(ab->pointB, b);
								CheckInside(ba->pointA, b);
								CheckInside(ba->pointB, a);
								CheckWitnessConsistency(*ab, tolerance);
								CheckWitnessConsistency(*ba, tolerance);
							};
							CheckPair(a, b);
							if (nA == 2.0)
							{
								CheckPair(Ellipse{ a.center, a.axes }, b);
								if (axesA.x == axesA.y)
								{
									CheckPair(Circle{ a.center, a.a }, b);
								}
							}
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConvexPairBoundaries")
{
	const SuperEllipse a{ 0, 0, 100, 30, 4 };
	CHECK(Geometry2D::Distance(a, SuperEllipse{ 130, 0, 30, 70, 16 }) == 0.0);
	CHECK(Geometry2D::Distance(a, Circle{ 0, 0, 1 }) == 0.0);
	const auto contained = Geometry2D::ClosestPoints(a, Circle{ 0, 0, 1 });
	REQUIRE(contained);
	CHECK(contained->pointA == contained->pointB);
	CHECK(contained->distance == 0.0);
	CHECK(std::isinf(Geometry2D::Distance(a, SuperEllipse{ 0, 0, 0, 0, 4 })));
	CHECK(not Geometry2D::ClosestPoints(a, SuperEllipse{ 0, 0, 0, 0, 4 }));
	const auto collapsed = Geometry2D::ClosestPoints(SuperEllipse{ 0, 0, 0, 10, 4 }, Circle{ 5, 0, 1 });
	REQUIRE(collapsed);
	CHECK(collapsed->distance == Test::Approx(4.0));
	CheckWitnessConsistency(*collapsed);
	const Ellipse ellipse{ 0, 0, 100, 30 };
	const SuperEllipse nearAxis{ 130.01, 1, 30, 70, 64 };
	CHECK(Abs(Geometry2D::Distance(ellipse, nearAxis) - 0.01) <= 1.0e-10);
	CHECK(Abs(Geometry2D::Distance(nearAxis, ellipse) - 0.01) <= 1.0e-10);
	const auto near = Geometry2D::ClosestPoints(ellipse, nearAxis);
	const auto reversed = Geometry2D::ClosestPoints(nearAxis, ellipse);
	REQUIRE(near);
	REQUIRE(reversed);
	CHECK(Abs(near->distance - 0.01) <= 1.0e-10);
	CHECK(Abs(reversed->distance - 0.01) <= 1.0e-10);
	CHECK(near->pointA.distanceFrom(Vec2{ 100, 0 }) <= 1.0e-9);
	CHECK(near->pointB.distanceFrom(Vec2{ 100.01, 0 }) <= 1.0e-9);
	CHECK(reversed->pointA.distanceFrom(near->pointB) <= 1.0e-9);
	CHECK(reversed->pointB.distanceFrom(near->pointA) <= 1.0e-9);
	CheckWitnessConsistency(*near);
	CheckWitnessConsistency(*reversed);
	for (const double n : { 0.5, 1.0 })
	{
		CHECK(Geometry2D::Distance(SuperEllipse{ 0, 0, 10, 10, n }, Circle{ 20, 0, 1 }) == Test::Approx(9.0));
	}
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.PointAndLineWitnesses")
{
	const auto Support = [](const Vec2& axes, const double n, const Vec2& normal)
	{
		const double q = (n / (n - 1.0));
		const Vec2 scaled = (axes * normal);
		const double m = Max(Abs(scaled.x), Abs(scaled.y));
		const double denominator = std::pow((std::pow(Abs(scaled.x / m), q) + std::pow(Abs(scaled.y / m), q)), (1.0 / n));
		return Vec2{ std::copysign((axes.x * std::pow(Abs(scaled.x / m), (q - 1.0)) / denominator), normal.x),
			std::copysign((axes.y * std::pow(Abs(scaled.y / m), (q - 1.0)) / denominator), normal.y) };
	};
	for (const double n : { 1.01, 1.25, 2.0, 4.0, 16.0, 64.0 })
	{
		for (const Vec2 axes : { Vec2{ 100, 30 }, Vec2{ 1, 100 } })
		{
			for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 1.0e-20 }, Vec2{ 1.0e-20, 1 }, Vec2{ 0.6, 0.8 } })
			{
				for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
				{
					const Vec2 normal = (direction.normalized() * sign), tangent{ -normal.y, normal.x };
					for (const double scale : { 0.01, 1.0, 100.0 })
					{
						const SuperEllipse shape{ (Vec2{ 7, -11 } * scale), (axes * scale), n };
						const Vec2 onShape = (shape.center + Support(shape.axes, n, normal));
						for (const double unscaledGap : { 0.0001, 0.01, 10.0 })
						{
							CAPTURE(n, axes, normal, scale, unscaledGap);
							const double gap = (unscaledGap * scale), tolerance = (1.0e-9 * scale);
							const Vec2 point = (onShape + normal * gap);
							const auto CheckPair = [&](const auto& first)
							{
								CHECK(Abs(Geometry2D::Distance(first, shape) - gap) <= tolerance);
								CHECK(Abs(Geometry2D::Distance(shape, first) - gap) <= tolerance);
								const auto pair = Geometry2D::ClosestPoints(first, shape), reversed = Geometry2D::ClosestPoints(shape, first);
								REQUIRE(pair);
								REQUIRE(reversed);
								CHECK(Abs(pair->distance - gap) <= tolerance);
								CHECK(Abs(reversed->distance - gap) <= tolerance);
								const Vec2 p = ((pair->pointB - shape.center) / shape.axes);
								CHECK((std::pow(Abs(p.x), n) + std::pow(Abs(p.y), n)) <= (1.0 + 1.0e-9));
								if constexpr (std::is_same_v<std::decay_t<decltype(first)>, Vec2>)
								{
									CHECK(pair->pointA == first);
								}
								else
								{
									CHECK(Geometry2D::Distance(pair->pointA, first) <= tolerance);
								}
								CHECK(reversed->pointA.distanceFrom(pair->pointB) <= tolerance);
								CHECK(reversed->pointB.distanceFrom(pair->pointA) <= tolerance);
								CheckWitnessConsistency(*pair, tolerance);
							};
							CheckPair(point);
							for (const Line line : {
								Line{ (point - tangent * (50 * scale)), (point + tangent * (100 * scale)) },
								Line{ point, (point + (normal * 70 + tangent * 30) * scale) }, Line{ point, point } })
							{
								CheckPair(line);
								CheckPair(Line{ line.end, line.start });
							}
						}
					}
				}
			}
		}
	}
	const SuperEllipse flat{ 0, 0, 100, 30, 64 };
	const Line nearAxis{ { 100.01, 1 }, { 100.01, 2 } };
	CHECK(Abs(Geometry2D::Distance(nearAxis, flat) - 0.01) <= 1.0e-10);
	const auto pair = Geometry2D::ClosestPoints(nearAxis, flat);
	REQUIRE(pair);
	CHECK(Abs(pair->pointA.x - 100.01) <= 1.0e-10);
	CHECK(Abs(pair->pointB.x - 100.0) <= 1.0e-10);
	CHECK(pair->pointB.y >= (1.0 - 1.0e-10));
	CHECK(pair->pointB.y <= (2.0 + 1.0e-10));
	CheckWitnessConsistency(*pair);
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.LinearBoundaries")
{
	for (const double n : { 1.0, 1.25, 2.0, 4.0, 64.0 })
	{
		CAPTURE(n);
		const SuperEllipse shape{ 0, 0, 100, 30, n };
		const auto CheckPair = [&](const auto& first, const double gap)
		{
			CHECK(Abs(Geometry2D::Distance(first, shape) - gap) <= 1.0e-9);
			CHECK(Abs(Geometry2D::Distance(shape, first) - gap) <= 1.0e-9);
			const auto pair = Geometry2D::ClosestPoints(first, shape), reversed = Geometry2D::ClosestPoints(shape, first);
			REQUIRE(pair);
			REQUIRE(reversed);
			CHECK(Abs(pair->distance - gap) <= 1.0e-9);
			CHECK(Abs(reversed->distance - gap) <= 1.0e-9);
			CheckWitnessConsistency(*pair);
		};
		const RectF rect{ 100.01, -20, 100, 40 };
		const Polygon polygon = rect.asPolygon();
		CheckPair(rect, 0.01);
		CheckPair(Rect{ 101, -20, 100, 40 }, 1.0);
		CheckPair(rect.asQuad(), 0.01);
		CheckPair(Triangle{ { 100.01, -20 }, { 200, 0 }, { 100.01, 20 } }, 0.01);
		CheckPair(polygon, 0.01);
		CheckPair(MultiPolygon{ polygon, Rect{ -500, -500, 10, 10 }.asPolygon() }, 0.01);
		CheckPair(LineString{ { 100.01, -20 }, { 100.01, 20 }, { 200, 20 } }, 0.01);
		for (const Line line : { Line{ { 100, -20 }, { 100, 20 } }, Line{ { 0, 0 }, { 150, 0 } },
			Line{ { 0, 0 }, { 1, 0 } }, Line{ { 0, 0 }, { 0, 0 } } })
		{
			CheckPair(line, 0.0);
		}
		const SuperEllipse collapsed{ 0, 0, 100, 0, n };
		const Line above{ { -50, 3 }, { 50, 3 } };
		CHECK(Geometry2D::Distance(above, collapsed) == Test::Approx(3.0));
		CHECK(std::isinf(Geometry2D::Distance(above, SuperEllipse{ 0, 0, 0, 0, n })));
		CHECK(not Geometry2D::ClosestPoints(above, SuperEllipse{ 0, 0, 0, 0, n }));
	}
	const SuperEllipse diamond{ 0, 0, 100, 30, 1 };
	const Vec2 normal = Vec2{ 0.3, 1 }.normalized();
	const Line edge{ Vec2{ 20, 24 } + normal * 0.0001, Vec2{ 80, 6 } + normal * 0.0001 };
	const Polygon reference{ { 100, 0 }, { 0, 30 }, { -100, 0 }, { 0, -30 } };
	for (const Line line : { edge, Line{ edge.end, edge.start }, Line{ edge.start, edge.start }, Line{ { 120, 10 }, { 200, 30 } } })
	{
		CHECK(Abs(Geometry2D::Distance(line, diamond) - Geometry2D::Distance(line, reference)) <= 1.0e-10);
		const auto pair = Geometry2D::ClosestPoints(line, diamond);
		REQUIRE(pair);
		CHECK(Geometry2D::Distance(pair->pointA, line) <= 1.0e-10);
		CHECK((Abs(pair->pointB.x / 100) + Abs(pair->pointB.y / 30)) <= (1.0 + 1.0e-12));
		CheckWitnessConsistency(*pair);
	}
	CHECK(Abs(Geometry2D::Distance(edge.start, diamond) - 0.0001) <= 1.0e-10);
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
		CHECK(result->distance == Test::Approx(3.0));
		CHECK((result->pointA == Vec2{ 2, 4 }));
		CheckWitnessConsistency(*result);
	}

	{
		const Triangle segment{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } };
		CHECK(Geometry2D::Distance(segment, Vec2{ 4, 3 }) == Test::Approx(3.0));
	}

	{
		const Bezier2 pointCurve{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };
		CHECK(Geometry2D::Distance(pointCurve, Vec2{ 4, 5 }) == Test::Approx(5.0));
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
		CHECK(result->distance == Test::Approx(4.0));
		CHECK(Geometry2D::Intersects(result->pointA, polygon));
		CHECK((result->pointB == Vec2{ 10, 10 }));
		CheckWitnessConsistency(*result);
	}

	{
		const Polygon left{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } } };
		const Polygon right{ Array<Vec2>{ Vec2{ 10, 0 }, Vec2{ 14, 0 }, Vec2{ 14, 4 }, Vec2{ 10, 4 } } };
		const MultiPolygon multi{ left, right };
		CHECK(Geometry2D::Distance(multi, Vec2{ 7, 2 }) == Test::Approx(3.0));
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

TEST_CASE("Geometry2D.Distance.Bezier.PointWitnesses")
{
	constexpr double u = 0.37;
	for (const double gap : { 1e-4, 1.0, 10.0 })
	{
		const Vec2 c0{ (-100 * u), (100 * u * u + gap) }, c1{ 100, (-200 * u) }, c2{ 0, 100 };
		const Bezier2 quadratic{ c0, (c0 + c1 / 2), (c0 + c1 + c2) };
		const Bezier3 cubic{ c0, (c0 + c1 / 3), (c0 + 2 * c1 / 3 + c2 / 3), (c0 + c1 + c2) };
		for (const double scale : { 0.01, 1.0, 100.0 })
		{
			for (const double angle : { 0.0, 0.7, Math::HalfPi })
			{
				CAPTURE(gap, scale, angle);
				const auto Transform = [&](const Vec2& p) { return ((p.rotated(angle) + Vec2{ 7, -11 }) * scale); };
				const Vec2 query = Transform({ 0, 0 }), expected = Transform({ 0, gap });
				const Bezier2 a{ Transform(quadratic.p0), Transform(quadratic.p1), Transform(quadratic.p2) };
				const Bezier3 b{ Transform(cubic.p0), Transform(cubic.p1), Transform(cubic.p2), Transform(cubic.p3) };
				CheckBezierPointDistance(a, query, expected, (1e-9 * scale));
				CheckBezierPointDistance(b, query, expected, (1e-9 * scale));
				CheckBezierPointDistance(a.reversed(), query, expected, (1e-9 * scale));
				CheckBezierPointDistance(b.reversed(), query, expected, (1e-9 * scale));
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.Bezier.PointBoundaries")
{
	CheckBezierPointDistance(Bezier2{ { 0, 0 }, { 30, 100 }, { 100, 0 } }, { -10, -10 }, { 0, 0 });
	CheckBezierPointDistance(Bezier3{ { 0, 0 }, { 30, 100 }, { 50, 50 }, { 100, 0 } }, { 110, 0 }, { 100, 0 });
	CheckBezierPointDistance(Bezier2{ { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 0, 0 }, { 3, 4 });
	CheckBezierPointDistance(Bezier3{ { 3, 4 }, { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 0, 0 }, { 3, 4 });
	CheckBezierPointDistance(Bezier2{ { 0, 0 }, { 100, 0 }, { 0, 0 } }, { 60, 2 }, { 50, 0 });
	CheckBezierPointDistance(Bezier3{ { 0, 0 }, { 100, 0 }, { -50, 0 }, { 50, 0 } }, { 10, 1 }, { 10, 0 });
	const Bezier2 a{ { 0, 0 }, { 30, 100 }, { 100, 0 } };
	const Bezier3 b{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 0, 0 } };
	for (const double t : { 0.0, 0.123, 0.5, 1.0 })
	{
		CAPTURE(t);
		CheckBezierPointDistance(a, a.pointAt(t), a.pointAt(t));
		CheckBezierPointDistance(b, b.pointAt(t), b.pointAt(t));
	}
	const auto pair = Geometry2D::ClosestPoints(Point{ 0, 151 }, b);
	REQUIRE(pair);
	CHECK(pair->distance == 1.0);
	CHECK((pair->pointB == Vec2{ 0, 150 }));
	CHECK(Geometry2D::Distance(b, Point{ 0, 151 }) == 1.0);
}

TEST_CASE("Geometry2D.Distance.Bezier.PointCompetingMinima")
{
	// x = 10000*(t - 0.5)^2, y = 1000*(t - 0.5)^3.
	const Vec2 c0{ 2500, -125 }, c1{ -10000, 750 }, c2{ 10000, -1500 }, c3{ 0, 1000 };
	const Bezier3 curve{ c0, (c0 + c1 / 3), (c0 + 2 * c1 / 3 + c2 / 3), (c0 + c1 + c2 + c3) };
	constexpr double v = 0.0055;
	const Vec2 expected{ (10000 * v * v), (1000 * v * v * v) };
	const Vec2 normal = Vec2{ (-3000 * v * v), (20000 * v) }.normalized();
	const Vec2 query = (expected + normal);
	CheckBezierPointDistance(curve, query, expected);
	CheckBezierPointDistance(curve.reversed(), query, expected);
	const auto pair = Geometry2D::ClosestPoints(curve, Line{ query, (query + normal) });
	REQUIRE(pair);
	CHECK(pair->pointA.distanceFrom(expected) <= 1e-8);
	CHECK(pair->pointB.distanceFrom(query) <= 1e-8);
	CHECK(Abs(pair->distance - 1.0) <= 1e-10);
}

TEST_CASE("Geometry2D.Distance.Bezier.RoundedShapeWitnesses")
{
	const auto CheckPair = [](const auto& curve, const auto& shape, const Vec2& a, const Vec2& b, const double tolerance)
	{
		const auto result = Geometry2D::ClosestPoints(curve, shape);
		const auto reversed = Geometry2D::ClosestPoints(shape, curve);
		REQUIRE(result);
		REQUIRE(reversed);
		CHECK(result->pointA.distanceFrom(a) <= tolerance);
		CHECK(result->pointB.distanceFrom(b) <= tolerance);
		CHECK(reversed->pointA == result->pointB);
		CHECK(reversed->pointB == result->pointA);
		CHECK(Abs(result->distance - a.distanceFrom(b)) <= tolerance);
		CHECK(Geometry2D::Distance(curve, shape) == result->distance);
		CHECK(Geometry2D::Distance(shape, curve) == result->distance);
		CheckWitnessConsistency(*result, tolerance);
	};
	for (const double scale : { 0.001, 1.0, 1000.0 })
	{
		for (const double sx : { -1.0, 1.0 })
		{
			for (const double sy : { -1.0, 1.0 })
			{
				const Vec2 normal{ (0.6 * sx), (0.8 * sy) };
				const Vec2 tangent{ normal.y, -normal.x };
				const Vec2 offset = (Vec2{ 17, -23 } * scale);
				const auto Transform = [&](const Vec2& p) { return (offset + (tangent * p.x + normal * p.y) * scale); };
				constexpr double u = 0.371;
				const Vec2 c0{ -100 * u, 50 * u * u }, c1{ 100, -100 * u }, c2{ 0, 50 };
				const Bezier2 quadratic{ Transform(c0), Transform(c0 + c1 / 2), Transform(c0 + c1 + c2) };
				const Bezier3 cubic{ Transform(c0), Transform(c0 + c1 / 3), Transform(c0 + 2 * c1 / 3 + c2 / 3), Transform(c0 + c1 + c2) };
				const Vec2 center = (offset - normal * (5 * scale));
				const Circle circle{ center, (2 * scale) };
				const Vec2 expected = (offset - normal * (3 * scale));
				// The selected corner faces the curve; the rectangle extends away from it.
				const RoundRect rounded{ (center.x + ((sx < 0) ? -2 : -18) * scale),
					(center.y + ((sy < 0) ? -2 : -28) * scale), (20 * scale), (30 * scale), (2 * scale) };
				const auto CheckCurve = [&](const auto& curve)
				{
					CheckPair(curve, circle, offset, expected, (1e-8 * scale));
					CheckPair(curve, rounded, offset, expected, (1e-8 * scale));
					CheckPair(curve.reversed(), circle, offset, expected, (1e-8 * scale));
					CheckPair(curve.reversed(), rounded, offset, expected, (1e-8 * scale));
				};
				CheckCurve(quadratic);
				CheckCurve(cubic);
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.Bezier.RoundedShapeBoundaries")
{
	const auto CheckPair = [](const auto& curve, const auto& shape, const double expected)
	{
		const auto result = Geometry2D::ClosestPoints(curve, shape);
		const auto reversed = Geometry2D::ClosestPoints(shape, curve);
		REQUIRE(result);
		REQUIRE(reversed);
		CHECK(Abs(result->distance - expected) <= 1e-9);
		CHECK(Geometry2D::Distance(curve, shape) == result->distance);
		CHECK(Geometry2D::Distance(shape, curve) == result->distance);
		CHECK(reversed->pointA == result->pointB);
		CHECK(reversed->pointB == result->pointA);
		CHECK(curve.computeClosestPoint(result->pointA).distanceFrom(result->pointA) <= 1e-8);
		CHECK(Geometry2D::Distance(result->pointB, shape) <= 1e-8);
		if (expected == 0.0)
		{
			CHECK(result->pointA == result->pointB);
		}
		CheckWitnessConsistency(*result);
	};
	const auto CheckCurve = [&](const auto& curve)
	{
		CheckPair(curve, Circle{ 0, 0, 1 }, 0.0);
		CheckPair(curve, Circle{ 0, 1, 1 }, 0.0);
		CheckPair(curve, Circle{ 0, 1.000001, 1 }, 0.000001);
		CheckPair(curve, Circle{ 0, 0.999999, 1 }, 0.0);
		CheckPair(curve, RoundRect{ -2, 1, 4, 6, 1 }, 1.0);
		CheckPair(curve, RoundRect{ -2, 0, 4, 6, 1 }, 0.0);
		CheckPair(curve, RoundRect{ -2, -3, 4, 6, 1 }, 0.0);
		CheckPair(curve, RoundRect{ -2, 1, 4, 6, 10 }, 1.0);
		CheckPair(curve, RoundRect{ -3, 1, 6, 4, 10 }, 1.0);
		CheckPair(curve, RoundRect{ -2, 1, 4, 4, 10 }, 1.0);
		CheckPair(curve, RoundRect{ -2, 1, 4, 6, 0 }, 1.0);
		CheckPair(curve, RoundRect{ -2, -3, 4, 6, 0 }, 0.0);
		CheckPair(curve, RoundRect{ 0, 1, 0, 6, 2 }, 1.0);
		CheckPair(curve, RoundRect{ -2, 1, 4, 0, 2 }, 1.0);
		for (const auto& empty : { RoundRect{ 0, 0, 0, 0, 0 }, RoundRect{ 0, 0, 0, 0, 2 } })
		{
			CHECK_FALSE(Geometry2D::ClosestPoints(curve, empty));
			CHECK(std::isinf(Geometry2D::Distance(curve, empty)));
		}
		CHECK_FALSE(Geometry2D::ClosestPoints(curve, Circle{ 0, 0, 0 }));
		CHECK(std::isinf(Geometry2D::Distance(curve, Circle{ 0, 0, 0 })));
	};
	CheckCurve(Bezier2{ { -10, 0 }, { 0, 0 }, { 10, 0 } });
	CheckCurve(Bezier3{ { -10, 0 }, { 100, 0 }, { -100, 0 }, { 10, 0 } });
	CheckCurve(Bezier2{ { 0, 0 }, { 0, 0 }, { 0, 0 } });
	CheckCurve(Bezier3{ { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } });
	CheckPair(Bezier2{ { 3, 4 }, { 5, 6 }, { 7, 8 } }, Circle{ 0, 0, 2 }, 3.0);
	CheckPair(Bezier3{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 0, 0 } }, Circle{ 0, 152, 1 }, 1.0);
}

TEST_CASE("Geometry2D.Distance.Bezier.RoundedShapeCompetingMinima")
{
	const Vec2 c0{ 2500, -125 }, c1{ -10000, 750 }, c2{ 10000, -1500 }, c3{ 0, 1000 };
	const Bezier3 curve{ c0, (c0 + c1 / 3), (c0 + 2 * c1 / 3 + c2 / 3), (c0 + c1 + c2 + c3) };
	constexpr double v = 0.0055;
	const Vec2 expected{ (10000 * v * v), (1000 * v * v * v) };
	const Vec2 normal = Vec2{ (-3000 * v * v), (20000 * v) }.normalized();
	const Vec2 center = (expected + normal);
	const auto CheckShape = [&](const auto& shape)
	{
		for (const auto& c : { curve, curve.reversed() })
		{
			const auto result = Geometry2D::ClosestPoints(c, shape);
			REQUIRE(result);
			CHECK(Abs(result->distance - 0.75) <= 1e-10);
			CHECK(result->pointA.distanceFrom(expected) <= 1e-8);
			CHECK(result->pointB.distanceFrom(expected + normal * 0.75) <= 1e-8);
			CHECK(Geometry2D::Distance(c, shape) == result->distance);
			CheckWitnessConsistency(*result);
		}
	};
	CheckShape(Circle{ center, 0.25 });
	CheckShape(RoundRect{ (center.x - 0.25), (center.y - 0.25), 0.5, 0.5, 0.25 });
}

TEST_CASE("Geometry2D.Distance.Bezier.RoundedShapeNarrowContact")
{
	const double u = 0.371;
	const Vec2 c0{ (-100 * u), (100 * u * u) }, c1{ 100, (-200 * u) }, c2{ 0, 100 };
	const auto CheckCurve = [&](const auto& curve)
	{
		for (const double gap : { -1e-8, 1e-8 })
		{
			const Circle circle{ 0, (-1 - gap), 1 };
			const RoundRect rounded{ -1, (-6 - gap), 2, 6, 1 };
			const auto CheckShape = [&](const auto& shape)
			{
				const auto result = Geometry2D::ClosestPoints(curve, shape);
				REQUIRE(result);
				CHECK(Abs(result->distance - Max(0.0, gap)) <= 1e-12);
				CHECK(result->pointA.distanceFrom(Vec2{ 0, 0 }) <= 1e-10);
				CHECK(Geometry2D::Distance(curve, shape) == result->distance);
				CheckWitnessConsistency(*result, 1e-12);
			};
			CheckShape(circle);
			CheckShape(rounded);
		}
	};
	CheckCurve(Bezier2{ c0, (c0 + c1 / 2), (c0 + c1 + c2) });
	CheckCurve(Bezier3{ c0, (c0 + c1 / 3), (c0 + 2 * c1 / 3 + c2 / 3), (c0 + c1 + c2) });
}

TEST_CASE("Geometry2D.Distance.Bezier.SeparatedWitnesses")
{
	const auto Elevate = [](const Bezier2& curve)
	{
		return Bezier3{ curve.p0, (curve.p0 + (curve.p1 - curve.p0) * (2.0 / 3.0)),
			(curve.p2 + (curve.p1 - curve.p2) * (2.0 / 3.0)), curve.p2 };
	};
	// x = left + width * t, y = curvature * x^2 * (1 + q * t) + gap.
	// For q > -1, opposite signs of curvature give supporting lines y = 0 and y = gap.
	const auto Profile = [](const double left, const double right, const double curvature, const double q, const double gap)
	{
		const double width = (right - left);
		const double c0 = (curvature * left * left + gap);
		const double c1 = (curvature * (2.0 * left * width + q * left * left));
		const double c2 = (curvature * (width * width + 2.0 * q * left * width));
		const double c3 = (curvature * q * width * width);
		return Bezier3{ { left, c0 }, { (left + width / 3.0), (c0 + c1 / 3.0) },
			{ (left + 2.0 * width / 3.0), (c0 + 2.0 * c1 / 3.0 + c2 / 3.0) }, { right, (c0 + c1 + c2 + c3) } };
	};
	for (const double gap : { 1.0e-8, 0.0001, 0.01, 10.0 })
	{
		for (const double scale : { 0.01, 1.0, 100.0 })
		{
			for (const double angle : { 0.0, 0.7, Math::HalfPi })
			{
				CAPTURE(gap, scale, angle);
				const auto TransformPoint = [&](const Vec2& p) { return ((p.rotated(angle) + Vec2{ 7, -11 }) * scale); };
				const auto Transform = [&](auto curve)
				{
					curve.p0 = TransformPoint(curve.p0);
					curve.p1 = TransformPoint(curve.p1);
					curve.p2 = TransformPoint(curve.p2);
					if constexpr (std::is_same_v<decltype(curve), Bezier3>)
					{
						curve.p3 = TransformPoint(curve.p3);
					}
					return curve;
				};
				const auto CheckPair = [&](const auto& a, const auto& b)
				{
					const double tolerance = (1.0e-10 * scale);
					CHECK(Abs(Geometry2D::Distance(a, b) - gap * scale) <= tolerance);
					CHECK(Abs(Geometry2D::Distance(b, a) - gap * scale) <= tolerance);
					const auto ab = Geometry2D::ClosestPoints(a, b);
					const auto ba = Geometry2D::ClosestPoints(b, a);
					REQUIRE(ab);
					REQUIRE(ba);
					CHECK(Abs(ab->distance - gap * scale) <= tolerance);
					CHECK(Abs(ba->distance - gap * scale) <= tolerance);
					CHECK(ab->pointA.distanceFrom(TransformPoint({ 0, 0 })) <= (1.0e-5 * scale));
					CHECK(ab->pointB.distanceFrom(TransformPoint({ 0, gap })) <= (1.0e-5 * scale));
					CHECK(ba->pointA.distanceFrom(TransformPoint({ 0, gap })) <= (1.0e-5 * scale));
					CHECK(ba->pointB.distanceFrom(TransformPoint({ 0, 0 })) <= (1.0e-5 * scale));
					CheckWitnessConsistency(*ab, tolerance);
					CheckWitnessConsistency(*ba, tolerance);
				};
				const Bezier2 a = Transform(Bezier2{ { -20, -4 }, { 30, 16 }, { 80, -64 } });
				const Bezier2 b = Transform(Bezier2{ { -30, (28.8 + gap) }, { -5, (-19.2 + gap) }, { 20, (12.8 + gap) } });
				CheckPair(a, b);
				CheckPair(a, Elevate(b));
				CheckPair(Elevate(a), Elevate(b));
				CheckPair(a.reversed(), b.reversed());
				const Bezier3 cubicA = Transform(Profile(-20, 80, -0.01, 0.3, 0.0));
				const Bezier3 cubicB = Transform(Profile(-30, 20, 0.032, -0.6, gap));
				CheckPair(a, cubicB);
				CheckPair(cubicA, cubicB);
				CheckPair(cubicA.reversed(), cubicB.reversed());
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.Bezier.EndpointsAndDegeneracy")
{
	const auto CheckPair = [](const auto& a, const auto& b, const Vec2& expectedA, const Vec2& expectedB)
	{
		const double expected = expectedA.distanceFrom(expectedB);
		CHECK(Abs(Geometry2D::Distance(a, b) - expected) <= 1.0e-10);
		CHECK(Abs(Geometry2D::Distance(b, a) - expected) <= 1.0e-10);
		const auto ab = Geometry2D::ClosestPoints(a, b);
		const auto ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(Abs(ab->distance - expected) <= 1.0e-10);
		CHECK(Abs(ba->distance - expected) <= 1.0e-10);
		CHECK(ab->pointA.distanceFrom(expectedA) <= 1.0e-7);
		CHECK(ab->pointB.distanceFrom(expectedB) <= 1.0e-7);
		CHECK(ba->pointA.distanceFrom(expectedB) <= 1.0e-7);
		CHECK(ba->pointB.distanceFrom(expectedA) <= 1.0e-7);
		CheckWitnessConsistency(*ab);
		CheckWitnessConsistency(*ba);
	};
	CheckPair(Bezier2{ { 0, 0 }, { 0, 0 }, { 2, 0 } },
		Bezier2{ { 5, 4 }, { 6, 6 }, { 7, 7 } }, { 2, 0 }, { 5, 4 });
	CheckPair(Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } },
		Bezier3{ { 10, 1 }, { 10, 2 }, { 10, 3 }, { 10, 4 } }, { 10, 0 }, { 10, 1 });
	CheckPair(Bezier3{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 0, 0 } },
		Bezier3{ { 0, 300 }, { 100, 500 }, { -100, 500 }, { 0, 300 } }, { 0, 150 }, { 0, 300 });
	CheckPair(Bezier2{ { 0, 0 }, { 0, 0 }, { 0, 0 } },
		Bezier3{ { 3, 4 }, { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 0, 0 }, { 3, 4 });
	CheckPair(Bezier2{ { 0, 0 }, { 5, 5 }, { 10, 0 } },
		Bezier2{ { 0, 5 }, { 5, 0 }, { 10, 5 } }, { 5, 2.5 }, { 5, 2.5 });
	const Bezier3 straight{ { 0, 0 }, { 100, 0 }, { -50, 0 }, { 50, 0 } };
	const auto parallel = Geometry2D::ClosestPoints(straight, straight.movedBy(0, 1));
	REQUIRE(parallel);
	CHECK(parallel->distance == Test::Approx(1.0));
	CHECK(parallel->pointA.x == Test::Approx(parallel->pointB.x));
	CHECK(parallel->pointA.y == 0.0);
	CHECK(parallel->pointB.y == 1.0);
	CheckWitnessConsistency(*parallel);
}

TEST_CASE("Geometry2D.Distance.Bezier.LineWitnesses")
{
	for (const double gap : { 1.0e-8, 0.0001, 0.01, 10.0 })
	{
		for (const double left : { -30.0, -0.001, 0.0 })
		{
			const double right = ((left == -30.0) ? 20.0 : 1.0), width = (right - left), k = 0.032;
			const double c0 = (k * left * left + gap), c1 = (2 * k * left * width), c2 = (k * width * width);
			const Bezier2 quadratic{ { left, c0 }, { (left + width / 2), (c0 + c1 / 2) }, { right, (c0 + c1 + c2) } };
			const Bezier3 elevated{ quadratic.p0, (quadratic.p0 + (quadratic.p1 - quadratic.p0) * (2.0 / 3.0)),
				(quadratic.p2 + (quadratic.p1 - quadratic.p2) * (2.0 / 3.0)), quadratic.p2 };
			// y = k*x^2*(1 - 0.6*t) + gap has its minimum at x = 0.
			const double b1 = (c1 - 0.6 * (c0 - gap)), b2 = (c2 - 0.6 * c1), b3 = (-0.6 * c2);
			const Bezier3 cubic{ { left, c0 }, { (left + width / 3), (c0 + b1 / 3) },
				{ (left + 2 * width / 3), (c0 + 2 * b1 / 3 + b2 / 3) }, { right, (c0 + b1 + b2 + b3) } };
			for (const double scale : { 0.01, 1.0, 100.0 })
			{
				for (const double angle : { 0.0, 0.7, Math::HalfPi })
				{
					CAPTURE(gap, left, scale, angle);
					const auto Transform = [&](const Vec2& point) { return ((point.rotated(angle) + Vec2{ 7, -11 }) * scale); };
					const auto CheckCurve = [&](auto curve)
					{
						curve.p0 = Transform(curve.p0);
						curve.p1 = Transform(curve.p1);
						curve.p2 = Transform(curve.p2);
						if constexpr (std::is_same_v<decltype(curve), Bezier3>)
						{
							curve.p3 = Transform(curve.p3);
						}
						const auto CheckPair = [&](const Line& line, const auto& b)
						{
							const double tolerance = (1.0e-10 * scale);
							CHECK(Abs(Geometry2D::Distance(line, b) - gap * scale) <= tolerance);
							CHECK(Abs(Geometry2D::Distance(b, line) - gap * scale) <= tolerance);
							const auto ab = Geometry2D::ClosestPoints(line, b), ba = Geometry2D::ClosestPoints(b, line);
							REQUIRE(ab);
							REQUIRE(ba);
							CHECK(Abs(ab->distance - gap * scale) <= tolerance);
							CHECK(Abs(ba->distance - gap * scale) <= tolerance);
							CHECK(ab->pointA.distanceFrom(Transform({ 0, 0 })) <= (1.0e-6 * scale));
							CHECK(ab->pointB.distanceFrom(Transform({ 0, gap })) <= (1.0e-6 * scale));
							CHECK(ba->pointA.distanceFrom(ab->pointB) <= tolerance);
							CHECK(ba->pointB.distanceFrom(ab->pointA) <= tolerance);
							CheckWitnessConsistency(*ab, tolerance);
						};
						for (const Line line : { Line{ Transform({ -20, 0 }), Transform({ 80, 0 }) },
							Line{ Transform({ 0, 0 }), Transform({ -10, -30 }) } })
						{
							CheckPair(line, curve);
							CheckPair(Line{ line.end, line.start }, curve.reversed());
						}
					};
					CheckCurve(quadratic);
					CheckCurve(elevated);
					CheckCurve(cubic);
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.Bezier.LineDegeneracyAndBoundaries")
{
	const auto CheckPair = [](const Line& line, const auto& curve, const Vec2& expectedA, const Vec2& expectedB)
	{
		const double distance = expectedA.distanceFrom(expectedB);
		CHECK(Abs(Geometry2D::Distance(line, curve) - distance) <= 1.0e-10);
		CHECK(Abs(Geometry2D::Distance(curve, line) - distance) <= 1.0e-10);
		const auto pair = Geometry2D::ClosestPoints(line, curve), reversed = Geometry2D::ClosestPoints(curve, line);
		REQUIRE(pair);
		REQUIRE(reversed);
		CHECK(pair->pointA.distanceFrom(expectedA) <= 1.0e-7);
		CHECK(pair->pointB.distanceFrom(expectedB) <= 1.0e-7);
		CHECK(reversed->pointA.distanceFrom(expectedB) <= 1.0e-7);
		CHECK(reversed->pointB.distanceFrom(expectedA) <= 1.0e-7);
		CheckWitnessConsistency(*pair);
	};
	// The nearer endpoint has two local distance minima on this curve.
	CheckPair(Line{ { -13, 74 }, { 30, 22 } }, Bezier3{ { 49, 77 }, { -49, 82 }, { 65, 49 }, { 40, -1 } },
		{ 30, 22 }, { 41.333363837603287, 26.25614582769596 });
	CheckPair(Line{ { 0, 151 }, { 0, 200 } }, Bezier3{ { 0, 0 }, { 100, 200 }, { -100, 200 }, { 0, 0 } }, { 0, 151 }, { 0, 150 });
	CheckPair(Line{ { 40, 0 }, { 50, 0 } }, Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } }, { 40, 0 }, { (50 / std::sqrt(3.0)), 0 });
	CheckPair(Line{ { 10, 1 }, { 10, 4 } }, Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } }, { 10, 1 }, { 10, 0 });
	CheckPair(Line{ { 0, 0 }, { 10, 0 } }, Bezier2{ { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 3, 0 }, { 3, 4 });
	CheckPair(Line{ { 0, 0 }, { 10, 0 } }, Bezier3{ { 3, 4 }, { 3, 4 }, { 3, 4 }, { 3, 4 } }, { 3, 0 }, { 3, 4 });
	CheckPair(Line{ { 0, 0 }, { 0, 0 } }, Bezier2{ { -1, 2 }, { 0, 0 }, { 1, 2 } }, { 0, 0 }, { 0, 1 });
	CheckPair(Line{ { 0, 0 }, { 10, 0 } }, Bezier3{ { 5, 3 }, { 6, 4 }, { 7, 5 }, { 8, 6 } }, { 5, 0 }, { 5, 3 });
	for (const Line line : { Line{ { -2, 0 }, { 2, 0 } }, Line{ { -2, 0.5 }, { 2, 0.5 } }, Line{ { 0, 0 }, { 0, 0 } } })
	{
		const Bezier2 curve{ { -1, 1 }, { 0, -1 }, { 1, 1 } };
		CHECK(Geometry2D::Distance(line, curve) == 0.0);
		const auto pair = Geometry2D::ClosestPoints(line, curve);
		REQUIRE(pair);
		CHECK(pair->pointA == pair->pointB);
		CHECK(Geometry2D::Distance(pair->pointA, line) <= 1.0e-9);
	}
}

TEST_CASE("Geometry2D.Distance.Bezier.LinearBoundaries")
{
	const Bezier2 curve{ { -30, 28.8001 }, { -5, -19.1999 }, { 20, 12.8001 } };
	const RectF rect{ -20, -40, 100, 40 };
	const auto CheckShape = [&](const auto& shape)
	{
		CHECK(Abs(Geometry2D::Distance(shape, curve) - 0.0001) <= 1.0e-10);
		CHECK(Abs(Geometry2D::Distance(curve, shape) - 0.0001) <= 1.0e-10);
		const auto pair = Geometry2D::ClosestPoints(shape, curve), reversed = Geometry2D::ClosestPoints(curve, shape);
		REQUIRE(pair);
		REQUIRE(reversed);
		CHECK(pair->pointA.distanceFrom(Vec2{ 0, 0 }) <= 1.0e-8);
		CHECK(pair->pointB.distanceFrom(Vec2{ 0, 0.0001 }) <= 1.0e-8);
		CHECK(reversed->pointA.distanceFrom(pair->pointB) <= 1.0e-10);
		CHECK(reversed->pointB.distanceFrom(pair->pointA) <= 1.0e-10);
		CheckWitnessConsistency(*pair);
	};
	CheckShape(rect);
	CheckShape(Rect{ -20, -40, 100, 40 });
	CheckShape(rect.asQuad());
	CheckShape(Triangle{ { -20, 0 }, { -20, -40 }, { 80, 0 } });
	CheckShape(rect.asPolygon());
	CheckShape(MultiPolygon{ rect.asPolygon(), Rect{ -500, -500, 10, 10 }.asPolygon() });
	CheckShape(LineString{ { -20, 0 }, { 80, 0 }, { 80, -40 } });
	CHECK(std::isinf(Geometry2D::Distance(Polygon{}, curve)));
	CHECK(not Geometry2D::ClosestPoints(Polygon{}, curve));
}

TEST_CASE("Geometry2D.Distance.Bezier.CompetingMinima")
{
	const auto CheckPair = [](const Bezier3& a, const Bezier3& b, const double expected)
	{
		CHECK(Abs(Geometry2D::Distance(a, b) - expected) <= 1.0e-10);
		CHECK(Abs(Geometry2D::Distance(b, a) - expected) <= 1.0e-10);
		const auto ab = Geometry2D::ClosestPoints(a, b);
		const auto ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(Abs(ab->distance - expected) <= 1.0e-10);
		CHECK(Abs(ba->distance - expected) <= 1.0e-10);
		CheckWitnessConsistency(*ab);
		CheckWitnessConsistency(*ba);
	};
	// Nearly coincident arches have their minimum near an endpoint, rather than at the apex.
	CheckPair(Bezier3{ { 0, 0 }, { 0, 100 }, { 100, 100 }, { 100, 0 } },
		Bezier3{ { 0, -0.001 }, { 0, 99.999 }, { 100, 99.999 }, { 100, -0.001 } }, 3.33334814818518e-9);
	CheckPair(Bezier3{ { 92.854574324756015, -76.762575334281166 }, { -3.2191555860900944, 55.808467218007962 },
		{ 74.409657327262451, 90.526888540266228 }, { 18.942587278851253, -70.767084585809982 } },
		Bezier3{ { 4.3182047114657784, 94.502104663640623 }, { 25.063131888316725, 64.595188364550893 },
		{ -43.117704181106916, 57.961316111168216 }, { 41.082149410381085, 92.783635272975403 } }, 50.956910249531042);
	CheckPair(Bezier3{ { 57.085537155612577, -93.969684563137307 }, { 58.656233352023165, 37.228454192967632 },
		{ 96.141659747306335, 47.874552870800983 }, { -26.365871164589695, -24.048509688014263 } },
		Bezier3{ { 54.714392834029745, 14.793743009051274 }, { -74.243475165811873, -56.717017116748849 },
		{ -69.463987119048966, -47.083003090084262 }, { 18.166275987707682, -50.122327809069432 } }, 5.1846193217859158);
}

TEST_CASE("Geometry2D.Distance.Polygon.intersection_and_containment_witnesses")
{
	const Polygon polygon{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 6, 6 }, { 6, 14 }, { 14, 14 }, { 14, 6 } } }
	};
	const auto CheckPair = [](const auto& a, const auto& b, const double expected)
	{
		CHECK(Geometry2D::Distance(a, b) == Test::Approx(expected));
		CHECK(Geometry2D::Distance(b, a) == Test::Approx(expected));
		const auto ab = Geometry2D::ClosestPoints(a, b);
		const auto ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(ab->distance == Test::Approx(expected));
		CHECK(ba->distance == Test::Approx(expected));
		CHECK(Geometry2D::Intersects(ab->pointA, a));
		CHECK(Geometry2D::Intersects(ab->pointB, b));
		CHECK(Geometry2D::Intersects(ba->pointA, b));
		CHECK(Geometry2D::Intersects(ba->pointB, a));
		CheckWitnessConsistency(*ab);
		CheckWitnessConsistency(*ba);
	};

	CheckPair(polygon, polygon, 0.0);
	CheckPair(polygon, Vec2{ 2, 3 }, 0.0);
	CheckPair(polygon, Vec2{ 10, 10 }, 4.0);
	CheckPair(polygon, LineString{ Vec2{ 2, 3 }, Vec2{ 2, 3 }, Vec2{ 2, 3 } }, 0.0);
	CheckPair(polygon, Line{ Vec2{ -5, 10 }, Vec2{ 25, 10 } }, 0.0);
	CheckPair(polygon, Circle{ Vec2{ 3, 3 }, 1.0 }, 0.0);
	CheckPair(polygon, RectF{ 1, 1, 2, 2 }.asPolygon(), 0.0);
	CheckPair(polygon, RectF{ -10, -10, 40, 40 }.asPolygon(), 0.0);
	CheckPair(polygon, RectF{ 7, 7, 2, 2 }.asPolygon(), 1.0);
	CheckPair(polygon, RectF{ 18, 1, 4, 2 }.asPolygon(), 0.0);
	CheckPair(polygon, RectF{ 30, 1, 4, 2 }.asPolygon(), 10.0);
	const MultiPolygon multi{ Polygon{}, polygon, RectF{ 30, 1, 4, 2 }.asPolygon(), Polygon{} };
	CheckPair(multi, Vec2{ 32, 2 }, 0.0);
	CheckPair(multi, Vec2{ 25, 2 }, 5.0);
	CHECK(std::isinf(Geometry2D::Distance(polygon, MultiPolygon{ Polygon{} })));
	CHECK(not Geometry2D::ClosestPoints(polygon, MultiPolygon{ Polygon{} }));
}

TEST_CASE("Geometry2D.Distance.Polygon.point_collapse")
{
	const Polygon source{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 6, 6 }, { 6, 14 }, { 14, 14 }, { 14, 6 } } }
	};
	const Vec2 point{ 2, 3 };
	const Polygon collapsed = source.scaledFrom(point, 0.0);
	REQUIRE(not collapsed.isEmpty());
	const auto CheckPair = [](const auto& a, const auto& b, const Vec2& pointA, const Vec2& pointB)
	{
		const double distance = pointA.distanceFrom(pointB);
		CHECK(Geometry2D::Distance(a, b) == Test::Approx(distance));
		CHECK(Geometry2D::Distance(b, a) == Test::Approx(distance));
		const auto ab = Geometry2D::ClosestPoints(a, b);
		const auto ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(ab->distance == Test::Approx(distance));
		CHECK(ba->distance == Test::Approx(distance));
		CHECK(ab->pointA.distanceFrom(pointA) < 1e-7);
		CHECK(ab->pointB.distanceFrom(pointB) < 1e-7);
		CHECK(ba->pointA.distanceFrom(pointB) < 1e-7);
		CHECK(ba->pointB.distanceFrom(pointA) < 1e-7);
		CheckWitnessConsistency(*ab);
		CheckWitnessConsistency(*ba);
	};

	CheckPair(collapsed, Vec2{ 5, 7 }, point, Vec2{ 5, 7 });
	CheckPair(collapsed, source.scaledFrom(Vec2{ 5, 7 }, 0.0), point, Vec2{ 5, 7 });
	CheckPair(collapsed, Line{ Vec2{ 5, 1 }, Vec2{ 5, 5 } }, point, Vec2{ 5, 3 });
	CheckPair(collapsed, Circle{ 8, 3, 1 }, point, Vec2{ 7, 3 });
	CheckPair(collapsed, Ellipse{ 8, 3, 2, 1 }, point, Vec2{ 6, 3 });
	CheckPair(collapsed, SuperEllipse{ Vec2{ 8, 3 }, SizeF{ 2, 1 }, 4 }, point, Vec2{ 6, 3 });
	CheckPair(collapsed, Bezier2{ Vec2{ 6, 1 }, Vec2{ 4, 3 }, Vec2{ 6, 5 } }, point, Vec2{ 5, 3 });
	CheckPair(collapsed, RoundRect{ 6, 1, 4, 4, 1 }, point, Vec2{ 6, 3 });
	CheckPair(collapsed, RectF{ 6, 1, 2, 4 }.asPolygon(), point, Vec2{ 6, 3 });
	CheckPair(collapsed, source, point, point);
	CheckPair(collapsed, collapsed, point, point);
	CheckPair(source.scaledFromOrigin(Vec2{ 0, 1 }).movedBy(point), Vec2{ 5, 7 }, Vec2{ 2, 7 }, Vec2{ 5, 7 });

	const Polygon area = RectF{ 20, 20, 4, 4 }.asPolygon();
	for (const MultiPolygon& multi : { MultiPolygon{ Polygon{}, area, collapsed }, MultiPolygon{ collapsed, area, Polygon{} } })
	{
		CheckPair(multi, Vec2{ 5, 7 }, point, Vec2{ 5, 7 });
		CheckPair(multi, RectF{ 6, 1, 2, 4 }.asPolygon(), point, Vec2{ 6, 3 });
		CheckPair(multi, MultiPolygon{ Polygon{}, source.scaledFrom(Vec2{ 5, 7 }, 0.0) }, point, Vec2{ 5, 7 });
	}
	CheckPair(MultiPolygon{ collapsed, source.scaledFrom(Vec2{ 30, 30 }, 0.0) }, Vec2{ 5, 7 }, point, Vec2{ 5, 7 });
	CHECK(std::isinf(Geometry2D::Distance(collapsed, MultiPolygon{ Polygon{} })));
	CHECK(not Geometry2D::ClosestPoints(collapsed, MultiPolygon{ Polygon{} }));
}

TEST_CASE("Geometry2D.Distance.Bezier.SimpleGeometry")
{
	const auto Elevate = [](const Bezier2& q)
	{
		return Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 };
	};
	const auto CheckPair = [](const auto& a, const auto& b, const double expected)
	{
		CHECK(Geometry2D::Distance(a, b) == Test::Approx(expected).epsilon(1.0e-11));
		CHECK(Geometry2D::Distance(b, a) == Test::Approx(expected).epsilon(1.0e-11));
		const auto ab = Geometry2D::ClosestPoints(a, b);
		const auto ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(ab->distance == Test::Approx(expected).epsilon(1.0e-11));
		CHECK(ba->distance == Test::Approx(expected).epsilon(1.0e-11));
		CHECK(Geometry2D::Distance(ab->pointA, a) < 1.0e-10);
		CHECK(Geometry2D::Distance(ab->pointB, b) < 1.0e-10);
		CHECK(Geometry2D::Distance(ba->pointB, a) < 1.0e-10);
		CHECK(Geometry2D::Distance(ba->pointA, b) < 1.0e-10);
		CheckWitnessConsistency(*ab);
		CheckWitnessConsistency(*ba);
	};
	const Bezier2 arch{ { -1, 0 }, { 0, 0.2 }, { 1, 0 } };
	for (const double y : { 0.0, 0.099, 0.1 })
	{
		const Bezier2 straight{ { -0.2, y }, { 0, y }, { 0.2, y } };
		const double expected = ((y == 0.0) ? 0.095921794750111 : 0.0);
		CheckPair(arch, straight, expected);
		CheckPair(arch, Elevate(straight), expected);
		CheckPair(Elevate(arch), straight, expected);
		CheckPair(Elevate(arch), Elevate(straight), expected);
		CheckPair(arch.reversed(), straight, expected);
	}
	const Vec2 point = arch.pointAt(0.371);
	CheckPair(arch, Bezier2{ point, point, point }, 0.0);
	CheckPair(Elevate(arch), Bezier3{ point, point, point, point }, 0.0);
	CheckPair(Bezier2{ { 0, 0 }, { 100, 0 }, { 0, 0 } }, Bezier2{ { 49, 0 }, { 50, 0 }, { 51, 0 } }, 0.0);
	CheckPair(Bezier2{ { 0, 0 }, { 100, 0 }, { 0, 0 } }, Bezier2{ { 51, 0 }, { 51, 0 }, { 51, 0 } }, 1.0);
	CheckPair(Bezier3{ { 0, 0 }, { 100, 0 }, { -100, 0 }, { 0, 0 } }, Bezier2{ { 30, 0 }, { 30, 0 }, { 30, 0 } }, (30 - 50 / std::sqrt(3.0)));
	for (const auto [diamond, distance] : { std::pair{ SuperEllipse{ 0, 0, 0.02, 0.01, 1 }, 0.09 },
		std::pair{ SuperEllipse{ 0, 0.1, 0.01, 0.001, 1 }, 0.0 }, std::pair{ SuperEllipse{ 0, 0.11, 0.02, 0.01, 1 }, 0.0 } })
	{
		CheckPair(arch, diamond, distance);
		CheckPair(Elevate(arch), diamond, distance);
	}
}

TEST_CASE("Geometry2D.Distance.Bezier.GeneralPairs")
{
	const auto Elevate = [](const Bezier2& q)
	{
		return Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 };
	};
	const auto CheckPair = [](const auto& a, const auto& b, const double lower, const double upper)
	{
		for (const auto& aa : { a, a.reversed() })
		{
			const auto ab = Geometry2D::ClosestPoints(aa, b), ba = Geometry2D::ClosestPoints(b, aa);
			REQUIRE(ab);
			REQUIRE(ba);
			for (const double distance : { ab->distance, ba->distance, Geometry2D::Distance(aa, b), Geometry2D::Distance(b, aa) })
			{
				CHECK(lower <= distance);
				CHECK(distance <= upper);
			}
			CHECK(Geometry2D::Distance(ab->pointA, aa) < 1.0e-11);
			CHECK(Geometry2D::Distance(ab->pointB, b) < 1.0e-11);
			CHECK(Geometry2D::Distance(ba->pointB, aa) < 1.0e-11);
			CHECK(Geometry2D::Distance(ba->pointA, b) < 1.0e-11);
			CheckWitnessConsistency(*ab);
			CheckWitnessConsistency(*ba);
		}
	};
	const auto CheckDegrees = [&](const Bezier2& a, const Bezier2& b, const double lower, const double upper)
	{
		CheckPair(a, b, lower, upper);
		CheckPair(a, Elevate(b), lower, upper);
		CheckPair(Elevate(a), b, lower, upper);
		CheckPair(Elevate(a), Elevate(b), lower, upper);
	};
	const Bezier2 a{ { -1, 0.1 }, { 0, -0.1 }, { 1, 0.1 } };
	CheckDegrees(a, Bezier2{ { -1, -0.098 }, { 0, 0.102 }, { 1, -0.098 } }, 0.0, 0.0);
	CheckDegrees(a, Bezier2{ { -1, -0.1 }, { 0, 0.1 }, { 1, -0.1 } }, 0.0, 0.0);
	CheckDegrees(a, a, 0.0, 0.0);
	CheckDegrees(a, a.split(0.371).second, 0.0, 0.0);
	CheckDegrees(a, a.movedBy(0, 1.0e-15), 0.0, 0.0);
	for (const double gap : { 1.0e-3, 1.0e-8, 1.0e-12 })
	{
		// The parabola has |dy/dx| <= 0.2, so its vertical translate is
		// separated by at least gap / sqrt(1 + 0.2^2), up to roundoff.
		CheckDegrees(a, a.movedBy(0, gap), (gap / std::sqrt(1.04) - 1.0e-16), (gap + 1.0e-16));
	}

	// A nearly coincident pair exhausts the predicate budget. Its conservative
	// true must not manufacture a zero distance or an off-curve common point.
	const Bezier3 near{ { -83, -28 }, { -80, -49 }, { -90, -30 }, { 82, -33 } };
	const Bezier3 shifted = near.movedBy(1.0e-10, 0.37e-10);
	CHECK(Geometry2D::Intersects(near, shifted));
	CheckPair(near, shifted, 1.0e-13, 1.1e-10);
}

TEST_CASE("Geometry2D.Distance.Bezier.GeneralSuperEllipse")
{
	const auto Elevate = [](const Bezier2& q)
	{
		return Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 };
	};
	const auto CheckPair = [](const auto& curve, const SuperEllipse& shape, const bool contact)
	{
		for (const auto& c : { curve, curve.reversed() })
		{
			const auto ab = Geometry2D::ClosestPoints(c, shape), ba = Geometry2D::ClosestPoints(shape, c);
			REQUIRE(ab);
			REQUIRE(ba);
			for (const double distance : { ab->distance, ba->distance, Geometry2D::Distance(c, shape), Geometry2D::Distance(shape, c) })
			{
				if (contact)
				{
					CHECK(distance == 0.0);
				}
				else
				{
					CHECK(0.1 < distance);
					CHECK(distance < 0.13);
				}
			}
			CHECK(Geometry2D::Distance(ab->pointA, c) < 1.0e-10);
			CHECK(Geometry2D::Distance(ba->pointB, c) < 1.0e-10);
			for (const Vec2& p : { ab->pointB, ba->pointA })
			{
				const Vec2 normalized = ((p - shape.center) / shape.axes);
				CHECK((std::pow(Abs(normalized.x), shape.n) + std::pow(Abs(normalized.y), shape.n)) <= (1.0 + 1.0e-10));
			}
			CheckWitnessConsistency(*ab);
			CheckWitnessConsistency(*ba);
		}
	};
	const Bezier2 arch{ { -1, 0 }, { 0, 0.25 }, { 1, 0 } };
	for (const double n : { 0.25, 0.5, 1.5, 4.0, 16.0 })
	{
		for (const auto& shape : { SuperEllipse{ arch.pointAt(0.371), 0.0001, 0.00001, n },
			SuperEllipse{ 0, 0.25, 0.125, 0.125, n } })
		{
			CheckPair(arch, shape, true);
			CheckPair(Elevate(arch), shape, true);
		}
		const SuperEllipse separate{ 0, 0, 0.02, 0.01, n };
		CheckPair(arch, separate, false);
		CheckPair(Elevate(arch), separate, false);
	}
	for (const double n : { 0.25, 0.5 })
	{
		const Vec2 offset{ 0.003, 0.006 };
		const auto curve = Elevate(arch).movedBy(offset);
		const auto tip = SuperEllipse{ 0, 0.25, 0.125, 0.125, n }.movedBy(offset);
		const auto closest = Geometry2D::ClosestPoints(curve, tip);
		REQUIRE(closest);
		CHECK(closest->distance == 0.0);
		CHECK(closest->pointA.distanceFrom(tip.top()) < 1.0e-14);
		CHECK(Geometry2D::Distance(curve, tip) == 0.0);
	}

	// A near tangent exhausts the predicate budget. Preserve an evaluated
	// positive distance instead of inferring a common point from its true.
	const Bezier2 near{ { 0.84685981844406344, 1.1005179725195844 },
		{ 0.91832975846205445, 0.9804672563978607 }, { 1.0264798602082323, 0.8970967020043239 } };
	const SuperEllipse shape{ 0, 0, 1, 1, 16 };
	for (const auto& c : { near, near.reversed() })
	{
		CHECK(Geometry2D::Intersects(c, shape));
		const auto closest = Geometry2D::ClosestPoints(c, shape);
		REQUIRE(closest);
		CHECK(0.0 < closest->distance);
		CHECK(closest->distance < 1.0e-9);
		CHECK(Geometry2D::Distance(c, shape) == closest->distance);
		CHECK(Geometry2D::Distance(closest->pointA, c) < 1.0e-12);
		CHECK((std::pow(Abs(closest->pointB.x), 16) + std::pow(Abs(closest->pointB.y), 16)) <= (1.0 + 1.0e-12));
		CheckWitnessConsistency(*closest);
	}
	CHECK(std::isinf(Geometry2D::Distance(arch, SuperEllipse{ 0, 0, 0, 0, 4 })));
	CHECK(not Geometry2D::ClosestPoints(arch, SuperEllipse{ 0, 0, 0, 0, 4 }));
}
