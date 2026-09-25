//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/Geometry2D/Overlaps.hpp>

TEST_CASE("Geometry2D.Overlaps.Rect")
{
	CHECK(Geometry2D::Overlaps(Rect{ 0, 0, 10, 10 }, Rect{ 5, 5, 10, 10 }));
	CHECK(not Geometry2D::Overlaps(Rect{ 0, 0, 10, 10 }, Rect{ 10, 0, 5, 5 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 0, 0, 10, 10 }, RectF{ 10, 10, 5, 5 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 0, 0, 0, 0 }, RectF{ 0, 0, 10, 10 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, 0, 0, 10 }, RectF{ 0, 0, 10, 10 }));
	CHECK(not Geometry2D::Overlaps(RectF{ 0, 5, 10, 0 }, RectF{ 0, 0, 10, 10 }));
}

TEST_CASE("Geometry2D.Overlaps.Circle")
{
	const Circle circle{ Vec2{ 0, 0 }, 5 };

	CHECK(Geometry2D::Overlaps(circle, Circle{ Vec2{ 9, 0 }, 5 }));
	CHECK(not Geometry2D::Overlaps(circle, Circle{ Vec2{ 10, 0 }, 5 }));
	CHECK(Geometry2D::Intersects(circle, Circle{ Vec2{ 10, 0 }, 5 }));
	CHECK(Geometry2D::Overlaps(circle, Circle{ Vec2{ 0, 0 }, 1 }));
	CHECK(not Geometry2D::Overlaps(circle, Circle{ Vec2{ 0, 0 }, 0 }));
	CHECK(not Geometry2D::Overlaps(Circle{ Vec2{ 0, 0 }, 0 }, circle));

	CHECK(Geometry2D::Overlaps(RectF{ -2, -2, 4, 4 }, circle));
	CHECK(not Geometry2D::Overlaps(RectF{ 5, -2, 4, 4 }, circle));
	CHECK(Geometry2D::Intersects(RectF{ 5, -2, 4, 4 }, circle));
}

TEST_CASE("Geometry2D.Overlaps.Triangle")
{
	const Triangle triangle{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 10 } };
	const Triangle inside{ Vec2{ 1, 1 }, Vec2{ 4, 1 }, Vec2{ 1, 4 } };
	const Triangle sharedEdge{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 5, -5 } };
	const Triangle pointTouch{ Vec2{ 10, 0 }, Vec2{ 12, 0 }, Vec2{ 10, 2 } };
	const Triangle segment{ Vec2{ 0, 0 }, Vec2{ 5, 0 }, Vec2{ 10, 0 } };
	const Triangle point{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };

	CHECK(Geometry2D::Overlaps(triangle, inside));
	CHECK(Geometry2D::Overlaps(triangle, triangle));
	CHECK(not Geometry2D::Overlaps(triangle, sharedEdge));
	CHECK(not Geometry2D::Overlaps(triangle, pointTouch));
	CHECK(not Geometry2D::Overlaps(triangle, segment));
	CHECK(not Geometry2D::Overlaps(triangle, point));
	CHECK(Geometry2D::Intersects(triangle, sharedEdge));
	CHECK(Geometry2D::Intersects(triangle, pointTouch));
}

