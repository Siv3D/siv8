//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("Geometry2D.Contains.Circle_Ellipse.circular")
{
	struct TestCase
	{
		Vec2 offset;
		double radius;
		bool expected;
	};
	const TestCase cases[] = {
		{ { 0, 0 }, 10, true },        // Same region
		{ { 0, 0 }, 9.999, true },     // Small clearance
		{ { 0, 0 }, 11, false },
		{ { 3, 4 }, 5, true },        // Internal tangency
		{ { 3, 4 }, 4, true },
		{ { 3, 4 }, 5.0001, false },  // Just beyond internal tangency
		{ { 10, 0 }, 1, false },
		{ { 20, 0 }, 10, false },     // External tangency
		{ { 30, 0 }, 1, false },
		{ { 0, 0 }, 0, false },       // Empty target
	};

	for (const Vec2 center : { Vec2{ 0, 0 }, Vec2{ 32, -64 } })
	{
		const Circle circle{ center, 10 };
		const Ellipse ellipse{ center, 10, 10 };

		for (const auto& test : cases)
		{
			CAPTURE(center, test.offset, test.radius);
			const Circle innerCircle{ center + test.offset, test.radius };
			const Ellipse innerEllipse{ innerCircle.center, test.radius, test.radius };
			CHECK(Geometry2D::Contains(circle, innerEllipse) == test.expected);
			CHECK(Geometry2D::Contains(ellipse, innerCircle) == test.expected);
			CHECK(circle.contains(innerEllipse) == test.expected);
			CHECK(ellipse.contains(innerCircle) == test.expected);
		}
	}
}

