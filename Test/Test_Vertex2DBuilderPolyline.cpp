//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <cmath>

namespace
{
	using Index = Vertex2D::IndexType;
	constexpr Index Offset = 37, Guard = 65535;
	const Float4 Ink{ 0.2f, 0.4f, 0.8f, 0.5f };

	struct Mesh
	{
		Array<Vertex2D> vertices;
		Array<Index> indices;
	};

	template <class Build>
	Mesh Capture(Build&& build)
	{
		Mesh mesh;
		Vertex2D sentinel;
		sentinel.set(-1000, -2000, -3, -4, Float4{ -1, -2, -3, -4 });
		uint32 allocations = 0;
		const auto allocate = [&](const uint16 vertices, const uint32 indices)
		{
			++allocations;
			mesh.vertices.assign(vertices + 2, sentinel);
			mesh.indices.assign(indices + 2, Guard);
			return Vertex2DBufferPointer{ mesh.vertices.data() + 1, mesh.indices.data() + 1, Offset };
		};
		const uint32 count = build(allocate);
		REQUIRE(allocations == 1);
		REQUIRE(mesh.indices.size() == count + 2);
		for (const auto* v : { &mesh.vertices.front(), &mesh.vertices.back() })
		{
			CHECK(v->pos == sentinel.pos);
			CHECK(v->color == sentinel.color);
		}
		CHECK(mesh.indices.front() == Guard);
		CHECK(mesh.indices.back() == Guard);
		for (const auto& v : mesh.vertices) CHECK(v.tex == sentinel.tex);
		mesh.vertices.pop_back();
		mesh.vertices.erase(mesh.vertices.begin());
		mesh.indices.pop_back();
		mesh.indices.erase(mesh.indices.begin());
		return mesh;
	}

	Mesh Solid(const Array<Vec2>& points, const bool closed, const bool inner = false,
		const float scale = 1, const Optional<Float2>& offset = none)
	{
		return Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat,
				points, offset, 8, inner, CloseRing{ closed }, Ink, scale);
		});
	}

	void CheckStrip(const Mesh& mesh, const Array<Float2>& points, const bool closed)
	{
		REQUIRE(mesh.vertices.size() == points.size() * 2);
		const size_t segments = (closed ? points.size() : points.size() - 1);
		REQUIRE(mesh.indices.size() == segments * 6);
		for (size_t i = 0; i < points.size(); ++i)
		{
			CAPTURE(i);
			// Each pair stays centered on its prepared contour point.
			const Vec2 center = (Vec2{ mesh.vertices[i * 2].pos } + Vec2{ mesh.vertices[i * 2 + 1].pos }) * 0.5;
			CHECK(center.distanceFrom(points[i]) < 0.0001);
		}
		for (size_t i = 0; i < segments; ++i)
		{
			const size_t next = ((i + 1) % points.size());
			const Index a = static_cast<Index>(i * 2), b = static_cast<Index>(next * 2);
			const Index expected[]{ a, Index(a + 1), b, b, Index(a + 1), Index(b + 1) };
			for (size_t k = 0; k < 6; ++k) CHECK(mesh.indices[i * 6 + k] == Offset + expected[k]);
			// Both ends of each strip edge lie on the parallel lines at +/- half
			// width. This checks joins geometrically without duplicating the miter formula.
			const Vec2 edge = (Vec2{ points[next] } - Vec2{ points[i] });
			for (const size_t j : { i, next })
			{
				for (size_t side = 0; side < 2; ++side)
				{
					const Vec2 delta = (Vec2{ mesh.vertices[j * 2 + side].pos } - Vec2{ points[i] });
					CHECK(edge.cross(delta) / edge.length() == Catch::Approx(side ? -4.0 : 4.0).margin(0.01));
				}
			}
		}
	}
}

