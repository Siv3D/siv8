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
	Image CaptureEllipsePie(Draw&& draw)
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
		REQUIRE(image.width() >= 480);
		REQUIRE(image.height() >= 280);
		return image;
	}
}

TEST_CASE("Renderer2D.ellipse_pie_full_sweeps")
{
	const ColorF inner{ 0.9, 0.2, 0.1, 0.5 }, outer{ 0.1, 0.3, 0.9, 0.5 };
	const Pattern::PolkaDot pattern{ .primary = inner, .background = outer };
	for (const int32 coloring : { 0, 1, 2 })
	{
		INFO(coloring);
		const auto draw = [&](const bool pie)
		{
			int32 i = 0;
			for (const double start : { 0.0, 0.31, -2.4 })
			{
				// Include sweeps just below one revolution that round to a full float sweep.
				for (const double sweep : { Math::TwoPi, -Math::TwoPi, 2 * Math::TwoPi, -2 * Math::TwoPi,
					(Math::TwoPi + 0.2), (-Math::TwoPi - 0.2),
					(Math::TwoPi - 1e-8), (-Math::TwoPi + 1e-8) })
				{
					const Ellipse ellipse{ 40 + (i % 6) * 80, 35 + (i / 6) * 64, (i % 2 ? -32 : 32), 24 };
					if (pie)
					{
						switch (coloring)
						{
						case 0: CHECK(&ellipse.drawPie(start, sweep, inner) == &ellipse); break;
						case 1: CHECK(&ellipse.drawPie(start, sweep, inner, outer) == &ellipse); break;
						case 2: CHECK(&ellipse.drawPie(start, sweep, pattern) == &ellipse); break;
						}
					}
					else
					{
						switch (coloring)
						{
						case 0: ellipse.draw(inner); break;
						case 1: ellipse.draw(inner, outer); break;
						case 2: ellipse.draw(pattern); break;
						}
					}
					++i;
				}
			}
		};
		const Image expected = CaptureEllipsePie([&] { draw(false); });
		CHECK(expected[35][40] != Palette::Black);
		const Image actual = CaptureEllipsePie([&] { draw(true); });
		CHECK((actual == expected));
	}
}

TEST_CASE("Renderer2D.ellipse_pie_partial_and_empty_sweeps")
{
	for (const double sweep : { 0.0, Math::HalfPi, -Math::HalfPi, (Math::TwoPi - 0.2) })
	{
		INFO(sweep);
		const Image actual = CaptureEllipsePie([&] { Ellipse{ 140, 120, 80, 40 }.drawPie(0, sweep); });
		CHECK((actual[110][160] == Palette::White) == (0.0 < sweep));
		CHECK((actual[110][120] == Palette::White) == ((sweep < 0.0) || (Math::Pi < sweep)));
		CHECK((actual[130][160] == Palette::White) == (Math::Pi < sweep));
		CHECK(actual[170][140] == Palette::Black);
	}
}
