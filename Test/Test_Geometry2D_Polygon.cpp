//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Distance.hpp>

namespace
{
	Polygon MakeDonut()
	{
		return Polygon{
			Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
			Array<Array<Vec2>>{ { { 6, 6 }, { 6, 14 }, { 14, 14 }, { 14, 6 } } }
		};
	}

	template <class Shape>
	void CheckIntersection(const Polygon& polygon, const Shape& shape, const bool expected)
	{
		CHECK(Geometry2D::Intersects(polygon, shape) == expected);
		CHECK(Geometry2D::Intersects(shape, polygon) == expected);
		CHECK(polygon.intersects(shape) == expected);
	}

	template <class ShapeA, class ShapeB>
	void CheckDistance(const ShapeA& a, const ShapeB& b, const double expected)
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
		CHECK(ab->pointA.distanceFrom(ab->pointB) == Test::Approx(expected));
	}
}

TEST_CASE("Geometry2D.Polygon.point_containment")
{
	const Polygon donut = MakeDonut();
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
	{
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			const Polygon polygon = donut.scaledFromOrigin(scale).movedBy(offset);
			for (int32 iy = -2; iy <= 42; ++iy)
			{
				for (int32 ix = -2; ix <= 42; ++ix)
				{
					const double x = (ix * 0.5), y = (iy * 0.5);
					const bool expected = InRange(x, 0.0, 20.0) && InRange(y, 0.0, 20.0)
						&& not ((6.0 < x) && (x < 14.0) && (6.0 < y) && (y < 14.0));
					const Vec2 point = (Vec2{ x, y } * scale + offset);
					CAPTURE(point, scale, offset);
					CHECK(Geometry2D::Intersects(point, polygon) == expected);
					CHECK(Geometry2D::Contains(polygon, point) == expected);
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Polygon.area_shapes")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		const Polygon polygon = MakeDonut().movedBy(offset);
		for (const Vec2 localCenter : { Vec2{ 3, 3 }, Vec2{ 10, 10 }, Vec2{ 22, 10 }, Vec2{ 6, 10 } })
		{
			const bool expected = ((localCenter.x == 3) || (localCenter.x == 6));
			const Vec2 center = (localCenter + offset);
			CAPTURE(center);
			const RectF rect{ (center - Vec2{ 0.5, 0.5 }), 1, 1 };
			CheckIntersection(polygon, rect, expected);
			CheckIntersection(polygon, Circle{ center, 0.5 }, expected);
			CheckIntersection(polygon, Ellipse{ center, 0.5, 0.25 }, expected);
			CheckIntersection(polygon, SuperEllipse{ center, SizeF{ 0.5, 0.25 }, 4.0 }, expected);
			CheckIntersection(polygon, SuperEllipse{ center, SizeF{ 0.5, 0.25 }, 0.5 }, expected);
			CheckIntersection(polygon, Triangle{ (center + Vec2{ -0.5, -0.5 }), (center + Vec2{ 0.5, -0.5 }), (center + Vec2{ 0, 0.5 }) }, expected);
			CheckIntersection(polygon, rect.asQuad(), expected);
			CheckIntersection(polygon, RoundRect{ rect, 0.25 }, expected);
			CheckIntersection(polygon, rect.asPolygon(), expected);
		}

		// Each shape contains the entire polygon, so checking its own center is insufficient.
		const Vec2 center = (offset + Vec2{ 10, 10 });
		CheckIntersection(polygon, Circle{ center, 32 }, true);
		CheckIntersection(polygon, Ellipse{ center, 32, 24 }, true);
		CheckIntersection(polygon, SuperEllipse{ center, SizeF{ 32, 24 }, 4.0 }, true);
		const RectF large{ (offset - Vec2{ 2, 2 }), 24, 24 };
		CheckIntersection(polygon, large, true);
		CheckIntersection(polygon, large.asQuad(), true);
		CheckIntersection(polygon, RoundRect{ large, 1 }, true);
		CheckIntersection(polygon, large.asPolygon(), true);
		CheckIntersection(polygon, Triangle{ (offset - Vec2{ 10, 10 }), (offset + Vec2{ 60, -10 }), (offset + Vec2{ -10, 60 }) }, true);
		CheckIntersection(polygon, Circle{ (offset + Vec2{ 10, 10 }), 4 }, true);
		CheckIntersection(polygon, Circle{ (offset + Vec2{ 10, 10 }), 3.5 }, false);
	}
}

TEST_CASE("Geometry2D.Polygon.lines_and_curves")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		const Polygon polygon = MakeDonut().movedBy(offset);
		const auto P = [offset](double x, double y) { return (offset + Vec2{ x, y }); };
		CheckIntersection(polygon, Line{ P(-2, 10), P(22, 10) }, true);
		CheckIntersection(polygon, Line{ P(7, 10), P(13, 10) }, false);
		CheckIntersection(polygon, Line{ P(6, 7), P(6, 13) }, true);
		CheckIntersection(polygon, Line{ P(6, 10), P(6, 10) }, true);
		CheckIntersection(polygon, LineString{ P(7, 7), P(7, 7), P(13, 7), P(13, 13) }, false);
		CheckIntersection(polygon, LineString{ P(7, 7), P(13, 7), P(13, 15) }, true);
		CheckIntersection(polygon, Bezier2{ P(7, 10), P(10, 8), P(13, 10) }, false);
		CheckIntersection(polygon, Bezier2{ P(-2, 10), P(10, 8), P(22, 10) }, true);
		CheckIntersection(polygon, Bezier3{ P(7, 10), P(8, 8), P(12, 8), P(13, 10) }, false);
		CheckIntersection(polygon, Bezier3{ P(-2, 10), P(8, 8), P(12, 8), P(22, 10) }, true);
	}
}

