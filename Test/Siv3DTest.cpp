//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

void TestPlatform();

void RunTest()
{
	Console.open();

	TestPlatform();
}

void TestPlatform()
{
	Console << U"SIV3D_PLATFORM_NAME: "	SIV3D_PLATFORM_NAME;
	Console << U"SIV3D_INTRINSIC_TYPE: " SIV3D_INTRINSIC_TYPE;
	Console << U"SIV3D_BUILD_TYPE: " SIV3D_BUILD_TYPE;
	Console << U"SIV3D_COMPILER_NAME: " SIV3D_COMPILER_NAME;
	Console << U"SIV3D_CPU_TYPE: " SIV3D_CPU_TYPE;
}