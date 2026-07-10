//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

// Circle と Circle / Triangle / Quad / Polygon / MultiPolygon は、包含、境界接触、empty、退化 polygonal shape を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Circle_AreaShapes")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };

		CHECK(Geometry2D::Intersects(circle, Circle{ Vec2{ 9, 0 }, 4 }));
		CHECK(Geometry2D::Intersects(Circle{ Vec2{ 9, 0 }, 4 }, circle));
		CHECK(Geometry2D::Intersects(circle, Circle{ Vec2{ 10, 0 }, 5 }));
		CHECK(not Geometry2D::Intersects(circle, Circle{ Vec2{ 11, 0 }, 5 }));
		CHECK(not Geometry2D::Intersects(circle, Circle{ Vec2{ 0, 0 }, 0 }));
		CHECK(not Geometry2D::Intersects(Circle{ Vec2{ 0, 0 }, 0 }, circle));

		static_assert(Geometry2D::Intersects(Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 10, 0 }, 5 }));
		static_assert(not Geometry2D::Intersects(Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 11, 0 }, 5 }));
	}

	{
		const Circle circle{ Vec2{ 5, 5 }, 3 };
		const Triangle crossing{ Vec2{ 0, 5 }, Vec2{ 10, 5 }, Vec2{ 5, 10 } };
		const Triangle inside{ Vec2{ 4, 4 }, Vec2{ 6, 4 }, Vec2{ 5, 6 } };
		const Triangle containing{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Triangle touching{ Vec2{ 8, 5 }, Vec2{ 12, 5 }, Vec2{ 8, 8 } };
		const Triangle outside{ Vec2{ 9, 9 }, Vec2{ 12, 9 }, Vec2{ 9, 12 } };
		const Triangle segmentTriangle{ Vec2{ 2, 5 }, Vec2{ 8, 5 }, Vec2{ 5, 5 } };
		const Triangle pointTriangle{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } };
		const Triangle outsidePointTriangle{ Vec2{ 9, 9 }, Vec2{ 9, 9 }, Vec2{ 9, 9 } };

		CHECK(Geometry2D::Intersects(circle, crossing));
		CHECK(Geometry2D::Intersects(crossing, circle));
		CHECK(Geometry2D::Intersects(circle, inside));
		CHECK(Geometry2D::Intersects(inside, circle));
		CHECK(Geometry2D::Intersects(circle, containing));
		CHECK(Geometry2D::Intersects(containing, circle));
		CHECK(Geometry2D::Intersects(circle, touching));
		CHECK(Geometry2D::Intersects(touching, circle));
		CHECK(not Geometry2D::Intersects(circle, outside));
		CHECK(not Geometry2D::Intersects(outside, circle));
		CHECK(Geometry2D::Intersects(circle, segmentTriangle));
		CHECK(Geometry2D::Intersects(segmentTriangle, circle));
		CHECK(Geometry2D::Intersects(circle, pointTriangle));
		CHECK(Geometry2D::Intersects(pointTriangle, circle));
		CHECK(not Geometry2D::Intersects(circle, outsidePointTriangle));
		CHECK(not Geometry2D::Intersects(outsidePointTriangle, circle));
	}

	{
		const Circle circle{ Vec2{ 5, 5 }, 3 };
		const Quad crossing{ Vec2{ 7, 2 }, Vec2{ 12, 2 }, Vec2{ 12, 8 }, Vec2{ 7, 8 } };
		const Quad inside{ Vec2{ 4, 4 }, Vec2{ 6, 4 }, Vec2{ 6, 6 }, Vec2{ 4, 6 } };
		const Quad containing{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Quad touching{ Vec2{ 8, 4 }, Vec2{ 12, 4 }, Vec2{ 12, 6 }, Vec2{ 8, 6 } };
		const Quad outside{ Vec2{ 9, 9 }, Vec2{ 12, 9 }, Vec2{ 12, 12 }, Vec2{ 9, 12 } };
		const Quad segmentQuad{ Vec2{ 2, 5 }, Vec2{ 8, 5 }, Vec2{ 8, 5 }, Vec2{ 2, 5 } };
		const Quad pointQuad{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } };
		const Quad outsidePointQuad{ Vec2{ 9, 9 }, Vec2{ 9, 9 }, Vec2{ 9, 9 }, Vec2{ 9, 9 } };

		CHECK(Geometry2D::Intersects(circle, crossing));
		CHECK(Geometry2D::Intersects(crossing, circle));
		CHECK(Geometry2D::Intersects(circle, inside));
		CHECK(Geometry2D::Intersects(inside, circle));
		CHECK(Geometry2D::Intersects(circle, containing));
		CHECK(Geometry2D::Intersects(containing, circle));
		CHECK(Geometry2D::Intersects(circle, touching));
		CHECK(Geometry2D::Intersects(touching, circle));
		CHECK(not Geometry2D::Intersects(circle, outside));
		CHECK(not Geometry2D::Intersects(outside, circle));
		CHECK(Geometry2D::Intersects(circle, segmentQuad));
		CHECK(Geometry2D::Intersects(segmentQuad, circle));
		CHECK(Geometry2D::Intersects(circle, pointQuad));
		CHECK(Geometry2D::Intersects(pointQuad, circle));
		CHECK(not Geometry2D::Intersects(circle, outsidePointQuad));
		CHECK(not Geometry2D::Intersects(outsidePointQuad, circle));
	}

	{
		const Circle circle{ Vec2{ 5, 5 }, 3 };
		const Polygon polygon{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };
		const Polygon crossing{ Array<Vec2>{ Vec2{ 7, 4 }, Vec2{ 12, 4 }, Vec2{ 12, 6 }, Vec2{ 7, 6 } } };
		const Polygon outside{ Array<Vec2>{ Vec2{ 9, 9 }, Vec2{ 12, 9 }, Vec2{ 12, 12 }, Vec2{ 9, 12 } } };
		const Polygon emptyPolygon;
		const MultiPolygon multiPolygon{ polygon };
		const MultiPolygon outsideMultiPolygon{ outside };
		const MultiPolygon emptyMultiPolygon;

		CHECK(Geometry2D::Intersects(circle, polygon));
		CHECK(Geometry2D::Intersects(polygon, circle));
		CHECK(Geometry2D::Intersects(circle, crossing));
		CHECK(Geometry2D::Intersects(crossing, circle));
		CHECK(not Geometry2D::Intersects(circle, outside));
		CHECK(not Geometry2D::Intersects(outside, circle));
		CHECK(not Geometry2D::Intersects(circle, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, circle));
		CHECK(Geometry2D::Intersects(circle, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, circle));
		CHECK(not Geometry2D::Intersects(circle, outsideMultiPolygon));
		CHECK(not Geometry2D::Intersects(outsideMultiPolygon, circle));
		CHECK(not Geometry2D::Intersects(circle, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, circle));
	}
}

