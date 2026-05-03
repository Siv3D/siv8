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
