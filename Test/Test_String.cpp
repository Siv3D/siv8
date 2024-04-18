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

[[nodiscard]]
static std::u32string RandomString(size_t length)
{
	std::u32string s;

	s.reserve(length);

	for (size_t i = 0; i < length; ++i)
	{
		s.push_back(static_cast<char32_t>(Random(0x110000)));
	}

	return s;
}

TEST_CASE("String.operator ==")
{
	const String s0 = RandomString(0);
	const String s3 = RandomString(3);
	const String s5 = RandomString(5);
	const String s8 = RandomString(8);
	const String s15 = RandomString(15);
	const String s9999 = RandomString(9999);
	const String s10000a = (s9999 + U'a');
	const String s10000b = (s9999 + U'b');
	const String sa10000 = (U'a' + s9999);
	const String sb10000 = (U'b' + s9999);

	CHECK_EQ(s0, s0);
	CHECK_EQ(s3, s3);
	CHECK_EQ(s5, s5);
	CHECK_EQ(s8, s8);
	CHECK_EQ(s15, s15);

	CHECK_EQ(s9999, s9999);
	CHECK_EQ(s10000a, s10000a);
	CHECK_EQ(s10000b, s10000b);
	CHECK_EQ(sa10000, sa10000);
	CHECK_EQ(sb10000, sb10000);

	CHECK_NE(s0, s3);
	CHECK_NE(s0, s9999);
	CHECK_NE(s3, s5);
	CHECK_NE(s3, s9999);
	CHECK_NE(s5, s8);
	CHECK_NE(s5, s9999);
	CHECK_NE(s8, s15);
	CHECK_NE(s8, s9999);
	CHECK_NE(s15, s9999);

	CHECK_NE(s9999, s0);
	CHECK_NE(s9999, s10000a);
	CHECK_NE(s9999, s10000b);
	CHECK_NE(s9999, sa10000);
	CHECK_NE(s9999, sb10000);

	CHECK_NE(s10000a, s0);
	CHECK_NE(s10000a, s9999);
	CHECK_NE(s10000a, s10000b);
	CHECK_NE(s10000a, sa10000);
	CHECK_NE(s10000a, sb10000);

	CHECK_NE(s10000b, s0);
	CHECK_NE(s10000b, s9999);
	CHECK_NE(s10000b, s10000a);
	CHECK_NE(s10000b, sa10000);
	CHECK_NE(s10000b, sb10000);

	CHECK_NE(sa10000, s0);
	CHECK_NE(sa10000, s9999);
	CHECK_NE(sa10000, s10000a);
	CHECK_NE(sa10000, s10000b);
	CHECK_NE(sa10000, sb10000);

	CHECK_NE(sb10000, s0);
	CHECK_NE(sb10000, s9999);
	CHECK_NE(sb10000, s10000a);
	CHECK_NE(sb10000, s10000b);
	CHECK_NE(sb10000, sa10000);
}

# if SIV3D_RUN_BENCHMARK

TEST_CASE("String.levenshteinDistanceFrom.Benchmark")
{
	const ScopedLogSilencer logSilencer;

	{
		const String s0a = RandomString(0);
		const String s0b = RandomString(0);

		const String s10a = RandomString(10);
		const String s10b = RandomString(10);

		const String s100a = RandomString(100);
		const String s100b = RandomString(100);

		const String s1000a = RandomString(1000);
		const String s1000b = RandomString(1000);

		const String s4000a = RandomString(4000);
		const String s4000b = RandomString(4000);

		{
			Bench{}.title("String::levenshteinDistanceFrom").run("s0", [&]() { doNotOptimizeAway(s0a.levenshteinDistanceFrom(s0b)); });
			Bench{}.title("String::levenshteinDistanceFrom").run("s10", [&]() { doNotOptimizeAway(s10a.levenshteinDistanceFrom(s10b)); });
			Bench{}.title("String::levenshteinDistanceFrom").run("s100", [&]() { doNotOptimizeAway(s100a.levenshteinDistanceFrom(s100b)); });
			Bench{}.title("String::levenshteinDistanceFrom").run("s1000", [&]() { doNotOptimizeAway(s1000a.levenshteinDistanceFrom(s1000b)); });
			Bench{}.title("String::levenshteinDistanceFrom").run("s4000", [&]() { doNotOptimizeAway(s4000a.levenshteinDistanceFrom(s4000b)); });
		}
	}
}

