//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Overlaps.hpp>

TEST_CASE("Geometry2D.Overlaps.CanonicalDispatchSymmetry")
{
	const auto rect = Rect{ -2, -2, 4, 4 };
	const auto rectF = RectF{ -1.5, -1.5, 3.0, 3.0 };
	const auto circle = Circle{ Vec2{ 0, 0 }, 2.0 };
	const auto ellipse = Ellipse{ Vec2{ 0, 0 }, 2.5, 1.5 };
	const auto superEllipse = SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 2.5, 1.5 }, 4.0 };
	const auto triangle = Triangle{ Vec2{ 0, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } };
	const auto quad = Quad{ Vec2{ -2, -2 }, Vec2{ 2, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } };
	const auto roundRect = RoundRect{ RectF{ -2, -2, 4, 4 }, 1.0 };
	const auto polygon = Polygon{ Array<Vec2>{ Vec2{ -2, -2 }, Vec2{ 2, -2 }, Vec2{ 2, 2 }, Vec2{ -2, 2 } } };
	const auto multiPolygon = MultiPolygon{ polygon };

	size_t failureCount = 0;

	auto CheckPair = [&](const auto& a, const auto& b)
	{
		const bool ab = Geometry2D::Overlaps(a, b);
		const bool ba = Geometry2D::Overlaps(b, a);
		failureCount += (ab != ba);
		failureCount += (ab && (not Geometry2D::Intersects(a, b)));
		failureCount += (ba && (not Geometry2D::Intersects(b, a)));
	};

	CheckPair(rect, rect);
	CheckPair(rect, rectF);
	CheckPair(rect, circle);
	CheckPair(rect, ellipse);
	CheckPair(rect, superEllipse);
	CheckPair(rect, triangle);
	CheckPair(rect, quad);
	CheckPair(rect, roundRect);
	CheckPair(rect, polygon);
	CheckPair(rect, multiPolygon);
	CheckPair(rectF, rectF);
	CheckPair(rectF, circle);
	CheckPair(rectF, ellipse);
	CheckPair(rectF, superEllipse);
	CheckPair(rectF, triangle);
	CheckPair(rectF, quad);
	CheckPair(rectF, roundRect);
	CheckPair(rectF, polygon);
	CheckPair(rectF, multiPolygon);
	CheckPair(circle, circle);
	CheckPair(circle, ellipse);
	CheckPair(circle, superEllipse);
	CheckPair(circle, triangle);
	CheckPair(circle, quad);
	CheckPair(circle, roundRect);
	CheckPair(circle, polygon);
	CheckPair(circle, multiPolygon);
	CheckPair(ellipse, ellipse);
	CheckPair(ellipse, superEllipse);
	CheckPair(ellipse, triangle);
	CheckPair(ellipse, quad);
	CheckPair(ellipse, roundRect);
	CheckPair(ellipse, polygon);
	CheckPair(ellipse, multiPolygon);
	CheckPair(superEllipse, superEllipse);
	CheckPair(superEllipse, triangle);
	CheckPair(superEllipse, quad);
	CheckPair(superEllipse, roundRect);
	CheckPair(superEllipse, polygon);
	CheckPair(superEllipse, multiPolygon);
	CheckPair(triangle, triangle);
	CheckPair(triangle, quad);
	CheckPair(triangle, roundRect);
	CheckPair(triangle, polygon);
	CheckPair(triangle, multiPolygon);
	CheckPair(quad, quad);
	CheckPair(quad, roundRect);
	CheckPair(quad, polygon);
	CheckPair(quad, multiPolygon);
	CheckPair(roundRect, roundRect);
	CheckPair(roundRect, polygon);
	CheckPair(roundRect, multiPolygon);
	CheckPair(polygon, polygon);
	CheckPair(polygon, multiPolygon);
	CheckPair(multiPolygon, multiPolygon);

	CHECK(failureCount == 0);
}
