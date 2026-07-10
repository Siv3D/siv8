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


// Rect / RectF 同士は、閉じた矩形集合として境界接触、点接触、empty、片側ゼロの線分矩形を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Rect_Rect")
{
	static_assert(Geometry2D::Intersects(Rect{ 0, 0, 10, 10 }, Rect{ 10, 0, 5, 5 }));
	static_assert(Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, RectF{ 10, 10, 5, 5 }));
	static_assert(not Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, RectF{ 11, 0, 5, 5 }));
	static_assert(not Geometry2D::Intersects(RectF{ 0, 0, 0, 0 }, RectF{ 0, 0, 10, 10 }));

	{
		const RectF rect{ 0, 0, 10, 10 };

		CHECK(Geometry2D::Intersects(rect, RectF{ 5, 5, 10, 10 }));
		CHECK(Geometry2D::Intersects(RectF{ 5, 5, 10, 10 }, rect));
		CHECK(Geometry2D::Intersects(rect, RectF{ 10, 2, 5, 5 }));
		CHECK(Geometry2D::Intersects(rect, RectF{ 10, 10, 5, 5 }));
		CHECK(not Geometry2D::Intersects(rect, RectF{ 11, 0, 5, 5 }));
		CHECK(not Geometry2D::Intersects(RectF{ 0, 0, 0, 0 }, rect));
		CHECK(not Geometry2D::Intersects(rect, RectF{ 0, 0, 0, 0 }));
		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 12 }, rect));
		CHECK(Geometry2D::Intersects(rect, RectF{ 5, 10, 5, 0 }));
		CHECK(not Geometry2D::Intersects(RectF{ 11, 0, 0, 10 }, rect));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };

		CHECK(Geometry2D::Intersects(rect, Rect{ 5, 5, 10, 10 }));
		CHECK(Geometry2D::Intersects(Rect{ 5, 5, 10, 10 }, rect));
		CHECK(Geometry2D::Intersects(rect, Rect{ 10, 0, 5, 5 }));
		CHECK(not Geometry2D::Intersects(rect, Rect{ 11, 0, 5, 5 }));
		CHECK(not Geometry2D::Intersects(rect, Rect{ 0, 0, 0, 0 }));
		CHECK(Geometry2D::Intersects(rect, RectF{ 5, -1, 0, 12 }));
		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 12 }, rect));
	}
}