TEST_CASE("String.operator ==.Benchmark")
{
	const ScopedLogSilencer logSilencer;
	
	{
		const std::u32string s0 = RandomString(0);
		const std::u32string s3 = RandomString(3);
		const std::u32string s5 = RandomString(5);
		const std::u32string s8 = RandomString(8);
		const std::u32string s10 = RandomString(10);
		const std::u32string s15 = RandomString(15);
		const std::u32string s20 = RandomString(20);
		const std::u32string s31 = RandomString(31);
		const std::u32string s32 = RandomString(32);
		const std::u32string s100 = RandomString(100);
		const std::u32string s445 = RandomString(445);
		const std::u32string s999 = RandomString(999);
		const std::u32string s9999 = RandomString(9999);

		{
			Bench{}.title("std::u32string::operator ==").run("s0", [&]() { doNotOptimizeAway(s0 == s0); });
			Bench{}.title("std::u32string::operator ==").run("s3", [&]() { doNotOptimizeAway(s3 == s3); });
			Bench{}.title("std::u32string::operator ==").run("s5", [&]() { doNotOptimizeAway(s5 == s5); });
			Bench{}.title("std::u32string::operator ==").run("s8", [&]() { doNotOptimizeAway(s8 == s8); });
			Bench{}.title("std::u32string::operator ==").run("s10", [&]() { doNotOptimizeAway(s10 == s10); });
			Bench{}.title("std::u32string::operator ==").run("s15", [&]() { doNotOptimizeAway(s15 == s15); });
			Bench{}.title("std::u32string::operator ==").run("s20", [&]() { doNotOptimizeAway(s20 == s20); });
			Bench{}.title("std::u32string::operator ==").run("s31", [&]() { doNotOptimizeAway(s31 == s31); });
			Bench{}.title("std::u32string::operator ==").run("s32", [&]() { doNotOptimizeAway(s32 == s32); });
			Bench{}.title("std::u32string::operator ==").run("s100", [&]() { doNotOptimizeAway(s100 == s100); });
			Bench{}.title("std::u32string::operator ==").run("s445", [&]() { doNotOptimizeAway(s445 == s445); });
			Bench{}.title("std::u32string::operator ==").run("s999", [&]() { doNotOptimizeAway(s999 == s999); });
			Bench{}.title("std::u32string::operator ==").run("s9999", [&]() { doNotOptimizeAway(s9999 == s9999); });
		}

		{
			std::u32string t0 = s0;
			std::u32string t3 = s3;
			std::u32string t5 = s5;
			std::u32string t8 = s8;
			std::u32string t10 = s10;
			std::u32string t15 = s15;
			std::u32string t20 = s20;
			std::u32string t31 = s31;
			std::u32string t32 = s32;
			std::u32string t100 = s100;
			std::u32string t445 = s445;
			std::u32string t999 = s999;
			std::u32string t9999 = s9999;

			++t3.back();
			++t5.back();
			++t8.back();
			++t10.back();
			++t15.back();
			++t20.back();
			++t31.back();
			++t32.back();
			++t100.back();
			++t445.back();
			++t999.back();
			++t9999.back();

			Bench{}.title("std::u32string::operator ==").run("t0", [&]() { doNotOptimizeAway(s0 == t0); });
			Bench{}.title("std::u32string::operator ==").run("t3", [&]() { doNotOptimizeAway(s3 == t3); });
			Bench{}.title("std::u32string::operator ==").run("t5", [&]() { doNotOptimizeAway(s5 == t5); });
			Bench{}.title("std::u32string::operator ==").run("t8", [&]() { doNotOptimizeAway(s8 == t8); });
			Bench{}.title("std::u32string::operator ==").run("t10", [&]() { doNotOptimizeAway(s10 == t10); });
			Bench{}.title("std::u32string::operator ==").run("t15", [&]() { doNotOptimizeAway(s15 == t15); });
			Bench{}.title("std::u32string::operator ==").run("t20", [&]() { doNotOptimizeAway(s20 == t20); });
			Bench{}.title("std::u32string::operator ==").run("t31", [&]() { doNotOptimizeAway(s31 == t31); });
			Bench{}.title("std::u32string::operator ==").run("t32", [&]() { doNotOptimizeAway(s32 == t32); });
			Bench{}.title("std::u32string::operator ==").run("t100", [&]() { doNotOptimizeAway(s100 == t100); });
			Bench{}.title("std::u32string::operator ==").run("t445", [&]() { doNotOptimizeAway(s445 == t445); });
			Bench{}.title("std::u32string::operator ==").run("t999", [&]() { doNotOptimizeAway(s999 == t999); });
			Bench{}.title("std::u32string::operator ==").run("t9999", [&]() { doNotOptimizeAway(s9999 == t9999); });
		}

		{
			std::u32string u0 = s0;
			std::u32string u3 = s3;
			std::u32string u5 = s5;
			std::u32string u8 = s8;
			std::u32string u10 = s10;
			std::u32string u15 = s15;
			std::u32string u20 = s20;
			std::u32string u31 = s31;
			std::u32string u32 = s32;
			std::u32string u100 = s100;
			std::u32string u445 = s445;
			std::u32string u999 = s999;
			std::u32string u9999 = s9999;

			++u3.front();
			++u5.front();
			++u8.front();
			++u10.front();
			++u15.front();
			++u20.front();
			++u31.front();
			++u32.front();
			++u100.front();
			++u445.front();
			++u999.front();
			++u9999.front();

			Bench{}.title("std::u32string::operator ==").run("u0", [&]() { doNotOptimizeAway(s0 == u0); });
			Bench{}.title("std::u32string::operator ==").run("u3", [&]() { doNotOptimizeAway(s3 == u3); });
			Bench{}.title("std::u32string::operator ==").run("u5", [&]() { doNotOptimizeAway(s5 == u5); });
			Bench{}.title("std::u32string::operator ==").run("u8", [&]() { doNotOptimizeAway(s8 == u8); });
			Bench{}.title("std::u32string::operator ==").run("u10", [&]() { doNotOptimizeAway(s10 == u10); });
			Bench{}.title("std::u32string::operator ==").run("u15", [&]() { doNotOptimizeAway(s15 == u15); });
			Bench{}.title("std::u32string::operator ==").run("u20", [&]() { doNotOptimizeAway(s20 == u20); });
			Bench{}.title("std::u32string::operator ==").run("u31", [&]() { doNotOptimizeAway(s31 == u31); });
			Bench{}.title("std::u32string::operator ==").run("u32", [&]() { doNotOptimizeAway(s32 == u32); });
			Bench{}.title("std::u32string::operator ==").run("u100", [&]() { doNotOptimizeAway(s100 == u100); });
			Bench{}.title("std::u32string::operator ==").run("u445", [&]() { doNotOptimizeAway(s445 == u445); });
			Bench{}.title("std::u32string::operator ==").run("u999", [&]() { doNotOptimizeAway(s999 == u999); });
			Bench{}.title("std::u32string::operator ==").run("u9999", [&]() { doNotOptimizeAway(s9999 == u9999); });
		}
	}

	{
		const String s0 = RandomString(0);
		const String s3 = RandomString(3);
		const String s5 = RandomString(5);
		const String s8 = RandomString(8);
		const String s10 = RandomString(10);
		const String s15 = RandomString(15);
		const String s20 = RandomString(20);
		const String s31 = RandomString(31);
		const String s32 = RandomString(32);
		const String s100 = RandomString(100);
		const String s445 = RandomString(445);
		const String s999 = RandomString(999);
		const String s9999 = RandomString(9999);

		{
			Bench{}.title("String::operator ==").run("s0", [&]() { doNotOptimizeAway(s0 == s0); });
			Bench{}.title("String::operator ==").run("s3", [&]() { doNotOptimizeAway(s3 == s3); });
			Bench{}.title("String::operator ==").run("s5", [&]() { doNotOptimizeAway(s5 == s5); });
			Bench{}.title("String::operator ==").run("s8", [&]() { doNotOptimizeAway(s8 == s8); });
			Bench{}.title("String::operator ==").run("s10", [&]() { doNotOptimizeAway(s10 == s10); });
			Bench{}.title("String::operator ==").run("s15", [&]() { doNotOptimizeAway(s15 == s15); });
			Bench{}.title("String::operator ==").run("s20", [&]() { doNotOptimizeAway(s20 == s20); });
			Bench{}.title("String::operator ==").run("s31", [&]() { doNotOptimizeAway(s31 == s31); });
			Bench{}.title("String::operator ==").run("s32", [&]() { doNotOptimizeAway(s32 == s32); });
			Bench{}.title("String::operator ==").run("s100", [&]() { doNotOptimizeAway(s100 == s100); });
			Bench{}.title("String::operator ==").run("s445", [&]() { doNotOptimizeAway(s445 == s445); });
			Bench{}.title("String::operator ==").run("s999", [&]() { doNotOptimizeAway(s999 == s999); });
			Bench{}.title("String::operator ==").run("s9999", [&]() { doNotOptimizeAway(s9999 == s9999); });
		}

		{
			String t0 = s0;
			String t3 = s3;
			String t5 = s5;
			String t8 = s8;
			String t10 = s10;
			String t15 = s15;
			String t20 = s20;
			String t31 = s31;
			String t32 = s32;
			String t100 = s100;
			String t445 = s445;
			String t999 = s999;
			String t9999 = s9999;

			++t3.back();
			++t5.back();
			++t8.back();
			++t10.back();
			++t15.back();
			++t20.back();
			++t31.back();
			++t32.back();
			++t100.back();
			++t445.back();
			++t999.back();
			++t9999.back();

			Bench{}.title("String::operator ==").run("t0", [&]() { doNotOptimizeAway(s0 == t0); });
			Bench{}.title("String::operator ==").run("t3", [&]() { doNotOptimizeAway(s3 == t3); });
			Bench{}.title("String::operator ==").run("t5", [&]() { doNotOptimizeAway(s5 == t5); });
			Bench{}.title("String::operator ==").run("t8", [&]() { doNotOptimizeAway(s8 == t8); });
			Bench{}.title("String::operator ==").run("t10", [&]() { doNotOptimizeAway(s10 == t10); });
			Bench{}.title("String::operator ==").run("t15", [&]() { doNotOptimizeAway(s15 == t15); });
			Bench{}.title("String::operator ==").run("t20", [&]() { doNotOptimizeAway(s20 == t20); });
			Bench{}.title("String::operator ==").run("t31", [&]() { doNotOptimizeAway(s31 == t31); });
			Bench{}.title("String::operator ==").run("t32", [&]() { doNotOptimizeAway(s32 == t32); });
			Bench{}.title("String::operator ==").run("t100", [&]() { doNotOptimizeAway(s100 == t100); });
			Bench{}.title("String::operator ==").run("t445", [&]() { doNotOptimizeAway(s445 == t445); });
			Bench{}.title("String::operator ==").run("t999", [&]() { doNotOptimizeAway(s999 == t999); });
			Bench{}.title("String::operator ==").run("t9999", [&]() { doNotOptimizeAway(s9999 == t9999); });
		}

		{
			String u0 = s0;
			String u3 = s3;
			String u5 = s5;
			String u8 = s8;
			String u10 = s10;
			String u15 = s15;
			String u20 = s20;
			String u31 = s31;
			String u32 = s32;
			String u100 = s100;
			String u445 = s445;
			String u999 = s999;
			String u9999 = s9999;

			++u3.front();
			++u5.front();
			++u8.front();
			++u10.front();
			++u15.front();
			++u20.front();
			++u31.front();
			++u32.front();
			++u100.front();
			++u445.front();
			++u999.front();
			++u9999.front();

			Bench{}.title("String::operator ==").run("u0", [&]() { doNotOptimizeAway(s0 == u0); });
			Bench{}.title("String::operator ==").run("u3", [&]() { doNotOptimizeAway(s3 == u3); });
			Bench{}.title("String::operator ==").run("u5", [&]() { doNotOptimizeAway(s5 == u5); });
			Bench{}.title("String::operator ==").run("u8", [&]() { doNotOptimizeAway(s8 == u8); });
			Bench{}.title("String::operator ==").run("u10", [&]() { doNotOptimizeAway(s10 == u10); });
			Bench{}.title("String::operator ==").run("u15", [&]() { doNotOptimizeAway(s15 == u15); });
			Bench{}.title("String::operator ==").run("u20", [&]() { doNotOptimizeAway(s20 == u20); });
			Bench{}.title("String::operator ==").run("u31", [&]() { doNotOptimizeAway(s31 == u31); });
			Bench{}.title("String::operator ==").run("u32", [&]() { doNotOptimizeAway(s32 == u32); });
			Bench{}.title("String::operator ==").run("u100", [&]() { doNotOptimizeAway(s100 == u100); });
			Bench{}.title("String::operator ==").run("u445", [&]() { doNotOptimizeAway(s445 == u445); });
			Bench{}.title("String::operator ==").run("u999", [&]() { doNotOptimizeAway(s999 == u999); });
			Bench{}.title("String::operator ==").run("u9999", [&]() { doNotOptimizeAway(s9999 == u9999); });
		}
	}
}


# endif