TEST_CASE("PolylineMesh.smooth_simplified_and_reused_scratch")
{
	// Deliberately alternate sharp, smooth, open and collapsed paths to check
	// that reuse does not retain directions or split points from earlier draws.
	for (const float scale : { 0.5f, 4.0f })
	for (const bool closed : { false, true })
	for (const bool inner : { false, true })
	{
		(void)Solid({ { 0, 0 }, { 40, 0 }, { 20, 20 } }, true);
		const Array<Vec2> source{ { 0, 0 }, { 0, 0 }, { 0.02, 0 }, { 80, 0 }, { 80, 60 }, { 0, 60 } };
		const Float2 offset{ 7, -11 };
		Array<Float2> expected{ { 7, -11 }, { 87, -11 }, { 87, 49 }, { 7, 49 } };
		const auto mesh = Solid(source, closed, inner, scale, offset);
		CheckStrip(mesh, expected, closed);
		for (const auto& v : mesh.vertices) CHECK(v.color == Ink);

		// Float2 Shape2D contours and double-precision Polygon contours share
		// preparation and must produce identical closed strips for exact inputs.
		if (closed)
		{
			const auto shape = Capture([&](const BufferCreatorFunc& allocate)
			{
				return Vertex2DBuilder::BuildShape2DFrame(allocate, expected, none, 8, Ink, scale);
			});
			CHECK(shape.indices == mesh.indices);
			REQUIRE(shape.vertices.size() == mesh.vertices.size());
			for (size_t i = 0; i < mesh.vertices.size(); ++i) CHECK(shape.vertices[i].pos == mesh.vertices[i].pos);
		}
		CheckStrip(Solid({ { 0, 0 }, { 80, 0 }, { 0, 0 } }, true), { { 0, 0 }, { 80, 0 } }, false);
	}
}

TEST_CASE("PolylineMesh.sharp_corners_and_source_colors")
{
	const Array<Vec2> source{ { 0, 0 }, { 0, 0 }, { 40, 0 }, { 20, 20 } };
	const Array<ColorF> colors{ Palette::Red, Palette::Yellow, Palette::Green, Palette::Blue };
	for (const float scale : { 0.5f, 4.0f })
	for (const bool closed : { false, true })
	{
		// A 135-degree turn: the inserted displacement has angle 67.5 degrees.
		const float epsilon = (0.001f / scale);
		const Float2 delta{ float(std::cos(Math::Pi * 3 / 8)) * epsilon, float(std::sin(Math::Pi * 3 / 8)) * epsilon };
		Array<Float2> expected{ { 0, 0 }, { 40, 0 }, Float2{ 40, 0 } + delta, { 20, 20 } };
		Array<size_t> sources{ 0, 2, 2, 3 };
		if (closed)
		{
			expected.push_back({ -delta.x, delta.y });
			sources.push_back(0);
		}
		const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat,
				source, none, 8, false, CloseRing{ closed }, colors, scale);
		});
		CheckStrip(mesh, expected, closed);
		for (size_t i = 0; i < sources.size(); ++i)
		{
			CHECK(mesh.vertices[i * 2].color == colors[sources[i]].toFloat4());
			CHECK(mesh.vertices[i * 2 + 1].color == colors[sources[i]].toFloat4());
		}
	}

	// Only the first corner is acute: exercise the delayed copy at ring closure.
	const Array<Vec2> firstSharp{ { 0, 0 }, { 100, 0 }, { 100, 100 }, { 80, 100 } };
	const auto mesh = Solid(firstSharp, true);
	REQUIRE(mesh.vertices.size() == 10);
	for (size_t i = 0; i < firstSharp.size(); ++i)
	{
		CHECK(((mesh.vertices[i * 2].pos + mesh.vertices[i * 2 + 1].pos) * 0.5f).distanceFrom(Float2{ firstSharp[i] }) < 0.0001f);
	}
}

