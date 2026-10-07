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

# if defined(SIV3D_IMAGE_ALLOCATION_TEST)
# include <Siv3D/Image.hpp>
# include <Siv3D/CPUInfo.hpp>
# include "Siv3DTestFramework.hpp"
// White-box checks share a translation unit with the unmodified implementation.
// The normal test application links the engine and uses only the public API.
# include "../Siv3D/src/Siv3D/Image/SivImage_SIMD.cpp"
# define SIV3D_RUN_BENCHMARK 0
using namespace s3d;
# else
# include "Siv3DTest.hpp"
# endif
# include <array>

// Enabled only in the isolated macOS checker, never in the engine test app.
// Interpose the allocation boundary instead of adding a production failure hook.
# if defined(SIV3D_IMAGE_ALLOCATION_TEST)
# include <cerrno>
# include <dlfcn.h>

namespace
{
	struct ImageAllocationProbe
	{
		bool fail = false;
		size_t calls = 0;
		size_t alignment = 0;
		size_t bytes = 0;
	};

	thread_local ImageAllocationProbe* g_imageAllocationProbe = nullptr;

	class ScopedImageAllocationProbe
	{
	public:
		explicit ScopedImageAllocationProbe(ImageAllocationProbe& probe) noexcept
		{
			g_imageAllocationProbe = &probe;
		}

		~ScopedImageAllocationProbe()
		{
			g_imageAllocationProbe = nullptr;
		}
	};
}

extern "C" int posix_memalign(void** result, const size_t alignment, const size_t bytes)
{
	using Allocate = int (*)(void**, size_t, size_t);
	static const auto allocate = reinterpret_cast<Allocate>(dlsym(RTLD_NEXT, "posix_memalign"));
	if (auto* probe = g_imageAllocationProbe)
	{
		++probe->calls;
		probe->alignment = alignment;
		probe->bytes = bytes;
		if (probe->fail)
		{
			return ENOMEM;
		}
	}
	return allocate(result, alignment, bytes);
}

# if SIV3D_CPU(X86_64)
// Query the actual CPU without linking the engine's CPUInfo initialization.
bool s3d::SupportsAVX2() noexcept
{
	return __builtin_cpu_supports("avx2");
}
# endif
# endif

