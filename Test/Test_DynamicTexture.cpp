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

# if SIV3D_PLATFORM(MACOS)

TEST_CASE("DynamicTexture.fillRegion.Metal")
{
	DynamicTexture texture{ Size{ 64, 64 }, Palette::Black };
	const Image image{ Size{ 7, 5 }, Palette::Red };
	const Rect rect{ 3, 4, image.size() };

	CHECK(texture.fillRegionIfNotBusy(image, rect));
	CHECK(texture.fillRegion(Palette::Blue, rect));
	CHECK(texture.fill(Palette::Black));

	CHECK_FALSE(texture.fillRegion(image, Rect{ -1, 4, image.size() }));
	CHECK_FALSE(texture.fillRegion(image, Rect{ 60, 60, image.size() }));
	CHECK_FALSE(texture.fillRegion(Image{ 6, 5 }, rect));
}

# endif
