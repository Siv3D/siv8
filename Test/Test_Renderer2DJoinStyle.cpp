//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Polygon/ClosedLineString.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <array>
# include <random>

namespace
{
	template <class Draw>
	Image CaptureJoin(Draw&& draw)
	{
		const auto background = Scene::GetBackground();
		const ScopeExit restore{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedColorMul2D mul{ Palette::White };
		const ScopedColorAdd2D add{ 0.0 };
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		return ScreenCapture::GetFrame();
	}

	template <class Fill>
	void DrawFrame(const Array<Vec2>& points, const double width, const Fill& fill, const JoinStyle style)
	{
		if (points.size() == 3) Triangle{ points[0], points[1], points[2] }.drawFrame(width, fill, style);
		else Quad{ points[0], points[1], points[2], points[3] }.drawFrame(width, fill, style);
	}

	Array<Vec2> Closed(const Array<Vec2>& points)
	{
		auto result = points;
		result << points[0] << points[1];
		return result;
	}

	void CheckBoundaryPixels(const Image& actual, const Image& expected)
	{
		// Different triangulations can round an edge across an MSAA sample.
		// Differences above 1/255 must be
		// confined to a one-pixel boundary neighborhood in both images.
		for (int y = 1; y + 1 < actual.height(); ++y)
			for (int x = 1; x + 1 < actual.width(); ++x)
			{
				if (Abs(static_cast<int>(actual[y][x].r) - expected[y][x].r) <= 1) continue;
				int amin = 255, amax = 0, emin = 255, emax = 0;
				for (int dy = -1; dy <= 1; ++dy)
					for (int dx = -1; dx <= 1; ++dx)
					{
						amin = Min(amin, static_cast<int>(actual[y + dy][x + dx].r));
						amax = Max(amax, static_cast<int>(actual[y + dy][x + dx].r));
						emin = Min(emin, static_cast<int>(expected[y + dy][x + dx].r));
						emax = Max(emax, static_cast<int>(expected[y + dy][x + dx].r));
					}
				CAPTURE(x, y, static_cast<int>(actual[y][x].r), static_cast<int>(expected[y][x].r), amin, amax, emin, emax);
				CHECK(amin < amax);
				CHECK(emin < emax);
				CHECK((emin <= actual[y][x].r && actual[y][x].r <= emax));
				CHECK((amin <= expected[y][x].r && expected[y][x].r <= amax));
			}
	}
}

TEST_CASE("Renderer2D.join_style_reference_pixels")
{
	const Array<Array<Vec2>> shapes{
		{ {80,60}, {240,80}, {130,210} },
		{ {80,60}, {240,80}, {220,210}, {100,190} },
		{ {80,60}, {240,80}, {150,110}, {100,210} }, // concave
		{ {80,60}, {240,210}, {80,210}, {240,60} }, // crossing
		{ {80,100}, {240,100}, {240,100}, {80,200} }, // zero edge
		{ {80,100}, {240,101}, {240,99} }, // nearly reversing
	};
	for (const auto& shape : shapes)
		for (const auto style : { JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter })
			for (const double width : { 12.0, 160.0 })
			{
				CAPTURE(shape.size(), static_cast<int>(style), width);
				const auto expected = CaptureJoin([&] { DrawClosedLineStringFallback(Closed(shape), style, width, ColorF{1, 0.5}); });
				const auto actual = CaptureJoin([&] { DrawFrame(shape, width, ColorF{1, 0.5}, style); });
				CheckBoundaryPixels(actual, expected);
			}
}

TEST_CASE("Renderer2D.join_style_transparency_and_winding")
{
	const Array<Vec2> shape{ {80,60}, {240,60}, {240,220}, {80,220} };
	for (const auto style : { JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter })
	{
		CAPTURE(static_cast<int>(style));
		const auto image = CaptureJoin([&] { DrawFrame(shape, 20, ColorF{1, 0.5}, style); });
		const auto reversed = CaptureJoin([&] { DrawFrame(shape.reversed(), 20, ColorF{1, 0.5}, style); });
		CHECK(image == reversed);
		CHECK(image[140][160] == Palette::Black);
		const auto solid = image[60][160];
		CHECK(solid.r > 0);
		for (int y = 40; y < 240; ++y)
			for (int x = 60; x < 260; ++x)
				CHECK(image[y][x].r <= solid.r + 1); // overlap would raise opacity from .5 to .75
	}
}

TEST_CASE("Renderer2D.join_style_inner_outer_collapsed_regression")
{
	const Quad square{80, 60, 240, 60, 240, 220, 80, 220};
	const auto expected = CaptureJoin([&] { Rect{80, 60, 160, 160}.draw(ColorF{1, 0.5}); });
	const auto actual = CaptureJoin([&] { square.drawFrame(200, 0, ColorF{1, 0.5}); });
	CheckBoundaryPixels(actual, expected);
}

namespace
{
	struct JoinMesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		Optional<uint32> count;
		uint32 requests = 0;
	};