static Image MakeTestImage(const int32 size)
{
	Image image{ size, Palette::White };

	uint32 i = 0;
	for (auto& pixel : image)
	{
		pixel = Color{ static_cast<uint8>(i * 73 + 19), static_cast<uint8>(i * 37 + 101),
			static_cast<uint8>(i * 11 + 233), static_cast<uint8>(i * 29) };
		++i;
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

	CHECK((image1) == (image2));
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

	CHECK((image1) == (image2));
}

TEST_CASE("Image.bgraToRGBA")
{
	const Image testImage = MakeTestImage(512);
	Image image1 = testImage;
	Image image2 = testImage;

	image1.bgraToRGBA(false);
	image2.bgraToRGBA(true);

	CHECK((image1) == (image2));
}

namespace
{
	Color ExpectedPixel(Color pixel, const int operation)
	{
		if (operation == 0)
		{
			pixel.r = static_cast<uint8>((pixel.r * pixel.a + 127) / 255);
			pixel.g = static_cast<uint8>((pixel.g * pixel.a + 127) / 255);
			pixel.b = static_cast<uint8>((pixel.b * pixel.a + 127) / 255);
		}
		else if (operation == 1)
		{
			// Compute the rounded Q16 reciprocal independently of the lookup table.
			const uint32 inverse = pixel.a ? ((255u * 65536u + pixel.a / 2u) / pixel.a) : 0;
			auto channel = [inverse](const uint8 value)
			{
				return static_cast<uint8>(Min(255u, (value * inverse + 32768u) >> 16));
			};
			pixel.r = channel(pixel.r);
			pixel.g = channel(pixel.g);
			pixel.b = channel(pixel.b);
		}
		else
		{
			std::swap(pixel.r, pixel.b);
		}
		return pixel;
	}

# if defined(SIV3D_IMAGE_ALLOCATION_TEST)
	using PixelKernel = void (*)(Color*, size_t);

	void CheckPixelKernel(const PixelKernel kernel, const int operation)
	{
		constexpr Color sentinel{ 231, 157, 83, 127 };
		// A whole initialized suffix catches writes into padding even without ASan.
		for (size_t count = 0; count <= 65; ++count)
		{
			for (uint32 alpha = 0; alpha <= 255; ++alpha)
			{
				CAPTURE(count, alpha, operation);
				alignas(Image::DataAlignment) std::array<Color, 80> pixels;
				pixels.fill(sentinel);
				for (size_t i = 0; i < count; ++i)
				{
					pixels[i] = Color{ static_cast<uint8>(i * 73 + alpha),
						static_cast<uint8>(i * 37 + 255 - alpha), static_cast<uint8>(i * 11), alpha };
				}
				auto expected = pixels;
				for (size_t i = 0; i < count; ++i)
				{
					expected[i] = ExpectedPixel(expected[i], operation);
				}
				kernel(pixels.data(), count);
				CHECK((pixels == expected));
			}
		}
		kernel(nullptr, 0);
	}
# endif

	void CheckImageStorage(const Image& image)
	{
		CHECK(static_cast<size_t>(std::distance(image.begin(), image.end())) == image.pixelCount());
		CHECK(image.size_bytes() == image.pixelCount() * sizeof(Color));
		CHECK(image.bytesPerRow() == image.width() * sizeof(Color));
		if (image)
		{
			CHECK(IsAligned(image.data(), Image::DataAlignment));
		}
	}
}

TEST_CASE("Image.pixel_operations.boundaries")
{
	for (int32 count = 0; count <= 65; ++count)
	{
		Image source{ count, 1 };
		Image expected;
		Image actual;
		for (uint32 alpha = 0; alpha <= 255; ++alpha)
		{
			for (int32 i = 0; i < count; ++i)
			{
				source[0][i] = Color{ static_cast<uint8>(i * 73 + alpha),
					static_cast<uint8>(i * 37 + 255 - alpha), static_cast<uint8>(i * 11), alpha };
			}
			for (int operation = 0; operation < 3; ++operation)
			{
				expected = source;
				for (auto& pixel : expected)
				{
					pixel = ExpectedPixel(pixel, operation);
				}
				for (const bool useSIMD : { false, true })
				{
					CAPTURE(count, alpha, operation, useSIMD);
					actual = source;
					if (operation == 0) { actual.premultiplyAlpha(useSIMD); }
					else if (operation == 1) { actual.unpremultiplyAlpha(useSIMD); }
					else { actual.bgraToRGBA(useSIMD); }
					CHECK((actual == expected));
				}
			}
		}
	}
}

# if defined(SIV3D_IMAGE_ALLOCATION_TEST)

TEST_CASE("Image.pixel_kernels.boundaries")
{
	CheckPixelKernel(s3d::PremultiplyAlpha_plain, 0);
	CheckPixelKernel(s3d::UnpremultiplyAlpha_plain, 1);
	CheckPixelKernel(s3d::BGRAtoRGBA_plain, 2);
# if SIV3D_INTRINSIC(SSE)
	// Exercise the fallback explicitly, including on CPUs supporting AVX2.
	CheckPixelKernel(s3d::PremultiplyAlpha_SSE41, 0);
	CheckPixelKernel(s3d::UnpremultiplyAlpha_SSE41, 1);
	CheckPixelKernel(s3d::BGRAtoRGBA_SSSE3, 2);
	if (SupportsAVX2())
	{
		CheckPixelKernel(s3d::PremultiplyAlpha_AVX2, 0);
		CheckPixelKernel(s3d::UnpremultiplyAlpha_AVX2, 1);
		CheckPixelKernel(s3d::BGRAtoRGBA_AVX2, 2);
	}
# elif SIV3D_INTRINSIC(NEON)
	CheckPixelKernel(s3d::BGRAtoRGBA_NEON, 2);
# endif
}

# endif

TEST_CASE("Image.SIMD.after_shrink")
{
	for (int32 count = 0; count <= 65; ++count)
	{
		CAPTURE(count);
		for (int operation = 0; operation < 3; ++operation)
		{
			const Color color{ 231, 157, 83, 127 };
			Image actual{ 80, 1, color };
			actual.resize(count, 1);
			// A copied image has its own allocation; the original retains spare
			// capacity. ASan checks both allocation and vector size boundaries.
			Image copied = actual;
			const Image expected{ count, 1, ExpectedPixel(color, operation) };
			for (Image* image : { &actual, &copied })
			{
				if (operation == 0) { image->premultiplyAlpha(); }
				else if (operation == 1) { image->unpremultiplyAlpha(); }
				else { image->bgraToRGBA(); }
				CHECK((*image == expected));
				CheckImageStorage(*image);
			}
		}
	}
}

TEST_CASE("Image.storage.transitions")
{
	const Color original{ 231, 157, 83, 127 };
	const Color added{ 19, 31, 53, 71 };
	for (const int32 count : { 1, 2, 3, 4, 5, 6, 7, 8, 9, 15, 16, 17, 63, 64, 65 })
	{
		CAPTURE(count);
		Image image{ 3, count, original };
		CheckImageStorage(image);
		Image copied = image;
		CheckImageStorage(copied);
		Image moved = std::move(copied);
		CheckImageStorage(moved);
		CHECK(copied.isEmpty());
		CHECK(copied.size() == Size{ 0, 0 });
		Image assigned{ 3, count * 2, added };
		const Color* const allocation = assigned.data();
		assigned = image;
		CHECK(assigned.data() == allocation);
		CHECK((assigned == image));
		const Image& self = assigned;
		assigned = self;
		CHECK((assigned == image));
		assigned.resizeHeight(count * 2, added);
		CHECK(assigned.data() == allocation);
		for (int32 y = 0; y < count * 2; ++y)
		{
			for (const auto pixel : assigned.row(y))
			{
				CHECK(pixel == ((y < count) ? original : added));
			}
		}
		assigned.resizeHeight(count);
		CHECK((assigned == image));
		assigned.resize(3, count, added);
		CHECK(assigned.data() == allocation);
		CHECK((assigned == Image{ 3, count, added }));
		assigned.shrink_to_fit();
		CheckImageStorage(assigned);
		assigned.swap(moved);
		CHECK((assigned == image));
		CheckImageStorage(assigned);
		CheckImageStorage(moved);
		moved.clear();
		CheckImageStorage(moved);
		moved = std::move(assigned);
		CHECK((moved == image));
		CheckImageStorage(moved);
		moved.release();
		CheckImageStorage(moved);
		CHECK(moved.isEmpty());
	}
}

TEST_CASE("Image.resize.boundaries")
{
	Image image{ 3, 2, Color{ 10, 20, 30, 40 } };
	image.resizeHeight(3);
	CHECK(image[2][0] == Color::Zero());
	image.resizeHeight(0);
	CHECK(image.size() == Size{ 0, 0 });
	image.resizeHeight(2);
	CHECK(image.isEmpty());
	image.resize(Image::MaxWidth, 1);
	CheckImageStorage(image);
	image.resize(1, Image::MaxHeight, Color::Zero());
	CheckImageStorage(image);
	image.resize(Size{ -1, 2 });
	CHECK(image.isEmpty());
	image.resize(Image::MaxWidth + 1, 1);
	CHECK(image.isEmpty());
	image.resize(3, 2);
	image.resizeHeight(Image::MaxHeight + 1);
	CHECK(image.isEmpty());
}

# if defined(SIV3D_IMAGE_ALLOCATION_TEST)

TEST_CASE("Image.allocation.padding")
{
	for (int32 count = 1; count <= 65; ++count)
	{
		CAPTURE(count);
		Image image;
		ImageAllocationProbe probe;
		{
			const ScopedImageAllocationProbe scope{ probe };
			image = Image{ count, 1, Color::Zero() };
		}
		REQUIRE(probe.calls == 1);
		CHECK(probe.alignment == 32);
		CHECK(probe.bytes % 32 == 0);
		CHECK(probe.bytes >= image.size_bytes());
		CheckImageStorage(image);
	}
}

TEST_CASE("Image.allocation.failure_preserves_image")
{
	const Color original{ 231, 157, 83, 127 };
	const Color added{ 19, 31, 53, 71 };
	const Image source{ 65, 7, added };
	for (int operation = 0; operation < 5; ++operation)
	{
		CAPTURE(operation);
		Image image{ 3, 2, original };
		const Image before = image;
		const Color* const allocation = image.data();
		auto mutate = [&]
		{
			if (operation == 0) { image = source; }
			else if (operation == 1) { image.resize(65, 7); }
			else if (operation == 2) { image.resize(65, 7, added); }
			else if (operation == 3) { image.resizeHeight(65); }
			else { image.resizeHeight(65, added); }
		};
		ImageAllocationProbe probe{ .fail = true };
		bool threw = false;
		{
			const ScopedImageAllocationProbe scope{ probe };
			try { mutate(); }
			catch (const std::bad_alloc&) { threw = true; }
		}
		REQUIRE(threw);
		REQUIRE(probe.calls == 1);
		CHECK(image.data() == allocation);
		CHECK((image == before));
		CheckImageStorage(image);
		// Reuse the same object after failure, including a successful allocation.
		mutate();
		CheckImageStorage(image);
		CHECK(image.size() == ((operation < 3) ? Size{ 65, 7 } : Size{ 3, 65 }));
		if (operation == 0 || operation == 2) { CHECK((image == source)); }
	}
}

# endif

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
	CHECK((image.get_if(Point{ 1, 1 })) == (&image[1, 1]));
	CHECK((std::as_const(image).get_if(1, 2)) == (&image[1, 2]));
	CHECK((image.get_if(Point{ -1, 0 })) == (nullptr));
	CHECK((image.get_if(Point{ 0, -1 })) == (nullptr));
	CHECK((image.get_if(2, 0)) == (nullptr));
	CHECK((image.get_if(0, 3)) == (nullptr));
	CHECK((image.get_if(std::numeric_limits<size_t>::max(), 0)) == (nullptr));
	CHECK((reinterpret_cast<uintptr_t>(image.data()) % Image::DataAlignment) == (uintptr_t{ 0 }));
	image.row(1)[2] = Color{ 40, 50, 60 };
	CHECK((image[Point{ 2, 1 }]) == (Color{ 40, 50, 60 }));
	image.release();
	CHECK((image.size()) == (Size{ 0, 0 }));
	CHECK((image.get_if(Point{})) == (nullptr));
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
	CHECK((std::ranges::distance(firstColumn)) == (2));
	CHECK((&firstColumn[0]) == (&image[0, 0]));
	CHECK((&firstColumn[1]) == (&image[1, 0]));
	firstColumn[1] = Color{ 40, 50, 60 };
	CHECK((image[1, 0]) == (Color{ 40, 50, 60 }));
	CHECK((image[1, 1]) == (Color{ 10, 20, 30 }));

	auto lastColumn = std::as_const(image).column(2);
	static_assert(std::same_as<std::ranges::range_reference_t<decltype(lastColumn)>, const Color&>);
	CHECK((std::ranges::distance(lastColumn)) == (2));
	CHECK((&lastColumn[0]) == (&image[0, 2]));
	CHECK((&lastColumn[1]) == (&image[1, 2]));

	Image singlePixel{ 1, 1, Color{ 70, 80, 90 } };
	auto singleColumn = singlePixel.column(0);
	CHECK((std::ranges::distance(singleColumn)) == (1));
	CHECK((&singleColumn[0]) == (&singlePixel[0, 0]));
}

# endif