// Rect / RectF と Circle / Ellipse / SuperEllipse は、内部、境界接触、接線、empty、退化線分を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Rect_RoundShapes")
{
	static_assert(Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, Circle{ Vec2{ 15, 5 }, 5 }));
	static_assert(Geometry2D::Intersects(Circle{ Vec2{ 15, 5 }, 5 }, RectF{ 0, 0, 10, 10 }));
	static_assert(not Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, Circle{ Vec2{ 16, 5 }, 5 }));
	static_assert(Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, Ellipse{ Vec2{ 15, 5 }, 5, 2 }));
	static_assert(Geometry2D::Intersects(Ellipse{ Vec2{ 15, 5 }, 5, 2 }, RectF{ 0, 0, 10, 10 }));
	static_assert(not Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, Ellipse{ Vec2{ 16, 5 }, 5, 2 }));
	static_assert(not Geometry2D::Intersects(RectF{ 0, 0, 0, 0 }, Circle{ Vec2{ 0, 0 }, 5 }));

	{
		const RectF rect{ 0, 0, 10, 10 };
		const Circle circle{ Vec2{ 15, 5 }, 5 };
		const Circle inside{ Vec2{ 5, 5 }, 2 };
		const Circle empty{ Vec2{ 5, 5 }, 0 };

		CHECK(Geometry2D::Intersects(rect, circle));
		CHECK(Geometry2D::Intersects(circle, rect));
		CHECK(Geometry2D::Intersects(rect, inside));
		CHECK(Geometry2D::Intersects(inside, rect));
		CHECK(not Geometry2D::Intersects(rect, Circle{ Vec2{ 16, 5 }, 5 }));
		CHECK(not Geometry2D::Intersects(Circle{ Vec2{ 16, 5 }, 5 }, rect));
		CHECK(not Geometry2D::Intersects(rect, empty));
		CHECK(not Geometry2D::Intersects(empty, rect));
		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 2 }, Circle{ Vec2{ 0, 0 }, 5 }));
		CHECK(Geometry2D::Intersects(Circle{ Vec2{ 0, 0 }, 5 }, RectF{ 5, -1, 0, 2 }));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };

		CHECK(Geometry2D::Intersects(rect, Circle{ Vec2{ 15, 5 }, 5 }));
		CHECK(Geometry2D::Intersects(Circle{ Vec2{ 15, 5 }, 5 }, rect));
		CHECK(not Geometry2D::Intersects(rect, Circle{ Vec2{ 16, 5 }, 5 }));
		CHECK(Geometry2D::Intersects(rect, Ellipse{ Vec2{ 15, 5 }, 5, 2 }));
		CHECK(Geometry2D::Intersects(Ellipse{ Vec2{ 15, 5 }, 5, 2 }, rect));
		CHECK(not Geometry2D::Intersects(rect, Ellipse{ Vec2{ 16, 5 }, 5, 2 }));
	}

	{
		const RectF rect{ 0, 0, 10, 10 };
		const Ellipse ellipse{ Vec2{ 15, 5 }, 5, 2 };
		const Ellipse inside{ Vec2{ 5, 5 }, 2, 1 };
		const Ellipse empty{ Vec2{ 5, 5 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };

		CHECK(Geometry2D::Intersects(rect, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, rect));
		CHECK(Geometry2D::Intersects(rect, inside));
		CHECK(Geometry2D::Intersects(inside, rect));
		CHECK(not Geometry2D::Intersects(rect, Ellipse{ Vec2{ 16, 5 }, 5, 2 }));
		CHECK(not Geometry2D::Intersects(Ellipse{ Vec2{ 16, 5 }, 5, 2 }, rect));
		CHECK(not Geometry2D::Intersects(rect, empty));
		CHECK(not Geometry2D::Intersects(empty, rect));
		CHECK(Geometry2D::Intersects(RectF{ -1, 2, 2, 0 }, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, RectF{ -1, 2, 2, 0 }));
		CHECK(Geometry2D::Intersects(RectF{ 2, -1, 0, 2 }, horizontalSegment));
		CHECK(Geometry2D::Intersects(horizontalSegment, RectF{ 2, -1, 0, 2 }));
		CHECK(not Geometry2D::Intersects(RectF{ 6, -1, 0, 2 }, verticalSegment));
	}

	{
		const RectF rect{ 0, 0, 10, 10 };
		const SuperEllipse superEllipse{ Vec2{ 15, 5 }, SizeF{ 5, 2 }, 4.0 };
		const SuperEllipse inside{ Vec2{ 5, 5 }, SizeF{ 2, 1 }, 4.0 };
		const SuperEllipse empty{ Vec2{ 5, 5 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };
		const SuperEllipse horizontalSegment{ Vec2{ 0, 0 }, SizeF{ 5, 0 }, 4.0 };

		CHECK(Geometry2D::Intersects(rect, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, rect));
		CHECK(Geometry2D::Intersects(rect, SuperEllipse{ Vec2{ 15, 5 }, SizeF{ 5, 2 }, 2.0 }));
		CHECK(Geometry2D::Intersects(SuperEllipse{ Vec2{ 15, 5 }, SizeF{ 5, 2 }, 2.0 }, rect));
		CHECK(Geometry2D::Intersects(rect, inside));
		CHECK(Geometry2D::Intersects(inside, rect));
		CHECK(not Geometry2D::Intersects(rect, SuperEllipse{ Vec2{ 16, 5 }, SizeF{ 5, 2 }, 4.0 }));
		CHECK(not Geometry2D::Intersects(SuperEllipse{ Vec2{ 16, 5 }, SizeF{ 5, 2 }, 4.0 }, rect));
		CHECK(not Geometry2D::Intersects(rect, empty));
		CHECK(not Geometry2D::Intersects(empty, rect));
		CHECK(Geometry2D::Intersects(RectF{ -1, 2, 2, 0 }, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, RectF{ -1, 2, 2, 0 }));
		CHECK(Geometry2D::Intersects(RectF{ 2, -1, 0, 2 }, horizontalSegment));
		CHECK(Geometry2D::Intersects(horizontalSegment, RectF{ 2, -1, 0, 2 }));
		CHECK(Geometry2D::Intersects(Rect{ 0, 0, 10, 10 }, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, Rect{ 0, 0, 10, 10 }));
	}
}


// Rect / RectF と RoundRect は、内部、境界接触、empty、片側ゼロ形状を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Rect_RoundRect")
{
	{
		const RectF rect{ 0, 0, 10, 10 };
		const RoundRect roundRect{ RectF{ 8, 2, 10, 6 }, 2 };
		const RoundRect inside{ RectF{ 2, 2, 4, 4 }, 1 };
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 2 };
		const RoundRect verticalSegment{ RectF{ 10, 0, 0, 10 }, 2 };
		const RoundRect squareCorners{ RectF{ 10, 2, 5, 5 }, 0 };

		CHECK(Geometry2D::Intersects(rect, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, rect));
		CHECK(Geometry2D::Intersects(rect, inside));
		CHECK(Geometry2D::Intersects(inside, rect));
		CHECK(Geometry2D::Intersects(rect, squareCorners));
		CHECK(Geometry2D::Intersects(squareCorners, rect));
		CHECK(Geometry2D::Intersects(rect, verticalSegment));
		CHECK(Geometry2D::Intersects(verticalSegment, rect));
		CHECK(not Geometry2D::Intersects(rect, RoundRect{ RectF{ 11, 11, 4, 4 }, 2 }));
		CHECK(not Geometry2D::Intersects(RoundRect{ RectF{ 11, 11, 4, 4 }, 2 }, rect));
		CHECK(not Geometry2D::Intersects(rect, empty));
		CHECK(not Geometry2D::Intersects(empty, rect));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };

		CHECK(Geometry2D::Intersects(rect, RoundRect{ RectF{ 8, 2, 10, 6 }, 2 }));
		CHECK(Geometry2D::Intersects(RoundRect{ RectF{ 8, 2, 10, 6 }, 2 }, rect));
		CHECK(not Geometry2D::Intersects(rect, RoundRect{ RectF{ 11, 11, 4, 4 }, 2 }));
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 2 }, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, RectF{ 5, -1, 0, 2 }));
		CHECK(not Geometry2D::Intersects(RectF{ -1, -1, 0.5, 0.5 }, roundRect));
	}
}

