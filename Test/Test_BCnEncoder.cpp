//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	constexpr int64 DDSHeaderBytes = (4 + 124);
	constexpr int64 DX10HeaderBytes = 20;

	struct EncoderCase
	{
		TextureFormat format;
		int64 headerBytes;
		int64 blockBytes;
		bool (BCnEncoder::*write)(const Image&, IWriter&, GenerateMipmaps) const;
		Blob (BCnEncoder::*encode)(const Image&, GenerateMipmaps) const;
	};

	constexpr EncoderCase Encoders[]{
		{ TextureFormat::BC1_RGBA_Unorm, DDSHeaderBytes, 8, &BCnEncoder::encodeBC1, &BCnEncoder::encodeBC1 },
		{ TextureFormat::BC3_RGBA_Unorm, DDSHeaderBytes, 16, &BCnEncoder::encodeBC3, &BCnEncoder::encodeBC3 },
		{ TextureFormat::BC4_R_Unorm, DDSHeaderBytes, 8, &BCnEncoder::encodeBC4, &BCnEncoder::encodeBC4 },
		{ TextureFormat::BC5_RG_Unorm, DDSHeaderBytes, 16, &BCnEncoder::encodeBC5, &BCnEncoder::encodeBC5 },
		{ TextureFormat::BC7_RGBA_Unorm, (DDSHeaderBytes + DX10HeaderBytes), 16, &BCnEncoder::encodeBC7, &BCnEncoder::encodeBC7 },
	};

	class BoundedWriter final : public MemoryWriter
	{
	public:

		explicit BoundedWriter(const int64 capacity) : m_capacity{ capacity } {}

		int64 write(const void* src, const int64 bytes) override
		{
			const int64 accepted = Min(bytes, (m_capacity - getPos()));
			if (accepted < bytes)
			{
				++shortWrites;
			}
			return MemoryWriter::write(src, accepted);
		}

		int32 shortWrites = 0;

	private:

		int64 m_capacity;
	};
}

TEST_CASE("BCnEncoder.empty_image")
{
	const BCnEncoder encoder;
	const Image empty;
	for (const auto& bc : Encoders)
	{
		CAPTURE(bc.format.value());
		for (const GenerateMipmaps mipmaps : { GenerateMipmaps::No, GenerateMipmaps::Yes })
		{
			CAPTURE(mipmaps.getBool());
			BoundedWriter writer{ 0 };
			CHECK_FALSE((encoder.*bc.write)(empty, writer, mipmaps));
			CHECK(writer.shortWrites == 0);
			CHECK_FALSE((encoder.*bc.encode)(empty, mipmaps));
		}
	}
}

TEST_CASE("BCnEncoder.short_write")
{
	const BCnEncoder encoder;
	const Image source{ Size{ 8, 8 }, Arg::generator = [](const int32 x, const int32 y)
		{
			return Color{ (x * 32), (y * 32), ((x + y) * 16) };
		} };
	for (const auto& bc : Encoders)
	{
		CAPTURE(bc.format.value());
		for (const GenerateMipmaps mipmaps : { GenerateMipmaps::No, GenerateMipmaps::Yes })
		{
			CAPTURE(mipmaps.getBool());
			// Four blocks at 8x8, then one each at 4x4, 2x2, and 1x1.
			const int64 blocks = (mipmaps ? 7 : 4);
			const Blob dds = (encoder.*bc.encode)(source, mipmaps);
			REQUIRE(dds.ssize() == (bc.headerBytes + blocks * bc.blockBytes));
			BoundedWriter complete{ dds.ssize() };
			CHECK((encoder.*bc.write)(source, complete, mipmaps));
			CHECK(complete.getBlob() == dds);
			CHECK(complete.shortWrites == 0);
			const BCnData native = BCnDecoder{}.decodeNative(std::make_unique<MemoryViewReader>(dds.data(), dds.size()), false);
			REQUIRE(native);
			CHECK(native.format == bc.format);
			CHECK(native.size == source.size());
			CHECK(native.textures.size() == (mipmaps ? 4u : 1u));

			const auto checkShortWrite = [&](const int64 capacity)
			{
				CAPTURE(capacity);
				BoundedWriter writer{ capacity };
				CHECK_FALSE((encoder.*bc.write)(source, writer, mipmaps));
				CHECK(writer.shortWrites == 1);
				CHECK(writer.getBlob() == dds.slice(0, static_cast<size_t>(capacity)));
			};

			checkShortWrite(0);
			checkShortWrite(3); // In the DDS signature.
			checkShortWrite(DDSHeaderBytes - 1);
			if (bc.headerBytes > DDSHeaderBytes)
			{
				checkShortWrite(bc.headerBytes - 1);
			}
			for (int64 block = 1; block <= blocks; ++block)
			{
				checkShortWrite(bc.headerBytes + block * bc.blockBytes - 1);
			}
		}
	}
}
