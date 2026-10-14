//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "ImageDrawingTestHelper.hpp"
# include <array>

using namespace s3d::ImageDrawingTest;

TEST_CASE("ImageDrawingTestHelper.CompositeByteRounding")
{
	// Literal expectations anchor the shared oracle independently of ImagePixel.
	const Color src{ 200, 80, 20, 128 }, dst{ 40, 120, 220, 64 };
	CHECK(Composite(src, dst, 0, false) == dst);
	CHECK(Composite(src, dst, 0, true) == dst);
	CHECK(Composite(src, dst, 128, false) == Color(120, 100, 120, 96));
	CHECK(Composite(src, dst, 128, true) == Color(131, 97, 106, 112));
	CHECK(Composite(src, dst, 255, false) == src);
	CHECK(Composite(src, dst, 255, true) == Color(168, 88, 60, 160));
	CHECK(Composite(src, Color{ 255, 0, 255, 0 }, 128, true) == Color(200, 80, 20, 64));
	CHECK(Composite(Color{ 200, 80, 20, 255 }, Color{ 40, 120, 220, 255 }, 128, true) == Color(120, 100, 120, 255));
	const Color transparent{ 210, 90, 10, 0 };
	CHECK(Composite(transparent, dst, 255, true) == dst);
	CHECK(Composite(transparent, dst, 255, false) == transparent);
	CHECK(Composite(Color{ 200, 80, 20, 1 }, transparent, 127, true) == transparent);
	CHECK(Composite(Color{ 200, 80, 20, 1 }, transparent, 128, true) == Color(200, 80, 20, 1));
}

TEST_CASE("ImageDrawingTestHelper.CompositeMaskOriginAndOrder")
{
	const Color src{ 200, 80, 20, 128 }, dst{ 40, 120, 220, 64 };
	Image mask{ 4, 3, Color{ 255, 0 } };
	mask[1][1] = Color{ 17, 23, 31, 128 };
	mask[1][2] = Color{ 0, 255 };
	mask[2][1] = Color{ 255, 0 };
	mask[2][2] = Color{ 7, 11, 13, 255 };
	Image actual{ 2, 2, dst };
	CompositeMask(actual, mask, src, false, { 1, 1 });
	CHECK(actual[0][0] == Color(120, 100, 120, 96));
	CHECK(actual[0][1] == src);
	CHECK(actual[1][0] == dst);
	CHECK(actual[1][1] == src);

	actual.fill(dst);
	CompositeMask(actual, mask, src, true, { 1, 1 });
	CHECK(actual[0][0] == Color(131, 97, 106, 112));
	CHECK(actual[0][1] == Color(168, 88, 60, 160));
	CHECK(actual[1][0] == dst);
	CHECK(actual[1][1] == Color(168, 88, 60, 160));

	// Repeated calls represent ordered members, so the second uses the updated image.
	Image pixel{ 1, 1, Color{ 0, 0 } }, full{ 1, 1, Color{ 0, 255 } };
	CompositeMask(pixel, full, Color{ 255, 0, 0, 255 }, true);
	CompositeMask(pixel, full, Color{ 0, 0, 255, 128 }, true);
	CHECK(pixel[0][0] == Color(127, 0, 128, 255));
	Image empty;
	CompositeMask(empty, empty, src, true);
	CHECK(empty.isEmpty());
}

