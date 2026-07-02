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
	CHECK_EQ(LineString{ points }, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ LineString::container_type{ Vec2{ 4, 4 }, Vec2{ 5, 5 } } }, LineString{ Vec2{ 4, 4 }, Vec2{ 5, 5 } });
	CHECK_EQ(LineString(2, Vec2{ 1, 2 }), LineString{ Vec2{ 1, 2 }, Vec2{ 1, 2 } });
	CHECK_EQ(LineString(2), LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ points.begin(), points.end() }, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ ipoints }, LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } });

	const LineString reserved{ Arg::reserve = 4 };
	CHECK(reserved.isEmpty());
	CHECK(reserved.capacity() >= 4);
}

TEST_CASE("LineString.assignment")
{
	LineString line;
	const Array<Vec2> points{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	const Array<Point> ipoints{ Point{ 2, 2 }, Point{ 3, 3 } };

	CHECK_EQ(&(line = points), &line);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(&(line = LineString::container_type{ Vec2{ 4, 4 } }), &line);
	CHECK_EQ(line, LineString{ Vec2{ 4, 4 } });
	CHECK_EQ(&(line = ipoints), &line);
	CHECK_EQ(line, LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } });
	CHECK_EQ(&(line = { Vec2{ 5, 5 }, Vec2{ 6, 6 } }), &line);
	CHECK_EQ(line, LineString{ Vec2{ 5, 5 }, Vec2{ 6, 6 } });

	CHECK_EQ(&(line.assign(2, Vec2{ 7, 7 })), &line);
	CHECK_EQ(line, LineString{ Vec2{ 7, 7 }, Vec2{ 7, 7 } });
	CHECK_EQ(&(line.assign(points.begin(), points.end())), &line);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(&(line.assign({ Vec2{ 8, 8 } })), &line);
	CHECK_EQ(line, LineString{ Vec2{ 8, 8 } });
	CHECK_EQ(&(line.assign_range(points)), &line);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.element_access")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };

	CHECK_EQ(line.at(1), Vec2{ 1, 1 });
	CHECK_THROWS_AS(line.at(3), std::out_of_range);
	line[1] = Vec2{ 9, 9 };
	CHECK_EQ(std::as_const(line)[1], Vec2{ 9, 9 });
	CHECK_EQ(line.front(), Vec2{ 0, 0 });
	CHECK_EQ(line.back(), Vec2{ 2, 2 });
	CHECK_EQ(LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 } }.front(), Vec2{ 3, 3 });
	CHECK_EQ(LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 } }.back(), Vec2{ 4, 4 });
	CHECK_EQ(*line.data(), Vec2{ 0, 0 });
	CHECK_EQ(Array<Vec2>{ line.begin(), line.end() }, line.asArray());
	CHECK_EQ(Array<Vec2>{ line.rbegin(), line.rend() }, Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 9, 9 }, Vec2{ 0, 0 } });
	CHECK_EQ(line.cbegin(), std::as_const(line).begin());
	CHECK_EQ(line.cend(), std::as_const(line).end());
	CHECK_EQ(line.crbegin(), std::as_const(line).rbegin());
	CHECK_EQ(line.crend(), std::as_const(line).rend());
}

TEST_CASE("LineString.state_capacity")
{
	LineString line;
	CHECK(line.empty());
	CHECK(line.isEmpty());
	CHECK_FALSE(static_cast<bool>(line));
	CHECK_EQ(line.size(), 0);
	CHECK_EQ(line.ssize(), 0);
	CHECK_FALSE(line.indexInBounds(0));

	line.reserve(4);
	CHECK(line.capacity() >= 4);
	line << Vec2{ 1, 1 } << Vec2{ 2, 2 };
	CHECK_FALSE(line.empty());
	CHECK(static_cast<bool>(line));
	CHECK_EQ(line.size(), 2);
	CHECK_EQ(line.ssize(), 2);
	CHECK_EQ(line.size_bytes(), (sizeof(Vec2) * 2));
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
	CHECK_EQ(*it, Vec2{ 1, 1 });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 4, 4 } });

	it = line.insert((line.begin() + 2), points.begin(), points.end());
	CHECK_EQ(*it, Vec2{ 2, 2 });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } });

	it = line.insert((line.begin() + 1), 2, Vec2{ 9, 9 });
	CHECK_EQ(*it, Vec2{ 9, 9 });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 9, 9 }, Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } });

	it = line.insert(line.end(), { Vec2{ 5, 5 }, Vec2{ 6, 6 } });
	CHECK_EQ(*it, Vec2{ 5, 5 });
	CHECK_EQ(line.tail(2), LineString{ Vec2{ 5, 5 }, Vec2{ 6, 6 } });
}

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

