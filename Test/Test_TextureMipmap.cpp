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

namespace
{
	void CheckMipColors(const Texture& texture, const std::span<const Color> colors)
	{
		REQUIRE(texture);
		REQUIRE(texture.mipLevels() == colors.size());
		CHECK(texture.hasMipmaps() == (1 < colors.size()));
		CHECK(texture.mipSize(texture.mipLevels()).isZero());
		const Transformer2D local{ Mat3x2::Identity(), Transformer2D::Target::SetLocal };
		const Transformer2D camera{ Mat3x2::Identity(), Transformer2D::Target::SetCamera };
		const ScopedQuadWarp2D warp{ Mat3x3::Identity(), ScopedQuadWarp2D::Target::Set };
		const ScopedScissorRect2D scissor{ none };
		const ScopedColorMul2D mul{ ColorF{ 1.0 }, ScopedColorMul2D::Target::Set };
		const ScopedColorAdd2D add{ ColorF{ 0.0, 0.0 }, ScopedColorAdd2D::Target::Set };
		const ScopedCustomShader2D shaders{ VertexShader{}, PixelShader{} };
		const ScopedRenderStates2D states{ BlendState::Opaque, SamplerState::ClampNearest };
		for (uint32 level = 0; level < texture.mipLevels(); ++level)
		{
			const Size size = texture.mipSize(level);
			const RenderTexture target{ size };
			{
				const ScopedRenderTarget2D renderTarget{ target };
				const ScopedViewport2D viewport{ Rect{ size } };
				texture.resized(size).draw();
			}
			Image actual;
			REQUIRE(target.readAsImage(actual));
			CHECK(actual == Image{ size, colors[level] });
		}
	}
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
		CheckMipColors(texture, std::span{ colors }.first(levels));
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
	data.size = Size{ 8, 8 };
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
	CheckMipColors(Texture{ data }, colors);
}
