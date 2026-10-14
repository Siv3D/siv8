//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <ThirdParty/nanobench/nanobench.h>

namespace
{
	Bezier3 Straight(const Vec2 from, const Vec2 to)
	{
		return Bezier3::FromHermite(from, to - from, to, to - from);
	}

	Spline2D CrossedPath(const CloseRing closed)
	{
		return Spline2D::FromBezierSegments(Array<Bezier3>{
			Straight({ 8.25, 8.75 }, { 56.25, 56.75 }),
			Straight({ 56.25, 56.75 }, { 56.25, 56.75 }),
			Straight({ 56.25, 56.75 }, { 8.25, 56.75 }),
			Straight({ 8.25, 56.75 }, { 56.25, 8.75 }),
			Straight({ 56.25, 8.75 }, { 8.25, 8.75 }) }, closed);
	}

	void WriteOutline(const Spline2D& path, const LineString& outline, Image& image,
		const bool paint, const LineCap start, const LineCap end, const double width,
		const Color color, const EnableAntialiasing aa)
	{
		if (path.isClosed())
		{
			if (paint) outline.paintClosed(image, width, color, aa);
			else outline.overwriteClosed(image, width, color, aa);
		}
		else
		{
			if (paint) outline.paint(image, start, end, width, color, aa);
			else outline.overwrite(image, start, end, width, color, aa);
		}
	}

	// Independent scalar straight-alpha composition, including byte rounding.
	Color Composite(const Color src, const Color dst, const uint32 coverage, const bool paint)
	{
		if (not paint)
		{
			const auto channel = [&](uint32 s, uint32 d) { return (s * coverage + d * (255 - coverage) + 127) / 255; };
			return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), channel(src.a, dst.a) };
		}
		const uint32 alpha = (src.a * coverage + 127) / 255;
		if (alpha == 0) return dst;
		const uint32 denominator = alpha * 255 + dst.a * (255 - alpha);
		const auto channel = [&](uint32 s, uint32 d) { return (s * alpha * 255 + d * dst.a * (255 - alpha) + denominator / 2) / denominator; };
		return Color{ channel(src.r, dst.r), channel(src.g, dst.g), channel(src.b, dst.b), (denominator + 127) / 255 };
	}
}

TEST_CASE("Spline2DImage.QualityCapsAndClosure")
{
	const Array<Vec2> knots{ { -8.25, 12.75 }, { 18.25, 60.75 }, { 42.25, -12.75 }, { 72.25, 48.75 } };
	const Array<Spline2D> paths{ Spline2D::FromCatmullRom(knots), Spline2D::FromCatmullRom(knots, CloseRing::Yes),
		CrossedPath(CloseRing::No), CrossedPath(CloseRing::Yes) };
	const Color background{ 30, 90, 150, 97 }, ink{ 240, 80, 20, 123 };
	for (size_t index = 0; index < paths.size(); ++index)
		for (const double error : { 0.25, 1.5 })
			for (const int32 budget : { 1, 7, 256 })
			{
				const auto& path = paths[index];
				const LineString outline = path.toLineStringAdaptive(error, budget);
				for (const LineCap start : { LineCap::Flat, LineCap::Square, LineCap::Round })
					for (const LineCap end : { LineCap::Flat, LineCap::Square, LineCap::Round })
						for (const double width : { 0.6, 8.5 })
							for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
								for (const bool paint : { false, true })
								{
									CAPTURE(index, error, budget, width, static_cast<int>(start), static_cast<int>(end), static_cast<bool>(aa), paint);
									Image actual{ 64, 64, background }, expected = actual;
									WriteOutline(path, outline, expected, paint, start, end, width, ink, aa);
									if (paint) CHECK(&path.paint(actual, start, end, width, ink, aa, error, budget) == &path);
									else CHECK(&path.overwrite(actual, start, end, width, ink, aa, error, budget) == &path);
									CHECK(actual == expected);
								}
			}

	const auto& path = paths.front();
	Image fine{ 64, 64, background }, coarse = fine;
	path.paint(fine, 3, ink, EnableAntialiasing::Yes, 0.1, 256);
	path.paint(coarse, 3, ink, EnableAntialiasing::Yes, 2.0, 256);
	CHECK(fine != coarse);
	// A per-curve budget of one retains all knots, not just the path endpoints.
	LineString chordPath{ knots.begin(), knots.end() };
	Image limited{ 64, 64, background }, chords = limited;
	path.overwrite(limited, 3, ink, EnableAntialiasing::Yes, 0.1, 1);
	chordPath.overwrite(chords, 3, ink);
	CHECK(limited == chords);
}

