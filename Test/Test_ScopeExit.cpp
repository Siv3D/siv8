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

static constexpr void EmptyFunction() {}

[[nodiscard]]
static constexpr int32 Test1()
{
	int32 i = 100;
	{
		const ScopeExit se = [&i]() { ++i; };
	}

	return i;
}

[[nodiscard]]
static constexpr int32 Test2()
{
	int32 i = 100;
	{
		ScopeExit se = [&i]() { ++i; };
		se.release();
	}

	return i;
}

TEST_CASE("ScopeExit")
{
	{
		constexpr ScopeExit se = []() {};
	}

	{
		constexpr ScopeExit se{ []() {} };
	}

	{
		constexpr ScopeExit se = EmptyFunction;
	}

	{
		constexpr ScopeExit se{ EmptyFunction };
	}

	{
		static_assert(Test1() == 101);
		static_assert(Test2() == 100);
	}

	{
		int32 i = 0;
		{
			const ScopeExit se = [&i]() { ++i; };
			CHECK_EQ(i, 0);
		}

		CHECK_EQ(i, 1);
	}

	{
		int32 i = 0;
		{
			ScopeExit se = [&i]() { ++i; };
			se.release();
			CHECK_EQ(i, 0);
		}

		CHECK_EQ(i, 0);
	}

	{
		int32 i = 0;
		{
			ScopeExit se1 = [&i]() { ++i; };
			ScopeExit se2 = [&i]() { ++i; };
		}

		CHECK_EQ(i, 2);
	}

	{
		int32 i = 0;
		{
			ScopeExit se1 = [&i]() { ++i; };
			ScopeExit se2{ std::move(se1) };
		}

		CHECK_EQ(i, 1);
	}

	{
		int32 i = 0;
		{
			ScopeExit se1 = [&i]() { ++i; };
			se1.release();
			ScopeExit se2{ std::move(se1) };
		}

		CHECK_EQ(i, 0);
	}

	// [Siv3D ToDo] add more tests
}
