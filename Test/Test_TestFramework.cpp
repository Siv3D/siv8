//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTestFramework.hpp"
# include <array>
# include <sstream>

using namespace s3d;

TEST_CASE("Test::Approx preserves symmetric and zero-adjacent tolerances")
{
	CHECK(1e-6 == Test::Approx(0));
	CHECK(0 == Test::Approx(-1e-6));
	CHECK_FALSE(1e-3 == Test::Approx(0));
	CHECK(1.25 == Test::Approx(1).epsilon(0.25).scale(0));
	CHECK(1 == Test::Approx(1.25).epsilon(0.25).scale(0));
	CHECK(-1.25 == Test::Approx(-1).epsilon(0.25).scale(0));
	CHECK_FALSE(0.5 == Test::Approx(0).epsilon(0.25));
	// With scale=1 and epsilon=0.25, the exact boundary at a=2, b=3 is excluded.
	CHECK_FALSE(2 == Test::Approx(3).epsilon(0.25));
	CHECK_FALSE(3 == Test::Approx(2).epsilon(0.25));
	CHECK(std::nextafter(2.0, 3.0) == Test::Approx(3).epsilon(0.25));
	CHECK_FALSE(0 == Test::Approx(0).epsilon(0));
	CHECK_FALSE(std::numeric_limits<double>::infinity() == Test::Approx(std::numeric_limits<double>::infinity()));
	CHECK_FALSE(std::numeric_limits<double>::quiet_NaN() == Test::Approx(0));
	std::ostringstream output;
	output << Test::Approx(1).epsilon(0.25).scale(2);
	CHECK(output.str() == "Approx(1, epsilon=0.25, scale=2)");
}

TEST_CASE("Test::CatchArguments keeps filters and UTF-8 argument boundaries")
{
	const std::array arguments{ "tests", "--test-only", "--test-verbose", "--test-case=*spaces 日本語 \"quoted\"*",
		"--durations", "yes", "--test-case", "Array*" };
	const std::vector<std::string> expected{ "tests", "*spaces 日本語 \"quoted\"*", "--durations", "yes", "Array*" };
	CHECK(Test::CatchArguments(static_cast<int>(arguments.size()), arguments.data()) == expected);
	const std::array positional{ "tests", "--", "--test-only", "--test-case=name" };
	const std::vector<std::string> expectedPositional{ positional.begin(), positional.end() };
	CHECK(Test::CatchArguments(static_cast<int>(positional.size()), positional.data()) == expectedPositional);
}

TEST_CASE("Test::CatchArguments rejects missing or empty legacy filters")
{
	for (const auto* filter : { "--test-case", "--test-case=", "--test-case=--help" })
	{
		const std::array arguments{ "tests", filter };
		CHECK_THROWS_AS(Test::CatchArguments(static_cast<int>(arguments.size()), arguments.data()), std::invalid_argument);
	}
	const std::array arguments{ "tests", "--test-case", "--test-only" };
	CHECK_THROWS_AS(Test::CatchArguments(static_cast<int>(arguments.size()), arguments.data()), std::invalid_argument);
}

# if defined(__APPLE__)

TEST_CASE("Test::CatchArguments removes Xcode's document revision preference")
{
	for (const auto* value : { "YES", "NO" })
	{
		const std::array arguments{ "tests", "-NSDocumentRevisionsDebugMode", value };
		CHECK(Test::CatchArguments(static_cast<int>(arguments.size()), arguments.data()) == std::vector<std::string>{ "tests" });
		const std::array filtered{ "tests", "-NSDocumentRevisionsDebugMode", value,
			"--test-only", "--test-case=Array*", "-NSDocumentRevisionsDebugMode", value };
		const std::vector<std::string> expected{ "tests", "Array*" };
		CHECK(Test::CatchArguments(static_cast<int>(filtered.size()), filtered.data()) == expected);
	}
}

TEST_CASE("Test::CatchArguments preserves malformed, unknown and positional Cocoa arguments")
{
	const std::vector<std::vector<const char*>> cases{
		{ "tests", "-NSDocumentRevisionsDebugMode" },
		{ "tests", "-NSDocumentRevisionsDebugMode", "Array*" },
		{ "tests", "-NSDocumentRevisionsDebugMode", "--unknown-option" },
		{ "tests", "-NSDocumentRevisionsDebugModeTypo", "YES" },
		{ "tests", "-NSUnknownPreference", "YES" },
		{ "tests", "--", "-NSDocumentRevisionsDebugMode", "YES" },
	};
	for (const auto& arguments : cases)
	{
		const std::vector<std::string> expected{ arguments.begin(), arguments.end() };
		CHECK(Test::CatchArguments(static_cast<int>(arguments.size()), arguments.data()) == expected);
	}
}

# endif
