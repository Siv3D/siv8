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
	enum class Coloring { Solid, Gradient, PerPoint, Pattern };

	LineString MakeLongLine(const size_t pointCount)
	{
		// Only the two vertical ends are visible. Extra points extend an off-screen
		// horizontal section, without simplification or sharp-corner splitting.
		LineString line{ Arg::reserve = pointCount };
		line << Vec2{ 100, 100 } << Vec2{ 100, -100 };
		for (size_t i = 0; i < (pointCount - 6); ++i)
		{
			line << Vec2{ (101.0 + i), -100 };
		}
		const double cornerX = (pointCount + 95.0);
		line << Vec2{ cornerX, -100 } << Vec2{ cornerX, -200 }
			<< Vec2{ 300, -200 } << Vec2{ 300, 100 };
		return line;
	}

	void DrawLine(const LineString& line, const Coloring coloring, const LineCap startCap, const LineCap endCap)
	{
		switch (coloring)
		{
		case Coloring::Solid:
			line.draw(startCap, endCap, 20, Palette::Green);
			break;
		case Coloring::Gradient:
			line.draw(startCap, endCap, 20, Palette::Green, Palette::Green);
			break;
		case Coloring::PerPoint:
			line.draw(startCap, endCap, 20, Array<ColorF>(line.size(), Palette::Green));
			break;
		case Coloring::Pattern:
			line.draw(startCap, endCap, 20, Pattern::PolkaDot{ .primary = Palette::Green, .background = Palette::Green });
			break;
		}
	}

	struct CapturedDraw
	{
		Image image;
		int64 triangleCount;
	};

	template <class Draw>
	CapturedDraw CaptureLine(Draw&& draw)
	{
		const ColorF background = Scene::GetBackground();
		const ScopeExit restoreBackground{ [&] { Scene::SetBackground(background); } };
		Scene::SetBackground(Palette::Black);
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const ScopedColorMul2D colorMul{ Palette::White };
		const ScopedColorAdd2D colorAdd{ 0.0 };
		REQUIRE(System::Update());
		RectF{ 20, 140, 20, 20 }.draw(Palette::Red);
		draw();
		RectF{ 60, 140, 20, 20 }.draw(Palette::Blue);
		ScreenCapture::RequestCurrentFrame();
		REQUIRE(System::Update());
		REQUIRE(ScreenCapture::HasNewFrame());
		REQUIRE(ScreenCapture::GetFrame().width() >= 320);
		REQUIRE(ScreenCapture::GetFrame().height() >= 180);
		return { ScreenCapture::GetFrame(), Profiler::GetFrameMetrics().triangleCount };
	}
}

TEST_CASE("LineString.index_count_with_caps")
{
	const LineString shortLine = MakeLongLine(256);
	// Equal colors make all overloads produce comparable images despite differing
	// off-screen path lengths; this test targets counts and subsequent draw alignment.
	for (const Coloring coloring : { Coloring::Solid, Coloring::Gradient, Coloring::PerPoint, Coloring::Pattern })
	{
		INFO(static_cast<int32>(coloring));
		for (const LineCap cap : { LineCap::Flat, LineCap::Square, LineCap::Round })
		{
			INFO(static_cast<int32>(cap));
			const auto reference = CaptureLine([&] { DrawLine(shortLine, coloring, cap, cap); });
			CHECK(reference.image[50][100] == Palette::Green);
			CHECK(reference.image[50][300] == Palette::Green);
			CHECK(reference.image[150][30] == Palette::Red);
			CHECK(reference.image[150][70] == Palette::Blue);
			const int64 capTriangles = (reference.triangleCount - 4 - 2 * (256 - 1));
			CHECK(capTriangles == ((cap == LineCap::Flat) ? 0 : ((cap == LineCap::Square) ? 4 : 18)));
			for (const size_t pointCount : { 10914u, 10915u, 10922u })
			{
				INFO(pointCount);
				const LineString line = MakeLongLine(pointCount);
				const auto actual = CaptureLine([&] { DrawLine(line, coloring, cap, cap); });
				CHECK((actual.image == reference.image));
				CHECK(actual.triangleCount == (4 + 2 * static_cast<int64>(pointCount - 1) + capTriangles));
			}
		}

		// One round cap produces exactly 65,535 indices at 10,919 points.
		const auto reference = CaptureLine([&] { DrawLine(shortLine, coloring, LineCap::Round, LineCap::Flat); });
		for (const size_t pointCount : { 10919u, 10920u })
		{
			INFO(pointCount);
			const LineString line = MakeLongLine(pointCount);
			const auto actual = CaptureLine([&] { DrawLine(line, coloring, LineCap::Round, LineCap::Flat); });
			CHECK((actual.image == reference.image));
			CHECK(actual.triangleCount == (4 + 2 * static_cast<int64>(pointCount - 1) + 9));
		}
	}
}

