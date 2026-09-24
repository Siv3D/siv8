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

	void CheckMixedSuperEllipseDistance(const SuperEllipse& a, const SuperEllipse& b,
		const double expected, const double tolerance)
	{
		const auto CheckInside = [](const Vec2& point, const SuperEllipse& shape)
		{
			const Vec2 p = ((point - shape.center) / shape.axes);
			CHECK((std::pow(Max(0.0, Abs(p.x) - 2.0e-12), shape.n)
				+ std::pow(Max(0.0, Abs(p.y) - 2.0e-12), shape.n)) <= (1.0 + 1.0e-12));
		};
		const auto Check = [&](const auto& convex)
		{
			const auto ab = Geometry2D::ClosestPoints(a, convex), ba = Geometry2D::ClosestPoints(convex, a);
			REQUIRE(ab);
			REQUIRE(ba);
			CHECK(Abs(ab->distance - expected) <= tolerance);
			CHECK(ba->distance == ab->distance);
			CHECK(Geometry2D::Distance(a, convex) == ab->distance);
			CHECK(Geometry2D::Distance(convex, a) == ab->distance);
			if (0.0 < expected)
			{
				CHECK(ab->distance > 0.0);
				CHECK(ba->pointA == ab->pointB);
				CHECK(ba->pointB == ab->pointA);
			}
			CheckInside(ab->pointA, a);
			CheckInside(ab->pointB, b);
			CheckInside(ba->pointA, b);
			CheckInside(ba->pointB, a);
			CheckWitnessConsistency(*ab, tolerance);
			CheckWitnessConsistency(*ba, tolerance);
		};
		Check(b);
		if (b.n == 2.0)
		{
			Check(Ellipse{ b.center, b.axes });
		}
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

TEST_CASE("Geometry2D.Distance.Ellipse.RoundRectWitnesses")
{
	for (const Vec2 axes : { Vec2{ 100, 30 }, Vec2{ 1, 100 } })
	{
		for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 }, Vec2{ 0.6, 0.8 }, Vec2{ 1, 0.001 } })
		{
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				for (const double scale : { 0.001, 1.0, 1000.0 })
				{
					const Ellipse ellipse{ (Vec2{ 7, -11 } * scale), (axes * scale) };
					const Vec2 normal = (direction.normalized() * sign);
					const Vec2 support = (ellipse.center + ellipse.axes * (ellipse.axes * normal).normalized());
					for (const double radius : { 0.0, (0.1 * scale), (10.0 * scale) })
					{
						for (const double gap : { (-1.0e-7 * scale), 0.0, (1.0e-10 * scale), (1.0e-5 * scale), (0.5 * scale) })
						{
							CAPTURE(axes, direction, sign, scale, radius, gap);
							const Vec2 corner = (support + normal * (radius + gap));
							const Vec2 extent = (Vec2{ 20, 30 } * scale), far = (corner + sign * extent);
							const RoundRect rounded{ (Min(corner.x, far.x) - radius), (Min(corner.y, far.y) - radius),
								(extent.x + 2 * radius), (extent.y + 2 * radius), radius };
							const auto pair = Geometry2D::ClosestPoints(ellipse, rounded), reversed = Geometry2D::ClosestPoints(rounded, ellipse);
							REQUIRE(pair);
							REQUIRE(reversed);
							const double expected = Max(gap, 0.0), tolerance = (1.0e-10 * scale);
							CHECK(Abs(pair->distance - expected) <= tolerance);
							CHECK(Abs(Geometry2D::Distance(ellipse, rounded) - expected) <= tolerance);
							CHECK(Geometry2D::Distance(rounded, ellipse) == Geometry2D::Distance(ellipse, rounded));
							CHECK(pair->pointA == reversed->pointB);
							CHECK(pair->pointB == reversed->pointA);
							CHECK(((pair->pointA - ellipse.center) / ellipse.axes).lengthSq() <= (1.0 + 1.0e-10));
							CHECK(Geometry2D::Distance(pair->pointB, rounded) <= tolerance);
							if (0.0 < gap)
							{
								CHECK(pair->distance > 0.0);
								CHECK(pair->pointA.distanceFrom(support) <= (1.0e-8 * scale));
								CHECK(pair->pointB.distanceFrom(support + normal * gap) <= (1.0e-8 * scale));
							}
							else
							{
								CHECK(pair->distance == 0.0);
								CHECK(pair->pointA == pair->pointB);
							}
							CheckWitnessConsistency(*pair, tolerance);
						}
					}
				}
			}
		}
	}
	const Ellipse ellipse{ 0, 0, 1, 2 };
	for (const RoundRect rounded : { RoundRect{ 1.1, -5, 10, 10, 100 }, RoundRect{ 1.1, -1, 0, 2, 0 } })
	{
		CHECK(Abs(Geometry2D::Distance(ellipse, rounded) - 0.1) <= 1.0e-12);
		CHECK(Abs(Geometry2D::Distance(SuperEllipse{ ellipse, 0.5 }, rounded) - 0.1) <= 1.0e-12);
	}
	CHECK(Geometry2D::Distance(Ellipse{ 0, 0, 0, 2 }, RoundRect{ 3, -1, 2, 2, 0.5 }) == 3.0);
	CHECK(std::isinf(Geometry2D::Distance(Ellipse{ 0, 0, 0, 0 }, RoundRect{ 3, -1, 2, 2, 0.5 })));
	CHECK(not Geometry2D::ClosestPoints(ellipse, RoundRect{ 0, 0, 0, 0, 0 }));
	CHECK(Geometry2D::Distance(ellipse, RoundRect{ -3, -3, 6, 6, 1 }) == 0.0);
	CHECK(Geometry2D::Distance(ellipse, RoundRect{ -0.1, -0.1, 0.2, 0.2, 0.05 }) == 0.0);
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.DiamondPairs")
{
	for (const double scale : { 0.001, 1.0, 1000.0 })
	{
		const SuperEllipse diamond{ (Vec2{ 7, -11 } * scale), (Vec2{ 3, 2 } * scale), 1.0 };
		for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
		{
			const Vec2 boundary = (diamond.center + sign * diamond.axes * 0.5);
			const Vec2 normal = (sign * Vec2{ 2, 3 }.normalized());
			for (const double n : { 1.01, 2.0, 4.0, 64.0 })
			{
				const Vec2 axes = (Vec2{ 0.3, 0.7 } * scale);
				const Vec2 scaled = (axes * Vec2{ Abs(normal.x), Abs(normal.y) });
				const double q = (n / (n - 1.0)), m = Max(scaled.x, scaled.y);
				const double denominator = std::pow((std::pow(scaled.x / m, q) + std::pow(scaled.y / m, q)), (1.0 / n));
				const Vec2 support = (sign * axes * Vec2{ std::pow(scaled.x / m, (q - 1.0)), std::pow(scaled.y / m, (q - 1.0)) } / denominator);
				for (const double gap : { (-1.0e-7 * scale), 0.0, (1.0e-10 * scale), (0.1 * scale) })
				{
					CAPTURE(n, scale, sign, gap);
					const SuperEllipse shape{ (boundary + normal * gap + support), axes, n };
					const auto Check = [&](const auto& other)
					{
						const double tolerance = (1.0e-10 * scale);
						const auto pair = Geometry2D::ClosestPoints(diamond, other), reversed = Geometry2D::ClosestPoints(other, diamond);
						REQUIRE(pair);
						REQUIRE(reversed);
						CHECK(Abs(pair->distance - Max(gap, 0.0)) <= tolerance);
						CHECK(Abs(Geometry2D::Distance(diamond, other) - Max(gap, 0.0)) <= tolerance);
						CHECK(Abs(Geometry2D::Distance(other, diamond) - Max(gap, 0.0)) <= tolerance);
						CHECK(Abs(reversed->distance - pair->distance) <= tolerance);
						const Vec2 p = ((pair->pointA - diamond.center) / diamond.axes);
						CHECK((Abs(p.x) + Abs(p.y)) <= (1.0 + 1.0e-10));
						if (0.0 < gap)
						{
							CHECK(pair->distance > 0.0);
							CHECK(pair->pointA.distanceFrom(boundary) <= (1.0e-8 * scale));
							CHECK(pair->pointB.distanceFrom(boundary + normal * gap) <= (1.0e-8 * scale));
						}
						else
						{
							CHECK(pair->distance == 0.0);
							CHECK(pair->pointA == pair->pointB);
						}
						CheckWitnessConsistency(*pair, tolerance);
					};
					Check(shape);
					if (n == 2.0) Check(Ellipse{ shape.center, shape.axes });
				}
			}
		}
	}
	const SuperEllipse diamond{ 0, 0, 1, 2, 1 };
	for (const double n : { 0.25, 0.5, 0.9, 1.0 })
	{
		CAPTURE(n);
		const SuperEllipse other{ 3.1, 0, 2, 1, n };
		const auto pair = Geometry2D::ClosestPoints(diamond, other);
		REQUIRE(pair);
		CHECK(Abs(pair->distance - 0.1) <= 1.0e-12);
		CHECK(Abs(Geometry2D::Distance(other, diamond) - 0.1) <= 1.0e-12);
		CHECK(pair->pointA.distanceFrom(Vec2{ 1, 0 }) <= 1.0e-12);
		CHECK(pair->pointB.distanceFrom(Vec2{ 1.1, 0 }) <= 1.0e-12);
	}
	const Vec2 boundary{ 0.04, 0.64 }, normal = Vec2{ 4, 1 }.normalized();
	const double gap = 1.0e-8;
	const SuperEllipse besideArc{ (boundary + normal * gap + Vec2{ 0.1, 0 }), Vec2{ 0.1, 0.02 }, 1.0 };
	const auto pair = Geometry2D::ClosestPoints(SuperEllipse{ 0, 0, 1, 1, 0.5 }, besideArc);
	REQUIRE(pair);
	CHECK(Abs(pair->distance - gap) <= 1.0e-12);
	CHECK(pair->pointA.distanceFrom(boundary) <= 1.0e-10);
	CHECK(pair->pointB.distanceFrom(boundary + normal * gap) <= 1.0e-10);
	CheckWitnessConsistency(*pair);
	CHECK(Geometry2D::Distance(Ellipse{ 0, 0, 0, 2 }, SuperEllipse{ 3, 0, 1, 1, 1 }) == 2.0);
	CHECK(Geometry2D::Distance(Ellipse{ 0, 0, 1, 2 }, SuperEllipse{ 3, 0, 0, 1, 1 }) == 2.0);
	CHECK(not Geometry2D::ClosestPoints(Ellipse{ 0, 0, 0, 0 }, diamond));
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

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConcavePointMinima")
{
	// For sqrt(x) + sqrt(y) = 1 and query (q, q), the central minimum
	// splits into two minima at q = 3/4, which reach the tips at q = 1.
	for (const double q : { 0.25000001, 0.3, 0.7, 0.75, 0.750001, 0.8, 0.9, 1.0, 1.2 })
	{
		const double expected = ((q <= 0.75) ? (std::sqrt(2.0) * (q - 0.25))
			: ((q <= 1.0) ? std::sqrt(2.0 * q - 1.0) : std::hypot(q, (q - 1.0))));
		for (const double scale : { 1.0e-4, 1.0, 1.0e4 })
		{
			const SuperEllipse shape{ (Vec2{ 7, -11 } * scale), scale, scale, 0.5 };
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				CAPTURE(q, scale, sign);
				const Vec2 point = (shape.center + sign * (q * scale));
				const auto Check = [&](const auto& target, const double distance)
				{
					const auto pair = Geometry2D::ClosestPoints(shape, target), reversed = Geometry2D::ClosestPoints(target, shape);
					REQUIRE(pair);
					REQUIRE(reversed);
					const double tolerance = (1.0e-11 * scale);
					CHECK(Abs(pair->distance - distance) <= tolerance);
					CHECK(Abs(Geometry2D::Distance(shape, target) - distance) <= tolerance);
					CHECK(Geometry2D::Distance(target, shape) == Geometry2D::Distance(shape, target));
					CHECK(reversed->pointA == pair->pointB);
					CHECK(reversed->pointB == pair->pointA);
					const Vec2 p = ((pair->pointA - shape.center) / shape.axes);
					CHECK(Abs(std::sqrt(Abs(p.x)) + std::sqrt(Abs(p.y)) - 1.0) <= 1.0e-7);
					CHECK(Geometry2D::Distance(pair->pointB, target) <= tolerance);
					CheckWitnessConsistency(*pair, tolerance);
				};
				Check(point, (expected * scale));
				Check(Line{ point, point }, (expected * scale));
				const double gap = (1.0e-8 * scale), radius = (expected * scale - gap);
				Check(Circle{ point, radius }, gap);
				Check(Ellipse{ point, radius, radius }, gap);
				Check(SuperEllipse{ point, radius, radius, 2.0 }, gap);
				const Vec2 extent = (Vec2{ 0.3, 0.7 } * scale), far = (point + sign * extent);
				for (const double r : { 0.0, radius })
				{
					Check(RoundRect{ (Min(point.x, far.x) - r), (Min(point.y, far.y) - r),
						(extent.x + 2 * r), (extent.y + 2 * r), r }, (expected * scale - r));
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.CircularRepresentations")
{
	const auto Check = [](const SuperEllipse& shape, const Circle& circle)
	{
		const auto reference = Geometry2D::ClosestPoints(shape, circle);
		const double distance = Geometry2D::Distance(shape, circle);
		const auto CheckRepresentation = [&](const auto& other)
		{
			const auto ab = Geometry2D::ClosestPoints(shape, other), ba = Geometry2D::ClosestPoints(other, shape);
			CHECK(Geometry2D::Distance(shape, other) == distance);
			CHECK(Geometry2D::Distance(other, shape) == distance);
			REQUIRE(static_cast<bool>(ab) == static_cast<bool>(reference));
			REQUIRE(static_cast<bool>(ba) == static_cast<bool>(reference));
			if (not ab) return;
			CHECK(ab->distance == reference->distance);
			CHECK(ba->distance == reference->distance);
			CHECK(Geometry2D::Distance(ab->pointA, shape) <= 1.0e-10);
			CHECK(Geometry2D::Distance(ab->pointB, circle) <= 1.0e-10);
			CHECK(Geometry2D::Distance(ba->pointA, circle) <= 1.0e-10);
			CHECK(Geometry2D::Distance(ba->pointB, shape) <= 1.0e-10);
			CheckWitnessConsistency(*ab);
			CheckWitnessConsistency(*ba);
		};
		CheckRepresentation(Ellipse{ circle.center, circle.r, circle.r });
		CheckRepresentation(SuperEllipse{ circle.center, circle.r, circle.r, 2.0 });
	};
	for (const double n : { 0.25, 0.5, 0.9, 1.0, 1.5, 2.0, 4.0, 64.0 })
	{
		CAPTURE(n);
		const SuperEllipse shape{ 0, 0, 1, 2, n };
		for (const double gap : { -1.0e-6, 0.0, 1.0e-8, 0.5 }) Check(shape, Circle{ (1.25 + gap), 0, 0.25 });
		Check(shape, Circle{ 1.5, 2.5, 0.25 });
		Check(shape, Circle{ 0.8, 1.8, 0.3 });
		Check(shape, Circle{ 0, 0, 3 });
		Check(shape, Circle{ 0, 0, 0.001 });
		Check(shape, Circle{ 3, 0, 0 });
		Check(SuperEllipse{ 0, 0, 0, 2, n }, Circle{ 2, 0, 0.5 });
		Check(SuperEllipse{ 0, 0, 0, 0, n }, Circle{ 2, 0, 0.5 });
	}
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConcavePairs")
{
	const auto Check = [](const SuperEllipse& a, const SuperEllipse& b, const double gap,
		const double scale, const Vec2& pointA, const Vec2& pointB)
	{
		const auto ab = Geometry2D::ClosestPoints(a, b), ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		const double tolerance = (1.0e-11 * scale), expected = Max(0.0, gap);
		CHECK(Abs(ab->distance - expected) <= tolerance);
		CHECK(Geometry2D::Distance(a, b) == ab->distance);
		CHECK(Geometry2D::Distance(b, a) == ab->distance);
		CHECK(ba->distance == ab->distance);
		if (0.0 < gap)
		{
			CHECK(ab->distance > 0.0);
			CHECK(ab->pointA.distanceFrom(pointA) <= (1.0e-9 * scale));
			CHECK(ab->pointB.distanceFrom(pointB) <= (1.0e-9 * scale));
			CHECK(ba->pointA.distanceFrom(pointB) <= (1.0e-9 * scale));
			CHECK(ba->pointB.distanceFrom(pointA) <= (1.0e-9 * scale));
		}
		else
		{
			CHECK(ab->distance == 0.0);
			CHECK(ab->pointA == ab->pointB);
			CHECK(Geometry2D::Distance(ab->pointA, a) <= tolerance);
			CHECK(Geometry2D::Distance(ab->pointB, b) <= tolerance);
		}
		CheckWitnessConsistency(*ab, tolerance);
		CheckWitnessConsistency(*ba, tolerance);
	};
	for (const double n : { 0.25, 0.5, 0.9, 1.0 })
	{
		for (const double m : { 0.25, 0.5, 0.9, 1.0 })
		{
			for (const double scale : { 0.001, 1.0, 1000.0 })
			{
				for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
				{
					const SuperEllipse a{ (Vec2{ 7, -11 } * scale), scale, scale, n };
					for (const double t : { 0.2, 0.8 })
					{
						const Vec2 p{ std::pow(t, (1.0 / n)), std::pow((1.0 - t), (1.0 / n)) };
						const Vec2 normal = Vec2{ std::pow(p.x, (n - 1.0)), std::pow(p.y, (n - 1.0)) }.normalized();
						const double width = (0.01 * Min(p.x, (1.0 - p.x)));
						const Vec2 axes{ width, (0.1 * width * normal.x / normal.y) };
						const Vec2 boundary = (a.center + sign * p * scale);
						for (const double gap : { (-1.0e-8 * scale), 0.0, (1.0e-8 * scale) })
						{
							CAPTURE(n, m, scale, sign, t, gap);
							const Vec2 tip = (boundary + sign * normal * gap);
							const SuperEllipse b{ (tip + sign * Vec2{ axes.x * scale, 0 }), (axes * scale), m };
							Check(a, b, gap, scale, boundary, tip);
						}
					}
				}
			}
			const SuperEllipse a{ 0, 0, 1, 2, n }, b{ 3.25, 0, 2, 1, m };
			Check(a, b, 0.25, 1.0, Vec2{ 1, 0 }, Vec2{ 1.25, 0 });
			CHECK(Geometry2D::Distance(a, SuperEllipse{ 0, 0, 0.01, 0.01, m }) == 0.0);
		}
	}
	CHECK(Geometry2D::Distance(SuperEllipse{ 0, 0, 0, 2, 0.5 }, SuperEllipse{ 3, 0, 1, 1, 0.25 }) == 2.0);
	CHECK(std::isinf(Geometry2D::Distance(SuperEllipse{ 0, 0, 0, 0, 0.5 }, SuperEllipse{ 3, 0, 1, 1, 0.25 })));
	CHECK(not Geometry2D::ClosestPoints(SuperEllipse{ 0, 0, 1, 1, 0.5 }, SuperEllipse{ 3, 0, 0, 0, 0.25 }));
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.MixedPairs")
{
	for (const double n : { 1.5, 2.0, 4.0, 64.0 })
	{
		for (const double scale : { 0.001, 1.0, 1000.0 })
		{
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				for (const bool transpose : { false, true })
				{
					const SuperEllipse a{ (Vec2{ 7, -11 } * scale), scale, scale, 0.5 };
					const Vec2 point = ((transpose ? Vec2{ 0.64, 0.04 } : Vec2{ 0.04, 0.64 }) * scale);
					const Vec2 normal = (transpose ? Vec2{ 1, 4 } : Vec2{ 4, 1 }).normalized();
					const Vec2 axes = ((transpose ? Vec2{ 0.05, 0.1 } : Vec2{ 0.1, 0.05 }) * scale);
					const Vec2 v = (axes * normal);
					const double q = (n / (n - 1.0)), m = Max(v.x, v.y);
					const double divisor = std::pow((std::pow((v.x / m), q) + std::pow((v.y / m), q)), (1.0 / n));
					const Vec2 support = (axes * Vec2{ std::pow((v.x / m), (q - 1.0)), std::pow((v.y / m), (q - 1.0)) } / divisor);
					for (const double gap : { -1.0e-8, 0.0, 1.0e-8 })
					{
						CAPTURE(n, scale, sign, transpose, gap);
						const SuperEllipse b{ (a.center + sign * (point + normal * (gap * scale) + support)), axes, n };
						CheckMixedSuperEllipseDistance(a, b, (Max(0.0, gap) * scale), (1.0e-11 * scale));
					}
				}
			}
		}
	}
	for (const double n : { 0.25, 0.5, 0.9 })
	{
		const SuperEllipse a{ 0, 0, 1, 2, n };
		CheckMixedSuperEllipseDistance(a, SuperEllipse{ 3.25, 0, 2, 1, 4 }, 0.25, 1.0e-12);
		CheckMixedSuperEllipseDistance(a, SuperEllipse{ 0, 3.25, 2, 1, 2 }, 0.25, 1.0e-12);
		CheckMixedSuperEllipseDistance(a, SuperEllipse{ 0, 0, 0.01, 0.02, 4 }, 0.0, 1.0e-12);
	}
	const SuperEllipse a{ 0, 0, 1, 2, 0.5 };
	CHECK(std::isinf(Geometry2D::Distance(a, Ellipse{ 3, 0, 0, 0 })));
	CHECK(not Geometry2D::ClosestPoints(SuperEllipse{ 0, 0, 0, 0, 0.5 }, Ellipse{ 3, 0, 1, 2 }));
	CHECK(Geometry2D::Distance(SuperEllipse{ 0, 0, 0, 2, 0.5 }, Ellipse{ 3, 0, 1, 2 }) == 2.0);
	CHECK(Geometry2D::Distance(a, SuperEllipse{ 3, 0, 0, 2, 4 }) == 2.0);
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.MixedPairMinima")
{
	// Separate minima missed by coarse seeding, including a thin shape whose
	// closest point lies near an axial tip. Reference distances use dense search.
	CheckMixedSuperEllipseDistance(
		SuperEllipse{ 0, 0, 2.7563086875845366, 1.2434426535424061, 0.36805284783883035 },
		SuperEllipse{ 0.5048082286360136, 0.7933453398773076, 0.25748797487815833, 0.50768141006027545, 2 },
		0.11097376672506587, 1.0e-11);
	CheckMixedSuperEllipseDistance(
		SuperEllipse{ 0, 0, 5.8406796012664302, 1.5403312861778973, 0.63621451232530046 },
		SuperEllipse{ 9.7874166499689998, 1.4083116280788432, 7.793208721770033, 0.21742450246262071, 6.7739001647195956 },
		0.747070075839119, 1.0e-11);
	CheckMixedSuperEllipseDistance(
		SuperEllipse{ 0, 0, 0.025502223617786958, 31.913887048438212, 0.48369753025088813 },
		SuperEllipse{ 0.041105130505651753, 22.651017155391525, 0.032728322746886945, 0.11452966120632972, 2.0800000077166874 },
		0.0078521372885064101, 1.0e-11);
	// Affine images of an osculating circle give noncircular ellipses. Near
	// contact the objective is almost flat, so check distance and membership.
	for (const double ratio : { 0.1, 2.0, 10.0 })
	{
		const Vec2 axes{ 1, ratio }, normal = Vec2{ 1, (1.0 / ratio) }.normalized();
		const SuperEllipse a{ 0, 0, axes, 0.5 };
		CheckMixedSuperEllipseDistance(a,
			SuperEllipse{ (axes * 0.75 + normal * 1.0e-8), (axes * std::sqrt(0.5)), 2 }, 1.0e-8, 1.0e-11);
	}
	CheckMixedSuperEllipseDistance(SuperEllipse{ 0, 0, 1, 2, 0.5 },
		SuperEllipse{ (Vec2{ 0.75, 1.5 } + Vec2{ 1, 0.5 }.normalized() * 0.0001), (Vec2{ 1, 2 } * std::sqrt(0.5)), 2 },
		9.999766004090694e-5, 1.0e-11);
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConcavePointNeighborhoods")
{
	for (const double n : { 0.25, 0.5, 0.9 })
	{
		for (const Vec2 axes : { Vec2{ 1, 1 }, Vec2{ 100, 1 }, Vec2{ 1, 100 } })
		{
			const SuperEllipse shape{ 0, 0, axes.x, axes.y, n };
			for (const double t : { 0.01, 0.1, 0.3, 0.5, 0.7, 0.9, 0.99 })
			{
				const Vec2 normalized{ std::pow(t, (1.0 / n)), std::pow((1.0 - t), (1.0 / n)) };
				const Vec2 onShape = (axes * normalized);
				const Vec2 normal = Vec2{ (std::pow(normalized.x, (n - 1.0)) / axes.x),
					(std::pow(normalized.y, (n - 1.0)) / axes.y) }.normalized();
				for (const double gap : { 1.0e-10, 1.0e-6 })
				{
					CAPTURE(n, axes, t, gap);
					const Vec2 query = (onShape + normal * gap);
					const auto pair = Geometry2D::ClosestPoints(query, shape);
					REQUIRE(pair);
					CHECK(Abs(pair->distance - gap) <= 1.0e-12);
					CHECK(Abs(Geometry2D::Distance(query, shape) - gap) <= 1.0e-12);
					CHECK(pair->pointB.distanceFrom(onShape) <= 1.0e-10);
					CHECK(pair->pointA == query);
					CheckWitnessConsistency(*pair, 1.0e-12);
				}
			}
		}
	}
	for (const double n : { 0.25, 0.5, 0.9, 1.0, 2.0, 4.0, 64.0 })
	{
		CAPTURE(n);
		const SuperEllipse shape{ 0, 0, 100, 30, n };
		CHECK(Geometry2D::Distance(Point{ 101, 0 }, shape) == 1.0);
		CHECK(Geometry2D::Distance(Circle{ 102, 0, 1 }, shape) == 1.0);
		CHECK(Geometry2D::Distance(shape, RoundRect{ 101, -5, 10, 20, 2 }) == 1.0);
		CHECK(Geometry2D::Distance(shape, RoundRect{ -5, 31, 20, 10, 2 }) == 1.0);
		CHECK(Geometry2D::Distance(Vec2{ 0, 0 }, shape) == 0.0);
	}
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConcaveLineEndpoints")
{
	for (const double n : { 0.25, 0.5, 0.9, 1.0 })
	{
		for (const Vec2 axes : { Vec2{ 1, 1 }, Vec2{ 1, 20 }, Vec2{ 20, 1 } })
		{
			for (const double scale : { 0.001, 1.0, 1000.0 })
			{
				const SuperEllipse shape{ (Vec2{ 7, -11 } * scale), (axes * scale), n };
				for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
				{
					const auto BoundaryAndNormal = [&](const double t)
					{
						const Vec2 p{ std::pow(t, (1.0 / n)), std::pow((1.0 - t), (1.0 / n)) };
						const Vec2 normal = Vec2{ (std::pow(p.x, (n - 1.0)) / axes.x), (std::pow(p.y, (n - 1.0)) / axes.y) }.normalized();
						return std::pair{ (shape.center + sign * shape.axes * p), (sign * normal) };
					};
					const auto [first, normal0] = BoundaryAndNormal(0.2);
					const auto [second, normal1] = BoundaryAndNormal(0.6);
					for (const double gap : { (1.0e-10 * scale), (1.0e-6 * scale), (0.001 * scale) })
					{
						CAPTURE(n, axes, scale, sign, gap);
						const Vec2 start = (first + normal0 * gap), end = (second + normal1 * (2 * gap));
						for (const Line line : { Line{ start, end }, Line{ end, start }, Line{ start, start },
							Line{ start, (start + normal0 * (3 * scale)) } })
						{
							const double tolerance = (1.0e-11 * scale);
							const auto pair = Geometry2D::ClosestPoints(line, shape), reversed = Geometry2D::ClosestPoints(shape, line);
							REQUIRE(pair);
							REQUIRE(reversed);
							CHECK(Abs(pair->distance - gap) <= tolerance);
							CHECK(Abs(Geometry2D::Distance(line, shape) - gap) <= tolerance);
							CHECK(Geometry2D::Distance(shape, line) == Geometry2D::Distance(line, shape));
							CHECK(pair->pointA.distanceFrom(start) <= tolerance);
							CHECK(pair->pointB.distanceFrom(first) <= (1.0e-9 * scale));
							CHECK(pair->pointA == reversed->pointB);
							CHECK(pair->pointB == reversed->pointA);
							CheckWitnessConsistency(*pair, tolerance);
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.ConcaveLinearBoundaries")
{
	const SuperEllipse shape{ 0, 0, 1, 1, 0.5 };
	const double gap = 1.0e-8;
	const Vec2 boundary{ 0.04, 0.64 };
	const Vec2 start = (boundary + Vec2{ 4, 1 }.normalized() * gap);
	const Vec2 end = (Vec2{ 0.36, 0.16 } + Vec2{ 2, 3 }.normalized() * (2 * gap));
	const Quad quad{ start, end, (end + Vec2{ 3, 3 }), (start + Vec2{ 3, 3 }) };
	const Polygon polygon{ { quad.p0, quad.p1, quad.p2, quad.p3 } };
	REQUIRE(not polygon.isEmpty());
	const auto Check = [&](const auto& target)
	{
		const auto pair = Geometry2D::ClosestPoints(target, shape), reversed = Geometry2D::ClosestPoints(shape, target);
		REQUIRE(pair);
		REQUIRE(reversed);
		CHECK(Abs(pair->distance - gap) <= 1.0e-12);
		CHECK(Abs(Geometry2D::Distance(target, shape) - gap) <= 1.0e-12);
		CHECK(Geometry2D::Distance(shape, target) == Geometry2D::Distance(target, shape));
		CHECK(pair->pointA.distanceFrom(start) <= 1.0e-12);
		CHECK(pair->pointB.distanceFrom(boundary) <= 1.0e-10);
		CHECK(pair->pointA == reversed->pointB);
		CHECK(pair->pointB == reversed->pointA);
		CheckWitnessConsistency(*pair, 1.0e-12);
	};
	Check(Line{ start, end });
	Check(LineString{ start, end, quad.p2 });
	Check(Triangle{ start, end, quad.p2 });
	Check(quad);
	Check(polygon);
	Check(MultiPolygon{ polygon, Rect{ 10, 10, 1, 1 }.asPolygon() });

	const Polygon frame{
		Array<Vec2>{ { -3, -3 }, { 3, -3 }, { 3, 3 }, { -3, 3 } },
		Array<Array<Vec2>>{ { { -2, -2 }, { -2, 2 }, { 2, 2 }, { 2, -2 } } }
	};
	REQUIRE(not frame.isEmpty());
	const auto pair = Geometry2D::ClosestPoints(frame, shape);
	REQUIRE(pair);
	CHECK(pair->distance == 1.0);
	CHECK(Geometry2D::Distance(frame, shape) == 1.0);
	CHECK(Max(Abs(pair->pointA.x), Abs(pair->pointA.y)) == 2.0);
	CHECK(Geometry2D::Distance(pair->pointB, shape) == 0.0);
	CheckWitnessConsistency(*pair);
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.LinearBoundaries")
{
	for (const double n : { 0.25, 0.5, 0.9, 1.0, 1.25, 2.0, 4.0, 64.0 })
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

TEST_CASE("Geometry2D.Distance.CommonPoints.Representatives")
{
	const auto CheckCommon = [](const auto& a, const auto& b)
	{
		const auto ab = Geometry2D::ClosestPoints(a, b), ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(ab->distance == 0.0);
		CHECK(ba->distance == 0.0);
		CHECK(ab->pointA == ab->pointB);
		CHECK(ba->pointA == ba->pointB);
		CHECK(Geometry2D::Intersects(ab->pointA, a));
		CHECK(Geometry2D::Intersects(ab->pointA, b));
		CHECK(Geometry2D::Intersects(ba->pointA, a));
		CHECK(Geometry2D::Intersects(ba->pointA, b));
	};
	const Polygon donut{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 6, 6 }, { 6, 14 }, { 14, 14 }, { 14, 6 } } }
	};
	// The rectangle's center is in the hole; its corners belong to the polygon.
	CheckCommon(RectF{ 5, 5, 10, 10 }, donut);
	CheckCommon(Circle{ 2, 2, 1 }, donut);
	CheckCommon(MultiPolygon{ Polygon{}, donut }, RectF{ 1, 1, 2, 2 });
	CheckCommon(RectF{ 0, 4, 10, 2 }, RectF{ 4, 0, 2, 10 });
	CheckCommon(Circle{ 0, 0, 2 }, Circle{ 3, 0, 2 });

	LineString path;
	for (int32 x = -20; x <= 2; ++x) path.emplace_back(x, 0);
	// The common points lie beyond the first 12 vertices, along a shared boundary.
	CheckCommon(path, RectF{ 0, 0, 10, 10 });
	CHECK(Geometry2D::Distance(RectF{ 7, 7, 2, 2 }, donut) == 1.0);
	CHECK_FALSE(Geometry2D::ClosestPoints(Polygon{}, donut).has_value());
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

TEST_CASE("Geometry2D.Distance.Bezier.EllipticSeparation")
{
	const auto CheckPair = [](const auto& curve, const auto& shape, const double gap, const double tolerance)
	{
		for (const auto& c : { curve, curve.reversed() })
		{
			const auto ab = Geometry2D::ClosestPoints(c, shape), ba = Geometry2D::ClosestPoints(shape, c);
			REQUIRE(ab);
			REQUIRE(ba);
			CHECK(Abs(ab->distance - gap) <= tolerance);
			CHECK(ab->distance == ba->distance);
			CHECK(ab->distance == Geometry2D::Distance(c, shape));
			CHECK(ab->distance == Geometry2D::Distance(shape, c));
			CHECK(ab->pointA == ba->pointB);
			CHECK(ab->pointB == ba->pointA);
			CHECK(Geometry2D::Distance(ab->pointA, c) <= tolerance);
			CHECK(Geometry2D::Distance(ab->pointB, shape) <= tolerance);
			CheckWitnessConsistency(*ab, tolerance);
		}
	};
	for (const double n : { 1.1, 1.5, 2.0, 4.0, 16.0, 64.0 })
	{
		for (const Vec2& normal : { Vec2{ 0, 1 }, Vec2{ -1, 0 }, Vec2{ 0.6, 0.8 } })
		{
			for (const double scale : { 0.01, 1.0, 100.0 })
			{
				const Vec2 offset = (Vec2{ -3, 2 } * scale), axes = (Vec2{ 1, 0.5 } * scale);
				const SuperEllipse shape{ offset, axes, n };
				const double q = (n / (n - 1.0));
				const Vec2 weighted = (axes * normal);
				const double norm = std::pow((std::pow(Abs(weighted.x), q) + std::pow(Abs(weighted.y), q)), (1.0 / q));
				const Vec2 support = offset + axes * Vec2{
					std::copysign(std::pow((Abs(weighted.x) / norm), (q - 1.0)), normal.x),
					std::copysign(std::pow((Abs(weighted.y) / norm), (q - 1.0)), normal.y) };
				const double gap = (1.0e-8 * scale), t0 = 0.371;
				const Vec2 tangent{ -normal.y, normal.x }, base = (support + normal * gap);
				// The entire curve lies beyond a supporting line by at least gap,
				// with equality at t0. This gives a global, analytic distance oracle.
				const auto PointAt = [&](const double t, const double cubic)
				{
					const double u = (t - t0);
					return (base + scale * (tangent * (2.0 * u + cubic * u * u * u) + normal * (0.5 * u * u)));
				};
				const auto Derivative = [&](const double t, const double cubic)
				{
					const double u = (t - t0);
					return (scale * (tangent * (2.0 + 3.0 * cubic * u * u) + normal * u));
				};
				const Bezier2 quadratic{ PointAt(0, 0), PointAt(0, 0) + Derivative(0, 0) * 0.5, PointAt(1, 0) };
				const auto CheckShape = [&](const auto& target)
				{
					CheckPair(quadratic, target, gap, (1.0e-11 * scale));
					for (const double cubic : { 0.0, 0.8 })
					{
						const Bezier3 curve{ PointAt(0, cubic), PointAt(0, cubic) + Derivative(0, cubic) / 3.0,
							PointAt(1, cubic) - Derivative(1, cubic) / 3.0, PointAt(1, cubic) };
						CheckPair(curve, target, gap, (1.0e-11 * scale));
					}
				};
				CheckShape(shape);
				if (n == 2.0) CheckShape(Ellipse{ offset, axes });
			}
		}
	}

	for (const double n : { 0.25, 0.5, 0.9 })
	{
		// The curve stays above the top tip, including between control points.
		const double t = 0.371, gap = 1.0e-8;
		const Bezier2 curve{ { -2.0 * t, 0.5 + gap + 0.5 * t * t },
			{ 1.0 - 2.0 * t, 0.5 + gap + 0.5 * t * t - 0.5 * t },
			{ 2.0 * (1.0 - t), 0.5 + gap + 0.5 * (1.0 - t) * (1.0 - t) } };
		const SuperEllipse shape{ 0, 0, 1, 0.5, n };
		CheckPair(curve, shape, gap, 1.0e-11);
		CheckPair(Bezier3{ curve.p0, curve.p0 + (curve.p1 - curve.p0) * (2.0 / 3),
			curve.p2 + (curve.p1 - curve.p2) * (2.0 / 3), curve.p2 }, shape, gap, 1.0e-11);
	}

	// Separate concave boundary branches compete for the nearest point.
	// References were computed by dense parameter sampling and interval refinement.
	CheckPair(Bezier3{ { 5.5617564617795381, 0.17666508013798171 }, { -4.5604151127169157, 0.17801755108014303 },
		{ 2.767875558078444, 0.10413739717110759 }, { -1.8121046520184694, 0.10440929234143076 } },
		SuperEllipse{ 0, 0, 2.8158581215670875, 0.11199782827096076, 0.23935855964217026 }, 0.042654805061624675, 1.0e-10);
	CheckPair(Bezier3{ { 2.6442679241788132, 0.29931210085442944 }, { 0.23820109294743458, -0.13312520079403226 },
		{ 1.7313673108963381, 0.35408600902009257 }, { -1.1546491201596358, 0.22088996920380138 } },
		SuperEllipse{ 0, 0, 1.4341105070007207, 0.18346678812697309, 0.17032970518439911 }, 0.058647606779981779, 1.0e-10);
}

TEST_CASE("Geometry2D.Distance.Bezier.EllipticReductions")
{
	const auto CheckCurve = [](const auto& curve)
	{
		for (const double radius : { 0.0, 0.01, 1.0, 10.0 })
		{
			const Vec2 center{ 0.25, 0.1 };
			const auto expected = Geometry2D::ClosestPoints(curve, Circle{ center, radius });
			const auto Check = [&](const auto& shape)
			{
				const auto actual = Geometry2D::ClosestPoints(curve, shape);
				CHECK(static_cast<bool>(actual) == static_cast<bool>(expected));
				if (actual && expected)
				{
					CHECK(Abs(actual->distance - expected->distance) < 1.0e-12);
					CHECK(Abs(Geometry2D::Distance(curve, shape) - expected->distance) < 1.0e-12);
					CheckWitnessConsistency(*actual);
				}
				else
				{
					CHECK(std::isinf(Geometry2D::Distance(curve, shape)));
				}
			};
			Check(Ellipse{ center, radius, radius });
			Check(SuperEllipse{ center, radius, radius, 2.0 });
		}
		for (const Vec2& axes : { Vec2{ 2, 0.5 }, Vec2{ 0.5, 2 }, Vec2{ 0, 2 }, Vec2{ 2, 0 }, Vec2{ 0, 0 } })
		{
			const Ellipse ellipse{ 0, 0, axes };
			const SuperEllipse equivalent{ ellipse, 2.0 };
			const auto a = Geometry2D::ClosestPoints(curve, ellipse), b = Geometry2D::ClosestPoints(curve, equivalent);
			CHECK(static_cast<bool>(a) == static_cast<bool>(b));
			CHECK(Geometry2D::Distance(curve, ellipse) == Geometry2D::Distance(curve, equivalent));
			if (a && b)
			{
				CHECK(a->distance == b->distance);
				CheckWitnessConsistency(*a);
				CHECK(Geometry2D::Distance(a->pointA, curve) < 1.0e-10);
				CHECK(Geometry2D::Distance(a->pointB, ellipse) < 1.0e-10);
			}
		}
	};
	for (const Bezier2& q : { Bezier2{ { -1, 0 }, { 0, 0.25 }, { 1, 0 } },
		Bezier2{ { -1, 0.5 }, { 0, 0.5 }, { -1, 0.5 } },
		Bezier2{ { 3, 0.5 }, { 1, 0.5 }, { 3, 0.5 } },
		Bezier2{ { 3, 4 }, { 3, 4 }, { 3, 4 } } })
	{
		CheckCurve(q);
		CheckCurve(Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 });
	}
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

TEST_CASE("Geometry2D.Distance.SuperEllipse.CommonPoints")
{
	const auto Support = [](const Vec2& axes, const double n, const Vec2& normal)
	{
		const double q = (n / (n - 1.0));
		const double x = Abs(axes.x * normal.x), y = Abs(axes.y * normal.y), scale = Max(x, y);
		const double denominator = std::pow(std::pow(x / scale, q) + std::pow(y / scale, q), (1.0 / n));
		return Vec2{ std::copysign(axes.x * std::pow(x / scale, q - 1.0) / denominator, normal.x),
			std::copysign(axes.y * std::pow(y / scale, q - 1.0) / denominator, normal.y) };
	};
	const auto CheckInside = [](const Vec2& point, const SuperEllipse& shape)
	{
		const Vec2 p = ((point - shape.center) / shape.axes);
		// A spatial allowance also works at concave tips with unbounded implicit derivatives.
		CHECK((std::pow(Max(0.0, Abs(p.x) - 2.0e-12), shape.n)
			+ std::pow(Max(0.0, Abs(p.y) - 2.0e-12), shape.n)) <= (1.0 + 1.0e-12));
	};
	const auto CheckPair = [&](const auto& a, const auto& b, const SuperEllipse& areaA, const SuperEllipse& areaB)
	{
		const auto ab = Geometry2D::ClosestPoints(a, b), ba = Geometry2D::ClosestPoints(b, a);
		REQUIRE(ab);
		REQUIRE(ba);
		CHECK(ab->distance == 0.0);
		CHECK(ba->distance == 0.0);
		CHECK(ab->pointA == ab->pointB);
		CHECK(ba->pointA == ba->pointB);
		CHECK(Geometry2D::Distance(a, b) == ab->distance);
		CHECK(Geometry2D::Distance(b, a) == ba->distance);
		CheckInside(ab->pointA, areaA);
		CheckInside(ab->pointB, areaB);
		CheckInside(ba->pointA, areaB);
		CheckInside(ba->pointB, areaA);
	};
	// Oblique tangencies include a case where intersection enumeration misses
	// the contact and the former representative-point fallback asserted.
	for (const auto [na, nb] : { std::pair{ 1.01, 1.01 }, { 1.5, 4.0 }, { 4.0, 64.0 }, { 2.0, 4.0 } })
	{
		for (const double scale : { 1.0e-6, 1.0, 1.0e6 })
		{
			for (const Vec2 sign : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
			{
				for (const Vec2 direction : { Vec2{ std::cos(0.423), std::sin(0.423) }, Vec2{ 0.6, 0.8 }, Vec2{ 1, 0 } })
				{
					for (const double gap : { -1.0e-6, 0.0 })
					{
						const Vec2 normal = (direction * sign);
						const SuperEllipse a{ Vec2{ 7, -11 } * scale, scale, 0.3 * scale, na };
						const Vec2 center = (a.center + Support(a.axes, na, normal)
							+ Support(Vec2{ 0.3, 0.7 } * scale, nb, normal) + normal * (gap * scale));
						const SuperEllipse b{ center, 0.3 * scale, 0.7 * scale, nb };
						CAPTURE(na, nb, scale, normal, gap);
						CheckPair(a, b, a, b);
						if (na == 2.0)
						{
							CheckPair(Ellipse{ a.center, a.axes }, b, a, b);
						}
					}
				}
			}
		}
	}
	for (const double n : { 0.25, 0.5, 0.75, 1.0, 1.5, 4.0 })
	{
		const SuperEllipse a{ 0, 0, 1, 1, n };
		CheckPair(a, SuperEllipse{ 1.25, 0, 0.25, 0.5, 0.5 }, a, SuperEllipse{ 1.25, 0, 0.25, 0.5, 0.5 });
		CheckPair(a, Circle{ 1.25, 0, 0.25 }, a, SuperEllipse{ 1.25, 0, 0.25, 0.25, 2 });
		CheckPair(a, RoundRect{ 1, -0.25, 1, 0.5, 0.25 }, a, SuperEllipse{ 1.25, 0, 0.25, 0.25, 2 });
		CheckPair(a, Circle{ 0, 0, 0.01 }, a, SuperEllipse{ 0, 0, 0.01, 0.01, 2 });
	}
	// Equal curvature at an off-axis contact in a concave/convex pair.
	const SuperEllipse concave{ 0, 0, 1, 1, 0.5 }, circle{ 0.75, 0.75, std::sqrt(0.5), std::sqrt(0.5), 2 };
	CheckPair(concave, circle, concave, circle);
	CheckPair(concave, Circle{ circle.center, circle.a }, concave, circle);
	CheckPair(concave, RoundRect{ (circle.center - circle.axes), circle.axes * 2.0, circle.a }, concave, circle);
}

TEST_CASE("Geometry2D.Distance.SuperEllipse.UnresolvedContact")
{
	const SuperEllipse a{ 0, 0, 1.1015102504184453, 71.991196139766672, 0.59369714221657377 };
	const SuperEllipse b{ 1.3608167100829833, 38.82908854224911, 1.2102148265530634, 0.082438418604875802, 2.2372216082402701 };
	// An exhausted intersection search does not establish a common point.
	CHECK(Geometry2D::Intersects(a, b));
	const auto ab = Geometry2D::ClosestPoints(a, b), ba = Geometry2D::ClosestPoints(b, a);
	REQUIRE(ab);
	REQUIRE(ba);
	CHECK(ab->distance > 0.0);
	CHECK(ba->distance > 0.0);
	CHECK(ab->distance < 1.0e-10);
	CHECK(ba->distance == ab->distance);
	CHECK(ba->pointA == ab->pointB);
	CHECK(ba->pointB == ab->pointA);
	CHECK(Geometry2D::Distance(a, b) == ab->distance);
	CHECK(Geometry2D::Distance(b, a) == ba->distance);
	CheckWitnessConsistency(*ab);
	CheckWitnessConsistency(*ba);
	const auto CheckBoundary = [](const Vec2& point, const SuperEllipse& shape)
	{
		const Vec2 p = ((point - shape.center) / shape.axes);
		CHECK(Abs(std::pow(Abs(p.x), shape.n) + std::pow(Abs(p.y), shape.n) - 1.0) < 1.0e-10);
	};
	CheckBoundary(ab->pointA, a);
	CheckBoundary(ab->pointB, b);
	CheckBoundary(ba->pointA, b);
	CheckBoundary(ba->pointB, a);
}
