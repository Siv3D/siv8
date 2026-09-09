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

# include <csignal>

TEST_CASE("ChildProcess")
{
	SECTION("invalid path")
	{
		ChildProcess child{ U"/path/that/does/not/exist" };
		CHECK_FALSE(child.isValid());
		CHECK_FALSE(child.isRunning());
		CHECK_FALSE(child.getExitCode().has_value());
	}

	SECTION("arguments and standard output")
	{
		ChildProcess child{
			U"/usr/bin/printf",
			Array<String>{ U"%s:%s", U"hello world", U"こんにちは" },
			ChildProcessPipe::StdIn
		};

		REQUIRE(child.isValid());

		std::string output;
		std::getline(child.istream(), output);
		child.wait();

		CHECK((output) == (std::string{ "hello world:こんにちは" }));
		REQUIRE(child.getExitCode().has_value());
		CHECK((*child.getExitCode()) == (0));
	}

	SECTION("standard input and output")
	{
		ChildProcess child{ U"/bin/cat", ChildProcessPipe::StdInOut };
		REQUIRE(child.isValid());

		child.ostream() << "Siv3D ChildProcess" << std::endl;

		std::string output;
		std::getline(child.istream(), output);
		CHECK((output) == (std::string{ "Siv3D ChildProcess" }));

		child.terminate();
		child.wait();

		CHECK_FALSE(child.isRunning());
		REQUIRE(child.getExitCode().has_value());
		CHECK((*child.getExitCode()) == (SIGTERM));
	}

	SECTION("exit status")
	{
		ChildProcess child{ U"/bin/sh", Array<String>{ U"-c", U"exit 37" } };
		REQUIRE(child.isValid());

		child.wait();

		CHECK_FALSE(child.isRunning());
		REQUIRE(child.getExitCode().has_value());
		CHECK((*child.getExitCode()) == (37));
	}

	SECTION("closed standard input pipe")
	{
		ChildProcess child{ U"/usr/bin/true", ChildProcessPipe::StdOut };
		REQUIRE(child.isValid());

		child.wait();
		child.ostream() << "ignored" << std::flush;

		CHECK_FALSE(child.ostream().good());
	}
}

# endif
