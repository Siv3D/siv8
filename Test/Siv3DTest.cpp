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

# include <Siv3D.hpp>

# define ANKERL_NANOBENCH_IMPLEMENT
# include <ThirdParty/nanobench/nanobench.h>

# define DOCTEST_CONFIG_IMPLEMENT
# define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
# if SIV3D_COMPILER(APPLE_CLANG) // Xcode では色付けを無効化
#   define DOCTEST_CONFIG_COLORS_NONE
# endif
# include <ThirdParty/doctest/doctest.h>

int32 RunTest()
{
	const auto& commandLineArgs = System::GetCommandLineArgs();
	if (commandLineArgs.contains(U"--test-only")
		&& (not commandLineArgs.contains(U"--test-verbose")))
	{
		Logger.setOutputLevel(LogLevel::Error);
	}

	Console.open();

	doctest::Context context;
	context.applyCommandLine(System::GetArgc(), System::GetArgv());

	FileSystem::Remove(U"../../Test/output/");

	const int32 exitCode = context.run();

	FileSystem::Remove(U"../../Test/output/");
	return exitCode;
}
