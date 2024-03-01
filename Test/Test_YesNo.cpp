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

TEST_CASE("YesNo")
{
	static_assert(YesNo<struct T1>{ true }.getBool() == true);
	static_assert(YesNo<struct T1>{ false }.getBool() == false);
	static_assert(YesNo{ YesNo<struct T1>{ true } }.getBool() == true);
	static_assert(YesNo{ YesNo<struct T1>{ false } }.getBool() == false);
	static_assert(YesNo<struct T1>{ true } ? true : false);
	static_assert(YesNo<struct T1>{ false } ? true : true);

	static_assert(std::is_trivially_copyable_v<YesNo<struct T1>> == true);
	static_assert(std::is_default_constructible_v<YesNo<struct T1>> == false);
	static_assert(std::is_convertible_v<YesNo<struct T1>, bool> == false);
	static_assert(std::is_convertible_v<YesNo<struct T1>, YesNo<struct T1>> == true);
	static_assert(std::is_convertible_v<YesNo<struct T1>, YesNo<struct T2>> == false);
}