	JoinMesh BuildJoin(const Array<Vec2>& points, const Vec2 widths, const JoinStyle style, const float scale = 1, const bool fail = false)
	{
		JoinMesh result;
		const auto allocate = [&](uint16 vertices, uint32 indices)
		{
			++result.requests;
			if (fail) return Vertex2DBufferPointer{ nullptr, nullptr, 0 };
			result.vertices.resize(vertices);
			result.indices.resize(indices);
			return Vertex2DBufferPointer{ result.vertices.data(), result.indices.data(), 37 };
		};
		result.count = Vertex2DBuilder::TryBuildSmallConvexFrame(allocate, points, style, widths.x, widths.y, Float4{1, 0.25f, 0.5f, 0.5f}, scale);
		return result;
	}

	JoinMesh BuildJoin(const Array<Vec2>& points, const double width, const JoinStyle style, const float scale = 1, const bool fail = false)
	{
		return BuildJoin(points, Vec2{width * 0.5, width * 0.5}, style, scale, fail);
	}

	// Independent convex clipping oracle: positive intersection area means the
	// mesh would blend a translucent stroke more than once.
	double IntersectionArea(const std::array<Vec2, 3>& a, const std::array<Vec2, 3>& b)
	{
		Array<Vec2> polygon{ a.begin(), a.end() };
		for (size_t edge = 0; edge < 3 && not polygon.empty(); ++edge)
		{
			Array<Vec2> clipped;
			const Vec2 start = b[edge], direction = (b[(edge + 1) % 3] - start);
			for (size_t i = 0; i < polygon.size(); ++i)
			{
				const Vec2 p = polygon[i], q = polygon[(i + 1) % polygon.size()];
				const double dp = direction.cross(p - start), dq = direction.cross(q - start);
				if (dp >= 0) clipped << p;
				if ((dp > 0 && dq < 0) || (dp < 0 && dq > 0)) clipped << (p + (q - p) * (dp / (dp - dq)));
			}
			polygon = std::move(clipped);
		}
		double area = 0;
		for (size_t i = 1; i + 1 < polygon.size(); ++i) area += (polygon[i] - polygon[0]).cross(polygon[i + 1] - polygon[0]);
		return Abs(area) * 0.5;
	}

	double CheckMesh(const JoinMesh& mesh)
	{
		REQUIRE(mesh.count.has_value());
		REQUIRE(*mesh.count == mesh.indices.size());
		REQUIRE(mesh.requests == 1);
		REQUIRE(mesh.indices.size() % 3 == 0);
		for (const auto index : mesh.indices) REQUIRE((37 <= index && index < 37 + mesh.vertices.size()));
		for (const auto& vertex : mesh.vertices)
		{
			CHECK(std::isfinite(vertex.pos.x));
			CHECK(std::isfinite(vertex.pos.y));
			CHECK(vertex.color == Float4{1, 0.25f, 0.5f, 0.5f});
		}
		Array<std::array<Vec2, 3>> triangles;
		double area = 0;
		for (size_t i = 0; i < mesh.indices.size(); i += 3)
		{
			const std::array<Vec2, 3> t{ mesh.vertices[mesh.indices[i] - 37].pos,
				mesh.vertices[mesh.indices[i + 1] - 37].pos, mesh.vertices[mesh.indices[i + 2] - 37].pos };
			const double signedArea = ((t[1] - t[0]).cross(t[2] - t[0]) * 0.5);
			CHECK(signedArea > 0);
			area += signedArea;
			for (const auto& previous : triangles) CHECK(IntersectionArea(t, previous) < 0.00001);
			triangles << t;
		}
		return area;
	}
}

