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

TEST_CASE("LineString.constructor")
{
	const Array<Vec2> points{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	const Array<Point> ipoints{ Point{ 2, 2 }, Point{ 3, 3 } };

	CHECK(LineString{}.isEmpty());
	CHECK((LineString{ points }) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((LineString{ LineString::container_type{ Vec2{ 4, 4 }, Vec2{ 5, 5 } } }) == (LineString{ Vec2{ 4, 4 }, Vec2{ 5, 5 } }));
	CHECK((LineString(2, Vec2{ 1, 2 })) == (LineString{ Vec2{ 1, 2 }, Vec2{ 1, 2 } }));
	CHECK((LineString(2)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 } }));
	CHECK((LineString{ points.begin(), points.end() }) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((LineString{ ipoints }) == (LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } }));

	const LineString reserved{ Arg::reserve = 4 };
	CHECK(reserved.isEmpty());
	CHECK(reserved.capacity() >= 4);
}

TEST_CASE("LineString.assignment")
{
	LineString line;
	const Array<Vec2> points{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	const Array<Point> ipoints{ Point{ 2, 2 }, Point{ 3, 3 } };

	CHECK((&(line = points)) == (&line));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((&(line = LineString::container_type{ Vec2{ 4, 4 } })) == (&line));
	CHECK((line) == (LineString{ Vec2{ 4, 4 } }));
	CHECK((&(line = ipoints)) == (&line));
	CHECK((line) == (LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } }));
	CHECK((&(line = { Vec2{ 5, 5 }, Vec2{ 6, 6 } })) == (&line));
	CHECK((line) == (LineString{ Vec2{ 5, 5 }, Vec2{ 6, 6 } }));

	CHECK((&(line.assign(2, Vec2{ 7, 7 }))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 7, 7 }, Vec2{ 7, 7 } }));
	CHECK((&(line.assign(points.begin(), points.end()))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((&(line.assign({ Vec2{ 8, 8 } }))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 8, 8 } }));
	CHECK((&(line.assign_range(points))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.element_access")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };

	CHECK((line.at(1)) == (Vec2{ 1, 1 }));
	CHECK_THROWS_AS(line.at(3), std::out_of_range);
	line[1] = Vec2{ 9, 9 };
	CHECK((std::as_const(line)[1]) == (Vec2{ 9, 9 }));
	CHECK((line.front()) == (Vec2{ 0, 0 }));
	CHECK((line.back()) == (Vec2{ 2, 2 }));
	CHECK((LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 } }.front()) == (Vec2{ 3, 3 }));
	CHECK((LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 } }.back()) == (Vec2{ 4, 4 }));
	CHECK((*line.data()) == (Vec2{ 0, 0 }));
	CHECK((Array<Vec2>{ line.begin(), line.end() }) == (line.asArray()));
	CHECK((Array<Vec2>{ line.rbegin(), line.rend() }) == (Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 9, 9 }, Vec2{ 0, 0 } }));
	CHECK((line.cbegin()) == (std::as_const(line).begin()));
	CHECK((line.cend()) == (std::as_const(line).end()));
	CHECK((line.crbegin()) == (std::as_const(line).rbegin()));
	CHECK((line.crend()) == (std::as_const(line).rend()));
}

TEST_CASE("LineString.state_capacity")
{
	LineString line;
	CHECK(line.empty());
	CHECK(line.isEmpty());
	CHECK_FALSE(static_cast<bool>(line));
	CHECK((line.size()) == (0u));
	CHECK((line.ssize()) == (0));
	CHECK_FALSE(line.indexInBounds(0));

	line.reserve(4);
	CHECK(line.capacity() >= 4u);
	line << Vec2{ 1, 1 } << Vec2{ 2, 2 };
	CHECK_FALSE(line.empty());
	CHECK(static_cast<bool>(line));
	CHECK((line.size()) == (2u));
	CHECK((line.ssize()) == (2));
	CHECK((line.size_bytes()) == ((sizeof(Vec2) * 2)));
	CHECK(line.indexInBounds(1));
	CHECK_FALSE(line.indexInBounds(2));
	CHECK(line.max_size() >= line.size());

	line.clear();
	CHECK(line.isEmpty());
	line.release();
	CHECK(line.isEmpty());
}