TEST_CASE("Geometry2D.Polygon.concavity_and_holes")
{
	const Polygon concave{ Array<Vec2>{ { 0, 0 }, { 12, 0 }, { 12, 12 }, { 8, 12 }, { 8, 4 }, { 4, 4 }, { 4, 12 }, { 0, 12 } } };
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		const Polygon polygon = concave.movedBy(offset);
		CheckIntersection(polygon, RectF{ (offset + Vec2{ 5, 6 }), 2, 2 }.asPolygon(), false);
		CheckIntersection(polygon, RectF{ (offset + Vec2{ 2, 6 }), 8, 2 }.asPolygon(), true);
		CheckIntersection(polygon, Line{ (offset + Vec2{ 4, 10 }), (offset + Vec2{ 8, 10 }) }, true);
		CheckIntersection(polygon, Vec2{ 6, 10 } + offset, false);
	}
	const Polygon donut = MakeDonut();
	CheckIntersection(donut, RectF{ 6, 6, 8, 8 }.asPolygon(), true);
	CheckIntersection(donut, RectF{ 7, 7, 6, 6 }.asPolygon(), false);
	CheckIntersection(donut, donut, true);
	const MultiPolygon multi{ Polygon{}, RectF{ 30, 30, 2, 2 }.asPolygon(), donut, Polygon{} };
	CheckIntersection(concave, multi, true);
	CheckIntersection(Polygon{}, multi, false);
	CheckIntersection(donut, MultiPolygon{ Polygon{} }, false);
}

TEST_CASE("Geometry2D.Polygon.collapsed")
{
	const Polygon donut = MakeDonut();
	for (const Vec2 scale : { Vec2{ 0, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 0 } })
	{
		const Polygon collapsed = donut.scaledFromOrigin(scale);
		const Vec2 point = (Vec2{ 10, 10 } * scale);
		CheckIntersection(collapsed, point, true);
		CHECK(collapsed.contains(collapsed));
		CHECK(collapsed.contains(Triangle{ point, point, point }));
		CHECK_FALSE(RectF{ point, 0, 0 }.contains(collapsed));
		CheckIntersection(collapsed, point + Vec2{ 1, 1 }, false);
		CheckIntersection(collapsed, Circle{ point, 0.5 }, true);
		CheckIntersection(collapsed, Line{ (point - Vec2{ 1, 1 }), (point + Vec2{ 1, 1 }) }, true);
	}
}

