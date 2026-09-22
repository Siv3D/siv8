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

namespace
{
	void CheckSuperEllipsePolygonal(const SuperEllipse& shape, const Triangle& triangle,
		const bool overlaps, const bool intersects)
	{
		auto Check = [&](const auto& other)
		{
			CHECK(Geometry2D::Overlaps(shape, other) == overlaps);
			CHECK(Geometry2D::Overlaps(other, shape) == overlaps);
			CHECK(Geometry2D::Intersects(shape, other) == intersects);
			CHECK(Geometry2D::Intersects(other, shape) == intersects);
		};
		Check(triangle);
		Check(Quad{ triangle.p0, triangle.p1, triangle.p2, triangle.p2 });
		const Polygon polygon = triangle.asPolygon();
		REQUIRE(not polygon.isEmpty());
		Check(polygon);
		Check(MultiPolygon{ Polygon{}, polygon });
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipsePolygonal.shallow_and_concave")
{
	const double step = (Math::TwoPi / 64.0);
	auto Boundary = [](const double angle, const double n)
	{
		return Vec2{ 1000 * std::pow(std::cos(angle), (2.0 / n)), 700 * std::pow(std::sin(angle), (2.0 / n)) };
	};
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		for (const Vec2 reflection : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
		{
			auto At = [&](const Vec2& p) { return (p * reflection + offset); };
			for (const double n : { 1.5, 4.0, 8.0 })
			{
				CAPTURE(offset, reflection, n);
				const Vec2 p = (Boundary((step * 0.5), n) * (1.0 - 1.0e-6));
				const Triangle small{ At(p + Vec2{ -0.0001, -0.0001 }), At(p + Vec2{ 0.0001, -0.0001 }), At(p + Vec2{ 0, 0.0001 }) };
				CheckSuperEllipsePolygonal(SuperEllipse{ offset, 1000, 700, n }, small, true, true);
			}
			const Vec2 p = ((Boundary((step * 7), 0.5) + Boundary((step * 8), 0.5)) * 0.5);
			REQUIRE((std::sqrt((p.x - 0.01) / 1000) + std::sqrt((p.y - 0.01) / 700)) > 1.0);
			const Triangle outside{ At(p + Vec2{ -0.01, -0.01 }), At(p + Vec2{ 0.01, -0.01 }), At(p + Vec2{ 0, 0.01 }) };
			CheckSuperEllipsePolygonal(SuperEllipse{ offset, 1000, 700, 0.5 }, outside, false, false);
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipsePolygonal.contact")
{
	for (const double n : { 1.0, 1.5, 4.0, 8.0 })
	{
		const SuperEllipse shape{ 0, 0, 5, 3, n };
		const double v = std::pow(0.5, (1.0 / n));
		for (const double gap : { -1.0e-7, 0.0, 1.0e-7 })
		{
			CAPTURE(n, gap);
			const Triangle triangle{
				Vec2{ (v - 0.25 + gap), (v + 0.25 + gap) } * shape.axes,
				Vec2{ (v + 0.25 + gap), (v - 0.25 + gap) } * shape.axes,
				Vec2{ (v + 0.4 + gap), (v + 0.4 + gap) } * shape.axes
			};
			CheckSuperEllipsePolygonal(shape, triangle, (gap < 0), (gap <= 0));
			CHECK(Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, shape) == (gap <= 0));
			CHECK(Geometry2D::Intersects(Line{ triangle.p1, triangle.p0 }, shape) == (gap <= 0));
		}
	}
	CheckSuperEllipsePolygonal(SuperEllipse{ 0, 0, 4, 4, 0.5 }, Triangle{ Vec2{ 1, 1 }, Vec2{ 2, 1 }, Vec2{ 1, 2 } }, false, true);
	CheckSuperEllipsePolygonal(SuperEllipse{ 0, 0, 4, 4, 0.5 }, Triangle{ Vec2{ -0.1, 3 }, Vec2{ 0.1, 3 }, Vec2{ 0, 5 } }, true, true);
	for (const double n : { 0.5, 1.0, 4.0 })
	{
		for (const double gap : { -1.0e-14, 0.0, 1.0e-14 })
		{
			const SuperEllipse shape{ 0, 0, 5, 3, n };
			const Triangle triangle{ Vec2{ (5 + gap), -1 }, Vec2{ (5 + gap), 1 }, Vec2{ 6, 0 } };
			CheckSuperEllipsePolygonal(shape, triangle, false, true);
			CHECK(Geometry2D::Intersects(Line{ triangle.p0, triangle.p1 }, shape));
			CHECK(Geometry2D::Intersects(LineString{ triangle.p0, triangle.p1 }, shape));
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipsePolygonal.holes_and_degeneration")
{
	const Polygon diamond{ Array<Vec2>{ { -8, 0 }, { 0, -4 }, { 8, 0 }, { 0, 4 } } };
	const Polygon diamondHole{ RectF{ -20, -20, 40, 40 }.asPolygon().outer(), Array<Array<Vec2>>{ { { -8, 0 }, { 0, 4 }, { 8, 0 }, { 0, -4 } } } };
	const Polygon rectHole{ RectF{ -20, -20, 40, 40 }.asPolygon().outer(), Array<Array<Vec2>>{ { { -8, -4 }, { -8, 4 }, { 8, 4 }, { 8, -4 } } } };
	REQUIRE(not diamondHole.isEmpty());
	REQUIRE(not rectHole.isEmpty());
	const Polygon point = diamond.scaledFromOrigin(0.0);
	const Polygon segment = diamond.scaledFromOrigin(Vec2{ 0, 1 });
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
	{
		for (const Vec2 reflection : { Vec2{ 1, 1 }, Vec2{ -1, 1 } })
		{
			for (const double n : { 0.5, 1.0, 1.5, 4.0 })
			{
				const SuperEllipse shape{ offset, 8, 4, n };
				auto Check = [&](const Polygon& source, const bool overlaps, const bool intersects)
				{
					const Polygon polygon = source.scaledFromOrigin(reflection).movedBy(offset);
					CAPTURE(offset, reflection, n, overlaps, intersects);
					CHECK(Geometry2D::Overlaps(shape, polygon) == overlaps);
					CHECK(Geometry2D::Overlaps(polygon, shape) == overlaps);
					CHECK(Geometry2D::Intersects(shape, polygon) == intersects);
					CHECK(Geometry2D::Overlaps(shape, MultiPolygon{ Polygon{}, polygon }) == overlaps);
				};
				Check(diamond, true, true);
				Check(diamondHole, (1.0 < n), true);
				Check(rectHole, false, true);
				Check(rectHole.scaledFromOrigin(0.999), true, true);
				Check(rectHole.scaledFromOrigin(1.001), false, false);
				Check(point, false, true);
				Check(segment, false, true);
				Check(Polygon{}, false, false);
			}
		}
	}
}

namespace
{
	// A common supporting normal constructs external tangency independently
	// of the intersection algorithm. Scaling the center difference crosses it.
	Vec2 ConvexSuperEllipseSupport(const SuperEllipse& shape, const Vec2& normal)
	{
		if (shape.n == 1.0)
		{
			return ((shape.a * normal.x) > (shape.b * normal.y)) ? Vec2{ shape.a, 0 } : Vec2{ 0, shape.b };
		}
		const double q = (shape.n / (shape.n - 1.0));
		const Vec2 weighted = (shape.axes * normal);
		const Vec2 scaled = (weighted / Max(weighted.x, weighted.y));
		const double denominator = std::pow((std::pow(scaled.x, q) + std::pow(scaled.y, q)), (1.0 / shape.n));
		return (shape.axes * Vec2{ std::pow(scaled.x, (q - 1.0)), std::pow(scaled.y, (q - 1.0)) } / denominator);
	}

	template <class Other>
	void CheckSuperEllipseCurved(const SuperEllipse& shape, const Other& other, const bool overlaps, const bool intersects)
	{
		CHECK(Geometry2D::Overlaps(shape, other) == overlaps);
		CHECK(Geometry2D::Overlaps(other, shape) == overlaps);
		CHECK(Geometry2D::Intersects(shape, other) == intersects);
		CHECK(Geometry2D::Intersects(other, shape) == intersects);
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.convex_contact")
{
	for (const double n : { 1.0, 1.1, 1.5, 4.0, 8.0, 32.0 })
	{
		for (const double angle : { 0.0, 0.13, 0.7, Math::HalfPi })
		{
			const Vec2 normal{ std::cos(angle), std::sin(angle) };
			const SuperEllipse source{ 0, 0, 1000, 700, n };
			const Vec2 boundary = ConvexSuperEllipseSupport(source, normal);
			for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
			{
				for (const Vec2 reflection : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
				{
					for (const double gap : { -1.0e-7, 0.0, 1.0e-7 })
					{
						// Large translations can round a constructed tangent to either side.
						if ((gap == 0.0) && (offset != Vec2{ 0, 0 }))
						{
							continue;
						}
						CAPTURE(n, angle, offset, reflection, gap);
						const SuperEllipse shape = source.movedBy(offset);
						auto At = [&](const Vec2& p) { return (p * reflection + offset); };
						const Vec2 circleCenter = ((boundary + normal * 100) * (1.0 + gap));
						CheckSuperEllipseCurved(shape, Circle{ At(circleCenter), 100 }, (gap < 0), (gap <= 0));
						const Vec2 rectCenter = At(circleCenter + Vec2{ 60, 80 });
						CheckSuperEllipseCurved(shape, RoundRect{ RectF{ rectCenter - Vec2{ 160, 180 }, 320, 360 }, 100 }, (gap < 0), (gap <= 0));
						for (const double m : { 1.0, 1.5, 2.0, 8.0 })
						{
							const SuperEllipse other{ 0, 0, 500, 200, m };
							const Vec2 center = At((boundary + ConvexSuperEllipseSupport(other, normal)) * (1.0 + gap));
							CAPTURE(m);
							CheckSuperEllipseCurved(shape, SuperEllipse{ center, other.axes, m }, (gap < 0), (gap <= 0));
							if (m == 2.0)
							{
								CheckSuperEllipseCurved(shape, Ellipse{ center, other.axes }, (gap < 0), (gap <= 0));
							}
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.convex_boundaries")
{
	const SuperEllipse shape{ 0, 0, 5, 3, 4 };
	CheckSuperEllipseCurved(shape, Circle{ 0, 0, 1 }, true, true);
	CheckSuperEllipseCurved(shape, Ellipse{ 0, 0, 1, 2 }, true, true);
	CheckSuperEllipseCurved(shape, shape, true, true);
	CheckSuperEllipseCurved(shape, RoundRect{ -1, -1, 2, 2, 0.5 }, true, true);
	CheckSuperEllipseCurved(shape, Circle{ 0, 0, 0 }, false, false);
	CheckSuperEllipseCurved(shape, Ellipse{ 0, 0, 0, 0 }, false, false);
	CheckSuperEllipseCurved(shape, Ellipse{ 5, 0, 0, 2 }, false, true);
	CheckSuperEllipseCurved(shape, SuperEllipse{ 5, 0, 0, 2, 4 }, false, true);
	CheckSuperEllipseCurved(shape, RoundRect{ 5, -2, 0, 4, 1 }, false, true);
	CheckSuperEllipseCurved(shape, RoundRect{ 0, 0, 0, 0, 1 }, false, false);
	for (const double radius : { 0.0, 1.0, 100.0 })
	{
		CheckSuperEllipseCurved(shape, RoundRect{ 5, -2, 2, 4, radius }, false, true);
		CheckSuperEllipseCurved(shape, RoundRect{ 4.999, -2, 2, 4, radius }, true, true);
		CheckSuperEllipseCurved(shape, RoundRect{ 5.001, -2, 2, 4, radius }, false, false);
	}
	for (const double gap : { -1.0e-14, 0.0, 1.0e-14 })
	{
		CheckSuperEllipseCurved(shape, Circle{ (6 + gap), 0, 1 }, false, true);
		CheckSuperEllipseCurved(shape, Ellipse{ (6 + gap), 0, 1, 2 }, false, true);
		CheckSuperEllipseCurved(shape, SuperEllipse{ (6 + gap), 0, 1, 2, 1 }, false, true);
		CheckSuperEllipseCurved(shape, RoundRect{ (5 + gap), -2, 2, 4, 1 }, false, true);
	}
	const SuperEllipse narrow{ 0, 0, 0.0049819843550270694, 523.29607687115197, 1.1 };
	CheckSuperEllipseCurved(narrow, SuperEllipse{ -65.259591771101611, 532.86270011942599, 67.022753833940598, 16.963664611356855, 4 }, false, true);
	const SuperEllipse diamond{ 0, 0, 1000, 700, 1 };
	CheckSuperEllipseCurved(diamond, diamond.movedBy(1000, 700), false, true);
	const SuperEllipse concave{ 0, 0, 5, 3, 0.5 };
	CheckSuperEllipseCurved(concave, shape, true, true);
	CheckSuperEllipseCurved(concave, shape.movedBy(20, 20), false, false);
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.concave_pairs")
{
	for (const double n : { 0.1, 0.25, 0.5, 0.9, 1.0 })
	{
		const SuperEllipse shape{ 0, 0, 1000, 700, n };
		for (const double x : { 0.0, 0.5, 1.0 })
		{
			// For equal shapes, a horizontal tip touches the other's boundary
			// at both ends of their shared horizontal range.
			const Vec2 contact{ (1000 * (1.0 + x)), (700 * std::pow((1.0 - std::pow(x, n)), (1.0 / n))) };
			for (const double gap : { -1.0e-7, 0.0, 1.0e-7 })
			{
				CAPTURE(n, x, gap);
				CheckSuperEllipseCurved(shape, shape.movedBy(contact * (1.0 + gap)), (gap < 0), (gap <= 0));
			}
		}
		CheckSuperEllipseCurved(shape, shape, true, true);
		CheckSuperEllipseCurved(shape, SuperEllipse{ 0, 0, 1, 1, 0.5 }, true, true);
		CheckSuperEllipseCurved(shape, shape.movedBy(3000, 2000), false, false);
	}
	const double contactY = (700 * std::pow((1.0 - std::sqrt(0.6)), 2.0));
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		for (const Vec2 reflection : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
		{
			for (const bool swapAxes : { false, true })
			{
				auto Orient = [&](Vec2 p)
				{
					if (swapAxes)
					{
						std::swap(p.x, p.y);
					}
					return p;
				};
				for (const double gap : { -0.001, 0.0, 0.001 })
				{
					if ((gap == 0.0) && (offset != Vec2{ 0, 0 }))
					{
						continue;
					}
					CAPTURE(offset, reflection, swapAxes, gap);
					const SuperEllipse a{ offset, Orient(Vec2{ 1000, 700 }), 0.5 };
					const SuperEllipse b{ offset + Orient(Vec2{ 1200, (contactY + gap) } * reflection), Orient(Vec2{ 600, 200 }), 0.25 };
					CheckSuperEllipseCurved(a, b, (gap < 0), (gap <= 0));
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.concave_boundaries")
{
	const SuperEllipse shape{ 0, 0, 4, 4, 0.5 };
	for (const Vec2 center : { Vec2{ 5, 1 }, Vec2{ 8, 0 }, Vec2{ 0, 8 } })
	{
		for (const double gap : { -1.0e-15, 0.0, 1.0e-15 })
		{
			CheckSuperEllipseCurved(shape, shape.movedBy(center * (1.0 + gap)), false, true);
		}
	}
	for (const double gap : { -1.0e-7, 0.0, 1.0e-7 })
	{
		// The concave shape's right tip touches an edge of the diamond.
		CheckSuperEllipseCurved(shape, SuperEllipse{ 5, (1.5 + gap), 4, 2, 1 }, (gap < 0), (gap <= 0));
	}
	CheckSuperEllipseCurved(shape, SuperEllipse{ 0, 0, 0, 0, 0.5 }, false, false);
	CheckSuperEllipseCurved(shape, SuperEllipse{ 0, 0, 0, 2, 0.5 }, false, true);
	CheckSuperEllipseCurved(shape, SuperEllipse{ 4, 0, 0, 2, 0.5 }, false, true);
	CheckSuperEllipseCurved(shape, SuperEllipse{ 5, 0, 0, 2, 0.5 }, false, false);
	CheckSuperEllipseCurved(shape, SuperEllipse{ 0, 4, 2, 0, 1 }, false, true);
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.mixed_contact")
{
	for (const double n : { 0.25, 0.5, 0.75, 0.9 })
	{
		for (const double t : { 0.3, 0.5, 0.7 })
		{
			const double p = (1.0 / n);
			const Vec2 boundary{ (1000 * std::pow(t, p)), (700 * std::pow((1 - t), p)) };
			const Vec2 normal = Vec2{ (700 * std::pow((1 - t), (p - 1))), (1000 * std::pow(t, (p - 1))) }.normalized();
			for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
			{
				for (const Vec2 reflection : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
				{
					for (const bool swapAxes : { false, true })
					{
						auto Orient = [&](const Vec2& v) { return swapAxes ? Vec2{ v.y, v.x } : v; };
						auto At = [&](const Vec2& v) { return (offset + Orient(v * reflection)); };
						const SuperEllipse shape{ offset, Orient(Vec2{ 1000, 700 }), n };
						for (const double gap : { -1.0e-7, 0.0, 1.0e-7 })
						{
							if ((gap == 0.0) && (offset != Vec2{ 0, 0 }))
							{
								continue; // Translating a constructed tangent can round it to either side.
							}
							CAPTURE(n, t, offset, reflection, swapAxes, gap);
							const Vec2 circleCenter = ((boundary + normal * 10) * (1 + gap));
							CheckSuperEllipseCurved(shape, Circle{ At(circleCenter), 10 }, (gap < 0), (gap <= 0));
							const Vec2 rectSize = Orient(Vec2{ 32, 36 });
							CheckSuperEllipseCurved(shape, RoundRect{ RectF{ At(circleCenter + Vec2{ 6, 8 }) - rectSize * 0.5, rectSize }, 10 }, (gap < 0), (gap <= 0));
							for (const double m : { 1.1, 1.5, 2.0, 4.0, 8.0 })
							{
								CAPTURE(m);
								const SuperEllipse other{ 0, 0, 10, 6, m };
								const Vec2 center = At((boundary + ConvexSuperEllipseSupport(other, normal)) * (1 + gap));
								CheckSuperEllipseCurved(shape, SuperEllipse{ center, Orient(other.axes), m }, (gap < 0), (gap <= 0));
								if (m == 2.0)
								{
									CheckSuperEllipseCurved(shape, Ellipse{ center, Orient(other.axes) }, (gap < 0), (gap <= 0));
								}
							}
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.mixed_flat_and_tip_contacts")
{
	// The circle has fourth-order contact at (250,250): along the concave
	// arc, squared distance minus radius squared is proportional to (t-1/2)^4.
	const SuperEllipse shape{ 0, 0, 1000, 1000, 0.5 };
	const double radius = std::sqrt(500000.0);
	for (const double gap : { -1.0e-7, -1.0e-12, 0.0, 1.0e-12, 1.0e-7 })
	{
		CAPTURE(gap);
		const Vec2 center = (Vec2{ 750, 750 } * (1 + gap));
		CheckSuperEllipseCurved(shape, Circle{ center, radius }, (gap < 0), (gap <= 0));
		CheckSuperEllipseCurved(shape, Ellipse{ center, radius, radius }, (gap < 0), (gap <= 0));
		CheckSuperEllipseCurved(shape, RoundRect{ RectF{ center - Vec2{ radius, radius }, (2 * radius), (2 * radius) }, radius }, (gap < 0), (gap <= 0));
	}

	// Contact positions obtained independently by maximizing the summed
	// vertical profiles in quadruple precision; these lie close to curve tips.
	const std::array<std::pair<SuperEllipse, SuperEllipse>, 3> tips{{
		{ SuperEllipse{ 0, 0, 0.040807768690165516, 181.01178228739653, 0.5 },
			SuperEllipse{ 0.071752891612946373, 31.986250921483371, 0.058042508574445671, 0.021635671515527116, 1.1 } },
		{ SuperEllipse{ 0, 0, 0.045161263405331747, 0.0050239517066610279, 0.999 },
			SuperEllipse{ 0.040474007231519282, 0.090960877202365759, 0.027748843494125829, 0.090441118621550479, 1.1 } },
		{ SuperEllipse{ 0, 0, 9.1626836579127477, 1.681409066700049, 0.999 },
			SuperEllipse{ 8.2036072086432661, 101.35501125508688, 43.429572721642749, 101.17957820586034, 1.1 } },
	}};
	for (const auto& [a, b] : tips)
	{
		for (const double gap : { -1.0e-7, 0.0, 1.0e-7 })
		{
			CAPTURE(a, b, gap);
			CheckSuperEllipseCurved(a, SuperEllipse{ b.center * (1 + gap), b.axes, b.n }, (gap < 0), (gap <= 0));
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.SuperEllipseCurved.mixed_boundaries")
{
	const SuperEllipse shape{ 0, 0, 4, 4, 0.5 };
	CheckSuperEllipseCurved(shape, Circle{ 0, 0, 1 }, true, true);
	CheckSuperEllipseCurved(shape, Ellipse{ 0, 0, 1, 2 }, true, true);
	CheckSuperEllipseCurved(shape, RoundRect{ -1, -1, 2, 2, 0.5 }, true, true);
	CheckSuperEllipseCurved(shape, Circle{ 0, 0, 0 }, false, false);
	CheckSuperEllipseCurved(shape, Ellipse{ 0, 0, 0, 0 }, false, false);
	CheckSuperEllipseCurved(shape, Ellipse{ 4, 0, 0, 2 }, false, true);
	CheckSuperEllipseCurved(shape, SuperEllipse{ 4, 0, 0, 2, 4 }, false, true);
	CheckSuperEllipseCurved(shape, RoundRect{ 4, -2, 0, 4, 1 }, false, true);
	CheckSuperEllipseCurved(shape, RoundRect{ 0, 0, 0, 0, 1 }, false, false);
	for (const double gap : { -1.0e-4, 0.0, 1.0e-4 })
	{
		CAPTURE(gap);
		CheckSuperEllipseCurved(shape, Circle{ (5 + gap), 0, 1 }, (gap < 0), (gap <= 0));
		CheckSuperEllipseCurved(shape, Ellipse{ 0, (6 + gap), 1, 2 }, (gap < 0), (gap <= 0));
		for (const double radius : { 0.0, 1.0, 2.0, 10.0 })
		{
			CheckSuperEllipseCurved(shape, RoundRect{ (4 + gap), -2, 2, 4, radius }, (gap < 0), (gap <= 0));
		}
	}
	for (const double gap : { -1.0e-15, 0.0, 1.0e-15 })
	{
		CheckSuperEllipseCurved(shape, Circle{ (5 + gap), 0, 1 }, false, true);
		CheckSuperEllipseCurved(shape, Ellipse{ (5 + gap), 0, 1, 2 }, false, true);
		CheckSuperEllipseCurved(shape, SuperEllipse{ (5 + gap), 0, 1, 2, 4 }, false, true);
		CheckSuperEllipseCurved(shape, RoundRect{ (4 + gap), -2, 2, 4, 1 }, false, true);
	}
}