TEST_CASE("Renderer2D.join_style_mesh_topology")
{
	const Array<Array<Vec2>> shapes{
		{{40,30}, {280,50}, {120,240}},
		{{40,30}, {280,50}, {260,240}, {60,220}},
		{{80,100}, {240,80}, {240,120}}, // bevel clip
	};
	for (const auto& shape : shapes)
		for (const bool reverse : {false, true})
			for (const double width : {0.25, 4.0, 12.0})
				for (const auto style : {JoinStyle::Miter, JoinStyle::Bevel, JoinStyle::Round})
				{
					CAPTURE(shape.size(), reverse, width, static_cast<int>(style));
					const auto mesh = BuildJoin(reverse ? shape.reversed() : shape, width, style);
					const double area = CheckMesh(mesh);
					double perimeter = 0;
					for (size_t i = 0; i < shape.size(); ++i) perimeter += shape[i].distanceFrom(shape[(i + 1) % shape.size()]);
					if (style == JoinStyle::Miter) CHECK(area == Catch::Approx(perimeter * width).epsilon(0.0001));
					else CHECK(area <= perimeter * width + 0.01);
				}
}

TEST_CASE("Renderer2D.join_style_mesh_rejection_and_failure")
{
	const Array<Array<Vec2>> rejected{
		{}, {{0,0}, {1,1}},
		{{0,0}, {100,0}, {100,0}},
		{{0,0}, {100,0}, {200,0}},
		{{0,0}, {100,0}, {50,20}, {0,100}},
		{{0,0}, {100,100}, {0,100}, {100,0}},
		{{0,0}, {100,0}, {50,1e-10}},
		{{0,0}, {100,0}, {100,1}, {0,100}}, // short inset edge collapses first
	};
	for (const auto& shape : rejected)
		for (const auto style : {JoinStyle::Miter, JoinStyle::Bevel, JoinStyle::Round})
		{
			const auto mesh = BuildJoin(shape, 10, style);
			CHECK_FALSE(mesh.count.has_value());
			CHECK(mesh.requests == 0);
		}
	const Array<Vec2> square{{0,0}, {100,0}, {100,100}, {0,100}};
	for (const double width : {0.0, -10.0, 100.0, 100.001, 120.0})
	{
		const auto mesh = BuildJoin(square, width, JoinStyle::Miter);
		CHECK_FALSE(mesh.count.has_value());
		CHECK(mesh.requests == 0);
	}
	CHECK(BuildJoin(square, 99.9, JoinStyle::Miter).count.has_value());
	const auto failed = BuildJoin(square, 10, JoinStyle::Miter, 1, true);
	REQUIRE(failed.count.has_value());
	CHECK(*failed.count == 0);
	CHECK(failed.requests == 1);
	CHECK(failed.vertices.empty());
}

TEST_CASE("Renderer2D.join_style_round_scale_and_bevel_threshold")
{
	const Array<Vec2> square{{80,60}, {240,60}, {240,220}, {80,220}};
	for (const double width : {2.0, 20.0, 80.0})
	{
		size_t previous = 0;
		for (const float scale : {0.25f, 1.0f, 4.0f, 8.0f})
		{
			const auto mesh = BuildJoin(square, width, JoinStyle::Round, scale);
			CheckMesh(mesh);
			CHECK(mesh.vertices.size() >= previous);
			previous = mesh.vertices.size();
			const size_t count = (mesh.vertices.size() - 4) / 4;
			for (size_t i = 0; i < 4; ++i)
				for (size_t k = 0; k < count; ++k)
				{
					const Vec2 v = mesh.vertices[4 + i * count + k].pos;
					CHECK(v.distanceFrom(square[i]) == Catch::Approx(width * 0.5).margin(0.00002));
					if (k)
					{
						const Vec2 prev = mesh.vertices[4 + i * count + k - 1].pos;
						const double sagitta = (width * 0.5 - ((v + prev) * 0.5).distanceFrom(square[i])) * scale;
						CHECK(sagitta < 0.5);
					}
				}
		}
	}
	for (const double angle : {20.0_deg, 59.99_deg, 60.01_deg, 90.0_deg})
	{
		const double y = (100 * std::tan(angle * 0.5));
		const Array<Vec2> triangle{{0,0}, {100,-y}, {100,y}};
		const auto bevel = BuildJoin(triangle, 2, JoinStyle::Bevel);
		CheckMesh(bevel);
		double minX = 0;
		for (const auto& v : bevel.vertices) minX = Min(minX, static_cast<double>(v.pos.x));
		const double expected = ((angle < 60.0_deg) ? -1.0 : (-1.0 / std::sin(angle * 0.5)));
		CHECK(minX == Catch::Approx(expected).margin(0.00001));
	}
}

