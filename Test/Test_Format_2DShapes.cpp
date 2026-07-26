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

TEST_CASE("Point")
{
	const Point zero{ 0, 0 };
	const Point zeroOne{ 0, 1 };
	const Point oneZero{ 1, 0 };
	const Point oneOne{ 1, 1 };
	const Point minMin{ std::numeric_limits<int32>::min(), std::numeric_limits<int32>::min() };
	const Point maxMax{ std::numeric_limits<int32>::max(), std::numeric_limits<int32>::max() };
	const Point minMax{ std::numeric_limits<int32>::min(), std::numeric_limits<int32>::max() };
	const Point maxMin{ std::numeric_limits<int32>::max(), std::numeric_limits<int32>::min() };

	CHECK(Format(zero) == U"(0, 0)");
	CHECK(Format(zeroOne) == U"(0, 1)");
	CHECK(Format(oneZero) == U"(1, 0)");
	CHECK(Format(oneOne) == U"(1, 1)");
	CHECK(Format(minMin) == U"(-2147483648, -2147483648)");
	CHECK(Format(maxMax) == U"(2147483647, 2147483647)");
	CHECK(Format(minMax) == U"(-2147483648, 2147483647)");
	CHECK(Format(maxMin) == U"(2147483647, -2147483648)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0)");
	CHECK(U"{}"_fmt(zeroOne) == U"(0, 1)");
	CHECK(U"{}"_fmt(oneZero) == U"(1, 0)");
	CHECK(U"{}"_fmt(oneOne) == U"(1, 1)");
	CHECK(U"{}"_fmt(minMin) == U"(-2147483648, -2147483648)");
	CHECK(U"{}"_fmt(maxMax) == U"(2147483647, 2147483647)");
	CHECK(U"{}"_fmt(minMax) == U"(-2147483648, 2147483647)");
	CHECK(U"{}"_fmt(maxMin) == U"(2147483647, -2147483648)");
}

TEST_CASE("Vec2")
{
	const Vec2 zero{ 0, 0 };
	const Vec2 value{ 1.5, -2.25 };

	CHECK(Format(zero) == U"(0, 0)");
	CHECK(Format(value) == U"(1.5, -2.25)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0)");
	CHECK(U"{}"_fmt(value) == U"(1.5, -2.25)");
	CHECK(U"{:.1f}"_fmt(Vec2{ 1, -2 }) == U"(1.0, -2.0)");
}

TEST_CASE("Line")
{
	const Line zero{};
	const Line value{ 1.5, -2.25, 3.75, 4.5 };

	CHECK(Format(zero) == U"((0, 0), (0, 0))");
	CHECK(Format(value) == U"((1.5, -2.25), (3.75, 4.5))");

	CHECK(U"{}"_fmt(zero) == U"((0, 0), (0, 0))");
	CHECK(U"{}"_fmt(value) == U"((1.5, -2.25), (3.75, 4.5))");
	CHECK(U"{:.1f}"_fmt(Line{ 1, 2, 3, 4 }) == U"((1.0, 2.0), (3.0, 4.0))");
}

TEST_CASE("LineString")
{
	const LineString empty;
	const LineString value{
		Vec2{ 1.5, -2.25 },
		Vec2{ 3.75, 4.5 },
	};

	CHECK(Format(empty) == U"[]");
	CHECK(Format(value) == U"[(1.5, -2.25), (3.75, 4.5)]");

	CHECK(U"{}"_fmt(empty) == U"[]");
	CHECK(U"{}"_fmt(value) == U"[(1.5, -2.25), (3.75, 4.5)]");
	CHECK(U"{:.1f}"_fmt(LineString{ Vec2{ 1, 2 }, Vec2{ 3, 4 } }) == U"[(1.0, 2.0), (3.0, 4.0)]");

	const LineString precise{
		Vec2{ -1.2345678901234567, -0.000000000000001 },
		Vec2{ 9.876543210987654, 4.567890123456789 },
	};

	const auto parsedEmpty = LineString::Parse(Format(empty));
	REQUIRE(parsedEmpty);
	CHECK_EQ(*parsedEmpty, empty);

	const auto parsedValue = LineString::Parse(Format(value).toUTF8());
	REQUIRE(parsedValue);
	CHECK_EQ(*parsedValue, value);

	const auto parsedPrecise = LineString::Parse(Format(precise));
	REQUIRE(parsedPrecise);
	CHECK_EQ(*parsedPrecise, precise);

	const auto parsedWithSpaces = LineString::Parse(
		U" \n [ (0e0, 1), (-2.5, 3E+2) ] \t");
	REQUIRE(parsedWithSpaces);
	CHECK_EQ(*parsedWithSpaces, LineString{ Vec2{ 0, 1 }, Vec2{ -2.5, 300 } });

	const auto parsedSingle = LineString::Parse(U"[(1, 2)]");
	REQUIRE(parsedSingle);
	CHECK_EQ(*parsedSingle, LineString{ Vec2{ 1, 2 } });

	const Array<StringView> invalidSources{
		U"",
		U" ",
		U"[",
		U"[()]",
		U"[(0, 0),]",
		U"[(0foo, 0)]",
		U"[(0, 1e999)]",
		U"[(0, 0)] trailing",
	};

	for (const auto source : invalidSources)
	{
		CHECK_FALSE(LineString::Parse(source));
	}
}

