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
	void CheckObjectKeys(const JSON& json, const std::initializer_list<StringView> keys)
	{
		REQUIRE(json.isObject());
		CHECK_EQ(json.size(), keys.size());

		for (const StringView key : keys)
		{
			CHECK(json.contains(key));
		}
	}

	template <class Type>
	[[nodiscard]]
	Type RoundTrip(const Type& value)
	{
		const JSON json = value;
		return json.get<Type>();
	}

	void CheckPolygonGeometry(const Polygon& actual, const Polygon& expected)
	{
		CHECK_EQ(actual.outer(), expected.outer());
		CHECK_EQ(actual.inners(), expected.inners());
	}
}

TEST_CASE("JSONSerialization 2D Shapes")
{
	// The order follows Geometry2D/Intersects.hpp.

	SUBCASE("Point")
	{
		const Point value{ -12, 34 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y" });
		CHECK_EQ(json[U"x"].get<int32>(), value.x);
		CHECK_EQ(json[U"y"].get<int32>(), value.y);
		CHECK_EQ(json.get<Point>(), value);
	}

	SUBCASE("Vec2")
	{
		const Vec2 value{ -1.25, 3.5 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y" });
		CHECK_EQ(json[U"x"].get<double>(), value.x);
		CHECK_EQ(json[U"y"].get<double>(), value.y);
		CHECK_EQ(json.get<Vec2>(), value);
	}

	SUBCASE("Line")
	{
		const Line value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 } };
		const JSON json = value;

		CheckObjectKeys(json, { U"start", U"end" });
		CHECK_EQ(json[U"start"].get<Vec2>(), value.start);
		CHECK_EQ(json[U"end"].get<Vec2>(), value.end);
		CHECK_EQ(json.get<Line>(), value);
	}

	SUBCASE("LineString")
	{
		const LineString value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 } };
		const JSON json = value;

		REQUIRE(json.isArray());
		REQUIRE_EQ(json.size(), value.size());
		CHECK_EQ(json[0].get<Vec2>(), value[0]);
		CHECK_EQ(json[1].get<Vec2>(), value[1]);
		CHECK_EQ(json[2].get<Vec2>(), value[2]);
		CHECK_EQ(json.get<LineString>(), value);
		CHECK(RoundTrip(LineString{}).isEmpty());
	}

	SUBCASE("Bezier2")
	{
		const Bezier2 value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 } };
		const JSON json = value;
		const Bezier2 result = json.get<Bezier2>();

		CheckObjectKeys(json, { U"p0", U"p1", U"p2" });
		CHECK_EQ(result.p0, value.p0);
		CHECK_EQ(result.p1, value.p1);
		CHECK_EQ(result.p2, value.p2);
	}

	SUBCASE("Bezier3")
	{
		const Bezier3 value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 }, Vec2{ -7.0, 8.0 } };
		const JSON json = value;
		const Bezier3 result = json.get<Bezier3>();

		CheckObjectKeys(json, { U"p0", U"p1", U"p2", U"p3" });
		CHECK_EQ(result.p0, value.p0);
		CHECK_EQ(result.p1, value.p1);
		CHECK_EQ(result.p2, value.p2);
		CHECK_EQ(result.p3, value.p3);
	}

	SUBCASE("Rect")
	{
		const Rect value{ -12, 34, 56, 78 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"w", U"h" });
		CHECK_EQ(json.get<Rect>(), value);
	}

	SUBCASE("RectF")
	{
		const RectF value{ -1.25, 3.5, 6.75, 8.0 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"w", U"h" });
		CHECK_EQ(json.get<RectF>(), value);
	}

	SUBCASE("Circle")
	{
		const Circle value{ -1.25, 3.5, 6.75 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"r" });
		CHECK_EQ(json.get<Circle>(), value);
	}

	SUBCASE("Ellipse")
	{
		const Ellipse value{ -1.25, 3.5, 6.75, 8.0 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"a", U"b" });
		CHECK_EQ(json.get<Ellipse>(), value);
	}

	SUBCASE("SuperEllipse")
	{
		const SuperEllipse value{ -1.25, 3.5, 6.75, 8.0, 2.5 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"a", U"b", U"n" });
		CHECK_EQ(json.get<SuperEllipse>(), value);
	}

	SUBCASE("Triangle")
	{
		const Triangle value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 } };
		const JSON json = value;

		CheckObjectKeys(json, { U"p0", U"p1", U"p2" });
		CHECK_EQ(json.get<Triangle>(), value);
	}

	SUBCASE("Quad")
	{
		const Quad value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 }, Vec2{ -7.0, 8.0 } };
		const JSON json = value;

		CheckObjectKeys(json, { U"p0", U"p1", U"p2", U"p3" });
		CHECK_EQ(json.get<Quad>(), value);
	}

	SUBCASE("RoundRect")
	{
		const RoundRect value{ -1.25, 3.5, 6.75, 8.0, 2.5 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"w", U"h", U"r" });
		CHECK_EQ(json.get<RoundRect>(), value);
	}

	SUBCASE("Polygon")
	{
		const Array<Vec2> outer{ Vec2{ 0, 0 }, Vec2{ 10, 0 }, Vec2{ 10, 10 }, Vec2{ 0, 10 } };
		const Array<Array<Vec2>> inners{
			{ Vec2{ 2, 2 }, Vec2{ 2, 8 }, Vec2{ 8, 8 }, Vec2{ 8, 2 } }
		};
		const Polygon value{ outer, inners };
		REQUIRE(value);

		const JSON json = value;
		CheckObjectKeys(json, { U"outer", U"inners" });
		CHECK_FALSE(json.contains(U"vertices"));
		CHECK_FALSE(json.contains(U"indices"));
		CHECK_FALSE(json.contains(U"boundingRect"));

		const Polygon result = json.get<Polygon>();
		REQUIRE(result);
		CheckPolygonGeometry(result, value);

		const Polygon emptyResult = RoundTrip(Polygon{});
		CHECK(emptyResult.isEmpty());
		CHECK(emptyResult.outer().isEmpty());
		CHECK(emptyResult.inners().isEmpty());

		const JSON invalid = JSON::Parse(U"{\"outer\":[{\"x\":0,\"y\":0},{\"x\":1,\"y\":0}],\"inners\":[]}");
		CHECK(invalid.get<Polygon>().isEmpty());
	}

	SUBCASE("MultiPolygon")
	{
		const Polygon first = RectF{ 0, 0, 10, 20 }.asPolygon();
		const Polygon second = Circle{ 30, 40, 5 }.asPolygon();
		const MultiPolygon value{ first, second };
		const JSON json = value;

		REQUIRE(json.isArray());
		REQUIRE_EQ(json.size(), value.size());

		const MultiPolygon result = json.get<MultiPolygon>();
		REQUIRE_EQ(result.size(), value.size());
		CheckPolygonGeometry(result[0], value[0]);
		CheckPolygonGeometry(result[1], value[1]);
		CHECK(RoundTrip(MultiPolygon{}).isEmpty());
	}
}
