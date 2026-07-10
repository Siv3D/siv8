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

// Ellipse と曲線系 AreaShape は、包含、境界接触、empty、partial-zero degeneration、近似境界判定を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Ellipse_CurvedAreaShapes")
{
	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const Ellipse overlapping{ Vec2{ 6, 0 }, 4, 2 };
		const Ellipse containing{ Vec2{ 0, 0 }, 10, 6 };
		const Ellipse tangent{ Vec2{ 10, 0 }, 5, 3 };
		const Ellipse outside{ Vec2{ 11, 0 }, 5, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };
		const Ellipse segmentHit{ Vec2{ 0, 2 }, 2, 0 };
		const Ellipse segmentMiss{ Vec2{ 6, 6 }, 2, 0 };

		CHECK(Geometry2D::Intersects(ellipse, overlapping));
		CHECK(Geometry2D::Intersects(overlapping, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, containing));
		CHECK(Geometry2D::Intersects(containing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, empty));
		CHECK(not Geometry2D::Intersects(empty, ellipse));
		CHECK(Geometry2D::Intersects(verticalSegment, segmentHit));
		CHECK(Geometry2D::Intersects(segmentHit, verticalSegment));
		CHECK(not Geometry2D::Intersects(verticalSegment, segmentMiss));
		CHECK(not Geometry2D::Intersects(segmentMiss, verticalSegment));
		CHECK(Geometry2D::Intersects(horizontalSegment, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, horizontalSegment));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const SuperEllipse crossing{ Vec2{ 6, 0 }, SizeF{ 4, 2 }, 4.0 };
		const SuperEllipse containing{ Vec2{ 0, 0 }, SizeF{ 10, 6 }, 4.0 };
		const SuperEllipse tangent{ Vec2{ 10, 0 }, SizeF{ 5, 3 }, 4.0 };
		const SuperEllipse outside{ Vec2{ 11, 0 }, SizeF{ 5, 3 }, 4.0 };
		const SuperEllipse ellipseEquivalent{ Vec2{ 6, 0 }, SizeF{ 4, 2 }, 2.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };

		CHECK(Geometry2D::Intersects(ellipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, containing));
		CHECK(Geometry2D::Intersects(containing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, ellipseEquivalent));
		CHECK(Geometry2D::Intersects(ellipseEquivalent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, empty));
		CHECK(not Geometry2D::Intersects(empty, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, ellipse));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const RoundRect crossing{ RectF{ 4, -2, 5, 4 }, 1.0 };
		const RoundRect containing{ RectF{ -8, -5, 16, 10 }, 2.0 };
		const RoundRect tangent{ RectF{ -5, 3, 10, 4 }, 1.0 };
		const RoundRect outside{ RectF{ 6, 4, 4, 4 }, 1.0 };
		const RoundRect rectEquivalent{ RectF{ 4, -2, 5, 4 }, 0.0 };
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 1.0 };
		const RoundRect verticalSegment{ RectF{ 0, -5, 0, 10 }, 1.0 };

		CHECK(Geometry2D::Intersects(ellipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, containing));
		CHECK(Geometry2D::Intersects(containing, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, outside));
		CHECK(not Geometry2D::Intersects(outside, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, rectEquivalent));
		CHECK(Geometry2D::Intersects(rectEquivalent, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, empty));
		CHECK(not Geometry2D::Intersects(empty, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, ellipse));
	}
}


