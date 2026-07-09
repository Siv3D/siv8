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
# include <limits>

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
}