TEST_CASE("Bezier2")
{
	const Bezier2 zero{};
	const Bezier2 value{
		Vec2{ 1.5, -2.25 },
		Vec2{ 3.75, 4.5 },
		Vec2{ 5.25, -6.5 },
	};

	CHECK(Format(zero) == U"((0, 0), (0, 0), (0, 0))");
	CHECK(Format(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5))");

	CHECK(U"{}"_fmt(zero) == U"((0, 0), (0, 0), (0, 0))");
	CHECK(U"{}"_fmt(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5))");
	CHECK(U"{:.1f}"_fmt(Bezier2{ Vec2{ 1, 2 }, Vec2{ 3, 4 }, Vec2{ 5, 6 } })
		== U"((1.0, 2.0), (3.0, 4.0), (5.0, 6.0))");
}

TEST_CASE("Bezier3")
{
	const Bezier3 zero{};
	const Bezier3 value{
		Vec2{ 1.5, -2.25 },
		Vec2{ 3.75, 4.5 },
		Vec2{ 5.25, -6.5 },
		Vec2{ 7.5, 8.25 },
	};

	CHECK(Format(zero) == U"((0, 0), (0, 0), (0, 0), (0, 0))");
	CHECK(Format(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5), (7.5, 8.25))");

	CHECK(U"{}"_fmt(zero) == U"((0, 0), (0, 0), (0, 0), (0, 0))");
	CHECK(U"{}"_fmt(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5), (7.5, 8.25))");
	CHECK(U"{:.1f}"_fmt(Bezier3{ Vec2{ 1, 2 }, Vec2{ 3, 4 }, Vec2{ 5, 6 }, Vec2{ 7, 8 } })
		== U"((1.0, 2.0), (3.0, 4.0), (5.0, 6.0), (7.0, 8.0))");
}

