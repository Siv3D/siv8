//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <array>
# include <random>

namespace
{
	const Array<Array<Vec2>> Contours{
		{ { 5.21, 4.37 }, { 29.21, 8.37 }, { 12.21, 29.37 } },
		{ { -9.21, 14.37 }, { 40.21, 15.37 }, { 16.21, 16.37 } },
		{ { 5.21, 4.37 }, { 27.21, 6.37 }, { 29.21, 26.37 }, { 4.21, 28.37 } },
		{ { 3.21, 3.37 }, { 29.21, 4.37 }, { 13.21, 13.37 }, { 8.21, 28.37 } },
		{ { 4.21, 4.37 }, { 28.21, 28.37 }, { 4.21, 28.37 }, { 28.21, 4.37 } },
		{ { 4.21, 4.37 }, { 28.21, 4.37 }, { 28.21, 4.37 }, { 12.21, 28.37 } },
		{ { 4.21, 4.37 }, { 16.21, 4.37 }, { 28.21, 4.37 }, { 12.21, 28.37 } },
		{ { 4.21, 4.37 }, { 28.21, 4.37 }, { 16.21, 4.37 }, { 12.21, 28.37 } },
		{ { 4.21, 4.37 }, { 28.21, 4.37 }, { 16.21, 4.37 } },
		{ { 4.21875, 4.375 }, { 11.21875, 25.375 }, { 5.21875, 7.375 } },
	};

	void Draw(const Array<Vec2>& p, Image& image, JoinStyle style, double width,
		Color color = Palette::White, EnableAntialiasing aa = EnableAntialiasing::Yes, bool paint = false)
	{
		const auto draw = [&](const auto& shape)
		{
			if (paint) CHECK(&shape.paintFrame(image, style, width, color, aa) == &shape);
			else CHECK(&shape.overwriteFrame(image, style, width, color, aa) == &shape);
		};
		if (p.size() == 3) draw(Triangle{ p[0], p[1], p[2] });
		else draw(Quad{ p[0], p[1], p[2], p[3] });
	}

	Array<Vec2> Normalize(const Array<Vec2>& input)
	{
		Array<Vec2> p;
		for (const Vec2 v : input) if (p.empty() || (p.back() != v)) p.push_back(v);
		if ((1 < p.size()) && (p.front() == p.back())) p.pop_back();
		return p;
	}

	// Independent pointwise oracle: projection onto finite edge bands, and an
	// intersection of half-planes at each outer corner. No polygon construction,
	// offset-line intersection, scanline span, or GPU output is used as a reference.
	bool Contains(const Array<Vec2>& p, const Vec2 sample, double half, JoinStyle style)
	{
		if (p.size() < 3) return false;
		for (size_t i = 0; i < p.size(); ++i)
		{
			const Vec2 delta = p[(i + 1) % p.size()] - p[i], q = sample - p[i];
			const double length = std::hypot(delta.x, delta.y);
			if ((0.0 <= q.dot(delta)) && (q.dot(delta) <= length * length)
				&& (Abs(delta.cross(q)) <= half * length)) return true;
			const Vec2 previous = (p[(i + p.size() - 1) % p.size()] - p[i]);
			if (previous.cross(delta) == 0.0) continue;
			const Vec2 a = previous.normalized(), b = delta / length;
			if ((q.dot(a) > 0.0) || (q.dot(b) > 0.0)
				|| (Abs(a.cross(q)) > half) || (Abs(b.cross(q)) > half)) continue;
			if ((style == JoinStyle::Bevel) && (std::atan2(Abs(a.cross(b)), a.dot(b)) < (60.0_deg - 1e-12))
				&& (q.dot(-(a + b).normalized()) > half)) continue;
			return true;
		}
		return false;
	}

	uint8 Coverage(const Array<Vec2>& p, int32 x, int32 y, double width, JoinStyle style, bool aa)
	{
		const int32 n = aa ? 4 : 1;
		int32 covered = 0;
		for (int32 sy = 0; sy < n; ++sy)
			for (int32 sx = 0; sx < n; ++sx)
				covered += Contains(p, { x + (sx + 0.5) / n, y + (sy + 0.5) / n }, width * 0.5, style);
		return static_cast<uint8>((covered * 255 + n * n / 2) / (n * n));
	}

