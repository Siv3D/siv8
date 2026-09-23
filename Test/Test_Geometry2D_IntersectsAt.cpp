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
	bool NearPoint(const Vec2& a, const Vec2& b, const double tolerance = 1.0e-7)
	{
		return (a.distanceFromSq(b) <= (tolerance * tolerance));
	}

	[[nodiscard]]
	bool ContainsNear(const Array<Vec2>& points, const Vec2& expected, const double tolerance = 1.0e-7)
	{
		for (const Vec2& point : points)
		{
			if (NearPoint(point, expected, tolerance))
			{
				return true;
			}
		}

		return false;
	}

	void CheckPointSet(const Optional<Array<Vec2>>& actual,
		const Array<Vec2>& expected, const double tolerance = 1.0e-7)
	{
		REQUIRE(actual.has_value());
		CHECK(actual->size() == expected.size());

		for (const Vec2& point : expected)
		{
			CHECK(ContainsNear(*actual, point, tolerance));
		}
	}
}

TEST_CASE("Geometry2D.IntersectsAt.OptionalSemantics")
{
	CHECK(not Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 1, 0 } },
		Line{ Vec2{ 0, 1 }, Vec2{ 1, 1 } }).has_value());

	const auto contained = Geometry2D::IntersectsAt(
		Line{ Vec2{ 2, 5 }, Vec2{ 8, 5 } },
		RectF{ 0, 0, 10, 10 });
	REQUIRE(contained.has_value());
	CHECK(contained->empty());

	CHECK(not Geometry2D::IntersectsAt(
		Circle{ Vec2{ 1, 1 }, 0 }, Vec2{ 1, 1 }).has_value());

	CHECK(not Geometry2D::IntersectsAt(
		LineString{}, RectF{ 0, 0, 10, 10 }).has_value());
}

TEST_CASE("Geometry2D.IntersectsAt.PointGeometry")
{
	CheckPointSet(Geometry2D::IntersectsAt(Vec2{ 5, 5 }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });
	CheckPointSet(Geometry2D::IntersectsAt(Line{ Vec2{ 5, 5 }, Vec2{ 5, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });
	CheckPointSet(Geometry2D::IntersectsAt(
		Triangle{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } },
		Circle{ Vec2{ 5, 5 }, 2 }),
		{ Vec2{ 5, 5 } });
	CheckPointSet(Geometry2D::IntersectsAt(
		Bezier2{ Vec2{ 5, 5 }, Vec2{ 5, 5 }, Vec2{ 5, 5 } },
		RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		LineString{ Vec2{ 5, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 5, 5 } });
}

TEST_CASE("Geometry2D.IntersectsAt.LineLine")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } },
		Line{ Vec2{ 5, -5 }, Vec2{ 5, 5 } }),
		{ Vec2{ 5, 0 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } },
		Line{ Vec2{ 10, 0 }, Vec2{ 20, 0 } }),
		{ Vec2{ 10, 0 } });

	const auto overlap = Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } },
		Line{ Vec2{ 5, 0 }, Vec2{ 15, 0 } });
	REQUIRE(overlap.has_value());
	CHECK(overlap->empty());

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 5, 0 }, Vec2{ 5, 0 } },
		Line{ Vec2{ 0, 0 }, Vec2{ 10, 0 } }),
		{ Vec2{ 5, 0 } });
}

TEST_CASE("Geometry2D.IntersectsAt.RectF")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 5, 5, 10, 10 }),
		{ Vec2{ 10, 5 }, Vec2{ 5, 10 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 10, 10, 5, 5 }),
		{ Vec2{ 10, 10 } });

	const auto sharedEdge = Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 10, 0, 5, 10 });
	REQUIRE(sharedEdge.has_value());
	CHECK(sharedEdge->empty());

	const auto identical = Geometry2D::IntersectsAt(
		RectF{ 0, 0, 10, 10 }, RectF{ 0, 0, 10, 10 });
	REQUIRE(identical.has_value());
	CHECK(identical->empty());

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -5, 5 }, Vec2{ 15, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 0, 5 }, Vec2{ 10, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -5, 5 }, Vec2{ 5, 5 } }, RectF{ 0, 0, 10, 10 }),
		{ Vec2{ 0, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		RectF{ 5, 0, 0, 10 }, RectF{ 0, 5, 10, 0 }),
		{ Vec2{ 5, 5 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Ellipse{ Vec2{ 5, 5 }, 0, 5 }, RectF{ 0, 5, 10, 0 }),
		{ Vec2{ 5, 5 } });
}

TEST_CASE("Geometry2D.IntersectsAt.Circle")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 10, 0 }, 5 }),
		{ Vec2{ 5, 0 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 6, 0 }, 5 }),
		{ Vec2{ 3, 4 }, Vec2{ 3, -4 } });

	const auto contained = Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 0, 0 }, 2 });
	REQUIRE(contained.has_value());
	CHECK(contained->empty());

	const auto identical = Geometry2D::IntersectsAt(
		Circle{ Vec2{ 0, 0 }, 5 }, Circle{ Vec2{ 0, 0 }, 5 });
	REQUIRE(identical.has_value());
	CHECK(identical->empty());
}

TEST_CASE("Geometry2D.IntersectsAt.PolygonalBoundaries")
{
	const Polygon square{ Array<Vec2>{
		Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }, square),
		{ Vec2{ 0, 5 }, Vec2{ 10, 5 } });

	const Polygon donut{
		Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } },
		Array<Array<Vec2>>{ Array<Vec2>{
			Vec2{ 3, 3 }, Vec2{ 3, 7 }, Vec2{ 7, 7 }, Vec2{ 7, 3 } } }
	};

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -1, 5 }, Vec2{ 11, 5 } }, donut),
		{ Vec2{ 0, 5 }, Vec2{ 3, 5 }, Vec2{ 7, 5 }, Vec2{ 10, 5 } });

	const Polygon touchingA{ Array<Vec2>{
		Vec2{ 0, 0 }, Vec2{ 2, 0 }, Vec2{ 2, 2 }, Vec2{ 0, 2 } } };
	const Polygon touchingB{ Array<Vec2>{
		Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const MultiPolygon multi{ touchingA, touchingB };

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 4 }, Vec2{ 4, 0 } }, multi),
		{ Vec2{ 2, 2 } });
}

