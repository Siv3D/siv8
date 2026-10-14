//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
//
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
//
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include "../Siv3D/src/Siv3D/Renderer2D/Vertex2DBuilder.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <array>

namespace
{
	struct ArcMesh
	{
		Array<Vertex2D> vertices;
		Array<Vertex2D::IndexType> indices;
		uint32 allocations = 0;
		Vertex2D::IndexType count = 0;
	};

	ArcMesh BuildArc(const LineCap cap, const float start, const float sweep,
		const ColorFillDirection coloring = ColorFillDirection::InOut, const bool fail = false)
	{
		ArcMesh mesh;
		const auto allocate = [&](uint16 vertices, uint32 indices)
		{
			++mesh.allocations;
			if (fail) return Vertex2DBufferPointer{ nullptr, nullptr, 0 };
			mesh.vertices.resize(vertices);
			mesh.indices.resize(indices);
			return Vertex2DBufferPointer{ mesh.vertices.data(), mesh.indices.data(), 37 };
		};
		mesh.count = Vertex2DBuilder::BuildCircleArc(allocate, cap, Float2{ 200, 140 }, 53, start, sweep, 20,
			coloring, Float4{ 1, 0, 0, 0.5f }, Float4{ 0, 0, 1, 0.5f }, 1);
		return mesh;
	}

	void CheckNear(const Float2 actual, const Vec2 expected)
	{
		CHECK(actual.x == Catch::Approx(expected.x).margin(0.001));
		CHECK(actual.y == Catch::Approx(expected.y).margin(0.001));
	}

	void DrawArc(const Circle& circle, const LineCap cap, const double sweep, const int32 coloring, const bool useDefault = false)
	{
		const ColorF red{ 1, 0, 0, 0.5 }, blue{ 0, 0, 1, 0.5 };
		const Pattern::PolkaDot pattern{ .primary = red, .background = blue };
		if (useDefault)
		{
			switch (coloring)
			{
			case 0: CHECK(&circle.drawArc(0, sweep, 7, 13, red) == &circle); break;
			case 1: CHECK(&circle.drawArc(0, sweep, 7, 13, red, blue) == &circle); break;
			case 2: CHECK(&circle.drawArc(0, sweep, 7, 13, Arg::start(red), Arg::end(blue)) == &circle); break;
			case 3: CHECK(&circle.drawArc(0, sweep, 7, 13, pattern) == &circle); break;
			}
		}
		else
		{
			switch (coloring)
			{
			case 0: CHECK(&circle.drawArc(cap, 0, sweep, 7, 13, red) == &circle); break;
			case 1: CHECK(&circle.drawArc(cap, 0, sweep, 7, 13, red, blue) == &circle); break;
			case 2: CHECK(&circle.drawArc(cap, 0, sweep, 7, 13, Arg::start(red), Arg::end(blue)) == &circle); break;
			case 3: CHECK(&circle.drawArc(cap, 0, sweep, 7, 13, pattern) == &circle); break;
			}
		}
	}

	struct CapturedArc
	{
		Image image;
		int64 triangles;
	};

