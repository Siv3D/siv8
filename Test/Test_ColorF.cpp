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

TEST_CASE("ColorF.fmt")
{
	constexpr ColorF c0{ 0, 0, 0, 0 };
	constexpr ColorF c1{ 0.001, 0.002, 0.003, 0.004 };
	constexpr ColorF c2{ 0.494, 0.5, 0.504, 0.506 };
	constexpr ColorF c3{ 0.9997, 0.9998, 0.9999, 1.0 };

	CHECK(U"{}"_fmt(c0) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(c1) == U"(0.001, 0.002, 0.003, 0.004)");
	CHECK(U"{}"_fmt(c2) == U"(0.494, 0.5, 0.504, 0.506)");
	CHECK(U"{}"_fmt(c3) == U"(0.9997, 0.9998, 0.9999, 1)");

	CHECK(U"{:.2f}"_fmt(c0) == U"(0.00, 0.00, 0.00, 0.00)");
	CHECK(U"{:.2f}"_fmt(c1) == U"(0.00, 0.00, 0.00, 0.00)");
	CHECK(U"{:.2f}"_fmt(c2) == U"(0.49, 0.50, 0.50, 0.51)");
	CHECK(U"{:.2f}"_fmt(c3) == U"(1.00, 1.00, 1.00, 1.00)");
}

TEST_CASE("ColorF.Format")
{
	constexpr ColorF c0{ 0, 0, 0, 0 };
	constexpr ColorF c1{ 0.001, 0.002, 0.003, 0.004 };
	constexpr ColorF c2{ 0.494, 0.5, 0.504, 0.506 };
	constexpr ColorF c3{ 0.9997, 0.9998, 0.9999, 1.0 };

	CHECK(Format(c0) == U"(0, 0, 0, 0)");
	CHECK(Format(c1) == U"(0.001, 0.002, 0.003, 0.004)");
	CHECK(Format(c2) == U"(0.494, 0.5, 0.504, 0.506)");
	CHECK(Format(c3) == U"(0.9997, 0.9998, 0.9999, 1)");
}