TEST_CASE("Geometry2D.Polygon.supplied_mesh")
{
	const Polygon original = MakeDonut();
	Array<Float2> vertices = original.vertices();
	std::reverse(vertices.begin(), vertices.end());
	Array<TriangleIndex> indices = original.indices();
	const auto last = static_cast<Vertex2D::IndexType>(vertices.size() - 1);
	for (auto& index : indices)
	{
		index = {
			static_cast<TriangleIndex::value_type>(last - index.i0),
			static_cast<TriangleIndex::value_type>(last - index.i1),
			static_cast<TriangleIndex::value_type>(last - index.i2)
		};
	}
	const Polygon polygon{ original.outer(), original.inners(), vertices, indices, original.boundingRect() };
	CheckIntersection(polygon, Vec2{ 2, 2 }, true);
	CheckIntersection(polygon, Vec2{ 10, 10 }, false);
	CheckIntersection(polygon, RectF{ 7, 7, 1, 1 }, false);
	CheckIntersection(polygon, Line{ Vec2{ -1, 10 }, Vec2{ 21, 10 } }, true);
	CHECK(polygon.contains(original));
	CHECK(polygon.contains(RectF{ 1, 1, 2, 2 }));
	CHECK_FALSE(polygon.contains(Line{ Vec2{ 1, 10 }, Vec2{ 19, 10 } }));
	CHECK_FALSE(polygon.contains(RectF{ 0, 0, 20, 20 }.asPolygon()));
}

TEST_CASE("Geometry2D.Polygon.distance_views")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		const Polygon polygon = MakeDonut().movedBy(offset);
		const auto P = [offset](double x, double y) { return (offset + Vec2{ x, y }); };
		CheckDistance(polygon, P(10, 10), 4.0);
		const auto tied = Geometry2D::ClosestPoints(polygon, P(10, 10));
		REQUIRE(tied);
		CHECK(tied->pointA == P(6, 10));
		CheckDistance(polygon, LineString{ P(9, 9), P(9, 9), P(11, 9) }, 3.0);
		CheckDistance(polygon, LineString{ P(9, 9), P(11, 9), P(11, 15) }, 0.0);
		CheckDistance(polygon, RectF{ P(30, 2), 2, 2 }.asPolygon(), 10.0);
		CheckDistance(polygon, Circle{ P(25, 10), 1 }, 4.0);
		const MultiPolygon multi{ Polygon{}, polygon, RectF{ P(30, 2), 2, 2 }.asPolygon(), Polygon{} };
		CheckDistance(multi, P(25, 3), 5.0);
		CheckDistance(multi, RectF{ P(7, 7), 2, 2 }.asPolygon(), 1.0);
		CheckDistance(multi, P(31, 3), 0.0);
		CheckDistance(LineString{ P(-2, 0), P(-2, 0), P(-2, 20) }, polygon, 2.0);
	}
}

