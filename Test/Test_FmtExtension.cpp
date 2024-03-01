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

TEST_CASE("FmtExtension.fmt::format")
{
	CHECK_EQ(fmt::format("{}", StringView{ U"あいう" }), "あいう");
	CHECK_EQ(fmt::format("{}", String{ U"あいう" }), "あいう");

	CHECK_EQ(fmt::format("{:>12}", StringView{ U"abc" }), "         abc");
	CHECK_EQ(fmt::format("{:>12}", String{ U"あいう" }), "      あいう");

	//fmt::format("{} {} {}", 10, 20);
	//fmt::format(U"{} {} {}", 10, 20);
}

TEST_CASE("FmtExtension._fmt")
{
	{
		CHECK_EQ(U"{}"_fmt(U"あいう"), U"あいう");
		CHECK_EQ(U"{}"_fmt(StringView{ U"あいう" }), U"あいう");
		CHECK_EQ(U"{}"_fmt(String{ U"あいう" }), U"あいう");

		CHECK_EQ(U"{:>12}"_fmt(U"abc"), U"         abc");
		CHECK_EQ(U"{:>12}"_fmt(StringView{ U"abc" }), U"         abc");
		CHECK_EQ(U"{:>12}"_fmt(String{ U"abc" }), U"         abc");

		CHECK_EQ(U"{:>12}"_fmt(U"あいう"), U"         あいう");
		CHECK_EQ(U"{:>12}"_fmt(StringView{ U"あいう" }), U"         あいう");
		CHECK_EQ(U"{:>12}"_fmt(String{ U"あいう" }), U"         あいう");
	}

	{
		std::array<int32, 3> vi{ 1, 2, 3 };
		const std::array<double, 3> vd{ 1.111, 2.222, 3.333 };
		const std::array<String, 4> vs{ U"aaa", U"bbb", U"あああ", U"いいい" };

		CHECK_EQ(U"{}"_fmt(vi), U"[1, 2, 3]");
		CHECK_EQ(U"{}"_fmt(vd), U"[1.111, 2.222, 3.333]");
		CHECK_EQ(U"{}"_fmt(vs), U"[aaa, bbb, あああ, いいい]");

		CHECK_EQ(U"{:.1f}"_fmt(vd), U"[1.1, 2.2, 3.3]");
	}

	{
		const Array<int32> vi{ 1, 2, 3 };
		const Array<double> vd{ 1.111, 2.222, 3.333 };
		const Array<String> vs{ U"aaa", U"bbb", U"あああ", U"いいい" };

		CHECK_EQ(U"{}"_fmt(vi), U"[1, 2, 3]");
		CHECK_EQ(U"{}"_fmt(vd), U"[1.111, 2.222, 3.333]");
		CHECK_EQ(U"{}"_fmt(vs), U"[aaa, bbb, あああ, いいい]");

		CHECK_EQ(U"{:.1f}"_fmt(vd), U"[1.1, 2.2, 3.3]");
	}

	{
		CHECK_EQ(U"{}"_fmt(none), U"none");
	}
}
