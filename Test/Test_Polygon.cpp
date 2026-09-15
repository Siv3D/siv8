//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"

namespace
{
	Polygon MakePolygonWithHole()
	{
		return Polygon{
			Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
			Array<Array<Vec2>>{ { { 6, 6 }, { 6, 14 }, { 14, 14 }, { 14, 6 } } }
		};
	}

	void CheckPolygonRings(const Polygon& polygon, const size_t holeCount)
	{
		REQUIRE(not polygon.isEmpty());
		REQUIRE(polygon.outer().size() >= 3);
		CHECK(polygon.triangleCount() > 0);
		CHECK(polygon.holeCount() == holeCount);
		CHECK(Geometry2D::IsClockwise(polygon.outer()));
		CHECK(polygon.outer().front() != polygon.outer().back());

		for (const auto& hole : polygon.inners())
		{
			REQUIRE(hole.size() >= 3);
			CHECK_FALSE(Geometry2D::IsClockwise(hole));
			CHECK(hole.front() != hole.back());
		}

		CHECK(Polygon::Validate(polygon.outer(), polygon.inners()) == PolygonFailureType::Ok);

		const auto parsed = Polygon::Parse(Format(polygon));
		REQUIRE(parsed);
		CHECK(parsed->outer() == polygon.outer());
		CHECK(parsed->inners() == polygon.inners());
	}

	void CheckTriangleIndices(const Polygon& polygon, const Polygon& reference)
	{
		REQUIRE(polygon.indices().size() == reference.indices().size());
		for (size_t i = 0; i < polygon.indices().size(); ++i)
		{
			CAPTURE(i);
			CHECK(polygon.indices()[i].i0 == reference.indices()[i].i0);
			CHECK(polygon.indices()[i].i1 == reference.indices()[i].i1);
			CHECK(polygon.indices()[i].i2 == reference.indices()[i].i2);
		}
	}

	template <class PointType>
	void CheckConvexHullCases()
	{
		const Array<Array<PointType>> degenerateCases{
			{},
			{ { 1, 2 } },
			{ { 0, 0 }, { 4, 4 } },
			{ { 1, 2 }, { 1, 2 }, { 1, 2 } },
			{ { 0, 0 }, { 4, 4 }, { 0, 0 }, { 4, 4 } },
			{ { 4, 0 }, { 0, 0 }, { 2, 0 }, { 1, 0 } },
			{ { 3, 4 }, { 3, 0 }, { 3, 2 }, { 3, 1 } },
			{ { -2, -2 }, { 2, 2 }, { 0, 0 }, { 1, 1 } },
		};

		for (size_t i = 0; i < degenerateCases.size(); ++i)
		{
			CAPTURE(i);
			const Polygon hull = Geometry2D::ConvexHull(std::span<const PointType>{ degenerateCases[i] });
			CHECK(hull.isEmpty());
			CHECK(hull.vertices().isEmpty());
			CHECK(hull.indices().isEmpty());
			CHECK(hull.holeCount() == 0);
		}

		const Array<PointType> points{
			{ 4, 4 }, { 2, 2 }, { 0, 0 }, { 4, 0 }, { 0, 4 }, { 2, 0 }, { 0, 0 },
		};
		const Polygon hull = Geometry2D::ConvexHull(std::span<const PointType>{ points });
		CheckPolygonRings(hull, 0);
		CHECK(hull.area() == Test::Approx(16.0));
		CHECK(hull.boundingRect() == RectF{ 0, 0, 4, 4 });
		for (const auto& point : points)
		{
			CHECK(Geometry2D::Contains(hull, Vec2{ point }));
		}
	}
}

