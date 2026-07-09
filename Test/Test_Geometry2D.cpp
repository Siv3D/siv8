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

// Point と Vec2 は、座標の完全一致で交差判定されることを確認する。
TEST_CASE("Geometry2D.Intersects.Point_Vec2")
{
	CHECK(Geometry2D::Intersects(Point{ 1, 2 }, Point{ 1, 2 }));
	CHECK(not Geometry2D::Intersects(Point{ 1, 2 }, Point{ 2, 1 }));
	CHECK(Geometry2D::Intersects(Point{ 1, 2 }, Vec2{ 1.0, 2.0 }));
	CHECK(not Geometry2D::Intersects(Point{ 1, 2 }, Vec2{ 1.0000000000000002, 2.0 }));
	CHECK(Geometry2D::Intersects(Vec2{ 1.0, 2.0 }, Point{ 1, 2 }));
	CHECK(not Geometry2D::Intersects(Vec2{ 1.0000000000000002, 2.0 }, Point{ 1, 2 }));
}

// 点と Line は、閉じた線分として端点・内部点・ゼロ長線分を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_Line")
{
	const Line horizontal{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
	const Line vertical{ Vec2{ 4, -5 }, Vec2{ 4, 5 } };
	const Line zeroLength{ Vec2{ 3, 4 }, Vec2{ 3, 4 } };

	CHECK(Geometry2D::Intersects(Vec2{ 0, 0 }, horizontal));
	CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, horizontal));
	CHECK(Geometry2D::Intersects(Vec2{ 10, 0 }, horizontal));
	CHECK(not Geometry2D::Intersects(Vec2{ 5, 1 }, horizontal));
	CHECK(not Geometry2D::Intersects(Vec2{ 11, 0 }, horizontal));
	CHECK(Geometry2D::Intersects(Vec2{ 4, 0 }, vertical));
	CHECK(Geometry2D::Intersects(Vec2{ 3, 4 }, zeroLength));
	CHECK(not Geometry2D::Intersects(Vec2{ 3, 5 }, zeroLength));
	CHECK(Geometry2D::Intersects(Point{ 5, 0 }, horizontal));
	CHECK(not Geometry2D::Intersects(Point{ 5, 1 }, horizontal));
	CHECK(Geometry2D::Intersects(Point{ 3, 4 }, zeroLength));
	CHECK(not Geometry2D::Intersects(Point{ 3, 5 }, zeroLength));
}


// Line 同士は、交差、端点接触、重なり、ゼロ長線分を閉じた線分として扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_Line")
{
	const Line horizontal{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };
	const Line vertical{ Vec2{ 5, -5 }, Vec2{ 5, 5 } };
	const Line touching{ Vec2{ 10, 0 }, Vec2{ 15, 5 } };
	const Line overlap{ Vec2{ 5, 0 }, Vec2{ 15, 0 } };
	const Line disjointCollinear{ Vec2{ 11, 0 }, Vec2{ 20, 0 } };
	const Line disjointParallel{ Vec2{ 0, 1 }, Vec2{ 10, 1 } };
	const Line pointOnLine{ Vec2{ 5, 0 }, Vec2{ 5, 0 } };
	const Line pointOffLine{ Vec2{ 5, 1 }, Vec2{ 5, 1 } };

	CHECK(Geometry2D::Intersects(horizontal, vertical));
	CHECK(Geometry2D::Intersects(vertical, horizontal));
	CHECK(Geometry2D::Intersects(horizontal, touching));
	CHECK(Geometry2D::Intersects(horizontal, overlap));
	CHECK(not Geometry2D::Intersects(horizontal, disjointCollinear));
	CHECK(not Geometry2D::Intersects(horizontal, disjointParallel));
	CHECK(Geometry2D::Intersects(horizontal, pointOnLine));
	CHECK(not Geometry2D::Intersects(horizontal, pointOffLine));
}

