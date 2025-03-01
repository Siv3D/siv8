//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("Color.operator ==")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	static_assert(c0 == c0);
	static_assert(c1 == c1);
	static_assert(c2 == c2);

	static_assert(not(c0 == c1));
	static_assert(not(c0 == c2));
	static_assert(not(c1 == c2));
}

TEST_CASE("Color.operator !=")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	static_assert(c0 != c1);
	static_assert(c0 != c2);
	static_assert(c1 != c2);

	static_assert(not(c0 != c0));
	static_assert(not(c1 != c1));
	static_assert(not(c2 != c2));
}

TEST_CASE("Color.fmt")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	CHECK(U"{}"_fmt(c0) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(c1) == U"(1, 2, 3, 4)");
	CHECK(U"{}"_fmt(c2) == U"(255, 254, 253, 252)");

	CHECK(U"{:02X}"_fmt(c0) == U"(00, 00, 00, 00)");
	CHECK(U"{:02X}"_fmt(c1) == U"(01, 02, 03, 04)");
	CHECK(U"{:02X}"_fmt(c2) == U"(FF, FE, FD, FC)");
}

TEST_CASE("Color.Format")
{
	constexpr Color c0{ 0, 0, 0, 0 };
	constexpr Color c1{ 1, 2, 3, 4 };
	constexpr Color c2{ 255, 254, 253, 252 };

	CHECK(Format(c0) == U"(0, 0, 0, 0)");
	CHECK(Format(c1) == U"(1, 2, 3, 4)");
	CHECK(Format(c2) == U"(255, 254, 253, 252)");
}
