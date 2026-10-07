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

TEST_CASE("Geometry2D.Raycast.RoundShapes.Contact")
{
	const auto Check = [](const auto& shape, const Vec2& point, const Vec2& normal,
		const Vec2& tangent, const double scale)
	{
		const double tolerance = (1.0e-8 * scale);
		for (const Vec2 direction : { normal, -normal, tangent, -tangent })
		{
			const Ray2D ray{ point, direction };
			CheckHit(Geometry2D::Raycast(ray, shape), ray, point, normal, 0.0, false, tolerance);
			CheckHit(Geometry2D::Raycast(ray, shape, 0.0), ray, point, normal, 0.0, false, tolerance);
		}
		for (const Vec2 direction : { tangent, -tangent })
		{
			const double distance = (0.5 * scale);
			const Ray2D ray{ (point - direction * distance), direction };
			CheckHit(Geometry2D::Raycast(ray, shape), ray, point, normal, distance, false, tolerance);
			CheckHit(Geometry2D::Raycast(ray, shape, distance), ray, point, normal, distance, false, tolerance);
			CHECK(not Geometry2D::Raycast(ray, shape, distance * 0.9));
			CHECK(not Geometry2D::Raycast(Ray2D{ (ray.origin + normal * (1.0e-6 * scale)), direction }, shape));
			const auto crossing = Geometry2D::Raycast(Ray2D{ (ray.origin - normal * (1.0e-6 * scale)), direction }, shape);
			REQUIRE(crossing);
			CHECK(crossing->distance < distance);
		}
	};
	for (const double scale : { 0.001, 1.0, 1000.0 })
	{
		const Vec2 center = (Vec2{ 7, -11 } * scale);
		for (const double angle : { 0.01, 0.37, 0.9, 2.2, 4.3 })
		{
			CAPTURE(scale, angle);
			const Vec2 unit{ std::cos(angle), std::sin(angle) };
			const Circle circle{ center, (5.0 * scale) };
			Check(circle, (center + unit * circle.r), unit, Vec2{ -unit.y, unit.x }, scale);
			const Ellipse ellipse{ center, (5.0 * scale), (3.0 * scale) };
			const Vec2 point = (center + unit * ellipse.axes);
			const Vec2 normal = (unit / ellipse.axes).normalized();
			const Vec2 tangent = (Vec2{ -unit.y, unit.x } * ellipse.axes).normalized();
			Check(ellipse, point, normal, tangent, scale);
			Check(SuperEllipse{ ellipse, 2.0 }, point, normal, tangent, scale);
		}
		const RoundRect rounded{ RectF{ center, SizeF{ 16.0 * scale, 12.0 * scale } }, (3.0 * scale) };
		for (const Vec2 sign : { Vec2{ -1, -1 }, Vec2{ 1, -1 }, Vec2{ 1, 1 }, Vec2{ -1, 1 } })
		{
			const Vec2 arcCenter = (center + Vec2{ ((sign.x < 0) ? 3.0 : 13.0), ((sign.y < 0) ? 3.0 : 9.0) } * scale);
			const Vec2 normal = (Vec2{ std::cos(0.37), std::sin(0.37) } * sign);
			Check(rounded, (arcCenter + normal * rounded.r), normal, Vec2{ -normal.y, normal.x }, scale);
		}
	}
}

TEST_CASE("Geometry2D.Raycast.RoundShapes.ContactBand")
{
	const auto Check = [](const auto& shape, const Vec2& point, const Vec2& tangent)
	{
		const auto near = Geometry2D::Raycast(Ray2D{ (point * (1.0 + 2.0e-15)), tangent }, shape, 0.0);
		REQUIRE(near);
		CHECK(near->distance == 0.0);
		CHECK(not near->startsInside);
		CHECK(not Geometry2D::Raycast(Ray2D{ (point * (1.0 + 1.0e-12)), tangent }, shape));
		CHECK(not Geometry2D::Raycast(Ray2D{ (point * (1.0 - 1.0e-12)), tangent }, shape, 0.0));
	};
	for (const double scale : { 0.01, 1.0, 100.0 })
	{
		CAPTURE(scale);
		const Circle circle{ 0, 0, (5.0 * scale) };
		Check(circle, (Vec2{ 0.6, 0.8 } * circle.r), Vec2{ -0.8, 0.6 });
		const Ellipse ellipse{ 0, 0, (5.0 * scale), (3.0 * scale) };
		const Vec2 point = (Vec2{ 0.6, 0.8 } * ellipse.axes);
		const Vec2 tangent = (Vec2{ -0.8, 0.6 } * ellipse.axes).normalized();
		Check(ellipse, point, tangent);
		Check(SuperEllipse{ ellipse, 2.0 }, point, tangent);
	}
}

