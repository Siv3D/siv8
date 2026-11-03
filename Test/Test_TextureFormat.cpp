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

static_assert(TextureFormat{ TextureFormat::R32_Float }.isFloat32());
static_assert(TextureFormat{ TextureFormat::R32G32_Float }.isFloat32());
static_assert(TextureFormat{ TextureFormat::R32G32B32A32_Float }.isFloat32());
static_assert(not TextureFormat{ TextureFormat::R11G11B10_UFloat }.isFloat32());

TEST_CASE("TextureFormat.properties_and_packed_layout")
{
	constexpr std::array<uint32, 22> PixelBytes{ 0, 1, 2, 2, 4, 4, 4, 4, 4, 4, 4, 8, 8, 16, 0, 0, 0, 0, 0, 0, 0, 0 };
	constexpr std::array<uint32, 22> BlockBytes{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 16, 16, 8, 16, 16, 16 };
	for (uint8 i = 0; i < PixelBytes.size(); ++i)
	{
		const TextureFormat format{ static_cast<TexturePixelFormat>(i) };
		CAPTURE(i);
		CHECK(format.isFloat32() == ((i == 8) || (i == 12) || (i == 13)));
		for (const uint32 width : { 0u, 1u, 3u, 4u, 5u, 7u, 16u, 16384u })
		{
			const size_t row = BlockBytes[i] ? (((width + 3) / 4) * BlockBytes[i]) : (width * PixelBytes[i]);
			CHECK(format.packedRowBytes(width) == row);
			CHECK(format.bytesPerRow(width) == (BlockBytes[i] ? row : ((row + 3) & ~size_t{ 3 })));
			for (const int32 height : { 0, 1, 3, 4, 5, 7, 16 })
			{
				const size_t rows = BlockBytes[i] ? ((height + 3) / 4) : height;
				CHECK(format.packedDataSize(Size{ static_cast<int32>(width), height }) == (row * rows));
			}
		}
		CHECK(format.packedDataSize(Size{ -1, 3 }) == 0);
		CHECK(format.packedDataSize(Size{ 3, -1 }) == 0);
	}
	CHECK(TextureFormat{ TextureFormat::R8_Unorm }.packedDataSize(Size{ 3, 2 }) == 6);
	CHECK(TextureFormat{ TextureFormat::R8G8B8A8_Unorm }.packedDataSize(Size{ 3, 2 }) == 24);
	CHECK(TextureFormat{ TextureFormat::BC1_RGBA_Unorm }.packedDataSize(Size{ 5, 3 }) == 16);
	const TextureFormat invalid{ static_cast<TexturePixelFormat>(255) };
	CHECK_FALSE(invalid.isFloat32());
	CHECK(invalid.packedRowBytes(4) == 0);
	CHECK(invalid.packedDataSize(Size{ 4, 4 }) == 0);
	const TextureFormat wide{ TextureFormat::R32G32B32A32_Float };
	constexpr uint64 LargeRow = (uint64{ std::numeric_limits<uint32>::max() } * 16);
	CHECK(wide.packedRowBytes(std::numeric_limits<uint32>::max()) == ((LargeRow <= std::numeric_limits<size_t>::max()) ? LargeRow : 0));
	CHECK(wide.packedDataSize(Size{ std::numeric_limits<int32>::max(), std::numeric_limits<int32>::max() }) == 0);
}
