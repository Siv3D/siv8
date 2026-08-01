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

// 異種型 Intersects overload の引数順が、同じ canonical 実装へ到達することを確認する。
TEST_CASE("Geometry2D.Intersects.CanonicalDispatchSymmetry")
{
	const Point point{ 0, 0 };
	const Vec2 vec2{ 0.5, 0.5 };
	const Line line{ Vec2{ -2, 0 }, Vec2{ 2, 0 } };
	const LineString lineString{ Vec2{ -2, -1 }, Vec2{ 0, 1 }, Vec2{ 2, -1 } };
	const Bezier2 bezier2{ Vec2{ -2, 0 }, Vec2{ 0, 2 }, Vec2{ 2, 0 } };
	const Bezier3 bezier3{ Vec2{ -2, 0 }, Vec2{ -1, 2 }, Vec2{ 1, 2 }, Vec2{ 2, 0 } };
	const Rect rect{ -2, -2, 4, 4 };
	const RectF rectF{ -1.5, -1.5, 3.0, 3.0 };
	const Circle circle{ Vec2{ 0, 0 }, 2.0 };
	const Ellipse ellipse{ Vec2{ 0, 0 }, 2.5, 1.5 };
	const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 2.5, 1.5 }, 4.0 };
	const Triangle triangle{ Vec2{ 0, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } };
	const Quad quad{ Vec2{ -2, -2 }, Vec2{ 2, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } };
	const RoundRect roundRect{ RectF{ -2, -2, 4, 4 }, 1.0 };
	const Polygon polygon{ Array<Vec2>{ Vec2{ -2, -2 }, Vec2{ 2, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } } };
	const MultiPolygon multiPolygon{ polygon };

	size_t failureCount = 0;

	auto CheckSymmetry = [&](const auto& a, const auto& b)
	{
		failureCount += (Geometry2D::Intersects(a, b) != Geometry2D::Intersects(b, a));
	};

	CheckSymmetry(point, vec2);
	CheckSymmetry(point, line);
	CheckSymmetry(point, lineString);
	CheckSymmetry(point, bezier2);
	CheckSymmetry(point, bezier3);
	CheckSymmetry(point, rect);
	CheckSymmetry(point, rectF);
	CheckSymmetry(point, circle);
	CheckSymmetry(point, ellipse);
	CheckSymmetry(point, superEllipse);
	CheckSymmetry(point, triangle);
	CheckSymmetry(point, quad);
	CheckSymmetry(point, roundRect);
	CheckSymmetry(point, polygon);
	CheckSymmetry(point, multiPolygon);
	CheckSymmetry(vec2, line);
	CheckSymmetry(vec2, lineString);
	CheckSymmetry(vec2, bezier2);
	CheckSymmetry(vec2, bezier3);
	CheckSymmetry(vec2, rect);
	CheckSymmetry(vec2, rectF);
	CheckSymmetry(vec2, circle);
	CheckSymmetry(vec2, ellipse);
	CheckSymmetry(vec2, superEllipse);
	CheckSymmetry(vec2, triangle);
	CheckSymmetry(vec2, quad);
	CheckSymmetry(vec2, roundRect);
	CheckSymmetry(vec2, polygon);
	CheckSymmetry(vec2, multiPolygon);
	CheckSymmetry(line, lineString);
	CheckSymmetry(line, bezier2);
	CheckSymmetry(line, bezier3);
	CheckSymmetry(line, rect);
	CheckSymmetry(line, rectF);
	CheckSymmetry(line, circle);
	CheckSymmetry(line, ellipse);
	CheckSymmetry(line, superEllipse);
	CheckSymmetry(line, triangle);
	CheckSymmetry(line, quad);
	CheckSymmetry(line, roundRect);
	CheckSymmetry(line, polygon);
	CheckSymmetry(line, multiPolygon);
	CheckSymmetry(lineString, bezier2);
	CheckSymmetry(lineString, bezier3);
	CheckSymmetry(lineString, rect);
	CheckSymmetry(lineString, rectF);
	CheckSymmetry(lineString, circle);
	CheckSymmetry(lineString, ellipse);
	CheckSymmetry(lineString, superEllipse);
	CheckSymmetry(lineString, triangle);
	CheckSymmetry(lineString, quad);
	CheckSymmetry(lineString, roundRect);
	CheckSymmetry(lineString, polygon);
	CheckSymmetry(lineString, multiPolygon);
	CheckSymmetry(bezier2, bezier3);
	CheckSymmetry(bezier2, rect);
	CheckSymmetry(bezier2, rectF);
	CheckSymmetry(bezier2, circle);
	CheckSymmetry(bezier2, ellipse);
	CheckSymmetry(bezier2, superEllipse);
	CheckSymmetry(bezier2, triangle);
	CheckSymmetry(bezier2, quad);
	CheckSymmetry(bezier2, roundRect);
	CheckSymmetry(bezier2, polygon);
	CheckSymmetry(bezier2, multiPolygon);
	CheckSymmetry(bezier3, rect);
	CheckSymmetry(bezier3, rectF);
	CheckSymmetry(bezier3, circle);
	CheckSymmetry(bezier3, ellipse);
	CheckSymmetry(bezier3, superEllipse);
	CheckSymmetry(bezier3, triangle);
	CheckSymmetry(bezier3, quad);
	CheckSymmetry(bezier3, roundRect);
	CheckSymmetry(bezier3, polygon);
	CheckSymmetry(bezier3, multiPolygon);
	CheckSymmetry(rect, rectF);
	CheckSymmetry(rect, circle);
	CheckSymmetry(rect, ellipse);
	CheckSymmetry(rect, superEllipse);
	CheckSymmetry(rect, triangle);
	CheckSymmetry(rect, quad);
	CheckSymmetry(rect, roundRect);
	CheckSymmetry(rect, polygon);
	CheckSymmetry(rect, multiPolygon);
	CheckSymmetry(rectF, circle);
	CheckSymmetry(rectF, ellipse);
	CheckSymmetry(rectF, superEllipse);
	CheckSymmetry(rectF, triangle);
	CheckSymmetry(rectF, quad);
	CheckSymmetry(rectF, roundRect);
	CheckSymmetry(rectF, polygon);
	CheckSymmetry(rectF, multiPolygon);
	CheckSymmetry(circle, ellipse);
	CheckSymmetry(circle, superEllipse);
	CheckSymmetry(circle, triangle);
	CheckSymmetry(circle, quad);
	CheckSymmetry(circle, roundRect);
	CheckSymmetry(circle, polygon);
	CheckSymmetry(circle, multiPolygon);
	CheckSymmetry(ellipse, superEllipse);
	CheckSymmetry(ellipse, triangle);
	CheckSymmetry(ellipse, quad);
	CheckSymmetry(ellipse, roundRect);
	CheckSymmetry(ellipse, polygon);
	CheckSymmetry(ellipse, multiPolygon);
	CheckSymmetry(superEllipse, triangle);
	CheckSymmetry(superEllipse, quad);
	CheckSymmetry(superEllipse, roundRect);
	CheckSymmetry(superEllipse, polygon);
	CheckSymmetry(superEllipse, multiPolygon);
	CheckSymmetry(triangle, quad);
	CheckSymmetry(triangle, roundRect);
	CheckSymmetry(triangle, polygon);
	CheckSymmetry(triangle, multiPolygon);
	CheckSymmetry(quad, roundRect);
	CheckSymmetry(quad, polygon);
	CheckSymmetry(quad, multiPolygon);
	CheckSymmetry(roundRect, polygon);
	CheckSymmetry(roundRect, multiPolygon);
	CheckSymmetry(polygon, multiPolygon);

	// canonical direction を変更した overload の非交差側も確認する。
	CheckSymmetry(Point{ 100, 100 }, LineString{ Vec2{ -2, 0 }, Vec2{ 2, 0 } });
	CheckSymmetry(Rect{ 100, 100, 4, 4 }, RectF{ -2, -2, 4, 4 });
	CheckSymmetry(Triangle{ Vec2{ 100, 100 }, Vec2{ 104, 100 }, Vec2{ 100, 104 } }, roundRect);
	CheckSymmetry(Quad{ Vec2{ 100, 100 }, Vec2{ 104, 100 }, Vec2{ 104, 104 }, Vec2{ 100, 104 } }, roundRect);

	CHECK(failureCount == 0);
}
