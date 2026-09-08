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

	// Test paths are relative to the repository's platform App directory.
	// Reject an unexpected working directory before writing or removing files.
	const FilePath currentDirectory = FileSystem::CurrentDirectory();
	if ((not FileSystem::IsFile(U"../../Test/Siv3DTest.cpp"))
		|| ((currentDirectory != FileSystem::FullPath(U"../../macOS/App/"))
			&& (currentDirectory != FileSystem::FullPath(U"../../WindowsDesktop/App/"))))
	{
		Console << U"Tests must run from this repository's macOS/App or WindowsDesktop/App directory.";
		return 1;
	}

	doctest::Context context;
	context.applyCommandLine(System::GetArgc(), System::GetArgv());

	const FilePath outputDirectory = FileSystem::FullPath(U"../../Test/output/");
	if (FileSystem::Exists(outputDirectory) && (not FileSystem::Remove(outputDirectory)))
	{
		Console << U"Could not clear test output: " << outputDirectory;
		return 1;
	}

	const int32 exitCode = context.run();

	if (FileSystem::Exists(outputDirectory) && (not FileSystem::Remove(outputDirectory)))
	{
		Console << U"Could not clear test output: " << outputDirectory;
		return 1;
	}
	return exitCode;
}
