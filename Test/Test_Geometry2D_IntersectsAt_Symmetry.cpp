//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/IntersectsAt.hpp>

namespace
{
	[[nodiscard]]
	bool NearPoint(const Vec2& a, const Vec2& b)
	{
		const double scale = Max({ Abs(a.x), Abs(a.y), Abs(b.x), Abs(b.y), 1.0 });
		const double tolerance = (1.0e-7 * scale);
		return (a.distanceFromSq(b) <= (tolerance * tolerance));
	}

	[[nodiscard]]
	bool EquivalentPointSets(const Array<Vec2>& a, const Array<Vec2>& b)
	{
		if (a.size() != b.size())
		{
			return false;
		}

		for (const Vec2& pointA : a)
		{
			bool found = false;

			for (const Vec2& pointB : b)
			{
				if (NearPoint(pointA, pointB))
				{
					found = true;
					break;
				}
			}

			if (not found)
			{
				return false;
			}
		}

		return true;
	}
}

TEST_CASE("Geometry2D.IntersectsAt.CanonicalDispatchSymmetry")
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

	auto CheckPair = [&](const auto& a, const auto& b)
	{
		const auto ab = Geometry2D::IntersectsAt(a, b);
		const auto ba = Geometry2D::IntersectsAt(b, a);
		const bool intersectsAB = Geometry2D::Intersects(a, b);
		const bool intersectsBA = Geometry2D::Intersects(b, a);
		failureCount += (ab.has_value() != intersectsAB);
		failureCount += (ba.has_value() != intersectsBA);
		failureCount += (ab.has_value() != ba.has_value());

		if (ab && ba)
		{
			failureCount += (not EquivalentPointSets(*ab, *ba));
		}
	};
	CheckPair(point, point);
	CheckPair(point, vec2);
	CheckPair(point, line);
	CheckPair(point, lineString);
	CheckPair(point, bezier2);
	CheckPair(point, bezier3);
	CheckPair(point, rect);
	CheckPair(point, rectF);
	CheckPair(point, circle);
	CheckPair(point, ellipse);
	CheckPair(point, superEllipse);
	CheckPair(point, triangle);
	CheckPair(point, quad);
	CheckPair(point, roundRect);
	CheckPair(point, polygon);
	CheckPair(point, multiPolygon);
	CheckPair(vec2, vec2);
	CheckPair(vec2, line);
	CheckPair(vec2, lineString);
	CheckPair(vec2, bezier2);
	CheckPair(vec2, bezier3);
	CheckPair(vec2, rect);
	CheckPair(vec2, rectF);
	CheckPair(vec2, circle);
	CheckPair(vec2, ellipse);
	CheckPair(vec2, superEllipse);
	CheckPair(vec2, triangle);
	CheckPair(vec2, quad);
	CheckPair(vec2, roundRect);
	CheckPair(vec2, polygon);
	CheckPair(vec2, multiPolygon);
	CheckPair(line, line);
	CheckPair(line, lineString);
	CheckPair(line, bezier2);
	CheckPair(line, bezier3);
	CheckPair(line, rect);
	CheckPair(line, rectF);
	CheckPair(line, circle);
	CheckPair(line, ellipse);
	CheckPair(line, superEllipse);
	CheckPair(line, triangle);
	CheckPair(line, quad);
	CheckPair(line, roundRect);
	CheckPair(line, polygon);
	CheckPair(line, multiPolygon);
	CheckPair(lineString, lineString);
	CheckPair(lineString, bezier2);
	CheckPair(lineString, bezier3);
	CheckPair(lineString, rect);
	CheckPair(lineString, rectF);
	CheckPair(lineString, circle);
	CheckPair(lineString, ellipse);
	CheckPair(lineString, superEllipse);
	CheckPair(lineString, triangle);
	CheckPair(lineString, quad);
	CheckPair(lineString, roundRect);
	CheckPair(lineString, polygon);
	CheckPair(lineString, multiPolygon);
	CheckPair(bezier2, bezier2);
	CheckPair(bezier2, bezier3);
	CheckPair(bezier2, rect);
	CheckPair(bezier2, rectF);
	CheckPair(bezier2, circle);
	CheckPair(bezier2, ellipse);
	CheckPair(bezier2, superEllipse);
	CheckPair(bezier2, triangle);
	CheckPair(bezier2, quad);
	CheckPair(bezier2, roundRect);
	CheckPair(bezier2, polygon);
	CheckPair(bezier2, multiPolygon);
	CheckPair(bezier3, bezier3);
	CheckPair(bezier3, rect);
	CheckPair(bezier3, rectF);
	CheckPair(bezier3, circle);
	CheckPair(bezier3, ellipse);
	CheckPair(bezier3, superEllipse);
	CheckPair(bezier3, triangle);
	CheckPair(bezier3, quad);
	CheckPair(bezier3, roundRect);
	CheckPair(bezier3, polygon);
	CheckPair(bezier3, multiPolygon);
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
