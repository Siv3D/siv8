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
			CHECK((actual.image == reference.image));
			CHECK(actual.triangleCount == (reference.triangleCount + rectangleCount * 2));
		}
	}
}

TEST_CASE("Renderer2D.index_batch_boundary")
{
	const Texture red{ Image{ 2, 2, Palette::Red } };
	REQUIRE(red);
	for (const Shape shape : { Shape::Line, Shape::Arrow, Shape::Arc })
	{
		INFO(static_cast<int32>(shape));
		const auto reference = CaptureBatchDraw([&] { DrawScene(red, shape); });
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
				DrawScene(red, shape);
			});
			CHECK((actual.image == reference.image));
			CHECK(actual.triangleCount == (reference.triangleCount + static_cast<int64>(triangleCount)));
		}
	}
}

TEST_CASE("Renderer2D.line_cap_combinations")
{
	for (const LineCap startCap : { LineCap::Flat, LineCap::Square, LineCap::Round })
	{
		for (const LineCap endCap : { LineCap::Flat, LineCap::Square, LineCap::Round })
		{
			INFO(static_cast<int32>(startCap));
			INFO(static_cast<int32>(endCap));
			const auto draw = [&]
			{
				Line{ 100, 60, 300, 60 }.draw(startCap, endCap, 20);
				Line{ 100, 120, 300, 120 }.draw(startCap, endCap, 20, Palette::Red, Palette::Blue);
			};
			const auto reference = CaptureBatchDraw(draw);
			REQUIRE(reference.image.width() >= 320);
			REQUIRE(reference.image.height() >= 140);
			CHECK(reference.image[60][200] == Palette::White);
			CHECK(reference.image[60][95] == ((startCap == LineCap::Flat) ? Palette::Black : Palette::White));
			CHECK(reference.image[60][305] == ((endCap == LineCap::Flat) ? Palette::Black : Palette::White));
			if (startCap == LineCap::Round)
			{
				CHECK(reference.image[120][95] == Palette::Red);
			}
			if (endCap == LineCap::Round)
			{
				CHECK(reference.image[120][305] == Palette::Blue);
			}

			// Seven vertices remain: a line body fits, but a body with a round cap does not.
			constexpr int32 rectangleCount = 16382;
			const auto actual = CaptureBatchDraw([&]
			{
				for (int32 i = 0; i < rectangleCount; ++i)
				{
					RectF{ -10, -10, 1, 1 }.draw();
				}
				draw();
			});
			CHECK((actual.image == reference.image));
			CHECK(actual.triangleCount == (reference.triangleCount + rectangleCount * 2));
		}
	}
}

TEST_CASE("Renderer2D.round_cap_allocation_failure")
{
	const auto reference = CaptureBatchDraw([]
	{
		RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
		RectF{ 60, 20, 20, 20 }.draw(Palette::Blue);
	});
	Mesh2D filler{ 3, 87381 };
	for (auto& vertex : filler.vertices)
	{
		vertex.set(Float2{ -10, -10 }, Float2{ 0, 0 }, Float4{ 1, 1, 1, 1 });
	}
	filler.indices.fill(TriangleIndex{ 0, 1, 2 });
	REQUIRE(filler.validate());
	Mesh2D lastFiller = filler;
	for (const auto& [shape, angle] : { std::pair{ Shape::Line, 0.0 }, std::pair{ Shape::Arrow, 0.0 },
		std::pair{ Shape::Arc, 180_deg }, std::pair{ Shape::Arc, 0.0 } })
	{
		INFO(static_cast<int32>(shape));
		INFO(angle);
		const size_t lastTriangleCount = ((shape == Shape::Arrow) ? 87379 : 87368);
		lastFiller.indices.resize(lastTriangleCount, TriangleIndex{ 0, 1, 2 });
		const auto actual = CaptureBatchDraw([&]
		{
			RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
			for (int32 i = 0; i < 15; ++i)
			{
				filler.draw();
			}
			lastFiller.draw();
			// Leave 49 indices for the line/arc, or 16 for the arrow. A body or cap
			// alone would fit, but the whole shape must fail without reserving any part.
			if (shape == Shape::Line)
			{
				Line{ 100, 100, 300, 100 }.draw(LineCap::Round, LineCap::Round, 20);
			}
			else if (shape == Shape::Arrow)
			{
				Line{ 100, 100, 300, 100 }.drawArrow(LineCap::Round, 20, SizeF{ 40, 40 }, Palette::Red, Palette::Blue);
			}
			else
			{
				Circle{ 200, 100, 60 }.drawArc(LineCap::Round, 0.0, angle, 10, 10, Arg::start(Palette::Red), Arg::end(Palette::Blue));
			}
			// A later, smaller draw must still succeed at the correct index position.
			RectF{ 60, 20, 20, 20 }.draw(Palette::Blue);
		});
		CHECK((actual.image == reference.image));
		CHECK(actual.triangleCount == (4 + 15 * 87381 + static_cast<int64>(lastTriangleCount)));
	}
}

