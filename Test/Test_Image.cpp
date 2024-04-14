//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

# if SIV3D_RUN_BENCHMARK

static Image MakeTestImage()
{
	Image image{ 512, 512, Palette::White };

	for (auto& pixel : image)
	{
		pixel.r = RandomUint8();
		pixel.g = RandomUint8();
		pixel.b = RandomUint8();
		pixel.a = RandomUint8();
	}

	return image;
}

TEST_CASE("Image.premultiplyAlpha.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	const Image testImage = MakeTestImage();
	Image image1 = testImage;
	Image image2 = testImage;
	Image image3 = testImage;
	Image image4 = testImage;

	{
		Bench{}.title("Image::premultiplyAlpha").run("premultiplyAlpha", [&]() { image1.premultiplyAlpha(); });
		Bench{}.title("Image::premultiplyAlpha").run("premultiplyAlpha_sse41", [&]() { image2.premultiplyAlpha_sse41(); });
		Bench{}.title("Image::premultiplyAlpha").run("premultiplyAlpha_avx2", [&]() { image3.premultiplyAlpha_avx2(); });
		Bench{}.title("Image::premultiplyAlpha").run("premultiplyAlpha_gcc", [&]() { image4.premultiplyAlpha_gcc(); });
	}
}

# endif
