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