TEST_CASE("Geometry2D.Contains.Circle_Ellipse.empty_and_non_circular")
{
	const Circle circle{ 0, 0, 10 };
	const Ellipse ellipse{ 0, 0, 10, 10 };
	const Circle emptyCircle{ 0, 0, -0.0 };
	const Ellipse emptyEllipse{ 0, 0, -0.0, 0.0 };
	CHECK_FALSE(emptyCircle.contains(ellipse));
	CHECK_FALSE(emptyEllipse.contains(circle));
	CHECK_FALSE(circle.contains(emptyEllipse));
	CHECK_FALSE(ellipse.contains(emptyCircle));
	CHECK_FALSE(emptyCircle.contains(emptyEllipse));
	CHECK_FALSE(emptyEllipse.contains(emptyCircle));

	CHECK(circle.contains(Ellipse{ 0, 0, 5, 3 }));
	CHECK_FALSE(circle.contains(Ellipse{ 0, 0, 5, 11 }));
	CHECK(Ellipse{ 0, 0, 12, 10 }.contains(Circle{ 0, 0, 5 }));
	CHECK_FALSE(Ellipse{ 0, 0, 12, 10 }.contains(Circle{ 0, 0, 11 }));
	CHECK(circle.contains(Ellipse{ 0, 0, 0, 10 }));
	CHECK(circle.contains(Ellipse{ 0, 0, 10, 0 }));
	CHECK_FALSE(Ellipse{ 0, 0, 0, 10 }.contains(Circle{ 0, 0, 1 }));
	CHECK_FALSE(Ellipse{ 0, 0, 10, 0 }.contains(Circle{ 0, 0, 1 }));
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.concave_in_convex_containers")
{
	for (const double n : { 0.125, 0.5, 0.9, 0.999 })
	{
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 32, -64 }, Vec2{ 134217728, -134217728 } })
		{
			for (const double gap : { -1.0e-5, 0.0, 1.0e-5 })
			{
				CAPTURE(n, offset, gap);
				const SuperEllipse shape{ offset, (4 + gap), 2, n };
				auto Check = [&](const auto& container)
				{
					CHECK(Geometry2D::Contains(container, shape) == (gap <= 0));
					CHECK(container.contains(shape) == (gap <= 0));
				};
				// The horizontal tips touch each container when gap == 0.
				Check(Circle{ offset, 4 });
				Check(Ellipse{ offset, 4, 3 });
				Check(SuperEllipse{ offset, 4, 3, 4 });
				Check(Triangle{ offset + Vec2{ 0, -4 }, offset + Vec2{ 8, 4 }, offset + Vec2{ -8, 4 } });
				Check(Quad{ offset + Vec2{ 0, -2 }, offset + Vec2{ 4, 0 }, offset + Vec2{ 0, 2 }, offset + Vec2{ -4, 0 } });
				Check(RoundRect{ RectF{ offset - Vec2{ 4, 3 }, 8, 6 }, 1 });
				Check(RectF{ offset - Vec2{ 4, 3 }, 8, 6 });
			}
		}
	}
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.support_scale")
{
	for (const double n : { 1.0, 1.000001, 1.001, 1.1, 2.0, 4.0 })
	{
		for (const double scale : { 0.01, 1.0, 1000.0 })
		{
			for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ 32, -64 }, Vec2{ 134217728, -134217728 } })
			{
				CAPTURE(n, scale, offset);
				const SuperEllipse shape{ offset, (4 * scale), (4 * scale), n };
				auto Check = [&](const auto& container, const bool expected)
				{
					CHECK(Geometry2D::Contains(container, shape) == expected);
					CHECK(container.contains(shape) == expected);
				};
				// For 1 <= n <= 4 the circumradius is at most 4*2^(1/4) < 5.
				Check(Circle{ offset, (5 * scale) }, true);
				if (n == 1.001)
				{
					// The bounding diamond protrudes, so the support calculation is needed.
					Check(Circle{ offset, (4.001 * scale) }, true);
				}
				Check(Circle{ offset, (3 * scale) }, false);
				Check(Ellipse{ offset, (5 * scale), (6 * scale) }, true);
				Check(Ellipse{ offset, (3 * scale), (6 * scale) }, false);
				Check(SuperEllipse{ offset, (5 * scale), (6 * scale), 4 }, true);
				Check(SuperEllipse{ offset, (3 * scale), (6 * scale), 4 }, false);
				Check(Triangle{ offset + Vec2{ 0, -10 } * scale, offset + Vec2{ 10, 5 } * scale, offset + Vec2{ -10, 5 } * scale }, true);
				Check(RoundRect{ RectF{ offset - Vec2{ 5, 5 } * scale, SizeF{ 10, 10 } * scale }, (2 * scale) }, true);
				Check(RoundRect{ RectF{ offset - Vec2{ 3, 3 } * scale, SizeF{ 6, 6 } * scale }, scale }, false);
				Check(Polygon{ RectF{ offset - Vec2{ 5, 5 } * scale, SizeF{ 10, 10 } * scale }.asPolygon() }, true);
				Check(Polygon{ RectF{ offset - Vec2{ 3, 3 } * scale, SizeF{ 6, 6 } * scale }.asPolygon() }, false);
			}
		}
	}
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.holes_and_empty_shapes")
{
	const SuperEllipse shape{ 0, 0, 4, 4, 0.5 };
	const Array<Vec2> outer{ { 0, -5 }, { 5, 0 }, { 0, 5 }, { -5, 0 } };
	const Polygon diamond{ outer };
	CHECK(diamond.contains(shape));
	// All four tips are inside, but the central hole excludes part of the shape.
	const Polygon centralHole{ outer, { { { -0.1, -0.1 }, { -0.1, 0.1 }, { 0.1, 0.1 }, { 0.1, -0.1 } } } };
	CHECK_FALSE(centralHole.contains(shape));
	// This hole lies in the bounding box, outside the convex hull of the shape.
	const Polygon outsideHole{ outer, { { { 2.9, 1.4 }, { 2.9, 1.6 }, { 3.1, 1.6 }, { 3.1, 1.4 } } } };
	CHECK(outsideHole.contains(shape));
	CHECK(MultiPolygon{ Polygon{}, diamond }.contains(shape));
	CHECK_FALSE(MultiPolygon{ centralHole }.contains(shape));
	CHECK(shape.contains(shape));
	CHECK(shape.contains(SuperEllipse{ 0, 0, 0.5, 0.5, 0.25 }));
	CHECK_FALSE(Circle{ 0, 0, 0 }.contains(shape));
	CHECK_FALSE(Ellipse{ 0, 0, 0, 4 }.contains(shape));
	CHECK_FALSE(RoundRect{ 0, 0, 0, 0, 1 }.contains(shape));
	CHECK_FALSE(Polygon{}.contains(shape));
	CHECK_FALSE(MultiPolygon{}.contains(shape));
	for (const double n : { 0.5, 1.0, 1.001, 4.0 })
	{
		CAPTURE(n);
		const Circle circle{ 0, 0, 4 };
		CHECK_FALSE(circle.contains(SuperEllipse{ 0, 0, 0, 0, n }));
		CHECK(circle.contains(SuperEllipse{ 0, 0, 4, 0, n }));
		CHECK(circle.contains(SuperEllipse{ 0, 0, 0, 4, n }));
		CHECK_FALSE(circle.contains(SuperEllipse{ 0, 0, 0, 5, n }));
	}
}

