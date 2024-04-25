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

static Image MakeTestImage(const Size size)
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

static Image MakeTestImage(const int32 size)
{
	return MakeTestImage(Size{ size, size });
}

TEST_CASE("Image.rotate90")
{
	{
		const Image testImage = MakeTestImage(512);
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;

		image1.rotate90();
		image4.rotate90_4();
		image8.rotate90_8();
		image16.rotate90_16();
		CHECK_EQ(image1, image4);
		CHECK_EQ(image1, image8);
		CHECK_EQ(image1, image16);
	}

	{
		const Image testImage = MakeTestImage(Size{ 512, 256 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;

		image1.rotate90();
		image4.rotate90_4();
		image8.rotate90_8();
		image16.rotate90_16();
		CHECK_EQ(image1, image4);
		CHECK_EQ(image1, image8);
		CHECK_EQ(image1, image16);
	}

	{
		const Image testImage = MakeTestImage(Size{ 11, 13 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;

		image1.rotate90();
		image4.rotate90_4();
		image8.rotate90_8();
		image16.rotate90_16();
		CHECK_EQ(image1, image4);
		CHECK_EQ(image1, image8);
		CHECK_EQ(image1, image16);
	}
}

TEST_CASE("Image.rotated90")
{
	{
		const Image testImage = MakeTestImage(512);
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;

		CHECK_EQ(image1.rotated90(), image4.rotated90_4());
		CHECK_EQ(image1.rotated90(), image8.rotated90_8());
		CHECK_EQ(image1.rotated90(), image16.rotated90_16());
	}

	{
		const Image testImage = MakeTestImage(Size{ 512, 256 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;

		CHECK_EQ(image1.rotated90(), image4.rotated90_4());
		CHECK_EQ(image1.rotated90(), image8.rotated90_8());
		CHECK_EQ(image1.rotated90(), image16.rotated90_16());
	}

	{
		const Image testImage = MakeTestImage(Size{ 11, 13 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;

		CHECK_EQ(image1.rotated90(), image4.rotated90_4());
		CHECK_EQ(image1.rotated90(), image8.rotated90_8());
		CHECK_EQ(image1.rotated90(), image16.rotated90_16());
	}
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

TEST_CASE("Image.rotate90.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	{
		const Image testImage = MakeTestImage(512);
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;
		Bench{}.title("Image::rotate90").run("old 512x512", [&]() { image1.rotate90(); doNotOptimizeAway(image1); });
		Bench{}.title("Image::rotate90").run("_4 new 512x512", [&]() { image4.rotate90_4(); doNotOptimizeAway(image4); });
		Bench{}.title("Image::rotate90").run("_8 new 512x512", [&]() { image8.rotate90_8(); doNotOptimizeAway(image8); });
		Bench{}.title("Image::rotate90").run("_16 new 512x512", [&]() { image16.rotate90_16(); doNotOptimizeAway(image16); });
	}

	{
		const Image testImage = MakeTestImage(Size{ 1024, 512 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;
		Bench{}.title("Image::rotate90").run("old 1024x512", [&]() { image1.rotate90(); doNotOptimizeAway(image1); });
		Bench{}.title("Image::rotate90").run("_4 new 1024x512", [&]() { image4.rotate90_4(); doNotOptimizeAway(image4); });
		Bench{}.title("Image::rotate90").run("_8 new 1024x512", [&]() { image8.rotate90_8(); doNotOptimizeAway(image8); });
		Bench{}.title("Image::rotate90").run("_16 new 1024x512", [&]() { image16.rotate90_16(); doNotOptimizeAway(image16); });
	}

	{
		const Image testImage = MakeTestImage(Size{ 999, 1111 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;
		Bench{}.title("Image::rotate90").run("old 999x1111", [&]() { image1.rotate90(); doNotOptimizeAway(image1); });
		Bench{}.title("Image::rotate90").run("_4 new 999x1111", [&]() { image4.rotate90_4(); doNotOptimizeAway(image4); });
		Bench{}.title("Image::rotate90").run("_8 new 999x1111", [&]() { image8.rotate90_8(); doNotOptimizeAway(image8); });
		Bench{}.title("Image::rotate90").run("_16 new 999x1111", [&]() { image16.rotate90_16(); doNotOptimizeAway(image16); });
	}
}

TEST_CASE("Image.rotated90.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	{
		const Image testImage = MakeTestImage(512);
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;
		Bench{}.title("Image::rotated90").run("old 512x512", [&]() { doNotOptimizeAway(image1.rotated90()); });
		Bench{}.title("Image::rotated90").run("_4 new 512x512", [&]() { doNotOptimizeAway(image4.rotated90_4()); });
		Bench{}.title("Image::rotated90").run("_8 new 512x512", [&]() { doNotOptimizeAway(image8.rotated90_8()); });
		Bench{}.title("Image::rotated90").run("_16 new 512x512", [&]() { doNotOptimizeAway(image16.rotated90_16()); });

	}

	{
		const Image testImage = MakeTestImage(Size{ 1024, 512 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;
		Bench{}.title("Image::rotated90").run("old 1024x512", [&]() { doNotOptimizeAway(image1.rotated90()); });
		Bench{}.title("Image::rotated90").run("_4 new 1024x512", [&]() { doNotOptimizeAway(image4.rotated90_4()); });
		Bench{}.title("Image::rotated90").run("_8 new 1024x512", [&]() { doNotOptimizeAway(image8.rotated90_8()); });
		Bench{}.title("Image::rotated90").run("_16 new 1024x512", [&]() { doNotOptimizeAway(image16.rotated90_16()); });
	}

	{
		const Image testImage = MakeTestImage(Size{ 999, 1111 });
		Image image1 = testImage;
		Image image4 = testImage;
		Image image8 = testImage;
		Image image16 = testImage;
		Bench{}.title("Image::rotated90").run("old 999x1111", [&]() { doNotOptimizeAway(image1.rotated90()); });
		Bench{}.title("Image::rotated90").run("_4 new 999x1111", [&]() { doNotOptimizeAway(image4.rotated90_4()); });
		Bench{}.title("Image::rotated90").run("_8 new 999x1111", [&]() { doNotOptimizeAway(image8.rotated90_8()); });
		Bench{}.title("Image::rotated90").run("_16 new 999x1111", [&]() { doNotOptimizeAway(image16.rotated90_16()); });
	}
}

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