TEST_CASE("Geometry2D.IntersectsAt.LineStringDeduplication")
{
	const LineString lineString{ Vec2{ -1, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 0 } };
	CheckPointSet(Geometry2D::IntersectsAt(
		lineString, Line{ Vec2{ 0, -1 }, Vec2{ 0, 1 } }),
		{ Vec2{ 0, 0 } });

	const auto identical = Geometry2D::IntersectsAt(lineString, lineString);
	REQUIRE(identical.has_value());
	CHECK(identical->empty());
}

TEST_CASE("Geometry2D.IntersectsAt.CurvedBoundaries")
{
	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } }, Ellipse{ Vec2{ 0, 0 }, 5, 3 }),
		{ Vec2{ -5, 0 }, Vec2{ 5, 0 } });

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -10, 0 }, Vec2{ 10, 0 } },
		SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 }),
		{ Vec2{ -5, 0 }, Vec2{ 5, 0 } }, 1.0e-6);

	const double dx = (2.0 - std::sqrt(3.0));
	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ -1, 1 }, Vec2{ 11, 1 } },
		RoundRect{ RectF{ 0, 0, 10, 10 }, 2 }),
		{ Vec2{ dx, 1 }, Vec2{ 10.0 - dx, 1 } }, 1.0e-6);

	CheckPointSet(Geometry2D::IntersectsAt(
		Line{ Vec2{ 0, 5 }, Vec2{ 10, 5 } },
		Bezier2{ Vec2{ 0, 0 }, Vec2{ 5, 10 }, Vec2{ 10, 0 } }),
		{ Vec2{ 5, 5 } }, 1.0e-6);

	const auto bezierBoundaryOverlap = Geometry2D::IntersectsAt(
		Bezier2{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } },
		RectF{ 0, 0, 10, 10 });
	REQUIRE(bezierBoundaryOverlap.has_value());
	CHECK(bezierBoundaryOverlap->empty());

	const auto identicalRoundRect = Geometry2D::IntersectsAt(
		RoundRect{ RectF{ 0, 0, 10, 10 }, 2 },
		RoundRect{ RectF{ 0, 0, 10, 10 }, 2 });
	REQUIRE(identicalRoundRect.has_value());
	CHECK(identicalRoundRect->empty());

	const auto equivalentCurves = Geometry2D::IntersectsAt(
		Ellipse{ Vec2{ 0, 0 }, 5, 3 },
		SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 2.0 });
	REQUIRE(equivalentCurves.has_value());
	CHECK(equivalentCurves->empty());
}

TEST_CASE("Geometry2D.IntersectsAt.Translation")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 1e10, -1e10 } })
	{
		const RectF a{ offset, 10, 10 };
		const RectF b{ offset + Vec2{ 5, 5 }, 10, 10 };
		CheckPointSet(a.intersectsAt(b), { offset + Vec2{ 10, 5 }, offset + Vec2{ 5, 10 } });
		const auto shared = a.intersectsAt(RectF{ offset + Vec2{ 10, 0 }, 10, 10 });
		REQUIRE(shared.has_value());
		CHECK(shared->empty());
		CheckPointSet(Line{ offset - Vec2{ 1, 1 }, offset + Vec2{ 11, 11 } }.intersectsAt(a),
			{ offset, offset + Vec2{ 10, 10 } });
	}
}

TEST_CASE("Geometry2D.IntersectsAt.Ellipse.NearContact")
{
	// Affine images of two circles give independently known ellipse intersections.
	for (const Vec2 stretch : { Vec2{ 1, 1 }, Vec2{ 1, 0.3 }, Vec2{ 0.1, 2 } })
	{
		for (const double scale : { 0.01, 1.0, 100.0 })
		{
			for (const Vec2 direction : { Vec2{ 0.6, 0.8 }, Vec2{ -0.6, 0.8 }, Vec2{ 0.6, -0.8 }, Vec2{ 0, 1 }, Vec2{ -1, 0 } })
			{
				const Vec2 offset = (Vec2{ 7, -11 } * scale);
				const Vec2 axes = (stretch * scale);
				const Ellipse a{ offset, (axes * 100.0) };
				for (const double distance : { 129.9999, 130.0, 70.0, 70.0001 })
				{
					CAPTURE(stretch, scale, direction, distance);
					const Ellipse b{ (offset + axes * direction * distance), (axes * 30.0) };
					const double along = ((100.0 * 100.0 - 30.0 * 30.0 + distance * distance) / (2.0 * distance));
					const double height = std::sqrt(Max(0.0, ((100.0 - along) * (100.0 + along))));
					const Vec2 perpendicular{ -direction.y, direction.x };
					const Vec2 p = (offset + axes * (direction * along + perpendicular * height));
					const Vec2 q = (offset + axes * (direction * along - perpendicular * height));
					const double tolerance = (scale * 1.0e-6);
					if ((distance == 130.0) || (distance == 70.0))
					{
						CheckPointSet(Geometry2D::IntersectsAt(a, b), { p }, tolerance);
						CheckPointSet(Geometry2D::IntersectsAt(b, a), { p }, tolerance);
					}
					else
					{
						CheckPointSet(Geometry2D::IntersectsAt(a, b), { p, q }, tolerance);
						CheckPointSet(Geometry2D::IntersectsAt(b, a), { p, q }, tolerance);
					}
				}
				CHECK(not Geometry2D::IntersectsAt(a,
					Ellipse{ (offset + axes * direction * 130.0001), (axes * 30.0) }));
				const auto contained = Geometry2D::IntersectsAt(a,
					Ellipse{ (offset + axes * direction * 69.9999), (axes * 30.0) });
				REQUIRE(contained.has_value());
				CHECK(contained->empty());
			}
		}
	}
}