TEST_CASE("Rect")
{
	const Rect zero{};
	const Rect value{ -1, 2, 30, 40 };

	CHECK(Format(zero) == U"(0, 0, 0, 0)");
	CHECK(Format(value) == U"(-1, 2, 30, 40)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(value) == U"(-1, 2, 30, 40)");
	CHECK(U"{:04d}"_fmt(value) == U"(-001, 0002, 0030, 0040)");
}

TEST_CASE("RectF")
{
	const RectF zero{};
	const RectF value{ 1.5, -2.25, 3.75, 4.5 };

	CHECK(Format(zero) == U"(0, 0, 0, 0)");
	CHECK(Format(value) == U"(1.5, -2.25, 3.75, 4.5)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(value) == U"(1.5, -2.25, 3.75, 4.5)");
	CHECK(U"{:.1f}"_fmt(RectF{ 1, 2, 3, 4 }) == U"(1.0, 2.0, 3.0, 4.0)");
}

TEST_CASE("Circle")
{
	const Circle zero{};
	const Circle value{ 1.5, -2.25, 3.75 };

	CHECK(Format(zero) == U"(0, 0, 0)");
	CHECK(Format(value) == U"(1.5, -2.25, 3.75)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0, 0)");
	CHECK(U"{}"_fmt(value) == U"(1.5, -2.25, 3.75)");
	CHECK(U"{:.1f}"_fmt(Circle{ 1, 2, 3 }) == U"(1.0, 2.0, 3.0)");
}

TEST_CASE("Ellipse")
{
	const Ellipse zero{};
	const Ellipse value{ 1.5, -2.25, 3.75, 4.5 };

	CHECK(Format(zero) == U"(0, 0, 0, 0)");
	CHECK(Format(value) == U"(1.5, -2.25, 3.75, 4.5)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0, 0, 0)");
	CHECK(U"{}"_fmt(value) == U"(1.5, -2.25, 3.75, 4.5)");
	CHECK(U"{:.1f}"_fmt(Ellipse{ 1, 2, 3, 4 }) == U"(1.0, 2.0, 3.0, 4.0)");
}

TEST_CASE("SuperEllipse")
{
	const SuperEllipse zero{};
	const SuperEllipse value{ 1.5, -2.25, 3.75, 4.5, 5.25 };

	CHECK(Format(zero) == U"(0, 0, 0, 0, 2)");
	CHECK(Format(value) == U"(1.5, -2.25, 3.75, 4.5, 5.25)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0, 0, 0, 2)");
	CHECK(U"{}"_fmt(value) == U"(1.5, -2.25, 3.75, 4.5, 5.25)");
	CHECK(U"{:.1f}"_fmt(SuperEllipse{ 1, 2, 3, 4, 5 }) == U"(1.0, 2.0, 3.0, 4.0, 5.0)");
}

TEST_CASE("Triangle")
{
	const Triangle zero{};
	const Triangle value{ 1.5, -2.25, 3.75, 4.5, 5.25, -6.5 };

	CHECK(Format(zero) == U"((0, 0), (0, 0), (0, 0))");
	CHECK(Format(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5))");

	CHECK(U"{}"_fmt(zero) == U"((0, 0), (0, 0), (0, 0))");
	CHECK(U"{}"_fmt(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5))");
	CHECK(U"{:.1f}"_fmt(Triangle{ 1, 2, 3, 4, 5, 6 })
		== U"((1.0, 2.0), (3.0, 4.0), (5.0, 6.0))");
}

TEST_CASE("Quad")
{
	const Quad zero{};
	const Quad value{ 1.5, -2.25, 3.75, 4.5, 5.25, -6.5, 7.5, 8.25 };

	CHECK(Format(zero) == U"((0, 0), (0, 0), (0, 0), (0, 0))");
	CHECK(Format(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5), (7.5, 8.25))");

	CHECK(U"{}"_fmt(zero) == U"((0, 0), (0, 0), (0, 0), (0, 0))");
	CHECK(U"{}"_fmt(value) == U"((1.5, -2.25), (3.75, 4.5), (5.25, -6.5), (7.5, 8.25))");
	CHECK(U"{:.1f}"_fmt(Quad{ 1, 2, 3, 4, 5, 6, 7, 8 })
		== U"((1.0, 2.0), (3.0, 4.0), (5.0, 6.0), (7.0, 8.0))");
}

TEST_CASE("RoundRect")
{
	const RoundRect zero{};
	const RoundRect value{ 1.5, -2.25, 3.75, 4.5, 5.25 };

	CHECK(Format(zero) == U"(0, 0, 0, 0, 0)");
	CHECK(Format(value) == U"(1.5, -2.25, 3.75, 4.5, 5.25)");

	CHECK(U"{}"_fmt(zero) == U"(0, 0, 0, 0, 0)");
	CHECK(U"{}"_fmt(value) == U"(1.5, -2.25, 3.75, 4.5, 5.25)");
	CHECK(U"{:.1f}"_fmt(RoundRect{ 1, 2, 3, 4, 5 }) == U"(1.0, 2.0, 3.0, 4.0, 5.0)");
}

TEST_CASE("Polygon")
{
	const Polygon empty;
	const Polygon square{
		Array<Vec2>{
			Vec2{ 0, 0 },
			Vec2{ 4, 0 },
			Vec2{ 4, 4 },
			Vec2{ 0, 4 },
		}
	};
	const Polygon donut{
		Array<Vec2>{
			Vec2{ 0, 0 },
			Vec2{ 10, 0 },
			Vec2{ 10, 10 },
			Vec2{ 0, 10 },
		},
		Array<Array<Vec2>>{
			Array<Vec2>{
				Vec2{ 3, 3 },
				Vec2{ 3, 7 },
				Vec2{ 7, 7 },
				Vec2{ 7, 3 },
			},
			Array<Vec2>{
				Vec2{ 8, 8 },
				Vec2{ 8, 9 },
				Vec2{ 9, 9 },
				Vec2{ 9, 8 },
			}
		}
	};
	const Polygon precise{
		Array<Vec2>{
			Vec2{ -1.2345678901234567, -0.000000000000001 },
			Vec2{ 9.876543210987654, -0.000000000000001 },
			Vec2{ 9.876543210987654, 4.567890123456789 },
			Vec2{ -1.2345678901234567, 4.567890123456789 },
		}
	};

	CHECK(Format(empty) == U"()");
	CHECK(Format(square) == U"(((0, 0),(4, 0),(4, 4),(0, 4)))");
	CHECK(Format(donut) == U"(((0, 0),(10, 0),(10, 10),(0, 10)),((3, 3),(3, 7),(7, 7),(7, 3)),((8, 8),(8, 9),(9, 9),(9, 8)))");

	CHECK(U"{}"_fmt(empty) == Format(empty));
	CHECK(U"{}"_fmt(square) == Format(square));
	CHECK(U"{}"_fmt(donut) == Format(donut));
	CHECK(U"{:.1f}"_fmt(square) == U"(((0.0, 0.0),(4.0, 0.0),(4.0, 4.0),(0.0, 4.0)))");

	const auto parsedEmpty = Polygon::Parse(Format(empty));
	REQUIRE(parsedEmpty);
	CHECK(parsedEmpty->isEmpty());

	const auto parsedSquare = Polygon::Parse(Format(square));
	REQUIRE(parsedSquare);
	CHECK_EQ(parsedSquare->outer(), square.outer());
	CHECK_EQ(parsedSquare->inners(), square.inners());

	const auto parsedDonut = Polygon::Parse(Format(donut).toUTF8());
	REQUIRE(parsedDonut);
	CHECK_EQ(parsedDonut->outer(), donut.outer());
	CHECK_EQ(parsedDonut->inners(), donut.inners());

	const auto parsedPrecise = Polygon::Parse(Format(precise));
	REQUIRE(parsedPrecise);
	CHECK_EQ(parsedPrecise->outer(), precise.outer());

	const auto parsedWithSpaces = Polygon::Parse(
		U" \n ( ( (0e0, 0), (4, 0), (4, 4), (0, 4) ) ) \t");
	REQUIRE(parsedWithSpaces);
	CHECK_EQ(parsedWithSpaces->outer(), square.outer());

	const Array<StringView> invalidSources{
		U"",
		U" ",
		U"(",
		U"(())",
		U"(((0, 0),(4, 0)))",
		U"(((0, 0),(4, 0),(0, 4)),)",
		U"(((0foo, 0),(4, 0),(0, 4)))",
		U"(((0, 0),(4, 0),(0, 1e999)))",
		U"(((0, 0),(4, 0),(0, 4))) trailing",
	};

	for (const auto source : invalidSources)
	{
		CHECK_FALSE(Polygon::Parse(source));
	}
}

TEST_CASE("MultiPolygon")
{
	const Polygon square{
		Array<Vec2>{
			Vec2{ 0, 0 },
			Vec2{ 4, 0 },
			Vec2{ 4, 4 },
			Vec2{ 0, 4 },
		}
	};
	const Polygon triangle{
		Array<Vec2>{
			Vec2{ 10, 0 },
			Vec2{ 12, 4 },
			Vec2{ 8, 4 },
		}
	};

	const MultiPolygon empty;
	const MultiPolygon one{ square };
	const MultiPolygon two{ square, triangle };
	const MultiPolygon withEmpty{ Polygon{} };

	CHECK(Format(empty) == U"[]");
	CHECK(Format(one) == U"[(((0, 0),(4, 0),(4, 4),(0, 4)))]");
	CHECK(Format(two) == U"[(((0, 0),(4, 0),(4, 4),(0, 4))), (((10, 0),(12, 4),(8, 4)))]");
	CHECK(Format(withEmpty) == U"[()]");

	CHECK(U"{}"_fmt(empty) == Format(empty));
	CHECK(U"{}"_fmt(one) == Format(one));
	CHECK(U"{}"_fmt(two) == Format(two));
	CHECK(U"{}"_fmt(withEmpty) == Format(withEmpty));
	CHECK(U"{:.1f}"_fmt(one) == U"[(((0.0, 0.0),(4.0, 0.0),(4.0, 4.0),(0.0, 4.0)))]");
}