	Color Composite(Color src, Color dst, uint32 coverage, bool paint)
	{
		if (!paint)
		{
			const auto channel = [&](uint32 s, uint32 d) { return (s * coverage + d * (255 - coverage) + 127) / 255; };
			return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), channel(src.a, dst.a) };
		}
		const uint32 a = (src.a * coverage + 127) / 255;
		if (a == 0) return dst;
		const uint32 denominator = a * 255 + dst.a * (255 - a);
		const auto channel = [&](uint32 s, uint32 d) { return (s * a * 255 + d * dst.a * (255 - a) + denominator / 2) / denominator; };
		return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), (denominator + 127) / 255 };
	}
}

TEST_CASE("ImageJoinStyle.InnerOuterReferenceAndCompositing")
{
	const Color ink{220, 60, 130, 113}, background{30, 90, 150, 97};
	for (size_t shape = 0; shape < 5; ++shape)
		for (const JoinStyle style : {JoinStyle::Bevel, JoinStyle::Miter})
			for (const Vec2 widths : {Vec2{0, 4.25}, Vec2{4.25, 0}, Vec2{0.15, 0.35},
				Vec2{2.25, 2.25}, Vec2{9.25, 3.25}, Vec2{100, 0}, Vec2{100, 12.25}})
				for (const bool aa : {false, true})
					for (const bool paint : {false, true})
					{
						CAPTURE(shape, style, widths, aa, paint);
						const auto& points = Contours[shape];
						const int samples = aa ? 4 : 1;
						Image expected{32, 32, background};
						for (int y = 0; y < 32; ++y)
							for (int x = 0; x < 32; ++x)
							{
								int covered = 0;
								for (int sy = 0; sy < samples; ++sy)
									for (int sx = 0; sx < samples; ++sx)
									{
										const Vec2 q{x + (sx + 0.5) / samples, y + (sy + 0.5) / samples};
										bool inside = false;
										for (size_t i = 0; i < points.size(); ++i)
										{
											const Vec2 a = points[i], b = points[(i + 1) % points.size()];
											if (((a.y > q.y) != (b.y > q.y)) && (q.x < a.x + (q.y - a.y) * (b.x - a.x) / (b.y - a.y))) inside = !inside;
										}
										const double radius = inside ? widths.x : widths.y;
										covered += (radius > 0 && Contains(points, q, radius, style));
									}
								expected[y][x] = Composite(ink, background, (covered * 255 + samples * samples / 2) / (samples * samples), paint);
							}
						for (const bool reverse : {false, true})
						{
							const auto p = reverse ? points.reversed() : points;
							const auto check = [&](const auto& polygon)
							{
								Image actual{32, 32, background}, omitted = actual;
								if (paint) CHECK(&polygon.paintFrame(actual, style, widths.x, widths.y, ink, EnableAntialiasing{aa}) == &polygon);
								else CHECK(&polygon.overwriteFrame(actual, style, widths.x, widths.y, ink, EnableAntialiasing{aa}) == &polygon);
								CHECK(actual == expected);
								if (style == JoinStyle::Bevel)
								{
									if (paint) polygon.paintFrame(omitted, widths.x, widths.y, ink, EnableAntialiasing{aa});
									else polygon.overwriteFrame(omitted, widths.x, widths.y, ink, EnableAntialiasing{aa});
									CHECK(omitted == actual);
								}
							};
							if (p.size() == 3) check(Triangle{p[0], p[1], p[2]});
							else check(Quad{p[0], p[1], p[2], p[3]});
						}
					}
}

TEST_CASE("ImageJoinStyle.ReferenceGeometry")
{
	for (size_t i = 0; i < Contours.size(); ++i)
		for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter })
			for (const double width : { 0.35, 4.25, 16.25, 80.25 })
				for (const bool aa : { false, true })
				{
					CAPTURE(i, style, width, aa);
					Image image{ 32, 32, Color{ 0, 0 } };
					Draw(Contours[i], image, style, width, Palette::White, EnableAntialiasing{ aa });
					const auto p = Normalize(Contours[i]);
					int32 errors = 0;
					for (int32 y = 0; y < 32; ++y)
						for (int32 x = 0; x < 32; ++x)
							errors += (image[y][x].a != Coverage(p, x, y, width, style, aa));
					CHECK(errors == 0);
				}
}