TEST_CASE("PolylineMesh.gradient_and_unsplit_turn")
{
	const Array<Vec2> source{ { 0, 0 }, { 40, 0 }, { 40, 30 } };
	const Float4 red{ 1, 0, 0, 0.5f }, blue{ 0, 0, 1, 0.5f };
	const auto mesh = Capture([&](const BufferCreatorFunc& allocate)
	{
		return Vertex2DBuilder::BuildLineString(allocate, LineCap::Flat, LineCap::Flat,
			source, none, 8, false, red, blue, 1);
	});
	CheckStrip(mesh, { { 0, 0 }, { 40, 0 }, { 40, 30 } }, false);
	const Float4 expected[]{ red, red.lerp(blue, 40.0f / 70.0f), blue };
	for (size_t i = 0; i < 3; ++i) CHECK(mesh.vertices[i * 2].color == expected[i]);

	// The inner path skips sharp splitting. This near reversal exercises the
	// existing alternate bisector without creating auxiliary points.
	const Array<Vec2> turn{ { 0, 0 }, { 40, 0 }, { 10, 0.1 } };
	const auto folded = Solid(turn, false, true);
	REQUIRE(folded.vertices.size() == 6);
	const Vec2 tangent = (turn[2] - turn[0]).normalized();
	const Vec2 normal{ -tangent.y, tangent.x };
	const Vec2 expectedMiter = (normal * (4 / tangent.x));
	CHECK(Vec2{ folded.vertices[2].pos }.distanceFrom(turn[1] + expectedMiter) < 0.0001);
	CHECK(Vec2{ folded.vertices[3].pos }.distanceFrom(turn[1] - expectedMiter) < 0.0001);
}

TEST_CASE("PolylineMesh.collapsed_and_allocation_failure")
{
	uint32 allocations = 0;
	const auto fail = [&](uint16, uint32) { ++allocations; return Vertex2DBufferPointer{}; };
	for (const Array<Vec2>& points : { Array<Vec2>{}, Array<Vec2>{ { 1, 1 } }, Array<Vec2>(4, Vec2{ 1, 1 }) })
	{
		CHECK(Vertex2DBuilder::BuildLineString(fail, LineCap::Flat, LineCap::Flat,
			points, none, 8, false, CloseRing::Yes, Ink, 1) == 0);
	}
	CHECK(allocations == 0);
	const Array<Vec2> square{ { 0, 0 }, { 40, 0 }, { 40, 40 }, { 0, 40 } };
	CHECK(Vertex2DBuilder::BuildLineString(fail, LineCap::Flat, LineCap::Flat,
		square, none, 8, false, CloseRing::Yes, Ink, 1) == 0);
	CHECK(allocations == 1);
	CheckStrip(Solid(square, true), { { 0, 0 }, { 40, 0 }, { 40, 40 }, { 0, 40 } }, true);
}

