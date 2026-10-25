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
	Image CaptureCircleFrame(Draw&& draw, const int64 expectedTriangles)
	{
		const ColorF previousBackground = Scene::GetBackground();
		const ScopeExit restoreBackground{ [&] { Scene::SetBackground(previousBackground); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedColorMul2D colorMul{ Palette::White };
		const ScopedColorAdd2D colorAdd{ 0.0 };

		REQUIRE(System::Update());
		RectF{ 20, 20, 20, 20 }.draw(Palette::Red);
		draw();
		RectF{ 250, 20, 20, 20 }.draw(Palette::Blue);
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		CHECK(Profiler::GetFrameMetrics().triangleCount == (expectedTriangles + 4));
		Image image = ScreenCapture::GetFrame();
		REQUIRE(image.width() >= 280);
		REQUIRE(image.height() >= 220);
		CHECK(image[30][30] == Palette::Red);
		CHECK(image[30][260] == Palette::Blue);
		CHECK(image[120][140] == Palette::Black);
		return image;
	}

	void DrawCircleFrame(const CircularDashStyle& style, const int32 coloring)
	{
		const Circle circle{ 140, 120, 80 };
		switch (coloring)
		{
		case 0: circle.drawDashedFrame(20, style); break;
		case 1: circle.drawDashedFrame(20, style, Palette::Red, Palette::Blue); break;
		case 2: circle.drawDashedFrame(10, 10, style); break;
		case 3: circle.drawDashedFrame(10, 10, style, Palette::Red, Palette::Blue); break;
		}
	}

	void CheckCircleVisible(const Image& image, const bool visible)
	{
		// 密な破線は 1 本が 1 ピクセル未満になるため、円周の各方向で画素の有無を調べる。
		for (const Point corner : { Point{ 130, 30 }, Point{ 130, 190 }, Point{ 50, 110 }, Point{ 210, 110 } })
		{
			bool found = false;
			for (int32 y = corner.y; y < (corner.y + 20); ++y)
			{
				for (int32 x = corner.x; x < (corner.x + 20); ++x)
				{
					found |= (image[y][x] != Palette::Black);
				}
			}
			CHECK(found == visible);
		}
	}

	void DrawReferenceFrame(const uint32 count, const bool gradient)
	{
		const ColorF inner = (gradient ? Palette::Red : Palette::White);
		const ColorF outer = (gradient ? Palette::Blue : Palette::White);
		const auto point = [](const double angle, const double radius)
		{
			return (Vec2{ 140, 120 } + Vec2{ std::sin(angle), -std::cos(angle) } * radius);
		};
		for (uint32 i = 0; i < count; ++i)
		{
			for (int32 j = 0; j < 16; ++j)
			{
				const double start = (Math::TwoPi * (i + j / 32.0) / count);
				const double end = (Math::TwoPi * (i + (j + 1) / 32.0) / count);
				Quad{ point(start, 70), point(start, 90), point(end, 90), point(end, 70) }.draw(inner, outer, outer, inner);
			}
		}
	}
}

TEST_CASE("Renderer2D.circle_dashed_frame_styles")
{
	for (const int32 coloring : { 0, 1, 2, 3 })
	{
		INFO(coloring);
		for (const uint32 dashCount : { 0u, 12u })
		{
			INFO(dashCount);
			for (const double ratio : { 0.0, 0.5, 1.0 })
			{
				INFO(ratio);
				const bool visible = (dashCount && (0.0 < ratio));
				// 半径 90 の外周では、12 本の破線は各 6 三角形、実線は 120 三角形。
				const int64 triangles = (visible ? ((ratio == 1.0) ? 120 : 72) : 0);
				const Image image = CaptureCircleFrame([&]
				{
					DrawCircleFrame({ .dashCount = dashCount, .dashRatio = ratio, .startAngle = -7.5_deg }, coloring);
				}, triangles);
				CheckCircleVisible(image, visible);
				if (visible)
				{
					if ((coloring == 1) || (coloring == 3))
					{
						CHECK(image[46][140].r > image[46][140].b);
						CHECK(image[34][140].b > image[34][140].r);
					}
					else
					{
						CHECK(image[40][140] == Palette::White);
					}
				}
			}
		}
	}
}

TEST_CASE("Renderer2D.circle_dashed_frame_short_arcs")
{
	for (const int32 coloring : { 0, 1, 2, 3 })
	{
		INFO(coloring);
		// 外周半径 90 の通常円は 60 分割。破線比率 0.5 では 30 本から各区間がその角度以下になる。
		for (const uint32 dashCount : { 29u, 30u, 31u, 64u, 256u })
		{
			INFO(dashCount);
			const Image reference = CaptureCircleFrame([&]
			{
				DrawReferenceFrame(dashCount, ((coloring == 1) || (coloring == 3)));
			}, (dashCount * 32));
			const Image actual = CaptureCircleFrame([&]
			{
				DrawCircleFrame({ .dashCount = dashCount }, coloring);
			}, (dashCount * ((dashCount < 30) ? 4 : 2)));
			CheckCircleVisible(actual, true);

			// 各破線を 16 区間の Quad で描いた画像を基準にする。分割位置が変わるため画素の完全一致は要求せず、
			// 円を含む領域の RGB 絶対差の合計を、基準画像の RGB 合計の 2% 以下に制限する。
			uint64 difference = 0, intensity = 0;
			for (int32 y = 20; y < 220; ++y)
			{
				for (int32 x = 40; x < 240; ++x)
				{
					const Color a = actual[y][x], r = reference[y][x];
					difference += (Abs(int32{ a.r } - r.r) + Abs(int32{ a.g } - r.g) + Abs(int32{ a.b } - r.b));
					intensity += (uint32{ r.r } + r.g + r.b);
				}
			}
			REQUIRE(intensity > 0);
			CHECK(difference <= (intensity / 50));
		}
	}
}
