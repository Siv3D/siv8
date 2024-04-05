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

# include <Siv3D.hpp>

# define ANKERL_NANOBENCH_IMPLEMENT
# include <ThirdParty/nanobench/nanobench.h>

# define DOCTEST_CONFIG_IMPLEMENT
# define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
# if SIV3D_COMPILER(APPLECLANG) // Xcode では色付けを無効化
#   define DOCTEST_CONFIG_COLORS_NONE
# endif
# include <ThirdParty/doctest/doctest.h>

void RunTest()
{
	Console.open();

	doctest::Context context;
	{
		FileSystem::Remove(U"../../Test/output/");

		context.run();

		FileSystem::Remove(U"../../Test/output/");
	}
}