TEST_CASE("Geometry2D.Polygon.scaled_bounds")
{
	const Polygon source = MakeDonut().movedBy(Vec2{ 3, 4 });
	for (const Vec2 scale : { Vec2{ 2, 3 }, Vec2{ -2, 3 }, Vec2{ 2, -3 }, Vec2{ -2, -3 }, Vec2{ 0, 1 } })
	{
		for (const Vec2 center : { Vec2{ 0, 0 }, Vec2{ 7, 9 } })
		{
			const Polygon polygon = source.scaledFrom(center, scale);
			const RectF expected = Geometry2D::BoundingRect(polygon.outer());
			CHECK(polygon.boundingRect() == expected);
			CHECK(polygon.boundingRect().w >= 0.0);
			CHECK(polygon.boundingRect().h >= 0.0);
			const Vec2 point = (center + (Vec2{ 5, 6 } - center) * scale);
			CheckIntersection(polygon, point, true);
			CheckIntersection(polygon, Circle{ point, 0.25 }, true);
		}
	}
	for (const double scale : { 2.0, -2.0, 0.0 })
	{
		const Polygon fromOrigin = source.scaledFromOrigin(scale);
		const Polygon fromCenter = source.scaledFrom(Vec2{ 7, 9 }, scale);
		CHECK(fromOrigin.boundingRect() == Geometry2D::BoundingRect(fromOrigin.outer()));
		CHECK(fromCenter.boundingRect() == Geometry2D::BoundingRect(fromCenter.outer()));
	}
}

TEST_CASE("Geometry2D.Polygon.contains_segments")
{
	const Polygon donut = MakeDonut();
	struct Case { Line line; bool expected; };
	const Array<Case> cases{
		{ { { 1, 1 }, { 19, 1 } }, true },
		{ { { 0, 0 }, { 20, 0 } }, true },
		{ { { 6, 6 }, { 14, 6 } }, true },
		{ { { 0, 6 }, { 20, 6 } }, true },
		{ { { 0, 0 }, { 6, 6 } }, true },
		{ { { 0, 12 }, { 12, 0 } }, true }, // touches one hole vertex
		{ { { 6, 6 }, { 14, 14 } }, false },
		{ { { 6, 10 }, { 14, 10 } }, false },
		{ { { 1, 10 }, { 19, 10 } }, false },
		{ { { 1, 1 }, { 19, 19 } }, false },
		{ { { 7, 7 }, { 13, 13 } }, false },
		{ { { 6, 10 }, { 6, 10 } }, true },
		{ { { 10, 10 }, { 10, 10 } }, false },
		{ { { -1, 0 }, { 21, 0 } }, false },
	};
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
	{
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			const Polygon polygon = donut.scaledFromOrigin(scale).movedBy(offset);
			for (size_t i = 0; i < cases.size(); ++i)
			{
				CAPTURE(i, offset, scale);
				const Line line{ (cases[i].line.start * scale + offset), (cases[i].line.end * scale + offset) };
				CHECK(Geometry2D::Contains(polygon, line) == cases[i].expected);
				CHECK(Geometry2D::Contains(polygon, Line{ line.end, line.start }) == cases[i].expected);
				CHECK(polygon.contains(LineString{ line.start, line.start, line.end }) == cases[i].expected);
			}
		}
	}
	CHECK_FALSE(donut.contains(LineString{}));
	CHECK_FALSE(donut.contains(RectF{ 2, 2, 0, 0 }));
	CHECK(donut.contains(Rect{ 0, 6, 20, 0 }));
	CHECK_FALSE(donut.contains(RectF{ 0, 10, 20, 0 }));
	CHECK(donut.contains(LineString{ Vec2{ 2, 2 } }));
	CHECK_FALSE(Polygon{}.contains(Line{ Vec2{ 1, 1 }, Vec2{ 1, 1 } }));
	CHECK(donut.contains(Triangle{ Vec2{ 0, 6 }, Vec2{ 10, 6 }, Vec2{ 20, 6 } }));
	CHECK_FALSE(donut.contains(Triangle{ Vec2{ 0, 10 }, Vec2{ 10, 10 }, Vec2{ 20, 10 } }));
	CHECK(donut.contains(Triangle{ Vec2{ 6, 6 }, Vec2{ 6, 6 }, Vec2{ 6, 6 } }));
	CHECK_FALSE(donut.contains(Triangle{ Vec2{ 10, 10 }, Vec2{ 10, 10 }, Vec2{ 10, 10 } }));
}

