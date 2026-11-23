//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <array>
# include <exception>
# include <latch>
# include <thread>

namespace
{
	Image ResizePattern(const Size size, const int32 seed)
	{
		return Image{ size, Arg::generator = [seed](const int32 x, const int32 y)
			{
				const int32 alpha = ((x * 19 + y * 23 + seed * 31) & 255);
				return Color{
					(((x * 13 + y * 7) & 255) * alpha / 255),
					(((x * 5 + y * 17) & 255) * alpha / 255),
					(((x * 11 + y * 3) & 255) * alpha / 255), alpha };
			} };
	}
}

TEST_CASE("ImageProcessing.Resize.constant")
{
	for (const Color color : { Color{ 27, 91, 173, 255 }, Color{ 17, 51, 83, 127 }, Color{ 0, 0, 0, 0 } })
	{
		for (const Size sourceSize : { Size{ 1, 1 }, Size{ 7, 5 }, Size{ 64, 33 } })
		{
			const Image source{ sourceSize, color };
			for (const Size targetSize : { Size{ 1, 1 }, Size{ 3, 2 }, Size{ 13, 11 }, Size{ 64, 33 } })
			{
				CAPTURE(color, sourceSize, targetSize);
				CHECK(ImageProcessing::Resize(source, targetSize) == Image(targetSize, color));
			}
		}
	}
}

TEST_CASE("ImageProcessing.Resize.identity")
{
	for (const Size size : { Size{ 1, 1 }, Size{ 1, 19 }, Size{ 23, 1 }, Size{ 31, 19 } })
	{
		CAPTURE(size);
		const Image source = ResizePattern(size, 7);
		CHECK(ImageProcessing::Resize(source, size) == source);
	}
}

TEST_CASE("ImageProcessing.Resize.gradient")
{
	const Image source{ Size{ 31, 19 }, Arg::generator = [](const int32 x, const int32 y)
		{
			return Color{ (x * 255 / 30), (y * 255 / 18), 73, 255 };
		} };
	const Image original = source;
	for (const Size size : { Size{ 13, 7 }, Size{ 61, 37 }, Size{ 47, 9 }, Size{ 9, 37 } })
	{
		CAPTURE(size);
		const Image resized = ImageProcessing::Resize(source, size);
		REQUIRE(resized.size() == size);
		CHECK(source == original);
		for (int32 y = 0; y < size.y; ++y)
		{
			for (int32 x = 0; x < size.x; ++x)
			{
				const Color pixel = resized[y][x];
				CHECK(pixel.b == 73);
				CHECK(pixel.a == 255);
				if (x > 0)
				{
					CHECK(pixel.r >= resized[y][x - 1].r);
				}
				if (y > 0)
				{
					CHECK(pixel.g >= resized[y - 1][x].g);
				}
			}
		}
		CHECK(resized[size.y / 2][0].r < resized[size.y / 2][size.x - 1].r);
		CHECK(resized[0][size.x / 2].g < resized[size.y - 1][size.x / 2].g);
	}
}

TEST_CASE("ImageProcessing.Resize.premultiplied_alpha")
{
	// A red coverage mask must retain R == A, including partially covered pixels.
	const Image source{ Size{ 17, 13 }, Arg::generator = [](const int32 x, const int32 y)
		{
			const int32 alpha = (((x / 3 + y / 2) % 2) ? 255 : 0);
			return Color{ alpha, 0, 0, alpha };
		} };
	for (const Size size : { Size{ 7, 5 }, Size{ 29, 23 }, Size{ 1, 1 } })
	{
		CAPTURE(size);
		const Image resized = ImageProcessing::Resize(source, size);
		REQUIRE(resized.size() == size);
		bool hasPartialCoverage = false;
		for (const Color pixel : resized)
		{
			CHECK(pixel.r == pixel.a);
			CHECK(pixel.g == 0);
			CHECK(pixel.b == 0);
			hasPartialCoverage |= ((0 < pixel.a) && (pixel.a < 255));
		}
		CHECK(hasPartialCoverage);
	}
}

TEST_CASE("ImageProcessing.Resize.concurrent")
{
	struct Job
	{
		Image source;
		std::array<Size, 3> sizes;
		std::array<Image, 3> expected;
	};

	constexpr size_t ThreadCount = 4;
	std::array<Job, ThreadCount> jobs;
	for (size_t i = 0; i < ThreadCount; ++i)
	{
		const int32 index = static_cast<int32>(i);
		Job& job = jobs[i];
		job.source = ResizePattern(Size{ 127 + index * 8, 95 + index * 6 }, index);
		job.sizes = { Size{ 61 + index, 49 + index * 2 },
			Size{ 181 + index * 6, 137 + index * 4 }, Size{ 31 + index * 2, 159 + index * 4 } };
		for (size_t j = 0; j < job.sizes.size(); ++j)
		{
			job.expected[j] = ImageProcessing::Resize(job.source, job.sizes[j]);
		}
	}

	std::latch start{ 1 };
	std::array<bool, ThreadCount> matches;
	matches.fill(true);
	std::array<std::exception_ptr, ThreadCount> errors;
	std::array<std::jthread, ThreadCount> workers;
	for (size_t i = 0; i < ThreadCount; ++i)
	{
		workers[i] = std::jthread{ [&, i]
			{
				start.wait();
				try
				{
					const Job& job = jobs[i];
					for (size_t repeat = 0; repeat < 16; ++repeat)
					{
						for (size_t j = 0; j < job.sizes.size(); ++j)
						{
							const Image resized = ImageProcessing::Resize(job.source, job.sizes[j]);
							matches[i] &= (resized == job.expected[j]);
						}
					}
				}
				catch (...)
				{
					errors[i] = std::current_exception();
				}
			} };
	}
	start.count_down();
	for (auto& worker : workers)
	{
		worker.join();
	}
	for (size_t i = 0; i < ThreadCount; ++i)
	{
		CAPTURE(i);
		REQUIRE(errors[i] == nullptr);
		CHECK(matches[i]);
	}
}