// Rect / RectF と Polygon / MultiPolygon は、包含、境界接触、empty container、片側ゼロ RectF を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Rect_PolygonContainers")
{
	const Polygon polygon{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };
	const MultiPolygon multiPolygon{ polygon };
	const Polygon emptyPolygon;
	const MultiPolygon emptyMultiPolygon;

	{
		const RectF rect{ 5, 5, 10, 10 };

		CHECK(Geometry2D::Intersects(rect, polygon));
		CHECK(Geometry2D::Intersects(polygon, rect));
		CHECK(Geometry2D::Intersects(rect, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, rect));
	}

	{
		CHECK(Geometry2D::Intersects(RectF{ 10, 2, 5, 5 }, polygon));
		CHECK(Geometry2D::Intersects(polygon, RectF{ 10, 2, 5, 5 }));
		CHECK(Geometry2D::Intersects(RectF{ 10, 10, 5, 5 }, polygon));
		CHECK(not Geometry2D::Intersects(RectF{ 11, 11, 2, 2 }, polygon));
		CHECK(not Geometry2D::Intersects(polygon, RectF{ 11, 11, 2, 2 }));
	}

	{
		CHECK(Geometry2D::Intersects(Rect{ 5, 5, 10, 10 }, polygon));
		CHECK(Geometry2D::Intersects(polygon, Rect{ 5, 5, 10, 10 }));
		CHECK(Geometry2D::Intersects(Rect{ 5, 5, 10, 10 }, multiPolygon));
		CHECK(Geometry2D::Intersects(multiPolygon, Rect{ 5, 5, 10, 10 }));
		CHECK(not Geometry2D::Intersects(Rect{ 11, 11, 2, 2 }, polygon));
	}

	{
		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 3 }, polygon));
		CHECK(Geometry2D::Intersects(polygon, RectF{ 5, -1, 0, 3 }));
		CHECK(not Geometry2D::Intersects(RectF{ 11, -1, 0, 3 }, polygon));
	}

	{
		CHECK(not Geometry2D::Intersects(RectF{ 0, 0, 0, 0 }, polygon));
		CHECK(not Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, emptyPolygon));
		CHECK(not Geometry2D::Intersects(emptyPolygon, RectF{ 0, 0, 10, 10 }));
		CHECK(not Geometry2D::Intersects(RectF{ 0, 0, 10, 10 }, emptyMultiPolygon));
		CHECK(not Geometry2D::Intersects(emptyMultiPolygon, RectF{ 0, 0, 10, 10 }));
	}
}

// Rect / RectF と Triangle / Quad は、内部、境界接触、点接触、退化 polygonal shape、片側ゼロ RectF を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Rect_PolygonalShapes")
{
	{
		const RectF rect{ 0, 0, 10, 10 };
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };

		CHECK(Geometry2D::Intersects(rect, triangle));
		CHECK(Geometry2D::Intersects(triangle, rect));
		CHECK(Geometry2D::Intersects(RectF{ 10, 0, 5, 5 }, triangle));
		CHECK(Geometry2D::Intersects(triangle, RectF{ 10, 0, 5, 5 }));
		CHECK(Geometry2D::Intersects(RectF{ 10, -5, 5, 5 }, triangle));
		CHECK(not Geometry2D::Intersects(RectF{ 8, 8, 3, 3 }, triangle));
		CHECK(not Geometry2D::Intersects(RectF{ 0, 0, 0, 0 }, triangle));
		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 3 }, triangle));
		CHECK(Geometry2D::Intersects(triangle, RectF{ 5, -1, 0, 3 }));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };

		CHECK(Geometry2D::Intersects(rect, triangle));
		CHECK(Geometry2D::Intersects(triangle, rect));
		CHECK(not Geometry2D::Intersects(Rect{ 8, 8, 3, 3 }, triangle));
	}

	{
		const Triangle segmentTriangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 5, 0 } };
		const Triangle pointTriangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 2, 2 }, segmentTriangle));
		CHECK(Geometry2D::Intersects(segmentTriangle, RectF{ 5, -1, 2, 2 }));
		CHECK(not Geometry2D::Intersects(RectF{ 5, 1, 2, 2 }, segmentTriangle));
		CHECK(Geometry2D::Intersects(RectF{ 2, 3, 3, 3 }, pointTriangle));
		CHECK(Geometry2D::Intersects(pointTriangle, RectF{ 2, 3, 3, 3 }));
		CHECK(not Geometry2D::Intersects(RectF{ 2, 5, 3, 3 }, pointTriangle));
	}

	{
		const RectF rect{ 0, 0, 10, 10 };
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };

		CHECK(Geometry2D::Intersects(rect, quad));
		CHECK(Geometry2D::Intersects(quad, rect));
		CHECK(Geometry2D::Intersects(RectF{ 10, 2, 5, 5 }, quad));
		CHECK(Geometry2D::Intersects(quad, RectF{ 10, 2, 5, 5 }));
		CHECK(Geometry2D::Intersects(RectF{ 10, 10, 5, 5 }, quad));
		CHECK(not Geometry2D::Intersects(RectF{ 11, 11, 2, 2 }, quad));
		CHECK(not Geometry2D::Intersects(RectF{ 0, 0, 0, 0 }, quad));
		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 3 }, quad));
		CHECK(Geometry2D::Intersects(quad, RectF{ 5, -1, 0, 3 }));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };

		CHECK(Geometry2D::Intersects(rect, quad));
		CHECK(Geometry2D::Intersects(quad, rect));
		CHECK(not Geometry2D::Intersects(Rect{ 11, 11, 2, 2 }, quad));
	}

	{
		const Quad segmentQuad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
		const Quad pointQuad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(RectF{ 5, -1, 0, 2 }, segmentQuad));
		CHECK(Geometry2D::Intersects(segmentQuad, RectF{ 5, -1, 0, 2 }));
		CHECK(not Geometry2D::Intersects(RectF{ 5, 1, 0, 2 }, segmentQuad));
		CHECK(Geometry2D::Intersects(RectF{ 2, 4, 2, 0 }, pointQuad));
		CHECK(Geometry2D::Intersects(pointQuad, RectF{ 2, 4, 2, 0 }));
		CHECK(not Geometry2D::Intersects(RectF{ 2, 5, 2, 0 }, pointQuad));
	}
}

