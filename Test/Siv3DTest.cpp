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
# include "Siv3DTest.hpp"

namespace
{
	FilePath g_outputDirectory;
}

FilePath s3d::Test::OutputPath(const FilePathView relativePath)
{
	if (g_outputDirectory.isEmpty())
	{
		throw Error{ U"Test output is available only while RunTest is executing." };
	}

	if (relativePath.isEmpty() || relativePath.starts_with(U'/')
		|| relativePath.contains(U'\\') || relativePath.contains(U':') || relativePath.contains(U'\0'))
	{
		throw Error{ U"Test output requires a relative path with '/' separators." };
	}

	for (size_t start = 0; start < relativePath.size();)
	{
		const size_t separator = relativePath.find(U'/', start);
		const size_t end = (separator == FilePathView::npos) ? relativePath.size() : separator;
		const FilePathView component = relativePath.substr(start, (end - start));
		if ((component == U".") || (component == U".."))
		{
			throw Error{ U"Test output paths must not contain '.' or '..' components." };
		}
		start = (end + 1);
	}

	return (g_outputDirectory + relativePath);
}

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
	g_outputDirectory = outputDirectory;
	const ScopeExit resetOutputDirectory{ [] { g_outputDirectory.clear(); } };
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
