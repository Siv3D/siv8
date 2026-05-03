//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("LineString.insert_range")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 3 } };
		const Array<Vec2> points{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		const auto it = line.insert_range((line.begin() + 1), points);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } });
		CHECK_EQ(*it, Vec2{ 1, 1 });
	}

	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 3 } };
		const Array<Point> points{ Point{ 1, 1 }, Point{ 2, 2 } };
		const auto it = line.insert_range((line.begin() + 1), points);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } });
		CHECK_EQ(*it, Vec2{ 1, 1 });
	}
}

TEST_CASE("LineString.erase_at")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		CHECK_EQ(&(line.erase_at(1)), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	}

	{
		CHECK_EQ(
			LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.erase_at(1),
			LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	}
}

TEST_CASE("LineString.erase_all")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.erase_all(Vec2{ 0, 0 }), 2);
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(line.erase_all(Vec2{ 9, 9 }), 0);
}

TEST_CASE("LineString.erase_first")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK(line.erase_first(Vec2{ 0, 0 }));
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	CHECK_FALSE(line.erase_first(Vec2{ 9, 9 }));
}

TEST_CASE("LineString.erase_all_if")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK_EQ(line.erase_all_if([](const Vec2& p) { return (p.y == 0); }), 2);
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 3, 1 } });
}

TEST_CASE("LineString.erase_first_if")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK(line.erase_first_if([](const Vec2& p) { return (p.y == 0); }));
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } });
	CHECK_FALSE(line.erase_first_if([](const Vec2& p) { return (p.x == 9); }));
}
