//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "ImageDrawingTestHelper.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <random>

using namespace s3d::ImageDrawingTest;

namespace
{
	const Array<Array<Vec2>> Contours{
		{ { 5.25, 4.75 }, { 29.25, 8.75 }, { 12.25, 29.75 } },
		{ { -9.25, 14.75 }, { 40.25, 15.75 }, { 16.25, 16.75 } },
		{ { 5.25, 4.75 }, { 27.25, 6.75 }, { 29.25, 26.75 }, { 4.25, 28.75 } },
		{ { 3.25, 3.75 }, { 29.25, 4.75 }, { 13.25, 13.75 }, { 8.25, 28.75 } },
		{ { 4.25, 4.75 }, { 28.25, 28.75 }, { 4.25, 28.75 }, { 28.25, 4.75 } },
		{ { 4.25, 4.75 }, { 28.25, 4.75 }, { 28.25, 4.75 }, { 12.25, 28.75 } },
		{ { 4.25, 4.75 }, { 16.25, 4.75 }, { 28.25, 4.75 }, { 12.25, 28.75 } },
	};

	void Draw(const Array<Vec2>& p, Image& image, double inner, double outer, Color color,
		EnableAntialiasing aa, bool paint = false, JoinStyle style = JoinStyle::Round)
	{
		const auto draw = [&](const auto& shape)
		{
			if (paint) CHECK(&shape.paintFrame(image, style, inner, outer, color, aa) == &shape);
			else CHECK(&shape.overwriteFrame(image, style, inner, outer, color, aa) == &shape);
		};
		if (p.size() == 3) draw(Triangle{ p[0], p[1], p[2] });
		else draw(Quad{ p[0], p[1], p[2], p[3] });
	}

	// Pointwise ray casting and perpendicular/endpoint distances, independent of
	// the rasterizer's scanline crossings, projection coefficients, and spans.
	uint8 Reference(const Array<Vec2>& points, Vec2 p, double inner, double outer, bool aa)
	{
		bool inside = false;
		double distance = std::numeric_limits<double>::infinity();
		for (size_t i = 0; i < points.size(); ++i)
		{
			const Vec2 a = points[i], b = points[(i + 1) % points.size()], d = b - a;
			if (((a.y > p.y) != (b.y > p.y)) && (p.x < a.x + (p.y - a.y) * d.x / d.y)) inside = !inside;
			const double length = std::hypot(d.x, d.y);
			double candidate;
			if ((length == 0) || ((p - a).dot(d) <= 0)) candidate = p.distanceFrom(a);
			else if ((p - b).dot(d) >= 0) candidate = p.distanceFrom(b);
			else candidate = Abs(d.cross(p - a)) / length;
			distance = Min(distance, candidate);
		}
		if (!aa) return (distance <= (inside ? inner : outer)) ? 255 : 0;
		const double sd = inside ? -distance : distance;
		const double covered = Clamp(outer - sd + 0.5, 0.0, 1.0) - Clamp(-inner - sd + 0.5, 0.0, 1.0);
		return static_cast<uint8>(std::lround(covered * 255));
	}
}

TEST_CASE("ImagePolygonFrame.DistanceAndWinding")
{
	for (size_t index = 0; index < Contours.size(); ++index)
		for (const Vec2 widths : { Vec2{ 0, 4.25 }, Vec2{ 4.25, 0 }, Vec2{ 0.15, 0.35 },
			Vec2{ 2.25, 2.25 }, Vec2{ 9.25, 3.25 }, Vec2{ 100, 0 }, Vec2{ 100, 12.25 } })
			for (const bool aa : { false, true })
			{
				CAPTURE(index, widths, aa);
				Image actual{ 32, 32, Color{ 0, 0 } };
				Draw(Contours[index], actual, widths.x, widths.y, Palette::White, EnableAntialiasing{ aa });
				int32 maxError = 0;
				for (int32 y = 0; y < 32; ++y)
					for (int32 x = 0; x < 32; ++x)
						maxError = Max(maxError, Abs(static_cast<int32>(actual[y][x].a)
							- Reference(Contours[index], { x + 0.5, y + 0.5 }, widths.x, widths.y, aa)));
				CHECK(maxError <= (aa ? 1 : 0));
				Image reversed{ 32, 32, Color{ 0, 0 } };
				Draw(Contours[index].reversed(), reversed, widths.x, widths.y, Palette::White, EnableAntialiasing{ aa });
				CHECK_THAT(actual, ImageEquals(reversed));
			}
}