TEST_CASE("Renderer2D.join_style_pattern_batch")
{
	const Array<Vec2> square{{80,60}, {240,60}, {240,220}, {80,220}};
	const Pattern::PolkaDot pattern{ .primary = ColorF{1, 0, 0, 0.5}, .background = ColorF{0, 0, 1, 0.5} };
	for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
	{
		CAPTURE(static_cast<int>(style));
		const auto normal = CaptureJoin([&] { DrawFrame(square, 20, pattern, style); });
		const auto batch = CaptureJoin([&]
		{
			for (int i = 0; i < 16382; ++i) RectF{-10,-10,1,1}.draw();
			DrawFrame(square, 20, pattern, style);
		});
		CHECK(normal == batch);
		if (style == JoinStyle::Miter || style == JoinStyle::Bevel)
		{
			const auto old = CaptureJoin([&] { DrawClosedLineStringFallback(Closed(square), style, 20, pattern); });
			CHECK(normal == old);
		}
	}
	for (const bool reversed : {false, true})
	{
		const auto shape = reversed ? square.reversed() : square;
		const auto zoom = CaptureJoin([&]
		{
			const Transformer2D transform{Mat3x2::Scale(4, Vec2{160,140})};
			const Array<Vec2> small{{140,120}, {180,120}, {180,160}, {140,160}};
			DrawFrame(reversed ? small.reversed() : small, 5, ColorF{1,0.5}, JoinStyle::Round);
		});
		const auto direct = CaptureJoin([&] { DrawFrame(shape, 20, ColorF{1,0.5}, JoinStyle::Round); });
		CHECK(zoom == direct);
	}
}

TEST_CASE("Renderer2D.join_style_default_bevel_and_single_composite")
{
	const Array<Array<Vec2>> shapes{
		{{80,140}, {240,120}, {240,160}}, // acute tip
		{{80,60}, {240,80}, {130,210}},
		{{80,60}, {240,60}, {240,220}, {80,220}},
		{{80,60}, {240,80}, {150,110}, {100,210}}, // concave
		{{80,60}, {240,210}, {80,210}, {240,60}}, // crossing
	};
	const ColorF ink{1, 0.5};
	const auto reference = CaptureJoin([&] { Rect{0,0,32,32}.draw(ink); });
	const int single = reference[16][16].r;
	const Pattern::PolkaDot pattern{ .primary = ColorF{1,0,0,0.5}, .background = ColorF{0,0,1,0.5} };
	for (const auto& points : shapes)
		for (const double width : {1.0, 20.0, 220.0})
		{
			CAPTURE(points, width);
			const auto check = [&](const auto& shape, const auto& fill)
			{
				const auto expected = CaptureJoin([&] { shape.drawFrame(width, fill, JoinStyle::Bevel); });
				const auto omitted = CaptureJoin([&] { shape.drawFrame(width, fill); });
				CHECK(omitted == expected);
				int maximum = 0;
				for (const Color pixel : omitted) maximum = std::max({maximum, int(pixel.r), int(pixel.g), int(pixel.b)});
				CHECK(maximum <= single + 1);
			};
			if (points.size() == 3)
			{
				const Triangle shape{points[0], points[1], points[2]};
				check(shape, ink);
				check(shape, pattern);
			}
			else
			{
				const Quad shape{points[0], points[1], points[2], points[3]};
				check(shape, ink);
				check(shape, pattern);
			}

		}
}