// Line と LineString は、空、単一点、連続線分、重複点を線分集合として扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_LineString")
{
	const Line horizontal{ Vec2{ 0, 0 }, Vec2{ 10, 0 } };

	{
		const LineString lineString;

		CHECK(not Geometry2D::Intersects(horizontal, lineString));
		CHECK(not Geometry2D::Intersects(lineString, horizontal));
	}

	{
		const LineString lineString{ Vec2{ 5, 0 } };

		CHECK(Geometry2D::Intersects(horizontal, lineString));
		CHECK(Geometry2D::Intersects(lineString, horizontal));
	}

	{
		const LineString lineString{ Vec2{ 5, -5 }, Vec2{ 5, 5 }, Vec2{ 20, 5 } };

		CHECK(Geometry2D::Intersects(horizontal, lineString));
		CHECK(Geometry2D::Intersects(lineString, horizontal));
	}

	{
		const LineString lineString{ Vec2{ 2, 2 }, Vec2{ 2, 2 }, Vec2{ 6, 2 } };

		CHECK(not Geometry2D::Intersects(horizontal, lineString));
		CHECK(not Geometry2D::Intersects(lineString, horizontal));
	}
}

// Line と Rect / RectF は、内部通過、境界接触、empty、片側ゼロの線分矩形を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_Rect")
{
	{
		const RectF rect{ 0, 0, 10, 10 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -5, 5 }, Vec2{ 15, 5 } }, rect));
		CHECK(Geometry2D::Intersects(rect, Line{ Vec2{ -5, 5 }, Vec2{ 15, 5 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 2, 2 }, Vec2{ 8, 8 } }, rect));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } }, rect));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, -1 }, Vec2{ 0, 0 } }, rect));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -5, -1 }, Vec2{ 15, -1 } }, rect));
	}

	{
		const RectF empty{ 0, 0, 0, 0 };
		const RectF verticalSegment{ 0, 0, 0, 10 };
		const RectF horizontalSegment{ 0, 0, 10, 0 };

		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 5 }, Vec2{ 1, 5 } }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 11 }, Vec2{ 1, 11 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 5, -1 }, Vec2{ 5, 1 } }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 11, -1 }, Vec2{ 11, 1 } }, horizontalSegment));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };
		const Rect empty{ 0, 0, 0, 0 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -5, 5 }, Vec2{ 15, 5 } }, rect));
		CHECK(Geometry2D::Intersects(rect, Line{ Vec2{ -5, 5 }, Vec2{ 15, 5 } }));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }, empty));
	}
}

// Line と Circle / Ellipse / SuperEllipse は、内部通過、接線、empty、退化線分を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_RoundShapes")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Circle empty{ Vec2{ 0, 0 }, 0 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }, circle));
		CHECK(Geometry2D::Intersects(circle, Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -10, 5 }, Vec2{ 10, 5 } }, circle));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 1, 0 } }, circle));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -10, 6 }, Vec2{ 10, 6 } }, circle));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 6, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -10, 3 }, Vec2{ 10, 3 } }, ellipse));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -10, 4 }, Vec2{ 10, 4 } }, ellipse));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 2 }, Vec2{ 1, 2 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 2, -1 }, Vec2{ 2, 1 } }, horizontalSegment));
	}

	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 4.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -10, 4 }, Vec2{ 10, 4 } }, superEllipse));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 2 }, Vec2{ 1, 2 } }, verticalSegment));
	}
}

