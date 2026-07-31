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

namespace
{
	class OneByteReader final : public IReader
	{
	public:

		OneByteReader(const uint8* const data, const size_t size) noexcept
			: m_data{ data }
			, m_size{ static_cast<int64>(size) } {}

		[[nodiscard]]
		bool supportsLookahead() const noexcept override
		{
			return false;
		}

		[[nodiscard]]
		bool isOpen() const noexcept override
		{
			return true;
		}

		[[nodiscard]]
		int64 size() const override
		{
			return m_size;
		}

		[[nodiscard]]
		int64 getPos() const override
		{
			return m_pos;
		}

		int64 setPos(const int64 pos) override
		{
			if (pos == m_pos)
			{
				return m_pos;
			}

			return -1;
		}

		int64 skip(const int64 offset) override
		{
			if ((offset < 0) || ((m_size - m_pos) < offset))
			{
				return -1;
			}

			m_pos += offset;
			return m_pos;
		}

		int64 read(void* const dst, const int64 size) override
		{
			if ((size <= 0) || (m_size <= m_pos))
			{
				return 0;
			}

			std::memcpy(dst, (m_data + m_pos), 1);
			++m_pos;
			return 1;
		}

		int64 read(void*, int64, int64) override
		{
			return 0;
		}

		int64 lookahead(void*, int64) const override
		{
			return 0;
		}

		int64 lookahead(void*, int64, int64) const override
		{
			return 0;
		}

	private:

		const uint8* m_data = nullptr;

		int64 m_size = 0;

		int64 m_pos = 0;
	};

	static constexpr std::array<uint8, 85> AnimatedGIF =
	{
		'G', 'I', 'F', '8', '9', 'a',
		0x02, 0x00, 0x01, 0x00, 0xF0, 0x00, 0x00,
		0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF,
		0x21, 0xFF, 0x0B, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0',
		0x03, 0x01, 0x02, 0x00, 0x00,
		0x21, 0xF9, 0x04, 0x04, 0x0A, 0x00, 0x00, 0x00,
		0x2C, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00,
		0x02, 0x02, 0x04, 0x51, 0x00,
		0x21, 0xF9, 0x04, 0x04, 0x14, 0x00, 0x00, 0x00,
		0x2C, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00,
		0x02, 0x02, 0x0C, 0x53, 0x00,
		0x3B,
	};

	static constexpr std::array<uint8, 196> AnimatedPNG =
	{
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
		0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00, 0x00, 0xF4, 0x22, 0x7F,
		0x8A, 0x00, 0x00, 0x00, 0x08, 0x61, 0x63, 0x54, 0x4C, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
		0x03, 0x6A, 0x84, 0xC2, 0xCA, 0x00, 0x00, 0x00, 0x1A, 0x66, 0x63, 0x54, 0x4C, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x0A, 0x00, 0x00, 0xF9, 0x29, 0xB6, 0x79, 0x00, 0x00, 0x00, 0x0E, 0x49,
		0x44, 0x41, 0x54, 0x78, 0x9C, 0x63, 0xF8, 0xCF, 0xC0, 0xF0, 0x1F, 0x84, 0x01, 0x11, 0xF7, 0x03,
		0xFD, 0xE3, 0xC5, 0xF5, 0xEF, 0x00, 0x00, 0x00, 0x1A, 0x66, 0x63, 0x54, 0x4C, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x0A, 0x01, 0x00, 0x5E, 0x39, 0x4E, 0x55, 0x00, 0x00, 0x00, 0x11, 0x66,
		0x64, 0x41, 0x54, 0x00, 0x00, 0x00, 0x02, 0x78, 0x9C, 0x63, 0x60, 0x60, 0xF8, 0xFF, 0x1F, 0x00,
		0x03, 0x02, 0x01, 0xFF, 0xF5, 0x7B, 0xA5, 0xD7, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44,
		0xAE, 0x42, 0x60, 0x82,
	};