TEST_CASE("ImagePolygonFrame.RandomDistanceAndNarrowImages")
{
	std::mt19937 random{ 20261014 };
	std::uniform_real_distribution<double> coordinate{ -16, 48 }, thickness{ 0, 16 };
	for (int32 sample = 0; sample < 100; ++sample)
	{
		Array<Vec2> points;
		for (int32 i = 0; i < (sample % 2 ? 3 : 4); ++i) points.emplace_back(coordinate(random), coordinate(random));
		const double inner = (sample % 3 ? thickness(random) : 0), outer = (sample % 3 == 1 ? 0 : thickness(random));
		for (const bool aa : { false, true })
			for (const Size size : { Size{ 32, 32 }, Size{ 1, 32 }, Size{ 32, 1 } })
			{
				CAPTURE(sample, inner, outer, aa, size);
				Image actual{ size, Color{ 0, 0 } };
				Draw(points, actual, inner, outer, Palette::White, EnableAntialiasing{ aa });
				int32 maxError = 0;
				for (int32 y = 0; y < size.y; ++y)
					for (int32 x = 0; x < size.x; ++x)
						maxError = Max(maxError, Abs(static_cast<int32>(actual[y][x].a)
							- Reference(points, { x + 0.5, y + 0.5 }, inner, outer, aa)));
				CHECK(maxError <= (aa ? 1 : 0));
			}
	}
}

TEST_CASE("ImagePolygonFrame.OneSidedAndCollapsedHole")
{
	const Quad square{ 8, 8, 24, 8, 24, 24, 8, 24 };
	for (const bool aa : { false, true })
	{
		Image inner{ 32, 32, Color{ 0, 0 } }, outer = inner, filled = inner;
		square.overwriteFrame(inner, JoinStyle::Round, 2, 0, Palette::White, EnableAntialiasing{ aa });
		square.overwriteFrame(outer, JoinStyle::Round, 0, 2, Palette::White, EnableAntialiasing{ aa });
		square.overwriteFrame(filled, JoinStyle::Round, 20, 0, Palette::White, EnableAntialiasing{ aa });
		CHECK(inner[8][12].a == 255);
		CHECK(inner[7][12].a == 0);
		CHECK(inner[16][16].a == 0);
		CHECK(outer[8][12].a == 0);
		CHECK(outer[7][12].a == 255);
		CHECK(outer[5][5].a == 0);
		CHECK(outer[6][6].a < 255); // Rounded outer corner, not a miter.
		CHECK(filled[16][16].a == 255);
		CHECK(filled[7][12].a == 0);
	}
	// A subpixel band retains its width instead of accumulating its two sides.
	Image thin{ 32, 32, Color{ 0, 0 } };
	Quad{ 8.5, 8.5, 24.5, 8.5, 24.5, 24.5, 8.5, 24.5 }.overwriteFrame(thin, JoinStyle::Round, 0.15, 0.35, Palette::White);
	CHECK(thin[8][16].a == 128);
}

TEST_CASE("ImagePolygonFrame.CompositingAndClipping")
{
	for (const auto& contour : Contours)
		for (const Vec2 offset : { Vec2{ 0, 0 }, Vec2{ -20, -20 }, Vec2{ 16, 16 }, Vec2{ 70, 40 } })
			for (const bool aa : { false, true })
				for (const bool paint : { false, true })
					for (const Color ink : { Color{ 220, 60, 130, 113 }, Color{ 80, 160, 240, 0 }, Color{ 200, 150, 40, 255 } })
					{
						const auto points = contour.map([&](Vec2 p) { return p + offset; });
						const Color background{ 30, 90, 150, 97 };
						Image mask{ 64, 64, Color{ 0, 0 } }, actual{ 32, 32, background }, expected = actual;
						const Array<Vec2> large = points.map([](Vec2 p) { return p + Vec2{ 16, 16 }; });
						Draw(large, mask, 7.25, 3.25, Palette::White, EnableAntialiasing{ aa });
						CompositeMask(expected, mask, ink, paint, { 16, 16 });
						Draw(points, actual, 7.25, 3.25, ink, EnableAntialiasing{ aa }, paint);
						CHECK_THAT(actual, ImageEquals(expected));
					}
}

TEST_CASE("ImagePolygonFrame.DegeneracyAndNoOps")
{
	const Color background{ 30, 90, 150, 97 };
	const Array<Array<Vec2>> emptyContours{
		{ { 8, 8 }, { 8, 8 }, { 8, 8 } }, { { 8, 8 }, { 24, 24 }, { 16, 16 } },
		{ { 8, 8 }, { 8, 8 }, { 24, 24 }, { 8, 8 } },
		{ { 8, 8 }, { 24, 8 }, { 24, 24 }, { 24, 8 } },
		{ { 8, 8 }, { 24, 8 }, { 8, 8 }, { 24, 24 } },
		{ { 8, 8 }, { 12, 12 }, { 24, 24 }, { 16, 16 } } };
	for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
		for (const bool aa : { false, true })
			for (const bool paint : { false, true })
			{
				for (const auto& points : emptyContours)
				{
					Image image{ 32, 32, background }, original = image;
					Draw(points, image, 4, 8, Palette::White, EnableAntialiasing{ aa }, paint, style);
					CHECK_THAT(image, ImageEquals(original));
				}
				for (const auto& points : Contours)
				{
					for (const Vec2 widths : { Vec2{ 0, 0 }, Vec2{ -1, 4 }, Vec2{ 4, -1 } })
					{
						Image image{ 32, 32, background }, original = image;
						Draw(points, image, widths.x, widths.y, Palette::White, EnableAntialiasing{ aa }, paint, style);
						CHECK_THAT(image, ImageEquals(original));
					}
					Image empty;
					Draw(points, empty, 4, 8, Palette::White, EnableAntialiasing{ aa }, paint, style);
					CHECK(empty.isEmpty());
				}
			}
}

