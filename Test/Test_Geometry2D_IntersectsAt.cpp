//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/IntersectsAt.hpp>

namespace
{
	[[nodiscard]]
	bool NearPoint(const Vec2& a, const Vec2& b, const double tolerance = 1.0e-7)
	{
		return (a.distanceFromSq(b) <= (tolerance * tolerance));
	}

	[[nodiscard]]
	bool ContainsNear(const Array<Vec2>& points, const Vec2& expected, const double tolerance = 1.0e-7)
	{
		for (const Vec2& point : points)
		{
			if (NearPoint(point, expected, tolerance))
			{
				return true;
			}
		}

		return false;
	}

	void CheckPointSet(const Optional<Array<Vec2>>& actual,
		const std::initializer_list<Vec2> expected, const double tolerance = 1.0e-7)
	{
		REQUIRE(actual.has_value());
		CHECK(actual->size() == expected.size());

		for (const Vec2& point : expected)
		{
			CHECK(ContainsNear(*actual, point, tolerance));
		}
	}
}

TEST_CASE("Geometry2D.IntersectsAt.OptionalSemantics")
{
	CHECK(not Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 1, 0 } },
		Line{ Vec2{ 0, 1 }, Vec2{ 1, 1 } }).has_value());

	const auto contained = Geometry2D::IntersectsAt(
		Line{ Vec2{ 2, 5 }, Vec2{ 8, 5 } },
		RectF{ 0, 0, 10, 10 });
	REQUIRE(contained.has_value());
	CHECK(contained->empty());

	CHECK(not Geometry2D::IntersectsAt(
		Circle{ Vec2{ 1, 1 }, 0 }, Vec2{ 1, 1 }).has_value());

	CHECK(not Geometry2D::IntersectsAt(
		LineString{}, RectF{ 0, 0, 10, 10 }).has_value());
}

TEST_CASE("Geometry2D.IntersectsAt.PointGeometry")
{
	CheckPointSet(Geometry2D::IntersectsAt(Vec2{ 5, 5 }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });
	CheckPointSet(Geometry2D::IntersectsAt(Line{ Vec2{ 5, 5 }, Vec2{ 5, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });
	CheckPointSet(Geometry2D::IntersectsAt(
		Triangle{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } },
		Circle{ Vec2{ 5, 5 }, 2 }),
		{ Vec2{ 5, 5 } });
	CheckPointSet(Geometry2D::IntersectsAt(
		Bezier2{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } },
		RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		LineString{ Vec2{ 5, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });
}

TEST_CASE("Geometry2D.IntersectsAt.LineLine")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } },
		Line{ Vec2{ 5, -5 }, Vec2{ 5, 5 } }),
		{ Vec2{ 5, 0 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } },
		Line{ Vec2{ 10, 0 }, Vec2{ 20, 0 } }),
		{ Vec2{ 10, 0 } });

	const auto overlap = Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } },
		Line{ Vec2{ 5, 0 }, Vec2{ 15, 0 } });
	REQUIRE(overlap.has_value());
	CHECK(overlap->empty());

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 5, 0 }, Vec2{ 5, 0 } },
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } }),
		{ Vec2{ 5, 0 } });
}

TEST_CASE("Geometry2D.IntersectsAt.RectF")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 5, 5, 10, 10 }),
		{ Vec2{ 10, 5 }, Vec2{ 5, 10 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 10, 10, 5, 5 }),
		{ Vec2{ 10, 10 } });

	const auto sharedEdge = Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 10, 0, 5, 10 });
	REQUIRE(sharedEdge.has_value());
	CHECK(sharedEdge->empty());

	const auto identical = Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 0, 0, 10, 10 });
	REQUIRE(identical.has_value());
	CHECK(identical->empty());

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -5, 5 }, Vec2{ 15, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 0, 5 }, Vec2{ 10, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -5, 5 }, Vec2{ 5, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 0, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		RectF{ 5, 0, 0, 10 }, RectF{ 0, 5, 10, 0 }),
		{ Vec2{ 5, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Ellipse{ Vec2{ 5, 5 }, 0, 5 }, RectF{ 0, 5, 10, 0 }),
		{ Vec2{ 5, 5 } });
}