// Circle と Ellipse / SuperEllipse / RoundRect は、曲面 area shape 同士の交差、接触、empty、退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Circle_CurvedAreaShapes")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Ellipse crossing{ Vec2{ 7, 0 }, 4, 2 };
		const Ellipse tangent{ Vec2{ 10, 0 }, 5, 2 };
		const Ellipse outside{ Vec2{ 12, 0 }, 5, 2 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 6 };
		const Ellipse outsideVerticalSegment{ Vec2{ 10, 0 }, 0, 3 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 6, 0 };

		CHECK(Geometry2D::Intersects(circle, crossing));
		CHECK(Geometry2D::Intersects(crossing, circle));
		CHECK(Geometry2D::Intersects(circle, tangent));
		CHECK(Geometry2D::Intersects(tangent, circle));
		CHECK(not Geometry2D::Intersects(circle, outside));
		CHECK(not Geometry2D::Intersects(outside, circle));
		CHECK(not Geometry2D::Intersects(circle, empty));
		CHECK(not Geometry2D::Intersects(empty, circle));
		CHECK(Geometry2D::Intersects(circle, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, circle));
		CHECK(not Geometry2D::Intersects(circle, outsideVerticalSegment));
		CHECK(not Geometry2D::Intersects(outsideVerticalSegment, circle));
		CHECK(Geometry2D::Intersects(circle, horizontalSegment));
		CHECK(Geometry2D::Intersects(horizontalSegment, circle));
	}

	// 扁平な Ellipse の近傍で、角度 Newton 法が誤った端点へ clamp されるケースの regression。
	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 10.0, 1.0 };
		const Vec2 center{ 8.776703201465617, 0.479473481158063 };
		const Circle intersecting{ center, 0.0003 };
		const Circle outside{ center, 0.0001 };

		CHECK(Geometry2D::Intersects(intersecting, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, intersecting));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
	}

	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const SuperEllipse crossing{ Vec2{ 7, 0 }, SizeF{ 4, 2 }, 4.0 };
		const SuperEllipse ellipseEquivalent{ Vec2{ 10, 0 }, SizeF{ 5, 2 }, 2.0 };
		const SuperEllipse outside{ Vec2{ 12, 0 }, SizeF{ 5, 2 }, 4.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 6 }, 4.0 };
		const SuperEllipse outsideVerticalSegment{ Vec2{ 10, 0 }, SizeF{ 0, 3 }, 4.0 };
		const SuperEllipse horizontalSegment{ Vec2{ 0, 0 }, SizeF{ 6, 0 }, 4.0 };

		CHECK(Geometry2D::Intersects(circle, crossing));
		CHECK(Geometry2D::Intersects(crossing, circle));
		CHECK(Geometry2D::Intersects(circle, ellipseEquivalent));
		CHECK(Geometry2D::Intersects(ellipseEquivalent, circle));
		CHECK(not Geometry2D::Intersects(circle, outside));
		CHECK(not Geometry2D::Intersects(outside, circle));
		CHECK(not Geometry2D::Intersects(circle, empty));
		CHECK(not Geometry2D::Intersects(empty, circle));
		CHECK(Geometry2D::Intersects(circle, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, circle));
		CHECK(not Geometry2D::Intersects(circle, outsideVerticalSegment));
		CHECK(not Geometry2D::Intersects(outsideVerticalSegment, circle));
		CHECK(Geometry2D::Intersects(circle, horizontalSegment));
		CHECK(Geometry2D::Intersects(horizontalSegment, circle));
	}

	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const RoundRect crossing{ RectF{ 4, -2, 6, 4 }, 1 };
		const RoundRect tangent{ RectF{ 5, -1, 2, 2 }, 1 };
		const RoundRect outside{ RectF{ 6, -1, 2, 2 }, 1 };
		const RoundRect rectEquivalent{ RectF{ 4, -1, 2, 2 }, 0 };
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 5 };
		const RoundRect verticalSegment{ RectF{ 0, -6, 0, 12 }, 5 };
		const RoundRect outsideVerticalSegment{ RectF{ 10, -3, 0, 6 }, 5 };

		CHECK(Geometry2D::Intersects(circle, crossing));
		CHECK(Geometry2D::Intersects(crossing, circle));
		CHECK(Geometry2D::Intersects(circle, tangent));
		CHECK(Geometry2D::Intersects(tangent, circle));
		CHECK(not Geometry2D::Intersects(circle, outside));
		CHECK(not Geometry2D::Intersects(outside, circle));
		CHECK(Geometry2D::Intersects(circle, rectEquivalent));
		CHECK(Geometry2D::Intersects(rectEquivalent, circle));
		CHECK(not Geometry2D::Intersects(circle, empty));
		CHECK(not Geometry2D::Intersects(empty, circle));
		CHECK(Geometry2D::Intersects(circle, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, circle));
		CHECK(not Geometry2D::Intersects(circle, outsideVerticalSegment));
		CHECK(not Geometry2D::Intersects(outsideVerticalSegment, circle));
	}
}