TEST_CASE("ImagePolygonFrame.CollinearSpurs")
{
	const Triangle triangle{ 4.25, 4.75, 16.25, 4.75, 12.25, 28.75 };
	const Array<Vec2> contour{ { 4.25, 4.75 }, { 28.25, 4.75 }, { 16.25, 4.75 }, { 12.25, 28.75 } };
	for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
		for (const bool aa : { false, true })
			for (const Vec2 widths : { Vec2{ 0, 4.25 }, Vec2{ 4.25, 0 }, Vec2{ 2.25, 3.25 } })
				for (size_t rotation = 0; rotation < 4; ++rotation)
				{
					Array<Vec2> points;
					for (size_t i = 0; i < 4; ++i) points.push_back(contour[(i + rotation) % 4]);
					Image expected{ 32, 32, Color{ 0, 0 } }, actual = expected, reversed = expected;
					triangle.overwriteFrame(expected, style, widths.x, widths.y, Palette::White, EnableAntialiasing{ aa });
					Draw(points, actual, widths.x, widths.y, Palette::White, EnableAntialiasing{ aa }, false, style);
					Draw(points.reversed(), reversed, widths.x, widths.y, Palette::White, EnableAntialiasing{ aa }, false, style);
					CHECK_THAT(actual, ImageEquals(expected));
					CHECK_THAT(reversed, ImageEquals(expected));
				}
}

TEST_CASE("ImagePolygonFrame.DestinationAlpha")
{
	const Triangle triangle{ Contours[0][0], Contours[0][1], Contours[0][2] };
	const Quad quad{ Contours[4][0], Contours[4][1], Contours[4][2], Contours[4][3] };
	for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
		for (const auto mode : { ImagePixel::BlendMode::SourceOver, ImagePixel::BlendMode::Overwrite })
			for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			{
				Image expected{ 32, 32, Color{ 30, 90, 150 } }, actual = expected;
				const Color ink{ 220, 60, 130, 113 };
				ImageDraw::TriangleFrame(expected, triangle, style, 3, 6, ink, mode, aa);
				ImageDraw::TriangleFrame(actual, triangle, style, 3, 6, ink, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				CHECK_THAT(actual, ImageEquals(expected));
				expected.fill(Color{ 30, 90, 150 }); actual = expected;
				ImageDraw::QuadFrame(expected, quad, style, 3, 6, ink, mode, aa);
				ImageDraw::QuadFrame(actual, quad, style, 3, 6, ink, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				CHECK_THAT(actual, ImageEquals(expected));
			}
}

TEST_CASE("ImagePolygonFrame.Benchmark", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Image polygon inner/outer frames").unit("draw").epochs(7).minEpochIterations(3);
	for (const int32 size : { 32, 256, 1024 })
		for (const size_t index : { size_t{ 0 }, size_t{ 2 }, size_t{ 3 }, size_t{ 4 } })
			for (const double width : { 1.0, 8.0, 32.0 })
				for (const bool aa : { false, true })
				{
					const auto p = Contours[index].map([&](Vec2 v) { return v * (size / 32.0); });
					Image image{ size, size, Color{ 30, 90, 150, 97 } };
					const auto measure = [&](const auto& shape)
					{
						for (const auto style : {JoinStyle::Bevel, JoinStyle::Round, JoinStyle::Miter})
							for (const int32 mode : { 0, 1, 2, 3 })
							{
								const std::string label = "size=" + std::to_string(size) + "/shape=" + std::to_string(index)
									+ "/width=" + std::to_string(width) + "/aa=" + std::to_string(aa) + "/mode=" + std::to_string(mode)
									+ (style == JoinStyle::Bevel ? "/Bevel" : style == JoinStyle::Round ? "/Round" : "/Miter");
								bench.run(label, [&]
								{
									const Color ink{ 220, 60, 130, 113 };
									if (mode == 0) shape.paintFrame(image, style, width, ink, EnableAntialiasing{ aa });
									else shape.paintFrame(image, style, mode == 2 ? width : (mode == 3 ? 0.0 : width * 0.5),
										mode == 3 ? width : (mode == 2 ? 0.0 : width * 0.5), ink, EnableAntialiasing{ aa });
									ankerl::nanobench::doNotOptimizeAway(image.data());
								});
							}
					};
					if (p.size() == 3) measure(Triangle{ p[0], p[1], p[2] });
					else measure(Quad{ p[0], p[1], p[2], p[3] });
				}
}