TEST_CASE("LineString.insert")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 4, 4 } };
	const Array<Vec2> points{ Vec2{ 2, 2 }, Vec2{ 3, 3 } };

	auto it = line.insert((line.begin() + 1), Vec2{ 1, 1 });
	CHECK((*it) == (Vec2{ 1, 1 }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 4, 4 } }));

	it = line.insert((line.begin() + 2), points.begin(), points.end());
	CHECK((*it) == (Vec2{ 2, 2 }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } }));

	it = line.insert((line.begin() + 1), 2, Vec2{ 9, 9 });
	CHECK((*it) == (Vec2{ 9, 9 }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 9, 9 }, Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } }));

	it = line.insert(line.end(), { Vec2{ 5, 5 }, Vec2{ 6, 6 } });
	CHECK((*it) == (Vec2{ 5, 5 }));
	CHECK((line.tail(2)) == (LineString{ Vec2{ 5, 5 }, Vec2{ 6, 6 } }));
}

TEST_CASE("LineString.insert_range")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 3 } };
		const Array<Vec2> points{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		const auto it = line.insert_range((line.begin() + 1), points);
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } }));
		CHECK((*it) == (Vec2{ 1, 1 }));
	}

	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 3 } };
		const Array<Point> points{ Point{ 1, 1 }, Point{ 2, 2 } };
		const auto it = line.insert_range((line.begin() + 1), points);
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } }));
		CHECK((*it) == (Vec2{ 1, 1 }));
	}
}

TEST_CASE("LineString.emplace")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	auto it = line.emplace((line.begin() + 1), 1, 1);
	CHECK((*it) == (Vec2{ 1, 1 }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));

	auto& back = line.emplace_back(3, 3);
	CHECK((back) == (Vec2{ 3, 3 }));
	auto& front = line.emplace_front(-1, -1);
	CHECK((front) == (Vec2{ -1, -1 }));
	CHECK((line) == (LineString{ Vec2{ -1, -1 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } }));
}

TEST_CASE("LineString.append_range")
{
	LineString line{ Vec2{ 0, 0 } };
	const Array<Vec2> points{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };

	line.append_range(points);
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
}

TEST_CASE("LineString.erase")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	auto it = line.erase(line.begin() + 1);
	CHECK((*it) == (Vec2{ 2, 2 }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } }));

	it = line.erase(line.begin(), line.begin() + 2);
	CHECK((*it) == (Vec2{ 3, 3 }));
	CHECK((line) == (LineString{ Vec2{ 3, 3 } }));
}

TEST_CASE("LineString.erase_at")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		CHECK((&(line.erase_at(1))) == (&line));
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } }));
	}

	{
		CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.erase_at(1)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } }));
	}
}

TEST_CASE("LineString.erase_all")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK((line.erase_all(Vec2{ 0, 0 })) == (2u));
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	CHECK((line.erase_all(Vec2{ 9, 9 })) == (0u));
}

TEST_CASE("LineString.erase_first")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK(line.erase_first(Vec2{ 0, 0 }));
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } }));
	CHECK_FALSE(line.erase_first(Vec2{ 9, 9 }));
}

TEST_CASE("LineString.erase_all_if")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK((line.erase_all_if([](const Vec2& p) { return (p.y == 0); })) == (2u));
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 3, 1 } }));
}

TEST_CASE("LineString.erase_first_if")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK(line.erase_first_if([](const Vec2& p) { return (p.y == 0); }));
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } }));
	CHECK_FALSE(line.erase_first_if([](const Vec2& p) { return (p.x == 9); }));
}