TEST_CASE("LineString.emplace")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	auto it = line.emplace((line.begin() + 1), 1, 1);
	CHECK_EQ(*it, Vec2{ 1, 1 });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });

	auto& back = line.emplace_back(3, 3);
	CHECK_EQ(back, Vec2{ 3, 3 });
	auto& front = line.emplace_front(-1, -1);
	CHECK_EQ(front, Vec2{ -1, -1 });
	CHECK_EQ(line, LineString{ Vec2{ -1, -1 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } });
}

TEST_CASE("LineString.append_range")
{
	LineString line{ Vec2{ 0, 0 } };
	const Array<Vec2> points{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };

	line.append_range(points);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
}

TEST_CASE("LineString.erase")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	auto it = line.erase(line.begin() + 1);
	CHECK_EQ(*it, Vec2{ 2, 2 });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } });

	it = line.erase(line.begin(), line.begin() + 2);
	CHECK_EQ(*it, Vec2{ 3, 3 });
	CHECK_EQ(line, LineString{ Vec2{ 3, 3 } });
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

TEST_CASE("LineString.push_pop_resize_swap")
{
	LineString line{ Vec2{ 1, 1 } };
	line.push_back(Vec2{ 2, 2 });
	line.push_front(Vec2{ 0, 0 });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });

	line.pop_front();
	line.pop_back();
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 } });

	line.resize(3, Vec2{ 9, 9 });
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 9, 9 } });
	line.resize(2);
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 9, 9 } });
	line.pop_front_N(9);
	CHECK(line.isEmpty());

	line = { Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	line.pop_back_N(2);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 } });

	LineString other{ Vec2{ 5, 5 } };
	line.swap(other);
	CHECK_EQ(line, LineString{ Vec2{ 5, 5 } });
	CHECK_EQ(other, LineString{ Vec2{ 0, 0 } });
	swap(line, other);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 } });
	CHECK_EQ(other, LineString{ Vec2{ 5, 5 } });
}

TEST_CASE("LineString.subspan")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	auto span = line.subspan(1, 2);
	CHECK_EQ(span.size(), 2);
	span[0] = Vec2{ 9, 9 };
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 9, 9 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } });

	const auto constSpan = std::as_const(line).subspan(2, 2);
	CHECK_EQ(constSpan.size(), 2);
	CHECK_EQ(constSpan[0], Vec2{ 2, 2 });
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
		CHECK_EQ(&(line.append(other)), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		const LineString::container_type points{ Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		CHECK_EQ(&(line.append(points)), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		const Array<Point> points{ Point{ 1, 1 }, Point{ 2, 2 } };
		CHECK_EQ(&(line.append(points.begin(), points.end())), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		CHECK_EQ(&(line.append({ Vec2{ 1, 1 }, Vec2{ 2, 2 } })), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	}

	{
		LineString line{ Vec2{ 0, 0 } };
		CHECK_EQ(&(line.append(2, Vec2{ 1, 1 })), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } });
	}

	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
		line.append(line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } });
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
		CHECK_EQ(line.size(), source.size());
	}

	{
		std::mt19937 rng{ 12345 };
		const Vec2& point = source.choice(rng);
		CHECK(source.contains(point));
	}

	{
		std::mt19937 rng1{ 12345 };
		std::mt19937 rng2{ 12345 };
		CHECK_EQ(source.choice(3, rng1), LineString{ source.asArray().choice(3, rng2) });
		CHECK_EQ(source.choice(10).size(), source.size());
	}

	{
		LineString line{ Vec2{ 42, 42 } };
		CHECK_EQ(line.choice(), Vec2{ 42, 42 });
		CHECK_EQ(std::as_const(line).choice(), Vec2{ 42, 42 });
	}
}

TEST_CASE("LineString.chunk")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 }, Vec2{ 5, 5 }, Vec2{ 6, 6 } };

	CHECK_EQ(source.chunk(3), Array<LineString>{
		LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } },
		LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 }, Vec2{ 5, 5 } },
		LineString{ Vec2{ 6, 6 } },
	});
	CHECK(source.chunk(0).isEmpty());
	CHECK(LineString{}.chunk(3).isEmpty());
}