TEST_CASE("Geometry2D.Contains.MultiPolygon.single_member")
{
	const Polygon left = RectF{ 0, 0, 4, 4 }.asPolygon();
	const Polygon right = RectF{ 8, 0, 4, 4 }.asPolygon();
	const MultiPolygon container{ Polygon{}, left, right };
	const MultiPolygon target{ Polygon{}, RectF{ 1, 1, 2, 2 }.asPolygon(), RectF{ 9, 1, 2, 2 }.asPolygon() };
	auto Check = [](const auto& a, const auto& b, const bool expected)
	{
		CHECK(Geometry2D::Contains(a, b) == expected);
		CHECK(a.contains(b) == expected);
	};
	Check(container, left, true);
	Check(container, right, true);
	Check(container, target, false);
	Check(container, container, false);
	Check(MultiPolygon{ RectF{ 0, 0, 12, 4 }.asPolygon() }, target, true);
	Check(container, MultiPolygon{ Polygon{}, left }, true);
	Check(container, MultiPolygon{ Polygon{} }, false);
	Check(MultiPolygon{ Polygon{} }, left, false);

	// The segment is covered by the union of two point-touching members,
	// but neither member alone contains it.
	const MultiPolygon touching{ RectF{ 0, 0, 2, 2 }.asPolygon(), RectF{ 2, 2, 2, 2 }.asPolygon() };
	Check(touching, Vec2{ 2, 2 }, true);
	Check(touching, Line{ Vec2{ 1, 1 }, Vec2{ 3, 3 } }, false);
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.concave_triangles_and_quads")
{
	for (const Vec2 axes : { Vec2{ 1, 1 }, Vec2{ 8, 2 } })
	{
		for (const Vec2 center : { Vec2{ 0, 0 }, Vec2{ 32, -64 } })
		{
			const SuperEllipse shape{ center, axes.x, axes.y, 0.5 };
			for (const double signX : { -1.0, 1.0 })
			{
				for (const double signY : { -1.0, 1.0 })
				{
					CAPTURE(axes, center, signX, signY);
					auto PointAt = [&](const double x, const double y)
					{
						return (center + axes * Vec2{ signX * x, signY * y });
					};
					const Vec2 a = PointAt(0.4, 0);
					const Vec2 b = PointAt(0, 0.4);
					const Triangle triangle{ center, a, b };
					CHECK(shape.contains(triangle));
					CHECK(shape.contains(Triangle{ b, a, center }));
					CHECK(shape.contains(triangle.asPolygon()));
					CHECK_FALSE(shape.contains(PointAt(0.4, 0.4))); // Bounding-box corner outside
					const Quad quad{ a, b, PointAt(-0.4, 0), PointAt(0, -0.4) };
					CHECK(shape.contains(quad));
					CHECK(shape.contains(Quad{ quad.p3, quad.p2, quad.p1, quad.p0 }));
					CHECK(shape.contains(quad.asPolygon()));
					const Quad concave{ center, a, PointAt(0.05, 0.05), b };
					CHECK(shape.contains(concave));
					CHECK(shape.contains(Quad{ concave.p1, concave.p2, concave.p3, concave.p0 }));
					// Every vertex is inside, but the sloping edge crosses the exterior.
					CHECK_FALSE(shape.contains(Triangle{ center, PointAt(0.6, 0), PointAt(0, 0.6) }));
					CHECK_FALSE(shape.contains(Quad{ PointAt(0.6, 0), PointAt(0, 0.6), PointAt(-0.6, 0), PointAt(0, -0.6) }));
					// The same segment represented by a line, triangle, or quad.
					for (const double extent : { 0.4, 0.6 })
					{
						const Vec2 p = PointAt(extent, 0);
						const Vec2 q = PointAt(0, extent);
						CHECK(shape.contains(Line{ p, q }) == (extent < 0.5));
						CHECK(shape.contains(Triangle{ p, q, p.lerp(q, 0.5) }) == (extent < 0.5));
						CHECK(shape.contains(Quad{ p, q, q, p }) == (extent < 0.5));
					}
					CHECK(shape.contains(Triangle{ a, a, a }));
					CHECK(shape.contains(Quad{ a, a, a, a }));
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.concave_line_tangency")
{
	for (const double n : { 0.125, 0.5, 0.9, 0.999 })
	{
		for (const double weight : { 0.125, 0.25, 0.5, 0.875 })
		{
			// The line X/a + Y/b = 1 touches the unit superellipse at
			// (weight^(1/n), (1-weight)^(1/n)).
			const double a = std::pow(weight, ((1.0 - n) / n));
			const double b = std::pow((1.0 - weight), ((1.0 - n) / n));
			for (const Vec2 center : { Vec2{ 0, 0 }, Vec2{ 8, -16 } })
			{
				const SuperEllipse shape{ center, 2, 4, n };
				for (const double gap : { -1.0e-6, 0.0, 1.0e-6 })
				{
					CAPTURE(n, weight, center, gap);
					for (const double signX : { -1.0, 1.0 })
					{
						for (const double signY : { -1.0, 1.0 })
						{
							const Vec2 p = (center + Vec2{ signX * 2 * a * (1 + gap), 0 });
							const Vec2 q = (center + Vec2{ 0, signY * 4 * b * (1 + gap) });
							CHECK(shape.contains(Line{ p, q }) == (gap <= 0));
							CHECK(shape.contains(Line{ q, p }) == (gap <= 0));
							CHECK(shape.contains(Triangle{ center, p, q }) == (gap <= 0));
						}
					}
				}
			}
		}
	}
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.concave_line_axis_crossings")
{
	const SuperEllipse shape{ 0, 0, 1, 1, 0.5 };
	const Line contained[] = {
		{ { -1, 0 }, { 1, 0 } }, { { 0, -1 }, { 0, 1 } },
		{ { -0.5, 0.01 }, { 0.5, 0.01 } },
		{ { 0.01, -0.5 }, { 0.01, 0.5 } },
		{ { -0.2, -0.2 }, { 0.2, 0.2 } },
		{ { -0.04, 0.54 }, { 0.54, -0.04 } }, // Two axis crossings, interior contact
		{ { 0.1, 0.1 }, { 0.1, 0.1 } },
	};
	for (const Line& line : contained)
	{
		CAPTURE(line);
		CHECK(shape.contains(line));
		CHECK(shape.contains(Line{ line.end, line.start }));
	}
	CHECK_FALSE(shape.contains(Line{ { -0.01, 0.55 }, { 0.55, -0.01 } }));
	CHECK_FALSE(shape.contains(Line{ { 0, 0 }, { 0.4, 0.4 } }));
	CHECK_FALSE(shape.contains(Line{ { 0.4, 0.4 }, { 0.4, 0.4 } }));
	CHECK(shape.contains(LineString{ { -0.04, 0.54 }, { 0.54, -0.04 }, { 0, 0 } }));
	CHECK_FALSE(shape.contains(LineString{ { -0.01, 0.55 }, { 0.55, -0.01 } }));
	CHECK_FALSE(shape.contains(LineString{}));
	CHECK(shape.contains(RectF{ -0.1, -0.1, 0.2, 0.2 }));
	CHECK_FALSE(shape.contains(RectF{ -0.3, -0.3, 0.6, 0.6 }));
}

TEST_CASE("Geometry2D.Contains.SuperEllipse.polyline_empty_degenerate_and_convex")
{
	for (const double n : { 0.5, 1.0, 2.0, 4.0 })
	{
		CAPTURE(n);
		const Triangle triangle{ { 0, 0 }, { 0.25, 0 }, { 0, 0.25 } };
		const Quad quad{ { 0.25, 0 }, { 0, 0.25 }, { -0.25, 0 }, { 0, -0.25 } };
		const SuperEllipse empty{ 0, 0, 0, 0, n };
		CHECK_FALSE(empty.contains(Line{ { 0, 0 }, { 0, 0 } }));
		CHECK_FALSE(empty.contains(triangle));
		CHECK_FALSE(empty.contains(quad));
		for (const Vec2 tip : { Vec2{ 1, 0 }, Vec2{ 0, 1 } })
		{
			const SuperEllipse segment{ 0, 0, tip.x, tip.y, n };
			CHECK(segment.contains(Line{ -tip, tip }));
			CHECK(segment.contains(Triangle{ -tip, tip, Vec2{ 0, 0 } }));
			CHECK(segment.contains(Quad{ -tip, tip, tip, -tip }));
			CHECK_FALSE(segment.contains(triangle));
			CHECK_FALSE(segment.contains(quad));
		}
		const SuperEllipse shape{ 0, 0, 1, 1, n };
		CHECK(shape.contains(triangle));
		CHECK(shape.contains(quad));
		CHECK_FALSE(shape.contains(triangle.movedBy(3, 0)));
		CHECK_FALSE(shape.contains(quad.movedBy(3, 0)));
	}
}

namespace
{
	template <class Function>
	void ForEachCircleRepresentation(const Circle& circle, const Function& function)
	{
		function(circle);
		function(Ellipse{ circle.center, circle.r, circle.r });
		function(SuperEllipse{ circle.center, circle.r, circle.r, 2 });
		const RectF bounds{ (circle.center - Vec2{ circle.r, circle.r }), (2 * circle.r), (2 * circle.r) };
		function(RoundRect{ bounds, circle.r });
		function(RoundRect{ bounds, (2 * circle.r) }); // Clamped corner radius
	}
}

TEST_CASE("Geometry2D.Contains.circular_representations")
{
	struct TestCase
	{
		Vec2 offset;
		double radius;
		bool expected;
	};
	const TestCase cases[] = {
		{ { 0, 0 }, 100, true },
		{ { 0, 0 }, 99.999, true },
		{ { 0, 0 }, 100.001, false },
		{ { 30, 40 }, 50, true },
		{ { 30, 40 }, 49.999, true },
		{ { 30, 40 }, 50.001, false },
		{ { 100, 0 }, 1, false },
		{ { 200, 0 }, 100, false },
		{ { 300, 0 }, 1, false },
		{ { 0, 0 }, 0, false },
	};
	for (const double scale : { 0.125, 1.0, 8.0 })
	{
		for (const Vec2 center : { Vec2{ 0, 0 }, Vec2{ 32, -64 } })
		{
			ForEachCircleRepresentation(Circle{ center, (100 * scale) }, [&](const auto& container)
			{
				for (const auto& test : cases)
				{
					CAPTURE(scale, center, test.offset, test.radius, container);
					ForEachCircleRepresentation(Circle{ (center + test.offset * scale), (test.radius * scale) }, [&](const auto& target)
					{
						CAPTURE(target);
						CHECK(Geometry2D::Contains(container, target) == test.expected);
						CHECK(container.contains(target) == test.expected);
					});
				}
			});
		}
	}
}

TEST_CASE("Geometry2D.Contains.circular_representations.empty_and_segments")
{
	ForEachCircleRepresentation(Circle{ 0, 0, 0 }, [&](const auto& empty)
	{
		ForEachCircleRepresentation(Circle{ 0, 0, 0 }, [&](const auto& target)
		{
			CHECK_FALSE(empty.contains(target));
		});
		ForEachCircleRepresentation(Circle{ 0, 0, 10 }, [&](const auto& target)
		{
			CHECK_FALSE(empty.contains(target));
		});
	});
	ForEachCircleRepresentation(Circle{ 0, 0, 10 }, [&](const auto& container)
	{
		for (const Vec2 direction : { Vec2{ 1, 0 }, Vec2{ 0, 1 } })
		{
			for (const double extent : { 10.0, 10.001 })
			{
				const Vec2 tip = (direction * extent);
				const Ellipse ellipse{ Vec2{ 0, 0 }, tip };
				const SuperEllipse superEllipse{ ellipse, 2 };
				const RoundRect roundRect{ RectF{ -tip, (2 * tip.x), (2 * tip.y) }, 3 };
				CAPTURE(container, direction, extent);
				CHECK(container.contains(ellipse) == (extent == 10.0));
				CHECK(container.contains(superEllipse) == (extent == 10.0));
				CHECK(container.contains(roundRect) == (extent == 10.0));
				CHECK_FALSE(ellipse.contains(container));
				CHECK_FALSE(superEllipse.contains(container));
				CHECK_FALSE(roundRect.contains(container));
			}
		}
	});
}

TEST_CASE("Geometry2D.Contains.circular_representations.non_circular_shapes")
{
	const Circle circle{ 0, 0, 10 };
	ForEachCircleRepresentation(circle, [&](const auto& equivalent)
	{
		auto Check = [&](const auto& other)
		{
			CAPTURE(equivalent, other);
			CHECK(equivalent.contains(other) == circle.contains(other));
			CHECK(other.contains(equivalent) == other.contains(circle));
		};
		Check(Ellipse{ 0, 0, 20, 5 });
		Check(Ellipse{ 0, 0, 5, 20 });
		Check(SuperEllipse{ 0, 0, 10, 10, 0.5 });
		Check(SuperEllipse{ 0, 0, 10, 10, 4 });
		Check(RoundRect{ -20, -5, 40, 10, 5 });
		Check(RoundRect{ -10, -10, 20, 20, 0 });
		Check(RectF{ -12, -12, 24, 24 }.asPolygon());
		Check(Triangle{ { 0, -30 }, { 30, 30 }, { -30, 30 } });
		// These shapes have equal extents but are not circular.
		CHECK_FALSE(equivalent.contains(SuperEllipse{ 0, 0, 10, 10, 4 }));
		CHECK_FALSE(equivalent.contains(RoundRect{ -10, -10, 20, 20, 0 }));
		CHECK_FALSE(SuperEllipse{ 0, 0, 10, 10, 0.5 }.contains(equivalent));
	});

	// n == 2 also reduces non-circular superellipses to ellipses.
	const Ellipse ellipse{ 4, -8, 10, 5 };
	const SuperEllipse superEllipse{ ellipse, 2 };
	CHECK(ellipse.contains(superEllipse));
	CHECK(superEllipse.contains(ellipse));
	CHECK_FALSE(ellipse.contains(SuperEllipse{ 4, -8, 10.001, 5, 2 }));
	CHECK_FALSE(superEllipse.contains(Ellipse{ 4, -8, 10.001, 5 }));
}
