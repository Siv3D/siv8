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
# include <Siv3D/2DShapes.hpp>
# include "Siv3DTestFramework.hpp"
using namespace s3d;
# else
# include "Siv3DTest.hpp"
# include <ThirdParty/nanobench/nanobench.h>
# include <sstream>
# endif
# include <limits>
# include <utility>

namespace
{
	template <class T>
	concept HasWholeImageView = requires(T&& image)
	{
		std::forward<T>(image)();
	};

	template <class T, class... Args>
	concept HasImageView = requires(T&& image, Args... args)
	{
		std::forward<T>(image)(args...);
	};

	template <class... Args>
	constexpr bool HasExpectedImageViewOverload = requires(Image& image, const Image& constImage, Args... args)
	{
		{ image(args...) } noexcept -> std::same_as<MutableImageView>;
		{ constImage(args...) } noexcept -> std::same_as<ImageView>;
	} && (not HasImageView<Image, Args...>) && (not HasImageView<const Image, Args...>);

	template <class T>
	concept WritableImageView = requires(T view)
	{
		view.fill(Color{ 1, 2, 3, 4 });
		view[0, 0] = Color{ 1, 2, 3, 4 };
	};

	static_assert(not HasWholeImageView<Image&> && not HasWholeImageView<const Image&>);
	static_assert(not HasWholeImageView<Image> && not HasWholeImageView<const Image>);
	static_assert(HasExpectedImageViewOverload<Rect>);
	static_assert(HasExpectedImageViewOverload<int32, int32, int32>);
	static_assert(HasExpectedImageViewOverload<int32, int32, int32, int32>);
	static_assert(HasExpectedImageViewOverload<Point, int32, int32>);
	static_assert(HasExpectedImageViewOverload<int32, int32, Size>);
	static_assert(HasExpectedImageViewOverload<Point, int32>);
	static_assert(HasExpectedImageViewOverload<Point, Size>);
	static_assert(std::convertible_to<MutableImageView, ImageView>);
	static_assert(not std::convertible_to<ImageView, MutableImageView>);
	static_assert(not std::constructible_from<MutableImageView, const Image&>);
	static_assert(not std::constructible_from<ImageView, const Color*, Size, int32>);
	static_assert(not std::constructible_from<MutableImageView, Color*, Size, int32>);
	static_assert(WritableImageView<MutableImageView> && WritableImageView<const MutableImageView>);
	static_assert(not WritableImageView<ImageView> && not WritableImageView<const ImageView>);
	static_assert(std::same_as<decltype(ImageView{}.row(0)), std::span<const Color>>);
	static_assert(std::same_as<decltype(MutableImageView{}.row(0)), std::span<Color>>);
	static_assert(std::is_trivially_copyable_v<ImageView>);
	static_assert(std::is_trivially_copyable_v<MutableImageView>);

	Image MakeViewTestImage(const Size size)
	{
		return Image{ size, Arg::generator = [](const int32 x, const int32 y)
			{
				return Color{ (x * 31 + y * 17) % 256, (x * 13 + y * 41) % 256,
					(x * 71 + y * 19) % 256, (x * 23 + y * 53) % 256 };
			} };
	}

	// Snapshot oracle: visit destination pixels, without using view/copy helpers.
	void ReferenceOverwrite(const Image& src, const Rect srcRect, Image& dst, const Rect dstRect, const Point pos)
	{
		for (int32 y = 0; y < dstRect.h; ++y)
		{
			for (int32 x = 0; x < dstRect.w; ++x)
			{
				const int64 sx = (static_cast<int64>(x) - pos.x);
				const int64 sy = (static_cast<int64>(y) - pos.y);
				if ((0 <= sx) && (sx < srcRect.w) && (0 <= sy) && (sy < srcRect.h))
				{
					dst[y + dstRect.y, x + dstRect.x] = src[sy + srcRect.y, sx + srcRect.x];
				}
			}
		}
	}
}

