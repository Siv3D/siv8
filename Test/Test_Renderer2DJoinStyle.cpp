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
				// Different triangulations can round an edge across an MSAA sample.
				// Round also changes tessellation. Differences above 1/255 must be
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
						CHECK(amin < amax);
						CHECK(emin < emax);
						CHECK((emin <= actual[y][x].r && actual[y][x].r <= emax));
						CHECK((amin <= expected[y][x].r && expected[y][x].r <= amax));
					}
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

namespace
{
	struct JoinMesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		Optional<uint32> count;
		uint32 requests = 0;
	};

	JoinMesh BuildJoin(const Array<Vec2>& points, const double width, const JoinStyle style, const float scale = 1, const bool fail = false)
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
		result.count = Vertex2DBuilder::BuildClosedLineString(allocate, points, style, width, Float4{1, 0.25f, 0.5f, 0.5f}, scale);
		return result;
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
	CHECK_FALSE(BuildJoin(square, 10, JoinStyle::Default).count.has_value());
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

TEST_CASE("Renderer2D.join_style_pattern_batch_and_default")
{
	const Array<Vec2> square{{80,60}, {240,60}, {240,220}, {80,220}};
	const Pattern::PolkaDot pattern{ .primary = ColorF{1, 0, 0, 0.5}, .background = ColorF{0, 0, 1, 0.5} };
	for (const auto style : {JoinStyle::Default, JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
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
		if (style == JoinStyle::Default)
		{
			const auto old = CaptureJoin([&] { LineString{Closed(square)}.drawClosed(20, pattern); });
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

TEST_CASE("Renderer2D.join_style_fallback_unchanged")
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
