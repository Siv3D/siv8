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