TEST_CASE("ImageJoinStyle.VariedGeometryAndWinding")
{
	std::mt19937 random{ 20261015 };
	std::uniform_real_distribution<double> coordinate{ -12, 36 }, thickness{ 0.1, 40 };
	for (int32 trial = 0; trial < 80; ++trial)
	{
		Array<Vec2> p;
		for (int32 i = 0; i < (trial % 2 ? 3 : 4); ++i) p.emplace_back(coordinate(random), coordinate(random));
		const double width = thickness(random);
		for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter })
			for (const bool aa : { false, true })
			{
				CAPTURE(trial, style, aa, width);
				Image image{ 24, 24, Color{ 0, 0 } };
				Draw(p, image, style, width, Palette::White, EnableAntialiasing{ aa });
				int32 errors = 0;
				for (int32 y = 0; y < 24; ++y)
					for (int32 x = 0; x < 24; ++x)
						errors += (image[y][x].a != Coverage(p, x, y, width, style, aa));
				CHECK(errors == 0);
				for (size_t start = 0; start < p.size(); ++start)
				{
					Array<Vec2> rotated;
					for (size_t i = 0; i < p.size(); ++i) rotated.push_back(p[(start + i) % p.size()]);
					Image shifted{ 24, 24, Color{ 0, 0 } }, reversed = shifted;
					Draw(rotated, shifted, style, width, Palette::White, EnableAntialiasing{ aa });
					Draw(rotated.reversed(), reversed, style, width, Palette::White, EnableAntialiasing{ aa });
					CHECK((shifted == image));
					CHECK((reversed == image));
				}
			}
	}
}

TEST_CASE("ImageJoinStyle.OmittedStyleAndExplicitRound")
{
	for (const auto& p : Contours)
		for (const bool aa : { false, true })
			for (const bool paint : { false, true })
				for (const double width : { -1.0, 0.0, 0.5, 5.25, 40.0 })
				{
					const Color ink{ 220, 60, 130, 113 };
					const Image background{ 32, 32, Color{ 30, 90, 150, 97 } };
					Image expected = background, actual = background, omitted = background;
					Draw(p, expected, JoinStyle::Bevel, width, ink, EnableAntialiasing{ aa }, paint);
					const auto draw = [&](const auto& shape)
					{
						if (paint) CHECK(&shape.paintFrame(omitted, width, ink, EnableAntialiasing{ aa }) == &shape);
						else CHECK(&shape.overwriteFrame(omitted, width, ink, EnableAntialiasing{ aa }) == &shape);
					};
					if (p.size() == 3) draw(Triangle{ p[0], p[1], p[2] });
					else draw(Quad{ p[0], p[1], p[2], p[3] });
					CHECK((omitted == expected));
					actual = expected = background;
					Draw(p, actual, JoinStyle::Round, width, ink, EnableAntialiasing{ aa }, paint);
					ImageDraw::ClosedLineString(expected, p, width, ink,
						paint ? ImagePixel::BlendMode::SourceOver : ImagePixel::BlendMode::Overwrite, EnableAntialiasing{ aa });
					CHECK((actual == expected));
				}
}

TEST_CASE("ImageJoinStyle.CompositingClippingAndNarrowImages")
{
	for (const auto& contour : Contours)
		for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter })
			for (const bool aa : { false, true })
				for (const bool paint : { false, true })
					for (const Color ink : { Color{ 220, 60, 130, 113 }, Color{ 80, 160, 240, 0 } })
						for (const Size size : { Size{ 32, 32 }, Size{ 1, 32 }, Size{ 32, 1 } })
						{
							const Color bg{ 30, 90, 150, 97 };
							const auto p = contour.map([](Vec2 v) { return v - Vec2{ 8, 12 }; });
							Image actual{ size, bg }, expected = actual;
							Image mask{ 64, 64, Color{ 0, 0 } };
							Draw(p.map([](Vec2 v) { return v + Vec2{ 16, 16 }; }), mask, style, 28.25, Palette::White, EnableAntialiasing{ aa });
							for (int32 y = 0; y < size.y; ++y)
								for (int32 x = 0; x < size.x; ++x)
									expected[y][x] = Composite(ink, bg, mask[y + 16][x + 16].a, paint);
							Draw(p, actual, style, 28.25, ink, EnableAntialiasing{ aa }, paint);
							CHECK((actual == expected));
						}
}