TEST_CASE("Geometry2D.IntersectsAt.Ellipse.NonProportionalContact")
{
	for (const Vec2 axes : { Vec2{ 100, 30 }, Vec2{ 0.1, 600 }, Vec2{ 30, 70 } })
	{
		for (const double angle : { 0.0, 0.37, 1.1, (Math::HalfPi + 1.0e-7), 2.5 })
		{
			const Vec2 normal{ std::cos(angle), std::sin(angle) };
			const auto Support = [&](const Vec2& radii)
			{
				return (radii * radii * normal / (radii * normal).length());
			};
			const Ellipse a{ Vec2{ 0, 0 }, axes };
			const Vec2 otherAxes{ 40, 10 };
			const Vec2 point = Support(axes);
			const Ellipse tangent{ (point + Support(otherAxes)), otherAxes };
			CAPTURE(axes, angle);
			CheckPointSet(Geometry2D::IntersectsAt(a, tangent), { point }, 1.0e-6);
			CheckPointSet(Geometry2D::IntersectsAt(tangent, a), { point }, 1.0e-6);
			CHECK(not Geometry2D::IntersectsAt(a, tangent.movedBy(normal * 1.0e-5)));
			const Ellipse crossing = tangent.movedBy(normal * -1.0e-5);
			const auto points = Geometry2D::IntersectsAt(a, crossing);
			REQUIRE(points.has_value());
			CHECK(points->size() == 2);
			for (const Vec2& p : *points)
			{
				CHECK(Abs(((p - a.center) / a.axes).lengthSq() - 1.0) < 1.0e-10);
				CHECK(Abs(((p - crossing.center) / crossing.axes).lengthSq() - 1.0) < 1.0e-10);
			}
		}
	}
}

TEST_CASE("Geometry2D.IntersectsAt.Ellipse.MultipleRoots")
{
	const double coordinate = (1.0 / std::sqrt(1.0 / 10000.0 + 1.0 / 900.0));
	CheckPointSet(Geometry2D::IntersectsAt(Ellipse{ 0, 0, 100, 30 }, Ellipse{ 0, 0, 30, 100 }),
		{ { coordinate, coordinate }, { coordinate, -coordinate }, { -coordinate, coordinate }, { -coordinate, -coordinate } });
	CheckPointSet(Geometry2D::IntersectsAt(Ellipse{ 0, 0, 100, 30 }, Ellipse{ 0, 0, 100, 15 }),
		{ { -100, 0 }, { 100, 0 } });
	const double y = (4.0 * std::sqrt(14.0) / 15.0);
	CheckPointSet(Geometry2D::IntersectsAt(Ellipse{ 0, 0, 2, 1 }, Ellipse{ 1, 0, 1, 2 }),
		{ { 2, 0 }, { 2.0 / 15.0, y }, { 2.0 / 15.0, -y } });
	const auto identical = Geometry2D::IntersectsAt(Ellipse{ 0, 0, 5, 3 }, Ellipse{ 0, 0, 5, 3 });
	REQUIRE(identical.has_value());
	CHECK(identical->empty());
}

TEST_CASE("Geometry2D.IntersectsAt.Ellipse.CircleArcs")
{
	const Circle circle{ 0, 0, 100 };
	const Ellipse circular{ 0, 0, 100, 100 };
	const Vec2 direction{ 0.6, 0.8 };
	for (const double distance : { 129.9999, 130.0, 70.0, 70.0001 })
	{
		const Ellipse other{ (direction * distance), 30, 30 };
		const auto expected = Geometry2D::IntersectsAt(circular, other);
		REQUIRE(expected.has_value());
		const auto actual = Geometry2D::IntersectsAt(circle, other);
		REQUIRE(actual.has_value());
		CHECK(actual->size() == expected->size());
		for (const Vec2& point : *expected)
		{
			CHECK(ContainsNear(*actual, point));
		}
	}
	const auto coincident = Geometry2D::IntersectsAt(circle, circular);
	REQUIRE(coincident.has_value());
	CHECK(coincident->empty());
	for (const double angle : { 0.1, 0.37, 1.1, 2.5, 4.2 })
	{
		const Vec2 normal{ std::cos(angle), std::sin(angle) };
		const Vec2 support = (Vec2{ 9.0 * normal.x, normal.y } / std::hypot(3.0 * normal.x, normal.y));
		const Circle small{ 0, 0, 2 };
		const Ellipse tangent{ (normal * 2.0 + support), 3, 1 };
		CheckPointSet(Geometry2D::IntersectsAt(small, tangent), { (normal * 2.0) });
		CheckPointSet(Geometry2D::IntersectsAt(tangent, small), { (normal * 2.0) });
		CHECK(not Geometry2D::IntersectsAt(small, tangent.movedBy(normal * 1.0e-5)));
		const auto crossing = Geometry2D::IntersectsAt(small, tangent.movedBy(normal * -1.0e-5));
		REQUIRE(crossing.has_value());
		CHECK(crossing->size() == 2);
	}

	const RoundRect rounded{ -10, -10, 20, 20, 4 };
	for (const Vec2 normal : { Vec2{ 0.6, 0.8 }, Vec2{ -0.6, 0.8 }, Vec2{ -0.6, -0.8 }, Vec2{ 0.6, -0.8 } })
	{
		const Vec2 corner{ ((normal.x < 0.0) ? -6.0 : 6.0), ((normal.y < 0.0) ? -6.0 : 6.0) };
		const Circle arcCircle{ corner, 4 };
		const Ellipse other{ (corner + normal * 6.9999), 3, 3 };
		const auto expected = Geometry2D::IntersectsAt(arcCircle, other);
		const auto actual = Geometry2D::IntersectsAt(rounded, other);
		REQUIRE(expected.has_value());
		REQUIRE(actual.has_value());
		CHECK(expected->size() == 2);
		CHECK(actual->size() == 2);
		for (const Vec2& point : *expected)
		{
			CHECK(ContainsNear(*actual, point));
		}
	}
}