TEST_CASE("LineString.in_groups")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 }, Vec2{ 5, 5 }, Vec2{ 6, 6 } };

	CHECK_EQ(source.in_groups(3), Array<LineString>{
		LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } },
		LineString{ Vec2{ 3, 3 }, Vec2{ 4, 4 } },
		LineString{ Vec2{ 5, 5 }, Vec2{ 6, 6 } },
	});
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } }.in_groups(5), Array<LineString>{
		LineString{ Vec2{ 0, 0 } },
		LineString{ Vec2{ 1, 1 } },
	});
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
	CHECK_EQ(line.count(Vec2{ 0, 0 }), 2);
	CHECK_EQ(line.count(Vec2{ 9, 9 }), 0);
}

TEST_CASE("LineString.count_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 } };
	CHECK_EQ(line.count_if([](const Vec2& p) { return (p.y == 0); }), 3);
}

TEST_CASE("LineString.fetch")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK_EQ(line.fetch(1, Vec2{ 9, 9 }), Vec2{ 1, 1 });
	CHECK_EQ(line.fetch(9, Vec2{ 9, 9 }), Vec2{ 9, 9 });
}

TEST_CASE("LineString.indexOf")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	const auto index = line.indexOf(Vec2{ 1, 1 });
	CHECK(index.has_value());
	CHECK_EQ(*index, 1);
	CHECK_FALSE(line.indexOf(Vec2{ 9, 9 }).has_value());
}

TEST_CASE("LineString.each")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	line.each([](Vec2& p) { p.x += 10; });
	CHECK_EQ(line, LineString{ Vec2{ 10, 0 }, Vec2{ 11, 1 } });

	const LineString constLine{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
	double sum = 0;
	constLine.each([&](const Vec2& p) { sum += p.x; });
	CHECK_EQ(sum, 1);
}

TEST_CASE("LineString.each_index")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
	line.each_index([](size_t i, Vec2& p) { p.x = static_cast<double>(i); });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 2, 0 } });

	const LineString constLine{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	size_t indexSum = 0;
	constLine.each_index([&](size_t i, const Vec2&) { indexSum += i; });
	CHECK_EQ(indexSum, 3);
}

TEST_CASE("LineString.each_sindex")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
	line.each_sindex([](isize i, Vec2& p) { p.y = static_cast<double>(i); });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 0, 1 }, Vec2{ 0, 2 } });

	const LineString constLine{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	isize sindexSum = 0;
	constLine.each_sindex([&](isize i, const Vec2&) { sindexSum += i; });
	CHECK_EQ(sindexSum, 3);
}

TEST_CASE("LineString.fill")
{
	LineString line(3);
	CHECK_EQ(&(line.fill(Vec2{ 1, 1 })), &line);
	CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.filter")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK_EQ(line.filter([](const Vec2& p) { return (p.y == 0); }), LineString{ Vec2{ 0, 0 }, Vec2{ 2, 0 } });
}

TEST_CASE("LineString.map")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK_EQ(line.map([](const Vec2& p) { return p.x; }), Array<double>{ 0, 1, 2 });
}

TEST_CASE("LineString.operator >>")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	Array<Vec2> visited;
	line >> [&](const Vec2& p) { visited << p; };
	CHECK_EQ(visited, line.asArray());
	CHECK_EQ((line >> [](const Vec2& p) { return p.x; }), Array<double>{ 0, 1, 2 });
}

TEST_CASE("LineString.join")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK_EQ(line.join(U" / "), line.asArray().join(U" / "));
	CHECK_EQ(line.join(U" / ", U"[", U"]"), line.asArray().join(U" / ", U"[", U"]"));
}

TEST_CASE("LineString.reverse_each")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	Array<Vec2> visited;
	line.reverse_each([&](const Vec2& p) { visited << p; });
	CHECK_EQ(visited, Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });

	LineString moved{ line };
	moved.reverse_each([](Vec2& p) { p.y += 10; });
	CHECK_EQ(moved, LineString{ Vec2{ 0, 10 }, Vec2{ 1, 11 }, Vec2{ 2, 12 } });
}

TEST_CASE("LineString.slice")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.slice(1, 2), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.slice(1, 2), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
}

TEST_CASE("LineString.head")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.head(2), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.tail")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.tail(2), LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } });
}