TEST_CASE("Renderer2D.linestring_allocation_failure")
{
	const auto reference = CaptureBatchDraw([]
	{
		RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
		RectF{ 60, 20, 20, 20 }.draw(Palette::Blue);
	});
	Mesh2D filler{ 3, 87381 };
	for (auto& vertex : filler.vertices)
	{
		vertex.set(Float2{ -10, -10 }, Float2{ 0, 0 }, Float4{ 1, 1, 1, 1 });
	}
	filler.indices.fill(TriangleIndex{ 0, 1, 2 });
	REQUIRE(filler.validate());
	Mesh2D lastFiller = filler;
	for (const int32 shape : { 0, 1, 2 })
	{
		INFO(shape);
		const LineString line = ((shape == 0) ? LineString{ Vec2{ 100, 100 }, Vec2{ 200, 100 }, Vec2{ 300, 100 } }
			: LineString((shape == 1) ? 1 : 3, Vec2{ 200, 100 }));
		const Array<ColorF> colors(line.size(), Palette::Green);
		for (const LineCap cap : { LineCap::Square, LineCap::Round })
		{
			INFO(static_cast<int32>(cap));
			// Leave 49 indices for round caps, or 16 (body) / 10 (caps only) for square caps.
			// Some parts would fit independently, but the whole shape must be rejected.
			const size_t lastTriangleCount = ((cap == LineCap::Round) ? 87368 : ((shape == 0) ? 87379 : 87381));
			lastFiller.indices.resize(lastTriangleCount, TriangleIndex{ 0, 1, 2 });
			for (const int32 coloring : { 0, 1, 2, 3 })
			{
				INFO(coloring);
				const auto actual = CaptureBatchDraw([&]
				{
					RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
					for (int32 i = 0; i < 15; ++i)
					{
						filler.draw();
					}
					lastFiller.draw();
					switch (coloring)
					{
					case 0: line.draw(cap, 20, Palette::Green); break;
					case 1: line.draw(cap, 20, Palette::Red, Palette::Blue); break;
					case 2: line.draw(cap, 20, colors); break;
					case 3: line.draw(cap, 20, Pattern::PolkaDot{ .primary = Palette::Green, .background = Palette::Green }); break;
					}
					RectF{ 60, 20, 20, 20 }.draw(Palette::Blue);
				});
				CHECK((actual.image == reference.image));
				CHECK(actual.triangleCount == (4 + 15 * 87381 + static_cast<int64>(lastTriangleCount)));
			}
		}
	}
}

TEST_CASE("Renderer2D.round_arc_directions")
{
	for (const double angle : { 0.0, 180_deg, -180_deg, 360_deg, -360_deg })
	{
		INFO(angle);
		for (const int32 coloring : { 0, 1, 2, 3 })
		{
			INFO(coloring);
			const auto draw = [&]
			{
				const Circle circle{ 200, 100, 60 };
				// Translucent colors cover cap/body overlap, including full-circle endpoints.
				const ColorF red{ 1.0, 0.0, 0.0, 0.5 }, blue{ 0.0, 0.0, 1.0, 0.5 };
				switch (coloring)
				{
				case 0: circle.drawArc(LineCap::Round, 0.0, angle, 10, 10, ColorF{ 1.0, 0.5 }); break;
				case 1: circle.drawArc(LineCap::Round, 0.0, angle, 10, 10, red, blue); break;
				case 2: circle.drawArc(LineCap::Round, 0.0, angle, 10, 10, Arg::start(red), Arg::end(blue)); break;
				case 3: circle.drawArc(LineCap::Round, 0.0, angle, 10, 10, Pattern::PolkaDot{ .primary = ColorF{ 1.0, 0.5 }, .background = ColorF{ 1.0, 0.5 } }); break;
				}
			};
			const auto reference = CaptureBatchDraw(draw);
			REQUIRE(reference.image.width() >= 320);
			REQUIRE(reference.image.height() >= 180);
			CHECK(reference.image[40][200] != Palette::Black);
			if (angle == 0.0)
			{
				CHECK(reference.triangleCount == 18);
			}
			else if ((coloring == 2) && (Abs(angle) == 180_deg))
			{
				CHECK(reference.image[40][200].r > reference.image[40][200].b);
				CHECK(reference.image[160][200].r < reference.image[160][200].b);
			}

			// Seven vertices remain: the first cap cannot fit in the current batch.
			constexpr int32 rectangleCount = 16382;
			const auto actual = CaptureBatchDraw([&]
			{
				for (int32 i = 0; i < rectangleCount; ++i)
				{
					RectF{ -10, -10, 1, 1 }.draw();
				}
				draw();
			});
			CHECK((actual.image == reference.image));
			CHECK(actual.triangleCount == (reference.triangleCount + rectangleCount * 2));
		}
	}
}

# endif