TEST_CASE("Geometry2D.IntersectsAt.Bezier.SimpleGeometry")
{
	const auto Elevate = [](const Bezier2& q)
	{
		return Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 };
	};
	const Bezier2 arch{ { -1, 0 }, { 0, 0.2 }, { 1, 0 } };
	const auto CheckPair = [](const auto& a, const auto& b, const Array<Vec2>& expected)
	{
		CheckPointSet(Geometry2D::IntersectsAt(a, b), expected, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(b, a), expected, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(a.reversed(), b), expected, 1.0e-10);
	};
	for (const double y : { 0.099, 0.1 })
	{
		const Bezier2 straight{ { -0.2, y }, { 0, y }, { 0.2, y } };
		const Array<Vec2> expected = ((y == 0.1) ? Array<Vec2>{ { 0, y } } : Array<Vec2>{ { -0.1, y }, { 0.1, y } });
		CheckPair(arch, straight, expected);
		CheckPair(arch, Elevate(straight), expected);
		CheckPair(Elevate(arch), straight, expected);
		CheckPair(Elevate(arch), Elevate(straight), expected);
	}
	const Vec2 point = arch.pointAt(0.371);
	CheckPair(arch, Bezier2{ point, point, point }, { point });
	CheckPair(Elevate(arch), Bezier3{ point, point, point, point }, { point });
	const Bezier2 retracing{ { 0, 0 }, { 100, 0 }, { 0, 0 } };
	CheckPair(retracing, Bezier3{ { 50, 0 }, { 51, 0 }, { 52, 0 }, { 53, 0 } }, { { 50, 0 } });
	const auto overlap = Geometry2D::IntersectsAt(retracing, Bezier2{ { 49, 0 }, { 50, 0 }, { 51, 0 } });
	REQUIRE(overlap.has_value());
	CHECK(overlap->empty());
	CHECK(not Geometry2D::IntersectsAt(arch, Bezier2{ { -0.2, 0 }, { 0, 0 }, { 0.2, 0 } }));

	// Both roots fall within one interval of the former uniform sampling.
	const double t = 0.371, gap = 1.0e-8;
	const Bezier2 narrow{ { 0, t * t - gap }, { 0.5, t * t - t - gap }, { 1, (1 - t) * (1 - t) - gap } };
	CheckPointSet(Geometry2D::IntersectsAt(Line{ 0, 0, 1, 0 }, narrow), { { t - 0.0001, 0 }, { t + 0.0001, 0 } }, 1.0e-10);
	// Cubic y(t) = (t - .2)(t - .201)(t - .8), x(t) = t.
	const double d = -0.2 * 0.201 * 0.8, c = (0.2 * 0.201 + 0.2 * 0.8 + 0.201 * 0.8), b = -(0.2 + 0.201 + 0.8);
	const Bezier3 cubic{ { 0, d }, { 1.0 / 3, d + c / 3 }, { 2.0 / 3, d + 2 * c / 3 + b / 3 }, { 1, d + c + b + 1 } };
	CheckPointSet(Geometry2D::IntersectsAt(Line{ 0, 0, 1, 0 }, cubic), { { 0.2, 0 }, { 0.201, 0 }, { 0.8, 0 } }, 1.0e-10);

	CheckPair(Bezier3{ { 20, 76 }, { -63, 13 }, { -18, -66 }, { 43, 60 } },
		Bezier3{ { 2, -2 }, { 2, 2 }, { 2, 5 }, { 2, 3 } }, { { 2, 0.484375 } });

	const SuperEllipse diamond{ 0, 0.1, 0.01, 0.001, 1 };
	const double x = (0.02 / (1.0 + std::sqrt(1.04)));
	const Array<Vec2> expected{ { -x, 0.1 * (1 - x * x) }, { x, 0.1 * (1 - x * x) } };
	CheckPair(arch, diamond, expected);
	CheckPair(Elevate(arch), diamond, expected);
	CheckPair(arch, SuperEllipse{ 0, 0.11, 0.02, 0.01, 1 }, { { 0, 0.1 } });
}

