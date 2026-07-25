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

	CHECK(Format(empty) == U"()");
	CHECK(Format(square) == U"(((0, 0),(4, 0),(4, 4),(0, 4)))");
	CHECK(Format(donut) == U"(((0, 0),(10, 0),(10, 10),(0, 10)),((3, 3),(3, 7),(7, 7),(7, 3)),((8, 8),(8, 9),(9, 9),(9, 8)))");

	CHECK(U"{}"_fmt(empty) == Format(empty));
	CHECK(U"{}"_fmt(square) == Format(square));
	CHECK(U"{}"_fmt(donut) == Format(donut));
	CHECK(U"{:.1f}"_fmt(square) == U"(((0.0, 0.0),(4.0, 0.0),(4.0, 4.0),(0.0, 4.0)))");
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