TEST_CASE("Geometry2D.Overlaps.Triangle.boundary")
{
	const Triangle lower{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 0, 4 } };
	const Triangle upper{ Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } };
	const Triangle pointTouch{ Vec2{ 4, 0 }, Vec2{ 8, 0 }, Vec2{ 4, 4 } };
	const std::array<std::array<size_t, 3>, 6> orders{
		{{ 0, 1, 2 }, { 1, 2, 0 }, { 2, 0, 1 }, { 0, 2, 1 }, { 2, 1, 0 }, { 1, 0, 2 }}
	};
	auto Reorder = [](const Triangle& triangle, const std::array<size_t, 3>& order)
	{
		const std::array<Vec2, 3> points{ triangle.p0, triangle.p1, triangle.p2 };
		return Triangle{ points[order[0]], points[order[1]], points[order[2]] };
	};

	for (const double angle : { 0.0, 0.37, 0.5, 1.0 })
	{
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, 1.0e10 } })
		{
			const Triangle a = lower.rotatedAt(Vec2{ 0, 0 }, angle).movedBy(offset);
			const Triangle b = upper.rotatedAt(Vec2{ 0, 0 }, angle).movedBy(offset);
			const Triangle c = pointTouch.rotatedAt(Vec2{ 0, 0 }, angle).movedBy(offset);
			const Triangle segment{ a.p0, a.p1, a.p1 };
			CAPTURE(angle, offset);
			for (const auto& aOrder : orders)
			{
				for (const auto& bOrder : orders)
				{
					const Triangle reorderedA = Reorder(a, aOrder);
					CAPTURE(aOrder, bOrder);
					CHECK_FALSE(Geometry2D::Overlaps(reorderedA, Reorder(b, bOrder)));
					CHECK_FALSE(Geometry2D::Overlaps(Reorder(b, bOrder), reorderedA));
					CHECK_FALSE(Geometry2D::Overlaps(reorderedA, Reorder(c, bOrder)));
					CHECK_FALSE(Geometry2D::Overlaps(reorderedA, Reorder(segment, bOrder)));
					CHECK(Geometry2D::Overlaps(reorderedA, Reorder(a, bOrder)));
				}
			}
		}
	}

	const Triangle crossing{ Vec2{ 1, -1 }, Vec2{ 5, 3 }, Vec2{ 1, 3 } };
	CHECK(Geometry2D::Overlaps(lower, crossing));
	CHECK(Geometry2D::Overlaps(crossing, lower));
	CHECK(Geometry2D::Overlaps(lower, upper.movedBy(-1.0e-12, -1.0e-12)));
	CHECK_FALSE(Geometry2D::Overlaps(lower, upper.movedBy(1.0e-12, 1.0e-12)));
}

TEST_CASE("Geometry2D.Overlaps.Triangle.callers")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, 1.0e10 } })
	{
		const RectF rect{ offset, 4, 4 };
		const RoundRect roundRect{ rect, 0.0 };
		const Triangle contact = Triangle{ Vec2{ 2, 6 }, Vec2{ 6, 2 }, Vec2{ 6, 6 } }.movedBy(offset);
		CAPTURE(offset);
		for (const double displacement : { 0.0, -0.25, 0.25 })
		{
			const Triangle triangle = contact.movedBy(displacement, displacement);
			const Quad quad{ triangle.p0, triangle.p1, triangle.p2, triangle.p2 };
			const bool expected = (displacement < 0.0);
			CAPTURE(displacement);
			auto Check = [&](const auto& a, const auto& b)
			{
				CHECK(Geometry2D::Overlaps(a, b) == expected);
				CHECK(Geometry2D::Overlaps(b, a) == expected);
			};
			Check(rect, triangle);
			Check(rect, quad);
			Check(roundRect, triangle);
			Check(roundRect, quad);
			Check(Quad{ rect.tl(), rect.tr(), rect.br(), rect.bl() }, triangle);
			Check(Quad{ rect.tl(), rect.tr(), rect.br(), rect.bl() }, quad);
		}
	}

	const Quad left{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } };
	const Quad right{ Vec2{ 4, 0 }, Vec2{ 8, 0 }, Vec2{ 8, 4 }, Vec2{ 4, 4 } };
	for (const double angle : { 0.37, 0.5, 1.0 })
	{
		const Quad a = left.rotatedAt(Vec2{ 0, 0 }, angle);
		const Quad b = right.rotatedAt(Vec2{ 0, 0 }, angle);
		CHECK_FALSE(Geometry2D::Overlaps(a, b));
		CHECK_FALSE(Geometry2D::Overlaps(b, a));
		CHECK(Geometry2D::Overlaps(a, a));
	}
}