TEST_CASE("ImageJoinStyle.BevelThresholdAndUnlimitedMiter")
{
	for (const double angle : { 0.01, 30.0, 59.99, 60.0, 60.01, 90.0, 179.99 })
	{
		const double radians = Math::ToRadians(angle * 0.5);
		const Vec2 tip{ 16.21, 6.37 };
		const Vec2 left{ -std::sin(radians) * 20, std::cos(radians) * 20 };
		const Array<Vec2> p{ tip, tip + left, tip + Vec2{ -left.x, left.y } };
		for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter })
		{
			Image image{ 32, 32, Color{ 0, 0 } };
			Draw(p, image, style, 4.25);
			int32 errors = 0;
			for (int32 y = 0; y < 32; ++y)
				for (int32 x = 0; x < 32; ++x)
					errors += (image[y][x].a != Coverage(p, x, y, 4.25, style, true));
			CAPTURE(angle, style);
			CHECK(errors == 0);
			if (angle == 30.0) CHECK(image[2][16].a == ((style == JoinStyle::Miter) ? 255 : 0));
		}
	}
}

TEST_CASE("ImageJoinStyle.EquilateralRotation")
{
	const double h = (10 * std::sqrt(3.0));
	const Array<Vec2> triangle{{-10,-h/3}, {10,-h/3}, {0,2*h/3}};
	for (int degrees = 0; degrees < 360; degrees += 15)
		for (const double width : {0.5, 4.0, 40.0})
			for (const bool aa : {false, true})
			{
				const auto points = triangle.map([&](const Vec2 p) { return p.rotated(Math::ToRadians(degrees)) + Vec2{16,16}; });
				Image actual{32,32, Color{0,0}}, expected = actual;
				Draw(points, actual, JoinStyle::Bevel, width, Palette::White, EnableAntialiasing{aa});
				Draw(points, expected, JoinStyle::Miter, width, Palette::White, EnableAntialiasing{aa});
				CAPTURE(degrees, width, aa);
				CHECK(actual == expected);
			}
}

TEST_CASE("ImageJoinStyle.DegenerateAndNoOp")
{
	const Array<Array<Vec2>> empty{
		{ { 8, 8 }, { 8, 8 }, { 8, 8 } },
		{ { 8, 8 }, { 8, 8 }, { 24, 8 } },
		{ { 8, 8 }, { 24, 8 }, { 24, 8 }, { 8, 8 } } };
	for (const auto style : { JoinStyle::Round, JoinStyle::Bevel, JoinStyle::Miter })
		for (const bool aa : { false, true })
			for (const bool paint : { false, true })
			{
				const Color bg{ 30, 90, 150, 97 };
				Image image{ 32, 32, bg }, original = image, noImage;
				for (const auto& p : empty) Draw(p, image, style, 8, Palette::White, EnableAntialiasing{ aa }, paint);
				for (const double width : { 0.0, -1.0 }) Draw(Contours[0], image, style, width, Palette::White, EnableAntialiasing{ aa }, paint);
				Draw(Contours[0], noImage, style, 8, Palette::White, EnableAntialiasing{ aa }, paint);
				Draw(Contours[0].map([](Vec2 p) { return p + Vec2{ 1000, 1000 }; }), image, style, 8, Palette::White, EnableAntialiasing{ aa }, paint);
				CHECK((image == original));
				CHECK(noImage.isEmpty());
			}
	// Backtracking adds only edge bands with flat extremities for Bevel/Miter.
	for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter })
	{
		Image image{ 32, 32, Color{ 0, 0 } };
		Triangle{ 5.25, 16.25, 29.25, 16.25, 15.25, 16.25 }.overwriteFrame(image, style, 4.5, Palette::White);
		CHECK(image[16][4].a == 0);
		CHECK(image[16][5].a == 191);
		CHECK(image[16][15].a == 255);
		CHECK(image[13][15].a == 0);
	}
}

