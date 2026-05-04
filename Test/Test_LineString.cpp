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

TEST_CASE("LineString.all_any_none")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK(line.all([](const Vec2& p) { return (0 <= p.x); }));
	CHECK_FALSE(line.all([](const Vec2& p) { return (p.y == 0); }));
	CHECK(line.any([](const Vec2& p) { return (p.y == 1); }));
	CHECK_FALSE(line.any([](const Vec2& p) { return (p.x == 9); }));
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

TEST_CASE("LineString.contains_count")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 0 } };
	CHECK(line.contains(Vec2{ 1, 1 }));
	CHECK_FALSE(line.contains(Vec2{ 9, 9 }));
	CHECK(line.contains_if([](const Vec2& p) { return (p.x == 2); }));
	CHECK_FALSE(line.contains_if([](const Vec2& p) { return (p.y == 9); }));
	CHECK_EQ(line.count(Vec2{ 0, 0 }), 2);
	CHECK_EQ(line.count(Vec2{ 9, 9 }), 0);
	CHECK_EQ(line.count_if([](const Vec2& p) { return (p.y == 0); }), 3);
}

TEST_CASE("LineString.fetch_indexOf")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
	CHECK_EQ(line.fetch(1, Vec2{ 9, 9 }), Vec2{ 1, 1 });
	CHECK_EQ(line.fetch(9, Vec2{ 9, 9 }), Vec2{ 9, 9 });

	const auto index = line.indexOf(Vec2{ 1, 1 });
	CHECK(index.has_value());
	CHECK_EQ(*index, 1);
	CHECK_FALSE(line.indexOf(Vec2{ 9, 9 }).has_value());
}

TEST_CASE("LineString.each")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
		line.each([](Vec2& p) { p.x += 10; });
		CHECK_EQ(line, LineString{ Vec2{ 10, 0 }, Vec2{ 11, 1 } });
	}

	{
		const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 } };
		double sum = 0;
		line.each([&](const Vec2& p) { sum += p.x; });
		CHECK_EQ(sum, 1);
	}

	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 0, 0 } };
		line.each_index([](size_t i, Vec2& p) { p.x = static_cast<double>(i); });
		line.each_sindex([](isize i, Vec2& p) { p.y = static_cast<double>(i); });
		CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	}

	{
		const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } };
		size_t indexSum = 0;
		isize sindexSum = 0;
		line.each_index([&](size_t i, const Vec2&) { indexSum += i; });
		line.each_sindex([&](isize i, const Vec2&) { sindexSum += i; });
		CHECK_EQ(indexSum, 3);
		CHECK_EQ(sindexSum, 3);
	}
}

TEST_CASE("LineString.fill_filter_map_join")
{
	{
		LineString line(3);
		CHECK_EQ(&(line.fill(Vec2{ 1, 1 })), &line);
		CHECK_EQ(line, LineString{ Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 1, 1 } });
	}

	{
		const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } };
		CHECK_EQ(line.filter([](const Vec2& p) { return (p.y == 0); }), LineString{ Vec2{ 0, 0 }, Vec2{ 2, 0 } });
		CHECK_EQ(line.map([](const Vec2& p) { return p.x; }), Array<double>{ 0, 1, 2 });
		CHECK_EQ(line.join(U" / "), line.asArray().join(U" / "));
		CHECK_EQ(line.join(U" / ", U"[", U"]"), line.asArray().join(U" / ", U"[", U"]"));
	}
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