// Triangle / Quad 同士は、内部、境界接触、点接触、退化 point / segment を閉じた点集合として扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.PolygonalShapes_PolygonalShapes")
{
	static_assert(Geometry2D::Intersects(
		Triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } },
		Triangle{ Vec2{ 5, -1 }, Vec2{ 11, -1 }, Vec2{ 5, 5 } }));

	static_assert(not Geometry2D::Intersects(
		Triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } },
		Triangle{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 11, 20 } }));

	static_assert(Geometry2D::Intersects(
		Quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } },
		Quad{ Vec2{ 10, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 10 }, Vec2{ 10, 10 } }));

	static_assert(not Geometry2D::Intersects(
		Quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } },
		Quad{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 20, 20 }, Vec2{ 11, 20 } }));

	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };

		CHECK(Geometry2D::Intersects(triangle, Triangle{ Vec2{ 5, -1 }, Vec2{ 11, -1 }, Vec2{ 5, 5 } }));
		CHECK(Geometry2D::Intersects(Triangle{ Vec2{ 5, -1 }, Vec2{ 11, -1 }, Vec2{ 5, 5 } }, triangle));
		CHECK(Geometry2D::Intersects(triangle, Triangle{ Vec2{ 10, 0 }, Vec2{ 15, -5 }, Vec2{ 15, 5 } }));
		CHECK(Geometry2D::Intersects(triangle, Triangle{ Vec2{ 5, -1 }, Vec2{ 5, 11 }, Vec2{ 5, 5 } }));
		CHECK(Geometry2D::Intersects(triangle, Triangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } }));
		CHECK(not Geometry2D::Intersects(triangle, Triangle{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 11, 20 } }));
		CHECK(not Geometry2D::Intersects(triangle, Triangle{ Vec2{ 11, -1 }, Vec2{ 11, 11 }, Vec2{ 11, 5 } }));
		CHECK(not Geometry2D::Intersects(triangle, Triangle{ Vec2{ 8, 8 }, Vec2{ 8, 8 }, Vec2{ 8, 8 } }));
	}

	{
		const Triangle triangle{ Vec2{ 2, 2 }, Vec2{ 8, 2 }, Vec2{ 5, 8 } };
		const Triangle bigTriangle{ Vec2{ -1, -1 }, Vec2{ 20, -1 }, Vec2{ -1, 20 } };
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Quad innerQuad{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } };

		CHECK(Geometry2D::Intersects(triangle, quad));
		CHECK(Geometry2D::Intersects(quad, triangle));
		CHECK(Geometry2D::Intersects(bigTriangle, innerQuad));
		CHECK(Geometry2D::Intersects(innerQuad, bigTriangle));
		CHECK(Geometry2D::Intersects(Triangle{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ -1, 6 } }, quad));
		CHECK(Geometry2D::Intersects(quad, Triangle{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ -1, 6 } }));
		CHECK(Geometry2D::Intersects(Triangle{ Vec2{ 10, 10 }, Vec2{ 15, 10 }, Vec2{ 10, 15 } }, quad));
		CHECK(not Geometry2D::Intersects(Triangle{ Vec2{ 11, 11 }, Vec2{ 12, 11 }, Vec2{ 11, 12 } }, quad));
	}

	{
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };

		CHECK(Geometry2D::Intersects(quad, Quad{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } }));
		CHECK(Geometry2D::Intersects(Quad{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } }, quad));
		CHECK(Geometry2D::Intersects(quad, Quad{ Vec2{ 10, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 10 }, Vec2{ 10, 10 } }));
		CHECK(Geometry2D::Intersects(quad, Quad{ Vec2{ 10, 10 }, Vec2{ 20, 10 }, Vec2{ 20, 20 }, Vec2{ 10, 20 } }));
		CHECK(Geometry2D::Intersects(quad, Quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } }));
		CHECK(Geometry2D::Intersects(quad, Quad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } }));
		CHECK(not Geometry2D::Intersects(quad, Quad{ Vec2{ 11, 11 }, Vec2{ 20, 11 }, Vec2{ 20, 20 }, Vec2{ 11, 20 } }));
		CHECK(not Geometry2D::Intersects(quad, Quad{ Vec2{ 11, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 0 }, Vec2{ 11, 0 } }));
		CHECK(not Geometry2D::Intersects(quad, Quad{ Vec2{ 11, 11 }, Vec2{ 11, 11 }, Vec2{ 11, 11 }, Vec2{ 11, 11 } }));
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

	// Bezier2 が直線に退化し、交点が線分境界で丸められるケースの regression。
	{
		const Bezier2 curve{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ 11, 5 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 0, 10 } }, curve));
		CHECK(Geometry2D::Intersects(curve, Line{ Vec2{ 0, 0 }, Vec2{ 0, 10 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 10, 0 }, Vec2{ 10, 10 } }, curve));
		CHECK(Geometry2D::Intersects(curve, Line{ Vec2{ 10, 0 }, Vec2{ 10, 10 } }));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 12, 0 }, Vec2{ 12, 10 } }, curve));
		CHECK(not Geometry2D::Intersects(curve, Line{ Vec2{ 12, 0 }, Vec2{ 12, 10 } }));
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

	// Bezier3 が直線に退化し、交点が線分境界で丸められるケースの regression。
	{
		const Bezier3 curve{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } };

		CHECK(Geometry2D::Intersects(Line{ Vec2{ 0, 0 }, Vec2{ 0, 10 } }, curve));
		CHECK(Geometry2D::Intersects(curve, Line{ Vec2{ 0, 0 }, Vec2{ 0, 10 } }));
		CHECK(Geometry2D::Intersects(Line{ Vec2{ 10, 0 }, Vec2{ 10, 10 } }, curve));
		CHECK(Geometry2D::Intersects(curve, Line{ Vec2{ 10, 0 }, Vec2{ 10, 10 } }));
		CHECK(not Geometry2D::Intersects(Line{ Vec2{ 12, 0 }, Vec2{ 12, 10 } }, curve));
		CHECK(not Geometry2D::Intersects(curve, Line{ Vec2{ 12, 0 }, Vec2{ 12, 10 } }));
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


