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