TEST_CASE("Renderer2D.join_style_common_convex_frames")
{
	const double h = (100 * std::sqrt(3.0));
	const Array<Array<Vec2>> shapes{
		{{-100,-h/3}, {100,-h/3}, {0,2*h/3}}, // equilateral, side 200
		{{-120,-60}, {120,-60}, {120,60}, {-120,60}}, // rectangle
		{{-80,-60}, {80,-60}, {120,60}, {-120,60}}, // trapezoid
		{{-120,-60}, {80,-60}, {120,60}, {-80,60}}, // parallelogram
		{{0,-60}, {100,0}, {0,60}, {-100,0}}, // rhombus
	};
	// Independent analytic inset-collapse widths. Stay clear of equality, where
	// double rounding can change which side of the strict topology check is used.
	const double limits[]{2*h/3, 120, 120, 120, 12000/std::sqrt(13600.0)};
	for (size_t i = 0; i < shapes.size(); ++i)
		for (int degrees = 0; degrees < 360; degrees += 15)
			for (const bool reverse : {false, true})
			{
				auto points = shapes[i].map([&](const Vec2 p) { return p.rotated(Math::ToRadians(degrees)) + Vec2{320,240}; });
				if (reverse) points.reverse();
				for (const double width : {0.5, 1.0, 2.0, 4.0, 8.0, 12.0, 20.0, 40.0, 80.0})
				{
					CAPTURE(i, degrees, reverse, width);
					const auto mesh = BuildJoin(points, width, JoinStyle::Bevel);
					REQUIRE(mesh.count.has_value());
					CHECK(mesh.requests == 1);
					CHECK(*mesh.count > 0);
					if (width == 20.0) CheckMesh(mesh);
					if (i == 0) CHECK(mesh.vertices.size() == 6); // No clipped tips at 60 degrees.
				}
				CHECK(BuildJoin(points, limits[i] * 0.999, JoinStyle::Bevel).count.has_value());
				const auto collapsed = BuildJoin(points, limits[i] * 1.001, JoinStyle::Bevel);
				CHECK_FALSE(collapsed.count.has_value());
				CHECK(collapsed.requests == 0);
			}
}

TEST_CASE("Renderer2D.join_style_equilateral_threshold")
{
	const double h = (100 * std::sqrt(3.0));
	const Array<Vec2> triangle{{-100,-h/3}, {100,-h/3}, {0,2*h/3}};
	for (const int degrees : {0, 30, 90, 165})
		for (const double width : {20.0, 180.0})
		{
			const auto points = triangle.map([&](const Vec2 p) { return p.rotated(Math::ToRadians(degrees)) + Vec2{320,240}; });
			CAPTURE(degrees, width);
			const auto expected = CaptureJoin([&] { DrawFrame(points, width, ColorF{1,0.5}, JoinStyle::Miter); });
			const auto actual = CaptureJoin([&] { DrawFrame(points, width, ColorF{1,0.5}, JoinStyle::Bevel); });
			const auto fallback = CaptureJoin([&] { DrawClosedLineStringFallback(Closed(points), JoinStyle::Bevel, width, ColorF{1,0.5}); });
			CheckBoundaryPixels(actual, expected);
			CheckBoundaryPixels(fallback, expected);
		}
}

TEST_CASE("Renderer2D.join_style_deterministic_topologies")
{
	std::minstd_rand rng{20261014};
	size_t accepted = 0, rejected = 0;
	for (size_t i = 0; i < 120; ++i)
	{
		Array<Vec2> points;
		for (size_t k = 0; k < 3 + (i % 2); ++k)
			points << Vec2{static_cast<double>(rng() % 241), static_cast<double>(rng() % 241)};
		for (const double width : {0.5, 12.0, 160.0})
			for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
			{
				CAPTURE(i, width, static_cast<int>(style));
				const auto mesh = BuildJoin(points, width, style);
				if (mesh.count)
				{
					++accepted;
					const double area = CheckMesh(mesh);
					if (style == JoinStyle::Miter)
					{
						double perimeter = 0;
						for (size_t k = 0; k < points.size(); ++k) perimeter += points[k].distanceFrom(points[(k + 1) % points.size()]);
						CHECK(area == Catch::Approx(perimeter * width).epsilon(0.0002));
					}
				}
				else
				{
					++rejected;
					CHECK(mesh.requests == 0);
				}
			}
	}
	CHECK(accepted > 100);
	CHECK(rejected > 100);
}

TEST_CASE("Renderer2D.join_style_fallback_matches_reference")
{
	const Array<Array<Vec2>> shapes{
		{{80,60}, {240,80}, {150,110}, {100,210}},
		{{80,60}, {240,210}, {80,210}, {240,60}},
		{{80,100}, {240,100}, {240,100}, {80,200}},
		{{80,100}, {240,100}, {160,100}},
		{{80,100}, {80,100}, {80,100}},
		{{80,60}, {240,80}, {130,210}},
	};
	for (const auto& shape : shapes)
		for (const double width : {-12.0, 0.0, 180.0})
			for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
			{
				REQUIRE_FALSE(BuildJoin(shape, width, style).count.has_value());
				const auto expected = CaptureJoin([&] { DrawClosedLineStringFallback(Closed(shape), style, width, ColorF{1,0.5}); });
				const auto actual = CaptureJoin([&] { DrawFrame(shape, width, ColorF{1,0.5}, style); });
				CHECK(actual == expected);
			}
}

