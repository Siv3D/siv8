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

// These counts target D3D11's fixed vertex and index buffer capacities.
# if SIV3D_PLATFORM(WINDOWS)

namespace
{
	struct CapturedDraw
	{
		Image image;
		int64 triangleCount;
	};

	template <class Draw>
	CapturedDraw CaptureBatchDraw(Draw&& draw)
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
		return { ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}

	enum class Shape
	{
		Line,
		LineString,
		Arc,
		Arrow,
	};

	void DrawScene(const Texture& texture, const Shape shape)
	{
		texture.resized(40, 40).draw(20, 20);
		{
			const ScopedColorMul2D colorMul{ Palette::Green };
			const Transformer2D transform{ Mat3x2::Translate(0, 20) };
			switch (shape)
			{
			case Shape::Line:
				Line{ 100, 100, 300, 100 }.draw(LineCap::Round, LineCap::Round, 20);
				break;
			case Shape::LineString:
				LineString{ Vec2{ 100, 100 }, Vec2{ 200, 100 }, Vec2{ 300, 100 } }.draw(LineCap::Round, 20);
				break;
			case Shape::Arc:
				Circle{ 200, 100, 60 }.drawArc(LineCap::Round, 0.0, 180_deg, 10, 10);
				break;
			case Shape::Arrow:
				Line{ 100, 100, 300, 100 }.drawArrow(LineCap::Round, 20, SizeF{ 40, 40 });
				break;
			}
		}
		RectF{ 180, 115, 40, 10 }.draw(Palette::Blue);
		texture.resized(40, 40).draw(260, 20);
	}
}

TEST_CASE("Renderer2D.vertex_batch_boundary")
{
	const Texture red{ Image{ 2, 2, Palette::Red } };
	REQUIRE(red);
	for (const Shape shape : { Shape::Line, Shape::LineString, Shape::Arc, Shape::Arrow })
	{
		INFO(static_cast<int32>(shape));
		const auto reference = CaptureBatchDraw([&] { DrawScene(red, shape); });
		REQUIRE(reference.image.width() >= 320);
		REQUIRE(reference.image.height() >= 200);
		CHECK(reference.image[30][30] == Palette::Red);
		CHECK(reference.image[120][200] == Palette::Blue);
		if (shape == Shape::Line)
		{
			CHECK(reference.image[120][95] == Palette::Green);
			CHECK(reference.image[120][150] == Palette::Green);
			CHECK(reference.image[120][305] == Palette::Green);
		}

		// The first texture adds four vertices before the shape. Cover a whole shape
		// fitting, a split between its parts, a split before it, and a later batch.
		for (const int32 rectangleCount : { 16376, 16377, 16381, 16382, 32764 })
		{
			INFO(rectangleCount);
			const auto actual = CaptureBatchDraw([&]
			{
				for (int32 i = 0; i < rectangleCount; ++i)
				{
					RectF{ -10, -10, 1, 1 }.draw();
				}
				DrawScene(red, shape);
			});
			CHECK(actual.image == reference.image);
			CHECK(actual.triangleCount == (reference.triangleCount + rectangleCount * 2));
		}
	}
}

TEST_CASE("Renderer2D.index_batch_boundary")
{
	const Texture red{ Image{ 2, 2, Palette::Red } };
	REQUIRE(red);
	const auto reference = CaptureBatchDraw([&] { DrawScene(red, Shape::Line); });
	for (const size_t triangleCount : { 87350u, 87368u, 87377u, 87378u, 87379u, 87381u })
	{
		INFO(triangleCount);
		// Degenerate off-screen triangles fill the index buffer with only three vertices.
		Mesh2D filler{ 3, triangleCount };
		for (auto& vertex : filler.vertices)
		{
			vertex.set(Float2{ -10, -10 }, Float2{ 0, 0 }, Float4{ 1, 1, 1, 1 });
		}
		filler.indices.fill(TriangleIndex{ 0, 1, 2 });
		REQUIRE(filler.validate());
		const auto actual = CaptureBatchDraw([&]
		{
			filler.draw();
			DrawScene(red, Shape::Line);
		});
		CHECK(actual.image == reference.image);
		CHECK(actual.triangleCount == (reference.triangleCount + static_cast<int64>(triangleCount)));
	}
}

# endif
