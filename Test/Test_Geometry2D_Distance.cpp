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