TEST_CASE("Renderer2D.join_style_round_transition")
{
	// Both the whole inset and a single short inset edge can terminate the mesh path.
	// These widths avoid unrelated arc-quality step thresholds.
	const Array<Array<Vec2>> shapes{
		{{-10.5,-10.5}, {10.5,-10.5}, {10.5,10.5}, {-10.5,10.5}},
		{{-30,-20}, {30,-20}, {30,-10}, {-30,20}},
		{{-16,-12}, {16,-12}, {0,20}},
	};
	for (size_t i = 0; i < shapes.size(); ++i)
	{
		const auto& shape = shapes[i];
		double low = 0.1, high = 100;
		for (int k = 0; k < 40; ++k)
		{
			const double mid = (low + high) * 0.5;
			if (BuildJoin(shape, mid, JoinStyle::Round).count) low = mid;
			else high = mid;
		}
		for (const float scale : {1.0f, 4.0f, 8.0f})
			for (const bool reverse : {false, true})
			{
				CAPTURE(i, scale, reverse, low, high);
				const auto points = reverse ? shape.reversed() : shape;
				const double beforeWidth = low - 0.001, afterWidth = high + 0.001;
				REQUIRE(BuildJoin(points, beforeWidth, JoinStyle::Round, scale).count.has_value());
				REQUIRE_FALSE(BuildJoin(points, afterWidth, JoinStyle::Round, scale).count.has_value());
				const auto capture = [&](const double width)
				{
					return CaptureJoin([&]
					{
						const Transformer2D transform{Mat3x2::Scale(scale).translated(400,300)};
						DrawFrame(points, width, ColorF{1,0.5}, JoinStyle::Round);
					});
				};
				const auto before = capture(beforeWidth), after = capture(afterWidth);
				size_t receding = 0;
				for (int y = 0; y < before.height(); ++y)
					for (int x = 0; x < before.width(); ++x)
						if (before[y][x].r > after[y][x].r + 1) ++receding;
				CHECK(receding == 0);
			}
	}
}

TEST_CASE("Renderer2D.join_style_wide_bevel_regression")
{
	// This closed stroke formerly failed libc++'s strict-weak-ordering check
	// in Boost's cluster sorting. Its inset has disappeared at width 160.
	const Array<Vec2> shape{{61,93}, {22,119}, {7,208}, {221,152}};
	constexpr double radius = 80;

	// Independent oracle: intersect the outward support half-planes, then the
	// bisector clipping planes for angles below 60 degrees. No Boost stroke
	// construction, join mesh, or tolerance-based angular sorting is used here.
	Array<Vec2> outline{{-1000,-1000}, {1000,-1000}, {1000,1000}, {-1000,1000}};
	const auto clip = [&](const Vec2 normal, const double offset)
	{
		Array<Vec2> result;
		for (size_t i = 0; i < outline.size(); ++i)
		{
			const Vec2 p = outline[i], q = outline[(i + 1) % outline.size()];
			const double dp = p.dot(normal) - offset, dq = q.dot(normal) - offset;
			if (dp <= 0) result << p;
			if ((dp < 0 && dq > 0) || (dp > 0 && dq < 0)) result << (p + (q - p) * (dp / (dp - dq)));
		}
		outline = std::move(result);
	};
	std::array<Vec2, 4> normals;
	for (size_t i = 0; i < 4; ++i)
	{
		const Vec2 d = (shape[(i + 1) % 4] - shape[i]).normalized();
		normals[i] = Vec2{-d.y, d.x}; // this input is counterclockwise in screen coordinates
		clip(normals[i], shape[i].dot(normals[i]) + radius);
	}
	for (size_t i = 0; i < 4; ++i)
	{
		const Vec2 a = normals[(i + 3) % 4], b = normals[i];
		if (a.dot(b) < -0.5)
		{
			const Vec2 bisector = (a + b).normalized();
			clip(bisector, shape[i].dot(bisector) + radius);
		}
	}
	REQUIRE(outline.size() == 5);
	const Polygon expectedPolygon{outline};
	REQUIRE(expectedPolygon.area() == Catch::Approx(79956.8081345733).margin(0.000001));
	const auto expected = CaptureJoin([&]
	{
		const Transformer2D transform{Mat3x2::Translate(160,80)};
		expectedPolygon.draw(ColorF{1,0.5});
	});
	for (const bool reverse : {false, true})
		for (size_t start = 0; start < 4; ++start)
		{
			CAPTURE(reverse, start);
			Array<Vec2> points;
			for (size_t i = 0; i < 4; ++i) points << shape[(start + i) % 4];
			if (reverse) points.reverse();
			REQUIRE_FALSE(BuildJoin(points, radius * 2, JoinStyle::Bevel).count.has_value());
			const auto actual = CaptureJoin([&]
			{
				const Transformer2D transform{Mat3x2::Translate(160,80)};
				DrawFrame(points, radius * 2, ColorF{1,0.5}, JoinStyle::Bevel);
			});
			CheckBoundaryPixels(actual, expected);
		}
}

