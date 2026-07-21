//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <cmath>
# include <Siv3D/Ray2D.hpp>
# include <Siv3D/RaycastHit2D.hpp>
# include <Siv3D/Geometry2D/Raycast.hpp>

namespace
{
	[[nodiscard]]
	bool Near(const double a, const double b, const double tolerance = 1.0e-8)
	{
		return (Abs(a - b) <= tolerance);
	}

	[[nodiscard]]
	bool NearPoint(const Vec2& a, const Vec2& b, const double tolerance = 1.0e-8)
	{
		return (a.distanceFromSq(b) <= (tolerance * tolerance));
	}

	void CheckHit(const Optional<RaycastHit2D>& actual,
		const Ray2D& ray, const Vec2& position, const Vec2& normal,
		const double distance, const bool startsInside,
		const double tolerance = 1.0e-8)
	{
		REQUIRE(actual.has_value());
		CHECK(Near(actual->distance, distance, tolerance));
		CHECK(NearPoint(actual->position, position, tolerance));
		CHECK(NearPoint(actual->position, ray.pointAt(actual->distance), tolerance));
		CHECK(NearPoint(actual->normal, normal, tolerance));
		CHECK(Near(actual->normal.length(), 1.0, tolerance));
		CHECK(actual->startsInside == startsInside);
	}
}

TEST_CASE("Ray2D.NormalizationAndPointAt")
{
	const Ray2D ray{ Vec2{ 2, 3 }, Vec2{ 3, 4 } };
	CHECK(Near(ray.direction.length(), 1.0));
	CHECK(NearPoint(ray.direction, Vec2{ 0.6, 0.8 }));
	CHECK(NearPoint(ray.pointAt(5.0), Vec2{ 5, 7 }));
}

TEST_CASE("Geometry2D.Raycast.Rect")
{
	const RectF rect{ 0, 0, 10, 10 };

	{
		const Ray2D ray{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, rect), ray,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 5.0, false);
		CHECK(not Geometry2D::Raycast(ray, rect, 4.999).has_value());
		CHECK(Geometry2D::Raycast(ray, rect, 5.0).has_value());
		CHECK(not Geometry2D::Raycast(ray, rect, -1.0).has_value());
	}

	{
		const Ray2D ray{ Vec2{ 5, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, rect), ray,
			Vec2{ 10, 5 }, Vec2{ 1, 0 }, 5.0, true);
	}

	{
		const Ray2D outward{ Vec2{ 0, 5 }, Vec2{ -1, 0 } };
		CheckHit(Geometry2D::Raycast(outward, rect), outward,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 0.0, false);

		const Ray2D inward{ Vec2{ 0, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(inward, rect), inward,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 0.0, false);
	}

	{
		const Ray2D edgeOverlap{ Vec2{ -5, 0 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(edgeOverlap, rect), edgeOverlap,
			Vec2{ 0, 0 }, Vec2{ 0, -1 }, 5.0, false);
	}

	CHECK(not Geometry2D::Raycast(
		Ray2D{ Vec2{ -5, 20 }, Vec2{ 1, 0 } }, rect).has_value());
	CHECK(not Geometry2D::Raycast(
		Ray2D{ Vec2{ -5, 5 }, Vec2{ -1, 0 } }, rect).has_value());
}

TEST_CASE("Geometry2D.Raycast.CircleEllipseSuperEllipse")
{
	{
		const Circle circle{ Vec2{ 0, 0 }, 5 };
		const Ray2D ray{ Vec2{ -10, 0 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, circle), ray,
			Vec2{ -5, 0 }, Vec2{ -1, 0 }, 5.0, false);

		const Ray2D internal{ Vec2{ 0, 0 }, Vec2{ 0, 1 } };
		CheckHit(Geometry2D::Raycast(internal, circle), internal,
			Vec2{ 0, 5 }, Vec2{ 0, 1 }, 5.0, true);

		const Ray2D tangent{ Vec2{ -10, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(tangent, circle), tangent,
			Vec2{ 0, 5 }, Vec2{ 0, 1 }, 10.0, false, 1.0e-7);
	}

	{
		const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };
		const Ray2D ray{ Vec2{ -10, 0 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, ellipse), ray,
			Vec2{ -5, 0 }, Vec2{ -1, 0 }, 5.0, false);
	}

	{
		const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };
		const Ray2D ray{ Vec2{ -10, 0 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, superEllipse), ray,
			Vec2{ -5, 0 }, Vec2{ -1, 0 }, 5.0, false, 1.0e-6);
	}

	{
		const SuperEllipse diamond{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 1.0 };
		const Ray2D ray{ Vec2{ -10, 0 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, diamond), ray,
			Vec2{ -5, 0 }, Vec2{ (-3.0 / std::sqrt(34.0)), (5.0 / std::sqrt(34.0)) }, 5.0, false, 1.0e-7);
	}
}