TEST_CASE("LineString.push_pop_resize_swap")
{
	LineString line{ Vec2{ 1, 1 } };
	line.push_back(Vec2{ 2, 2 });
	line.push_front(Vec2{ 0, 0 });
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));

	line.pop_front();
	line.pop_back();
	CHECK((line) == (LineString{ Vec2{ 1, 1 } }));

	line.resize(3, Vec2{ 9, 9 });
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 9, 9 } }));
	line.resize(2);
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 9, 9 } }));
	line.pop_front_N(9);
	CHECK(line.isEmpty());

	line = { Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	line.pop_back_N(2);
	CHECK((line) == (LineString{ Vec2{ 0, 0 } }));

	LineString other{ Vec2{ 5, 5 } };
	line.swap(other);
	CHECK((line) == (LineString{ Vec2{ 5, 5 } }));
	CHECK((other) == (LineString{ Vec2{ 0, 0 } }));
	swap(line, other);
	CHECK((line) == (LineString{ Vec2{ 0, 0 } }));
	CHECK((other) == (LineString{ Vec2{ 5, 5 } }));
}

TEST_CASE("LineString.subspan")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	auto span = line.subspan(1, 2);
	CHECK((span.size()) == (2u));
	span[0] = Vec2{ 9, 9 };
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 9, 9 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } }));

	const auto constSpan = std::as_const(line).subspan(2, 2);
	CHECK((constSpan.size()) == (2u));
	CHECK((constSpan[0]) == (Vec2{ 2, 2 }));
}

TEST_CASE("LineString.all")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK(line.all([](const Vec2& p) { return (0 <= p.x); }));
	CHECK_FALSE(line.all([](const Vec2& p) { return (p.y == 0); }));
}

TEST_CASE("LineString.any")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK(line.any([](const Vec2& p) { return (p.y == 1); }));
	CHECK_FALSE(line.any([](const Vec2& p) { return (p.x == 9); }));
}

TEST_CASE("LineString.none")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK(line.none([](const Vec2& p) { return (p.x == 9); }));
	CHECK_FALSE(line.none([](const Vec2& p) { return (p.y == 1); }));
}

TEST_CASE("LineString.append")
{
	{
		LineString line{ Vec2{ 0, 0 } };
		const LineString other{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		CHECK((&(line.append(other))) == (&line));
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		const LineString::container_type points{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		CHECK((&(line.append(points))) == (&line));
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		const Array<Point> points{ Point{ 1, 1 }, Point{ 2, 2 } };
		CHECK((&(line.append(points.begin(), points.end()))) == (&line));
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		CHECK((&(line.append({ Vec2{ 1, 1 }, Vec2{ 2, 2 } }))) == (&line));
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		CHECK((&(line.append(2, Vec2{ 1, 1 }))) == (&line));
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } }));
	}

	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
		line.append(line);
		CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	}
}

TEST_CASE("LineString.choice")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } };

	{
		LineString line{ source };
		std::mt19937 rng{ 12345 };
		line.choice(rng) = Vec2{ 99, 99 };
		CHECK(line.contains(Vec2{ 99, 99 }));
		CHECK((line.size()) == (source.size()));
	}

	{
		std::mt19937 rng{ 12345 };
		const Vec2& point = source.choice(rng);
		CHECK(source.contains(point));
	}

	{
		std::mt19937 rng1{ 12345 };
		std::mt19937 rng2{ 12345 };
		CHECK((source.choice(3, rng1)) == (LineString{ source.asArray().choice(3, rng2) }));
		CHECK((source.choice(10).size()) == (source.size()));
	}

	{
		LineString line{ Vec2{ 42, 42 } };
		CHECK((line.choice()) == (Vec2{ 42, 42 }));
		CHECK((std::as_const(line).choice()) == (Vec2{ 42, 42 }));
	}
}

TEST_CASE("LineString.chunk")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 }, Vec2{ 5, 5 }, Vec2{ 6, 6 } };

	CHECK((source.chunk(3)) == (Array<LineString>{
		LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } },
		LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 }, Vec2{ 5, 5 } },
		LineString{ Vec2{ 6, 6 } },
	}));
	CHECK(source.chunk(0).isEmpty());
	CHECK(LineString{}.chunk(3).isEmpty());
}

TEST_CASE("LineString.in_groups")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 }, Vec2{ 5, 5 }, Vec2{ 6, 6 } };

	CHECK((source.in_groups(3)) == (Array<LineString>{
		LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } },
		LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 } },
		LineString{ Vec2{ 5, 5 }, Vec2{ 6, 6 } },
	}));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }.in_groups(5)) == (Array<LineString>{
		LineString{ Vec2{ 0, 0 } },
		LineString{ Vec2{ 1, 1 } },
	}));
	CHECK(source.in_groups(0).isEmpty());
	CHECK(LineString{}.in_groups(3).isEmpty());
}

