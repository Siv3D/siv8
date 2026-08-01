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
	SUBCASE("invalid path")
	{
		ChildProcess child{ U"/path/that/does/not/exist" };
		CHECK_FALSE(child.isValid());
		CHECK_FALSE(child.isRunning());
		CHECK_FALSE(child.getExitCode().has_value());
	}

	SUBCASE("arguments and standard output")
	{
		ChildProcess child{
			U"/usr/bin/printf",
			Array<String>{ U"%s:%s", U"hello world", U"こんにちは" },
			Pipe::StdIn
		};

		REQUIRE(child.isValid());

		std::string output;
		std::getline(child.istream(), output);
		child.wait();

		CHECK_EQ(output, std::string{ "hello world:こんにちは" });
		REQUIRE(child.getExitCode().has_value());
		CHECK_EQ(*child.getExitCode(), 0);
	}

	SUBCASE("standard input and output")
	{
		ChildProcess child{ U"/bin/cat", Pipe::StdInOut };
		REQUIRE(child.isValid());

		child.ostream() << "Siv3D ChildProcess" << std::endl;

		std::string output;
		std::getline(child.istream(), output);
		CHECK_EQ(output, std::string{ "Siv3D ChildProcess" });

		child.terminate();
		child.wait();

		CHECK_FALSE(child.isRunning());
		REQUIRE(child.getExitCode().has_value());
		CHECK_EQ(*child.getExitCode(), SIGTERM);
	}

	SUBCASE("exit status")
	{
		ChildProcess child{ U"/bin/sh", Array<String>{ U"-c", U"exit 37" } };
		REQUIRE(child.isValid());

		child.wait();

		CHECK_FALSE(child.isRunning());
		REQUIRE(child.getExitCode().has_value());
		CHECK_EQ(*child.getExitCode(), 37);
	}

	SUBCASE("closed standard input pipe")
	{
		ChildProcess child{ U"/usr/bin/true", Pipe::StdOut };
		REQUIRE(child.isValid());

		child.wait();
		child.ostream() << "ignored" << std::flush;

		CHECK_FALSE(child.ostream().good());
	}
}

# endif
