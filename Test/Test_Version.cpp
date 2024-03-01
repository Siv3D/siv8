//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("Version")
{
	Console << U"SIV3D_VERSION_MAJOR: " << SIV3D_VERSION_MAJOR;
	Console << U"SIV3D_VERSION_MINOR: " << SIV3D_VERSION_MINOR;
	Console << U"SIV3D_VERSION_REVISION: " << SIV3D_VERSION_REVISION;
	Console << U"SIV3D_VERSION_PRERELEASE_TAG: " << SIV3D_VERSION_PRERELEASE_TAG;
	Console << U"SIV3D_VERSION: " << SIV3D_VERSION;
	Console << U"SIV3D_VERSION_SHORT_STRING: "	SIV3D_VERSION_SHORT_STRING;
	Console << U"SIV3D_VERSION_STRING: " SIV3D_VERSION_STRING;
	Console << U"SIV3D_VERSION_NAME: " SIV3D_VERSION_NAME;
}
