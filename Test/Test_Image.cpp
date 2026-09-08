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

TEST_CASE("Image.unpremultiplyAlpha")
{
	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;
	image1.premultiplyAlpha();
	image2.premultiplyAlpha();
	image1.unpremultiplyAlpha(false);
	image2.unpremultiplyAlpha(true);

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

TEST_CASE("Image.unpremultiplyAlpha.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;
	image1.premultiplyAlpha();
	image2.premultiplyAlpha();

	{
		Bench{}.title("Image::unpremultiplyAlpha").run("SIMD off", [&]() { image1.unpremultiplyAlpha(false); });
		Bench{}.title("Image::unpremultiplyAlpha").run("SIMD on", [&]() { image2.unpremultiplyAlpha(true); });
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

TEST_CASE("Image.array_like_contract")
{
	Image image{ 3, 2, Color{ 10, 20, 30 } };
	CHECK_EQ(image.get_if(Point{ 1, 1 }), &image[1, 1]);
	CHECK_EQ(std::as_const(image).get_if(1, 2), &image[1, 2]);
	CHECK_EQ(image.get_if(Point{ -1, 0 }), nullptr);
	CHECK_EQ(image.get_if(Point{ 0, -1 }), nullptr);
	CHECK_EQ(image.get_if(2, 0), nullptr);
	CHECK_EQ(image.get_if(0, 3), nullptr);
	CHECK_EQ(image.get_if(std::numeric_limits<size_t>::max(), 0), nullptr);
	CHECK_EQ(reinterpret_cast<uintptr_t>(image.data()) % Image::DataAlignment, uintptr_t{ 0 });
	image.row(1)[2] = Color{ 40, 50, 60 };
	CHECK_EQ(image[Point{ 2, 1 }], Color{ 40, 50, 60 });
	image.release();
	CHECK_EQ(image.size(), Size{ 0, 0 });
	CHECK_EQ(image.get_if(Point{}), nullptr);
	CHECK(image.isEmpty());
}

# if defined(__cpp_lib_ranges_stride)

namespace
{
	template <class T>
	concept HasImageColumn = requires(T&& image)
	{
		std::forward<T>(image).column(0);
	};

	static_assert(HasImageColumn<Image&>);
	static_assert(HasImageColumn<const Image&>);
	static_assert(not HasImageColumn<Image>);
	static_assert(not HasImageColumn<const Image>);
}

TEST_CASE("Image.column")
{
	Image image{ 3, 2, Color{ 10, 20, 30 } };
	auto firstColumn = image.column(0);
	static_assert(std::same_as<std::ranges::range_reference_t<decltype(firstColumn)>, Color&>);
	CHECK_EQ(std::ranges::distance(firstColumn), 2);
	CHECK_EQ(&firstColumn[0], &image[0, 0]);
	CHECK_EQ(&firstColumn[1], &image[1, 0]);
	firstColumn[1] = Color{ 40, 50, 60 };
	CHECK_EQ(image[1, 0], Color{ 40, 50, 60 });
	CHECK_EQ(image[1, 1], Color{ 10, 20, 30 });

	auto lastColumn = std::as_const(image).column(2);
	static_assert(std::same_as<std::ranges::range_reference_t<decltype(lastColumn)>, const Color&>);
	CHECK_EQ(std::ranges::distance(lastColumn), 2);
	CHECK_EQ(&lastColumn[0], &image[0, 2]);
	CHECK_EQ(&lastColumn[1], &image[1, 2]);

	Image singlePixel{ 1, 1, Color{ 70, 80, 90 } };
	auto singleColumn = singlePixel.column(0);
	CHECK_EQ(std::ranges::distance(singleColumn), 1);
	CHECK_EQ(&singleColumn[0], &singlePixel[0, 0]);
}

# endif