// Ellipse と polygonal / polygon container area shapes は、包含、境界接触、empty、片側ゼロ軸を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Ellipse_PolygonalAreaShapes")
{
	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };
		const Triangle crossing{ Vec2{ 4, -1 }, Vec2{ 10, -1 }, Vec2{ 4, 1 } };
		const Triangle containing{ Vec2{ -10, -10 }, Vec2{ 10, -10 }, Vec2{ 0, 10 } };
		const Triangle tangent{ Vec2{ -5, 3 }, Vec2{ 5, 3 }, Vec2{ 0, 8 } };
		const Triangle outside{ Vec2{ 6, 4 }, Vec2{ 10, 4 }, Vec2{ 6, 8 } };
		const Triangle segmentTriangle{ Vec2{ -1, 0 }, Vec2{ 1, 0 }, Vec2{ 0, 0 } };
		const Triangle outsideSegmentTriangle{ Vec2{ 6, 0 }, Vec2{ 7, 0 }, Vec2{ 6.5, 0 } };

		CHECK(Geometry2D::Intersects(ellipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, containing));
		CHECK(Geometry2D::Intersects(containing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(not Geometry2D::Intersects(empty, containing));
		CHECK(not Geometry2D::Intersects(containing, empty));
		CHECK(Geometry2D::Intersects(verticalSegment, segmentTriangle));
		CHECK(Geometry2D::Intersects(segmentTriangle, verticalSegment));
		CHECK(Geometry2D::Intersects(horizontalSegment, segmentTriangle));
		CHECK(Geometry2D::Intersects(segmentTriangle, horizontalSegment));
		CHECK(not Geometry2D::Intersects(ellipse, outsideSegmentTriangle));
		CHECK(not Geometry2D::Intersects(outsideSegmentTriangle, ellipse));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const Quad crossing{ Vec2{ 4, -1 }, Vec2{ 8, -1 }, Vec2{ 8, 1 }, Vec2{ 4, 1 } };
		const Quad containing{ Vec2{ -6, -4 }, Vec2{ 6, -4 }, Vec2{ 6, 4 }, Vec2{ -6, 4 } };
		const Quad tangent{ Vec2{ -5, 3 }, Vec2{ 5, 3 }, Vec2{ 5, 5 }, Vec2{ -5, 5 } };
		const Quad outside{ Vec2{ 6, 4 }, Vec2{ 10, 4 }, Vec2{ 10, 8 }, Vec2{ 6, 8 } };
		const Quad segmentQuad{ Vec2{ -1, 0 }, Vec2{ 1, 0 }, Vec2{ 1, 0 }, Vec2{ -1, 0 } };
		const Quad pointQuad{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
		const Quad outsidePointQuad{ Vec2{ 6, 0 }, Vec2{ 6, 0 }, Vec2{ 6, 0 }, Vec2{ 6, 0 } };

		CHECK(Geometry2D::Intersects(ellipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, containing));
		CHECK(Geometry2D::Intersects(containing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, segmentQuad));
		CHECK(Geometry2D::Intersects(segmentQuad, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, pointQuad));
		CHECK(Geometry2D::Intersects(pointQuad, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outsidePointQuad));
		CHECK(not Geometry2D::Intersects(outsidePointQuad, ellipse));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Polygon crossing{ Array<Vec2>{ Vec2{ 4, -1 }, Vec2{ 8, -1 }, Vec2{ 8, 1 }, Vec2{ 4, 1 } } };
		const Polygon containing{ Array<Vec2>{ Vec2{ -6, -4 }, Vec2{ 6, -4 }, Vec2{ 6, 4 }, Vec2{ -6, 4 } } };
		const Polygon tangent{ Array<Vec2>{ Vec2{ -5, 3 }, Vec2{ 5, 3 }, Vec2{ 5, 5 }, Vec2{ -5, 5 } } };
		const Polygon outside{ Array<Vec2>{ Vec2{ 6, 4 }, Vec2{ 10, 4 }, Vec2{ 10, 8 }, Vec2{ 6, 8 } } };
		const Polygon segmentHit{ Array<Vec2>{ Vec2{ -1, -1 }, Vec2{ 1, -1 }, Vec2{ 1, 1 }, Vec2{ -1, 1 } } };
		const Polygon emptyPolygon;
		const MultiPolygon multiPolygon{ outside, crossing };
		const MultiPolygon outsideMultiPolygon{ outside };
		const MultiPolygon emptyMultiPolygon;

		CHECK(Geometry2D::Intersects(ellipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, containing));
		CHECK(Geometry2D::Intersects(containing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, ellipse));
		CHECK(not Geometry2D::Intersects(empty, containing));
		CHECK(not Geometry2D::Intersects(containing, empty));
		CHECK(Geometry2D::Intersects(verticalSegment, segmentHit));
		CHECK(Geometry2D::Intersects(segmentHit, verticalSegment));
		CHECK(Geometry2D::Intersects(ellipse, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outsideMultiPolygon));
		CHECK(not Geometry2D::Intersects(outsideMultiPolygon, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, ellipse));
	}
}


// Triangle / Quad と Polygon / MultiPolygon は、包含、境界接触、empty container、退化 point / segment を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.PolygonalShapes_PolygonContainers")
{
	const Polygon polygon{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };
	const MultiPolygon multiPolygon{ polygon };
	const Polygon emptyPolygon;
	const MultiPolygon emptyMultiPolygon;

	{
		const Triangle crossing{ Vec2{ 5, -1 }, Vec2{ 11, -1 }, Vec2{ 5, 5 } };
		const Triangle inside{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 2, 4 } };
		const Triangle containing{ Vec2{ -1, -1 }, Vec2{ 20, -1 }, Vec2{ -1, 20 } };
		const Triangle touching{ Vec2{ 10, 2 }, Vec2{ 15, 2 }, Vec2{ 10, 8 } };
		const Triangle outside{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 11, 20 } };

		CHECK(Geometry2D::Intersects(crossing, polygon));
		CHECK(Geometry2D::Intersects(polygon, crossing));
		CHECK(Geometry2D::Intersects(inside, polygon));
		CHECK(Geometry2D::Intersects(polygon, inside));
		CHECK(Geometry2D::Intersects(containing, polygon));
		CHECK(Geometry2D::Intersects(polygon, containing));
		CHECK(Geometry2D::Intersects(touching, polygon));
		CHECK(Geometry2D::Intersects(polygon, touching));
		CHECK(not Geometry2D::Intersects(outside, polygon));
		CHECK(not Geometry2D::Intersects(polygon, outside));

		CHECK(Geometry2D::Intersects(crossing, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, crossing));
		CHECK(not Geometry2D::Intersects(outside, multiPolygon));
		CHECK(not Geometry2D::Intersects(multiPolygon, outside));
	}

	{
		const Triangle segmentTriangle{ Vec2{ -1, 5 }, Vec2{ 11, 5 }, Vec2{ 5, 5 } };
		const Triangle pointTriangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };
		const Triangle outsidePointTriangle{ Vec2{ 11, 11 }, Vec2{ 11, 11 }, Vec2{ 11, 11 } };

		CHECK(Geometry2D::Intersects(segmentTriangle, polygon));
		CHECK(Geometry2D::Intersects(polygon, segmentTriangle));
		CHECK(Geometry2D::Intersects(pointTriangle, polygon));
		CHECK(Geometry2D::Intersects(polygon, pointTriangle));
		CHECK(not Geometry2D::Intersects(outsidePointTriangle, polygon));
		CHECK(not Geometry2D::Intersects(polygon, outsidePointTriangle));
	}

	{
		const Quad crossing{ Vec2{ 8, 2 }, Vec2{ 12, 2 }, Vec2{ 12, 8 }, Vec2{ 8, 8 } };
		const Quad inside{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } };
		const Quad containing{ Vec2{ -1, -1 }, Vec2{ 11, -1 }, Vec2{ 11, 11 }, Vec2{ -1, 11 } };
		const Quad touching{ Vec2{ 10, 2 }, Vec2{ 15, 2 }, Vec2{ 15, 8 }, Vec2{ 10, 8 } };
		const Quad outside{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 20, 20 }, Vec2{ 11, 20 } };

		CHECK(Geometry2D::Intersects(crossing, polygon));
		CHECK(Geometry2D::Intersects(polygon, crossing));
		CHECK(Geometry2D::Intersects(inside, polygon));
		CHECK(Geometry2D::Intersects(polygon, inside));
		CHECK(Geometry2D::Intersects(containing, polygon));
		CHECK(Geometry2D::Intersects(polygon, containing));
		CHECK(Geometry2D::Intersects(touching, polygon));
		CHECK(Geometry2D::Intersects(polygon, touching));
		CHECK(not Geometry2D::Intersects(outside, polygon));
		CHECK(not Geometry2D::Intersects(polygon, outside));

		CHECK(Geometry2D::Intersects(crossing, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, crossing));
		CHECK(not Geometry2D::Intersects(outside, multiPolygon));
		CHECK(not Geometry2D::Intersects(multiPolygon, outside));
	}

	{
		const Quad segmentQuad{ Vec2{ -1, 5 }, Vec2{ 11, 5 }, Vec2{ 11, 5 }, Vec2{ -1, 5 } };
		const Quad pointQuad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };
		const Quad outsidePointQuad{ Vec2{ 11, 11 }, Vec2{ 11, 11 }, Vec2{ 11, 11 }, Vec2{ 11, 11 } };

		CHECK(Geometry2D::Intersects(segmentQuad, polygon));
		CHECK(Geometry2D::Intersects(polygon, segmentQuad));
		CHECK(Geometry2D::Intersects(pointQuad, polygon));
		CHECK(Geometry2D::Intersects(polygon, pointQuad));
		CHECK(not Geometry2D::Intersects(outsidePointQuad, polygon));
		CHECK(not Geometry2D::Intersects(polygon, outsidePointQuad));
	}

	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };

		CHECK(not Geometry2D::Intersects(triangle, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, triangle));
		CHECK(not Geometry2D::Intersects(quad, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, quad));
		CHECK(not Geometry2D::Intersects(triangle, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, triangle));
		CHECK(not Geometry2D::Intersects(quad, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, quad));
	}
}


