//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <limits>
# include <numeric>

namespace
{
	constexpr size_t DDSHeaderBytes = (4 + 124);

	// Expose the original file size even if reads stop early.
	class ChunkReader final : public MemoryViewReader
	{
	public:

		ChunkReader(const Blob& dds, const size_t readableBytes)
			: MemoryViewReader{ dds.data(), readableBytes }
			, m_size{ static_cast<int64>(dds.size()) } {}

		int64 size() const override { return m_size; }

		int64 read(void* dst, const int64 bytes) override
		{
			return MemoryViewReader::read(dst, Min<int64>(bytes, 7));
		}

		int64 read(void* dst, const int64 pos, const int64 bytes) override
		{
			return MemoryViewReader::read(dst, pos, Min<int64>(bytes, 7));
		}

	private:

		int64 m_size;
	};

	void SetMipCount(Blob& dds, const uint32 count)
	{
		// dwMipMapCount is a little-endian DWORD at file offset 28.
		for (size_t i = 0; i < 4; ++i)
		{
			dds.data()[28 + i] = static_cast<Byte>((count >> (8 * i)) & 0xFF);
		}
	}

	Image DecodeImage(const Blob& dds)
	{
		return BCnDecoder{}.decode(std::make_unique<MemoryViewReader>(dds.data(), dds.size()), {}, PremultiplyAlpha::No);
	}

	BCnData DecodeNative(const Blob& dds, const bool sRGB = false)
	{
		return BCnDecoder{}.decodeNative(std::make_unique<MemoryViewReader>(dds.data(), dds.size()), sRGB);
	}
}

TEST_CASE("BCnDecoder.complete_base_required")
{
	const ScopedLogSilencer silence;
	const Image source{ 12, 8, Palette::White }; // Two rows of compressed blocks.
	const BCnEncoder encoder;
	const std::pair<TextureFormat, Blob> files[] = {
		{ TextureFormat::BC1_RGBA_Unorm, encoder.encodeBC1(source, GenerateMipmaps::No) },
		{ TextureFormat::BC3_RGBA_Unorm, encoder.encodeBC3(source, GenerateMipmaps::No) },
		{ TextureFormat::BC4_R_Unorm, encoder.encodeBC4(source, GenerateMipmaps::No) },
		{ TextureFormat::BC5_RG_Unorm, encoder.encodeBC5(source, GenerateMipmaps::No) },
		{ TextureFormat::BC7_RGBA_Unorm, encoder.encodeBC7(source, GenerateMipmaps::No) },
	};

	for (const auto& [format, dds] : files)
	{
		CAPTURE(format.value());
		REQUIRE(dds);
		CHECK(DecodeImage(dds).size() == source.size());
		const BCnData native = DecodeNative(dds);
		REQUIRE(native);
		CHECK(native.format == format);
		REQUIRE(native.textures.size() == 1);
		CHECK(native.textures[0].size() == format.packedDataSize(source.size()));

		const Blob shortBase = dds.slice(0, dds.size() - 1);
		CHECK_FALSE(DecodeImage(shortBase));
		CHECK_FALSE(DecodeNative(shortBase));
	}
}

TEST_CASE("BCnDecoder.declared_mip_chain")
{
	const ScopedLogSilencer silence;
	const Image source{ 12, 8, Palette::Red };
	Blob dds = BCnEncoder{}.encodeBC1(source);
	// 12x8 -> 6x4 -> 3x2 -> 1x1. Each BC1 block occupies 8 bytes.
	constexpr std::array<size_t, 4> LevelBytes{ 48, 16, 8, 8 };
	REQUIRE(dds.size() == (DDSHeaderBytes + 48 + 16 + 8 + 8));
	const Image base = DecodeImage(dds);
	REQUIRE(base.size() == source.size());

	SECTION("Declared prefix through any level, including an implicit base")
	{
		for (uint32 count = 0; count <= LevelBytes.size(); ++count)
		{
			CAPTURE(count);
			const uint32 levels = Max(count, 1u);
			const size_t end = DDSHeaderBytes + std::accumulate(LevelBytes.begin(), LevelBytes.begin() + levels, size_t{ 0 });
			Blob prefix = dds.slice(0, end);
			SetMipCount(prefix, count);
			CHECK(DecodeImage(prefix) == base);
			const BCnData native = DecodeNative(prefix);
			REQUIRE(native);
			CHECK(native.size == source.size());
			REQUIRE(native.textures.size() == levels);
			size_t offset = DDSHeaderBytes;
			for (uint32 level = 0; level < levels; ++level)
			{
				CHECK(native.textures[level] == dds.slice(offset, LevelBytes[level]));
				offset += LevelBytes[level];
			}
		}
	}

	SECTION("Missing byte in each declared lower mip")
	{
		size_t end = (DDSHeaderBytes + LevelBytes[0]);
		for (uint32 level = 1; level < LevelBytes.size(); ++level)
		{
			CAPTURE(level);
			end += LevelBytes[level];
			Blob truncated = dds.slice(0, end - 1);
			SetMipCount(truncated, level + 1);
			CHECK(DecodeImage(truncated) == base);
			CHECK_FALSE(DecodeNative(truncated));
		}
	}

	SECTION("Declared count beyond 1x1")
	{
		dds.resize(dds.size() + 8); // Enough bytes for one more block.
		for (const uint32 count : { 5u, std::numeric_limits<uint32>::max() })
		{
			CAPTURE(count);
			SetMipCount(dds, count);
			CHECK_FALSE(DecodeImage(dds));
			CHECK_FALSE(DecodeNative(dds));
		}
	}

	SECTION("Wrong DDS signature")
	{
		dds.data()[0] = Byte{ 0 };
		CHECK_FALSE(DecodeImage(dds));
		CHECK_FALSE(DecodeNative(dds));
	}

	SECTION("Partial reads and early end despite an advertised complete file")
	{
		const size_t baseEnd = (DDSHeaderBytes + LevelBytes[0]);
		const BCnDecoder decoder;
		const BCnData complete = DecodeNative(dds);
		REQUIRE(complete);
		for (const size_t end : { baseEnd - 1, dds.size() - 1, dds.size() })
		{
			CAPTURE(end);
			const Image decoded = decoder.decode(std::make_unique<ChunkReader>(dds, end), {}, PremultiplyAlpha::No);
			if (end < baseEnd) { CHECK_FALSE(decoded); }
			else { CHECK(decoded == base); }
			const BCnData native = decoder.decodeNative(std::make_unique<ChunkReader>(dds, end), false);
			if (end < dds.size()) { CHECK_FALSE(native); }
			else { CHECK(native.textures == complete.textures); }
		}
	}
}

TEST_CASE("BCnDecoder.dx10_header")
{
	const ScopedLogSilencer silence;
	const Blob dds = BCnEncoder{}.encodeBC7(Image{ 4, 4, Palette::White }, GenerateMipmaps::No);
	constexpr size_t DX10HeaderBytes = 20;
	REQUIRE(dds.size() == (DDSHeaderBytes + DX10HeaderBytes + 16));
	CHECK(DecodeNative(dds, true).format == TextureFormat::BC7_RGBA_Unorm_SRGB);
	for (const size_t size : { DDSHeaderBytes - 1, DDSHeaderBytes + DX10HeaderBytes - 1 })
	{
		CAPTURE(size);
		const Blob truncated = dds.slice(0, size);
		CHECK_FALSE(DecodeImage(truncated));
		CHECK_FALSE(DecodeNative(truncated));
	}
}