TEST_CASE("LineString.contains")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 } };
	CHECK(line.contains(Vec2{ 1, 1 }));
	CHECK_FALSE(line.contains(Vec2{ 9, 9 }));
}

TEST_CASE("LineString.contains_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 } };
	CHECK(line.contains_if([](const Vec2& p) { return (p.x == 2); }));
	CHECK_FALSE(line.contains_if([](const Vec2& p) { return (p.y == 9); }));
}

TEST_CASE("LineString.count")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 } };
	CHECK((line.count(Vec2{ 0, 0 })) == (2));
	CHECK((line.count(Vec2{ 9, 9 })) == (0));
}

TEST_CASE("LineString.count_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 } };
	CHECK((line.count_if([](const Vec2& p) { return (p.y == 0); })) == (3));
}

TEST_CASE("LineString.fetch")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK((line.fetch(1, Vec2{ 9, 9 })) == (Vec2{ 1, 1 }));
	CHECK((line.fetch(9, Vec2{ 9, 9 })) == (Vec2{ 9, 9 }));
}

TEST_CASE("LineString.indexOf")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	const auto index = line.indexOf(Vec2{ 1, 1 });
	CHECK(index.has_value());
	CHECK((*index) == (1u));
	CHECK_FALSE(line.indexOf(Vec2{ 9, 9 }).has_value());
}

TEST_CASE("LineString.each")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	line.each([](Vec2& p) { p.x += 10; });
	CHECK((line) == (LineString{ Vec2{ 10, 0 }, Vec2{ 11, 1 } }));

	const LineString constLine{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	double sum = 0;
	constLine.each([&](const Vec2& p) { sum += p.x; });
	CHECK((sum) == (1));
}

TEST_CASE("LineString.each_index")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
	line.each_index([](size_t i, Vec2& p) { p.x = static_cast<double>(i); });
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 2, 0 } }));

	const LineString constLine{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	size_t indexSum = 0;
	constLine.each_index([&](size_t i, const Vec2&) { indexSum += i; });
	CHECK((indexSum) == (3u));
}

TEST_CASE("LineString.each_sindex")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
	line.each_sindex([](isize i, Vec2& p) { p.y = static_cast<double>(i); });
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 0, 1 }, Vec2{ 0, 2 } }));

	const LineString constLine{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	isize sindexSum = 0;
	constLine.each_sindex([&](isize i, const Vec2&) { sindexSum += i; });
	CHECK((sindexSum) == (3));
}

TEST_CASE("LineString.fill")
{
	LineString line(3);
	CHECK((&(line.fill(Vec2{ 1, 1 }))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.filter")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK((line.filter([](const Vec2& p) { return (p.y == 0); })) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 0 } }));
}

TEST_CASE("LineString.map")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK((line.map([](const Vec2& p) { return p.x; })) == (Array<double>{ 0, 1, 2 }));
}

TEST_CASE("LineString.operator >>")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	Array<Vec2> visited;
	line >> [&](const Vec2& p) { visited << p; };
	CHECK((visited) == (line.asArray()));
	CHECK(((line >> [](const Vec2& p) { return p.x; })) == (Array<double>{ 0, 1, 2 }));
}

TEST_CASE("LineString.join")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK((line.join(U" / ")) == (line.asArray().join(U" / ")));
	CHECK((line.join(U" / ", U"[", U"]")) == (line.asArray().join(U" / ", U"[", U"]")));
}

TEST_CASE("LineString.reverse_each")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	Array<Vec2> visited;
	line.reverse_each([&](const Vec2& p) { visited << p; });
	CHECK((visited) == (Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));

	LineString moved{ line };
	moved.reverse_each([](Vec2& p) { p.y += 10; });
	CHECK((moved) == (LineString{ Vec2{ 0, 10 }, Vec2{ 1, 11 }, Vec2{ 2, 12 } }));
}

TEST_CASE("LineString.slice")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.slice(1, 2)) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.slice(1, 2)) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
}

TEST_CASE("LineString.head")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.take(2)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.tail")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.tail(2)) == (LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } }));
}