TEST_CASE("Polygon.generated_rings.boolean_operations")
{
	const RectF outer{ 0, 0, 20, 20 };
	const RectF hole{ 6, 6, 8, 8 };
	const Polygon outerPolygon = outer.asPolygon();
	const Polygon holePolygon = hole.asPolygon();
	const Polygon donut = MakePolygonWithHole();
	const RectF contained{ 1, 1, 2, 2 };

	const Array<MultiPolygon> results{
		Geometry2D::Subtract(outer, holePolygon),
		Geometry2D::Subtract(outerPolygon, hole),
		Geometry2D::Subtract(outerPolygon, holePolygon),
		Geometry2D::Xor(outer, holePolygon),
		Geometry2D::Xor(holePolygon, outer),
		Geometry2D::Xor(outerPolygon, holePolygon),
		Geometry2D::And(outer, donut),
		Geometry2D::And(donut, outer),
		Geometry2D::And(outerPolygon, donut),
		Geometry2D::Or(contained, donut),
		Geometry2D::Or(donut, contained),
		Geometry2D::Or(donut, contained.asPolygon()),
		Geometry2D::Or(MultiPolygon{ donut }, contained.asPolygon()),
	};

	for (size_t i = 0; i < results.size(); ++i)
	{
		CAPTURE(i);
		REQUIRE(results[i].size() == 1);
		const Polygon& result = results[i][0];
		CheckPolygonRings(result, 1);
		CHECK(result.area() == Test::Approx(336.0));
		CHECK(Geometry2D::Contains(result, Vec2{ 2, 2 }));
		CHECK_FALSE(Geometry2D::Contains(result, Vec2{ 10, 10 }));

		const MultiPolygon filled = Geometry2D::Or(result, hole);
		REQUIRE(filled.size() == 1);
		CheckPolygonRings(filled[0], 0);
		CHECK(filled[0].area() == Test::Approx(400.0));

		const MultiPolygon twoHoles = Geometry2D::Subtract(result, contained);
		REQUIRE(twoHoles.size() == 1);
		CheckPolygonRings(twoHoles[0], 2);
		CHECK(twoHoles[0].area() == Test::Approx(332.0));
	}
}

TEST_CASE("Polygon.generated_rings.buffers_and_simplification")
{
	const Polygon donut = MakePolygonWithHole();
	for (const double distance : { -1.0, 1.0 })
	{
		CAPTURE(distance);
		const Polygon miter = donut.computeMiterBufferPolygon(distance);
		CheckPolygonRings(miter, 1);
		CHECK(miter.area() == Test::Approx(distance > 0.0 ? 448.0 : 224.0));

		const Polygon round = donut.computeRoundBufferPolygon(distance);
		CheckPolygonRings(round, 1);
		CHECK(Geometry2D::Contains(round, Vec2{ 3, 3 }));
		CHECK_FALSE(Geometry2D::Contains(round, Vec2{ 10, 10 }));
		CHECK((round.area() > donut.area()) == (distance > 0.0));
	}

	const Polygon simplified = donut.simplified(0.25);
	CheckPolygonRings(simplified, 1);
	CHECK(simplified.area() == Test::Approx(336.0));
}

TEST_CASE("Polygon.generated_rings.other_construction_paths")
{
	const Polygon donut = MakePolygonWithHole();
	CheckPolygonRings(donut, 1);
	Polygon appended = donut;
	REQUIRE(appended.append(RectF{ 1, 1, 2, 2 }));
	CheckPolygonRings(appended, 1);
	REQUIRE(appended.append(RectF{ 2, 2, 2, 2 }.asPolygon()));
	CheckPolygonRings(appended, 1);
	CHECK(appended.area() == Test::Approx(336.0));

	const auto corrected = Polygon::Correct(donut.outer(), donut.inners());
	REQUIRE(corrected.size() == 1);
	CheckPolygonRings(corrected[0], 1);
	const auto repaired = Polygon::Correct(donut.outer().reversed(), donut.inners());
	REQUIRE(repaired.size() == 1);
	CheckPolygonRings(repaired[0], 1);
	CHECK(repaired[0].area() == Test::Approx(336.0));

	const LineString ring{ donut.outer() };
	CheckPolygonRings(ring.computeMiterBufferPolygon(2.0, CloseRing::Yes), 1);
	CheckPolygonRings(ring.computeRoundBufferPolygon(2.0, CloseRing::Yes), 1);
}