// Bezier2 と Rect / RectF は、内部通過、境界接触、empty、片側ゼロの線分矩形を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_Rect")
{
	{
		const RectF rect{ 0, 0, 10, 10 };
		const Bezier2 crossing{ Vec2{ -5, 5 }, Vec2{ 5, 5 }, Vec2{ 15, 5 } };
		const Bezier2 inside{ Vec2{ 2, 2 }, Vec2{ 5, 4 }, Vec2{ 8, 2 } };
		const Bezier2 outside{ Vec2{ -5, -1 }, Vec2{ 5, -1 }, Vec2{ 15, -1 } };

		CHECK(Geometry2D::Intersects(crossing, rect));
		CHECK(Geometry2D::Intersects(rect, crossing));
		CHECK(Geometry2D::Intersects(inside, rect));
		CHECK(not Geometry2D::Intersects(outside, rect));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };
		const RectF empty{ 0, 0, 0, 0 };
		const RectF verticalSegment{ 0, 0, 0, 10 };
		const RectF horizontalSegment{ 0, 0, 10, 0 };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -5, 5 }, Vec2{ 5, 5 }, Vec2{ 15, 5 } }, rect));
		CHECK(Geometry2D::Intersects(rect, Bezier2{ Vec2{ -5, 5 }, Vec2{ 5, 5 }, Vec2{ 15, 5 } }));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 5 }, Vec2{ 0, 5 }, Vec2{ 1, 5 } }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -1, 11 }, Vec2{ 0, 11 }, Vec2{ 1, 11 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 5, -1 }, Vec2{ 5, 0 }, Vec2{ 5, 1 } }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 11, -1 }, Vec2{ 11, 0 }, Vec2{ 11, 1 } }, horizontalSegment));
	}
}


// Bezier3 と Rect / RectF は、内部通過、境界接触、empty、片側ゼロの線分矩形を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_Rect")
{
	{
		const RectF rect{ 0, 0, 10, 10 };
		const Bezier3 crossing{ Vec2{ -5, 5 }, Vec2{ 0, 5 }, Vec2{ 10, 5 }, Vec2{ 15, 5 } };
		const Bezier3 inside{ Vec2{ 2, 2 }, Vec2{ 4, 4 }, Vec2{ 6, 4 }, Vec2{ 8, 2 } };
		const Bezier3 outside{ Vec2{ -5, -1 }, Vec2{ 0, -1 }, Vec2{ 10, -1 }, Vec2{ 15, -1 } };

		CHECK(Geometry2D::Intersects(crossing, rect));
		CHECK(Geometry2D::Intersects(rect, crossing));
		CHECK(Geometry2D::Intersects(inside, rect));
		CHECK(not Geometry2D::Intersects(outside, rect));
	}

	{
		const Rect rect{ 0, 0, 10, 10 };
		const RectF empty{ 0, 0, 0, 0 };
		const RectF verticalSegment{ 0, 0, 0, 10 };
		const RectF horizontalSegment{ 0, 0, 10, 0 };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -5, 5 }, Vec2{ 0, 5 }, Vec2{ 10, 5 }, Vec2{ 15, 5 } }, rect));
		CHECK(Geometry2D::Intersects(rect, Bezier3{ Vec2{ -5, 5 }, Vec2{ 0, 5 }, Vec2{ 10, 5 }, Vec2{ 15, 5 } }));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 5 }, Vec2{ 0, 5 }, Vec2{ 0, 5 }, Vec2{ 1, 5 } }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -1, 11 }, Vec2{ 0, 11 }, Vec2{ 0, 11 }, Vec2{ 1, 11 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 5, -1 }, Vec2{ 5, 0 }, Vec2{ 5, 0 }, Vec2{ 5, 1 } }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 11, -1 }, Vec2{ 11, 0 }, Vec2{ 11, 0 }, Vec2{ 11, 1 } }, horizontalSegment));
	}
}