TEST_CASE("Geometry2D.Overlaps.Quad")
{
	const Quad quad{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
	const Quad overlapping{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } };
	const Quad sharedEdge{ Vec2{ 10, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 10 }, Vec2{ 10, 10 } };
	const Quad triangleCollapse{ Vec2{ 1, 1 }, Vec2{ 4, 1 }, Vec2{ 1, 4 }, Vec2{ 1, 4 } };
	const Quad segmentCollapse{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 0 }, Vec2{ 0, 0 } };
	const Quad pointCollapse{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } };

	CHECK(Geometry2D::Overlaps(quad, overlapping));
	CHECK(not Geometry2D::Overlaps(quad, sharedEdge));
	CHECK(Geometry2D::Overlaps(quad, triangleCollapse));
	CHECK(not Geometry2D::Overlaps(quad, segmentCollapse));
	CHECK(not Geometry2D::Overlaps(quad, pointCollapse));
}

TEST_CASE("Geometry2D.Overlaps.Quad.small_translated")
{
	constexpr double step = 0.000244140625;
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		const RectF bounds{ offset - Vec2{ 2, 2 }, 4, 4 };
		const Polygon polygon = bounds.asPolygon();
		REQUIRE(not polygon.isEmpty());
		for (const Vec2 reflection : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 }, Vec2{ -1, -1 } })
		{
			auto At = [&](const Vec2& point) { return (point * reflection + offset); };
			const Vec2 a = At({ 0, 0 });
			const Vec2 b = At({ step, 0 });
			const Vec2 c = At({ step, step });
			const Vec2 d = At({ 0, step });
			CAPTURE(offset, reflection);
			auto Check = [&](const Quad& quad, const bool expected)
			{
				CAPTURE(quad, expected);
				auto CheckPair = [&](const char* shapeType, const auto& other)
				{
					CAPTURE(shapeType);
					CHECK(Geometry2D::Overlaps(quad, other) == expected);
					CHECK(Geometry2D::Overlaps(other, quad) == expected);
				};
				CheckPair("RectF", bounds);
				CheckPair("Circle", Circle{ offset, 2 });
				CheckPair("Ellipse", Ellipse{ offset, 2, 3 });
				CheckPair("SuperEllipse", SuperEllipse{ offset, 2, 3, 4 });
				CheckPair("Triangle", Triangle{ bounds.tl(), bounds.tr(), offset + Vec2{ 0, 2 } });
				CheckPair("Quad", Quad{ bounds.tl(), bounds.tr(), bounds.br(), bounds.bl() });
				CheckPair("RoundRect", RoundRect{ bounds, 1 });
				CheckPair("Polygon", polygon);
				CheckPair("MultiPolygon", MultiPolygon{ polygon });
			};
			Check(Quad{ a, b, c, d }, true);
			Check(Quad{ a, b, c, c }, true);
			Check(Quad{ a, b, b, c }, true);
			Check(Quad{ a, a, b, c }, true);
			Check(Quad{ a, b, c, a }, true);
			Check(Quad{ a, b, b, a }, false);
			Check(Quad{ a, a, b, b }, false);
			Check(Quad{ a, a, a, a }, false);
			const Vec2 e = At({ 0.1, -0.3 });
			const Vec2 f = At({ 0.7, 0.2 });
			Check(Quad{ e, f, f, e }, false);
			Check(Quad{ e, e, f, f }, false);

			const Quad square{ a, b, c, d };
			CHECK(Geometry2D::Overlaps(square, square));
			CHECK_FALSE(Geometry2D::Overlaps(square, square.movedBy(step * reflection.x, 0)));
			CHECK_FALSE(Geometry2D::Overlaps(square, square.movedBy(2 * step * reflection.x, 0)));
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.Polygon_MultiPolygon")
{
	const Polygon polygon{ Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } } };
	const Polygon overlapping{ Array<Vec2>{ Vec2{ 5, 5 }, Vec2{ 15, 5 }, Vec2{ 15, 15 }, Vec2{ 5, 15 } } };
	const Polygon sharedEdge{ Array<Vec2>{ Vec2{ 10, 0 }, Vec2{ 20, 0 }, Vec2{ 20, 10 }, Vec2{ 10, 10 } } };
	const Polygon inside{ Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 4, 2 }, Vec2{ 4, 4 }, Vec2{ 2, 4 } } };
	const Polygon empty;
	const MultiPolygon multiPolygon{ polygon };
	const MultiPolygon overlappingMultiPolygon{ overlapping };
	const MultiPolygon sharedEdgeMultiPolygon{ sharedEdge };
	const MultiPolygon emptyMultiPolygon;

	CHECK(Geometry2D::Overlaps(polygon, overlapping));
	CHECK(not Geometry2D::Overlaps(polygon, sharedEdge));
	CHECK(Geometry2D::Overlaps(polygon, inside));
	CHECK(not Geometry2D::Overlaps(polygon, empty));
	CHECK(Geometry2D::Overlaps(multiPolygon, overlappingMultiPolygon));
	CHECK(not Geometry2D::Overlaps(multiPolygon, sharedEdgeMultiPolygon));
	CHECK(not Geometry2D::Overlaps(multiPolygon, emptyMultiPolygon));
	CHECK(not Geometry2D::Overlaps(emptyMultiPolygon, multiPolygon));
}