TEST_CASE("LineString.take")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.take(2), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.take(2), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.take_while")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.take_while([](const Vec2& p) { return (p.x < 2); }), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.take_while([](const Vec2& p) { return (p.x < 2); }), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.values_at")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.values_at({ 3, 1 }), LineString{ Vec2{ 3, 3 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.head_span")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	auto head = line.head_span(2);
	CHECK_EQ(head.size(), 2);
	head[0] = Vec2{ 9, 9 };
	CHECK_EQ(line.front(), Vec2{ 9, 9 });
}

TEST_CASE("LineString.tail_span")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto tail = line.tail_span(2);
	CHECK_EQ(tail.size(), 2);
	CHECK_EQ(tail[0], Vec2{ 2, 2 });
}

TEST_CASE("LineString.head_view")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto head = line.head_view(2);
	CHECK_EQ(LineString{ head.begin(), head.end() }, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });

	Array<Vec2> rvalueHead;
	for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.head_view(2))
	{
		rvalueHead << p;
	}
	CHECK_EQ(rvalueHead, Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.tail_view")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto tail = line.tail_view(2);
	CHECK_EQ(LineString{ tail.begin(), tail.end() }, LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } });

	Array<Vec2> rvalueTail;
	for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.tail_view(2))
	{
		rvalueTail << p;
	}
	CHECK_EQ(rvalueTail, Array<Vec2>{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
}

TEST_CASE("LineString.reverse_view")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	const auto reversed = line.reverse_view();
	CHECK_EQ(LineString{ reversed.begin(), reversed.end() }, LineString{ Vec2{ 3, 3 }, Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });

	Array<Vec2> rvalueReversed;
	for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reverse_view())
	{
		rvalueReversed << p;
	}
	CHECK_EQ(rvalueReversed, Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
}

TEST_CASE("LineString.reverse")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	CHECK_EQ(&(line.reverse()), &line);
	CHECK_EQ(line, LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reverse(), LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
}

TEST_CASE("LineString.reversed")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.reversed(), LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reversed(), LineString{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
}

TEST_CASE("LineString.without")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.without(Vec2{ 0, 0 }), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.without(Vec2{ 0, 0 }), LineString{ Vec2{ 1, 1 } });
}

TEST_CASE("LineString.without_at")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.without_at(1), LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.without_at(1), LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } });
}

TEST_CASE("LineString.without_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.without_if([](const Vec2& p) { return (p.x == 0); }), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.without_if([](const Vec2& p) { return (p.x == 0); }), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
}

TEST_CASE("LineString.replace")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(&(line.replace(Vec2{ 0, 0 }, Vec2{ 9, 9 })), &line);
	CHECK_EQ(line, LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.replace(Vec2{ 0, 0 }, Vec2{ 9, 9 }), LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } });
}

TEST_CASE("LineString.replaced")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.replaced(Vec2{ 0, 0 }, Vec2{ 9, 9 }), LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 2, 2 } });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.replaced(Vec2{ 0, 0 }, Vec2{ 9, 9 }), LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } });
}

TEST_CASE("LineString.replace_if")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK_EQ(&(line.replace_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 })), &line);
	CHECK_EQ(line, LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 3, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } }.replace_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 }), LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } });
}

TEST_CASE("LineString.replaced_if")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } };
	CHECK_EQ(line.replaced_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 }), LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 }, Vec2{ 3, 1 } });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 }, Vec2{ 3, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } }.replaced_if([](const Vec2& p) { return (p.y == 0); }, Vec2{ 9, 9 }), LineString{ Vec2{ 9, 9 }, Vec2{ 1, 1 }, Vec2{ 9, 9 } });
}

TEST_CASE("LineString.rotate")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(&(line.rotate(2)), &line);
	CHECK_EQ(line, LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.rotate(1), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 0, 0 } });
}

TEST_CASE("LineString.rotated")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.rotated(1), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.rotated(2), LineString{ Vec2{ 2, 2 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.shuffle")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } };
	std::mt19937 rng1{ 12345 };
	std::mt19937 rng2{ 12345 };
	LineString shuffled = source;
	CHECK_EQ(&(shuffled.shuffle(rng1)), &shuffled);
	CHECK_EQ(shuffled, source.shuffled(rng2));
	CHECK_EQ(source, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } });

	for (const auto& point : source)
	{
		CHECK_EQ(shuffled.count(point), 1);
	}

	std::mt19937 rng3{ 12345 };
	CHECK_EQ(LineString{ source }.shuffle().size(), source.size());
	CHECK_EQ(LineString{ source }.shuffle(rng3).size(), source.size());
}