TEST_CASE("Geometry2D.Raycast.PolygonalShapes")
{
	{
		const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
		const Ray2D ray{ Vec2{ 2, -5 }, Vec2{ 0, 1 } };
		CheckHit(Geometry2D::Raycast(ray, triangle), ray,
			Vec2{ 2, 0 }, Vec2{ 0, -1 }, 5.0, false);

		const Triangle reversed{ triangle.p0, triangle.p2, triangle.p1 };
		CheckHit(Geometry2D::Raycast(ray, reversed), ray,
			Vec2{ 2, 0 }, Vec2{ 0, -1 }, 5.0, false);
	}

	{
		const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Ray2D ray{ Vec2{ 5, 15 }, Vec2{ 0, -1 } };
		CheckHit(Geometry2D::Raycast(ray, quad), ray,
			Vec2{ 5, 10 }, Vec2{ 0, 1 }, 5.0, false);
	}

	{
		const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };
		const Ray2D ray{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, roundRect), ray,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 5.0, false);
	}
}

TEST_CASE("Geometry2D.Raycast.PolygonHole")
{
	const Polygon polygon{
		Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } },
		Array<Array<Vec2>>{ Array<Vec2>{ Vec2{ 3, 3 }, Vec2{ 3, 7 }, Vec2{ 7, 7 }, Vec2{ 7, 3 } } }
	};

	{
		const Ray2D fromHole{ Vec2{ 5, 5 }, Vec2{ -1, 0 } };
		CheckHit(Geometry2D::Raycast(fromHole, polygon), fromHole,
			Vec2{ 3, 5 }, Vec2{ 1, 0 }, 2.0, false);
	}

	{
		const Ray2D fromMaterial{ Vec2{ 1, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(fromMaterial, polygon), fromMaterial,
			Vec2{ 3, 5 }, Vec2{ 1, 0 }, 2.0, true);
	}

	{
		const Ray2D fromOutside{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(fromOutside, polygon), fromOutside,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 5.0, false);
	}
}

TEST_CASE("Geometry2D.Raycast.MultiPolygon")
{
	const Polygon a{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } } };
	const Polygon b{ Array<Vec2>{ Vec2{ 8, 0 }, Vec2{ 12, 0 }, Vec2{ 12, 4 }, Vec2{ 8, 4 } } };
	const MultiPolygon multi{ a, b };
	const Ray2D ray{ Vec2{ -5, 2 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(ray, multi), ray,
		Vec2{ 0, 2 }, Vec2{ -1, 0 }, 5.0, false);

	const Ray2D insideSecond{ Vec2{ 10, 2 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(insideSecond, multi), insideSecond,
		Vec2{ 12, 2 }, Vec2{ 1, 0 }, 2.0, true);

	const Polygon touchingA{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 2, 0 }, Vec2{ 2, 2 }, Vec2{ 0, 2 } } };
	const Polygon touchingB{ Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const MultiPolygon pointTouch{ touchingA, touchingB };
	const Ray2D vertexRay{ Vec2{ 2, -2 }, Vec2{ 0, 1 } };
	const auto vertexHit = Geometry2D::Raycast(vertexRay, pointTouch);
	REQUIRE(vertexHit.has_value());
	CHECK(Near(vertexHit->distance, 2.0));
	CHECK(NearPoint(vertexHit->position, Vec2{ 2, 0 }));
	CHECK(Near(vertexHit->normal.length(), 1.0));
}

TEST_CASE("Geometry2D.Raycast.EmptyAndRuntimeDegeneration")
{
	const Ray2D ray{ Vec2{ -5, 0 }, Vec2{ 1, 0 } };

	CHECK(not Geometry2D::Raycast(ray, RectF{ 0, 0, 0, 0 }).has_value());
	CHECK(not Geometry2D::Raycast(ray, RectF{ 0, -5, 0, 10 }).has_value());
	CHECK(not Geometry2D::Raycast(ray, Circle{ Vec2{ 0, 0 }, 0 }).has_value());
	CHECK(not Geometry2D::Raycast(ray, Ellipse{ Vec2{ 0, 0 }, 0, 5 }).has_value());
	CHECK(not Geometry2D::Raycast(ray,
		Triangle{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } }).has_value());
	CHECK(not Geometry2D::Raycast(ray,
		Quad{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } }).has_value());
	CHECK(not Geometry2D::Raycast(ray, Polygon{}).has_value());
	CHECK(not Geometry2D::Raycast(ray, MultiPolygon{}).has_value());
}