TEST_CASE("Spline2DImage.DefaultsAndOverloads")
{
	for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
	{
		const auto path = CrossedPath(closed);
		const auto outline = path.toLineStringAdaptive();
		for (const bool paint : { false, true })
			for (const bool explicitQuality : { false, true })
				for (int32 form = 0; form < 3; ++form)
				{
					Image actual{ 64, 64, Color{ 30, 90, 150, 97 } }, expected = actual;
					const Color ink{ 240, 80, 20, 123 };
					const double width = form == 0 ? 1 : 6;
					const auto cap = form == 2 ? LineCap::Flat : LineCap::Round;
					const auto aa = explicitQuality ? EnableAntialiasing::No : EnableAntialiasing::Yes;
					WriteOutline(path, outline, expected, paint, cap, cap, width, ink, aa);
					const Spline2D* returned = nullptr;
					if (paint && explicitQuality)
					{
						if (form == 0) returned = &path.paint(actual, ink, aa, 0.5, 256);
						else if (form == 1) returned = &path.paint(actual, width, ink, aa, 0.5, 256);
						else returned = &path.paint(actual, cap, width, ink, aa, 0.5, 256);
					}
					else if (paint)
					{
						if (form == 0) returned = &path.paint(actual, ink);
						else if (form == 1) returned = &path.paint(actual, width, ink);
						else returned = &path.paint(actual, cap, width, ink);
					}
					else if (explicitQuality)
					{
						if (form == 0) returned = &path.overwrite(actual, ink, aa, 0.5, 256);
						else if (form == 1) returned = &path.overwrite(actual, width, ink, aa, 0.5, 256);
						else returned = &path.overwrite(actual, cap, width, ink, aa, 0.5, 256);
					}
					else
					{
						if (form == 0) returned = &path.overwrite(actual, ink);
						else if (form == 1) returned = &path.overwrite(actual, width, ink);
						else returned = &path.overwrite(actual, cap, width, ink);
					}
					CHECK(returned == &path);
					CHECK(actual == expected);
				}
	}
}

TEST_CASE("Spline2DImage.CompositesJoinsAndIntersectionsOnce")
{
	for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		{
			const auto path = CrossedPath(closed);
			Image mask{ 64, 64, Color{ 0, 0 } };
			path.overwrite(mask, 8, Palette::White, aa);
			CHECK(mask[32][32].a == 255); // Crossing
			CHECK(mask[56][56].a == 255); // Internal knot next to a collapsed interval
			CHECK(mask[8][8].a == 255); // Closed seam or coincident open endpoints
			for (const bool paint : { false, true })
				for (const uint32 alpha : { 0u, 113u, 255u })
				{
					const Color ink{ 220, 60, 130, alpha };
					Image actual{ 64, 64 }, expected = actual;
					for (size_t i = 0; i < mask.size_bytes() / sizeof(Color); ++i)
					{
						const Color background{ uint8(i * 73), uint8(i * 31), uint8(i * 17), uint8(i * 23) };
						actual.data()[i] = background;
						expected.data()[i] = Composite(ink, background, mask.data()[i].a, paint);
					}
					if (paint) path.paint(actual, 8, ink, aa);
					else path.overwrite(actual, 8, ink, aa);
					CHECK(actual == expected);
				}
		}
}

TEST_CASE("Spline2DImage.EmptyDegenerateAndInvalid")
{
	const Vec2 a{ 8.25, 8.75 }, b{ 56.25, 56.75 };
	const auto point = Straight(a, a);
	const Array<Spline2D> emptyPaths{ Spline2D{}, Spline2D::FromBezierSegments(Array<Bezier3>{ point }),
		Spline2D::FromBezierSegments(Array<Bezier3>{ point, point }, CloseRing::Yes),
		Spline2D::FromBezierSegments(Array<Bezier3>{ Straight(a, b), Straight(b, a) }, CloseRing::Yes) };
	const Image before{ 64, 64, Color{ 30, 90, 150, 97 } };
	const auto regular = CrossedPath(CloseRing::No);
	for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
		for (const LineCap cap : { LineCap::Flat, LineCap::Round, LineCap::Square })
		{
			Image image = before;
			for (const auto& path : emptyPaths)
			{
				path.paint(image, cap, 8, Palette::Red, aa);
				path.overwrite(image, cap, 8, Palette::Red, aa);
			}
			CHECK(image == before);
			for (const double width : { 0.0, -8.0 })
			{
				regular.paint(image, cap, width, Palette::Red, aa);
				regular.overwrite(image, cap, width, Palette::Red, aa);
			}
			CHECK(image == before);
			Image empty;
			regular.paint(empty, cap, 8, Palette::Red, aa);
			regular.overwrite(empty, cap, 8, Palette::Red, aa);
			CHECK(empty.isEmpty());
		}
	for (const auto& path : { Spline2D{}, regular, CrossedPath(CloseRing::Yes) })
		for (const double error : { -1.0, 0.0, 0.5 })
			for (const int32 budget : { -1, 0, 256, 4097 })
				if (error <= 0 || budget < 1 || budget > 4096)
				{
					Image image = before;
					CHECK_THROWS_AS(path.paint(image, 4, Palette::Red, EnableAntialiasing::Yes, error, budget), std::invalid_argument);
					CHECK_THROWS_AS(path.overwrite(image, 4, Palette::Red, EnableAntialiasing::Yes, error, budget), std::invalid_argument);
					CHECK(image == before);
				}
	Image upper = before, expected = before;
	regular.paint(upper, 4, Palette::Red, EnableAntialiasing::Yes, 0.25, 4096);
	regular.toLineStringAdaptive(0.25, 4096).paint(expected, 4, Palette::Red);
	CHECK(upper == expected);
}