	static constexpr std::array<uint8, 328> CompositedPNG =
	{
		0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52,
		0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x08, 0x06, 0x00, 0x00, 0x00, 0xF4, 0x22, 0x7F,
		0x8A, 0x00, 0x00, 0x00, 0x08, 0x61, 0x63, 0x54, 0x4C, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
		0x03, 0xE5, 0xC4, 0x37, 0x6A, 0x00, 0x00, 0x00, 0x1A, 0x66, 0x63, 0x54, 0x4C, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x0A, 0x00, 0x00, 0xF9, 0x29, 0xB6, 0x79, 0x00, 0x00, 0x00, 0x0E, 0x49,
		0x44, 0x41, 0x54, 0x78, 0x9C, 0x63, 0xF8, 0xCF, 0xC0, 0xF0, 0x1F, 0x84, 0x01, 0x11, 0xF7, 0x03,
		0xFD, 0xE3, 0xC5, 0xF5, 0xEF, 0x00, 0x00, 0x00, 0x1A, 0x66, 0x63, 0x54, 0x4C, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x02, 0x00, 0x0A, 0x01, 0x00, 0x5E, 0x39, 0x4E, 0x55, 0x00, 0x00, 0x00, 0x11, 0x66,
		0x64, 0x41, 0x54, 0x00, 0x00, 0x00, 0x02, 0x78, 0x9C, 0x63, 0x60, 0x60, 0xF8, 0xFF, 0x1F, 0x00,
		0x03, 0x02, 0x01, 0xFF, 0xF5, 0x7B, 0xA5, 0xD7, 0x00, 0x00, 0x00, 0x1A, 0x66, 0x63, 0x54, 0x4C,
		0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x0A, 0x02, 0x01, 0x13, 0x6B, 0x08, 0x99, 0x00, 0x00,
		0x00, 0x11, 0x66, 0x64, 0x41, 0x54, 0x00, 0x00, 0x00, 0x04, 0x78, 0x9C, 0x63, 0x60, 0xF8, 0xCF,
		0xD0, 0x00, 0x00, 0x03, 0x82, 0x01, 0x80, 0x19, 0x06, 0xDA, 0x35, 0x00, 0x00, 0x00, 0x1A, 0x66,
		0x63, 0x54, 0x4C, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x0A, 0x00, 0x00, 0xC8, 0x3E, 0x2B,
		0x27, 0x00, 0x00, 0x00, 0x0F, 0x66, 0x64, 0x41, 0x54, 0x00, 0x00, 0x00, 0x06, 0x78, 0x9C, 0x63,
		0xF8, 0x0F, 0x04, 0x00, 0x09, 0xFB, 0x03, 0xFD, 0x7E, 0xF9, 0x9D, 0xCB, 0x00, 0x00, 0x00, 0x00,
		0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82,
	};

	template <size_t Size>
	[[nodiscard]]
	static std::unique_ptr<IReader> MakeReader(const std::array<uint8, Size>& bytes)
	{
		return std::make_unique<MemoryReader>(bytes.data(), bytes.size());
	}
}