TEST_CASE("LineString.shuffled")
{
	const LineString source{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 4, 4 } };
	std::mt19937 rng1{ 12345 };
	std::mt19937 rng2{ 12345 };
	CHECK_EQ(LineString{ source }.shuffled(rng1), source.shuffled(rng2));
	CHECK_EQ(source.shuffled().size(), source.size());
}

TEST_CASE("LineString.unique_consecutive")
{
	LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } };
	CHECK_EQ(&(line.unique_consecutive()), &line);
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } }.unique_consecutive(), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.uniqued_consecutive")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } };
	CHECK_EQ(line.uniqued_consecutive(), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ line }.uniqued_consecutive(), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
}

TEST_CASE("LineString.sum")
{
	CHECK_EQ(LineString{ Vec2{ 0, 1 }, Vec2{ 2, 3 }, Vec2{ 4, 5 } }.sum(), Vec2{ 6, 9 });
	CHECK_EQ(LineString{}.sum(), Vec2{ 0, 0 });
}

TEST_CASE("LineString.num_vertices_segments")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK_EQ(line.num_vertices(), 3);
	CHECK_EQ(line.num_segments(), 2);
	CHECK_EQ(line.num_segments(CloseRing::Yes), 3);
	CHECK_EQ(LineString{ Vec2{ 0, 0 } }.num_segments(CloseRing::Yes), 0);
}

TEST_CASE("LineString.segment")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK_EQ(line.segment(0), Line{ Vec2{ 0, 0 }, Vec2{ 3, 0 } });
	CHECK_EQ(line.segment(1), Line{ Vec2{ 3, 0 }, Vec2{ 3, 4 } });
	CHECK_EQ(line.segment(2, CloseRing::Yes), Line{ Vec2{ 3, 4 }, Vec2{ 0, 0 } });
	CHECK_THROWS_AS(static_cast<void>(line.segment(2)), std::out_of_range);
}

TEST_CASE("LineString.tangent_normal")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK_EQ(line.tangentAtSegment(0), Vec2{ 1, 0 });
	CHECK_EQ(line.normalAtSegment(0), Vec2{ 0, -1 });
	CHECK_EQ(line.tangentAtVertex(0), Vec2{ 1, 0 });
	CHECK_EQ(line.normalAtVertex(0), Vec2{ 0, -1 });
	CHECK_EQ(line.tangentAtVertex(2), Vec2{ 0, 1 });
	CHECK_EQ(line.normalAtVertex(2), Vec2{ 1, 0 });
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
	CHECK_EQ(line.movedBy(1, -1), LineString{ Vec2{ 1, -1 }, Vec2{ 3, 2 } });
	CHECK_EQ(line.movedBy(Vec2{ -1, 2 }), LineString{ Vec2{ -1, 2 }, Vec2{ 1, 5 } });
	CHECK_EQ(line.withOffset(1, -1), LineString{ Vec2{ 1, -1 }, Vec2{ 3, 2 } });
	CHECK_EQ(line.withOffsetX(10), LineString{ Vec2{ 10, 0 }, Vec2{ 12, 3 } });
	CHECK_EQ(line.withOffsetY(10), LineString{ Vec2{ 0, 10 }, Vec2{ 2, 13 } });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 2, 3 } });

	LineString moved{ line };
	CHECK_EQ(&(moved.moveBy(1, -1)), &moved);
	CHECK_EQ(moved, LineString{ Vec2{ 1, -1 }, Vec2{ 3, 2 } });
	CHECK_EQ(LineString{ line }.movedBy(Vec2{ 1, 1 }), LineString{ Vec2{ 1, 1 }, Vec2{ 3, 4 } });
}