TEST_CASE("ImageView.operator_overloads")
{
	Image image = MakeViewTestImage({ 13, 9 });
	struct RegionCase
	{
		Rect input;
		Rect expected;
	};
	const RegionCase rectangles[] = {
		{ { 3, 2, 7, 5 }, { 3, 2, 7, 5 } },
		{ { -2, -1, 7, 5 }, { 0, 0, 5, 4 } },
		{ { 11, 7, 7, 5 }, { 11, 7, 2, 2 } },
		{ { 13, 2, 7, 5 }, { 0, 0, 0, 0 } },
		{ { 3, 2, 0, 5 }, { 0, 0, 0, 0 } },
		{ { 3, 2, 7, -1 }, { 0, 0, 0, 0 } },
	};
	const RegionCase squares[] = {
		{ { 3, 2, 5, 5 }, { 3, 2, 5, 5 } },
		{ { -2, -1, 5, 5 }, { 0, 0, 3, 4 } },
		{ { 11, 7, 5, 5 }, { 11, 7, 2, 2 } },
		{ { 13, 2, 5, 5 }, { 0, 0, 0, 0 } },
		{ { 3, 2, 0, 0 }, { 0, 0, 0, 0 } },
		{ { 3, 2, -1, -1 }, { 0, 0, 0, 0 } },
	};
	const auto check = [&](const ImageView view, const Rect expected)
	{
		CHECK(view.size() == expected.size);
		CHECK(view.data() == (expected.w ? &image[expected.y, expected.x] : nullptr));
		CHECK(view.bytesPerRow() == (expected.w ? image.bytesPerRow() : 0));
	};
	const auto checkOverloads = [&](auto& source)
	{
		for (const auto& [input, expected] : rectangles)
		{
			for (const ImageView view : { source(input.x, input.y, input.w, input.h),
				source(input.pos, input.w, input.h), source(input.x, input.y, input.size), source(input.pos, input.size) })
			{
				check(view, expected);
			}
		}
		for (const auto& [input, expected] : squares)
		{
			check(source(input.x, input.y, input.w), expected);
			check(source(input.pos, input.w), expected);
		}
	};
	checkOverloads(image);
	checkOverloads(std::as_const(image));
}

TEST_CASE("ImageView.access_and_subviews")
{
	Image image = MakeViewTestImage({ 13, 9 });
	const auto view = image(Rect{ 3, 2, 7, 5 });
	const ImageView read = view;
	CHECK(view.size() == Size{ 7, 5 });
	CHECK(view.width() == 7);
	CHECK(view.height() == 5);
	CHECK(view.pixelCount() == 35);
	CHECK(view.bytesPerRow() == image.bytesPerRow());
	CHECK(view.data() == &image[2, 3]);
	CHECK(read.data() == view.data());
	CHECK(read.size() == view.size());
	CHECK(read.bytesPerRow() == view.bytesPerRow());
	CHECK(static_cast<bool>(read));
	CHECK(not view.isEmpty());
	CHECK(view.row(4).size() == 7);
	CHECK(read.row(4).data() == &image[6, 3]);
	CHECK(&view[1][2] == &image[3, 5]);
	CHECK(&read[Point{ 2, 1 }] == &image[3, 5]);
	CHECK(&read[1, 2] == &image[3, 5]);
	view[Point{ 2, 1 }] = Color{ 7, 11, 13, 17 };
	CHECK(image[3, 5] == Color{ 7, 11, 13, 17 });

	const auto nested = view.subview(Rect{ 2, 1, 9, 9 });
	CHECK(nested.size() == Size{ 5, 4 });
	CHECK(nested.data() == &image[3, 5]);
	CHECK(nested.bytesPerRow() == image.bytesPerRow());
	const auto nestedRead = read.subview(Rect{ -2, -1, 4, 3 });
	CHECK(nestedRead.size() == Size{ 2, 2 });
	CHECK(nestedRead.data() == view.data());
	CHECK(std::as_const(image)(Rect{ image.size() }).data() == image.data());
	CHECK(std::as_const(image)(Rect{ 3, 2, 7, 5 }).cloned() == read.cloned());

	// A temporary view can lend a row as long as its Image remains alive.
	auto row = image(Rect{ 1, 1, 3, 2 }).row(1);
	row[2] = Color{ 19, 23, 29, 31 };
	CHECK(image[2, 3] == Color{ 19, 23, 29, 31 });
}