// Bezier2 と Circle / Ellipse は、最短距離判定、接線、empty、Ellipse 退化線分を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_RoundShapes")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Circle empty{ Vec2{ 0, 0 }, 0 };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -10, 0 }, Vec2{ 0, 0 }, Vec2{ 10, 0 } }, circle));
		CHECK(Geometry2D::Intersects(circle, Bezier2{ Vec2{ -10, 0 }, Vec2{ 0, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -10, 5 }, Vec2{ 0, 5 }, Vec2{ 10, 5 } }, circle));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 0 } }, circle));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } }, circle));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -10, 6 }, Vec2{ 0, 6 }, Vec2{ 10, 6 } }, circle));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } }, empty));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 6, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -10, 0 }, Vec2{ 0, 0 }, Vec2{ 10, 0 } }, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, Bezier2{ Vec2{ -10, 0 }, Vec2{ 0, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -10, 3 }, Vec2{ 0, 3 }, Vec2{ 10, 3 } }, ellipse));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -10, 4 }, Vec2{ 0, 4 }, Vec2{ 10, 4 } }, ellipse));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 2 }, Vec2{ 0, 2 }, Vec2{ 1, 2 } }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -1, 6 }, Vec2{ 0, 6 }, Vec2{ 1, 6 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 2, -1 }, Vec2{ 2, 0 }, Vec2{ 2, 1 } }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 6, -1 }, Vec2{ 6, 0 }, Vec2{ 6, 1 } }, horizontalSegment));
	}
}


// Bezier3 と Circle / Ellipse は、最短距離判定、接線、empty、Ellipse 退化線分を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_RoundShapes")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Circle empty{ Vec2{ 0, 0 }, 0 };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -10, 0 }, Vec2{ -3, 0 }, Vec2{ 3, 0 }, Vec2{ 10, 0 } }, circle));
		CHECK(Geometry2D::Intersects(circle, Bezier3{ Vec2{ -10, 0 }, Vec2{ -3, 0 }, Vec2{ 3, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -10, 5 }, Vec2{ -3, 5 }, Vec2{ 3, 5 }, Vec2{ 10, 5 } }, circle));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 0 }, Vec2{ 0, 1 }, Vec2{ 0, 1 }, Vec2{ 1, 0 } }, circle));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } }, circle));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -10, 6 }, Vec2{ -3, 6 }, Vec2{ 3, 6 }, Vec2{ 10, 6 } }, circle));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } }, empty));
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 6, 3 };
		const Ellipse empty{ Vec2{ 0, 0 }, 0, 0 };
		const Ellipse verticalSegment{ Vec2{ 0, 0 }, 0, 5 };
		const Ellipse horizontalSegment{ Vec2{ 0, 0 }, 5, 0 };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -10, 0 }, Vec2{ -3, 0 }, Vec2{ 3, 0 }, Vec2{ 10, 0 } }, ellipse));
		CHECK(Geometry2D::Intersects(ellipse, Bezier3{ Vec2{ -10, 0 }, Vec2{ -3, 0 }, Vec2{ 3, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -10, 3 }, Vec2{ -3, 3 }, Vec2{ 3, 3 }, Vec2{ 10, 3 } }, ellipse));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -10, 4 }, Vec2{ -3, 4 }, Vec2{ 3, 4 }, Vec2{ 10, 4 } }, ellipse));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 2 }, Vec2{ 0, 2 }, Vec2{ 0, 2 }, Vec2{ 1, 2 } }, verticalSegment));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -1, 6 }, Vec2{ 0, 6 }, Vec2{ 0, 6 }, Vec2{ 1, 6 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 2, -1 }, Vec2{ 2, 0 }, Vec2{ 2, 0 }, Vec2{ 2, 1 } }, horizontalSegment));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 6, -1 }, Vec2{ 6, 0 }, Vec2{ 6, 0 }, Vec2{ 6, 1 } }, horizontalSegment));
	}
}

// Bezier2 と Triangle / Quad は、内部通過、境界接触、仕様で許可された単純な退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_PolygonalShapes")
{
	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Triangle segmentTriangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 5, 0 } };
		const Triangle pointTriangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 5 }, Vec2{ 2, 5 }, Vec2{ 6, 5 } }, triangle));
		CHECK(Geometry2D::Intersects(triangle, Bezier2{ Vec2{ -1, 5 }, Vec2{ 2, 5 }, Vec2{ 6, 5 } }));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } }, triangle));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 8, 8 }, Vec2{ 10, 10 }, Vec2{ 12, 12 } }, triangle));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 5, -1 }, Vec2{ 5, 0 }, Vec2{ 5, 1 } }, segmentTriangle));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 2, 4 }, Vec2{ 3, 4 }, Vec2{ 4, 4 } }, pointTriangle));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 2, 5 }, Vec2{ 3, 5 }, Vec2{ 4, 5 } }, pointTriangle));
	}

	{
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Quad segmentQuad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
		const Quad pointQuad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ 11, 5 } }, quad));
		CHECK(Geometry2D::Intersects(quad, Bezier2{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ 11, 5 } }));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } }, quad));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -1, -1 }, Vec2{ -2, -2 }, Vec2{ -3, -3 } }, quad));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 5, -1 }, Vec2{ 5, 0 }, Vec2{ 5, 1 } }, segmentQuad));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 2, 4 }, Vec2{ 3, 4 }, Vec2{ 4, 4 } }, pointQuad));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 2, 5 }, Vec2{ 3, 5 }, Vec2{ 4, 5 } }, pointQuad));
	}
}


