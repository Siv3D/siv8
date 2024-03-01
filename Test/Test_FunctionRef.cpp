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

static int32 Eval(int32 x, int32 y, FunctionRef<int32(int32, int32)> f)
{
	return f(x, y);
}

static int32 Add(int32 x, int32 y)
{
	return (x + y);
}

TEST_CASE("FunctionRef")
{
	static_assert(sizeof(FunctionRef<int32(int32, int32)>) == (sizeof(void*) * 2));

	CHECK_EQ(Eval(1, 2, Add), 3);
	CHECK_EQ(Eval(1, 2, [](int32 x, int32 y) { return (x + y); }), 3);
}