TEST_CASE("Geometry2D.IntersectsAt.Bezier.GeneralPairs")
{
	const auto Elevate = [](const Bezier2& q)
	{
		return Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 };
	};
	const auto CheckPair = [](const auto& a, const auto& b, const Array<Vec2>& expected, const double scale = 1.0)
	{
		for (const auto& aa : { a, a.reversed() })
		{
			for (const auto& bb : { b, b.reversed() })
			{
				for (const auto& points : { Geometry2D::IntersectsAt(aa, bb), Geometry2D::IntersectsAt(bb, aa) })
				{
					CheckPointSet(points, expected, (scale * 1.0e-7));
					REQUIRE(points.has_value());
					for (const Vec2& p : *points)
					{
						CHECK(Geometry2D::Distance(p, a) <= (scale * 1.0e-11));
						CHECK(Geometry2D::Distance(p, b) <= (scale * 1.0e-11));
					}
				}
			}
		}
	};
	const auto CheckDegrees = [&](const Bezier2& a, const Bezier2& b, const Array<Vec2>& expected, const double scale)
	{
		CheckPair(a, b, expected, scale);
		CheckPair(a, Elevate(b), expected, scale);
		CheckPair(Elevate(a), b, expected, scale);
		CheckPair(Elevate(a), Elevate(b), expected, scale);
	};
	const Bezier2 a{ { -1, 0.1 }, { 0, -0.1 }, { 1, 0.1 } };
	constexpr double X = 0.27;
	const Bezier2 tangent{ { -1, -0.1 - 0.4 * X - 0.2 * X * X },
		{ 0, 0.1 - 0.2 * X * X }, { 1, -0.1 + 0.4 * X - 0.2 * X * X } };
	for (const double scale : { 1.0e-6, 1.0, 1.0e6 })
	{
		const Vec2 offset = (Vec2{ 13, -7 } * scale);
		const auto Transform = [&](const Bezier2& q)
		{
			return Bezier2{ q.p0 * scale + offset, q.p1 * scale + offset, q.p2 * scale + offset };
		};
		const auto aa = Transform(a), bb = Transform(tangent);
		CheckDegrees(aa, bb, { Vec2{ X, 0.1 * X * X } * scale + offset }, scale);
		// Both crossings lie between adjacent samples of the former polyline.
		const double delta = std::sqrt(5.0e-8);
		const double x0 = (X - delta), x1 = (X + delta);
		CheckDegrees(aa, Transform(tangent.movedBy(0, 1.0e-8)),
			{ Vec2{ x0, 0.1 * x0 * x0 } * scale + offset, Vec2{ x1, 0.1 * x1 * x1 } * scale + offset }, scale);
		CHECK(not Geometry2D::IntersectsAt(aa, Transform(tangent.movedBy(0, -1.0e-8))));
		CheckDegrees(aa, aa, {}, scale);
		CheckDegrees(aa, Transform(a.split(0.371).second), {}, scale);
		CheckDegrees(aa, Transform(Bezier2{ a.p2, { 2, -1 }, { 3, 1 } }), { aa.p2 }, scale);
	}

	// A(t)=(T3(u),u), B(s)=(v,T3(v)), u=2t-1, v=2s-1, have nine intersections.
	const Bezier3 nineA{ { -1, -1 }, { 5, -1.0 / 3 }, { -5, 1.0 / 3 }, { 1, 1 } };
	const Bezier3 nineB{ { -1, -1 }, { -1.0 / 3, 5 }, { 1.0 / 3, -5 }, { 1, 1 } };
	Array<Vec2> ninePoints;
	for (const double u : { -1.0, -std::cos(Math::Pi / 5), -std::cos(Math::Pi / 4), -std::cos(2 * Math::Pi / 5),
		0.0, std::cos(2 * Math::Pi / 5), std::cos(Math::Pi / 4), std::cos(Math::Pi / 5), 1.0 })
	{
		ninePoints.push_back({ (4 * u * u * u - 3 * u), u });
	}
	CheckPair(nineA, nineB, ninePoints);
	// yB(t)-yA(t)=(t-.2)(t-.201)(t-.8), with xA(t)=xB(t)=t.
	const double d = (-0.2 * 0.201 * 0.8), c = (0.2 * 0.201 + 0.2 * 0.8 + 0.201 * 0.8), b = -(0.2 + 0.201 + 0.8);
	CheckPair(Bezier2{ { 0, 0 }, { 0.5, 0 }, { 1, 1 } },
		Bezier3{ { 0, d }, { 1.0 / 3, d + c / 3 }, { 2.0 / 3, d + 2 * c / 3 + (b + 1) / 3 }, { 1, d + c + b + 2 } },
		{ { 0.2, 0.04 }, { 0.201, 0.040401 }, { 0.8, 0.64 } });
	const Bezier3 loop{ { 0, 0 }, { 3, 4 }, { -3, 4 }, { 0, 0 } };
	CheckPair(loop, loop.split(0.371).second, {});
	CheckPair(loop, Bezier3{ loop.pointAt(0.23), { 4, 2 }, { 2, -1 }, { 4, -3 } }, { loop.pointAt(0.23) });
	const Bezier3 cusp{ { 0.25, -0.125 }, { -1.0 / 12, 0.125 }, { -1.0 / 12, -0.125 }, { 0.25, 0.125 } };
	CheckPair(cusp, Bezier2{ { -1, 1 }, { 0, -1 }, { 1, 1 } }, { { 0, 0 } });
}