TEST_CASE("LineString.take")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.take(2)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.take(2)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.take_while")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.take_while([](const Vec2& p) { return (p.x < 2); })) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.take_while([](const Vec2& p) { return (p.x < 2); })) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.values_at")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.values_at({ 3, 1 })) == (LineString{ Vec2{ 3, 3 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.head_span")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	auto head = line.head_span(2);
	CHECK((head.size()) == (2u));
	head[0] = Vec2{ 9, 9 };
	CHECK((line.front()) == (Vec2{ 9, 9 }));
}

TEST_CASE("LineString.tail_span")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto tail = line.tail_span(2);
	CHECK((tail.size()) == (2u));
	CHECK((tail[0]) == (Vec2{ 2, 2 }));
}

TEST_CASE("LineString.head_view")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto head = line.head_view(2);
	CHECK((LineString{ head.begin(), head.end() }) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));

	Array<Vec2> rvalueHead;
	for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.head_view(2))
	{
		rvalueHead << p;
	}
	CHECK((rvalueHead) == (Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.tail_view")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto tail = line.tail_view(2);
	CHECK((LineString{ tail.begin(), tail.end() }) == (LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } }));

	Array<Vec2> rvalueTail;
	for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.tail_view(2))
	{
		rvalueTail << p;
	}
	CHECK((rvalueTail) == (Array<Vec2>{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
}

TEST_CASE("LineString.reverse_view")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto reversed = line.reverse_view();
	CHECK((LineString{ reversed.begin(), reversed.end() }) == (LineString{ Vec2{ 3, 3 }, Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));

	Array<Vec2> rvalueReversed;
	for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reverse_view())
	{
		rvalueReversed << p;
	}
	CHECK((rvalueReversed) == (Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
}

TEST_CASE("LineString.reverse")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	CHECK((&(line.reverse())) == (&line));
	CHECK((line) == (LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reverse()) == (LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
}

TEST_CASE("LineString.reversed")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	CHECK((line.reversed()) == (LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reversed()) == (LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
}

TEST_CASE("LineString.without")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK((line.without(Vec2{ 0, 0 })) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.without(Vec2{ 0, 0 })) == (LineString{ Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.without_at")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK((line.without_at(1)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.without_at(1)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } }));
}

TEST_CASE("LineString.without_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK((line.without_if([](const Vec2& p) { return (p.x == 0); })) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.without_if([](const Vec2& p) { return (p.x == 0); })) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } }));
}

TEST_CASE("LineString.replace")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK((&(line.replace(Vec2{ 0, 0 }, Vec2{ 9, 9 }))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.replace(Vec2{ 0, 0 }, Vec2{ 9, 9 })) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } }));
}

TEST_CASE("LineString.replaced")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK((line.replaced(Vec2{ 0, 0 }, Vec2{ 9, 9 })) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 2, 2 } }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.replaced(Vec2{ 0, 0 }, Vec2{ 9, 9 })) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } }));
}

TEST_CASE("LineString.replace_if")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK((&(line.replace_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 }))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 3, 1 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } }.replace_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 })) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } }));
}

TEST_CASE("LineString.replaced_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK((line.replaced_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 })) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 3, 1 } }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } }.replaced_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 })) == (LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } }));
}

TEST_CASE("LineString.rotate")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((&(line.rotate(2))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.rotate(1)) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 0, 0 } }));
}

TEST_CASE("LineString.rotated")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK((line.rotated(1)) == (LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 0, 0 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.rotated(2)) == (LineString{ Vec2{ 2, 2 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.shuffle")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } };
	std::mt19937 rng1{ 12345 };
	std::mt19937 rng2{ 12345 };
	LineString shuffled = source;
	CHECK((&(shuffled.shuffle(rng1))) == (&shuffled));
	CHECK((shuffled) == (source.shuffled(rng2)));
	CHECK((source) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } }));

	for (const auto& point : source)
	{
		CHECK((shuffled.count(point)) == (1));
	}

	std::mt19937 rng3{ 12345 };
	CHECK((LineString{ source }.shuffle().size()) == (source.size()));
	CHECK((LineString{ source }.shuffle(rng3).size()) == (source.size()));
}

