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

TEST_CASE("Types")
{
	static_assert(sizeof(int8) == 1);
	static_assert(sizeof(uint8) == 1);
	static_assert(sizeof(int16) == 2);
	static_assert(sizeof(uint16) == 2);
	static_assert(sizeof(int32) == 4);
	static_assert(sizeof(uint32) == 4);
	static_assert(sizeof(int64) == 8);
	static_assert(sizeof(uint64) == 8);
	static_assert(sizeof(char8) == 1);
	static_assert(sizeof(char16) == 2);
	static_assert(sizeof(char32) == 4);
	static_assert(sizeof(isize) == sizeof(std::ptrdiff_t));
}
