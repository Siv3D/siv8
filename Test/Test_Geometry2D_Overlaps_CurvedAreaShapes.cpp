//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Overlaps.hpp>

TEST_CASE("Geometry2D.Overlaps.Ellipse")
{
	const Ellipse ellipse{ Vec2{ 0, 0 }, 5, 3 };

	CHECK(Geometry2D::Overlaps(RectF{ -2, -2, 4, 4 }, ellipse));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, -2, 4, 4 }, ellipse));
	CHECK(Geometry2D::Intersects(RectF{ 5, -2, 4, 4 }, ellipse));

	CHECK(Geometry2D::Overlaps(Circle{ Vec2{ 9, 0 }, 5 }, ellipse));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 10, 0 }, 5 }, ellipse));
	CHECK(Geometry2D::Intersects(Circle{ Vec2{ 10, 0 }, 5 }, ellipse));

	CHECK(Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 9, 0 }, 5, 3 }));
	CHECK(not Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 10, 0 }, 5, 3 }));
	CHECK(Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 0, 0 }, 1, 1 }));
	CHECK(not Geometry2D::Overlaps(ellipse, Ellipse{ Vec2{ 0, 0 }, 0, 3 }));
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipse")
{
	const SuperEllipse superEllipse{ Vec2{ 0, 0 }, SizeF{ 5, 3 }, 4.0 };

	CHECK(Geometry2D::Overlaps(RectF{ -2, -2, 4, 4 }, superEllipse));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, -2, 4, 4 }, superEllipse));
	CHECK(Geometry2D::Intersects(RectF{ 5, -2, 4, 4 }, superEllipse));

	CHECK(Geometry2D::Overlaps(Circle{ Vec2{ 9, 0 }, 5 }, superEllipse));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 10, 0 }, 5 }, superEllipse));

	CHECK(Geometry2D::Overlaps(superEllipse,
		SuperEllipse{ Vec2{ 9, 0 }, SizeF{ 5, 3 }, 4.0 }));
	CHECK(not Geometry2D::Overlaps(superEllipse,
		SuperEllipse{ Vec2{ 10, 0 }, SizeF{ 5, 3 }, 4.0 }));
	CHECK(not Geometry2D::Overlaps(superEllipse,
		SuperEllipse{ Vec2{ 0, 0 }, SizeF{ 0, 3 }, 4.0 }));
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipse.triangles")
{
	for (const double n : { 0.5, 1.0, 4.0 })
	{
		const SuperEllipse shape{ Vec2{ 0, 0 }, SizeF{ 4, 3 }, n };
		const Triangle inside{ Vec2{ 0, 0 }, Vec2{ 0.25, 0 }, Vec2{ 0, 0.25 } };
		const Triangle crossing{ Vec2{ 3, -1 }, Vec2{ 5, -1 }, Vec2{ 3, 1 } };
		const Triangle contact{ Vec2{ -1, 4 }, Vec2{ 0, 3 }, Vec2{ 1, 4 } };
		const Triangle outside{ Vec2{ 3, 3 }, Vec2{ 4, 2.75 }, Vec2{ 4, 3 } };
		CAPTURE(n);
		auto Check = [&](const Triangle& triangle, const bool expected)
		{
			const Quad quad{ triangle.p0, triangle.p1, triangle.p2, triangle.p2 };
			CHECK(Geometry2D::Overlaps(shape, triangle) == expected);
			CHECK(Geometry2D::Overlaps(triangle, shape) == expected);
			CHECK(Geometry2D::Overlaps(shape, quad) == expected);
			CHECK(Geometry2D::Overlaps(quad, shape) == expected);
		};
		Check(inside, true);
		Check(crossing, true);
		Check(contact, false);
		Check(outside, false);
		CHECK(Geometry2D::Overlaps(shape, shape));
		CHECK(Geometry2D::Overlaps(shape, shape.movedBy(1, 0)));
		CHECK_FALSE(Geometry2D::Overlaps(shape, shape.movedBy(8, 0)));
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipsePolygon.rings")
{
	const Polygon donut{
		Array<Vec2>{ { -10, -10 }, { 10, -10 }, { 10, 10 }, { -10, 10 } },
		Array<Array<Vec2>>{ { { -4, -4 }, { -4, 4 }, { 4, 4 }, { 4, -4 } } }
	};
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
	{
		for (const double n : { 1.0, 4.0 })
		{
			const SuperEllipse shape{ offset, SizeF{ 2, 2 }, n };
			const Polygon square = RectF{ offset - Vec2{ 1, 1 }, 2, 2 }.asPolygon();
			CAPTURE(offset, n);
			CHECK(Geometry2D::Overlaps(shape, square));
			CHECK(Geometry2D::Overlaps(square, shape));
			CHECK(Geometry2D::Overlaps(shape, MultiPolygon{ Polygon{}, square }));
			CHECK_FALSE(Geometry2D::Overlaps(shape, donut.movedBy(offset)));
			CHECK_FALSE(Geometry2D::Overlaps(shape, square.movedBy(3, 0)));
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.RoundRect")
{
	const RoundRect roundRect{ RectF{ 0, 0, 10, 10 }, 2.0 };

	CHECK(Geometry2D::Overlaps(RectF{ 9, 2, 5, 6 }, roundRect));
	CHECK(not Geometry2D::Overlaps(RectF{ 10, 2, 5, 6 }, roundRect));
	CHECK(Geometry2D::Intersects(RectF{ 10, 2, 5, 6 }, roundRect));

	CHECK(Geometry2D::Overlaps(Circle{ Vec2{ 14, 5 }, 5 }, roundRect));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 15, 5 }, 5 }, roundRect));
	CHECK(Geometry2D::Intersects(Circle{ Vec2{ 15, 5 }, 5 }, roundRect));

	CHECK(Geometry2D::Overlaps(roundRect, RoundRect{ RectF{ 9, 0, 10, 10 }, 2.0 }));
	CHECK(not Geometry2D::Overlaps(roundRect, RoundRect{ RectF{ 10, 0, 10, 10 }, 2.0 }));

	CHECK(Geometry2D::Overlaps(Ellipse{ Vec2{ 14, 5 }, 5, 3 }, roundRect));
	CHECK(not Geometry2D::Overlaps(Ellipse{ Vec2{ 15, 5 }, 5, 3 }, roundRect));

	CHECK(not Geometry2D::Overlaps(RoundRect{ RectF{ 5, 0, 0, 10 }, 2.0 }, roundRect));
	CHECK(not Geometry2D::Overlaps(RoundRect{ RectF{ 0, 0, 0, 0 }, 2.0 }, roundRect));
}

TEST_CASE("Geometry2D.Overlaps.EllipsePolygon.rings")
{
	const Polygon square = RectF{ 0, 0, 8, 4 }.asPolygon();
	const Polygon triangle{ Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 0, 7.5 } } };
	const Polygon concave{ Array<Vec2>{ { 0, 0 }, { 12, 0 }, { 12, 12 }, { 8, 12 }, { 8, 4 }, { 4, 4 }, { 4, 12 }, { 0, 12 } } };
	const Polygon donut{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 4, 4 }, { 4, 16 }, { 16, 16 }, { 16, 4 } } }
	};
	const Polygon point = square.scaledFrom(Vec2{ 2, 2 }, 0.0);
	const Polygon segment = square.scaledFrom(Vec2{ 2, 2 }, Vec2{ 0, 1 }).rotated(0.5);
	REQUIRE(not point.isEmpty());
	REQUIRE(not segment.isEmpty());

	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			auto Check = [&](const Ellipse& ellipse, const Polygon& polygon, const bool expected)
			{
				CAPTURE(offset, scale, ellipse, expected);
				const Ellipse e{ ellipse.center * scale + offset, ellipse.axes };
				const Polygon p = polygon.scaledFromOrigin(scale).movedBy(offset);
				const MultiPolygon multi{ Polygon{}, p };
				auto CheckShape = [&](const auto& shape)
				{
					CHECK(Geometry2D::Overlaps(shape, p) == expected);
					CHECK(Geometry2D::Overlaps(p, shape) == expected);
					CHECK(Geometry2D::Overlaps(shape, multi) == expected);
					CHECK(Geometry2D::Overlaps(multi, shape) == expected);
				};
				CheckShape(e);
				CheckShape(SuperEllipse{ e, 2.0 });
			};
			Check(Ellipse{ 4, 2, 2, 1 }, square, true);
			Check(Ellipse{ 4, 2, 8, 4 }, square, true);
			Check(Ellipse{ 8, 2, 2, 1 }, square, true);
			Check(Ellipse{ 10, 2, 2, 1 }, square, false);
			Check(Ellipse{ -6, -4, 10, 5 }, square, false);
			Check(Ellipse{ -6, -4, 10.25, 5.125 }, square, true);
			Check(Ellipse{ 12, 8, 8, 4 }, triangle, false);
			Check(Ellipse{ 12, 8, 8.03125, 4.015625 }, triangle, true);
			Check(Ellipse{ 12, 8, 7.96875, 3.984375 }, triangle, false);
			Check(Ellipse{ 6, 8, 2, 1 }, concave, false);
			Check(Ellipse{ 6, 8, 2.125, 1 }, concave, true);
			Check(Ellipse{ 10, 10, 2, 1 }, donut, false);
			Check(Ellipse{ 10, 10, 6, 4 }, donut, false);
			Check(Ellipse{ 10, 10, 6.125, 4 }, donut, true);
			Check(Ellipse{ 4, 10, 2, 1 }, donut, true);
			Check(Ellipse{ 10, 10, 24, 16 }, donut, true);
			Check(Ellipse{ 4, 2, 0, 1 }, square, false);
			Check(Ellipse{ 4, 2, 2, 0 }, square, false);
			Check(Ellipse{ 4, 2, 8, 4 }, point, false);
			Check(Ellipse{ 0, 0, 8, 4 }, segment, false);
			Check(Ellipse{ 4, 2, 2, 1 }, Polygon{}, false);
		}
	}

	const Ellipse ellipse{ 0, 0, 10, 5 };
	const Polygon chord{ Array<Vec2>{ { 10, 0 }, { 6, 4 }, { 0, 5 } } };
	CHECK(Geometry2D::Overlaps(ellipse, chord));
	CHECK(Geometry2D::Intersects(Line{ Vec2{ 10, 0 }, Vec2{ 12, 0 } }, ellipse));
	CHECK(Geometry2D::Intersects(Line{ Vec2{ 10, 0 }, Vec2{ 10, 0 } }, ellipse));
}