TEST_CASE("Geometry2D.Polygon.contains_tangencies_and_collinear_edges")
{
	const Polygon comb{ Array<Vec2>{
		{ 0, 0 }, { 20, 0 }, { 20, 8 }, { 16, 8 }, { 16, 4 }, { 14, 4 },
		{ 14, 8 }, { 12, 8 }, { 12, 4 }, { 10, 4 }, { 10, 8 }, { 8, 8 },
		{ 8, 4 }, { 6, 4 }, { 6, 8 }, { 0, 8 }
	} };
	CHECK(comb.contains(Line{ Vec2{ 0, 4 }, Vec2{ 20, 4 } }));
	CHECK_FALSE(comb.contains(Line{ Vec2{ 0, 8 }, Vec2{ 20, 8 } }));
	CHECK_FALSE(comb.contains(Line{ Vec2{ 6, 8 }, Vec2{ 16, 8 } }));
	CHECK_FALSE(comb.contains(Line{ Vec2{ 1, 6 }, Vec2{ 19, 6 } }));
	const Polygon peaks{ Array<Vec2>{ { 0, 0 }, { 1, 1 }, { 2, 0 }, { 3, 1 }, { 4, 0 }, { 4, 3 }, { 0, 3 } } };
	// Endpoints and the midpoint are on the boundary, but the open intervals are outside.
	CHECK_FALSE(peaks.contains(Line{ Vec2{ 0, 0 }, Vec2{ 4, 0 } }));
	CHECK(peaks.contains(Line{ Vec2{ 0, 1 }, Vec2{ 4, 1 } }));
	CHECK_FALSE(peaks.contains(Line{ Vec2{ 0, 0 }, Vec2{ 4, 0.5 } }));
}

TEST_CASE("Geometry2D.Polygon.contains_area_and_holes")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		const Polygon donut = MakeDonut().movedBy(offset);
		const auto P = [offset](double x, double y) { return (offset + Vec2{ x, y }); };
		CHECK(donut.contains(donut));
		CHECK(donut.contains(RectF{ P(1, 1), 4, 4 }));
		CHECK(donut.contains(Triangle{ P(0, 0), P(12, 0), P(0, 12) }));
		CHECK_FALSE(donut.contains(RectF{ P(0, 0), 20, 20 }));
		CHECK_FALSE(donut.contains(RectF{ P(6, 6), 8, 8 }));
		CHECK_FALSE(donut.contains(Triangle{ P(6, 6), P(14, 6), P(14, 14) }));
		CHECK_FALSE(donut.contains(RectF{ P(7, 7), 6, 6 }.asPolygon()));
		CHECK_FALSE(donut.contains(RectF{ P(6, 6), 8, 8 }.asPolygon()));
		CHECK(donut.contains(Circle{ P(3, 3), 1 }));
		CHECK_FALSE(donut.contains(Circle{ P(10, 10), 9 }));
		CHECK_FALSE(donut.contains(Polygon{}));
		const MultiPolygon multi{ Polygon{}, donut, Polygon{} };
		CHECK(Geometry2D::Contains(multi, donut));
		CHECK(donut.contains(multi));
		CHECK_FALSE(donut.contains(MultiPolygon{ Polygon{} }));
		CHECK(donut.scaledFrom(P(0, 0), Vec2{ -1, 1 }).contains(donut.scaledFrom(P(0, 0), Vec2{ -1, 1 })));
	}
	const Polygon donut = MakeDonut();
	const auto WithHole = [](double left, double top, double right, double bottom)
	{
		return Polygon{ Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
			Array<Array<Vec2>>{ { { left, top }, { left, bottom }, { right, bottom }, { right, top } } } };
	};
	CHECK(donut.contains(WithHole(5, 5, 15, 15)));
	CHECK_FALSE(donut.contains(WithHole(7, 7, 13, 13)));
	CHECK_FALSE(donut.contains(WithHole(6, 6, 13, 14)));
	CHECK_FALSE(donut.contains(WithHole(7, 6, 15, 14)));
	CHECK(donut.contains(WithHole(6, 5, 15, 14)));
	const Polygon triangleHole{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 5, 5 }, { 5, 15 }, { 15, 5 } } }
	};
	CHECK_FALSE(triangleHole.contains(Triangle{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 5, 15 } }));
	CHECK_FALSE(triangleHole.contains(Polygon{ Array<Vec2>{ { 5, 5 }, { 15, 5 }, { 5, 15 } } }));
}