TEST_CASE("LineString.slice_head_tail_take")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.slice(1, 2), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.slice(1, 2), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(line.head(2), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(line.tail(2), LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } });
	CHECK_EQ(line.take(2), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.take(2), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.take_while_values_at")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
	CHECK_EQ(line.take_while([](const Vec2& p) { return (p.x < 2); }), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.take_while([](const Vec2& p) { return (p.x < 2); }), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	CHECK_EQ(line.values_at({ 3, 1 }), LineString{ Vec2{ 3, 3 }, Vec2{ 1, 1 } });
}

TEST_CASE("LineString.span_view")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
		auto head = line.head_span(2);
		CHECK_EQ(head.size(), 2);
		head[0] = Vec2{ 9, 9 };
		CHECK_EQ(line.front(), Vec2{ 9, 9 });

		const LineString& cref = line;
		const auto tail = cref.tail_span(2);
		CHECK_EQ(tail.size(), 2);
		CHECK_EQ(tail[0], Vec2{ 2, 2 });
	}

	{
		const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
		const auto head = line.head_view(2);
		const auto tail = line.tail_view(2);
		const auto reversed = line.reverse_view();
		CHECK_EQ(LineString{ head.begin(), head.end() }, LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });
		CHECK_EQ(LineString{ tail.begin(), tail.end() }, LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 } });
		CHECK_EQ(LineString{ reversed.begin(), reversed.end() }, LineString{ Vec2{ 3, 3 }, Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	}

	{
		Array<Vec2> head;
		for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.head_view(2))
		{
			head << p;
		}
		CHECK_EQ(head, Array<Vec2>{ Vec2{ 0, 0 }, Vec2{ 1, 1 } });

		Array<Vec2> tail;
		for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.tail_view(2))
		{
			tail << p;
		}
		CHECK_EQ(tail, Array<Vec2>{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });

		Array<Vec2> reversed;
		for (const auto& p : LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.reverse_view())
		{
			reversed << p;
		}
		CHECK_EQ(reversed, Array<Vec2>{ Vec2{ 2, 2 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	}
}

TEST_CASE("LineString.without")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } };
	CHECK_EQ(line.without(Vec2{ 0, 0 }), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } }.without(Vec2{ 0, 0 }), LineString{ Vec2{ 1, 1 } });
	CHECK_EQ(line.without_at(1), LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.without_at(1), LineString{ Vec2{ 0, 0 }, Vec2{ 2, 2 } });
	CHECK_EQ(line.without_if([](const Vec2& p) { return (p.x == 0); }), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
	CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.without_if([](const Vec2& p) { return (p.x == 0); }), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 } });
}

TEST_CASE("LineString.rotate")
{
	{
		LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
		CHECK_EQ(&(line.rotate(2)), &line);
		CHECK_EQ(line, LineString{ Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	}

	{
		const LineString line{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 } };
		CHECK_EQ(line.rotated(1), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 3, 3 }, Vec2{ 0, 0 } });
		CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.rotate(1), LineString{ Vec2{ 1, 1 }, Vec2{ 2, 2 }, Vec2{ 0, 0 } });
		CHECK_EQ(LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 2, 2 } }.rotated(2), LineString{ Vec2{ 2, 2 }, Vec2{ 0, 0 }, Vec2{ 1, 1 } });
	}
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
	std::mt19937 rng4{ 12345 };
	CHECK_EQ(LineString{ source }.shuffle(rng3), LineString{ source }.shuffled(rng4));
	CHECK_EQ(source.shuffled().size(), source.size());
	CHECK_EQ(LineString{ source }.shuffle().size(), source.size());
}

TEST_CASE("LineString.uniqued_consecutive")
{
	const LineString line{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } };
	CHECK_EQ(line.uniqued_consecutive(), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(line, LineString{ Vec2{ 0, 0 }, Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
	CHECK_EQ(LineString{ line }.uniqued_consecutive(), LineString{ Vec2{ 0, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 0 } });
}

TEST_CASE("LineString.sort_by")
{
	const auto compareX = [](const Vec2& a, const Vec2& b) { return (a.x < b.x); };
	const auto compareY = [](const Vec2& a, const Vec2& b) { return (a.y < b.y); };

	{
		LineString line{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } };
		CHECK_EQ(&(line.sort_by(compareX)), &line);
		CHECK_EQ(line, LineString{ Vec2{ 0, 2 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } });
	}

	{
		const LineString line{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } };
		CHECK_EQ(line.sorted_by(compareY), LineString{ Vec2{ 2, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 2 } });
		CHECK_EQ(line, LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } });
	}

	{
		CHECK_EQ(LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }.sort_by(compareX), LineString{ Vec2{ 0, 2 }, Vec2{ 1, 1 }, Vec2{ 2, 0 } });
		CHECK_EQ(LineString{ Vec2{ 2, 0 }, Vec2{ 0, 2 }, Vec2{ 1, 1 } }.sorted_by(compareY), LineString{ Vec2{ 2, 0 }, Vec2{ 1, 1 }, Vec2{ 0, 2 } });
	}
}