TEST_CASE("ImageView.clipping")
{
	Image image = MakeViewTestImage({ 7, 5 });
	constexpr int32 lo = std::numeric_limits<int32>::min();
	constexpr int32 hi = std::numeric_limits<int32>::max();
	for (const int32 x : { lo, -9, -2, 0, 3, 7, hi })
	{
		for (const int32 y : { lo, -6, -1, 0, 2, 5, hi })
		{
			for (const int32 w : { lo, -1, 0, 1, 4, 9, hi })
			{
				for (const int32 h : { lo, -1, 0, 1, 3, 8, hi })
				{
					CAPTURE(x, y, w, h);
					const auto view = image(Rect{ x, y, w, h });
					const auto read = std::as_const(image)(Rect{ x, y, w, h });
					const int64 left = std::max<int64>(0, x);
					const int64 top = std::max<int64>(0, y);
					const int64 right = std::min<int64>(image.width(), (static_cast<int64>(x) + w));
					const int64 bottom = std::min<int64>(image.height(), (static_cast<int64>(y) + h));
					if ((w <= 0) || (h <= 0) || (right <= left) || (bottom <= top))
					{
						CHECK(view.isEmpty());
						CHECK(view.data() == nullptr);
						CHECK(view.size() == Size{ 0, 0 });
						CHECK(view.bytesPerRow() == 0);
					}
					else
					{
						CHECK(view.size() == Size{ right - left, bottom - top });
						CHECK(view.data() == &image[top, left]);
					}
					CHECK(read.size() == view.size());
					CHECK(read.data() == view.data());
				}
			}
		}
	}
}

TEST_CASE("ImageView.empty")
{
	Image empty;
	Image dst = MakeViewTestImage({ 3, 2 });
	const Image before = dst;
	for (const ImageView view : { ImageView{}, ImageView{ MutableImageView{} }, std::as_const(empty)(Rect{ empty.size() }) })
	{
		CHECK(not view);
		CHECK(view.pixelCount() == 0);
		CHECK(view.data() == nullptr);
		CHECK(view.cloned().isEmpty());
		CHECK(view.subview(Rect{ 0, 0, 1, 1 }).isEmpty());
		view.overwrite(dst);
		CHECK(dst == before);
	}
	empty(Rect{ empty.size() }).fill(Color{ 7, 11, 13 });
	dst(Rect{ dst.size() }).overwrite(empty(Rect{ empty.size() }));
	dst.overwrite(empty, Point{ 0, 0 });
	empty.overwrite(dst, Point{ 0, 0 });
	CHECK(dst == before);
	const auto allocation = dst.data();
	ImageView{}.copyTo(dst);
	CHECK(dst.isEmpty());
	dst.resize(before.size());
	CHECK(dst.data() == allocation);
}

TEST_CASE("ImageView.fill_strided_and_contiguous")
{
	const Color fill{ 201, 103, 47, 13 };
	for (const int32 width : { 1, 2, 7, 8, 9, 15, 16, 17, 31, 32, 33, 65 })
	{
		for (const int32 height : { 1, 2, 5 })
		{
			for (const int32 offset : { 0, 1, 3, 7 })
			{
				CAPTURE(width, height, offset);
				Image image = MakeViewTestImage({ width + offset, height + 2 });
				Image expected = image;
				for (int32 y = 1; y <= height; ++y)
				{
					for (int32 x = offset; x < image.width(); ++x)
					{
						expected[y, x] = fill;
					}
				}
				image(Rect{ offset, 1, width, height }).fill(fill);
				CHECK(image == expected);
				image.fill(fill);
				expected(Rect{ expected.size() }).fill(fill);
				CHECK(image == expected);
				CHECK(image == Image(image.size(), fill));
			}
		}
	}
}

TEST_CASE("ImageView.copy_and_clone_reuse")
{
	const Image src = MakeViewTestImage({ 17, 11 });
	Image dst{ 30, 20, Color{ 1, 2, 3, 4 } };
	const auto allocation = dst.data();
	for (const Rect rect : { Rect{ 0, 0, 17, 11 }, Rect{ 1, 2, 7, 5 }, Rect{ 16, 10, 1, 1 } })
	{
		const auto view = src(rect);
		Image expected{ rect.size };
		ReferenceOverwrite(src, rect, expected, Rect{ Point{ 0, 0 }, rect.size }, Point{ 0, 0 });
		view.copyTo(dst);
		CHECK(dst.data() == allocation);
		CHECK(dst == expected);
		CHECK(view.cloned() == expected);
	}
	Image mutableSrc = src;
	Image clone = mutableSrc(Rect{ 1, 2, 7, 5 }).cloned();
	mutableSrc.fill(Color{ 255, 0, 0, 0 });
	CHECK(clone == src(Rect{ 1, 2, 7, 5 }).cloned());
}