TEST_CASE("Renderer2D.join_style_round_fallback_scaling")
{
	const Array<Array<Vec2>> shapes{
		{{80,60}, {240,80}, {150,110}, {100,210}}, // concave
		{{80,60}, {240,210}, {80,210}, {240,60}}, // crossing
		{{80,60}, {240,80}, {130,210}}, // collapsed inset
	};
	for (const auto& shape : shapes)
		for (const bool reverse : {false, true})
		{
			const auto points = reverse ? shape.reversed() : shape;
			REQUIRE_FALSE(BuildJoin(points, 180, JoinStyle::Round).count.has_value());
			const auto direct = CaptureJoin([&] { DrawFrame(points, 180, ColorF{1,0.5}, JoinStyle::Round); });
			const auto scaled = CaptureJoin([&]
			{
				Array<Vec2> small;
				for (const auto p : points) small << p * 0.25;
				const Transformer2D transform{Mat3x2::Scale(4)};
				DrawFrame(small, 45, ColorF{1,0.5}, JoinStyle::Round);
			});
			CHECK(direct == scaled);
		}
}

TEST_CASE("Renderer2D.join_style_inner_outer_regions")
{
	const Array<Array<Vec2>> shapes{
		{{80, 60}, {240, 80}, {130, 210}},
		{{80, 140}, {240, 120}, {240, 160}},
		{{80, 60}, {240, 60}, {240, 220}, {80, 220}},
		{{80, 60}, {240, 80}, {150, 110}, {100, 210}},
		{{80, 60}, {240, 210}, {80, 210}, {240, 60}},
	};
	const ColorF ink{1, 0.5};
	const auto reference = CaptureJoin([&] { Rect{0, 0, 32, 32}.draw(ink); });
	const uint32 single = reference[16][16].r;
	for (size_t i = 0; i < shapes.size(); ++i)
		for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
			for (const Vec2 widths : {Vec2{0, 12}, Vec2{12, 0}, Vec2{4, 16}, Vec2{160, 8}})
			{
				CAPTURE(i, style, widths);
				const auto& p = shapes[i];
				const auto check = [&](const auto& shape, const auto& reverse)
				{
					const auto actual = CaptureJoin([&] { shape.drawFrame(widths.x, widths.y, ink, style); });
					const auto reversed = CaptureJoin([&] { reverse.drawFrame(widths.x, widths.y, ink, style); });
					const auto fallback = CaptureJoin([&] { DrawSmallPolygonFrameFallback(p, style, widths.x, widths.y, ink); });
					CheckBoundaryPixels(actual, reversed);
					CheckBoundaryPixels(actual, fallback);
					Image expected{actual.size(), Palette::Black};
					shape.overwriteFrame(expected, style, widths.x, widths.y, Color{single});
					// CPU distance AA and GPU chord/MSAA coverage differ at corners.
					// Compare uniform full/empty regions away from the AA boundary.
					// Pixel-center masks miss subpixel wedges at acute tips and holes.
					size_t interiorErrors = 0;
					for (int y = 1; y + 1 < expected.height(); ++y)
						for (int x = 1; x + 1 < expected.width(); ++x)
						{
							bool uniform = (expected[y][x].r == 0 || expected[y][x].r == single);
							for (int dy = -1; dy <= 1; ++dy)
								for (int dx = -1; dx <= 1; ++dx) uniform &= (expected[y + dy][x + dx].r == expected[y][x].r);
							interiorErrors += uniform && (Abs(int(actual[y][x].r) - expected[y][x].r) > 1);
						}
					CHECK(interiorErrors == 0);
					uint32 maximum = 0;
					for (const Color pixel : actual) maximum = Max(maximum, uint32(pixel.r));
					CHECK(maximum <= single + 1);
				};
				if (p.size() == 3) check(Triangle{p[0], p[1], p[2]}, Triangle{p[2], p[1], p[0]});
				else check(Quad{p[0], p[1], p[2], p[3]}, Quad{p[3], p[2], p[1], p[0]});
			}
}

