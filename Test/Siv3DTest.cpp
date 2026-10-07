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

	# if SIV3D_PLATFORM(WINDOWS)
	if (not commandLineArgs.contains(U"--test-only"))
	{
		Console.open();
	}
	# else
	Console.open();
	# endif

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

	Catch::Session session;
	session.configData().defaultColourMode = Catch::ColourMode::PlatformDefault;
	std::vector<std::string> arguments;
	try
	{
		arguments = Test::CatchArguments(System::GetArgc(), System::GetArgv());
	}
	catch (const std::invalid_argument& error)
	{
		Console << Unicode::FromUTF8(error.what());
		return 1;
	}
	std::vector<const char*> argv;
	argv.reserve(arguments.size());
	for (const auto& argument : arguments)
	{
		argv.push_back(argument.c_str());
	}
	if (const int result = session.applyCommandLine(static_cast<int>(argv.size()), argv.data()))
	{
		return result;
	}

	const FilePath outputDirectory = FileSystem::FullPath(U"../../Test/output/");
	g_outputDirectory = outputDirectory;
	const ScopeExit resetOutputDirectory{ [] { g_outputDirectory.clear(); } };
	if (FileSystem::Exists(outputDirectory) && (not FileSystem::Remove(outputDirectory)))
	{
		Console << U"Could not clear test output: " << outputDirectory;
		return 1;
	}

	const int32 exitCode = session.run();

	if (FileSystem::Exists(outputDirectory) && (not FileSystem::Remove(outputDirectory)))
	{
		Console << U"Could not clear test output: " << outputDirectory;
		return 1;
	}
	return exitCode;
}
