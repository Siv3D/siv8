//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "TextureMipmapTestHelper.hpp"
# include <limits>

static_assert(ImageProcessing::CalculateMipmapSize(Size{ 7, 5 }, 0) == Size{ 7, 5 });
static_assert(ImageProcessing::CalculateMipmapSize(Size{ 7, 5 }, 1) == Size{ 3, 2 });
static_assert(ImageProcessing::CalculateMipmapSize(Size{ 7, 5 }, 2) == Size{ 1, 1 });
static_assert(ImageProcessing::CalculateMipmapSize(Size{ 7, 5 }, 3) == Size{ 0, 0 });

TEST_CASE("Texture.mipmap_size_calculation_and_resources")
{
	for (const Size base : { Size{ 1, 1 }, Size{ 1, 7 }, Size{ 7, 1 }, Size{ 7, 5 }, Size{ 16, 8 } })
	{
		const RenderTexture texture{ base, Palette::Red, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
		const MSRenderTexture ms{ base, Palette::Red, MSAASampleCount::X2MSAA, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
		const DynamicTexture dynamic{ Image{ base, Palette::Red }, TextureDesc::Mipmap };
		const Texture immutable{ Image{ base, Palette::Red }, TextureDesc::Mipmap };
		REQUIRE(texture);
		REQUIRE(ms);
		REQUIRE(dynamic);
		REQUIRE(immutable);
		Size expected = base;
		for (uint32 level = 0; level < texture.mipLevels(); ++level)
		{
			CAPTURE(base, level);
			CHECK(ImageProcessing::CalculateMipmapSize(base, level) == expected);
			for (const Texture* item : { static_cast<const Texture*>(&texture), &immutable, static_cast<const Texture*>(&dynamic), static_cast<const Texture*>(&ms) })
			{
				CHECK(item->mipSize(level) == expected);
			}
			Image actual;
			REQUIRE(texture.readAsImage(actual, level));
			CHECK(actual == Image{ expected, Palette::Red });
			REQUIRE(ms.readAsImage(actual, level));
			CHECK(actual == Image{ expected, Palette::Red });
			Array<Byte> raw(texture.format().packedDataSize(expected));
			REQUIRE(texture.read(raw, level));
			CHECK(std::memcmp(raw.data(), actual.data(), raw.size()) == 0);
			expected = Size{ Max(1, expected.x / 2), Max(1, expected.y / 2) };
		}
		CHECK(texture.mipSize(texture.mipLevels()) == Size{ 0, 0 });
		CHECK(texture.mipSize(std::numeric_limits<uint32>::max()) == Size{ 0, 0 });
		const RenderTexture noMips{ base };
		CHECK(noMips.mipSize(0) == base);
		CHECK(noMips.mipSize(1) == Size{ 0, 0 });
	}
	CHECK(Texture{}.mipSize(0) == Size{ 0, 0 });
	for (const Size base : { Size{ 0, 5 }, Size{ 5, 0 }, Size{ -1, 5 }, Size{ 5, -1 } })
	{
		CHECK(ImageProcessing::CalculateMipmapSize(base, 0) == Size{ 0, 0 });
	}
	CHECK(ImageProcessing::CalculateMipmapSize(Size{ 16384, 1 }, 14) == Size{ 1, 1 });
	CHECK(ImageProcessing::CalculateMipmapSize(Size{ 16384, 1 }, 15) == Size{ 0, 0 });
	CHECK(ImageProcessing::CalculateMipmapSize(Size{ 1, 1 }, std::numeric_limits<uint32>::max()) == Size{ 0, 0 });
}

TEST_CASE("Texture.explicit_mipmap_prefix")
{
	const std::array<Color, 4> colors{ Palette::Red, Palette::Lime, Palette::Blue, Palette::White };
	const Size base{ 15, 9 };
	const Image level1{ 7, 4, colors[1] };
	const Image level2{ 3, 2, colors[2] };
	const Image level3{ 1, 1, colors[3] };
	const auto check = [&](const Size size, const Array<Image>& mipmaps, const size_t levels, const TextureDesc desc = TextureDesc::Default2D)
	{
		const Texture texture{ Image{ size, colors[0] }, mipmaps, desc };
		Test::CheckMipColors(texture, std::span{ colors }.first(levels));
	};

	SECTION("Complete odd-size chain") { check(base, { level1, level2, level3 }, 4); }
	SECTION("Valid partial chain") { check(base, { level1, level2 }, 3); }
	SECTION("Empty chain") { check(base, {}, 1); }
	SECTION("Single-column chain") { check({ 1, 7 }, { Image{ 1, 3, colors[1] }, Image{ 1, 1, colors[2] } }, 3); }
	SECTION("Wrong first level") { check(base, { Image{ 8, 4 }, level2 }, 1); }
	SECTION("Wrong middle level") { check(base, { level1, Image{ 3, 3 }, level3 }, 2); }
	SECTION("Empty middle level") { check(base, { level1, Image{}, level3 }, 2); }
	SECTION("Excess levels after 1x1")
	{
		Array<Image> mipmaps(255);
		mipmaps[0] = level1;
		mipmaps[1] = level2;
		mipmaps[2] = level3;
		check(base, mipmaps, 4);
	}
	SECTION("Extra level after 1x1 base") { check({ 1, 1 }, { Image{} }, 1); }
	SECTION("Mipmaps disabled") { check(base, { level1, Image{} }, 1, TextureDesc::NoMipmap); }
	SECTION("Empty base")
	{
		const Texture texture{ Image{}, Array<Image>{ level1 } };
		CHECK_FALSE(texture);
	}
}

TEST_CASE("Texture.bcn_mipmap_upload_and_sizes")
{
	BCnData data;
	data.size = Size{ 12, 8 };
	data.format = TextureFormat::BC1_RGBA_Unorm;
	const std::array<uint16, 4> endpoints{ 0xF800, 0x07E0, 0x001F, 0xFFFF };
	const std::array<Color, 4> colors{ Palette::Red, Palette::Lime, Palette::Blue, Palette::White };
	for (uint32 level = 0; level < 4; ++level)
	{
		const Size size = ImageProcessing::CalculateMipmapSize(data.size, level);
		Blob blocks{ data.format.packedDataSize(size) };
		std::memset(blocks.data(), 0, blocks.size());
		for (size_t offset = 0; offset < blocks.size(); offset += 8)
		{
			// BC1: endpoint 0 is RGB565; all two-bit indices select that endpoint.
			blocks.data()[offset] = static_cast<Byte>(endpoints[level] & 0xFF);
			blocks.data()[offset + 1] = static_cast<Byte>(endpoints[level] >> 8);
		}
		data.textures.push_back(std::move(blocks));
	}
	const auto check = [&](const size_t levels)
	{
		Test::CheckMipColors(Texture{ data }, std::span{ colors }.first(levels));
	};
	SECTION("Complete chain through 6x4, 3x2 and 1x1") { check(4); }
	SECTION("Valid partial chain")
	{
		data.textures.resize(3);
		check(3);
	}
	SECTION("Base only")
	{
		data.textures.resize(1);
		check(1);
	}
	SECTION("First mip one byte short")
	{
		data.textures[1].resize(15); // 6x4 needs two BC1 blocks.
		check(1);
	}
	SECTION("Middle mip one byte too large")
	{
		data.textures[2].resize(9);
		check(2);
	}
	SECTION("Empty middle mip followed by a valid mip")
	{
		data.textures[2].clear();
		check(2);
	}
	SECTION("Extra levels after 1x1")
	{
		data.textures.resize(256);
		check(4);
	}
}

TEST_CASE("Texture.bcn_invalid_base")
{
	BCnData data{ Size{ 4, 4 }, TextureFormat::BC1_RGBA_Unorm, { Blob{ 8 } } };
	SECTION("Missing level zero") { data.textures.clear(); }
	SECTION("Empty level zero followed by a valid block") { data.textures = { Blob{}, Blob{ 8 } }; }
	SECTION("Level zero one byte short") { data.textures[0].resize(7); }
	SECTION("Level zero one byte too large") { data.textures[0].resize(9); }
	SECTION("Unknown format") { data.format = TextureFormat::Unknown; }
	SECTION("Invalid format value") { data.format = static_cast<TexturePixelFormat>(255); }
	SECTION("Eight-byte block supplied for BC3") { data.format = TextureFormat::BC3_RGBA_Unorm; }
	SECTION("Uncompressed format with correct byte count")
	{
		data.format = TextureFormat::R8_Unorm;
		data.textures[0].resize(16);
	}
	SECTION("Invalid base dimensions")
	{
		for (const Size size : { Size{ 0, 4 }, Size{ 4, 0 }, Size{ -4, 4 }, Size{ 4, -4 },
			Size{ 3, 4 }, Size{ 4, 5 }, Size{ Image::MaxWidth + 4, 4 }, Size{ 4, Image::MaxHeight + 4 } })
		{
			CAPTURE(size);
			data.size = size;
			data.textures[0].resize(data.format.packedDataSize(size));
			CHECK_FALSE(Texture{ data });
		}
		return;
	}
	CHECK_FALSE(Texture{ data });
}

TEST_CASE("Texture.bcn_block_size_and_dimension_limits")
{
	for (const TextureFormat format : { TextureFormat::BC1_RGBA_Unorm, TextureFormat::BC1_RGBA_Unorm_SRGB,
		TextureFormat::BC3_RGBA_Unorm, TextureFormat::BC3_RGBA_Unorm_SRGB, TextureFormat::BC4_R_Unorm,
		TextureFormat::BC5_RG_Unorm, TextureFormat::BC7_RGBA_Unorm, TextureFormat::BC7_RGBA_Unorm_SRGB })
	{
		CAPTURE(format.value());
		const size_t bytes = format.blockSize();
		const BCnData data{ Size{ 4, 4 }, format, { Blob{ bytes }, Blob{ bytes }, Blob{ bytes } } };
		const Texture texture{ data };
		REQUIRE(texture);
		CHECK(texture.format() == format);
		CHECK(texture.mipLevels() == 3);
		CHECK(texture.mipSize(2) == Size{ 1, 1 });
	}
	for (const Size size : { Size{ Image::MaxWidth, 4 }, Size{ 4, Image::MaxHeight } })
	{
		const TextureFormat format = TextureFormat::BC1_RGBA_Unorm;
		const BCnData data{ size, format, { Blob{ format.packedDataSize(size) } } };
		const Texture texture{ data };
		REQUIRE(texture);
		CHECK(texture.size() == size);
	}
}

TEST_CASE("Texture.bcn_from_dds_reader")
{
	const Size size{ 12, 8 };
	const Blob dds = BCnEncoder{}.encodeBC3(Image{ size, Palette::White });
	REQUIRE(dds);
	const Texture texture{ MemoryViewReader{ dds.data(), dds.size() }, TextureDesc::Mipmap };
	REQUIRE(texture);
	CHECK(texture.format() == TextureFormat::BC3_RGBA_Unorm);
	CHECK(texture.size() == size);
	CHECK(texture.mipLevels() == 4);
}