// Polygon / MultiPolygon 同士は、包含、境界接触、empty container、member-wise 判定を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.PolygonContainers_PolygonContainers")
{
	const Polygon base{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };
	const Polygon overlap{ Array<Vec2>{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } } };
	const Polygon inside{ Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const Polygon containing{ Array<Vec2>{ Vec2{ -1, -1 }, Vec2{ 11, -1 }, Vec2{ 11, 11 }, Vec2{ -1, 11 } } };
	const Polygon edgeTouch{ Array<Vec2>{ Vec2{ 10, 2 }, Vec2{ 15, 2 }, Vec2{ 15, 8 }, Vec2{ 10, 8 } } };
	const Polygon pointTouch{ Array<Vec2>{ Vec2{ 10, 10 }, Vec2{ 15, 10 }, Vec2{ 10, 15 } } };
	const Polygon outside{ Array<Vec2>{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 20, 20 }, Vec2{ 11, 20 } } };
	const Polygon emptyPolygon;

	CHECK(Geometry2D::Intersects(base, overlap));
	CHECK(Geometry2D::Intersects(overlap, base));
	CHECK(Geometry2D::Intersects(base, inside));
	CHECK(Geometry2D::Intersects(inside, base));
	CHECK(Geometry2D::Intersects(base, containing));
	CHECK(Geometry2D::Intersects(containing, base));
	CHECK(Geometry2D::Intersects(base, edgeTouch));
	CHECK(Geometry2D::Intersects(edgeTouch, base));
	CHECK(Geometry2D::Intersects(base, pointTouch));
	CHECK(Geometry2D::Intersects(pointTouch, base));
	CHECK(not Geometry2D::Intersects(base, outside));
	CHECK(not Geometry2D::Intersects(outside, base));
	CHECK(not Geometry2D::Intersects(base, emptyPolygon));
	CHECK(not Geometry2D::Intersects(emptyPolygon, base));

	{
		const MultiPolygon multi{ base };
		const MultiPolygon overlapMulti{ overlap };
		const MultiPolygon outsideMulti{ outside };
		const MultiPolygon emptyMulti;

		CHECK(Geometry2D::Intersects(base, overlapMulti));
		CHECK(Geometry2D::Intersects(overlapMulti, base));
		CHECK(Geometry2D::Intersects(multi, overlap));
		CHECK(Geometry2D::Intersects(overlap, multi));
		CHECK(Geometry2D::Intersects(multi, overlapMulti));
		CHECK(Geometry2D::Intersects(overlapMulti, multi));
		CHECK(not Geometry2D::Intersects(base, outsideMulti));
		CHECK(not Geometry2D::Intersects(outsideMulti, base));
		CHECK(not Geometry2D::Intersects(multi, outsideMulti));
		CHECK(not Geometry2D::Intersects(outsideMulti, multi));
		CHECK(not Geometry2D::Intersects(base, emptyMulti));
		CHECK(not Geometry2D::Intersects(emptyMulti, base));
		CHECK(not Geometry2D::Intersects(multi, emptyMulti));
		CHECK(not Geometry2D::Intersects(emptyMulti, multi));
	}

	{
		const Polygon far{ Array<Vec2>{ Vec2{ 100, 100 }, Vec2{ 110, 100 }, Vec2{ 110, 110 }, Vec2{ 100, 110 } } };
		const MultiPolygon multi{ far, base };

		CHECK(Geometry2D::Intersects(overlap, multi));
		CHECK(Geometry2D::Intersects(multi, overlap));
	}
}