TEST_CASE("Geometry2D.ConvexHull.Point")
{
	CheckConvexHullCases<Point>();
}

TEST_CASE("Geometry2D.ConvexHull.Float2")
{
	CheckConvexHullCases<Float2>();
}

TEST_CASE("Geometry2D.ConvexHull.Vec2")
{
	CheckConvexHullCases<Vec2>();
}

TEST_CASE("Polygon.computeConvexHull")
{
	CHECK(Polygon{}.computeConvexHull().isEmpty());
	const Polygon donut = MakePolygonWithHole();
	const Polygon hull = donut.computeConvexHull();
	CheckPolygonRings(hull, 0);
	CHECK(hull.area() == Test::Approx(400.0));
	CHECK(hull.boundingRect() == donut.boundingRect());

	for (const Vec2 scale : { Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 0, 1 } })
	{
		const Polygon collapsedHull = donut.scaledFromOrigin(scale).computeConvexHull();
		CHECK(collapsedHull.isEmpty());
		CHECK(collapsedHull.vertices().isEmpty());
		CHECK(collapsedHull.indices().isEmpty());
	}
}

TEST_CASE("Polygon.outline.empty")
{
	const Polygon polygon;
	CHECK(polygon.outline().isEmpty());
	CHECK(polygon.outline(CloseRing::Yes).isEmpty());
	CHECK(polygon.outline(0.0, 10.0).isEmpty());
	CHECK(polygon.outline(0.0, 0.0).isEmpty());
	CHECK(polygon.outline(-5.0, -10.0).isEmpty());
}

TEST_CASE("Polygon.outline.segment")
{
	const Polygon polygon{ { { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } } };
	const LineString firstCorner{ { 5, 0 }, { 10, 0 }, { 10, 5 } };
	CHECK(polygon.outline(5.0, 10.0) == firstCorner);
	CHECK(polygon.outline(15.0, -10.0) == firstCorner);
	const LineString closingCorner{ { 0, 5 }, { 0, 0 }, { 5, 0 } };
	CHECK(polygon.outline(35.0, 10.0) == closingCorner);
	CHECK(polygon.outline(-5.0, 10.0) == closingCorner);
	CHECK(polygon.outline(5.0, 0.0) == LineString{ { 5, 0 }, { 5, 0 } });
}

TEST_CASE("Polygon.construction.moves_owned_holes")
{
	for (const auto skipValidation : { SkipValidation::No, SkipValidation::Yes })
	{
		for (const bool suppliedBounds : { false, true })
		{
			CAPTURE(suppliedBounds, skipValidation);
			const Polygon reference = MakePolygonWithHole();
			Array<Vec2> outer = reference.outer();
			Array<Array<Vec2>> holes = reference.inners();
			const auto* holeArrayStorage = holes.data();
			const auto* holeVertexStorage = holes.front().data();
			const RectF bounds{ -1, -1, 22, 22 };

			const Polygon polygon = suppliedBounds
				? Polygon{ outer, std::move(holes), bounds, skipValidation }
				: Polygon{ outer, std::move(holes), skipValidation };
			CheckPolygonRings(polygon, 1);
			CHECK(polygon.inners().data() == holeArrayStorage);
			CHECK(polygon.inners().front().data() == holeVertexStorage);
			CHECK(polygon.outer().data() != outer.data());
			CHECK(polygon.boundingRect() == (suppliedBounds ? bounds : reference.boundingRect()));
			CHECK(polygon.vertices() == reference.vertices());
			CheckTriangleIndices(polygon, reference);

			outer.front() = Vec2{ -100, -100 };
			CHECK(polygon.outer() == reference.outer());
		}
	}
}