TEST_CASE("Geometry2D.IntersectsAt.Circle")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 10, 0 }, 5 }),
		{ Vec2{ 5, 0 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 6, 0 }, 5 }),
		{ Vec2{ 3, 4 }, Vec2{ 3, -4 } });

	const auto contained = Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 0, 0 }, 2 });
	REQUIRE(contained.has_value());
	CHECK(contained->empty());

	const auto identical = Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 0, 0 }, 5 });
	REQUIRE(identical.has_value());
	CHECK(identical->empty());
}

TEST_CASE("Geometry2D.IntersectsAt.PolygonalBoundaries")
{
	const Polygon square{ Array<Vec2>{
		Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }, square),
		{ Vec2{ 0, 5 }, Vec2{ 10, 5 } });

	const Polygon donut{
		Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } },
		Array<Array<Vec2>>{ Array<Vec2>{
			Vec2{ 3, 3 }, Vec2{ 3, 7 }, Vec2{ 7, 7 }, Vec2{ 7, 3 } } }
	};

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }, donut),
		{ Vec2{ 0, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 10, 5 } });

	const Polygon touchingA{ Array<Vec2>{
		Vec2{ 0, 0 }, Vec2{ 2, 0 }, Vec2{ 2, 2 }, Vec2{ 0, 2 } } };
	const Polygon touchingB{ Array<Vec2>{
		Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const MultiPolygon multi{ touchingA, touchingB };

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 4 }, Vec2{ 4, 0 } }, multi),
		{ Vec2{ 2, 2 } });
}

TEST_CASE("Geometry2D.IntersectsAt.LineStringDeduplication")
{
	const LineString lineString{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } };
	CheckPointSet(Geometry2D::IntersectsAt(
		lineString, Line{ Vec2{ 0, -1 }, Vec2{ 0, 1 } }),
		{ Vec2{ 0, 0 } });

	const auto identical = Geometry2D::IntersectsAt(lineString, lineString);
	REQUIRE(identical.has_value());
	CHECK(identical->empty());
}

TEST_CASE("Geometry2D.IntersectsAt.CurvedBoundaries")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }, Ellipse{ Vec2{ 0, 0 }, 5, 3 }),
		{ Vec2{ -5, 0 }, Vec2{ 5, 0 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } },
		SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 }),
		{ Vec2{ -5, 0 }, Vec2{ 5, 0 } }, 1.0e-6);

	const double dx = (2.0 - std::sqrt(3.0));
	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -1, 1 }, Vec2{ 11, 1 } },
		RoundRect{ RectF{ 0, 0, 10, 10 }, 2 }),
		{ Vec2{ dx, 1 }, Vec2{ 10.0 - dx, 1 } }, 1.0e-6);

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 5 }, Vec2{ 10, 5 } },
		Bezier2{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } }),
		{ Vec2{ 5, 5 } }, 1.0e-6);

	const auto bezierBoundaryOverlap = Geometry2D::IntersectsAt(
		Bezier2{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } },
		RectF{ 0, 0, 10, 10 });
	REQUIRE(bezierBoundaryOverlap.has_value());
	CHECK(bezierBoundaryOverlap->empty());

	const auto identicalRoundRect = Geometry2D::IntersectsAt(
		RoundRect{ RectF{ 0, 0, 10, 10 }, 2 },
		RoundRect{ RectF{ 0, 0, 10, 10 }, 2 });
	REQUIRE(identicalRoundRect.has_value());
	CHECK(identicalRoundRect->empty());

	const auto equivalentCurves = Geometry2D::IntersectsAt(
		Ellipse{ Vec2{ 0, 0 }, 5, 3 },
		SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 2.0 });
	REQUIRE(equivalentCurves.has_value());
	CHECK(equivalentCurves->empty());
}