TEST_CASE("ImageJoinStyle.PixelBoundaryAndCollapsedHole")
{
	// A square miter frame is exactly outer-minus-inner axis-aligned rectangles.
	// These coordinates put edges on pixel and subpixel centers to exercise ties.
	for (const double offset : { 0.0, 0.125, 0.5 })
		for (const double width : { 0.25, 1.0, 4.0, 16.0, 40.0 })
			for (const bool aa : { false, true })
				for (const auto style : { JoinStyle::Bevel, JoinStyle::Miter })
				{
					const double lo = 8 + offset, hi = 24 + offset, half = width * 0.5;
					const Quad square{ lo, lo, hi, lo, hi, hi, lo, hi };
					Image image{ 32, 32, Color{ 0, 0 } };
					square.overwriteFrame(image, style, width, Palette::White, EnableAntialiasing{ aa });
					const int32 n = aa ? 4 : 1;
					int32 errors = 0;
					for (int32 y = 0; y < 32; ++y)
						for (int32 x = 0; x < 32; ++x)
						{
							int32 covered = 0;
							for (int32 sy = 0; sy < n; ++sy)
								for (int32 sx = 0; sx < n; ++sx)
								{
									const double px = x + (sx + 0.5) / n, py = y + (sy + 0.5) / n;
									const bool outer = ((lo - half <= px) && (px < hi + half) && (lo - half <= py) && (py < hi + half));
									const bool inner = ((lo + half <= px) && (px < hi - half) && (lo + half <= py) && (py < hi - half));
									covered += outer && !inner;
								}
							errors += (image[y][x].a != (covered * 255 + n * n / 2) / (n * n));
						}
					CAPTURE(offset, width, aa, style);
					CHECK(errors == 0);
				}
}

TEST_CASE("ImageJoinStyle.DestinationAlpha")
{
	for (const auto style : { JoinStyle::Round, JoinStyle::Bevel, JoinStyle::Miter })
		for (const auto mode : { ImagePixel::BlendMode::SourceOver, ImagePixel::BlendMode::Overwrite })
			for (const auto aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			{
				Image expected{ 32, 32, Color{ 30, 90, 150 } }, actual = expected;
				const Color ink{ 220, 60, 130, 113 };
				const Triangle triangle{ Contours[0][0], Contours[0][1], Contours[0][2] };
				const Quad quad{ Contours[4][0], Contours[4][1], Contours[4][2], Contours[4][3] };
				ImageDraw::TriangleFrame(expected, triangle, style, 8, ink, mode, aa);
				ImageDraw::TriangleFrame(actual, triangle, style, 8, ink, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				CHECK((actual == expected));
				expected.fill(Color{ 30, 90, 150 }); actual = expected;
				ImageDraw::QuadFrame(expected, quad, style, 28, ink, mode, aa);
				ImageDraw::QuadFrame(actual, quad, style, 28, ink, mode, aa, ImageDraw::DstAlpha::AssumeOpaque);
				CHECK((actual == expected));
			}
}

TEST_CASE("ImageJoinStyle.Benchmark", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Image centered frame joins").unit("draw").epochs(7).minEpochIterations(5);
	const std::array styles{ Optional<JoinStyle>{}, Optional<JoinStyle>{JoinStyle::Bevel},
		Optional<JoinStyle>{JoinStyle::Round}, Optional<JoinStyle>{JoinStyle::Miter} };
	for (const int32 size : { 32, 256 })
		for (const size_t index : { size_t{ 0 }, size_t{ 2 }, size_t{ 3 }, size_t{ 4 } })
			for (const double width : { 1.0, 8.0, 40.0 })
				for (const bool aa : { false, true })
				{
					const auto p = Contours[index].map([&](Vec2 v) { return v * (size / 32.0); });
					Image image{ size, size, Color{ 30, 90, 150, 97 } };
					const auto measure = [&](const auto& shape)
					{
						for (const auto style : styles)
						{
							const std::string label = "size=" + std::to_string(size) + "/shape=" + std::to_string(index)
								+ "/width=" + std::to_string(width) + "/aa=" + std::to_string(aa) + "/style=" + (style ? Format(*style).toUTF8() : "omitted");
							bench.run(label, [&]
							{
								const Color ink{ 220, 60, 130, 113 };
								if (!style) shape.paintFrame(image, width, ink, EnableAntialiasing{ aa });
								else shape.paintFrame(image, *style, width, ink, EnableAntialiasing{ aa });
								ankerl::nanobench::doNotOptimizeAway(image.data());
							});
						}
					};
					if (p.size() == 3) measure(Triangle{ p[0], p[1], p[2] });
					else measure(Quad{ p[0], p[1], p[2], p[3] });
				}
}