TEST_CASE("Geometry2D.Polygon.contains_small_hole")
{
	constexpr double size = 0x1p-20;
	const Polygon polygon{
		Array<Vec2>{ { 0, 0 }, { 8, 0 }, { 8, 8 }, { 0, 8 } },
		Array<Array<Vec2>>{ { { 1, 1 }, { 1, 1 + size }, { 1 + size, 1 + size }, { 1 + size, 1 } } }
	};
	REQUIRE(polygon);
	CHECK_FALSE(polygon.contains(Triangle{ Vec2{ 0, 0 }, Vec2{ 8, 0 }, Vec2{ 0, 8 } }));
	CHECK_FALSE(polygon.contains(RectF{ 0, 0, 8, 8 }.asPolygon()));
	CHECK_FALSE(polygon.contains(Line{ Vec2{ 0, 0 }, Vec2{ 8, 8 } }));
	CHECK(polygon.contains(Line{ Vec2{ 0, 1 }, Vec2{ 8, 1 } }));
}

TEST_CASE("Geometry2D.Polygon.contains_convex_and_concave_containers")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 } })
	{
		const Polygon small = RectF{ offset + Vec2{ 1, 1 }, 1, 1 }.asPolygon();
		const Polygon large = RectF{ offset + Vec2{ 1, 1 }, 4, 4 }.asPolygon();
		const Vec2 center = (offset + Vec2{ 2, 2 });
		CHECK(Geometry2D::Contains(Circle{ center, 2 }, small));
		CHECK_FALSE(Geometry2D::Contains(Circle{ center, 2 }, large));
		CHECK(Geometry2D::Contains(Ellipse{ center, 2, 2 }, small));
		CHECK(Geometry2D::Contains(SuperEllipse{ center, SizeF{ 2, 2 }, 4 }, small));
		CHECK(Geometry2D::Contains(RoundRect{ RectF{ offset, 4, 4 }, 1 }, small));
		CHECK(Geometry2D::Contains(Triangle{ offset, offset + Vec2{ 8, 0 }, offset + Vec2{ 0, 8 } }, small));
		const SuperEllipse nonConvex{ offset, SizeF{ 4, 4 }, 0.5 };
		CHECK(Geometry2D::Contains(nonConvex, Polygon{ Array<Vec2>{ offset, offset + Vec2{ 0.5, 0 }, offset + Vec2{ 0, 0.5 } } }));
		CHECK_FALSE(Geometry2D::Contains(nonConvex, Polygon{ Array<Vec2>{ offset, offset + Vec2{ 3, 0 }, offset + Vec2{ 0, 3 } } }));
	}
	const Quad concave{ Vec2{ 0, 0 }, Vec2{ 3, 3 }, Vec2{ 6, 0 }, Vec2{ 3, 6 } };
	const Polygon bridge{ Array<Vec2>{ { 1, 1 }, { 5, 1 }, { 3, 5 } } };
	CHECK_FALSE(Geometry2D::Contains(concave, bridge));
	const Polygon container{ Array<Vec2>{ concave.p0, concave.p1, concave.p2, concave.p3 } };
	CHECK(container.contains(concave));
	CHECK_FALSE(container.contains(bridge));
}