TEST_CASE("LineString.scale")
{
	const LineString line{ Vec2{ 1, 2 }, Vec2{ 3, 4 } };
	CHECK_EQ(line.scaledFromOrigin(2), LineString{ Vec2{ 2, 4 }, Vec2{ 6, 8 } });
	CHECK_EQ(line.scaledFromOrigin(2, 3), LineString{ Vec2{ 2, 6 }, Vec2{ 6, 12 } });
	CHECK_EQ(line.scaledFromOrigin(Vec2{ 2, 3 }), LineString{ Vec2{ 2, 6 }, Vec2{ 6, 12 } });
	CHECK_EQ(line.scaledFrom(Vec2{ 1, 1 }, 2), LineString{ Vec2{ 1, 3 }, Vec2{ 5, 7 } });
	CHECK_EQ(line.scaledFrom(Vec2{ 1, 1 }, 2, 3), LineString{ Vec2{ 1, 4 }, Vec2{ 5, 10 } });
	CHECK_EQ(line.scaledFrom(Vec2{ 1, 1 }, Vec2{ 2, 3 }), LineString{ Vec2{ 1, 4 }, Vec2{ 5, 10 } });
	CHECK_EQ(line, LineString{ Vec2{ 1, 2 }, Vec2{ 3, 4 } });

	LineString scaled{ line };
	CHECK_EQ(&(scaled.scaleFromOrigin(2)), &scaled);
	CHECK_EQ(scaled, LineString{ Vec2{ 2, 4 }, Vec2{ 6, 8 } });
	CHECK_EQ(&(scaled.scaleFrom(Vec2{ 2, 2 }, 0.5)), &scaled);
	CHECK_EQ(scaled, LineString{ Vec2{ 2, 3 }, Vec2{ 4, 5 } });
}

TEST_CASE("LineString.computeBoundingRect")
{
	const LineString line{ Vec2{ -1, -2 }, Vec2{ 3, 4 }, Vec2{ 0, 1 } };
	CHECK_EQ(line.computeBoundingRect(), RectF{ -1, -2, 4, 6 });
}

TEST_CASE("LineString.computeLength")
{
	CHECK_EQ(LineString{}.computeLength(), 0);
	CHECK_EQ(LineString{ Vec2{ 0, 0 } }.computeLength(CloseRing::Yes), 0);

	const LineString line{ Vec2{ 0, 0 }, Vec2{ 3, 0 }, Vec2{ 3, 4 } };
	CHECK_EQ(line.computeLength(), 7);
	CHECK_EQ(line.computeLength(CloseRing::Yes), 12);
}

TEST_CASE("LineString.simplified")
{
	CHECK_EQ(LineString{ Vec2{ 0, 0 } }.simplified(), LineString{ Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 2, 0 } }.simplified(0.1), LineString{ Vec2{ 0, 0 }, Vec2{ 2, 0 } });
}

TEST_CASE("LineString.densified")
{
	CHECK_EQ(LineString{ Vec2{ 0, 0 } }.densified(2.0), LineString{ Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 4, 0 } }.densified(2.0), LineString{ Vec2{ 0, 0 }, Vec2{ (4.0 / 3.0), 0 }, Vec2{ (8.0 / 3.0), 0 }, Vec2{ 4, 0 } });
}

TEST_CASE("LineString.catmullRom")
{
	const LineString shortLine{ Vec2{ 0, 0 } };
	CHECK_EQ(shortLine.catmullRom(4), shortLine);

	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 0 }, Vec2{ 1, 1 } };
	const LineString curve = line.catmullRom(2);
	CHECK_EQ(curve.size(), 5);
	CHECK_EQ(curve.front(), line.front());
	CHECK_EQ(curve.back(), line.back());

	const LineString closedCurve = line.catmullRom(CloseRing::Yes, 2);
	CHECK_EQ(closedCurve.size(), 7);
	CHECK_EQ(closedCurve.front(), line.front());
	CHECK_EQ(closedCurve.back(), line.front());
}

TEST_CASE("LineString.sort_by")
{
	const auto compareX = [](const Vec2& a, const Vec2& b) { return (a.x < b.x); };

	LineString line{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } };
	CHECK_EQ(&(line.sort_by(compareX)), &line);
	CHECK_EQ(line, LineString{ Vec2{ 0, 2 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } });
	CHECK_EQ(LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }.sort_by(compareX), LineString{ Vec2{ 0, 2 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } });
}

TEST_CASE("LineString.sorted_by")
{
	const auto compareY = [](const Vec2& a, const Vec2& b) { return (a.y < b.y); };
	const LineString line{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } };
	CHECK_EQ(line.sorted_by(compareY), LineString{ Vec2{ 2, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 2 } });
	CHECK_EQ(line, LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }.sorted_by(compareY), LineString{ Vec2{ 2, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 2 } });
}