	template <class Draw>
	CapturedArc CaptureArc(Draw&& draw)
	{
		const ColorF background = Scene::GetBackground();
		const ScopeExit restore{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedColorMul2D colorMul{ Palette::White };
		const ScopedColorAdd2D colorAdd{ 0.0 };
		REQUIRE(System::Update());
		draw();
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		Image image = ScreenCapture::GetFrame();
		REQUIRE(image.width() >= 320);
		REQUIRE(image.height() >= 240);
		return { std::move(image), Profiler::GetFrameMetrics().triangleCount };
	}
}

TEST_CASE("Renderer2D.circle_arc_square_geometry")
{
	for (const float start : { 0.0f, 0.37f, -2.4f })
		for (const float sweep : { 0.0f, 0.03f, 1.5f, -1.5f, 4.5f, -4.5f, Math::TwoPiF - 0.03f })
		{
			CAPTURE(start, sweep);
			const auto mesh = BuildArc(LineCap::Square, start, sweep);
			REQUIRE(mesh.allocations == 1);
			REQUIRE(mesh.vertices.size() >= 6);
			CHECK(mesh.count == mesh.indices.size());
			CHECK(mesh.indices.size() == (mesh.vertices.size() / 2 - 1) * 6);
			const double begin = start + Min(sweep, 0.0f), end = begin + Abs(sweep);
			const Vec2 startDir{ std::sin(begin), -std::cos(begin) }, endDir{ std::sin(end), -std::cos(end) };
			const Vec2 startTangent{ -startDir.y, startDir.x }, endTangent{ -endDir.y, endDir.x };
			const Vec2 center{ 200, 140 };
			const size_t n = mesh.vertices.size();
			CheckNear(mesh.vertices[0].pos, center + startDir * 53 - startTangent * 10);
			CheckNear(mesh.vertices[1].pos, center + startDir * 73 - startTangent * 10);
			CheckNear(mesh.vertices[2].pos, center + startDir * 53);
			CheckNear(mesh.vertices[3].pos, center + startDir * 73);
			CheckNear(mesh.vertices[n - 2].pos, center + endDir * 53 + endTangent * 10);
			CheckNear(mesh.vertices[n - 1].pos, center + endDir * 73 + endTangent * 10);
			for (const auto index : mesh.indices) CHECK((37 <= index && index < 37 + n));
			for (size_t i = 0; i < mesh.indices.size(); i += 3)
			{
				const Vec2 a = mesh.vertices[mesh.indices[i] - 37].pos;
				const Vec2 b = mesh.vertices[mesh.indices[i + 1] - 37].pos;
				const Vec2 c = mesh.vertices[mesh.indices[i + 2] - 37].pos;
				CHECK((b - a).cross(c - a) > 0);
			}
		}
}

TEST_CASE("Renderer2D.circle_arc_square_colors_and_failure")
{
	for (const float sweep : { 0.0f, 1.5f, -1.5f })
		for (const auto coloring : { ColorFillDirection::InOut, ColorFillDirection::LeftRight })
		{
			CAPTURE(sweep, static_cast<int32>(coloring));
			const auto mesh = BuildArc(LineCap::Square, 0.37f, sweep, coloring);
			REQUIRE(mesh.vertices.size() >= 6);
			const size_t pairs = (mesh.vertices.size() / 2);
			const double length = (63 * Abs(sweep) + 20);
			for (size_t i = 0; i < pairs; ++i)
			{
				double f = 0;
				if (i == pairs - 1) f = 1;
				else if (i != 0) f = (10 + (pairs == 3 ? 0 : 63 * Abs(sweep) * (i - 1) / (pairs - 3))) / length;
				if (sweep < 0) f = 1 - f;
				for (size_t side = 0; side < 2; ++side)
				{
					const Float4 color = mesh.vertices[i * 2 + side].color;
					const double blue = (coloring == ColorFillDirection::InOut ? side : f);
					CHECK(color.x == Catch::Approx(1 - blue).margin(0.00001));
					CHECK(color.z == Catch::Approx(blue).margin(0.00001));
					CHECK(color.y == 0);
					CHECK(color.w == 0.5f);
				}
			}
			const auto failure = BuildArc(LineCap::Square, 0.37f, sweep, coloring, true);
			CHECK(failure.allocations == 1);
			CHECK(failure.count == 0);
			CHECK(failure.vertices.isEmpty());
		}
}

TEST_CASE("Renderer2D.circle_arc_defaults_and_full_square")
{
	const Circle circle{ 200, 140, 60 };
	for (const double sweep : { 0.0, 1.5, -4.5, Math::TwoPi, -Math::TwoPi, 10.0, -10.0 })
		for (const int32 coloring : { 0, 1, 2, 3 })
		{
			CAPTURE(sweep, coloring);
			const auto flat = CaptureArc([&] { DrawArc(circle, LineCap::Flat, sweep, coloring); });
			const auto defaults = CaptureArc([&] { DrawArc(circle, LineCap::Flat, sweep, coloring, true); });
			CHECK((defaults.image == flat.image));
			CHECK(defaults.triangles == flat.triangles);
			if (sweep == 0) CHECK(flat.triangles == 0);
			if (Math::TwoPi <= Abs(sweep))
			{
				const auto square = CaptureArc([&] { DrawArc(circle, LineCap::Square, sweep, coloring); });
				CHECK((square.image == flat.image));
				CHECK(square.triangles == flat.triangles);
			}
		}
}

TEST_CASE("Renderer2D.circle_arc_square_pixels_and_batch")
{
	const Circle circle{ 200, 140, 60 };
	for (const double sweep : { 0.0, Math::HalfPi, -Math::HalfPi })
		for (const int32 coloring : { 0, 1, 2, 3 })
		{
			CAPTURE(sweep, coloring);
			const auto square = CaptureArc([&] { DrawArc(circle, LineCap::Square, sweep, coloring); });
			const int32 startX = (sweep < 0 ? 205 : 195);
			CHECK(square.image[77][startX] != Palette::Black);
			CHECK(square.image[77][sweep < 0 ? 215 : 185] == Palette::Black);
			CHECK(square.image[60][startX] == Palette::Black);
			if (sweep == 0)
			{
				CHECK(square.triangles == 4);
				CHECK(square.image[77][205] != Palette::Black);
				if (coloring == 0) CHECK(square.image[77][200] == square.image[77][195]);
			}
			else CHECK(square.image[145][sweep < 0 ? 137 : 263] != Palette::Black);
			const auto batch = CaptureArc([&]
			{
				for (int32 i = 0; i < 16382; ++i) RectF{ -10, -10, 1, 1 }.draw();
				DrawArc(circle, LineCap::Square, sweep, coloring);
			});
			CHECK((batch.image == square.image));
			CHECK(batch.triangles == square.triangles + 32764);
		}
}

TEST_CASE("Renderer2D.benchmark_circle_arc_vertices", "[.benchmark]")
{
	std::array<Vertex2D, 520> vertices;
	std::array<Vertex2D::IndexType, 1600> indices;
	const auto allocate = [&](uint16, uint32) { return Vertex2DBufferPointer{ vertices.data(), indices.data(), 0 }; };
	ankerl::nanobench::Bench bench;
	bench.title("Circle arc vertices").unit("arc").epochs(11).minEpochIterations(200);
	for (const float radius : { 8.0f, 60.0f, 300.0f })
		for (const float sweep : { 0.15f, 1.5f, 4.5f })
			for (const LineCap cap : { LineCap::Flat, LineCap::Square })
				for (const auto coloring : { ColorFillDirection::InOut, ColorFillDirection::LeftRight })
				{
					const std::string name = std::to_string(radius) + "/" + std::to_string(sweep)
						+ (cap == LineCap::Flat ? "/Flat" : "/Square") + (coloring == ColorFillDirection::InOut ? "/radial" : "/along");
					bench.run(name, [&]
					{
						const auto count = Vertex2DBuilder::BuildCircleArc(allocate, cap, Float2{ 200, 140 }, radius - 3,
							0.37f, sweep, 8, coloring, Float4{ 1, 0, 0, 0.5f }, Float4{ 0, 0, 1, 0.5f }, 1);
						ankerl::nanobench::doNotOptimizeAway(count);
						ankerl::nanobench::doNotOptimizeAway(vertices.data());
						ankerl::nanobench::doNotOptimizeAway(indices.data());
					});
				}
}