TEST_CASE("LineString.shuffled")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } };
	std::mt19937 rng1{ 12345 };
	std::mt19937 rng2{ 12345 };
	CHECK((LineString{ source }.shuffled(rng1)) == (source.shuffled(rng2)));
	CHECK((source.shuffled().size()) == (source.size()));
}

TEST_CASE("LineString.unique_consecutive")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } };
	CHECK((&(line.unique_consecutive())) == (&line));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } }.unique_consecutive()) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }));
}

TEST_CASE("LineString.uniqued_consecutive")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } };
	CHECK((line.uniqued_consecutive()) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
	CHECK((LineString{ line }.uniqued_consecutive()) == (LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }));
}

TEST_CASE("LineString.sum")
{
	CHECK((LineString{ Vec2{ 0, 1 }, Vec2{ 2, 3 }, Vec2{ 4, 5 } }.sum()) == (Vec2{ 6, 9 }));
	CHECK((LineString{}.sum()) == (Vec2{ 0, 0 }));
}

TEST_CASE("LineString.vertexCount_segmentCount")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK((line.vertexCount()) == (3u));
	CHECK((line.segmentCount()) == (2u));
	CHECK((line.segmentCount(CloseRing::Yes)) == (3u));
	CHECK((LineString{ Vec2{ 0, 0 } }.segmentCount(CloseRing::Yes)) == (0u));
}

TEST_CASE("LineString.segment")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK((line.segment(0)) == (Line{ Vec2{ 0, 0 }, Vec2{ 3, 0 } }));
	CHECK((line.segment(1)) == (Line{ Vec2{ 3, 0 }, Vec2{ 3, 4 } }));
	CHECK((line.segment(2, CloseRing::Yes)) == (Line{ Vec2{ 3, 4 }, Vec2{ 0, 0 } }));
	CHECK_THROWS_AS(static_cast<void>(line.segment(2)), std::out_of_range);
}

TEST_CASE("LineString.tangent_normal")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK((line.tangentAtSegment(0)) == (Vec2{ 1, 0 }));
	CHECK((line.normalAtSegment(0)) == (Vec2{ 0, -1 }));
	CHECK((line.tangentAtVertex(0)) == (Vec2{ 1, 0 }));
	CHECK((line.normalAtVertex(0)) == (Vec2{ 0, -1 }));
	CHECK((line.tangentAtVertex(2)) == (Vec2{ 0, 1 }));
	CHECK((line.normalAtVertex(2)) == (Vec2{ 1, 0 }));
	CHECK_THROWS_AS(static_cast<void>(line.tangentAtVertex(3)), std::out_of_range);
	CHECK_THROWS_AS(static_cast<void>(line.normalAtSegment(2)), std::out_of_range);

	const Vec2 shortTangent = LineString{ Vec2{ 0, 0 } }.tangentAtVertex(0);
	const Vec2 shortNormal = LineString{ Vec2{ 0, 0 } }.normalAtSegment(0);
	CHECK(IsNaN(shortTangent.x));
	CHECK(IsNaN(shortTangent.y));
	CHECK(IsNaN(shortNormal.x));
	CHECK(IsNaN(shortNormal.y));
}

TEST_CASE("LineString.move_offset")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 2, 3 } };
	CHECK((line.movedBy(1, -1)) == (LineString{ Vec2{ 1, -1 }, Vec2{ 3, 2 } }));
	CHECK((line.movedBy(Vec2{ -1, 2 })) == (LineString{ Vec2{ -1, 2 }, Vec2{ 1, 5 } }));
	CHECK((line.withOffset(1, -1)) == (LineString{ Vec2{ 1, -1 }, Vec2{ 3, 2 } }));
	CHECK((line.withOffsetX(10)) == (LineString{ Vec2{ 10, 0 }, Vec2{ 12, 3 } }));
	CHECK((line.withOffsetY(10)) == (LineString{ Vec2{ 0, 10 }, Vec2{ 2, 13 } }));
	CHECK((line) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 3 } }));

	LineString moved{ line };
	CHECK((&(moved.moveBy(1, -1))) == (&moved));
	CHECK((moved) == (LineString{ Vec2{ 1, -1 }, Vec2{ 3, 2 } }));
	CHECK((LineString{ line }.movedBy(Vec2{ 1, 1 })) == (LineString{ Vec2{ 1, 1 }, Vec2{ 3, 4 } }));
}