// Bezier3 と Triangle / Quad は、内部通過、境界接触、仕様で許可された単純な退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_PolygonalShapes")
{
	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Triangle segmentTriangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 5, 0 } };
		const Triangle pointTriangle{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 5 }, Vec2{ 1, 5 }, Vec2{ 3, 5 }, Vec2{ 6, 5 } }, triangle));
		CHECK(Geometry2D::Intersects(triangle, Bezier3{ Vec2{ -1, 5 }, Vec2{ 1, 5 }, Vec2{ 3, 5 }, Vec2{ 6, 5 } }));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 7, 0 }, Vec2{ 10, 0 } }, triangle));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 8, 8 }, Vec2{ 9, 9 }, Vec2{ 11, 11 }, Vec2{ 12, 12 } }, triangle));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 5, -1 }, Vec2{ 5, 0 }, Vec2{ 5, 0 }, Vec2{ 5, 1 } }, segmentTriangle));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 2, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 4, 4 } }, pointTriangle));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 2, 5 }, Vec2{ 3, 5 }, Vec2{ 3, 5 }, Vec2{ 4, 5 } }, pointTriangle));
	}

	{
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Quad segmentQuad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
		const Quad pointQuad{ Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 } };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } }, quad));
		CHECK(Geometry2D::Intersects(quad, Bezier3{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } }));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 7, 0 }, Vec2{ 10, 0 } }, quad));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -1, -1 }, Vec2{ -2, -2 }, Vec2{ -2, -2 }, Vec2{ -3, -3 } }, quad));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 5, -1 }, Vec2{ 5, 0 }, Vec2{ 5, 0 }, Vec2{ 5, 1 } }, segmentQuad));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 2, 4 }, Vec2{ 3, 4 }, Vec2{ 3, 4 }, Vec2{ 4, 4 } }, pointQuad));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 2, 5 }, Vec2{ 3, 5 }, Vec2{ 3, 5 }, Vec2{ 4, 5 } }, pointQuad));
	}
}

// 空の Polygon / MultiPolygon は、Bezier2 と交差しないことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_EmptyPolygonContainers")
{
	const Bezier2 curve{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } };
	const Polygon polygon;
	const MultiPolygon multiPolygon;

	CHECK(not Geometry2D::Intersects(curve, polygon));
	CHECK(not Geometry2D::Intersects(polygon, curve));
	CHECK(not Geometry2D::Intersects(curve, multiPolygon));
	CHECK(not Geometry2D::Intersects(multiPolygon, curve));
}


// 空の Polygon / MultiPolygon は、Bezier3 と交差しないことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_EmptyPolygonContainers")
{
	const Bezier3 curve{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } };
	const Polygon polygon;
	const MultiPolygon multiPolygon;

	CHECK(not Geometry2D::Intersects(curve, polygon));
	CHECK(not Geometry2D::Intersects(polygon, curve));
	CHECK(not Geometry2D::Intersects(curve, multiPolygon));
	CHECK(not Geometry2D::Intersects(multiPolygon, curve));
}


// Bezier2 と SuperEllipse は、n == 2 の Ellipse 委譲、退化線分、近似判定を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_SuperEllipse")
{
	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 4.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -10, 0 }, Vec2{ 0, 0 }, Vec2{ 10, 0 } }, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, Bezier2{ Vec2{ -10, 0 }, Vec2{ 0, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 1, 1 }, Vec2{ 2, 1 }, Vec2{ 3, 1 } }, superEllipse));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -10, 4 }, Vec2{ 0, 4 }, Vec2{ 10, 4 } }, superEllipse));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 2 }, Vec2{ 0, 2 }, Vec2{ 1, 2 } }, verticalSegment));
	}

	{
		const SuperEllipse ellipseEquivalent{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 2.0 };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -10, 3 }, Vec2{ 0, 3 }, Vec2{ 10, 3 } }, ellipseEquivalent));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -10, 4 }, Vec2{ 0, 4 }, Vec2{ 10, 4 } }, ellipseEquivalent));
	}
}


// Bezier3 と SuperEllipse は、n == 2 の Ellipse 委譲、退化線分、近似判定を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_SuperEllipse")
{
	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 4.0 };
		const SuperEllipse empty{ Vec2{ 0, 0 }, SizeF{ 0, 0 }, 4.0 };
		const SuperEllipse verticalSegment{ Vec2{ 0, 0 }, SizeF{ 0, 5 }, 4.0 };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -10, 0 }, Vec2{ -3, 0 }, Vec2{ 3, 0 }, Vec2{ 10, 0 } }, superEllipse));
		CHECK(Geometry2D::Intersects(superEllipse, Bezier3{ Vec2{ -10, 0 }, Vec2{ -3, 0 }, Vec2{ 3, 0 }, Vec2{ 10, 0 } }));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 1, 1 }, Vec2{ 2, 1 }, Vec2{ 2, 1 }, Vec2{ 3, 1 } }, superEllipse));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -10, 4 }, Vec2{ -3, 4 }, Vec2{ 3, 4 }, Vec2{ 10, 4 } }, superEllipse));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 2 }, Vec2{ 0, 2 }, Vec2{ 0, 2 }, Vec2{ 1, 2 } }, verticalSegment));
	}

	{
		const SuperEllipse ellipseEquivalent{ Vec2{ 0, 0 }, SizeF{ 6, 3 }, 2.0 };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -10, 3 }, Vec2{ -3, 3 }, Vec2{ 3, 3 }, Vec2{ 10, 3 } }, ellipseEquivalent));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -10, 4 }, Vec2{ -3, 4 }, Vec2{ 3, 4 }, Vec2{ 10, 4 } }, ellipseEquivalent));
	}
}

