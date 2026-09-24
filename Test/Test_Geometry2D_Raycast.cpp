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

TEST_CASE("Geometry2D.Raycast.SuperEllipse.AxisBoundary")
{
	for (const double n : { 4.0, 8.0, 16.0, 64.0 })
	{
		const SuperEllipse shape{ Vec2{ 7, -11 }, SizeF{ 80, 5 }, n };
		for (const Vec2 normal : { Vec2{ 1, 0 }, Vec2{ -1, 0 }, Vec2{ 0, 1 }, Vec2{ 0, -1 } })
		{
			CAPTURE(n, normal);
			const Vec2 boundary = (shape.center + shape.axes * normal);
			for (const Vec2 direction : { normal, -normal, Vec2{ -normal.y, normal.x } })
			{
				const Ray2D ray{ boundary, direction };
				CheckHit(Geometry2D::Raycast(ray, shape), ray, boundary, normal, 0.0, false);
				CheckHit(Geometry2D::Raycast(ray, shape, 0.0), ray, boundary, normal, 0.0, false);
			}
			const Ray2D inside{ (boundary - normal * 0.001), normal };
			CheckHit(Geometry2D::Raycast(inside, shape), inside, boundary, normal, 0.001, true);
		}
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

TEST_CASE("Geometry2D.Raycast.PolygonCollapse")
{
	const Polygon source{
		Array<Vec2>{ { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } },
		Array<Array<Vec2>>{ { { 3, 3 }, { 3, 7 }, { 7, 7 }, { 7, 3 } } }
	};
	for (const Vec2 scale : { Vec2{ 0, 0 }, Vec2{ 0, 1 }, Vec2{ 1, 0 } })
	{
		for (const double angle : { 0.0, 0.5 })
		{
			CAPTURE(scale, angle);
			const Polygon collapsed = source.scaledFrom(Vec2{ 5, 5 }, scale).rotatedAt(Vec2{ 5, 5 }, angle);
			REQUIRE(not collapsed.isEmpty());
			for (const Ray2D ray : {
				Ray2D{ Vec2{ -5, 5 }, Vec2{ 1, 0 } },
				Ray2D{ Vec2{ 5, -5 }, Vec2{ 0, 1 } },
				Ray2D{ Vec2{ 5, 5 }, Vec2{ 1, 0 } } })
			{
				CHECK(not Geometry2D::Raycast(ray, collapsed));
				CHECK(not Geometry2D::Raycast(ray, MultiPolygon{ Polygon{}, collapsed }));
			}
		}
	}
}

TEST_CASE("Geometry2D.Raycast.MultiPolygonCollapse")
{
	const Polygon source = RectF{ 0, 0, 10, 10 }.asPolygon();
	const Polygon segment = source.scaledFromOrigin(Vec2{ 0, 1 });
	const Polygon point = source.scaledFrom(Vec2{ 0, 5 }, 0.0);
	const Polygon target = source.movedBy(20, 0);
	const Ray2D ray{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };

	for (const auto& multi : {
		MultiPolygon{ segment, point, Polygon{}, target },
		MultiPolygon{ target, Polygon{}, point, segment } })
	{
		CheckHit(Geometry2D::Raycast(ray, multi), ray,
			Vec2{ 20, 5 }, Vec2{ -1, 0 }, 25.0, false);
		CHECK(not Geometry2D::Raycast(ray, multi, 24.0));
	}

	const Ray2D inside{ Vec2{ 5, 5 }, Vec2{ 1, 0 } };
	const Polygon internalSegment = source.scaledFrom(inside.origin, Vec2{ 0, 1 });
	const Polygon internalPoint = source.scaledFrom(inside.origin, 0.0);
	for (const Polygon& collapsed : { internalSegment, internalPoint })
	{
		for (const auto& multi : { MultiPolygon{ collapsed, source }, MultiPolygon{ source, collapsed } })
		{
			CheckHit(Geometry2D::Raycast(inside, multi), inside,
				Vec2{ 10, 5 }, Vec2{ 1, 0 }, 5.0, true);
		}
	}
}

TEST_CASE("Geometry2D.Raycast.PolygonAreaAfterScaling")
{
	const Polygon source{ Array<Vec2>{ { 0, 5 }, { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } } };
	const Ray2D ray{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };
	for (const double scale : { 1.0, 1.0e-10 })
	{
		CAPTURE(scale);
		const Polygon polygon = source.scaledFromOrigin(Vec2{ scale, 1 });
		CheckHit(Geometry2D::Raycast(ray, polygon), ray,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 5.0, false);
	}
}

TEST_CASE("Geometry2D.Raycast.PolygonReflection")
{
	const Polygon source{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 6, 6 }, { 6, 14 }, { 14, 14 }, { 14, 6 } } }
	};
	for (const Vec2 scale : { Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
	{
		CAPTURE(scale);
		auto Check = [&](const auto& shape)
		{
			const Ray2D outside{ Vec2{ -5, 10 }, Vec2{ 1, 0 } };
			CheckHit(Geometry2D::Raycast(outside, shape), outside,
				Vec2{ 0, 10 }, Vec2{ -1, 0 }, 5.0, false);
			const Ray2D hole{ Vec2{ 10, 10 }, Vec2{ -1, 0 } };
			CheckHit(Geometry2D::Raycast(hole, shape), hole,
				Vec2{ 6, 10 }, Vec2{ 1, 0 }, 4.0, false);
			const Ray2D inside{ Vec2{ 2, 10 }, Vec2{ 1, 0 } };
			CheckHit(Geometry2D::Raycast(inside, shape), inside,
				Vec2{ 6, 10 }, Vec2{ 1, 0 }, 4.0, true);
		};
		Check(source.scaledFrom(Vec2{ 10, 10 }, scale));
		Check(MultiPolygon{ Polygon{}, source }.scaledFrom(Vec2{ 10, 10 }, scale));
	}
}

TEST_CASE("Geometry2D.Raycast.CoincidentCandidates")
{
	const RectF rect{ 0, 0, 10, 10 };
	for (const Vec2 offset : { Vec2{ 2, 1 }, Vec2{ 1, 2 }, Vec2{ 1, 1 } })
	{
		CAPTURE(offset);
		const Vec2 entryNormal = (offset.y < offset.x) ? Vec2{ -1, 0 } : Vec2{ 0, -1 };
		const Vec2 exitNormal = (offset.x < offset.y) ? Vec2{ 0, 1 } : Vec2{ 1, 0 };
		const Ray2D outside{ -offset, offset };
		CheckHit(Geometry2D::Raycast(outside, rect), outside,
			Vec2{ 0, 0 }, entryNormal, offset.length(), false);
		const Ray2D inside{ (Vec2{ 10, 10 } - offset), offset };
		CheckHit(Geometry2D::Raycast(inside, rect), inside,
			Vec2{ 10, 10 }, exitNormal, offset.length(), true);
	}

	// The overlapping edge may be visited before or after its adjacent edges.
	for (const double y : { 0.0, 10.0 })
	{
		const Ray2D ray{ Vec2{ -5, y }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(ray, rect, 5.0), ray,
			Vec2{ 0, y }, Vec2{ 0, (y == 0.0) ? -1.0 : 1.0 }, 5.0, false);
	}
}

TEST_CASE("Geometry2D.Raycast.NearestMemberOrder")
{
	const Polygon near = RectF{ 0, 0, 10, 10 }.asPolygon();
	const Polygon far = Triangle{ Vec2{ 20, 0 }, Vec2{ 30, 0 }, Vec2{ 30, 10 } }.asPolygon();
	for (const auto& multi : { MultiPolygon{ near, far }, MultiPolygon{ far, near } })
	{
		const Ray2D outside{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(outside, multi), outside,
			Vec2{ 0, 5 }, Vec2{ -1, 0 }, 5.0, false);
		CHECK(not Geometry2D::Raycast(outside, multi, 4.0));
		const Ray2D inside{ Vec2{ 5, 5 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(inside, multi), inside,
			Vec2{ 10, 5 }, Vec2{ 1, 0 }, 5.0, true);
	}
}

TEST_CASE("Geometry2D.Raycast.StartsInsideNearBoundary")
{
	auto Check = [](const auto& shape)
	{
		for (const double inset : { -1.0e-7, -1.0e-11, 0.0, 1.0e-11, 1.0e-7 })
		{
			CAPTURE(inset);
			const Ray2D ray{ Vec2{ (10000 + inset), 10005 }, Vec2{ 1, 0 } };
			const auto hit = Geometry2D::Raycast(ray, shape);
			REQUIRE(hit.has_value());
			CHECK(hit->startsInside == (1.0e-10 < inset));
		}
	};
	const RectF rect{ 10000, 10000, 10, 10 };
	Check(rect);
	Check(Circle{ Vec2{ 10005, 10005 }, 5 });
	Check(Ellipse{ Vec2{ 10005, 10005 }, 5, 3 });
	Check(SuperEllipse{ Vec2{ 10005, 10005 }, SizeF{ 5, 3 }, 1 });
	Check(SuperEllipse{ Vec2{ 10005, 10005 }, SizeF{ 5, 3 }, 2 });
	Check(SuperEllipse{ Vec2{ 10005, 10005 }, SizeF{ 5, 3 }, 4 });
	Check(RoundRect{ rect, 2 });
	Check(rect.asPolygon());
	Check(MultiPolygon{ rect.asPolygon() });
}