TEST_CASE("Geometry2D.IntersectsAt.Bezier.EllipseBoundary")
{
	const auto Elevate = [](const Bezier2& q)
	{
		return Bezier3{ q.p0, q.p0 + (q.p1 - q.p0) * (2.0 / 3), q.p2 + (q.p1 - q.p2) * (2.0 / 3), q.p2 };
	};
	const auto CheckBoundary = [](const auto& curve, const auto& shape, const Ellipse& boundary, const size_t count)
	{
		for (const auto& c : { curve, curve.reversed() })
		{
			for (const auto& points : { Geometry2D::IntersectsAt(c, shape), Geometry2D::IntersectsAt(shape, c), c.intersectsAt(shape) })
			{
				REQUIRE(points.has_value());
				CHECK(points->size() == count);
				for (const Vec2& p : *points)
				{
					const Vec2 normalized = ((p - boundary.center) / boundary.axes);
					CHECK(Abs(normalized.lengthSq() - 1.0) < 1.0e-7);
					CHECK(Geometry2D::Distance(p, curve) <= (Max(boundary.a, boundary.b) * 1.0e-7));
				}
			}
		}
	};
	const Bezier2 arch{ { -1, 0 }, { 0, 0.25 }, { 1, 0 } };
	for (const double scale : { 1.0e-12, 1.0e-6, 1.0, 1.0e6 })
	{
		const Vec2 offset = (Vec2{ 13, -7 } * scale);
		const Bezier2 curve{ arch.p0 * scale + offset, arch.p1 * scale + offset, arch.p2 * scale + offset };
		const Vec2 center = curve.pointAt(0.371);
		const Circle circle{ center, (1.0e-5 * scale) };
		const Ellipse ellipse{ center, (1.0e-4 * scale), (1.0e-5 * scale) };
		// Both boundary crossings fall within one old sampling interval.
		CheckBoundary(curve, circle, Ellipse{ center, circle.r, circle.r }, 2);
		CheckBoundary(Elevate(curve), circle, Ellipse{ center, circle.r, circle.r }, 2);
		CheckBoundary(curve, ellipse, ellipse, 2);
		CheckBoundary(Elevate(curve), ellipse, ellipse, 2);
		CheckBoundary(curve, SuperEllipse{ ellipse, 2 }, ellipse, 2);
		CheckBoundary(Elevate(curve), SuperEllipse{ ellipse, 2 }, ellipse, 2);
		const auto points = Geometry2D::IntersectsAt(curve, ellipse);
		REQUIRE(points.has_value());
		REQUIRE(points->size() == 2);
		CHECK(((*points)[0].x - center.x) * ((*points)[1].x - center.x) < 0.0);
	}
	for (const double t : { 0.25, 0.371, 0.713 })
	{
		const Bezier2 tangent{ { 0, 1 + t * t }, { 0.5, 1 + t * t - t }, { 1, 1 + (1 - t) * (1 - t) } };
		const Circle circle{ t, 0, 1 };
		const Ellipse ellipse{ t, 0, 0.25, 1 };
		CheckBoundary(tangent, circle, Ellipse{ t, 0, 1, 1 }, 1);
		CheckBoundary(Elevate(tangent), ellipse, ellipse, 1);
		CheckPointSet(Geometry2D::IntersectsAt(tangent, circle), { { t, 1 } }, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(tangent, ellipse), { { t, 1 } }, 1.0e-10);
		CHECK(not Geometry2D::IntersectsAt(tangent.movedBy(0, 1.0e-6), circle));
	}
	// x=2*T3(u), y=u/10 has six distinct crossings of the unit circle.
	const Bezier3 six{ { -2, -0.1 }, { 10, -1.0 / 30 }, { -10, 1.0 / 30 }, { 2, 0.1 } };
	CheckBoundary(six, Circle{ 0, 0, 1 }, Ellipse{ 0, 0, 1, 1 }, 6);
	CheckBoundary(six, SuperEllipse{ 0, 0, 1, 1, 2 }, Ellipse{ 0, 0, 1, 1 }, 6);
	CheckBoundary(six, RoundRect{ -1, -1, 2, 2, 1 }, Ellipse{ 0, 0, 1, 1 }, 6);
	CheckPointSet(Geometry2D::IntersectsAt(Bezier2{ { -2, 0 }, { 0, 0 }, { 2, 0 } }, RoundRect{ -1, -1, 2, 2, 1 }),
		{ { -1, 0 }, { 1, 0 } });
	CheckPointSet(Geometry2D::IntersectsAt(Bezier2{ { 1, 0 }, { 2, 2 }, { 3, 0 } }, Circle{ 0, 0, 1 }), { { 1, 0 } });
	CheckPointSet(Geometry2D::IntersectsAt(arch, Circle{ 0, 0, 2 }), {});
	CheckPointSet(Geometry2D::IntersectsAt(Bezier2{ { 0, 0 }, { 0, 0 }, { 0, 0 } }, Ellipse{ 0, 0, 1, 2 }), { { 0, 0 } });
	CHECK(not Geometry2D::IntersectsAt(arch, Circle{ 0, 0, 0 }));
	CHECK(not Geometry2D::IntersectsAt(arch, Ellipse{ 0, 0, 0, 0 }));
	CheckPointSet(Geometry2D::IntersectsAt(arch, SuperEllipse{ 0, 0, 0, 1, 2 }), { { 0, 0.125 } });
	CheckPointSet(Geometry2D::IntersectsAt(Circle{ 0, 0, 1 }, SuperEllipse{ 0, 0, 1, 1, 2 }), {});
}

TEST_CASE("Geometry2D.IntersectsAt.Bezier.SmallLinearBoundaries")
{
	const Bezier2 base{ { 0, 0.1875 }, { 0.5, -0.3125 }, { 1, 0.1875 } }; // y=x*x-x+3/16
	for (const double scale : { 1.0e-12, 1.0e-6, 1.0e-3, 1.0, 1.0e6 })
	{
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 13, -7 } * scale })
		{
			const auto Transform = [&](const Vec2& p) { return (p * scale + offset); };
			const Bezier2 curve{ Transform(base.p0), Transform(base.p1), Transform(base.p2) };
			const Bezier3 cubic{ curve.p0, curve.p0 + (curve.p1 - curve.p0) * (2.0 / 3),
				curve.p2 + (curve.p1 - curve.p2) * (2.0 / 3), curve.p2 };
			const auto CheckShape = [&](const auto& shape, const Array<Vec2>& expected)
			{
				Array<Vec2> points;
				for (const Vec2& p : expected)
				{
					points.push_back(Transform(p));
				}
				const auto CheckCurve = [&](const auto& c)
				{
					CHECK(Geometry2D::Intersects(c, shape));
					CheckPointSet(Geometry2D::IntersectsAt(c, shape), points, (scale * 1.0e-12));
					CheckPointSet(Geometry2D::IntersectsAt(shape, c), points, (scale * 1.0e-12));
					CheckPointSet(c.intersectsAt(shape), points, (scale * 1.0e-12));
				};
				CheckCurve(curve);
				CheckCurve(curve.reversed());
				CheckCurve(cubic);
				CheckCurve(cubic.reversed());
			};
			// The other supporting-line root at x=.75 lies outside this edge.
			const Line edge{ Transform({ 0.2, 0 }), Transform({ 0.3, 0 }) };
			CheckShape(edge, { { 0.25, 0 } });
			CheckShape(Line{ edge.end, edge.start }, { { 0.25, 0 } });
			CheckShape(LineString{ Transform({ 0.2, 0 }), Transform({ 0.25, 0 }), Transform({ 0.3, 0 }) }, { { 0.25, 0 } });
			CheckShape(Triangle{ Transform({ 0.2, -0.1 }), Transform({ 0.3, -0.1 }), Transform({ 0.2, 0.1 }) },
				{ { 0.2, 0.0275 }, { 0.25, 0 } });
			const RectF rect{ Transform({ 0.2, -0.1 }), (0.1 * scale), (0.2 * scale) };
			const Array<Vec2> expected{ { 0.2, 0.0275 }, { 0.3, -0.0225 } };
			CheckShape(rect, expected);
			CheckShape(Quad{ rect.tl(), rect.tr(), rect.br(), rect.bl() }, expected);
			CheckShape(RoundRect{ rect, (0.01 * scale) }, expected);
			const Polygon polygon = Polygon{ { { 0.2, -0.1 }, { 0.3, -0.1 }, { 0.3, 0.1 }, { 0.2, 0.1 } } }.scaledFromOrigin(scale).movedBy(offset);
			CheckShape(polygon, expected);
			CheckShape(MultiPolygon{ polygon }, expected);
			// A root just outside a short edge must not turn into a contact at small scales.
			const Line outside{ Transform({ 0.2501, 0 }), Transform({ 0.3, 0 }) };
			CHECK(not Geometry2D::Intersects(curve, outside));
			CHECK(not Geometry2D::IntersectsAt(curve, outside));
			CHECK(not Geometry2D::Intersects(cubic, outside));
			CHECK(not Geometry2D::IntersectsAt(cubic, outside));
			// Translation puts a genuine root at the origin, where coordinate magnitude alone gives no tolerance.
			CheckPointSet(Geometry2D::IntersectsAt(curve.movedBy(-Transform({ 0.25, 0 })), Line{ { 0, 0 }, { (0.05 * scale), 0 } }),
				{ { 0, 0 } }, (scale * 1.0e-12));
		}
	}
}