// Bezier2 と RoundRect は、内部通過、境界接触、empty、RectF 退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_RoundRect")
{
	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ 11, 5 } }, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, Bezier2{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ 11, 5 } }));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 2, 2 }, Vec2{ 5, 5 }, Vec2{ 8, 8 } }, roundRect));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ 0, 5 }, Vec2{ 0, 5 }, Vec2{ 0, 5 } }, roundRect));
		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -5, -1 }, Vec2{ 5, -1 }, Vec2{ 15, -1 } }, roundRect));
	}

	{
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 2 };
		const RoundRect verticalSegment{ RectF{ 0, 0, 0, 10 }, 2 };
		const RoundRect squareCorners{ RectF{ 0, 0, 10, 10 }, 0 };

		CHECK(not Geometry2D::Intersects(Bezier2{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 5 }, Vec2{ 0, 5 }, Vec2{ 1, 5 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Bezier2{ Vec2{ -1, 5 }, Vec2{ 5, 5 }, Vec2{ 11, 5 } }, squareCorners));
	}
}


// Bezier3 と RoundRect は、内部通過、境界接触、empty、RectF 退化を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_RoundRect")
{
	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2 };

		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } }, roundRect));
		CHECK(Geometry2D::Intersects(roundRect, Bezier3{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } }));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 2, 2 }, Vec2{ 4, 4 }, Vec2{ 6, 6 }, Vec2{ 8, 8 } }, roundRect));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ 0, 5 }, Vec2{ 0, 5 }, Vec2{ 0, 5 }, Vec2{ 0, 5 } }, roundRect));
		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -5, -1 }, Vec2{ 0, -1 }, Vec2{ 10, -1 }, Vec2{ 15, -1 } }, roundRect));
	}

	{
		const RoundRect empty{ RectF{ 0, 0, 0, 0 }, 2 };
		const RoundRect verticalSegment{ RectF{ 0, 0, 0, 10 }, 2 };
		const RoundRect squareCorners{ RectF{ 0, 0, 10, 10 }, 0 };

		CHECK(not Geometry2D::Intersects(Bezier3{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } }, empty));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 5 }, Vec2{ 0, 5 }, Vec2{ 0, 5 }, Vec2{ 1, 5 } }, verticalSegment));
		CHECK(Geometry2D::Intersects(Bezier3{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } }, squareCorners));
	}
}

// Bezier2 と Bezier2 / Bezier3 は、近似線分化と既存 Line x Bezier kernel による交差判定を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier2_Curves")
{
	{
		const Bezier2 arch{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } };
		const Bezier2 vertical{ Vec2{ 5, -1 }, Vec2{ 5, 5 }, Vec2{ 5, 11 } };
		const Bezier2 outside{ Vec2{ 20, -1 }, Vec2{ 20, 5 }, Vec2{ 20, 11 } };

		CHECK(Geometry2D::Intersects(arch, vertical));
		CHECK(Geometry2D::Intersects(vertical, arch));
		CHECK(Geometry2D::Intersects(arch, arch));
		CHECK(not Geometry2D::Intersects(arch, outside));
	}

	{
		const Bezier2 arch{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } };
		const Bezier3 vertical{ Vec2{ 5, -1 }, Vec2{ 5, 2 }, Vec2{ 5, 8 }, Vec2{ 5, 11 } };
		const Bezier3 outside{ Vec2{ 20, -1 }, Vec2{ 20, 2 }, Vec2{ 20, 8 }, Vec2{ 20, 11 } };

		CHECK(Geometry2D::Intersects(arch, vertical));
		CHECK(Geometry2D::Intersects(vertical, arch));
		CHECK(not Geometry2D::Intersects(arch, outside));
	}
}


// Bezier3 と Bezier3 は、近似線分化と既存 Line x Bezier3 kernel による交差判定を扱うことを確認する。
TEST_CASE("Geometry2D.Intersects.Bezier3_Curves")
{
	const Bezier3 arch{ Vec2{ 0, 0 }, Vec2{ 3, 9 }, Vec2{ 7, 9 }, Vec2{ 10, 0 } };
	const Bezier3 vertical{ Vec2{ 5, -1 }, Vec2{ 5, 2 }, Vec2{ 5, 8 }, Vec2{ 5, 11 } };
	const Bezier3 horizontal{ Vec2{ -1, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 11, 5 } };
	const Bezier3 outside{ Vec2{ 20, -1 }, Vec2{ 20, 2 }, Vec2{ 20, 8 }, Vec2{ 20, 11 } };

	CHECK(Geometry2D::Intersects(arch, vertical));
	CHECK(Geometry2D::Intersects(vertical, arch));
	CHECK(Geometry2D::Intersects(arch, horizontal));
	CHECK(Geometry2D::Intersects(arch, arch));
	CHECK(not Geometry2D::Intersects(arch, outside));
}


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
