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

TEST_CASE("SequenceFormatter")
{
	{
		const int32 ia[3] = { 1, 2, 3 };
		const double da[3] = { 1.111, 2.222, 3.333 };
		const String sa[4] = { U"aaa", U"bbb", U"あああ", U"いいい" };

		{
			FormatData formatData;
			Formatter(formatData, ia);
			CHECK_EQ(formatData.string, U"[1, 2, 3]");
		}

		{
			FormatData formatData;
			Formatter(formatData, da);
			CHECK_EQ(formatData.string, U"[1.111, 2.222, 3.333]");
		}

		{
			FormatData formatData;
			Formatter(formatData, sa);
			CHECK_EQ(formatData.string, U"[\"aaa\", \"bbb\", \"あああ\", \"いいい\"]");
		}
	}

	{
		const std::array<int32, 3> ai = { 1, 2, 3 };
		const std::array<double, 3> ad = { 1.111, 2.222, 3.333 };
		const std::array<String, 4> as = { U"aaa", U"bbb", U"あああ", U"いいい" };

		{
			FormatData formatData;
			Formatter(formatData, ai);
			CHECK_EQ(formatData.string, U"[1, 2, 3]");
		}

		{
			FormatData formatData;
			Formatter(formatData, ad);
			CHECK_EQ(formatData.string, U"[1.111, 2.222, 3.333]");
		}

		{
			FormatData formatData;
			Formatter(formatData, as);
			CHECK_EQ(formatData.string, U"[\"aaa\", \"bbb\", \"あああ\", \"いいい\"]");
		}
	}

	{
		const std::vector<int32> vi = { 1, 2, 3 };
		const std::vector<double> vd = { 1.111, 2.222, 3.333 };
		const std::vector<String> vs = { U"aaa", U"bbb", U"あああ", U"いいい" };

		{
			FormatData formatData;
			Formatter(formatData, vi);
			CHECK_EQ(formatData.string, U"[1, 2, 3]");
		}

		{
			FormatData formatData;
			Formatter(formatData, vd);
			CHECK_EQ(formatData.string, U"[1.111, 2.222, 3.333]");
		}

		{
			FormatData formatData;
			Formatter(formatData, vs);
			CHECK_EQ(formatData.string, U"[\"aaa\", \"bbb\", \"あああ\", \"いいい\"]");
		}
	}

	{
		const Array<int32> ai = { 1, 2, 3 };
		const Array<double> ad = { 1.111, 2.222, 3.333 };
		const Array<String> as = { U"aaa", U"bbb", U"あああ", U"いいい" };

		{
			FormatData formatData;
			Formatter(formatData, ai);
			CHECK_EQ(formatData.string, U"[1, 2, 3]");
		}

		{
			FormatData formatData;
			Formatter(formatData, ad);
			CHECK_EQ(formatData.string, U"[1.111, 2.222, 3.333]");
		}

		{
			FormatData formatData;
			Formatter(formatData, as);
			CHECK_EQ(formatData.string, U"[\"aaa\", \"bbb\", \"あああ\", \"いいい\"]");
		}
	}

	{
		const std::initializer_list<int32> il = { 1, 2, 3 };
		const std::initializer_list<double> dl = { 1.111, 2.222, 3.333 };
		const std::initializer_list<String> sl = { U"aaa", U"bbb", U"あああ", U"いいい" };

		{
			FormatData formatData;
			Formatter(formatData, il);
			CHECK_EQ(formatData.string, U"[1, 2, 3]");
		}

		{
			FormatData formatData;
			Formatter(formatData, dl);
			CHECK_EQ(formatData.string, U"[1.111, 2.222, 3.333]");
		}

		{
			FormatData formatData;
			Formatter(formatData, sl);
			CHECK_EQ(formatData.string, U"[\"aaa\", \"bbb\", \"あああ\", \"いいい\"]");
		}
	}
}