TEST_CASE("Geometry2D.Overlaps.Polygon.rings")
{
	const Polygon square = RectF{ 0, 0, 4, 4 }.asPolygon();
	const Polygon donut{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 4, 4 }, { 4, 16 }, { 16, 16 }, { 16, 4 } } }
	};
	const Polygon lower{ Array<Vec2>{ { 0, 0 }, { 4, 0 }, { 0, 4 } } };
	const Polygon upper{ Array<Vec2>{ { 4, 0 }, { 4, 4 }, { 0, 4 } } };
	const Polygon diamond{ Array<Vec2>{ { 2, 0 }, { 4, 2 }, { 2, 4 }, { 0, 2 } } };

	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, 134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			CAPTURE(offset, scale);
			auto Check = [&](const Polygon& a, const Polygon& b, const bool expected)
			{
				const Polygon transformedA = a.scaledFromOrigin(scale).movedBy(offset);
				const Polygon transformedB = b.scaledFromOrigin(scale).movedBy(offset);
				CHECK(Geometry2D::Overlaps(transformedA, transformedB) == expected);
				CHECK(Geometry2D::Overlaps(transformedB, transformedA) == expected);
				CHECK(Geometry2D::Overlaps(MultiPolygon{ Polygon{}, transformedA }, transformedB) == expected);
				CHECK(Geometry2D::Overlaps(MultiPolygon{ transformedA }, MultiPolygon{ Polygon{}, transformedB }) == expected);
			};
			Check(square, square, true);
			Check(square, RectF{ 1, 1, 1, 1 }.asPolygon(), true);
			Check(square, square.movedBy(3, 3), true);
			Check(square, square.movedBy(4, 0), false);
			Check(square, square.movedBy(4, 4), false);
			Check(square, RectF{ 2, 0, 4, 2 }.asPolygon(), true);
			Check(square, diamond, true); // Every diamond vertex lies on the square's boundary.
			Check(lower, upper, false);
			Check(donut, donut, true);
			Check(donut, RectF{ 6, 6, 8, 8 }.asPolygon(), false);
			Check(donut, RectF{ 4, 4, 12, 12 }.asPolygon(), false);
			Check(donut, RectF{ 2, 8, 16, 2 }.asPolygon(), true);
			Check(donut, RectF{ 0, 0, 20, 20 }.asPolygon(), true);
			Check(square, Polygon{}, false);
		}
	}

	const Polygon point = square.scaledFrom(Vec2{ 2, 2 }, 0.0);
	const Polygon segment = square.scaledFrom(Vec2{ 2, 2 }, Vec2{ 0, 1 }).rotated(0.5);
	REQUIRE(not point.isEmpty());
	REQUIRE(not segment.isEmpty());
	CHECK_FALSE(Geometry2D::Overlaps(square, point));
	CHECK_FALSE(Geometry2D::Overlaps(point, square));
	CHECK_FALSE(Geometry2D::Overlaps(square, segment));
	CHECK_FALSE(Geometry2D::Overlaps(segment, square));

	for (const double angle : { 0.37, 0.5, 1.0 })
	{
		const Polygon a = lower.rotated(angle);
		const Polygon b = upper.rotated(angle);
		CHECK(Geometry2D::Overlaps(a, a));
		CHECK_FALSE(Geometry2D::Overlaps(a, b));
		CHECK_FALSE(Geometry2D::Overlaps(b, a));
		CHECK_FALSE(Geometry2D::Overlaps(donut.rotated(angle), RectF{ 4, 4, 12, 12 }.asPolygon().rotated(angle)));
	}
}