TEST_CASE("Geometry2D.Overlaps.RoundRectPolygon.rings")
{
	const Polygon square = RectF{ 0, 0, 4, 4 }.asPolygon();
	const Polygon triangle{ Array<Vec2>{ { 0, 0 }, { 10, 0 }, { 0, 7.5 } } };
	const Polygon concave{ Array<Vec2>{ { 0, 0 }, { 12, 0 }, { 12, 12 }, { 8, 12 }, { 8, 4 }, { 4, 4 }, { 4, 12 }, { 0, 12 } } };
	const Polygon donut{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 4, 4 }, { 4, 16 }, { 16, 16 }, { 16, 4 } } }
	};
	const Polygon point = square.scaledFrom(Vec2{ 2, 2 }, 0.0);
	const Polygon segment = square.scaledFrom(Vec2{ 2, 2 }, Vec2{ 0, 1 }).rotated(0.5);
	REQUIRE(not point.isEmpty());
	REQUIRE(not segment.isEmpty());

	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			auto Check = [&](const RoundRect& roundRect, const Polygon& polygon, const bool expected)
			{
				CAPTURE(offset, scale, roundRect, expected);
				const RoundRect r{ RectF{ Arg::center = roundRect.rect.center() * scale + offset, roundRect.rect.size }, roundRect.r };
				const Polygon p = polygon.scaledFromOrigin(scale).movedBy(offset);
				const MultiPolygon multi{ Polygon{}, p };
				CHECK(Geometry2D::Overlaps(r, p) == expected);
				CHECK(Geometry2D::Overlaps(p, r) == expected);
				CHECK(Geometry2D::Overlaps(r, multi) == expected);
				CHECK(Geometry2D::Overlaps(multi, r) == expected);
			};
			for (const double radius : { 0.0, 0.25, 1.0, 8.0 })
			{
				CAPTURE(radius);
				Check(RoundRect{ RectF{ 1, 1, 2, 2 }, radius }, square, true);
				Check(RoundRect{ RectF{ -2, -2, 8, 8 }, radius }, square, true);
				Check(RoundRect{ RectF{ 3, 1, 2, 2 }, radius }, square, true);
				Check(RoundRect{ RectF{ 4, 1, 2, 2 }, radius }, square, false);
				Check(RoundRect{ RectF{ 4, 4, 2, 2 }, radius }, square, false);
				Check(RoundRect{ RectF{ 4, 6, 4, 2 }, radius }, concave, false);
				Check(RoundRect{ RectF{ 3.75, 6, 4.5, 2 }, radius }, concave, true);
				Check(RoundRect{ RectF{ 6, 6, 8, 8 }, radius }, donut, false);
				Check(RoundRect{ RectF{ 4, 4, 12, 12 }, radius }, donut, false);
				Check(RoundRect{ RectF{ 2, 8, 16, 2 }, radius }, donut, true);
				Check(RoundRect{ RectF{ -4, -4, 28, 28 }, radius }, donut, true);
				Check(RoundRect{ RectF{ 0, 0, 4, 4 }, radius }, point, false);
				Check(RoundRect{ RectF{ -4, -4, 8, 8 }, radius }, segment, false);
				Check(RoundRect{ RectF{ 0, 0, 4, 4 }, radius }, Polygon{}, false);
			}
			Check(RoundRect{ RectF{ 2, 4, 10, 10 }, 4 }, triangle, false);
			Check(RoundRect{ RectF{ 1.875, 4, 10, 10 }, 4 }, triangle, true);
			Check(RoundRect{ RectF{ 2.125, 4, 10, 10 }, 4 }, triangle, false);
			Check(RoundRect{ RectF{ 0, 0, 10, 10 }, 5 }, RectF{ 8, 9, 2, 2 }.asPolygon(), false);
			Check(RoundRect{ RectF{ 0, 0, 10, 10 }, 5 }, RectF{ 7.875, 8.875, 2, 2 }.asPolygon(), true);
			Check(RoundRect{ RectF{ 1, 1, 0, 2 }, 1 }, square, false);
			Check(RoundRect{ RectF{ 1, 1, 2, 0 }, 1 }, square, false);
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.EllipseEllipse.shallow_overlap")
{
	auto Check = [](const auto& a, const auto& b, const bool overlaps, const bool intersects)
	{
		CHECK(Geometry2D::Overlaps(a, b) == overlaps);
		CHECK(Geometry2D::Overlaps(b, a) == overlaps);
		CHECK(Geometry2D::Intersects(a, b) == intersects);
		CHECK(Geometry2D::Intersects(b, a) == intersects);
	};
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		const Ellipse a{ offset, 2000, 1000 };
		for (const double angle : { 0.04908738521234052, 0.37, 0.5, 1.0 })
		{
			for (const double separation : { 1.9999999, 2.0000001 })
			{
				CAPTURE(offset, angle, separation);
				const Ellipse b = a.movedBy(separation * a.axes.x * std::cos(angle), separation * a.axes.y * std::sin(angle));
				const bool expected = (separation < 2.0);
				Check(a, b, expected, expected);
				Check(a, SuperEllipse{ b, 2.0 }, expected, expected);
				Check(SuperEllipse{ a, 2.0 }, SuperEllipse{ b, 2.0 }, expected, expected);
			}
		}
		Check(Ellipse{ offset, 5, 10 }, Ellipse{ offset + Vec2{ 28, 224 }, 65, 234 }, false, true);
		Check(Ellipse{ offset, 5, 10 }, Ellipse{ offset + Vec2{ 27.99, 223.99 }, 65, 234 }, true, true);
		Check(Ellipse{ offset, 5, 10 }, Ellipse{ offset + Vec2{ 28.01, 224.01 }, 65, 234 }, false, false);
		Check(a, a.movedBy(4000, 0), false, true);
		Check(a, a, true, true);
		Check(a, Ellipse{ offset, 1, 2 }, true, true);
		Check(a, Ellipse{ offset, 0, 2 }, false, true);
		Check(a, Ellipse{ offset, 0, 0 }, false, false);
	}
	for (const double gap : { -1.0e-14, 0.0, 1.0e-14 })
	{
		Check(Ellipse{ 0, 0, 5, 3 }, Ellipse{ 10 + gap, 0, 5, 3 }, false, true);
	}
}

