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