TEST_CASE("ImageView.copy_to_owner")
{
	Image image = MakeViewTestImage({ 13, 9 });
	const auto allocation = image.data();
	const Image before = image;
	image(Rect{ image.size() }).copyTo(image);
	CHECK(image.data() == allocation);
	CHECK(image == before);
	image(Rect{ 2, 3, 7, 5 }).copyTo(image);
	CHECK(image == before(Rect{ 2, 3, 7, 5 }).cloned());
}

TEST_CASE("ImageView.overwrite_clipping_and_strides")
{
	const Image src = MakeViewTestImage({ 13, 9 });
	Image contiguous{ src.size() };
	src.overwrite(contiguous, Point{ 0, 0 });
	CHECK(contiguous == src);
	contiguous.fill(Color{ 0, 0 });
	src(Rect{ src.size() }).overwrite(contiguous);
	CHECK(contiguous == src);
	const Rect srcRect{ 1, 2, 9, 6 };
	const Rect dstRect{ 3, 1, 8, 7 };
	for (const int32 x : { std::numeric_limits<int32>::min(), -10, -3, 0, 2, 8, std::numeric_limits<int32>::max() })
	{
		for (const int32 y : { std::numeric_limits<int32>::min(), -7, -2, 0, 3, 7, std::numeric_limits<int32>::max() })
		{
			CAPTURE(x, y);
			Image dst{ 17, 10, Color{ 123, 45, 67, 89 } };
			Image expected = dst;
			ReferenceOverwrite(src, srcRect, expected, dstRect, Point{ x, y });
			src(srcRect).overwrite(dst(dstRect), Point{ x, y });
			CHECK(dst == expected);

			Image whole{ 17, 10, Color{ 123, 45, 67, 89 } };
			expected = whole;
			ReferenceOverwrite(src, Rect{ Point{ 0, 0 }, src.size() }, expected,
				Rect{ Point{ 0, 0 }, whole.size() }, Point{ x, y });
			src.overwrite(whole, Point{ x, y });
			CHECK(whole == expected);
			whole.fill(Color{ 123, 45, 67, 89 });
			src(Rect{ src.size() }).overwrite(whole, Point{ x, y });
			CHECK(whole == expected);
		}
	}
}

TEST_CASE("ImageView.overwrite_overlap")
{
	const Image original = MakeViewTestImage({ 9, 8 });
	const Rect rectangles[] = { { 0, 0, 9, 8 }, { 1, 1, 7, 6 }, { 0, 2, 8, 5 }, { 4, 0, 5, 8 } };
	for (const Rect srcRect : rectangles)
	{
		for (const Rect dstRect : rectangles)
		{
			for (const int32 x : { -3, -1, 0, 1, 3 })
			{
				for (const int32 y : { -3, -1, 0, 1, 3 })
				{
					CAPTURE(srcRect, dstRect, x, y);
					Image actual = original;
					Image expected = original;
					ReferenceOverwrite(original, srcRect, expected, dstRect, Point{ x, y });
					actual(srcRect).overwrite(actual(dstRect), Point{ x, y });
					CHECK(actual == expected);
				}
			}
		}
	}

	Image actual = original;
	Image expected = original;
	ReferenceOverwrite(original, rectangles[0], expected, rectangles[1], Point{ -1, -1 });
	actual.overwrite(actual(rectangles[1]), Point{ -1, -1 });
	CHECK(actual == expected);
	actual = original;
	actual.overwrite(actual, Point{ 1, 1 });
	CHECK(actual == original); // Preserve the existing owner-to-self no-op.
}

# if !defined(SIV3D_IMAGE_ALLOCATION_TEST)