TEST_CASE("Polygon.construction.multiple_holes_triangulation")
{
	const Array<Vec2> outer{ { 0, 0 }, { 12, 0 }, { 12, 4 }, { 8, 4 }, { 8, 12 }, { 0, 12 } };
	const Array<Array<Vec2>> holes{
		{ { 1, 1 }, { 1, 3 }, { 3, 3 }, { 3, 1 } },
		{ { 1, 7 }, { 1, 10 }, { 4, 10 }, { 4, 7 } },
	};
	const Polygon polygon{ outer, holes };
	CheckPolygonRings(polygon, 2);
	CHECK(polygon.area() == Test::Approx(99.0));
	CHECK(polygon.outer() == outer);
	CHECK(polygon.inners() == holes);

	Array<Float2> expectedVertices;
	expectedVertices.append_range(outer);
	for (const auto& hole : holes)
	{
		expectedVertices.append_range(hole);
	}
	CHECK(polygon.vertices() == expectedVertices);

	double triangleArea = 0.0;
	for (const auto& index : polygon.indices())
	{
		REQUIRE(index.i0 < expectedVertices.size());
		REQUIRE(index.i1 < expectedVertices.size());
		REQUIRE(index.i2 < expectedVertices.size());
		const Vec2 p0 = expectedVertices[index.i0];
		const Vec2 p1 = expectedVertices[index.i1];
		const Vec2 p2 = expectedVertices[index.i2];
		const double area2x = (p1 - p0).cross(p2 - p0);
		CHECK(area2x > 0.0);
		triangleArea += (area2x * 0.5);
	}
	CHECK(triangleArea == Test::Approx(99.0));
	CHECK(Geometry2D::Contains(polygon, Vec2{ 6, 6 }));
	CHECK_FALSE(Geometry2D::Contains(polygon, Vec2{ 2, 2 }));
	CHECK_FALSE(Geometry2D::Contains(polygon, Vec2{ 2, 8 }));
	CHECK_FALSE(Geometry2D::Contains(polygon, Vec2{ 10, 8 }));
}

TEST_CASE("Polygon.construction.validation_failure")
{
	const Polygon reference = MakePolygonWithHole();
	for (const bool suppliedBounds : { false, true })
	{
		CAPTURE(suppliedBounds);
		Array<Array<Vec2>> holes = reference.inners();
		holes.front().reverse();
		const Polygon polygon = suppliedBounds
			? Polygon{ reference.outer(), std::move(holes), reference.boundingRect() }
			: Polygon{ reference.outer(), std::move(holes) };
		CHECK(polygon.isEmpty());
		CHECK(polygon.inners().isEmpty());
		CHECK(polygon.vertices().isEmpty());
		CHECK(polygon.indices().isEmpty());
		CHECK(polygon.boundingRect() == RectF::Empty());
	}
}

TEST_CASE("Polygon.append.disconnected_preserves_original")
{
	Polygon polygon = MakePolygonWithHole();
	const Polygon original = polygon;
	CHECK_FALSE(polygon.append(RectF{ 30, 30, 5, 5 }));
	CHECK_FALSE(polygon.append(RectF{ 30, 30, 5, 5 }.asPolygon()));
	CHECK(polygon.outer() == original.outer());
	CHECK(polygon.inners() == original.inners());
	CHECK(polygon.vertices() == original.vertices());
	CheckTriangleIndices(polygon, original);
}

TEST_CASE("Polygon.generated_rings.disconnected_union")
{
	const Polygon donut = MakePolygonWithHole();
	const Polygon other = RectF{ 30, 30, 5, 5 }.asPolygon();
	const Array<MultiPolygon> results{
		Geometry2D::Or(donut, other),
		Geometry2D::Or(MultiPolygon{ donut }, other),
	};
	for (const auto& result : results)
	{
		REQUIRE(result.size() == 2);
		size_t holes = 0;
		double area = 0.0;
		for (const auto& polygon : result)
		{
			CheckPolygonRings(polygon, polygon.holeCount());
			holes += polygon.holeCount();
			area += polygon.area();
		}
		CHECK(holes == 1);
		CHECK(area == Test::Approx(361.0));
	}
}