// Line と Triangle / Quad は、内部通過、境界接触、仕様で許可された単純な退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_PolygonalShapes")
{
	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Triangle segmentTriangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 5, 0 } };
		const Triangle pointTriangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 5 }, Vec2{ 6, 5 } }, triangle));
		CHECK(Geometry2D::Intersects(triangle, Line{ Vec2{ -1, 5 }, Vec2{ 6, 5 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } }, triangle));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 8, 8 }, Vec2{ 12, 12 } }, triangle));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 5, -1 }, Vec2{ 5, 1 } }, segmentTriangle));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 2, 4 }, Vec2{ 3, 4 } }, pointTriangle));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 2, 5 }, Vec2{ 3, 5 } }, pointTriangle));
	}

	{
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Quad segmentQuad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
		const Quad pointQuad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }, quad));
		CHECK(Geometry2D::Intersects(quad, Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } }, quad));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, -1 }, Vec2{ -2, -2 } }, quad));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 5, -1 }, Vec2{ 5, 1 } }, segmentQuad));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 2, 4 }, Vec2{ 3, 4 } }, pointQuad));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 2, 5 }, Vec2{ 3, 5 } }, pointQuad));
	}
}

// Line と Bezier2 / Bezier3 は、交差、端点接触、同一直線上の重なりを扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_Bezier")
{
	{
		const Bezier2 curve{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 5 }, Vec2{ 10, 5 } }, curve));
		CHECK(Geometry2D::Intersects(curve, Line{ Vec2{ 0, 5 }, Vec2{ 10, 5 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, curve));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 0, 11 }, Vec2{ 10, 11 } }, curve));
	}

	{
		const Bezier2 curve{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ 3, 0 }, Vec2{ 7, 0 } }, curve));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 11, 0 }, Vec2{ 12, 0 } }, curve));
	}

	{
		const Bezier3 curve{ Vec2{ 0, 0 }, Vec2{ 3, 9 }, Vec2{ 7, 9 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 5 }, Vec2{ 10, 5 } }, curve));
		CHECK(Geometry2D::Intersects(curve, Line{ Vec2{ 0, 5 }, Vec2{ 10, 5 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, curve));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 0, 10 }, Vec2{ 10, 10 } }, curve));
	}

	{
		const Bezier3 curve{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 7, 0 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ 3, 0 }, Vec2{ 7, 0 } }, curve));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 11, 0 }, Vec2{ 12, 0 } }, curve));
	}
}

// Line と RoundRect は、内部通過、境界接触、empty、RectF 退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Line_RoundRect")
{
	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 5, -1 }, Vec2{ 5, 1 } }, roundRect));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, -1 }, Vec2{ 1, 1 } }, roundRect));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 0, -1 } }, roundRect));
	}

	{
		const RoundRect rectEquivalent{ RectF{ 0, 0, 10, 10 }, 0 };
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 5 };
		const RoundRect verticalSegment{ RectF{ 0, 0, 0, 10 }, 5 };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, rectEquivalent));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ -1, 5 }, Vec2{ 1, 5 } }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 11 }, Vec2{ 1, 11 } }, verticalSegment));
	}
}

// 点と LineString は、空、単一点、連続線分、重複点を仕様通り扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_LineString")
{
	{
		const LineString lineString;

		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, lineString));
		CHECK(not Geometry2D::Intersects(Point{ 0, 0 }, lineString));
	}

	{
		const LineString lineString{ Vec2{ 1, 1 } };

		CHECK(Geometry2D::Intersects(Vec2{ 1, 1 }, lineString));
		CHECK(not Geometry2D::Intersects(Vec2{ 1, 2 }, lineString));
	}

	{
		const LineString lineString{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 } };

		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, lineString));
		CHECK(Geometry2D::Intersects(Vec2{ 10, 5 }, lineString));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 5 }, lineString));
	}

	{
		const LineString lineString{ Vec2{ 2, 2 }, Vec2{ 2, 2 }, Vec2{ 6, 2 } };

		CHECK(Geometry2D::Intersects(Vec2{ 2, 2 }, lineString));
		CHECK(Geometry2D::Intersects(Vec2{ 4, 2 }, lineString));
		CHECK(not Geometry2D::Intersects(Vec2{ 4, 3 }, lineString));
	}
}