TEST_CASE("Geometry2D.Polygon.intersects_at_views")
{
	const Polygon donut = MakeDonut();
	const auto crossing = Geometry2D::IntersectsAt(Line{ Vec2{ -1, 10 }, Vec2{ 21, 10 } }, donut);
	REQUIRE(crossing.has_value());
	const Array<Vec2> expected{ { 20, 10 }, { 0, 10 }, { 6, 10 }, { 14, 10 } };
	REQUIRE(crossing->size() == expected.size());
	for (size_t i = 0; i < expected.size(); ++i)
	{
		CHECK((*crossing)[i].distanceFrom(expected[i]) < 1e-12);
	}
	const auto reverse = Geometry2D::IntersectsAt(donut, Line{ Vec2{ 21, 10 }, Vec2{ -1, 10 } });
	REQUIRE(reverse.has_value());
	REQUIRE(reverse->size() == expected.size());
	for (size_t i = 0; i < expected.size(); ++i)
	{
		CHECK((*reverse)[i].distanceFrom(expected[i]) < 1e-12);
	}
	const auto duplicates = Geometry2D::IntersectsAt(LineString{ { -1, 10 }, { -1, 10 }, { 21, 10 }, { -1, 10 } }, donut);
	REQUIRE(duplicates.has_value());
	CHECK(*duplicates == *crossing);
	const auto sharedEdge = Geometry2D::IntersectsAt(Line{ Vec2{ 6, 6 }, Vec2{ 6, 14 } }, donut);
	REQUIRE(sharedEdge.has_value());
	CHECK(sharedEdge->isEmpty());
	CHECK(not Geometry2D::IntersectsAt(Line{ Vec2{ 7, 7 }, Vec2{ 13, 13 } }, donut));
	const MultiPolygon multi{ Polygon{}, donut, donut.movedBy(30, 0), Polygon{} };
	const auto across = Geometry2D::IntersectsAt(Line{ Vec2{ -1, 10 }, Vec2{ 51, 10 } }, multi);
	REQUIRE(across.has_value());
	CHECK(across->size() == 8);
	CHECK(not Geometry2D::IntersectsAt(Polygon{}, multi));
}

TEST_CASE("Geometry2D.Polygon.contains_affine_rectangles")
{
	// The rectangle oracle remains valid after each invertible affine map.
	constexpr std::array<double, 8> coordinates{ -1, 0, 5, 6, 10, 14, 20, 21 };
	for (const int32 transform : { 0, 1, 2 })
	{
		const auto P = [transform](double x, double y)
		{
			if (transform == 0)
			{
				return Vec2{ x, y };
			}
			const Vec2 point{ (2 * x + y), (x + 3 * y) };
			return Vec2{ 134217728, -134217728 } + point * Vec2{ (transform == 1 ? 1 : -1), 1 };
		};
		// Keep reflected ring order, as Polygon::scaledFromOrigin() does.
		const Polygon container{
			Array<Vec2>{ P(0, 0), P(20, 0), P(20, 20), P(0, 20) },
			Array<Array<Vec2>>{ { P(6, 6), P(6, 14), P(14, 14), P(14, 6) } },
			SkipValidation::Yes
		};
		REQUIRE(container);
		for (size_t left = 0; left < coordinates.size(); ++left)
		{
			for (size_t right = left + 1; right < coordinates.size(); ++right)
			{
				for (size_t top = 0; top < coordinates.size(); ++top)
				{
					for (size_t bottom = top + 1; bottom < coordinates.size(); ++bottom)
					{
						const double l = coordinates[left], r = coordinates[right];
						const double t = coordinates[top], b = coordinates[bottom];
						const bool expected = (0 <= l) && (r <= 20) && (0 <= t) && (b <= 20)
							&& ((r <= 6) || (14 <= l) || (b <= 6) || (14 <= t));
						const Quad target{ P(l, t), P(r, t), P(r, b), P(l, b) };
						CAPTURE(transform, l, r, t, b);
						CHECK(container.contains(target) == expected);
						CHECK(container.contains(Polygon{ Array<Vec2>{ target.p0, target.p1, target.p2, target.p3 }, SkipValidation::Yes }) == expected);
					}
				}
			}
		}
	}
}