TEST_CASE("Polygon.generated_rings.closed_line_string_duplicates")
{
	const LineString ring{ { 0, 0 }, { 20, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 }, { 0, 0 } };
	const LineString original = ring;
	const Polygon miter = ring.computeMiterBufferPolygon(2.0, CloseRing::Yes);
	CheckPolygonRings(miter, 1);
	CHECK(miter.area() == Test::Approx(320.0));
	CheckPolygonRings(ring.computeRoundBufferPolygon(2.0, CloseRing::Yes), 1);
	CHECK(ring == original);
}

TEST_CASE("Geometry2D.ComposePolygons.empty_and_short_rings")
{
	CHECK(Geometry2D::ComposePolygons({}).isEmpty());
	CHECK(Geometry2D::ComposePolygons({ {}, { { 1, 1 } }, { { 0, 0 }, { 2, 2 } } }).isEmpty());
	const Polygon reference = MakePolygonWithHole();
	const LineString outer{ reference.outer() };
	const LineString hole{ reference.inners().front() };
	CHECK(Geometry2D::ComposePolygons({ hole }).isEmpty());
	const auto result = Geometry2D::ComposePolygons({ {}, outer, { { 1, 1 } }, hole });
	REQUIRE(result.size() == 1);
	CheckPolygonRings(result.front(), 1);
	CHECK(result.front().outer() == reference.outer());
	CHECK(result.front().inners() == reference.inners());
}

TEST_CASE("Geometry2D.ComposePolygons.nested_components")
{
	const LineString outerA{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } };
	const LineString outerB{ { 30, 30 }, { 40, 30 }, { 40, 40 }, { 30, 40 } };
	const LineString island{ { 9, 9 }, { 11, 9 }, { 11, 11 }, { 9, 11 } };
	const LineString holeA1{ { 2, 2 }, { 2, 4 }, { 4, 4 }, { 4, 2 } };
	const LineString holeA2{ { 8, 8 }, { 8, 12 }, { 12, 12 }, { 12, 8 } };
	const LineString holeB{ { 32, 32 }, { 32, 35 }, { 35, 35 }, { 35, 32 } };
	const LineString islandHole{ { 9.5, 9.5 }, { 9.5, 10.5 }, { 10.5, 10.5 }, { 10.5, 9.5 } };
	const Array<LineString> rings{ islandHole, holeA1, outerB, outerA, holeB, island, holeA2 };
	const Array<LineString> original = rings;
	const auto result = Geometry2D::ComposePolygons(rings);
	REQUIRE(result.size() == 3);
	const Array<Polygon> expected{
		Polygon{ outerB, { holeB.asArray() } },
		Polygon{ outerA, { holeA1.asArray(), holeA2.asArray() } },
		Polygon{ island, { islandHole.asArray() } },
	};
	for (size_t i = 0; i < expected.size(); ++i)
	{
		CAPTURE(i);
		CheckPolygonRings(result[i], expected[i].holeCount());
		CHECK(result[i].outer() == expected[i].outer());
		CHECK(result[i].inners() == expected[i].inners());
		CHECK(result[i].area() == expected[i].area());
	}
	CHECK(rings == original);
}

TEST_CASE("Geometry2D.ComposePolygons.concave_outer")
{
	const LineString outer{ { 0, 0 }, { 12, 0 }, { 12, 4 }, { 8, 4 }, { 8, 12 }, { 0, 12 } };
	const LineString hole{ { 1, 7 }, { 1, 10 }, { 4, 10 }, { 4, 7 } };
	const LineString outside{ { 9, 7 }, { 9, 10 }, { 11, 10 }, { 11, 7 } };
	const auto result = Geometry2D::ComposePolygons({ outside, hole, outer });
	REQUIRE(result.size() == 1);
	CheckPolygonRings(result.front(), 1);
	CHECK(result.front().area() == Test::Approx(103.0));
	CHECK(result.front().inners().front() == hole.asArray());
}

