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

TEST_CASE("TupleFormatter")
{
	{
		const std::tuple<int32, String, double, char32, Optional<int32>> t(111, U"bbb", 123.4567, U'あ', none);
		FormatData formatData;
		Formatter(formatData, t);
		CHECK_EQ(formatData.string, U"(111, \"bbb\", 123.4567, 'あ', none)");
	}
}
