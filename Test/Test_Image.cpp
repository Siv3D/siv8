//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

static Image MakeTestImage(const int32 size)
{
	Image image{ size, Palette::White };

	for (auto& pixel : image)
	{
		pixel.r = RandomUint8();
		pixel.g = RandomUint8();
		pixel.b = RandomUint8();
		pixel.a = RandomUint8();
	}

	return image;
}

TEST_CASE("Image.premultiplyAlpha")
{
	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;

	image1.premultiplyAlpha(false);
	image2.premultiplyAlpha(true);

	CHECK_EQ(image1, image2);
}

TEST_CASE("Image.bgraToRGBA")
{
	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;

	image1.bgraToRGBA(false);
	image2.bgraToRGBA(true);

	CHECK_EQ(image1, image2);
}

# if SIV3D_RUN_BENCHMARK

TEST_CASE("Image.premultiplyAlpha.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;

	{
		Bench{}.title("Image::premultiplyAlpha").run("SIMD off", [&]() { image1.premultiplyAlpha(false); });
		Bench{}.title("Image::premultiplyAlpha").run("SIMD on", [&]() { image2.premultiplyAlpha(true); });
	}
}

TEST_CASE("Image.bgraToRGBA.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;

	{
		Bench{}.title("Image::bgraToRGBA").run("SIMD off", [&]() { image1.bgraToRGBA(false); });
		Bench{}.title("Image::bgraToRGBA").run("SIMD on", [&]() { image2.bgraToRGBA(true); });
	}
}

# endif