namespace
{
# if defined(_MSC_VER)
# define SIV3D_IMAGE_VIEW_NOINLINE __declspec(noinline)
# else
# define SIV3D_IMAGE_VIEW_NOINLINE __attribute__((noinline))
# endif

	// Match the previous full-image paths, including the out-of-line row copy.
	SIV3D_IMAGE_VIEW_NOINLINE void BaselineFill(Image& image, const Color color)
	{
		std::fill(image.begin(), image.end(), color);
	}

	SIV3D_IMAGE_VIEW_NOINLINE void BaselineCopyRows(const Color* src, Color* dst,
		const Size size, const int32 srcStride, const int32 dstStride)
	{
		for (int32 y = 0; y < size.y; ++y)
		{
			std::memcpy((dst + static_cast<size_t>(y) * dstStride),
				(src + static_cast<size_t>(y) * srcStride), (size.x * sizeof(Color)));
		}
	}

	SIV3D_IMAGE_VIEW_NOINLINE void BaselineOverwrite(const Image& src, Image& dst, const Point pos)
	{
		if (&src == &dst) { return; }
		const int32 x0 = Max(pos.x, 0), y0 = Max(pos.y, 0);
		const int32 x1 = Min(pos.x + src.width(), dst.width());
		const int32 y1 = Min(pos.y + src.height(), dst.height());
		const int32 w = Max(x1 - x0, 0), h = Max(y1 - y0, 0);
		if (w * h == 0) { return; }
		BaselineCopyRows(&src[Max(0, -pos.y)][Max(0, -pos.x)], &dst[y0][x0],
			Size{ w, h }, src.width(), dst.width());
	}

# undef SIV3D_IMAGE_VIEW_NOINLINE
}

TEST_CASE("ImageView.benchmark.copy_fill", "[.benchmark]")
{
	using ankerl::nanobench::Bench;
	using ankerl::nanobench::doNotOptimizeAway;
	for (const Size size : { Size{ 32, 32 }, Size{ 512, 512 }, Size{ 2048, 1024 } })
	{
		const Image src = MakeViewTestImage(size);
		Image dst{ size };
		const Color color{ 23, 47, 89, 131 };
		std::ostringstream report;
		Bench bench;
		bench.title(std::to_string(size.x) + "x" + std::to_string(size.y))
			.unit("pixel").batch(src.pixelCount()).epochs(11)
			.minEpochTime(std::chrono::milliseconds(2)).warmup(1)
			.performanceCounters(false).output(&report);
		bench.run("fill/previous", [&] { BaselineFill(dst, color); doNotOptimizeAway(dst.data()); });
		bench.run("fill/Image", [&] { dst.fill(color); doNotOptimizeAway(dst.data()); });
		bench.run("fill/view", [&] { dst(Rect{ dst.size() }).fill(color); doNotOptimizeAway(dst.data()); });
		CHECK(dst == Image(size, color));
		bench.run("copy/previous", [&] { BaselineOverwrite(src, dst, Point{ 0, 0 }); doNotOptimizeAway(dst.data()); });
		bench.run("copy/Image", [&] { src.overwrite(dst, Point{ 0, 0 }); doNotOptimizeAway(dst.data()); });
		bench.run("copy/view", [&] { src(Rect{ src.size() }).overwrite(dst(Rect{ dst.size() })); doNotOptimizeAway(dst.data()); });
		CHECK(dst == src);

		const auto read = src(Rect{ 1, 1, size.x - 3, size.y - 2 });
		const auto write = dst(Rect{ 2, 0, size.x - 3, size.y - 2 });
		bench.batch(read.pixelCount());
		bench.run("fill/ROI rows", [&]
		{
			for (int32 y = 0; y < write.height(); ++y) { std::fill(write.row(y).begin(), write.row(y).end(), color); }
			doNotOptimizeAway(dst.data());
		});
		bench.run("fill/ROI view", [&] { write.fill(color); doNotOptimizeAway(dst.data()); });
		bench.run("copy/ROI rows", [&]
		{
			BaselineCopyRows(read.data(), write.data(), read.size(), src.width(), dst.width());
			doNotOptimizeAway(dst.data());
		});
		bench.run("copy/ROI view", [&] { read.overwrite(write); doNotOptimizeAway(dst.data()); });
		std::cout << report.str();
	}
}

# endif