// LineString と各 Shape は、closed segment union として既存の Line 判定に展開されることを確認する。
TEST_CASE("Geometry2D.Intersects.LineString_Shapes")
{
	{
		const LineString empty;
		const RectF rect{ 0, 0, 10, 10 };

		CHECK(not Geometry2D::Intersects(empty, rect));
		CHECK(not Geometry2D::Intersects(rect, empty));
	}

	{
		const LineString point{ Vec2{ 5, 5 } };
		const RectF rect{ 0, 0, 10, 10 };
		const Circle circle{ Vec2{ 0, 0 }, 5 };

		CHECK(Geometry2D::Intersects(point, rect));
		CHECK(Geometry2D::Intersects(rect, point));
		CHECK(not Geometry2D::Intersects(point, circle));
		CHECK(not Geometry2D::Intersects(circle, point));
	}

	{
		const LineString segments{ Vec2{ -5, 5 }, Vec2{ 5, 5 }, Vec2{ 20, 5 } };
		const Rect rect{ 0, 0, 10, 10 };
		const RectF rectF{ 0, 0, 10, 10 };
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Ellipse ellipse{ Vec2{ 0, 0 }, 6, 3 };
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 4.0 };
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(Geometry2D::Intersects(segments, rect));
		CHECK(Geometry2D::Intersects(rect, segments));
		CHECK(Geometry2D::Intersects(segments, rectF));
		CHECK(Geometry2D::Intersects(rectF, segments));
		CHECK(Geometry2D::Intersects(segments, circle));
		CHECK(Geometry2D::Intersects(circle, segments));
		CHECK(not Geometry2D::Intersects(segments, ellipse));
		CHECK(not Geometry2D::Intersects(ellipse, segments));
		CHECK(not Geometry2D::Intersects(segments, superEllipse));
		CHECK(not Geometry2D::Intersects(superEllipse, segments));
		CHECK(Geometry2D::Intersects(segments, triangle));
		CHECK(Geometry2D::Intersects(triangle, segments));
		CHECK(Geometry2D::Intersects(segments, quad));
		CHECK(Geometry2D::Intersects(quad, segments));
		CHECK(Geometry2D::Intersects(segments, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, segments));
	}

	{
		const LineString a{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 } };
		const LineString b{ Vec2{ 5, -5 }, Vec2{ 5, 5 } };
		const LineString c{ Vec2{ 20, 20 }, Vec2{ 30, 30 } };
		const LineString point{ Vec2{ 10, 5 } };

		CHECK(Geometry2D::Intersects(a, b));
		CHECK(Geometry2D::Intersects(b, a));
		CHECK(not Geometry2D::Intersects(a, c));
		CHECK(not Geometry2D::Intersects(c, a));
		CHECK(Geometry2D::Intersects(a, point));
		CHECK(Geometry2D::Intersects(point, a));
	}

	{
		const LineString segments{ Vec2{ 0, 5 }, Vec2{ 10, 5 } };
		const Bezier2 curve2{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } };
		const Bezier3 curve3{ Vec2{ 0, 0 }, Vec2{ 3, 9 }, Vec2{ 7, 9 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(segments, curve2));
		CHECK(Geometry2D::Intersects(curve2, segments));
		CHECK(Geometry2D::Intersects(segments, curve3));
		CHECK(Geometry2D::Intersects(curve3, segments));
	}

	{
		const LineString segments{ Vec2{ -1, 0 }, Vec2{ 1, 0 } };
		const Polygon polygon;
		const MultiPolygon multiPolygon;

		CHECK(not Geometry2D::Intersects(segments, polygon));
		CHECK(not Geometry2D::Intersects(polygon, segments));
		CHECK(not Geometry2D::Intersects(segments, multiPolygon));
		CHECK(not Geometry2D::Intersects(multiPolygon, segments));
	}
}

