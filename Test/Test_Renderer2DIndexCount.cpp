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

namespace
{
	template <class Draw>
	Image CaptureIndexedDraw(Draw&& draw, const Optional<int64> expectedTriangles = none)
	{
		const ColorF previousBackground = Scene::GetBackground();
		const ScopeExit restoreBackground{ [&] { Scene::SetBackground(previousBackground); } };
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
		if (expectedTriangles)
		{
			CHECK(Profiler::GetFrameMetrics().triangleCount == *expectedTriangles);
		}
		Image image = ScreenCapture::GetFrame();
		REQUIRE(image.width() >= 300);
		REQUIRE(image.height() >= 240);
		return image;
	}

	void CheckGridPixels(const Image& image, const Color expected)
	{
		for (const int32 y : { 32, 100, 168 })
		{
			for (const int32 x : { 32, 120, 208 })
			{
				CHECK(image[y][x] == expected);
			}
		}
		CHECK(image[190][230] == Palette::Black);
	}
}

TEST_CASE("Renderer2D.indexed_grid_draw")
{
	const Texture red{ Image{ 2, 2, Palette::Red } };
	REQUIRE(red);
	for (const int32 divisions : { 128 })
	{
		INFO(divisions);
		const Mesh2D mesh = Mesh2D::Grid(RectF{ 20, 20, 200, 160 }, Size{ divisions, divisions });
		REQUIRE(mesh.validate());
		const int64 triangleCount = static_cast<int64>(mesh.indices.size());
		CheckGridPixels(CaptureIndexedDraw([&] { mesh.draw(); }, triangleCount), Palette::White);
		CheckGridPixels(CaptureIndexedDraw([&] { mesh.draw(Vec2{ 0, 0 }, red); }, triangleCount), Palette::Red);
	}
}

TEST_CASE("Renderer2D.index_count_boundaries")
{
	for (const size_t triangleCount : { 21845u, 21846u, 87381u, 87382u })
	{
		INFO(triangleCount);
		Mesh2D mesh{ 6, triangleCount };
		const Float2 positions[] = { { 20, 20 }, { 80, 20 }, { 20, 80 }, { 120, 20 }, { 180, 20 }, { 120, 80 } };
		for (size_t i = 0; i < mesh.vertices.size(); ++i)
		{
			mesh.vertices[i].set(positions[i], Float2{ 0, 0 }, Float4{ 1, 1, 1, 1 });
		}
		// Degenerate middle triangles keep the GPU workload small while preserving the count.
		mesh.indices.fill(TriangleIndex{ 0, 0, 0 });
		mesh.indices.front() = TriangleIndex{ 0, 1, 2 };
		mesh.indices.back() = TriangleIndex{ 3, 4, 5 };
		REQUIRE(mesh.validate());
		// D3D11 rejects a request larger than its single index buffer in this stage.
		// Metal can accept it within its growable buffer's existing capacity limit.
		bool accepted = true;
		# if SIV3D_PLATFORM(WINDOWS)
			accepted = (triangleCount <= 87381);
		# endif
		const Image image = CaptureIndexedDraw([&]
		{
			RectF{ 240, 20, 20, 20 }.draw(Palette::Green);
			mesh.draw();
			RectF{ 240, 60, 20, 20 }.draw(Palette::Blue);
		}, static_cast<int64>((accepted ? triangleCount : 0) + 4));
		CHECK(image[28][28] == (accepted ? Palette::White : Palette::Black));
		CHECK(image[28][128] == (accepted ? Palette::White : Palette::Black));
		CHECK(image[28][248] == Palette::Green);
		CHECK(image[68][248] == Palette::Blue);
	}
}

TEST_CASE("Renderer2D.polygon_index_count")
{
	const Mesh2D mesh = Mesh2D::Grid(RectF{ 20, 20, 200, 160 }, Size{ 128, 128 });
	Array<Float2> vertices;
	vertices.reserve(mesh.vertices.size());
	for (const auto& vertex : mesh.vertices)
	{
		vertices.push_back(vertex.pos);
	}
	const Polygon polygon{ { Vec2{ 20, 20 }, Vec2{ 220, 20 }, Vec2{ 220, 180 }, Vec2{ 20, 180 } },
		Array<Array<Vec2>>{}, std::move(vertices), mesh.indices, RectF{ 20, 20, 200, 160 }, SkipValidation::Yes };
	const Pattern::PolkaDot whitePattern{ .primary = Palette::White, .background = Palette::White };
	const int64 triangleCount = static_cast<int64>(mesh.indices.size());
	CheckGridPixels(CaptureIndexedDraw([&] { polygon.draw(); }, triangleCount), Palette::White);
	CheckGridPixels(CaptureIndexedDraw([&] { polygon.draw(Vec2{ 0, 0 }, whitePattern); }, triangleCount), Palette::White);
	CheckGridPixels(CaptureIndexedDraw([&] { polygon.drawTransformed(0.0, Vec2{ 0, 0 }); }, triangleCount), Palette::White);
	CheckGridPixels(CaptureIndexedDraw([&] { polygon.drawTransformed(0.0, Vec2{ 0, 0 }, whitePattern); }, triangleCount), Palette::White);
}