TEST_CASE("Spline2DImage.ReverseAndClipping")
{
	for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
	{
		const auto path = CrossedPath(closed);
		const auto reversed = path.reversed();
		Array<Bezier3> moved;
		for (const auto& curve : path.segments()) moved.push_back(curve.movedBy(-20, -16));
		const auto clippedPath = Spline2D::FromBezierSegments(moved, closed);
		for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
			for (const bool paint : { false, true })
			{
				const Color background{ 30, 90, 150, 97 }, ink{ 220, 60, 130, 113 };
				Image forward{ 64, 64, background }, backward = forward;
				Image clipped{ 32, 32, background };
				if (paint)
				{
					path.paint(forward, LineCap::Square, LineCap::Flat, 8, ink, aa);
					reversed.paint(backward, LineCap::Flat, LineCap::Square, 8, ink, aa);
					clippedPath.paint(clipped, LineCap::Square, LineCap::Flat, 8, ink, aa);
				}
				else
				{
					path.overwrite(forward, LineCap::Square, LineCap::Flat, 8, ink, aa);
					reversed.overwrite(backward, LineCap::Flat, LineCap::Square, 8, ink, aa);
					clippedPath.overwrite(clipped, LineCap::Square, LineCap::Flat, 8, ink, aa);
				}
				CHECK(forward == backward);
				bool equal = true;
				for (int32 y = 0; y < 32; ++y)
					for (int32 x = 0; x < 32; ++x)
						equal &= (clipped[y][x] == forward[y + 16][x + 20]);
				CHECK(equal);
			}
	}
}

TEST_CASE("Spline2DImage.Benchmark", "[.benchmark]")
{
	ankerl::nanobench::Bench bench;
	bench.title("Spline Image writing").unit("draw").epochs(9).minEpochIterations(8);
	for (const int32 size : { 64, 256 })
		for (const int32 intervals : { 4, 32, 128 })
			for (const CloseRing closed : { CloseRing::No, CloseRing::Yes })
			{
				Array<Vec2> knots;
				for (int32 i = 0; i <= intervals; ++i)
					knots.push_back({ size * (0.1 + 0.8 * i / intervals), size * (0.5 + 0.3 * std::sin(i * 0.7)) });
				const auto path = Spline2D::FromCatmullRom(knots, closed);
				const auto outline = path.toLineStringAdaptive();
				const std::string base = "/" + std::to_string(size) + "/" + std::to_string(path.segmentCount()) + (closed ? "/closed" : "/open");
				LineString reusable;
				path.toLineStringAdaptive(reusable);
				bench.run("conversion" + base, [&]
				{
					path.toLineStringAdaptive(reusable);
					ankerl::nanobench::doNotOptimizeAway(reusable.data());
				});
				Image image{ size, Color{ 40, 70, 100, 255 } };
				const Color ink{ 211, 83, 147, 113 };
				for (const double width : { 1.5, 12.0 })
					for (const EnableAntialiasing aa : { EnableAntialiasing::No, EnableAntialiasing::Yes })
						for (const bool paint : { false, true })
						{
							const auto suffix = base + (width == 1.5 ? "/thin" : "/wide") + (aa ? "/AA" : "/noAA") + (paint ? "/paint" : "/overwrite");
							const auto run = [&](const char* name, const auto& draw)
							{
								image.fill(Color{ 40, 70, 100, 255 });
								bench.run(name + suffix, [&] { draw(); ankerl::nanobench::doNotOptimizeAway(image.data()); });
							};
							run("direct", [&]
							{
								if (paint) path.paint(image, width, ink, aa);
								else path.overwrite(image, width, ink, aa);
							});
							run("explicit-conversion", [&]
							{
								const auto converted = path.toLineStringAdaptive();
								WriteOutline(path, converted, image, paint, LineCap::Round, LineCap::Round, width, ink, aa);
							});
							run("cached", [&] { WriteOutline(path, outline, image, paint, LineCap::Round, LineCap::Round, width, ink, aa); });
						}
			}
}