// SuperEllipse と polygonal area shapes は、包含、境界接触、empty、partial-zero axis を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.SuperEllipse_PolygonalAreaShapes")
{
	const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };
	const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
	const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };
	const SuperEllipse ellipseEquivalent{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 2.0 };
	const Polygon polygon{ Array<Vec2>{ Vec2{ -2, -2 }, Vec2{ 2, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } } };
	const Polygon crossingPolygon{ Array<Vec2>{ Vec2{ 4, -1 }, Vec2{ 7, -1 }, Vec2{ 7, 1 }, Vec2{ 4, 1 } } };
	const Polygon outsidePolygon{ Array<Vec2>{ Vec2{ 6, 4 }, Vec2{ 8, 4 }, Vec2{ 8, 6 }, Vec2{ 6, 6 } } };
	const Polygon emptyPolygon;
	const MultiPolygon multiPolygon{ polygon };
	const MultiPolygon outsideMultiPolygon{ outsidePolygon };
	const MultiPolygon emptyMultiPolygon;

	{
		const Triangle crossing{ Vec2{ 5, -1 }, Vec2{ 8, -1 }, Vec2{ 5, 2 } };
		const Triangle inside{ Vec2{ -1, -1 }, Vec2{ 1, -1 }, Vec2{ 0, 1 } };
		const Triangle containing{ Vec2{ -10, -10 }, Vec2{ 10, -10 }, Vec2{ -10, 10 } };
		const Triangle outside{ Vec2{ 6, 4 }, Vec2{ 8, 4 }, Vec2{ 6, 6 } };
		const Triangle segmentHit{ Vec2{ -1, 0 }, Vec2{ 1, 0 }, Vec2{ 0, 0 } };
		const Triangle segmentMiss{ Vec2{ 1, 4 }, Vec2{ 2, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(superEllipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, inside));
		CHECK(Geometry2D::Intersects(inside, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, containing));
		CHECK(Geometry2D::Intersects(containing, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, outside));
		CHECK(not Geometry2D::Intersects(outside, superEllipse));
		CHECK(not Geometry2D::Intersects(empty, containing));
		CHECK(not Geometry2D::Intersects(containing, empty));
		CHECK(Geometry2D::Intersects(verticalSegment, segmentHit));
		CHECK(Geometry2D::Intersects(segmentHit, verticalSegment));
		CHECK(not Geometry2D::Intersects(verticalSegment, segmentMiss));
		CHECK(not Geometry2D::Intersects(segmentMiss, verticalSegment));
		CHECK(Geometry2D::Intersects(ellipseEquivalent, crossing));
		CHECK(Geometry2D::Intersects(crossing, ellipseEquivalent));
	}

	{
		const Quad crossing{ Vec2{ 4, -1 }, Vec2{ 7, -1 }, Vec2{ 7, 1 }, Vec2{ 4, 1 } };
		const Quad inside{ Vec2{ -1, -1 }, Vec2{ 1, -1 }, Vec2{ 1, 1 }, Vec2{ -1, 1 } };
		const Quad containing{ Vec2{ -10, -10 }, Vec2{ 10, -10 }, Vec2{ 10, 10 }, Vec2{ -10, 10 } };
		const Quad outside{ Vec2{ 6, 4 }, Vec2{ 8, 4 }, Vec2{ 8, 6 }, Vec2{ 6, 6 } };
		const Quad segmentHit{ Vec2{ -1, 0 }, Vec2{ 1, 0 }, Vec2{ 1, 0 }, Vec2{ -1, 0 } };
		const Quad pointHit{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
		const Quad pointMiss{ Vec2{ 6, 6 }, Vec2{ 6, 6 }, Vec2{ 6, 6 }, Vec2{ 6, 6 } };

		CHECK(Geometry2D::Intersects(superEllipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, inside));
		CHECK(Geometry2D::Intersects(inside, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, containing));
		CHECK(Geometry2D::Intersects(containing, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, outside));
		CHECK(not Geometry2D::Intersects(outside, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, segmentHit));
		CHECK(Geometry2D::Intersects(segmentHit, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, pointHit));
		CHECK(Geometry2D::Intersects(pointHit, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, pointMiss));
		CHECK(not Geometry2D::Intersects(pointMiss, superEllipse));
	}

	{
		CHECK(Geometry2D::Intersects(superEllipse, polygon));
		CHECK(Geometry2D::Intersects(polygon, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, crossingPolygon));
		CHECK(Geometry2D::Intersects(crossingPolygon, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, outsidePolygon));
		CHECK(not Geometry2D::Intersects(outsidePolygon, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, outsideMultiPolygon));
		CHECK(not Geometry2D::Intersects(outsideMultiPolygon, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, superEllipse));
	}
}

// SuperEllipse / RoundRect と残りの area shapes は、曲線境界、包含、境界接触、empty、退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.RemainingAreaShapes")
{
	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };
		const SuperEllipse crossing{ Vec2{ 6, 0 }, SizeF{ 4, 2 }, 4.0 };
		const SuperEllipse containing{ Vec2{ 0, 0 }, SizeF{ 10, 6 }, 4.0 };
		const SuperEllipse tangent{ Vec2{ 9, 0 }, SizeF{ 4, 2 }, 4.0 };
		const SuperEllipse outside{ Vec2{ 10.5, 0 }, SizeF{ 4, 2 }, 4.0 };
		const SuperEllipse ellipseEquivalent{ Vec2{ 6, 0 }, SizeF{ 4, 2 }, 2.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };

		CHECK(Geometry2D::Intersects(superEllipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, containing));
		CHECK(Geometry2D::Intersects(containing, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, outside));
		CHECK(not Geometry2D::Intersects(outside, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, ellipseEquivalent));
		CHECK(Geometry2D::Intersects(ellipseEquivalent, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, empty));
		CHECK(not Geometry2D::Intersects(empty, superEllipse));
		CHECK(Geometry2D::Intersects(verticalSegment, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, verticalSegment));
	}

	// n < 1 では、境界のサンプル点を結ぶ chord が実際の集合の外側を通る。
	// 96 分割 chord 上に極小形状を置いても、実形状同士が離れていれば交差しないことを確認する。
	{
		const SuperEllipse concave{ Vec2{ 0, 0 }, SizeF{ 10, 10 }, 0.5 };
		const SuperEllipse chordOnlyHit{
			Vec2{ 9.957315794865192, 0.00009148799614165133 },
			SizeF{ 1e-6, 1e-6 },
			4.0
		};
		const SuperEllipse tangent{ Vec2{ 10.001, 0 }, SizeF{ 0.001, 0.001 }, 4.0 };
		const SuperEllipse outside{ Vec2{ 10.0011, 0 }, SizeF{ 0.001, 0.001 }, 4.0 };

		CHECK(not Geometry2D::Intersects(concave, chordOnlyHit));
		CHECK(not Geometry2D::Intersects(chordOnlyHit, concave));
		CHECK(Geometry2D::Intersects(concave, tangent));
		CHECK(Geometry2D::Intersects(tangent, concave));
		CHECK(not Geometry2D::Intersects(concave, outside));
		CHECK(not Geometry2D::Intersects(outside, concave));
	}

	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };
		const RoundRect crossing{ RectF{ 4, -2, 5, 4 }, 1.0 };
		const RoundRect containing{ RectF{ -8, -5, 16, 10 }, 2.0 };
		const RoundRect tangent{ RectF{ -5, 3, 10, 4 }, 1.0 };
		const RoundRect outside{ RectF{ 6, 4, 4, 4 }, 1.0 };
		const RoundRect rectEquivalent{ RectF{ 4, -2, 5, 4 }, 0.0 };
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 1.0 };
		const RoundRect verticalSegment{ RectF{ 0, -5, 0, 10 }, 1.0 };

		CHECK(Geometry2D::Intersects(superEllipse, crossing));
		CHECK(Geometry2D::Intersects(crossing, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, containing));
		CHECK(Geometry2D::Intersects(containing, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, tangent));
		CHECK(Geometry2D::Intersects(tangent, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, outside));
		CHECK(not Geometry2D::Intersects(outside, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, rectEquivalent));
		CHECK(Geometry2D::Intersects(rectEquivalent, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, empty));
		CHECK(not Geometry2D::Intersects(empty, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, superEllipse));
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 2.0 };
		const RoundRect verticalSegment{ RectF{ 5, -2, 0, 4 }, 2.0 };
		const Triangle crossing{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ -1, 8 } };
		const Triangle inside{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 2, 4 } };
		const Triangle containing{ Vec2{ -10, -10 }, Vec2{ 20, -10 }, Vec2{ -10, 20 } };
		const Triangle outside{ Vec2{ 12, 12 }, Vec2{ 14, 12 }, Vec2{ 12, 14 } };
		const Triangle segmentHit{ Vec2{ 5, -1 }, Vec2{ 5, 3 }, Vec2{ 5, 1 } };
		const Triangle segmentMiss{ Vec2{ 12, 0 }, Vec2{ 12, 4 }, Vec2{ 12, 2 } };

		CHECK(Geometry2D::Intersects(roundRect, crossing));
		CHECK(Geometry2D::Intersects(crossing, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, inside));
		CHECK(Geometry2D::Intersects(inside, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, containing));
		CHECK(Geometry2D::Intersects(containing, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, outside));
		CHECK(not Geometry2D::Intersects(outside, roundRect));
		CHECK(not Geometry2D::Intersects(empty, containing));
		CHECK(not Geometry2D::Intersects(containing, empty));
		CHECK(Geometry2D::Intersects(verticalSegment, segmentHit));
		CHECK(Geometry2D::Intersects(segmentHit, verticalSegment));
		CHECK(not Geometry2D::Intersects(verticalSegment, segmentMiss));
		CHECK(not Geometry2D::Intersects(segmentMiss, verticalSegment));
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };
		const Quad crossing{ Vec2{ -1, 4 }, Vec2{ 2, 4 }, Vec2{ 2, 6 }, Vec2{ -1, 6 } };
		const Quad inside{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } };
		const Quad containing{ Vec2{ -10, -10 }, Vec2{ 20, -10 }, Vec2{ 20, 20 }, Vec2{ -10, 20 } };
		const Quad outside{ Vec2{ 12, 12 }, Vec2{ 14, 12 }, Vec2{ 14, 14 }, Vec2{ 12, 14 } };
		const Quad segmentHit{ Vec2{ 5, -1 }, Vec2{ 5, 3 }, Vec2{ 5, 3 }, Vec2{ 5, -1 } };
		const Quad pointHit{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } };
		const Quad pointMiss{ Vec2{ 12, 12 }, Vec2{ 12, 12 }, Vec2{ 12, 12 }, Vec2{ 12, 12 } };

		CHECK(Geometry2D::Intersects(roundRect, crossing));
		CHECK(Geometry2D::Intersects(crossing, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, inside));
		CHECK(Geometry2D::Intersects(inside, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, containing));
		CHECK(Geometry2D::Intersects(containing, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, outside));
		CHECK(not Geometry2D::Intersects(outside, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, segmentHit));
		CHECK(Geometry2D::Intersects(segmentHit, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, pointHit));
		CHECK(Geometry2D::Intersects(pointHit, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, pointMiss));
		CHECK(not Geometry2D::Intersects(pointMiss, roundRect));
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };
		const RoundRect overlapping{ RectF{ 8, 2, 5, 5 }, 1.0 };
		const RoundRect containing{ RectF{ -5, -5, 20, 20 }, 3.0 };
		const RoundRect tangent{ RectF{ 10, 2, 4, 4 }, 1.0 };
		const RoundRect outside{ RectF{ 11, 0, 4, 4 }, 1.0 };
		const RoundRect rectEquivalent{ RectF{ 8, 2, 5, 5 }, 0.0 };
		const RoundRect verticalSegment{ RectF{ 5, -2, 0, 4 }, 1.0 };

		CHECK(Geometry2D::Intersects(roundRect, overlapping));
		CHECK(Geometry2D::Intersects(overlapping, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, containing));
		CHECK(Geometry2D::Intersects(containing, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, tangent));
		CHECK(Geometry2D::Intersects(tangent, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, outside));
		CHECK(not Geometry2D::Intersects(outside, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, rectEquivalent));
		CHECK(Geometry2D::Intersects(rectEquivalent, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, roundRect));
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };
		const Polygon polygon{ Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
		const Polygon crossingPolygon{ Array<Vec2>{ Vec2{ -1, 4 }, Vec2{ 2, 4 }, Vec2{ 2, 6 }, Vec2{ -1, 6 } } };
		const Polygon outsidePolygon{ Array<Vec2>{ Vec2{ 12, 12 }, Vec2{ 14, 12 }, Vec2{ 14, 14 }, Vec2{ 12, 14 } } };
		const Polygon emptyPolygon;
		const MultiPolygon multiPolygon{ polygon };
		const MultiPolygon outsideMultiPolygon{ outsidePolygon };
		const MultiPolygon emptyMultiPolygon;

		CHECK(Geometry2D::Intersects(roundRect, polygon));
		CHECK(Geometry2D::Intersects(polygon, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, crossingPolygon));
		CHECK(Geometry2D::Intersects(crossingPolygon, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, outsidePolygon));
		CHECK(not Geometry2D::Intersects(outsidePolygon, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, outsideMultiPolygon));
		CHECK(not Geometry2D::Intersects(outsideMultiPolygon, roundRect));
		CHECK(not Geometry2D::Intersects(roundRect, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, roundRect));
	}
}