TEST_CASE("Geometry2D.ComposePolygons.many_holes")
{
	Array<LineString> rings{ { { 0, 0 }, { 100, 0 }, { 100, 100 }, { 0, 100 } } };
	for (int32 y = 0; y < 8; ++y)
	{
		for (int32 x = 0; x < 8; ++x)
		{
			const Vec2 p{ 2.0 + (x * 10.0), 2.0 + (y * 10.0) };
			rings.push_back(LineString{ p, p + Vec2{ 0, 2 }, p + Vec2{ 2, 2 }, p + Vec2{ 2, 0 } });
		}
	}
	const auto result = Geometry2D::ComposePolygons(rings);
	REQUIRE(result.size() == 1);
	CheckPolygonRings(result.front(), 64);
	CHECK(result.front().area() == Test::Approx(9744.0));
}

TEST_CASE("Geometry2D.ComposePolygons.invalid_holes_best_effort")
{
	const LineString outer{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } };
	const LineString first{ { 2, 2 }, { 2, 6 }, { 6, 6 }, { 6, 2 } };
	const LineString overlapping{ { 4, 4 }, { 4, 8 }, { 8, 8 }, { 8, 4 } };
	const LineString crossingOuter{ { 18, 2 }, { 18, 6 }, { 22, 6 }, { 22, 2 } };
	const LineString last{ { 12, 12 }, { 12, 16 }, { 16, 16 }, { 16, 12 } };
	const auto result = Geometry2D::ComposePolygons({ outer, first, overlapping, crossingOuter, last });
	REQUIRE(result.size() == 1);
	CheckPolygonRings(result.front(), 2);
	CHECK(result.front().inners() == Array<Array<Vec2>>{ first.asArray(), last.asArray() });
	CHECK(result.front().area() == Test::Approx(368.0));
}

TEST_CASE("Polygon.Correct.orientation_and_holes")
{
	const Polygon reference = MakePolygonWithHole();
	for (const bool reverseOuter : { false, true })
	{
		for (const bool reverseHole : { false, true })
		{
			CAPTURE(reverseOuter, reverseHole);
			const auto outer = reverseOuter ? reference.outer().reversed() : reference.outer();
			auto holes = reference.inners();
			if (reverseHole)
			{
				holes.front().reverse();
			}
			const auto results = Polygon::Correct(outer, holes);
			REQUIRE(results.size() == 1);
			CheckPolygonRings(results.front(), 1);
			CHECK(results.front().area() == reference.area());
			const Polygon one = Polygon::CorrectOne(outer, holes);
			CHECK(one.outer() == results.front().outer());
			CHECK(one.inners() == results.front().inners());
			CheckTriangleIndices(one, results.front());
		}
	}
}

TEST_CASE("Polygon.Correct.degenerate")
{
	const Array<Array<Vec2>> cases{
		{}, { { 1, 2 } }, { { 0, 0 }, { 4, 4 } },
		{ { 0, 0 }, { 2, 2 }, { 4, 4 } },
		{ { 1, 2 }, { 1, 2 }, { 1, 2 } },
	};
	for (const auto& outer : cases)
	{
		CHECK(Polygon::Correct(outer).isEmpty());
		CHECK(Polygon::CorrectOne(outer).isEmpty());
	}
}

TEST_CASE("Polygon.CorrectOne.multiple_components")
{
	const Array<Array<Vec2>> cases{
		{ { 0, 0 }, { 6, 6 }, { 0, 6 }, { 2, 0 } },
		{ { 0, 0 }, { 4, 4 }, { 0, 4 }, { 4, 0 } },
	};
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		for (size_t i = 0; i < cases.size(); ++i)
		{
			CAPTURE(i, offset);
			const auto outer = cases[i].map([offset](const Vec2& point) { return (point + offset); });
			const auto results = Polygon::Correct(outer);
			REQUIRE(results.size() == 2);
			const Polygon* largest = &results.front();
			for (const auto& polygon : results)
			{
				CheckPolygonRings(polygon, 0);
				if (largest->area() < polygon.area())
				{
					largest = &polygon;
				}
			}
			const Polygon one = Polygon::CorrectOne(outer);
			CHECK(one.outer() == largest->outer());
			CHECK(one.inners() == largest->inners());
			CHECK(one.area() == largest->area());
			CheckTriangleIndices(one, *largest);
		}
	}
}