TEST_CASE("Geometry2D.Overlaps.Polygon.affine_rectangles")
{
	const std::array<std::array<Vec2, 2>, 4> bases = {{
		{ Vec2{ 1, 0 }, Vec2{ 0, 1 } },
		{ Vec2{ 1, 2 }, Vec2{ 1, 3 } },
		{ Vec2{ 0, -1 }, Vec2{ 1, 0 } },
		{ Vec2{ -1, 0 }, Vec2{ 0, 1 } },
	}};
	for (const auto& basis : bases)
	{
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
		{
			auto Make = [&](const double x, const double y, const double size)
			{
				auto P = [&](const double px, const double py)
				{
					return (offset + basis[0] * px + basis[1] * py);
				};
				return Polygon{ Array<Vec2>{ P(x, y), P(x + size, y), P(x + size, y + size), P(x, y + size) }, SkipValidation::Yes };
			};
			const Polygon a = Make(0, 0, 4);
			REQUIRE(not a.isEmpty());
			for (int32 x = -4; x <= 4; ++x)
			{
				for (int32 y = -4; y <= 4; ++y)
				{
					for (const double size : { 1.0, 4.0 })
					{
						CAPTURE(basis, offset, x, y, size);
						const Polygon b = Make(x, y, size);
						const bool expected = ((x < 4) && (y < 4) && (0 < x + size) && (0 < y + size));
						CHECK(Geometry2D::Overlaps(a, b) == expected);
						CHECK(Geometry2D::Overlaps(b, a) == expected);
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.Polygon.supplied_mesh")
{
	const Polygon original = RectF{ 134217728, 134217728, 4, 4 }.asPolygon();
	Array<Float2> vertices = original.vertices();
	std::reverse(vertices.begin(), vertices.end());
	Array<TriangleIndex> indices = original.indices();
	const auto last = static_cast<TriangleIndex::value_type>(vertices.size() - 1);
	for (auto& index : indices)
	{
		index = { static_cast<TriangleIndex::value_type>(last - index.i0),
			static_cast<TriangleIndex::value_type>(last - index.i1),
			static_cast<TriangleIndex::value_type>(last - index.i2) };
	}
	const Polygon polygon{ original.outer(), original.inners(), vertices, indices, original.boundingRect() };
	CHECK(Geometry2D::Overlaps(polygon, original));
	CHECK(Geometry2D::Overlaps(original, polygon));
	CHECK_FALSE(Geometry2D::Overlaps(polygon, original.movedBy(4, 0)));
}

TEST_CASE("Geometry2D.Overlaps.RectPolygon.rings")
{
	const Polygon square = RectF{ 0, 0, 4, 4 }.asPolygon();
	const Polygon diamond{ Array<Vec2>{ { 2, 0 }, { 4, 2 }, { 2, 4 }, { 0, 2 } } };
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
		CAPTURE(offset);
		auto Check = [&](const RectF& rect, const Polygon& polygon, const bool expected)
		{
			const RectF movedRect = rect.movedBy(offset);
			const Polygon movedPolygon = polygon.movedBy(offset);
			const MultiPolygon multi{ Polygon{}, movedPolygon };
			auto CheckRect = [&](const auto& r)
			{
				CHECK(Geometry2D::Overlaps(r, movedPolygon) == expected);
				CHECK(Geometry2D::Overlaps(movedPolygon, r) == expected);
				CHECK(Geometry2D::Overlaps(r, multi) == expected);
				CHECK(Geometry2D::Overlaps(multi, r) == expected);
			};
			CheckRect(movedRect);
			if (offset.x < 1.0e9)
			{
				CheckRect(Rect{ static_cast<int32>(movedRect.x), static_cast<int32>(movedRect.y),
					static_cast<int32>(movedRect.w), static_cast<int32>(movedRect.h) });
			}
		};

		Check(RectF{ 0, 0, 4, 4 }, square, true);
		Check(RectF{ 1, 1, 1, 1 }, square, true);
		Check(RectF{ -1, -1, 6, 6 }, square, true);
		Check(RectF{ 3, 3, 2, 2 }, square, true);
		Check(RectF{ 4, 0, 2, 2 }, square, false);
		Check(RectF{ 4, 4, 2, 2 }, square, false);
		Check(RectF{ 0, 0, 1, 1 }, diamond, false); // Touches a slanted edge at one vertex.
		Check(RectF{ 0, 0, 4, 4 }, diamond, true);
		Check(RectF{ 2, 6, 8, 2 }, concave, true);
		Check(RectF{ 5, 6, 2, 2 }, concave, false);
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			const Polygon reflected = donut.scaledFrom(Vec2{ 10, 10 }, scale);
			Check(RectF{ 6, 6, 8, 8 }, reflected, false);
			Check(RectF{ 4, 4, 12, 12 }, reflected, false);
			Check(RectF{ 2, 8, 16, 2 }, reflected, true);
			Check(RectF{ 0, 0, 20, 20 }, reflected, true);
		}
		Check(RectF{ 0, 0, 4, 4 }, point, false);
		Check(RectF{ -4, -4, 8, 8 }, segment, false);
		Check(RectF{ 0, 0, 4, 4 }, Polygon{}, false);
		Check(RectF{ 1, 1, 0, 2 }, square, false);
		Check(RectF{ 1, 1, 2, 0 }, square, false);
	}
}

TEST_CASE("Geometry2D.Overlaps.RectPolygon.fractional_rectangles")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
	{
		const Polygon polygon = RectF{ offset, 1, 1 }.asPolygon();
		for (int32 x = -4; x <= 4; ++x)
		{
			for (int32 y = -4; y <= 4; ++y)
			{
				const RectF rect{ offset + Vec2{ x * 0.25, y * 0.25 }, 0.25, 0.25 };
				const bool expected = ((-1 < x) && (x < 4) && (-1 < y) && (y < 4));
				CAPTURE(offset, rect);
				CHECK(Geometry2D::Overlaps(rect, polygon) == expected);
				CHECK(Geometry2D::Overlaps(polygon, rect) == expected);
			}
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.TrianglePolygon.rings")
{
	const Triangle lower{ Vec2{ 0, 0 }, Vec2{ 4, 0 }, Vec2{ 0, 4 } };
	const Triangle upper{ Vec2{ 4, 0 }, Vec2{ 4, 4 }, Vec2{ 0, 4 } };
	const Polygon square = RectF{ 0, 0, 4, 4 }.asPolygon();
	const Polygon concave{ Array<Vec2>{ { 0, 0 }, { 12, 0 }, { 12, 12 }, { 8, 12 }, { 8, 4 }, { 4, 4 }, { 4, 12 }, { 0, 12 } } };
	const Polygon donut{
		Array<Vec2>{ { 0, 0 }, { 20, 0 }, { 20, 20 }, { 0, 20 } },
		Array<Array<Vec2>>{ { { 4, 4 }, { 4, 16 }, { 16, 16 }, { 16, 4 } } }
	};

	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 }, Vec2{ 1.0e10, -1.0e10 } })
	{
		for (const Vec2 scale : { Vec2{ 1, 1 }, Vec2{ -1, 1 }, Vec2{ 1, -1 } })
		{
			auto Check = [&](const Triangle& triangle, const Polygon& polygon, const bool expected)
			{
				CAPTURE(offset, scale, triangle, expected);
				const Triangle t = triangle.scaledFromOrigin(scale).movedBy(offset);
				const Polygon p = polygon.scaledFromOrigin(scale).movedBy(offset);
				const MultiPolygon multi{ Polygon{}, p };
				for (const Triangle candidate : { t, Triangle{ t.p0, t.p2, t.p1 } })
				{
					CHECK(Geometry2D::Overlaps(candidate, p) == expected);
					CHECK(Geometry2D::Overlaps(p, candidate) == expected);
					CHECK(Geometry2D::Overlaps(candidate, multi) == expected);
					CHECK(Geometry2D::Overlaps(multi, candidate) == expected);
				}
			};
			Check(lower, lower.asPolygon(), true);
			Check(lower, upper.asPolygon(), false);
			Check(lower, RectF{ 1, 1, 1, 1 }.asPolygon(), true);
			Check(lower, RectF{ -1, -1, 6, 6 }.asPolygon(), true);
			Check(lower, RectF{ 3, 0, 2, 2 }.asPolygon(), true);
			Check(lower, RectF{ 2, 2, 2, 2 }.asPolygon(), false);
			Check(lower, RectF{ 3, 3, 1, 1 }.asPolygon(), false);
			Check(Triangle{ Vec2{ 2, 0 }, Vec2{ 2, 2 }, Vec2{ 0, 2 } }, lower.asPolygon(), true);
			Check(Triangle{ Vec2{ 5, 6 }, Vec2{ 7, 6 }, Vec2{ 5, 8 } }, concave, false);
			Check(Triangle{ Vec2{ 2, 6 }, Vec2{ 10, 6 }, Vec2{ 6, 8 } }, concave, true);
			Check(Triangle{ Vec2{ 6, 6 }, Vec2{ 10, 6 }, Vec2{ 6, 10 } }, donut, false);
			Check(Triangle{ Vec2{ 4, 4 }, Vec2{ 16, 4 }, Vec2{ 4, 16 } }, donut, false);
			Check(Triangle{ Vec2{ 2, 8 }, Vec2{ 18, 8 }, Vec2{ 10, 10 } }, donut, true);
			Check(Triangle{ Vec2{ 0, 0 }, Vec2{ 40, 0 }, Vec2{ 0, 40 } }, donut, true);
			Check(Triangle{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } }, square, false);
			Check(Triangle{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 1, 1 } }, square, false);
			Check(lower, square.scaledFrom(Vec2{ 1, 1 }, 0.0), false);
			Check(lower, square.scaledFrom(Vec2{ 1, 1 }, Vec2{ 0, 1 }).rotated(0.5), false);
			Check(lower, Polygon{}, false);
		}
	}

	for (const double angle : { 0.37, 0.5, 1.0 })
	{
		const Triangle a = lower.rotatedAt(Vec2{ 0, 0 }, angle);
		const Triangle b = upper.rotatedAt(Vec2{ 0, 0 }, angle);
		CHECK(Geometry2D::Overlaps(a, a.asPolygon()));
		CHECK_FALSE(Geometry2D::Overlaps(a, b.asPolygon()));
		CHECK_FALSE(Geometry2D::Overlaps(b.asPolygon(), a));
	}
}

TEST_CASE("Geometry2D.Overlaps.QuadPolygon.rings")
{
	for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 134217728, -134217728 } })
	{
		const Quad quad{ offset, offset + Vec2{ 4, 0 }, offset + Vec2{ 4, 4 }, offset + Vec2{ 0, 4 } };
		const Polygon polygon = RectF{ offset, 4, 4 }.asPolygon();
		CHECK(Geometry2D::Overlaps(quad, polygon));
		CHECK(Geometry2D::Overlaps(polygon, quad));
		CHECK(Geometry2D::Overlaps(quad, MultiPolygon{ Polygon{}, polygon }));
		CHECK_FALSE(Geometry2D::Overlaps(quad, polygon.movedBy(4, 0)));
		const Quad triangle{ quad.p0, quad.p1, quad.p2, quad.p2 };
		CHECK(Geometry2D::Overlaps(triangle, polygon));
	}
}

