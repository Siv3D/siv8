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

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(WINDOWS)

TEST_CASE("DynamicTexture.fillRegion")
{
	DynamicTexture texture{ Size{ 64, 64 }, Palette::Black };
	const Image image{ Size{ 7, 5 }, Palette::Red };
	const Rect rect{ 3, 4, image.size() };

	CHECK(texture.fillRegionIfNotBusy(image, rect));
	CHECK(texture.fillRegion(Palette::Blue, rect));
	CHECK(texture.fill(Palette::Black));

	CHECK_FALSE(texture.fillRegion(image, Rect{ -1, 4, image.size() }));
	CHECK_FALSE(texture.fillRegion(image, Rect{ 60, 60, image.size() }));
	CHECK_FALSE(texture.fillRegion(image, Rect{ 3, 4, 0, image.height() }));
	CHECK_FALSE(texture.fillRegion(Image{ 6, 5 }, rect));
}

TEST_CASE("DynamicTexture.R16G16B16A16_Float.Tuple")
{
	using Pixel = std::tuple<HalfFloat, HalfFloat, HalfFloat, HalfFloat>;

	const Pixel fullPixel{ HalfFloat{ 0.125f }, HalfFloat{ 0.25f }, HalfFloat{ 0.5f }, HalfFloat{ 0.75f } };
	const Grid<Pixel> image{ Size{ 7, 5 }, fullPixel };
	DynamicTexture texture;

	REQUIRE(texture.fill(image));
	REQUIRE(texture.format() == TextureFormat::R16G16B16A16_Float);
	CHECK(texture.fill(image));

	const Pixel regionPixel{ HalfFloat{ 0.75f }, HalfFloat{ 0.5f }, HalfFloat{ 0.25f }, HalfFloat{ 0.125f } };
	const Grid<Pixel> region{ Size{ 3, 2 }, regionPixel };
	CHECK(texture.fillRegion(region, Rect{ 2, 1, region.size() }));
}

# if SIV3D_PLATFORM(WINDOWS)

TEST_CASE("DynamicTexture.fill.D3D11.NonSquare")
{
	DynamicTexture wideTexture{ Size{ 64, 7 }, Palette::Black };
	DynamicTexture tallTexture{ Size{ 7, 64 }, Palette::Black };

	CHECK(wideTexture.fill(Palette::Red));
	CHECK(tallTexture.fill(Palette::Blue));
}

TEST_CASE("DynamicTexture.generateMips.D3D11")
{
	DynamicTexture texture{ Size{ 64, 64 }, Palette::Black, TextureFormat::R8G8B8A8_Unorm, TextureDesc::Mipmap };
	const Image image{ Size{ 7, 5 }, Palette::Red };
	const Rect rect{ 3, 4, image.size() };

	REQUIRE(texture.hasMipmaps());
	CHECK(1 < texture.mipLevels());
	CHECK(texture.fillRegion(image, rect));
	texture.generateMips();
}

# endif

# endif