TEST_CASE("Geometry2D.ComposePolygons.repaired_outer")
{
	const LineString outer{ { 0, 0 }, { 6, 6 }, { 0, 6 }, { 2, 0 } };
	const LineString hole{ { 1.5, 4 }, { 1.5, 5 }, { 2.5, 5 }, { 2.5, 4 } };
	const auto results = Geometry2D::ComposePolygons({ hole, outer });
	REQUIRE(results.size() == 2);
	size_t holeCount = 0;
	double area = 0.0;
	for (const auto& polygon : results)
	{
		CheckPolygonRings(polygon, polygon.holeCount());
		holeCount += polygon.holeCount();
		area += polygon.area();
	}
	CHECK(holeCount == 1);
	CHECK(area == Test::Approx(14.0));
}

TEST_CASE("Polygon.Correct.closed_rings")
{
	const Polygon reference = MakePolygonWithHole();
	auto outer = reference.outer();
	auto holes = reference.inners();
	outer.push_back(outer.front());
	holes.front().push_back(holes.front().front());
	const auto results = Polygon::Correct(outer, holes);
	REQUIRE(results.size() == 1);
	CheckPolygonRings(results.front(), 1);
	CHECK(results.front().area() == reference.area());
}

TEST_CASE("Polygon.Validate.failure_classification")
{
	const Polygon reference = MakePolygonWithHole();
	CHECK(Polygon::Validate({}) == PolygonFailureType::FewPoints);
	CHECK(Polygon::Validate(reference.outer().reversed()) == PolygonFailureType::WrongOrientation);
	auto outer = reference.outer();
	outer.push_back(outer.front());
	CHECK(Polygon::Validate(outer) == PolygonFailureType::DuplicatePoints);
	CHECK(Polygon::Validate(reference.outer(), { { { 1, 1 }, { 2, 2 } } }) == PolygonFailureType::FewPoints);
	auto holes = reference.inners();
	holes.front().push_back(holes.front().front());
	CHECK(Polygon::Validate(reference.outer(), holes) == PolygonFailureType::DuplicatePoints);
}

TEST_CASE("Geometry2D.ComposePolygons.font_glyphs")
{
	const Font font{ 40 };
	REQUIRE(font);
	for (const char32 ch : StringView{ U"B8@日田語" })
	{
		CAPTURE(static_cast<uint32>(ch));
		const auto glyph = font.generatePolygonGlyph(ch);
		REQUIRE(not glyph.polygons.isEmpty());
		for (const auto& polygon : glyph.polygons)
		{
			CheckPolygonRings(polygon, polygon.holeCount());
		}
	}
}

TEST_CASE("Geometry2D.ComposePolygons.vertex_precision")
{
	const Vec2 offset{ 134217728, 134217728 };
	const LineString outer = LineString{ { 0, 0 }, { 7, 0 }, { 7, 7 }, { 0, 7 } }.movedBy(offset);
	const LineString hole = LineString{ { 1, 1 }, { 1, 2 }, { 2, 2 }, { 2, 1 } }.movedBy(offset);
	const auto result = Geometry2D::ComposePolygons({ hole, outer });
	REQUIRE(result.size() == 1);
	CheckPolygonRings(result.front(), 1);
	// Hole assignment uses Vec2 contours even when Float2 rendering vertices collapse.
	CHECK(result.front().outer() == outer.asArray());
	CHECK(result.front().inners().front() == hole.asArray());
	CHECK(result.front().boundingRect() == RectF{ offset, Vec2{ 7, 7 } });
}

TEST_CASE("Geometry2D.ComposePolygons.touching_hole")
{
	const LineString outer{ { 0, 0 }, { 7, 0 }, { 7, 7 }, { 0, 7 } };
	const LineString hole{ { 0, 3 }, { 2, 5 }, { 2, 1 } };
	const auto result = Geometry2D::ComposePolygons({ outer, hole });
	REQUIRE(result.size() == 1);
	CheckPolygonRings(result.front(), 1);
	CHECK(result.front().area() == Test::Approx(45.0));
}