TEST_CASE("LineString.scale")
{
	const LineString line{ Vec2{ 1, 2 }, Vec2{ 3, 4 } };
	CHECK((line.scaledFromOrigin(2)) == (LineString{ Vec2{ 2, 4 }, Vec2{ 6, 8 } }));
	CHECK((line.scaledFromOrigin(2, 3)) == (LineString{ Vec2{ 2, 6 }, Vec2{ 6, 12 } }));
	CHECK((line.scaledFromOrigin(Vec2{ 2, 3 })) == (LineString{ Vec2{ 2, 6 }, Vec2{ 6, 12 } }));
	CHECK((line.scaledFrom(Vec2{ 1, 1 }, 2)) == (LineString{ Vec2{ 1, 3 }, Vec2{ 5, 7 } }));
	CHECK((line.scaledFrom(Vec2{ 1, 1 }, 2, 3)) == (LineString{ Vec2{ 1, 4 }, Vec2{ 5, 10 } }));
	CHECK((line.scaledFrom(Vec2{ 1, 1 }, Vec2{ 2, 3 })) == (LineString{ Vec2{ 1, 4 }, Vec2{ 5, 10 } }));
	CHECK((line) == (LineString{ Vec2{ 1, 2 }, Vec2{ 3, 4 } }));

	LineString scaled{ line };
	CHECK((&(scaled.scaleFromOrigin(2))) == (&scaled));
	CHECK((scaled) == (LineString{ Vec2{ 2, 4 }, Vec2{ 6, 8 } }));
	CHECK((&(scaled.scaleFrom(Vec2{ 2, 2 }, 0.5))) == (&scaled));
	CHECK((scaled) == (LineString{ Vec2{ 2, 3 }, Vec2{ 4, 5 } }));
}

TEST_CASE("LineString.computeBoundingRect")
{
	const LineString line{ Vec2{ -1, -2 }, Vec2{ 3, 4 }, Vec2{ 0, 1 } };
	CHECK((line.computeBoundingRect()) == (RectF{ -1, -2, 4, 6 }));
}

TEST_CASE("LineString.computeLength")
{
	CHECK((LineString{}.computeLength()) == (0));
	CHECK((LineString{ Vec2{ 0, 0 } }.computeLength(CloseRing::Yes)) == (0));

	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK((line.computeLength()) == (7));
	CHECK((line.computeLength(CloseRing::Yes)) == (12));
}

TEST_CASE("LineString.simplified")
{
	CHECK((LineString{ Vec2{ 0, 0 } }.simplified()) == (LineString{ Vec2{ 0, 0 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 2, 0 } }.simplified(0.1)) == (LineString{ Vec2{ 0, 0 }, Vec2{ 2, 0 } }));
}

TEST_CASE("LineString.densified")
{
	CHECK((LineString{ Vec2{ 0, 0 } }.densified(2.0)) == (LineString{ Vec2{ 0, 0 } }));
	CHECK((LineString{ Vec2{ 0, 0 }, Vec2{ 4, 0 } }.densified(2.0)) == (LineString{ Vec2{ 0, 0 }, Vec2{ (4.0 / 3.0), 0 }, Vec2{ (8.0 / 3.0), 0 }, Vec2{ 4, 0 } }));
}

TEST_CASE("LineString.catmullRom")
{
	const LineString shortLine{ Vec2{ 0, 0 } };
	CHECK((shortLine.catmullRom(4)) == (shortLine));

	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 1, 1 } };
	const LineString curve = line.catmullRom(2);
	CHECK((curve.size()) == (5u));
	CHECK((curve.front()) == (line.front()));
	CHECK((curve.back()) == (line.back()));

	const LineString closedCurve = line.catmullRom(CloseRing::Yes, 2);
	CHECK((closedCurve.size()) == (7u));
	CHECK((closedCurve.front()) == (line.front()));
	CHECK((closedCurve.back()) == (line.front()));
}