TEST_CASE("Geometry2D.Overlaps.EllipseRoundRect.shallow_overlap")
{
	auto Check = [](const auto& a, const RoundRect& b, const bool overlaps, const bool intersects)
	{
		CHECK(Geometry2D::Overlaps(a, b) == overlaps);
		CHECK(Geometry2D::Overlaps(b, a) == overlaps);
		CHECK(Geometry2D::Intersects(a, b) == intersects);
		CHECK(Geometry2D::Intersects(b, a) == intersects);
	};
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		const Ellipse ellipse{ offset, 2000, 1000 };
		for (const double angle : { 0.04908738521234052, 0.37, 0.5, 1.0 })
		{
			const Vec2 boundary{ ellipse.axes.x * std::cos(angle), ellipse.axes.y * std::sin(angle) };
			const Vec2 normal = Vec2{ std::cos(angle) / ellipse.axes.x, std::sin(angle) / ellipse.axes.y }.normalized();
			for (const double gap : { -0.0001, 0.0001 })
			{
				CAPTURE(offset, angle, gap);
				const Vec2 center = (offset + boundary + normal * (1000 + gap));
				const RoundRect roundRect{ RectF{ Arg::center = center, 2000, 2000 }, 1000 };
				Check(ellipse, roundRect, (gap < 0), (gap < 0));
				Check(SuperEllipse{ ellipse, 2.0 }, roundRect, (gap < 0), (gap < 0));
			}
		}
		const Ellipse a{ offset, 65, 117 };
		for (const SizeF size : { SizeF{ 10, 10 }, SizeF{ 10, 20 }, SizeF{ 20, 10 }, SizeF{ 20, 20 } })
		{
			CAPTURE(offset, size);
			Check(a, RoundRect{ RectF{ offset + Vec2{ 23, 107 }, size }, 5 }, false, true);
			Check(a, RoundRect{ RectF{ offset + Vec2{ 22.99, 106.99 }, size }, 5 }, true, true);
			Check(a, RoundRect{ RectF{ offset + Vec2{ 23.01, 107.01 }, size }, 5 }, false, false);
		}
		Check(a, RoundRect{ RectF{ offset + Vec2{ 65, -1 }, 5, 2 }, 0 }, false, true);
		Check(a, RoundRect{ RectF{ offset + Vec2{ 64.99, -1 }, 5, 2 }, 0 }, true, true);
		Check(a, RoundRect{ RectF{ offset + Vec2{ -1, -1 }, 2, 2 }, 100 }, true, true);
		Check(a, RoundRect{ RectF{ offset, 0, 2 }, 1 }, false, true);
		Check(Ellipse{ offset, 0, 2 }, RoundRect{ RectF{ offset + Vec2{ -1, -1 }, 2, 2 }, 1 }, false, true);
		Check(a, RoundRect{ RectF{ offset, 0, 0 }, 1 }, false, false);
	}
	for (const double gap : { -1.0e-14, 0.0, 1.0e-14 })
	{
		Check(Ellipse{ 0, 0, 5, 3 }, RoundRect{ RectF{ 5 + gap, -2, 4, 4 }, 2 }, false, true);
	}
}