// 点と Bezier2 / Bezier3 は、端点、曲線上の点、点への退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_Bezier")
{
	{
		const Bezier2 curve{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(curve.p0, curve));
		CHECK(Geometry2D::Intersects(curve.pointAt(0.5), curve));
		CHECK(Geometry2D::Intersects(curve.p2, curve));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 6 }, curve));
	}

	{
		const Bezier2 curve{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };

		CHECK(Geometry2D::Intersects(Vec2{ 1, 1 }, curve));
		CHECK(not Geometry2D::Intersects(Vec2{ 1, 1.0001 }, curve));
	}

	{
		const Bezier3 curve{ Vec2{ 0, 0 }, Vec2{ 3, 9 }, Vec2{ 7, 9 }, Vec2{ 10, 0 } };

		CHECK(Geometry2D::Intersects(curve.p0, curve));
		CHECK(Geometry2D::Intersects(curve.pointAt(0.5), curve));
		CHECK(Geometry2D::Intersects(curve.p3, curve));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 8 }, curve));
	}

	{
		const Bezier3 curve{ Vec2{ 2, 3 }, Vec2{ 2, 3 }, Vec2{ 2, 3 }, Vec2{ 2, 3 } };

		CHECK(Geometry2D::Intersects(Vec2{ 2, 3 }, curve));
		CHECK(not Geometry2D::Intersects(Vec2{ 2, 3.0001 }, curve));
	}
}

// 点と Rect / RectF は、境界を含み、all-zero は empty、片側ゼロは線分として扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_Rect")
{
	{
		const RectF rect{ 0, 0, 10, 20 };

		CHECK(Geometry2D::Intersects(Vec2{ 0, 0 }, rect));
		CHECK(Geometry2D::Intersects(Vec2{ 10, 20 }, rect));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 10 }, rect));
		CHECK(not Geometry2D::Intersects(Vec2{ 10.1, 20 }, rect));
		CHECK(Geometry2D::Intersects(Point{ 10, 20 }, rect));
		CHECK(not Geometry2D::Intersects(Point{ 11, 20 }, rect));
	}

	{
		const RectF empty{ 0, 0, 0, 0 };
		const RectF verticalSegment{ 0, 0, 0, 10 };
		const RectF horizontalSegment{ 0, 0, 10, 0 };

		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, empty));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 5 }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 1, 5 }, verticalSegment));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 1 }, horizontalSegment));
	}

	{
		const Rect rect{ 0, 0, 10, 20 };
		const Rect empty{ 0, 0, 0, 0 };
		const Rect verticalSegment{ 0, 0, 0, 10 };
		const Rect horizontalSegment{ 0, 0, 10, 0 };

		CHECK(Geometry2D::Intersects(Vec2{ 10, 20 }, rect));
		CHECK(Geometry2D::Intersects(Point{ 10, 20 }, rect));
		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, empty));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 5 }, verticalSegment));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, horizontalSegment));
	}
}

// 点と Circle / Ellipse / SuperEllipse は、empty と退化線分、および境界を含むことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_RoundShapes")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Circle empty{ Vec2{ 0, 0 }, 0 };

		CHECK(Geometry2D::Intersects(Vec2{ 0, 0 }, circle));
		CHECK(Geometry2D::Intersects(Vec2{ 3, 4 }, circle));
		CHECK(not Geometry2D::Intersects(Vec2{ 6, 0 }, circle));
		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, empty));
		CHECK(Geometry2D::Intersects(Point{ 3, 4 }, circle));
		CHECK(not Geometry2D::Intersects(Point{ 6, 0 }, circle));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 6, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };

		CHECK(Geometry2D::Intersects(Vec2{ 6, 0 }, ellipse));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 3 }, ellipse));
		CHECK(not Geometry2D::Intersects(Vec2{ 6.1, 0 }, ellipse));
		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, empty));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 5 }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 1, 5 }, verticalSegment));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 1 }, horizontalSegment));
	}

	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 2.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 2.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 2.0 };
		const SuperEllipse horizontalSegment{ Vec2{ 0, 0 }, SizeF{ 5, 0 }, 2.0 };

		CHECK(Geometry2D::Intersects(Vec2{ 6, 0 }, superEllipse));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 3 }, superEllipse));
		CHECK(not Geometry2D::Intersects(Vec2{ 6.1, 0 }, superEllipse));
		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, empty));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 5 }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 1, 5 }, verticalSegment));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 1 }, horizontalSegment));
	}
}