TEST_CASE("Geometry2D.IntersectsAt.SmallLinearBoundaryEvents")
{
	for (const double scale : { 1.0e-12, 1.0e-6, 1.0, 1.0e6 })
	{
		const Bezier2 arch{ { -scale, 0 }, { 0, 0.2 * scale }, { scale, 0 } };
		const double x = (0.02 / (1.0 + std::sqrt(1.04)));
		CheckPointSet(Geometry2D::IntersectsAt(arch, SuperEllipse{ 0, (0.1 * scale), (0.01 * scale), (0.001 * scale), 1 }),
			{ Vec2{ -x, 0.1 * (1 - x * x) } * scale, Vec2{ x, 0.1 * (1 - x * x) } * scale }, (scale * 1.0e-12));
		const LineString a{ Vec2{ 0, 0 }, Vec2{ 0.5, 0 } * scale, Vec2{ 0.5, 1 } * scale };
		const LineString b{ Vec2{ 0.25, 0 } * scale, Vec2{ 0.75, 0 } * scale,
			Vec2{ 0.75, 0.25 } * scale, Vec2{ 0.25, 0.25 } * scale };
		// Shared segments suppress their own points, not a nearby isolated crossing.
		CheckPointSet(Geometry2D::IntersectsAt(a, b), { Vec2{ 0.5, 0.25 } * scale }, (scale * 1.0e-12));
		CheckPointSet(Geometry2D::IntersectsAt(b, a), { Vec2{ 0.5, 0.25 } * scale }, (scale * 1.0e-12));
		CheckPointSet(Geometry2D::IntersectsAt(Line{ { 0, 0 }, { scale, 0 } }, RectF{ (0.25 * scale), -scale, (0.5 * scale), (2 * scale) }),
			{ Vec2{ 0.25, 0 } * scale, Vec2{ 0.75, 0 } * scale }, (scale * 1.0e-12));
	}
}

TEST_CASE("Geometry2D.IntersectsAt.CurvedBoundaryScale")
{
	for (const double scale : { 1.0e-12, 1.0e-6, 1.0, 1.0e6 })
	{
		const Vec2 offset = (Vec2{ 13, -7 } * scale);
		const auto P = [&](const double x, const double y) { return (Vec2{ x, y } * scale + offset); };
		const double h = std::sqrt(0.75);
		const Circle circle{ offset, scale }, other{ P(1, 0), scale };
		CheckPointSet(Geometry2D::IntersectsAt(circle, other), { P(0.5, -h), P(0.5, h) }, (scale * 1.0e-12));
		CheckPointSet(Geometry2D::IntersectsAt(Ellipse{ offset, 2 * scale, scale }, Ellipse{ P(2, 0), 2 * scale, scale }),
			{ P(1, -h), P(1, h) }, (scale * 1.0e-12));
		const Line crossing{ P(-2, 0), P(2, 0) };
		CheckPointSet(Geometry2D::IntersectsAt(crossing, circle), { P(-1, 0), P(1, 0) }, (scale * 1.0e-12));
		CheckPointSet(Geometry2D::IntersectsAt(crossing, Ellipse{ offset, scale, 2 * scale }), { P(-1, 0), P(1, 0) }, (scale * 1.0e-12));
		CheckPointSet(Geometry2D::IntersectsAt(Line{ P(-2, 1), P(2, 1) }, circle), { P(0, 1) }, (scale * 1.0e-12));
		const RoundRect rounded{ RectF{ offset, 10 * scale, 10 * scale }, 2 * scale };
		const double x = (2 - std::sqrt(3.0));
		CheckPointSet(Geometry2D::IntersectsAt(Line{ P(-1, 1), P(11, 1) }, rounded), { P(x, 1), P(10 - x, 1) }, (scale * 1.0e-12));
		// A shared top-left arc must not suppress a crossing on another boundary.
		CheckPointSet(Geometry2D::IntersectsAt(rounded, RoundRect{ RectF{ offset, 12 * scale, 6 * scale }, 2 * scale }),
			{ P(10, 6) }, (scale * 1.0e-12));
		const SuperEllipse diamond{ offset, scale, scale, 1 };
		const double small = ((1 - std::sqrt(0.28)) * 0.5), large = (1 - small);
		Array<Vec2> expected;
		for (const double sx : { -1.0, 1.0 })
		{
			for (const double sy : { -1.0, 1.0 })
			{
				expected.push_back(P(sx * small, sy * large));
				expected.push_back(P(sx * large, sy * small));
			}
		}
		CheckPointSet(Geometry2D::IntersectsAt(diamond, Circle{ offset, 0.8 * scale }), expected, (scale * 1.0e-12));
		CheckPointSet(Geometry2D::IntersectsAt(Ellipse{ offset, 0.8 * scale, 0.8 * scale }, diamond), expected, (scale * 1.0e-12));
	}
	const Vec2 offset{ 1.0e8, -1.0e8 };
	const RoundRect translated{ RectF{ offset, 0.1, 0.1 }, 0.02 };
	const double x = (0.02 - std::sqrt(0.0003));
	CheckPointSet(Geometry2D::IntersectsAt(Line{ offset + Vec2{ -0.01, 0.01 }, offset + Vec2{ 0.11, 0.01 } }, translated),
		{ offset + Vec2{ x, 0.01 }, offset + Vec2{ 0.1 - x, 0.01 } }, 1.0e-7);
	const Line longLine{ -1000, 0, 1000, 0 };
	CheckPointSet(Geometry2D::IntersectsAt(longLine, Circle{ 0.371, 0, 1.0e-5 }),
		{ { 0.371 - 1.0e-5, 0 }, { 0.371 + 1.0e-5, 0 } }, 1.0e-12);
}

