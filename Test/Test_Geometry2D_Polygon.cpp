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