TEST_CASE("LineString.rejected_body_has_no_caps")
{
	const auto reference = CaptureLine([] {});
	for (const Coloring coloring : { Coloring::Solid, Coloring::Gradient, Coloring::PerPoint, Coloring::Pattern })
	{
		INFO(static_cast<int32>(coloring));
		// 10,923 exceeds the prepared strip's existing limit, while 32,760 is
		// rejected before preparation. Neither should leave visible end caps.
		for (const size_t pointCount : { 10923u, 32760u })
		{
			INFO(pointCount);
			const LineString line = MakeLongLine(pointCount);
			for (const LineCap cap : { LineCap::Square, LineCap::Round })
			{
				INFO(static_cast<int32>(cap));
				const auto actual = CaptureLine([&] { DrawLine(line, coloring, cap, cap); });
				CHECK((actual.image == reference.image));
				CHECK(actual.triangleCount == 4);
			}
		}

		// A single point (including one obtained by simplification) still has caps.
		const auto single = CaptureLine([&] { DrawLine(LineString{ Vec2{ 100, 100 } }, coloring, LineCap::Round, LineCap::Round); });
		CHECK(single.image[100][100] == Palette::Green);
		CHECK(single.triangleCount == 22);
		const auto collapsed = CaptureLine([&] { DrawLine(LineString(3, Vec2{ 100, 100 }), coloring, LineCap::Round, LineCap::Round); });
		CHECK((collapsed.image == single.image));
		CHECK(collapsed.triangleCount == single.triangleCount);
		const auto empty = CaptureLine([&] { DrawLine(LineString{}, coloring, LineCap::Round, LineCap::Round); });
		CHECK((empty.image == reference.image));
		CHECK(empty.triangleCount == 4);
	}
}

TEST_CASE("LineString.cap_combinations")
{
	for (const Coloring coloring : { Coloring::Solid, Coloring::Gradient, Coloring::PerPoint, Coloring::Pattern })
	{
		INFO(static_cast<int32>(coloring));
		for (const bool singlePoint : { false, true })
		{
			INFO(singlePoint);
			const LineString line = (singlePoint ? LineString{ Vec2{ 200, 100 } }
				: LineString{ Vec2{ 100, 100 }, Vec2{ 200, 100 }, Vec2{ 300, 100 } });
			for (const LineCap startCap : { LineCap::Flat, LineCap::Square, LineCap::Round })
			{
				for (const LineCap endCap : { LineCap::Flat, LineCap::Square, LineCap::Round })
				{
					INFO(static_cast<int32>(startCap));
					INFO(static_cast<int32>(endCap));
					const auto actual = CaptureLine([&] { DrawLine(line, coloring, startCap, endCap); });
					const int32 startX = (singlePoint ? 200 : 100);
					const int32 endX = (singlePoint ? 200 : 300);
					CHECK(actual.image[100][startX - 5] == ((startCap == LineCap::Flat) ? Palette::Black : Palette::Green));
					CHECK(actual.image[100][endX + 5] == ((endCap == LineCap::Flat) ? Palette::Black : Palette::Green));
					// The outer corners distinguish square caps from round caps.
					CHECK(actual.image[91][startX - 9] == ((startCap == LineCap::Square) ? Palette::Green : Palette::Black));
					CHECK(actual.image[91][endX + 9] == ((endCap == LineCap::Square) ? Palette::Green : Palette::Black));
					const auto capTriangles = [](const LineCap cap) { return ((cap == LineCap::Square) ? 2 : ((cap == LineCap::Round) ? 9 : 0)); };
					CHECK(actual.triangleCount == (4 + (singlePoint ? 0 : 4) + capTriangles(startCap) + capTriangles(endCap)));
				}
			}
		}
	}
}

TEST_CASE("LineString.cap_gradient_direction")
{
	for (const LineCap cap : { LineCap::Square, LineCap::Round })
	{
		INFO(static_cast<int32>(cap));
		// A collapsed colored line uses the original endpoint colors for its caps.
		const auto gradient = CaptureLine([&]
		{
			LineString{ Vec2{ 200, 100 } }.draw(cap, 20, Palette::Red, Palette::Blue);
		});
		const auto perPoint = CaptureLine([&]
		{
			LineString(3, Vec2{ 200, 100 }).draw(cap, 20, Array<ColorF>{ Palette::Red, Palette::Green, Palette::Blue });
		});
		CHECK((gradient.image == perPoint.image));
		CHECK(gradient.image[100][195].r > gradient.image[100][195].b);
		CHECK(gradient.image[100][205].r < gradient.image[100][205].b);
		const auto normal = CaptureLine([&]
		{
			LineString{ Vec2{ 100, 100 }, Vec2{ 200, 60 }, Vec2{ 300, 100 } }.draw(cap, 20, Palette::Red, Palette::Blue);
		});
		CHECK(normal.image[100][95].r > normal.image[100][95].b);
		CHECK(normal.image[100][305].r < normal.image[100][305].b);
	}
}
