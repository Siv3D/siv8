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

	// 多項式係数が 1.0 より十分小さくても、曲線全体の相対スケールで解かれることを確認する。
	{
		const Bezier2 curve{ Vec2{ 0, 0 }, Vec2{ 5e-17, 0 }, Vec2{ 1e-16, 0 } };

		CHECK(Geometry2D::Intersects(Vec2{ 5e-17, 0 }, curve));
	}

	{
		const Bezier3 curve{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1e-16, 0 } };

		CHECK(Geometry2D::Intersects(Vec2{ 1.25e-17, 0 }, curve));
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

	// 片側だけを線分近似する実装でも、引数順によって結果が変化しないことを確認する。
	{
		const Bezier2 a{ Vec2{ 0, 2 }, Vec2{ 2, -2 }, Vec2{ -4, -4 } };
		const Bezier2 b{ Vec2{ 2, -6 }, Vec2{ 4, 2 }, Vec2{ -4, 4 } };

		CHECK(Geometry2D::Intersects(a, b) == Geometry2D::Intersects(b, a));
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

	// 片側だけを線分近似する実装でも、引数順によって結果が変化しないことを確認する。
	{
		const Bezier3 a{ Vec2{ -2, 4 }, Vec2{ 4, -4 }, Vec2{ -2, 0 }, Vec2{ -2, -6 } };
		const Bezier3 b{ Vec2{ 6, 2 }, Vec2{ -6, 0 }, Vec2{ 2, 4 }, Vec2{ 4, 4 } };

		CHECK(Geometry2D::Intersects(a, b) == Geometry2D::Intersects(b, a));
	}
}