TEST_CASE("PolylineMesh.gradient_distances_and_caps")
{
	const Float4 red{ 1, 0, 0, 0.25f }, blue{ 0, 0, 1, 0.75f };
	for (const bool sharp : { false, true })
	for (const bool inner : { false, true })
	for (const float scale : { 0.5f, 4.0f })
	for (const LineCap start : { LineCap::Flat, LineCap::Square, LineCap::Round })
	for (const LineCap end : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		CAPTURE(sharp, inner, scale, static_cast<int>(start), static_cast<int>(end));
		const Array<Vec2> source = sharp ? Array<Vec2>{ { 0, 0 }, { 40, 0 }, { 20, 20 } }
			: Array<Vec2>{ { 0, 0 }, { 0, 0 }, { 0.02, 0 }, { 40, 0 }, { 40, 30 } };
		Array<Float2> contour{ { 0, 0 }, { 40, 0 } };
		if (sharp && not inner)
		{
			const float epsilon = (0.001f / scale);
			contour.push_back(Float2{ 40, 0 } + Float2{ float(std::cos(Math::Pi * 3 / 8)) * epsilon, float(std::sin(Math::Pi * 3 / 8)) * epsilon });
		}
		contour.push_back(sharp ? Float2{ 20, 20 } : Float2{ 40, 30 });
		const auto solid = Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildLineString(allocate, start, end, source, none, 8, inner, CloseRing::No, Ink, scale);
		});
		const auto gradient = Capture([&](const BufferCreatorFunc& allocate)
		{
			return Vertex2DBuilder::BuildLineString(allocate, start, end, source, none, 8, inner, red, blue, scale);
		});
		REQUIRE(gradient.vertices.size() == solid.vertices.size());
		CHECK(gradient.indices == solid.indices);
		for (size_t i = 0; i < solid.vertices.size(); ++i) CHECK(gradient.vertices[i].pos == solid.vertices[i].pos);

		// Independent double-precision distances along the expected contour.
		// The start/end caps each contribute half the stroke width to the ramp.
		Array<double> distances{ 0.0 };
		for (size_t i = 1; i < contour.size(); ++i)
		{
			const Vec2 delta = (Vec2{ contour[i] } - Vec2{ contour[i - 1] });
			distances.push_back(distances.back() + std::hypot(delta.x, delta.y));
		}
		const double startLength = (start == LineCap::Flat ? 0.0 : 4.0);
		const double total = (startLength + distances.back() + (end == LineCap::Flat ? 0.0 : 4.0));
		for (size_t i = 0; i < contour.size(); ++i)
		{
			const double t = ((startLength + distances[i]) / total);
			for (size_t side = 0; side < 2; ++side)
			{
				const auto color = gradient.vertices[i * 2 + side].color;
				CHECK(color.x == Catch::Approx(1 - t).margin(0.000002));
				CHECK(color.y == 0);
				CHECK(color.z == Catch::Approx(t).margin(0.000002));
				CHECK(color.w == Catch::Approx(0.25 + 0.5 * t).margin(0.000002));
			}
		}
	}
}

TEST_CASE("PolylineMesh.gradient_zero_lengths_and_failure")
{
	const Float4 red{ 1, 0, 0, 1 }, blue{ 0, 0, 1, 1 };
	const auto build = [&](const BufferCreatorFunc& allocate, const Array<Vec2>& points, const Optional<Float2>& offset, const LineCap cap)
	{
		return Vertex2DBuilder::BuildLineString(allocate, cap, cap, points, offset, 8, false, red, blue, 1);
	};
	// The double-precision source keeps both points, but its Float2 offset
	// makes the prepared edge zero length. Reuse scratch after a longer path.
	(void)Capture([&](const BufferCreatorFunc& allocate) { return build(allocate, { { 0, 0 }, { 40, 0 }, { 20, 20 } }, none, LineCap::Round); });
	const auto zero = Capture([&](const BufferCreatorFunc& allocate)
	{
		return build(allocate, { { 0, 0 }, { 1, 0 } }, Float2{ 16777216, 0 }, LineCap::Flat);
	});
	REQUIRE(zero.vertices.size() == 4);
	for (const auto& v : zero.vertices)
	{
		CHECK(v.pos == Float2{ 16777216, 0 });
		CHECK(v.color == red);
	}
	const auto single = Capture([&](const BufferCreatorFunc& allocate) { return build(allocate, { { 10, 20 } }, none, LineCap::Round); });
	const auto collapsed = Capture([&](const BufferCreatorFunc& allocate) { return build(allocate, Array<Vec2>(4, Vec2{ 10, 20 }), none, LineCap::Round); });
	REQUIRE(collapsed.vertices.size() == single.vertices.size());
	CHECK(collapsed.indices == single.indices);
	for (size_t i = 0; i < single.vertices.size(); ++i)
	{
		CHECK(collapsed.vertices[i].pos == single.vertices[i].pos);
		CHECK(collapsed.vertices[i].color == single.vertices[i].color);
	}
	uint32 allocations = 0;
	const auto fail = [&](uint16, uint32) { ++allocations; return Vertex2DBufferPointer{}; };
	CHECK(build(fail, { { 0, 0 }, { 40, 0 }, { 20, 20 } }, none, LineCap::Round) == 0);
	CHECK(allocations == 1);
}
