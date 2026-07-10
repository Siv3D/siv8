//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Overlaps.hpp>

TEST_CASE("Geometry2D.Overlaps.Rect")
{
	CHECK(Geometry2D::Overlaps(Rect{ 0, 0, 10, 10 }, Rect{ 5, 5, 10, 10 }));
	CHECK(not Geometry2D::Overlaps(Rect{ 0, 0, 10, 10 }, Rect{ 10, 0, 5, 5 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 0, 0, 10, 10 }, RectF{ 10, 10, 5, 5 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 0, 0, 0, 0 }, RectF{ 0, 0, 10, 10 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, 0, 0, 10 }, RectF{ 0, 0, 10, 10 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 0, 5, 10, 0 }, RectF{ 0, 0, 10, 10 }));
}

TEST_CASE("Geometry2D.Overlaps.Circle")
{
	const Circle circle{ Vec2{ 0, 0 }, 5 };

	CHECK(Geometry2D::Overlaps(circle, Circle{ Vec2{ 9, 0 }, 5 }));
	CHECK(not Geometry2D::Overlaps(circle, Circle{ Vec2{ 10, 0 }, 5 }));
	CHECK(Geometry2D::Intersects(circle, Circle{ Vec2{ 10, 0 }, 5 }));
	CHECK(Geometry2D::Overlaps(circle, Circle{ Vec2{ 0, 0 }, 1 }));
	CHECK(not Geometry2D::Overlaps(circle, Circle{ Vec2{ 0, 0 }, 0 }));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 0, 0 }, 0 }, circle));

	CHECK(Geometry2D::Overlaps(RectF{ -2, -2, 4, 4 }, circle));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, -2, 4, 4 }, circle));
	CHECK(Geometry2D::Intersects(RectF{ 5, -2, 4, 4 }, circle));
}

TEST_CASE("Geometry2D.Overlaps.Triangle")
{
	const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
	const Triangle inside{ Vec2{ 1, 1 }, Vec2{ 4, 1 }, Vec2{ 1, 4 } };
	const Triangle sharedEdge{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 5, -5 } };
	const Triangle pointTouch{ Vec2{ 10, 0 }, Vec2{ 12, 0 }, Vec2{ 10, 2 } };
	const Triangle segment{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } };
	const Triangle point{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };

	CHECK(Geometry2D::Overlaps(triangle, inside));
	CHECK(Geometry2D::Overlaps(triangle, triangle));
	CHECK(not Geometry2D::Overlaps(triangle, sharedEdge));
	CHECK(not Geometry2D::Overlaps(triangle, pointTouch));
	CHECK(not Geometry2D::Overlaps(triangle, segment));
	CHECK(not Geometry2D::Overlaps(triangle, point));
	CHECK(Geometry2D::Intersects(triangle, sharedEdge));
	CHECK(Geometry2D::Intersects(triangle, pointTouch));
}

TEST_CASE("Geometry2D.Overlaps.Quad")
{
	const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
	const Quad overlapping{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } };
	const Quad sharedEdge{ Vec2{ 10, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 10 }, Vec2{ 10, 10 } };
	const Quad triangleCollapse{ Vec2{ 1, 1 }, Vec2{ 4, 1 }, Vec2{ 1, 4 }, Vec2{ 1, 4 } };
	const Quad segmentCollapse{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
	const Quad pointCollapse{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };

	CHECK(Geometry2D::Overlaps(quad, overlapping));
	CHECK(not Geometry2D::Overlaps(quad, sharedEdge));
	CHECK(Geometry2D::Overlaps(quad, triangleCollapse));
	CHECK(not Geometry2D::Overlaps(quad, segmentCollapse));
	CHECK(not Geometry2D::Overlaps(quad, pointCollapse));
}

TEST_CASE("Geometry2D.Overlaps.Polygon_MultiPolygon")
{
	const Polygon polygon{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };
	const Polygon overlapping{ Array<Vec2>{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } } };
	const Polygon sharedEdge{ Array<Vec2>{ Vec2{ 10, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 10 }, Vec2{ 10, 10 } } };
	const Polygon inside{ Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const Polygon empty;
	const MultiPolygon multiPolygon{ polygon };
	const MultiPolygon overlappingMultiPolygon{ overlapping };
	const MultiPolygon sharedEdgeMultiPolygon{ sharedEdge };
	const MultiPolygon emptyMultiPolygon;

	CHECK(Geometry2D::Overlaps(polygon, overlapping));
	CHECK(not Geometry2D::Overlaps(polygon, sharedEdge));
	CHECK(Geometry2D::Overlaps(polygon, inside));
	CHECK(not Geometry2D::Overlaps(polygon, empty));
	CHECK(Geometry2D::Overlaps(multiPolygon, overlappingMultiPolygon));
	CHECK(not Geometry2D::Overlaps(multiPolygon, sharedEdgeMultiPolygon));
	CHECK(not Geometry2D::Overlaps(multiPolygon, emptyMultiPolygon));
	CHECK(not Geometry2D::Overlaps(emptyMultiPolygon, multiPolygon));
}