TEST_CASE("LineString.sort_by")
{
	const auto compareX = [](const Vec2& a, const Vec2& b) { return (a.x < b.x); };

	LineString line{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } };
	CHECK((&(line.sort_by(compareX))) == (&line));
	CHECK((line) == (LineString{ Vec2{ 0, 2 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } }));
	CHECK((LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }.sort_by(compareX)) == (LineString{ Vec2{ 0, 2 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } }));
}

TEST_CASE("LineString.sorted_by")
{
	const auto compareY = [](const Vec2& a, const Vec2& b) { return (a.y < b.y); };
	const LineString line{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } };
	CHECK((line.sorted_by(compareY)) == (LineString{ Vec2{ 2, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 2 } }));
	CHECK((line) == (LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }));
	CHECK((LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }.sorted_by(compareY)) == (LineString{ Vec2{ 2, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 2 } }));
}


namespace
{
	struct VertexArrayAdapter
	{
		Array<Vec2> values;
		const Array<Vec2>& asArray() const& { return values; }
		Array<Vec2> asArray() && { return std::move(values); }
	};
	struct InvalidVertexArrayAdapter
	{
		Array<int32> asArray() const { return {}; }
	};
}

TEST_CASE("LineString.array_like_contract")
{
	static_assert(not std::is_constructible_v<LineString, InvalidVertexArrayAdapter>);
	static_assert(not std::is_assignable_v<LineString&, InvalidVertexArrayAdapter>);
	static_assert(std::same_as<decltype(LineString{}.append(LineString{})), LineString>);
	static_assert(std::same_as<decltype(LineString{}.fill(Vec2{})), LineString>);
	static_assert(std::same_as<decltype(LineString{}.moveBy(1, 2)), LineString>);
	static_assert(std::same_as<decltype(LineString{}.choice()), Vec2>);
	static_assert(std::same_as<decltype(LineString{} << Vec2{}), LineString>);
	VertexArrayAdapter adapter{ { Vec2{ 0, 1 }, Vec2{ 2, 3 }, Vec2{ 4, 5 } } };
	LineString line{ adapter };
	line = VertexArrayAdapter{ adapter.values };
	CHECK((line.asArray()) == (adapter.values));
	CHECK((line.get_if(1)) == (&line[1]));
	CHECK((std::as_const(line).get_if(1)) == (&line[1]));
	CHECK((line.get_if(line.size())) == (nullptr));
	CHECK((LineString{}.indexOf_if([](Vec2) { return true; })) == (none));
	CHECK((line.find_if([](Vec2 p) { return p.x > 1; })) == (&line[1]));
	CHECK((line.find_if([](Vec2 p) { return p.x < 0; })) == (nullptr));
	CHECK((line.indexOf_if([](Vec2 p) { return p.y == 5; })) == (Optional<size_t>{ 2 }));
	CHECK((line.drop(1)) == (LineString{ Vec2{ 2, 3 }, Vec2{ 4, 5 } }));
	CHECK(line.drop(100).isEmpty());
	CHECK((line.drop_while([](Vec2 p) { return p.x < 2; })) == (line.drop(1)));
	CHECK((line.slice(1)) == (line.drop(1)));
	CHECK(line.slice(3).isEmpty());
	CHECK_THROWS_AS((void) line.slice(4), std::out_of_range);
	CHECK_THROWS_AS((void) LineString{}.choice(), std::out_of_range);
	CHECK((line.map(&Vec2::x)) == (Array<double>{ 0, 2, 4 }));

	line.reserve(32);
	const auto storage = line.data();
	auto result = std::move(line).drop(1).filter([](Vec2 p) { return p.x == 4; }).moveBy(1, 2);
	CHECK((result) == (LineString{ Vec2{ 5, 7 } }));
	CHECK((result.data()) == (storage));
	CHECK(result.capacity() >= 32);
	result.append(std::move(result));
	CHECK((result.size()) == (size_t{ 1 }));
	result.append(result);
	CHECK((result.size()) == (size_t{ 2 }));
	CHECK((result[0]) == (result[1]));
	CHECK_THROWS_AS((void) result.filter([](Vec2) -> bool { throw std::runtime_error("predicate"); }), std::runtime_error);
}