TEST_CASE("Geometry2D.IntersectsAt.LineSuperEllipse")
{
	for (const double n : { 0.25, 0.5, 0.999, 1.001, 1.5, 4.0, 16.0 })
	{
		for (const double scale : { 1.0e-12, 1.0e-6, 1.0, 1.0e6 })
		{
			const double x = (0.001 * std::pow((1.0 - std::pow((0.123 / 0.124), n)), (1.0 / n)));
			const SuperEllipse shape{ 0, 0, 0.001 * scale, 0.124 * scale, n };
			const Line line{ -scale, 0.123 * scale, scale, 0.123 * scale };
			const Array<Vec2> expected{ Vec2{ -x, 0.123 } * scale, Vec2{ x, 0.123 } * scale };
			CheckPointSet(Geometry2D::IntersectsAt(line, shape), expected, (scale * 1.0e-13));
			CheckPointSet(Geometry2D::IntersectsAt(shape, Line{ line.end, line.start }), expected, (scale * 1.0e-13));
			CheckPointSet(Geometry2D::IntersectsAt(Line{ -scale, 0.124 * scale, scale, 0.124 * scale }, shape),
				{ { 0, 0.124 * scale } }, (scale * 1.0e-13));
		}
	}
	// A concave boundary can cross a segment four times, with a maximum between the inner roots.
	const double inner = std::sqrt(0.125);
	CheckPointSet(Geometry2D::IntersectsAt(Line{ -1, 1.75, 1.75, -1 }, SuperEllipse{ 0, 0, 1, 1, 0.5 }),
		{ { -0.015625, 0.765625 }, { 0.765625, -0.015625 }, { 0.375 - inner, 0.375 + inner }, { 0.375 + inner, 0.375 - inner } }, 1.0e-12);
}

TEST_CASE("Geometry2D.IntersectsAt.SuperEllipse.CurvedSearch")
{
	for (const double n : { 0.5, 0.75, 1.5, 4.0, 16.0 })
	{
		for (const double scale : { 1.0e-6, 1.0, 1.0e6 })
		{
			const Bezier2 curve{ Vec2{ -1, 0 } * scale, Vec2{ 0, 0.25 } * scale, Vec2{ 1, 0 } * scale };
			const Bezier3 cubic{ curve.p0, curve.p0 + (curve.p1 - curve.p0) * (2.0 / 3),
				curve.p2 + (curve.p1 - curve.p2) * (2.0 / 3), curve.p2 };
			const SuperEllipse shape{ curve.pointAt(0.371), 1.0e-4 * scale, 1.0e-5 * scale, n };
			const auto CheckCurve = [&](const auto& c)
			{
				CAPTURE(n, scale);
				const auto points = Geometry2D::IntersectsAt(c, shape);
				REQUIRE(points.has_value());
				REQUIRE(points->size() == 2);
				for (const Vec2& p : *points)
				{
					const Vec2 q = ((p - shape.center) / shape.axes);
					CHECK(Abs(std::pow(Abs(q.x), n) + std::pow(Abs(q.y), n) - 1.0) < 1.0e-7);
					CHECK(Geometry2D::Distance(p, c) < (scale * 1.0e-12));
				}
				CheckPointSet(Geometry2D::IntersectsAt(shape, c), *points, (scale * 1.0e-12));
				CheckPointSet(Geometry2D::IntersectsAt(c.reversed(), shape), *points, (scale * 1.0e-12));
			};
			CheckCurve(curve);
			CheckCurve(cubic);
		}
		const double y = std::pow(1.0 - std::pow(0.5, n), (1.0 / n));
		const SuperEllipse a{ 0, 0, 1, 1, n }, b{ 1, 0, 1, 1, n };
		CheckPointSet(Geometry2D::IntersectsAt(a, b), { { 0.5, y }, { 0.5, -y } }, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(b, a), { { 0.5, y }, { 0.5, -y } }, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(a, Circle{ 0, 0, 1 }), { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } }, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(a, Ellipse{ 0, 0, 1, 1 }), { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } }, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(a, RoundRect{ -1, -1, 2, 2, 1 }), { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } }, 1.0e-10);
		CheckPointSet(Geometry2D::IntersectsAt(a, a), {});
	}
	const double u = std::pow(0.5, 0.25), t = 0.371;
	const Vec2 p{ u, u }, direction{ 1, -1 }, normal{ 1, 1 };
	const Vec2 start = (p - direction * t + normal * (t * t));
	const Bezier2 tangent{ start, start + direction * 0.5 - normal * t, p + direction * (1 - t) + normal * ((1 - t) * (1 - t)) };
	CheckPointSet(Geometry2D::IntersectsAt(tangent, SuperEllipse{ 0, 0, 1, 1, 4 }), { p }, 1.0e-7);
	CHECK(not Geometry2D::IntersectsAt(tangent.movedBy(normal * 1.0e-5), SuperEllipse{ 0, 0, 1, 1, 4 }));
	CheckPointSet(Geometry2D::IntersectsAt(Bezier2{ { 0, 1 }, { 0, 0 }, { 1, 0 } }, SuperEllipse{ 0, 0, 1, 1, 0.5 }), {});
}