TEST_CASE("Renderer2D.join_style_inner_outer_common_meshes")
{
	const double h = (100 * std::sqrt(3.0));
	const Array<Array<Vec2>> shapes{
		{{-100, -h/3}, {100, -h/3}, {0, 2*h/3}},
		{{-120, -60}, {120, -60}, {120, 60}, {-120, 60}},
		{{-80, -60}, {80, -60}, {120, 60}, {-120, 60}},
		{{-120, -60}, {80, -60}, {120, 60}, {-80, 60}},
		{{0, -60}, {100, 0}, {0, 60}, {-100, 0}},
	};
	for (size_t i = 0; i < shapes.size(); ++i)
		for (int degrees = 0; degrees < 360; degrees += 15)
			for (const bool reverse : {false, true})
				for (const Vec2 widths : {Vec2{0, 20}, Vec2{20, 0}, Vec2{4, 16}, Vec2{16, 4}, Vec2{40, 200}})
					for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
					{
						CAPTURE(i, degrees, reverse, widths, style);
						auto points = shapes[i].map([&](Vec2 p) { return p.rotated(Math::ToRadians(degrees)) + Vec2{320, 240}; });
						if (reverse) points.reverse();
						const auto mesh = BuildJoin(points, widths, style);
						REQUIRE(mesh.count.has_value());
						CHECK(mesh.requests == 1);
						CHECK(*mesh.count > 0);
						if (degrees == 0) CheckMesh(mesh);
					}
}

TEST_CASE("Renderer2D.join_style_inner_outer_defaults_patterns_and_no_ops")
{
	const ColorF ink{1, 0.5};
	const PatternParameters pattern = Pattern::PolkaDot{.primary = ink, .background = ColorF{0.3, 0.8, 1, 0.5}};
	const auto blank = CaptureJoin([] {});
	const auto check = [&](const auto& shape)
	{
		for (const Vec2 widths : {Vec2{4, 16}, Vec2{160, 0}})
		{
			const auto solid = CaptureJoin([&] { shape.drawFrame(widths.x, widths.y, ink); });
			const auto explicitSolid = CaptureJoin([&] { shape.drawFrame(widths.x, widths.y, ink, JoinStyle::Bevel); });
			CHECK(solid == explicitSolid);
			const auto omitted = CaptureJoin([&] { shape.drawFrame(widths.x, widths.y, pattern); });
			const auto explicitPattern = CaptureJoin([&] { shape.drawFrame(widths.x, widths.y, pattern, JoinStyle::Bevel); });
			CHECK(omitted == explicitPattern);
			const auto rollover = CaptureJoin([&]
			{
				for (int i = 0; i < 16382; ++i) RectF{-10, -10, 1, 1}.draw();
				shape.drawFrame(widths.x, widths.y, pattern);
			});
			CHECK(rollover == omitted);
		}
		for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
		{
			const auto noOp = CaptureJoin([&]
			{
				shape.drawFrame(0, 0, ink, style);
				shape.drawFrame(-1, 4, ink, style);
				shape.drawFrame(4, -1, pattern, style);
				Triangle{80, 80, 120, 120, 160, 160}.drawFrame(4, 8, ink, style);
				Quad{80, 80, 120, 80, 80, 80, 80, 120}.drawFrame(4, 8, pattern, style);
			});
			CHECK(noOp == blank);
		}
	};
	check(Triangle{80, 60, 240, 80, 130, 210});
	check(Quad{80, 60, 240, 210, 80, 210, 240, 60});
}