// 点と Triangle / Quad は、境界を含み、仕様で許可された単純な退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_PolygonalShapes")
{
	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Triangle pointTriangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Vec2{ 2, 2 }, triangle));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, triangle));
		CHECK(not Geometry2D::Intersects(Vec2{ 8, 8 }, triangle));
		CHECK(Geometry2D::Intersects(Vec2{ 3, 4 }, pointTriangle));
		CHECK(not Geometry2D::Intersects(Vec2{ 3, 5 }, pointTriangle));
	}

	{
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Quad segmentQuad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
		const Quad pointQuad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Vec2{ 5, 5 }, quad));
		CHECK(Geometry2D::Intersects(Vec2{ 10, 5 }, quad));
		CHECK(not Geometry2D::Intersects(Vec2{ 11, 5 }, quad));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, segmentQuad));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 1 }, segmentQuad));
		CHECK(Geometry2D::Intersects(Vec2{ 3, 4 }, pointQuad));
		CHECK(not Geometry2D::Intersects(Vec2{ 3, 5 }, pointQuad));
	}
}

// 点と RoundRect は、empty、RectF 退化、角丸判定、NaN 入力の accidental accept 回帰を確認する。
TEST_CASE("Geometry2D.Intersects.Point_RoundRect")
{
	{
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 5 };
		const RoundRect verticalSegment{ RectF{ 0, 0, 0, 10 }, 5 };
		const RoundRect horizontalSegment{ RectF{ 0, 0, 10, 0 }, 5 };

		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, empty));
		CHECK(Geometry2D::Intersects(Vec2{ 0, 5 }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 1, 5 }, verticalSegment));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, 1 }, horizontalSegment));
	}

	{
		const RoundRect rectEquivalent{ RectF{ 0, 0, 10, 10 }, 0 };
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(Geometry2D::Intersects(Vec2{ 0, 0 }, rectEquivalent));
		CHECK(Geometry2D::Intersects(Vec2{ 10, 10 }, rectEquivalent));
		CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, roundRect));
		CHECK(Geometry2D::Intersects(Vec2{ 1, 1 }, roundRect));
		CHECK(Geometry2D::Intersects(Vec2{ 5, 0 }, roundRect));
		CHECK(Geometry2D::Intersects(Vec2{ 10, 5 }, roundRect));
		CHECK(not Geometry2D::Intersects(Vec2{ -0.1, 5 }, roundRect));
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(not Geometry2D::Intersects(Vec2{ QNaN<double>, 5 }, roundRect));
		CHECK(not Geometry2D::Intersects(Vec2{ 5, QNaN<double> }, roundRect));
	}
}

// 空の Polygon / MultiPolygon は、どの点とも交差しないことを確認する。
TEST_CASE("Geometry2D.Intersects.Point_EmptyPolygonContainers")
{
	const Polygon polygon;
	const MultiPolygon multiPolygon;

	CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, polygon));
	CHECK(not Geometry2D::Intersects(Point{ 0, 0 }, polygon));
	CHECK(not Geometry2D::Intersects(Vec2{ 0, 0 }, multiPolygon));
	CHECK(not Geometry2D::Intersects(Point{ 0, 0 }, multiPolygon));
	CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, polygon));
	CHECK(not Geometry2D::Intersects(polygon, Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }));
	CHECK(not Geometry2D::Intersects(Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }, multiPolygon));
	CHECK(not Geometry2D::Intersects(multiPolygon, Line{ Vec2{ -1, 0 }, Vec2{ 1, 0 } }));
}
