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

namespace
{
	template <class Range>
	Array<Point> Collect(Range&& range)
	{
		Array<Point> result;

		for (const auto& point : range)
		{
			result << point;
			REQUIRE(result.size() < 1024);
		}

		return result;
	}
}

TEST_CASE("Step2D.step")
{
	CHECK_EQ(Collect(step(Size{ 3, 2 })), Array<Point>{
		Point{ 0, 0 }, Point{ 1, 0 }, Point{ 2, 0 },
		Point{ 0, 1 }, Point{ 1, 1 }, Point{ 2, 1 },
	});
}

TEST_CASE("Step2D.step_start")
{
	CHECK_EQ(Collect(step(Point{ 10, 20 }, Size{ 3, 2 })), Array<Point>{
		Point{ 10, 20 }, Point{ 11, 20 }, Point{ 12, 20 },
		Point{ 10, 21 }, Point{ 11, 21 }, Point{ 12, 21 },
	});
}

TEST_CASE("Step2D.step_stride")
{
	CHECK_EQ(Collect(step(Point{ 10, 20 }, Size{ 3, 2 }, Size{ 2, 5 })), Array<Point>{
		Point{ 10, 20 }, Point{ 12, 20 }, Point{ 14, 20 },
		Point{ 10, 25 }, Point{ 12, 25 }, Point{ 14, 25 },
	});
}

TEST_CASE("Step2D.step_negative_stride")
{
	CHECK_EQ(Collect(step(Point{ 10, 20 }, Size{ 3, 2 }, Size{ -2, -5 })), Array<Point>{
		Point{ 10, 20 }, Point{ 8, 20 }, Point{ 6, 20 },
		Point{ 10, 15 }, Point{ 8, 15 }, Point{ 6, 15 },
	});
}

TEST_CASE("Step2D.step_empty")
{
	{
		const auto range = step(Size{ 0, 2 });
		CHECK(range.begin() == range.end());
		CHECK(Collect(range).isEmpty());
	}

	{
		const auto range = step(Size{ 2, 0 });
		CHECK(range.begin() == range.end());
		CHECK(Collect(range).isEmpty());
	}

	CHECK(Collect(step(Size{ -1, 2 })).isEmpty());
	CHECK(Collect(step(Size{ 2, -1 })).isEmpty());
	CHECK(Collect(step(Point{ 10, 20 }, Size{ 0, 2 })).isEmpty());
	CHECK(Collect(step(Point{ 10, 20 }, Size{ 2, 0 }, Size{ 2, 5 })).isEmpty());
}

TEST_CASE("Step2D.step_backward")
{
	CHECK_EQ(Collect(step_backward(Size{ 3, 2 })), Array<Point>{
		Point{ 2, 1 }, Point{ 1, 1 }, Point{ 0, 1 },
		Point{ 2, 0 }, Point{ 1, 0 }, Point{ 0, 0 },
	});
}

TEST_CASE("Step2D.step_backward_empty")
{
	{
		const auto range = step_backward(Size{ 0, 2 });
		CHECK(range.begin() == range.end());
		CHECK(Collect(range).isEmpty());
	}

	{
		const auto range = step_backward(Size{ 2, 0 });
		CHECK(range.begin() == range.end());
		CHECK(Collect(range).isEmpty());
	}

	CHECK(Collect(step_backward(Size{ -1, 2 })).isEmpty());
	CHECK(Collect(step_backward(Size{ 2, -1 })).isEmpty());
}

TEST_CASE("Step2D.reusable_range")
{
	const auto range = step(Point{ 10, 20 }, Size{ 2, 2 }, Size{ 3, 4 });
	const Array<Point> expected{
		Point{ 10, 20 }, Point{ 13, 20 },
		Point{ 10, 24 }, Point{ 13, 24 },
	};

	CHECK_EQ(Collect(range), expected);
	CHECK_EQ(Collect(range), expected);
	CHECK(range.begin() != range.end());
	CHECK_EQ(*range.begin(), Point{ 10, 20 });
}

TEST_CASE("Step2D.iterator")
{
	auto it = step(Point{ 3, 4 }, Size{ 2, 2 }, Size{ 10, 20 }).begin();
	CHECK_EQ(*it, Point{ 3, 4 });

	const auto old = it++;
	CHECK_EQ(*old, Point{ 3, 4 });
	CHECK_EQ(*it, Point{ 13, 4 });

	++it;
	CHECK_EQ(*it, Point{ 3, 24 });
}