TEST_CASE("Geometry2D.Overlaps.CirclePolygon.rings")
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
			auto Check = [&](const Circle& circle, const Polygon& polygon, const bool expected)
			{
				CAPTURE(offset, scale, circle, expected);
				const Circle c{ circle.center * scale + offset, circle.r };
				const Polygon p = polygon.scaledFromOrigin(scale).movedBy(offset);
				const MultiPolygon multi{ Polygon{}, p };
				CHECK(Geometry2D::Overlaps(c, p) == expected);
				CHECK(Geometry2D::Overlaps(p, c) == expected);
				CHECK(Geometry2D::Overlaps(c, multi) == expected);
				CHECK(Geometry2D::Overlaps(multi, c) == expected);
			};
			Check(Circle{ 2, 2, 1 }, square, true);
			Check(Circle{ 2, 2, 4 }, square, true);
			Check(Circle{ 4, 2, 1 }, square, true);
			Check(Circle{ 5, 2, 1 }, square, false);
			Check(Circle{ -3, -4, 5 }, square, false);
			Check(Circle{ -3, -4, 5.125 }, square, true);
			Check(Circle{ -3, -4, 4.875 }, square, false);
			Check(Circle{ 6, 8, 4 }, triangle, false); // Tangent to the slanted edge.
			Check(Circle{ 6, 8, 4.015625 }, triangle, true);
			Check(Circle{ 6, 8, 3.984375 }, triangle, false);
			Check(Circle{ 6, 8, 1 }, concave, false);
			Check(Circle{ 6, 8, 2 }, concave, false);
			Check(Circle{ 6, 8, 2.125 }, concave, true);
			Check(Circle{ 10, 10, 2 }, donut, false);
			Check(Circle{ 10, 10, 6 }, donut, false);
			Check(Circle{ 10, 10, 6.125 }, donut, true);
			Check(Circle{ 4, 10, 1 }, donut, true);
			Check(Circle{ 10, 10, 20 }, donut, true);
			Check(Circle{ 2, 2, 0 }, square, false);
			Check(Circle{ 2, 2, 4 }, point, false);
			Check(Circle{ 0, 0, 8 }, segment, false);
			Check(Circle{ 2, 2, 1 }, Polygon{}, false);
		}
	}
}

TEST_CASE("Geometry2D.Overlaps.CirclePolygon.contact")
{
	const Circle circle{ 0, 0, 5 };
	const Polygon triangle{ Array<Vec2>{ { 5, 0 }, { 3, 4 }, { 0, 5 } } };
	CHECK(Geometry2D::Overlaps(circle, triangle)); // A chord crosses the disk with both endpoints on the circle.
	CHECK(Geometry2D::Intersects(Line{ Vec2{ 5, 0 }, Vec2{ 6, 0 } }, circle));
	CHECK(Geometry2D::Intersects(Line{ Vec2{ 5, 0 }, Vec2{ 5, 0 } }, circle));
	CHECK(Geometry2D::Intersects(Line{ Vec2{ 10, 0 }, Vec2{ 0, 7.5 } }, Circle{ 6, 8, 4 }));
}