TEST_CASE("Geometry2D.Raycast.RoundShapes.DistantOrigin")
{
	const Ray2D ray{ Vec2{ -1.0e8, 0 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(ray, Circle{ 0, 0, 1 }), ray, Vec2{ -1, 0 }, Vec2{ -1, 0 }, (1.0e8 - 1.0), false);
	CheckHit(Geometry2D::Raycast(ray, Ellipse{ 0, 0, 2, 1 }), ray, Vec2{ -2, 0 }, Vec2{ -1, 0 }, (1.0e8 - 2.0), false);
	CHECK(not Geometry2D::Raycast(Ray2D{ Vec2{ -1.0e8, 1.001 }, Vec2{ 1, 0 } }, Circle{ 0, 0, 1 }));
	CHECK(not Geometry2D::Raycast(Ray2D{ Vec2{ -1.0e8, 1.001 }, Vec2{ 1, 0 } }, Ellipse{ 0, 0, 2, 1 }));
}

TEST_CASE("Geometry2D.Raycast.SuperEllipse.InteriorCenter")
{
	for (const double n : { 0.25, 0.5, 0.9, 1.1, 1.5, 1.99, 2.01, 4.0, 64.0 })
	{
		const SuperEllipse shape{ Vec2{ 7, -11 }, SizeF{ 5, 3 }, n };
		const auto hit = Geometry2D::Raycast(Ray2D{ shape.center, Vec2{ 1, 0 } }, shape);
		CAPTURE(n);
		REQUIRE(hit);
		CHECK(hit->startsInside);
		CHECK(Near(hit->distance, 5.0));
		CHECK(hit->position.distanceFrom(shape.center + Vec2{ 5, 0 }) < 1.0e-9);
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

TEST_CASE("Geometry2D.Raycast.SuperEllipse.ConcaveFirstHit")
{
	const double inner = std::sqrt(0.125);
	const std::array<Vec2, 4> roots{
		Vec2{ -0.015625, 0.765625 }, Vec2{ 0.375 - inner, 0.375 + inner },
		Vec2{ 0.375 + inner, 0.375 - inner }, Vec2{ 0.765625, -0.015625 }
	};
	for (const double scale : { 0.001, 1.0, 1000.0 })
	{
		const SuperEllipse shape{ (Vec2{ 7, -11 } * scale), SizeF{ scale, scale }, 0.5 };
		for (const bool reversed : { false, true })
		{
			const Vec2 direction = (reversed ? Vec2{ -1, 1 } : Vec2{ 1, -1 }).normalized();
			for (size_t i = 0; i < roots.size(); ++i)
			{
				const Vec2 root = roots[reversed ? (3 - i) : i];
				const Vec2 point = (shape.center + root * scale);
				const Vec2 normal = Vec2{
					std::copysign(1.0 / std::sqrt(Abs(root.x)), root.x),
					std::copysign(1.0 / std::sqrt(Abs(root.y)), root.y) }.normalized();
				const double distance = (0.01 * scale);
				const Ray2D ray{ (point - direction * distance), direction };
				CAPTURE(scale, reversed, i);
				CheckHit(Geometry2D::Raycast(ray, shape), ray, point, normal, distance, (i % 2 == 1));
				CHECK(not Geometry2D::Raycast(ray, shape, distance * 0.9));
				CheckHit(Geometry2D::Raycast(ray, shape, distance), ray, point, normal, distance, (i % 2 == 1));
			}
		}
	}
}

TEST_CASE("Geometry2D.Raycast.SuperEllipse.TangentAndBoundaryOrigin")
{
	for (const double n : { 0.5, 0.9, 1.1, 1.5, 2.0, 4.0, 64.0 })
	{
		const SuperEllipse shape{ Vec2{ 7, -11 }, SizeF{ 5, 3 }, n };
		const double coordinate = std::pow(0.5, (1.0 / n));
		const Vec2 point = (shape.center + shape.axes * coordinate);
		const Vec2 normal = Vec2{ 1.0 / shape.a, 1.0 / shape.b }.normalized();
		const Vec2 tangent = Vec2{ shape.a, -shape.b }.normalized();
		for (const Vec2 direction : { tangent, -tangent })
		{
			CAPTURE(n, direction);
			const Ray2D ray{ (point - direction * 0.25), direction };
			CheckHit(Geometry2D::Raycast(ray, shape, 0.5), ray, point, normal, 0.25, (n < 1.0));
			CHECK(not Geometry2D::Raycast(ray, shape, 0.2));
		}
		for (const Vec2 direction : { normal, -normal, tangent })
		{
			const Ray2D ray{ point, direction };
			CheckHit(Geometry2D::Raycast(ray, shape), ray, point, normal, 0.0, false);
			CheckHit(Geometry2D::Raycast(ray, shape, 0.0), ray, point, normal, 0.0, false);
		}
		const Ray2D miss{ (point + normal * 0.001), tangent };
		if (1.0 < n)
		{
			CHECK(not Geometry2D::Raycast(miss, shape));
		}
	}
}

TEST_CASE("Geometry2D.Raycast.SuperEllipse.BoundaryOriginWithoutResolvedRoot")
{
	const SuperEllipse shape{ Vec2{ 7, -11 }, SizeF{ 0.05, 0.03 }, 0.25 };
	const double t = 0.65;
	const Vec2 point = (shape.center + shape.axes * Vec2{ std::pow(t, 4.0), std::pow(1.0 - t, 4.0) });
	const Vec2 normal = Vec2{ std::pow(t, -3.0) / shape.a, std::pow(1.0 - t, -3.0) / shape.b }.normalized();
	const Vec2 origin = (point - normal * 1.0e-14);
	const Ray2D ray{ origin, Vec2{ -normal.y, normal.x } };
	CheckHit(Geometry2D::Raycast(ray, shape, 0.01), ray, origin, normal, 0.0, false);
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

TEST_CASE("Geometry2D.Raycast.Range.ShapesAndEndpoints")
{
	const Ray2D ray{ Vec2{ -10, 0 }, Vec2{ 1, 0 } };
	const auto Check = [&](const auto& shape, const Vec2 exitNormal = Vec2{ 1, 0 })
	{
		const auto first = Geometry2D::Raycast(ray, shape);
		REQUIRE(first);
		const auto lowerEndpoint = Geometry2D::Raycast(ray, shape, first->distance, 30.0);
		CheckHit(lowerEndpoint, ray, first->position, first->normal, first->distance, false);
		CheckHit(Geometry2D::Raycast(ray, shape, 5.001, 30.0), ray,
			Vec2{ 5, 0 }, exitNormal, 15.0, false);
		CheckHit(Geometry2D::Raycast(ray, shape, 5.001, 15.0), ray,
			Vec2{ 5, 0 }, exitNormal, 15.0, false);
		CHECK(not Geometry2D::Raycast(ray, shape, 5.001, 14.999));
		CHECK(not Geometry2D::Raycast(ray, shape, 15.001, Math::Inf));
		CHECK(not Geometry2D::Raycast(ray, shape, -0.001, 30.0));
		CHECK(not Geometry2D::Raycast(ray, shape, 10.0, 5.0));
		CHECK(not Geometry2D::Raycast(ray, shape, 0.0, -1.0));
	};
	const Rect rect{ -5, -3, 10, 6 };
	Check(rect);
	Check(RectF{ rect });
	Check(Circle{ Vec2{ 0, 0 }, 5 });
	Check(Ellipse{ Vec2{ 0, 0 }, 5, 3 });
	Check(Triangle{ Vec2{ -5, -3 }, Vec2{ 15, -3 }, Vec2{ -5, 3 } }, Vec2{ 3, 10 }.normalized());
	Check(Quad{ Vec2{ -5, -3 }, Vec2{ 5, -3 }, Vec2{ 5, 3 }, Vec2{ -5, 3 } });
	Check(RoundRect{ RectF{ rect }, 2 });
	Check(RectF{ rect }.asPolygon());
	Check(MultiPolygon{ RectF{ rect }.asPolygon() });
	for (const double n : { 0.25, 0.5, 0.9, 1.0, 1.1, 1.5, 2.0, 4.0, 16.0, 64.0 })
	{
		CAPTURE(n);
		// At a diamond's corner the normal is nonunique; check only its position.
		const SuperEllipse shape{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, n };
		const auto next = Geometry2D::Raycast(ray, shape, 5.001, 30.0);
		REQUIRE(next);
		CHECK(Near(next->distance, 15.0));
		CHECK(NearPoint(next->position, Vec2{ 5, 0 }));
		CHECK(not next->startsInside);
		const auto first = Geometry2D::Raycast(ray, shape);
		REQUIRE(first);
		const auto lowerEndpoint = Geometry2D::Raycast(ray, shape, first->distance, 30.0);
		REQUIRE(lowerEndpoint);
		CHECK(lowerEndpoint->distance == first->distance);
	}
}

TEST_CASE("Geometry2D.Raycast.Range.ZeroLowerBoundCompatibility")
{
	const auto Check = [&](const auto& shape)
	{
		for (const Ray2D ray : {
			Ray2D{ Vec2{ -10, 0 }, Vec2{ 1, 0 } },
			Ray2D{ Vec2{ 0, 0 }, Vec2{ 1, 0 } },
			Ray2D{ Vec2{ -5, 0 }, Vec2{ -1, 0 } },
			Ray2D{ Vec2{ -5, 0 }, Vec2{ 1, 0 } },
			Ray2D{ Vec2{ -10, 3 }, Vec2{ 1, 0 } },
			Ray2D{ Vec2{ -10, 30 }, Vec2{ 1, 0 } } })
		{
			for (const double limit : { -1.0, 0.0, 5.0, 20.0, Math::Inf })
			{
				const auto original = Geometry2D::Raycast(ray, shape, limit);
				const auto ranged = Geometry2D::Raycast(ray, shape, 0.0, limit);
				REQUIRE(original.has_value() == ranged.has_value());
				if (original)
				{
					CHECK(ranged->distance == original->distance);
					CHECK(ranged->position == original->position);
					CHECK(ranged->normal == original->normal);
					CHECK(ranged->startsInside == original->startsInside);
				}
			}
		}
	};
	Check(Rect{ -5, -3, 10, 6 });
	Check(RectF{ -5, -3, 10, 6 });
	Check(Circle{ Vec2{ 0, 0 }, 5 });
	Check(Ellipse{ Vec2{ 0, 0 }, 5, 3 });
	Check(Triangle{ Vec2{ -5, -3 }, Vec2{ 5, -3 }, Vec2{ 0, 6 } });
	Check(Quad{ Vec2{ -5, -3 }, Vec2{ 4, -3 }, Vec2{ 5, 3 }, Vec2{ -4, 3 } });
	Check(RoundRect{ RectF{ -5, -3, 10, 6 }, 2 });
	Check(RectF{ -5, -3, 10, 6 }.asPolygon());
	Check(MultiPolygon{ RectF{ -5, -3, 10, 6 }.asPolygon() });
	for (const double n : { 0.5, 1.0, 2.0, 4.0 })
	{
		Check(SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, n });
	}
}

TEST_CASE("Geometry2D.Raycast.Range.BoundaryRestartAndOriginClassification")
{
	const auto Check = [&](const auto& shape)
	{
		const Ray2D inward{ Vec2{ 5, 0 }, Vec2{ -1, 0 } };
		CheckHit(Geometry2D::Raycast(inward, shape, 0.001, Math::Inf), inward,
			Vec2{ -5, 0 }, Vec2{ -1, 0 }, 10.0, false);
		const Ray2D outward{ Vec2{ 5, 0 }, Vec2{ 1, 0 } };
		CHECK(not Geometry2D::Raycast(outward, shape, 0.001, Math::Inf));
		const Ray2D fromInside{ Vec2{ 0, 0 }, Vec2{ 1, 0 } };
		CheckHit(Geometry2D::Raycast(fromInside, shape, 1.0, 10.0), fromInside,
			Vec2{ 5, 0 }, Vec2{ 1, 0 }, 5.0, true);
	};
	Check(RectF{ -5, -3, 10, 6 });
	Check(Circle{ Vec2{ 0, 0 }, 5 });
	Check(Ellipse{ Vec2{ 0, 0 }, 5, 3 });
	Check(RoundRect{ RectF{ -5, -3, 10, 6 }, 2 });
	for (const double n : { 0.25, 0.5, 0.9, 1.1, 1.5, 2.0, 4.0, 16.0, 64.0 })
	{
		Check(SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, n });
	}

	const Polygon hole{
		Array<Vec2>{ { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } },
		Array<Array<Vec2>>{ { { 3, 3 }, { 3, 7 }, { 7, 7 }, { 7, 3 } } }
	};
	const Ray2D fromHoleBoundary{ Vec2{ 3, 5 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(fromHoleBoundary, hole, 0.001, 10.0), fromHoleBoundary,
		Vec2{ 7, 5 }, Vec2{ -1, 0 }, 4.0, false);
	const Ray2D fromMaterial{ Vec2{ 1, 5 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(fromMaterial, hole, 3.0, 10.0), fromMaterial,
		Vec2{ 7, 5 }, Vec2{ -1, 0 }, 6.0, true);

	const Polygon concave{ Array<Vec2>{
		{ 0, 0 }, { 10, 0 }, { 10, 10 }, { 7, 10 }, { 7, 3 }, { 3, 3 }, { 3, 10 }, { 0, 10 } } };
	CheckHit(Geometry2D::Raycast(fromHoleBoundary, concave, 0.001, 10.0), fromHoleBoundary,
		Vec2{ 7, 5 }, Vec2{ -1, 0 }, 4.0, false);
}

TEST_CASE("Geometry2D.Raycast.Range.LowerBoundAndOverlap")
{
	const RectF rect{ 0, 0, 10, 10 };
	const Ray2D cross{ Vec2{ -5, 5 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(cross, rect, 5.0, 5.0), cross,
		Vec2{ 0, 5 }, Vec2{ -1, 0 }, 5.0, false);
	CHECK(not Geometry2D::Raycast(cross, rect, std::nextafter(5.0, Math::Inf), 6.0));
	const Ray2D boundary{ Vec2{ 0, 5 }, Vec2{ -1, 0 } };
	CHECK(not Geometry2D::Raycast(boundary, rect, 1e-16, 10.0));
	CHECK(not Geometry2D::Raycast(Ray2D{ Vec2{ 5, 0 }, Vec2{ 1, 0 } },
		Circle{ Vec2{ 0, 0 }, 5 }, 1e-16, 10.0));

	const Ray2D along{ Vec2{ -5, 0 }, Vec2{ 1, 0 } };
	for (const double upper : { 7.0, 12.0, 20.0 })
	{
		CheckHit(Geometry2D::Raycast(along, rect, 7.0, upper), along,
			Vec2{ 2, 0 }, Vec2{ 0, -1 }, 7.0, false);
	}
	CHECK(not Geometry2D::Raycast(along, rect, 16.0, 20.0));
	const Ray2D tangent{ Vec2{ -10, 5 }, Vec2{ 1, 0 } };
	const Circle circle{ Vec2{ 0, 0 }, 5 };
	CheckHit(Geometry2D::Raycast(tangent, circle, 10.0, 10.0), tangent,
		Vec2{ 0, 5 }, Vec2{ 0, 1 }, 10.0, false);
	CHECK(not Geometry2D::Raycast(tangent, circle, 10.001, 30.0));
}

TEST_CASE("Geometry2D.Raycast.Range.MultiPolygonAndDegeneration")
{
	const Polygon a = RectF{ 0, 0, 4, 4 }.asPolygon();
	const Polygon b = a.movedBy(8, 0);
	const Polygon segment = a.scaledFromOrigin(Vec2{ 0, 1 });
	const Ray2D ray{ Vec2{ -5, 2 }, Vec2{ 1, 0 } };
	for (const auto& multi : { MultiPolygon{ a, segment, b }, MultiPolygon{ b, segment, a } })
	{
		CheckHit(Geometry2D::Raycast(ray, multi, 10.0, 30.0), ray,
			Vec2{ 8, 2 }, Vec2{ -1, 0 }, 13.0, false);
		CheckHit(Geometry2D::Raycast(ray, multi, 14.0, 30.0), ray,
			Vec2{ 12, 2 }, Vec2{ 1, 0 }, 17.0, false);
	}
	const MultiPolygon thin{ RectF{ -10, -1, 10, 2 }.asPolygon(),
		RectF{ 1e-7, -1, 1e-7, 2 }.asPolygon() };
	const Ray2D fromWall{ Vec2{ 0, 0 }, Vec2{ 1, 0 } };
	CheckHit(Geometry2D::Raycast(fromWall, thin, 1e-8, 1.0), fromWall,
		Vec2{ 1e-7, 0 }, Vec2{ -1, 0 }, 1e-7, false, 1e-15);
	CHECK(not Geometry2D::Raycast(fromWall, thin, 1e-6, 1.0));
	CHECK(not Geometry2D::Raycast(ray, segment, 0.001, 30.0));
	CHECK(not Geometry2D::Raycast(ray, MultiPolygon{ segment, Polygon{} }, 0.001, 30.0));
	CHECK(not Geometry2D::Raycast(ray, RectF{ 0, 0, 0, 4 }, 0.001, 30.0));
	CHECK(not Geometry2D::Raycast(ray, Circle{ Vec2{ 0, 0 }, 0 }, 0.001, 30.0));
	CHECK(not Geometry2D::Raycast(ray, Polygon{}, 0.001, 30.0));
}

TEST_CASE("Geometry2D.Raycast.Range.SuperEllipseFourCrossings")
{
	const double inner = std::sqrt(0.125);
	const std::array<Vec2, 4> roots{
		Vec2{ -0.015625, 0.765625 }, Vec2{ 0.375 - inner, 0.375 + inner },
		Vec2{ 0.375 + inner, 0.375 - inner }, Vec2{ 0.765625, -0.015625 }
	};
	for (const double scale : { 0.001, 1.0, 1000.0 })
	for (const bool reversed : { false, true })
	{
		const SuperEllipse shape{ Vec2{ 7, -11 } * scale, SizeF{ scale, scale }, 0.5 };
		const Vec2 direction = (reversed ? Vec2{ -1, 1 } : Vec2{ 1, -1 }).normalized();
		const Vec2 first = roots[reversed ? 3 : 0];
		const Ray2D ray{ shape.center + first * scale - direction * (0.01 * scale), direction };
		double minimum = 0.005 * scale;
		for (size_t i = 0; i < roots.size(); ++i)
		{
			CAPTURE(scale, reversed, i);
			const Vec2 root = roots[reversed ? 3 - i : i];
			const Vec2 position = shape.center + root * scale;
			const Vec2 normal = Vec2{ std::copysign(1.0 / std::sqrt(Abs(root.x)), root.x),
				std::copysign(1.0 / std::sqrt(Abs(root.y)), root.y) }.normalized();
			const double distance = (position - ray.origin).dot(direction);
			CheckHit(Geometry2D::Raycast(ray, shape, minimum, Math::Inf), ray,
				position, normal, distance, false);
			minimum = distance + 0.01 * scale;
		}
		CHECK(not Geometry2D::Raycast(ray, shape, minimum, Math::Inf));
	}
}

TEST_CASE("Geometry2D.Raycast.Range.ReflectInsideHole")
{
	const Polygon hole{
		Array<Vec2>{ { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 } },
		Array<Array<Vec2>>{ { { 3, 3 }, { 3, 7 }, { 7, 7 }, { 7, 3 } } }
	};
	Vec2 position{ 5, 5 }, direction{ -1, 0 };
	double remaining = 10;
	int calls = 0;
	while ((0 < remaining) && (calls < 8))
	{
		const auto hit = Geometry2D::Raycast(Ray2D{ position, direction }, hole,
			(calls == 0) ? 0.0 : 0.001, remaining);
		++calls;
		if (not hit)
		{
			position += direction * remaining;
			remaining = 0;
			break;
		}
		position = hit->position;
		remaining -= hit->distance;
		direction -= 2 * direction.dot(hit->normal) * hit->normal;
	}
	CHECK(calls == 3);
	CHECK(remaining == 0);
	CHECK(position == Vec2{ 3, 5 });
}

TEST_CASE("Geometry2D.Raycast.Range.RoundRectArcRestart")
{
	const RoundRect shape{ RectF{ -5, -3, 10, 6 }, 2 };
	for (const Vec2 signs : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ -1, -1 }, Vec2{ 1, -1 } })
	{
		const Vec2 normal = signs.normalized();
		const Vec2 point = Vec2{ 3, 1 } * signs + normal * 2;
		const Ray2D ray{ point, -normal };
		const double minimum = 0.001;
		const auto next = Geometry2D::Raycast(ray, shape, minimum, 30.0);
		const Ray2D shifted{ ray.pointAt(minimum), ray.direction };
		const auto reference = Geometry2D::Raycast(shifted, shape, 30.0 - minimum);
		REQUIRE(reference);
		CheckHit(next, ray, reference->position, reference->normal, reference->distance + minimum, false);
		CHECK(not Geometry2D::Raycast(Ray2D{ point, normal }, shape, minimum, 30.0));
	}
}