TEST_CASE("GIFDecoder")
{
	GIFDecoder decoder;
	uint8 header[IImageDecoder::RequiredHeaderBytes]{};
	std::memcpy(header, AnimatedGIF.data(), sizeof(header));

	CHECK_EQ(decoder.name(), U"GIF");
	CHECK_EQ(decoder.imageFormat(), ImageFormat::GIF);
	CHECK(decoder.isHeader(header));

	MemoryReader infoReader{ AnimatedGIF.data(), AnimatedGIF.size() };
	const Optional<ImageInfo> info = decoder.getImageInfo(infoReader);
	REQUIRE(info);
	CHECK_EQ(info->size, Size{ 2, 1 });
	CHECK_EQ(info->imageFormat, ImageFormat::GIF);
	CHECK_FALSE(info->isAnimated);

	const Image image = decoder.decode(
		MakeReader(AnimatedGIF),
		{},
		PremultiplyAlpha::No);
	REQUIRE(image);
	CHECK_EQ(image.size(), Size{ 2, 1 });
	CHECK_EQ(image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(image[0][1], Color{ 255, 0, 0 });

	const Image shortReads = decoder.decode(
		std::make_unique<OneByteReader>(
			AnimatedGIF.data(),
			AnimatedGIF.size()),
		{},
		PremultiplyAlpha::No);
	REQUIRE(shortReads);
	CHECK_EQ(shortReads.size(), Size{ 2, 1 });
}

TEST_CASE("DecodeAnimatedImage GIF")
{
	const AnimatedImageDecodeResult result = DecodeAnimatedImage(MakeReader(AnimatedGIF));
	REQUIRE(result);
	CHECK_EQ(result.image.size(), size_t{ 2 });
	CHECK_EQ(result.image.playCount, uint32{ 3 });
	CHECK_EQ(result.image.frames[0].duration.count(), doctest::Approx(0.1));
	CHECK_EQ(result.image.frames[1].duration.count(), doctest::Approx(0.2));
	CHECK_EQ(result.image.frames[0].image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(result.image.frames[1].image[0][0], Color{ 0, 0, 255 });

	const AnimatedImageDecodeResult shortReads = DecodeAnimatedImage(
		std::make_unique<OneByteReader>(
			AnimatedGIF.data(),
			AnimatedGIF.size()));
	REQUIRE(shortReads);
	CHECK_EQ(shortReads.image.size(), size_t{ 2 });

	AnimatedImageDecodeOptions frameLimit;
	frameLimit.maxFrames = 1;
	const AnimatedImageDecodeResult limited = DecodeAnimatedImage(
		MakeReader(AnimatedGIF),
		frameLimit);
	CHECK_FALSE(limited);
	CHECK_EQ(limited.error, AnimatedImageDecodeError::TooManyFrames);
	CHECK(limited.image.isEmpty());

	AnimatedImageDecodeOptions workingMemoryLimit;
	workingMemoryLimit.maxWorkingMemoryBytes = 1;
	const AnimatedImageDecodeResult workingMemoryLimited = DecodeAnimatedImage(
		MakeReader(AnimatedGIF),
		workingMemoryLimit);
	CHECK_FALSE(workingMemoryLimited);
	CHECK_EQ(workingMemoryLimited.error, AnimatedImageDecodeError::WorkingMemoryLimitExceeded);
	CHECK(workingMemoryLimited.image.isEmpty());

	const AnimatedImageDecodeResult invalid = DecodeAnimatedImage(
		std::make_unique<MemoryReader>(
			AnimatedGIF.data(),
			(AnimatedGIF.size() - 5)));
	CHECK_FALSE(invalid);
}

TEST_CASE("DecodeAnimatedImage APNG")
{
	const AnimatedImageDecodeResult result = DecodeAnimatedImage(MakeReader(AnimatedPNG));
	REQUIRE(result);
	CHECK_EQ(result.image.size(), size_t{ 2 });
	CHECK_EQ(result.image.playCount, uint32{ 3 });
	CHECK_EQ(result.image.frames[0].duration.count(), doctest::Approx(0.1));
	CHECK_EQ(result.image.frames[1].duration.count(), doctest::Approx(0.2));
	CHECK_EQ(result.image.frames[0].image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(result.image.frames[0].image[0][1], Color{ 255, 0, 0 });
	CHECK_EQ(result.image.frames[1].image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(result.image.frames[1].image[0][1], Color{ 0, 0, 255 });

	const AnimatedImageDecodeResult shortReads = DecodeAnimatedImage(
		std::make_unique<OneByteReader>(
			AnimatedPNG.data(),
			AnimatedPNG.size()));
	REQUIRE(shortReads);
	CHECK_EQ(shortReads.image.size(), size_t{ 2 });

	const AnimatedImageDecodeResult composited = DecodeAnimatedImage(MakeReader(CompositedPNG));
	REQUIRE(composited);
	REQUIRE_EQ(composited.image.size(), size_t{ 4 });
	CHECK_EQ(composited.image.frames[0].image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(composited.image.frames[1].image[0][1], Color{ 0, 0, 255 });
	CHECK_EQ(composited.image.frames[2].image[0][0], Color{ 127, 128, 0 });
	CHECK_EQ(composited.image.frames[2].image[0][1], Color{ 0, 0 });
	CHECK_EQ(composited.image.frames[3].image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(composited.image.frames[3].image[0][1], Color{ 255, 255, 255 });

	auto consecutiveRestorePreviousPNG = CompositedPNG;
	consecutiveRestorePreviousPNG[149] = 2;
	consecutiveRestorePreviousPNG[151] = 0x75;
	consecutiveRestorePreviousPNG[152] = 0x14;
	consecutiveRestorePreviousPNG[153] = 0x1D;
	consecutiveRestorePreviousPNG[154] = 0x96;
	const AnimatedImageDecodeResult consecutiveRestorePrevious =
		DecodeAnimatedImage(MakeReader(consecutiveRestorePreviousPNG));
	REQUIRE(consecutiveRestorePrevious);
	REQUIRE_EQ(consecutiveRestorePrevious.image.size(), size_t{ 4 });
	CHECK_EQ(
		consecutiveRestorePrevious.image.frames[2].image[0][0],
		Color{ 127, 128, 0 });
	CHECK_EQ(
		consecutiveRestorePrevious.image.frames[2].image[0][1],
		Color{ 255, 0, 0 });
	CHECK_EQ(
		consecutiveRestorePrevious.image.frames[3].image[0][0],
		Color{ 255, 0, 0 });
	CHECK_EQ(
		consecutiveRestorePrevious.image.frames[3].image[0][1],
		Color{ 255, 255, 255 });

	auto staticPNG = AnimatedPNG;
	std::memcpy((staticPNG.data() + 37), "tEXt", 4);
	const AnimatedImageDecodeResult notAnimated = DecodeAnimatedImage(MakeReader(staticPNG));
	CHECK_FALSE(notAnimated);
	CHECK_EQ(notAnimated.error, AnimatedImageDecodeError::NotAnimated);

	auto interlacedPNG = AnimatedPNG;
	interlacedPNG[28] = 1;
	interlacedPNG[29] = 0x83;
	interlacedPNG[30] = 0x25;
	interlacedPNG[31] = 0x4F;
	interlacedPNG[32] = 0x1C;
	const AnimatedImageDecodeResult interlaced = DecodeAnimatedImage(MakeReader(interlacedPNG));
	CHECK_FALSE(interlaced);
	CHECK_EQ(interlaced.error, AnimatedImageDecodeError::UnsupportedFeature);

	AnimatedImageDecodeOptions byteLimit;
	byteLimit.maxDecodedBytes = 8;
	const AnimatedImageDecodeResult limited = DecodeAnimatedImage(
		MakeReader(AnimatedPNG),
		byteLimit);
	CHECK_FALSE(limited);
	CHECK_EQ(limited.error, AnimatedImageDecodeError::DecodedBytesLimitExceeded);
	CHECK(limited.image.isEmpty());

	AnimatedImageDecodeOptions frameLimit;
	frameLimit.maxFrames = 1;
	const AnimatedImageDecodeResult tooManyFrames = DecodeAnimatedImage(
		MakeReader(AnimatedPNG),
		frameLimit);
	CHECK_FALSE(tooManyFrames);
	CHECK_EQ(tooManyFrames.error, AnimatedImageDecodeError::TooManyFrames);
	CHECK(tooManyFrames.image.isEmpty());
}

TEST_CASE("AnimatedImageReader GIF")
{
	AnimatedImageReader empty;
	CHECK_FALSE(empty);
	CHECK_EQ(empty.imageSize(), Size{ 0, 0 });
	CHECK_EQ(empty.playCount(), uint32{ 0 });
	CHECK_EQ(empty.error(), AnimatedImageDecodeError::None);
	AnimatedImageFrame emptyFrame;
	CHECK_EQ(empty.readFrame(emptyFrame), AnimatedImageReadStatus::Error);
	CHECK_EQ(empty.error(), AnimatedImageDecodeError::ReadError);
	empty.close();
	CHECK_EQ(empty.error(), AnimatedImageDecodeError::None);

	AnimatedImageReader reader{ MemoryReader{ AnimatedGIF.data(), AnimatedGIF.size() } };
	REQUIRE(reader);
	CHECK_EQ(reader.imageSize(), Size{ 2, 1 });
	CHECK_EQ(reader.playCount(), uint32{ 3 });
	CHECK_EQ(reader.error(), AnimatedImageDecodeError::None);

	AnimatedImageFrame frame;
	frame.image.resize(2, 1);
	const Color* const imageData = frame.image.data();
	CHECK_EQ(reader.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image.data(), imageData);
	CHECK_EQ(frame.duration.count(), doctest::Approx(0.1));
	CHECK_EQ(frame.image[0][0], Color{ 255, 0, 0 });

	CHECK_EQ(reader.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image.data(), imageData);
	CHECK_EQ(frame.duration.count(), doctest::Approx(0.2));
	CHECK_EQ(frame.image[0][0], Color{ 0, 0, 255 });

	const AnimatedImageReadResult end = reader.readFrame();
	CHECK_FALSE(end);
	CHECK_EQ(end.status, AnimatedImageReadStatus::EndOfStream);
	CHECK_EQ(end.error, AnimatedImageDecodeError::None);
	CHECK(reader);

	AnimatedImageReader valueReader{ MakeReader(AnimatedGIF) };
	const AnimatedImageReadResult first = valueReader.readFrame();
	REQUIRE(first);
	CHECK_EQ(first.status, AnimatedImageReadStatus::Frame);
	CHECK_EQ(first.error, AnimatedImageDecodeError::None);
	CHECK_EQ(first.frame.image[0][0], Color{ 255, 0, 0 });

	AnimatedImageReader movedReader{ std::move(valueReader) };
	CHECK_FALSE(valueReader);
	REQUIRE(movedReader);
	const AnimatedImageReadResult movedSecond = movedReader.readFrame();
	REQUIRE(movedSecond);
	CHECK_EQ(movedSecond.frame.image[0][0], Color{ 0, 0, 255 });
	CHECK(valueReader.open(
		MemoryReader{ AnimatedPNG.data(), AnimatedPNG.size() }));
	CHECK(valueReader);
	CHECK_EQ(valueReader.imageSize(), Size{ 2, 1 });
	CHECK_EQ(valueReader.playCount(), uint32{ 3 });

	AnimatedImageDecodeOptions streamByteLimit;
	streamByteLimit.maxDecodedBytes = 8;
	AnimatedImageReader byteLimitedReader{
		MakeReader(AnimatedGIF),
		streamByteLimit
	};
	REQUIRE(byteLimitedReader);
	CHECK_EQ(
		byteLimitedReader.readFrame(frame),
		AnimatedImageReadStatus::Frame);
	CHECK_EQ(
		byteLimitedReader.readFrame(frame),
		AnimatedImageReadStatus::Frame);
	CHECK_EQ(
		byteLimitedReader.readFrame(frame),
		AnimatedImageReadStatus::EndOfStream);

	AnimatedImageDecodeOptions insufficientByteLimit;
	insufficientByteLimit.maxDecodedBytes = 7;
	AnimatedImageReader insufficientByteLimitedReader{
		MakeReader(AnimatedGIF),
		insufficientByteLimit
	};
	CHECK_FALSE(insufficientByteLimitedReader);
	CHECK_EQ(
		insufficientByteLimitedReader.error(),
		AnimatedImageDecodeError::DecodedBytesLimitExceeded);

	AnimatedImageDecodeOptions frameLimit;
	frameLimit.maxFrames = 1;
	AnimatedImageReader frameLimitedReader{
		MakeReader(AnimatedGIF),
		frameLimit
	};
	REQUIRE(frameLimitedReader);
	CHECK_EQ(
		frameLimitedReader.readFrame(frame),
		AnimatedImageReadStatus::Frame);
	const Color previousPixel = frame.image[0][0];
	CHECK_EQ(
		frameLimitedReader.readFrame(frame),
		AnimatedImageReadStatus::Error);
	CHECK_EQ(frameLimitedReader.error(), AnimatedImageDecodeError::TooManyFrames);
	CHECK_EQ(frame.image[0][0], previousPixel);
	CHECK_FALSE(frameLimitedReader);

	AnimatedImageReader truncated{
		std::make_unique<MemoryReader>(
			AnimatedGIF.data(),
			(AnimatedGIF.size() - 5))
	};
	REQUIRE(truncated);
	CHECK_EQ(truncated.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(truncated.readFrame(frame), AnimatedImageReadStatus::Error);
	CHECK_NE(truncated.error(), AnimatedImageDecodeError::None);
}

TEST_CASE("AnimatedImageReader APNG")
{
	AnimatedImageReader reader{
		std::make_unique<OneByteReader>(
			AnimatedPNG.data(),
			AnimatedPNG.size())
	};
	REQUIRE(reader);
	CHECK_EQ(reader.imageSize(), Size{ 2, 1 });
	CHECK_EQ(reader.playCount(), uint32{ 3 });

	AnimatedImageFrame frame;
	frame.image.resize(2, 1);
	const Color* const imageData = frame.image.data();
	CHECK_EQ(reader.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image.data(), imageData);
	CHECK_EQ(frame.image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(frame.image[0][1], Color{ 255, 0, 0 });

	const AnimatedImageReadResult second = reader.readFrame();
	REQUIRE(second);
	CHECK_EQ(second.frame.image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(second.frame.image[0][1], Color{ 0, 0, 255 });
	CHECK_EQ(reader.readFrame(frame), AnimatedImageReadStatus::EndOfStream);

	AnimatedImageReader composited{ MakeReader(CompositedPNG) };
	REQUIRE(composited);
	CHECK_EQ(composited.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(composited.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image[0][1], Color{ 0, 0, 255 });
	CHECK_EQ(composited.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image[0][0], Color{ 127, 128, 0 });
	CHECK_EQ(frame.image[0][1], Color{ 0, 0 });
	CHECK_EQ(composited.readFrame(frame), AnimatedImageReadStatus::Frame);
	CHECK_EQ(frame.image[0][0], Color{ 255, 0, 0 });
	CHECK_EQ(frame.image[0][1], Color{ 255, 255, 255 });
	CHECK_EQ(
		composited.readFrame(frame),
		AnimatedImageReadStatus::EndOfStream);

	auto staticPNG = AnimatedPNG;
	std::memcpy((staticPNG.data() + 37), "tEXt", 4);
	AnimatedImageReader notAnimated{ MakeReader(staticPNG) };
	CHECK_FALSE(notAnimated);
	CHECK_EQ(notAnimated.error(), AnimatedImageDecodeError::NotAnimated);

	const std::array<uint8, 8> invalidData{};
	AnimatedImageReader invalid{ MakeReader(invalidData) };
	CHECK_FALSE(invalid);
	CHECK_EQ(invalid.error(), AnimatedImageDecodeError::InvalidFormat);
}