TEST_CASE("ImageDrawingTestHelper.SamplePositionsAndCoverage")
{
	Array<Vec2> positions;
	CHECK(SampleCoverage({ -2, 3 }, false, [&](Vec2 p) { positions.push_back(p); return true; }) == 255);
	CHECK(positions == Array<Vec2>{ Vec2{ -1.5, 3.5 } });
	positions.clear();
	CHECK(SampleCoverage({ -2, 3 }, true, [&](Vec2 p) { positions.push_back(p); return false; }) == 0);
	const Array<Vec2> expected{
		{ -1.875, 3.125 }, { -1.625, 3.125 }, { -1.375, 3.125 }, { -1.125, 3.125 },
		{ -1.875, 3.375 }, { -1.625, 3.375 }, { -1.375, 3.375 }, { -1.125, 3.375 },
		{ -1.875, 3.625 }, { -1.625, 3.625 }, { -1.375, 3.625 }, { -1.125, 3.625 },
		{ -1.875, 3.875 }, { -1.625, 3.875 }, { -1.375, 3.875 }, { -1.125, 3.875 } };
	CHECK(positions == expected);
	constexpr std::array<uint8, 17> coverage{ 0, 16, 32, 48, 64, 80, 96, 112, 128, 143, 159, 175, 191, 207, 223, 239, 255 };
	for (int32 count = 0; count <= 16; ++count)
	{
		CAPTURE(count);
		int32 visited = 0;
		CHECK(SampleCoverage({ 0, 0 }, true, [&](Vec2) { return visited++ < count; }) == coverage[count]);
		CHECK(visited == 16);
	}
}

TEST_CASE("ImageDrawingTestHelper.SampleMaskBoundaryAndBackground")
{
	// A 1.5 x 1.5 square covers 1, 1/2, 1/2, and 1/4 of these four pixels.
	const auto contains = [](Vec2 p) { return p.x < 1.5 && p.y < 1.5; };
	const Image aa = SampleMask({ 2, 2 }, true, contains);
	CHECK(aa[0][0] == Color(255, 255));
	CHECK(aa[0][1] == Color(128, 128));
	CHECK(aa[1][0] == Color(128, 128));
	CHECK(aa[1][1] == Color(64, 64));
	const Image noAA = SampleMask({ 2, 2 }, false, contains);
	CHECK(noAA[0][0] == Color(255, 255));
	CHECK(noAA[0][1] == Color(0, 0));
	CHECK(noAA[1][0] == Color(0, 0));
	CHECK(noAA[1][1] == Color(0, 0));
	CHECK(SampleMask({ 0, 0 }, true, contains).isEmpty());
	const Image background = Background({ 3, 2 });
	CHECK(background[0][0] == Color(0, 0, 0, 0));
	CHECK(background[0][1] == Color(73, 31, 17, 23));
	CHECK(background[1][2] == Color(109, 155, 85, 115));
	CHECK(Background({ 0, 0 }).isEmpty());
}

TEST_CASE("ImageDrawingTestHelper.ImageEqualsDiagnostics")
{
	const Image expected{ 2, 2, Color{ 10, 20, 30, 0 } };
	ImageEquals matcher{ expected };
	CHECK(matcher.match(expected));
	CHECK_FALSE(matcher.match(Image{ 1, 4, Color{ 10, 20, 30, 0 } }));
	CHECK(matcher.describe().find("expected image 2x2; actual size 1x4") != std::string::npos);
	for (const Color changed : { Color{ 11, 20, 30, 0 }, Color{ 10, 21, 30, 0 }, Color{ 10, 20, 31, 0 }, Color{ 10, 20, 30, 1 } })
	{
		Image actual = expected;
		actual[1][1] = changed;
		CHECK_FALSE(matcher.match(actual));
		CHECK(matcher.describe().find("first difference at (1, 1)") != std::string::npos);
		CHECK(matcher.describe().find("expected RGBA (10, 20, 30, 0)") != std::string::npos);
	}
	Image actual = expected;
	actual[1][1] = Color{ 40, 50, 60, 70 };
	actual[0][1] = Color{ 80, 90, 100, 110 };
	CHECK_FALSE(matcher.match(actual));
	CHECK(matcher.describe().find("first difference at (1, 0): actual RGBA (80, 90, 100, 110)") != std::string::npos);
	std::ostringstream diagnostic;
	Catch::makeMatchExpr(actual, matcher).streamReconstructedExpression(diagnostic);
	CHECK(diagnostic.str() == "Image(2x2) equals expected image 2x2; first difference at (1, 0): actual RGBA (80, 90, 100, 110), expected RGBA (10, 20, 30, 0)");
	CHECK(matcher.match(expected));
	CHECK(matcher.describe().find("difference") == std::string::npos);
	const Image empty;
	CHECK_THAT(empty, ImageEquals(empty));
	CHECK_FALSE(ImageEquals(empty).match(expected));
}
