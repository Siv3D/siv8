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
		CHECK((json.size()) == (keys.size()));

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
		CHECK((actual.outer()) == (expected.outer()));
		CHECK((actual.inners()) == (expected.inners()));
	}
}

TEST_CASE("JSONSerialization 2D Shapes")
{
	// The order follows Geometry2D/Intersects.hpp.

	SECTION("Point")
	{
		const Point value{ -12, 34 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y" });
		CHECK((json[U"x"].get<int32>()) == (value.x));
		CHECK((json[U"y"].get<int32>()) == (value.y));
		CHECK((json.get<Point>()) == (value));
	}

	SECTION("Vec2")
	{
		const Vec2 value{ -1.25, 3.5 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y" });
		CHECK((json[U"x"].get<double>()) == (value.x));
		CHECK((json[U"y"].get<double>()) == (value.y));
		CHECK((json.get<Vec2>()) == (value));
	}

	SECTION("Line")
	{
		const Line value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 } };
		const JSON json = value;

		CheckObjectKeys(json, { U"start", U"end" });
		CHECK((json[U"start"].get<Vec2>()) == (value.start));
		CHECK((json[U"end"].get<Vec2>()) == (value.end));
		CHECK((json.get<Line>()) == (value));
	}

	SECTION("LineString")
	{
		const LineString value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 } };
		const JSON json = value;

		REQUIRE(json.isArray());
		REQUIRE((json.size()) == (value.size()));
		CHECK((json[0].get<Vec2>()) == (value[0]));
		CHECK((json[1].get<Vec2>()) == (value[1]));
		CHECK((json[2].get<Vec2>()) == (value[2]));
		CHECK((json.get<LineString>()) == (value));
		CHECK(RoundTrip(LineString{}).isEmpty());
	}

	SECTION("Bezier2")
	{
		const Bezier2 value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 } };
		const JSON json = value;
		const Bezier2 result = json.get<Bezier2>();

		CheckObjectKeys(json, { U"p0", U"p1", U"p2" });
		CHECK((result.p0) == (value.p0));
		CHECK((result.p1) == (value.p1));
		CHECK((result.p2) == (value.p2));
	}

	SECTION("Bezier3")
	{
		const Bezier3 value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 }, Vec2{ -7.0, 8.0 } };
		const JSON json = value;
		const Bezier3 result = json.get<Bezier3>();

		CheckObjectKeys(json, { U"p0", U"p1", U"p2", U"p3" });
		CHECK((result.p0) == (value.p0));
		CHECK((result.p1) == (value.p1));
		CHECK((result.p2) == (value.p2));
		CHECK((result.p3) == (value.p3));
	}

	SECTION("Rect")
	{
		const Rect value{ -12, 34, 56, 78 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"w", U"h" });
		CHECK((json.get<Rect>()) == (value));
	}

	SECTION("RectF")
	{
		const RectF value{ -1.25, 3.5, 6.75, 8.0 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"w", U"h" });
		CHECK((json.get<RectF>()) == (value));
	}

	SECTION("Circle")
	{
		const Circle value{ -1.25, 3.5, 6.75 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"r" });
		CHECK((json.get<Circle>()) == (value));
	}

	SECTION("Ellipse")
	{
		const Ellipse value{ -1.25, 3.5, 6.75, 8.0 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"a", U"b" });
		CHECK((json.get<Ellipse>()) == (value));
	}

	SECTION("SuperEllipse")
	{
		const SuperEllipse value{ -1.25, 3.5, 6.75, 8.0, 2.5 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"a", U"b", U"n" });
		CHECK((json.get<SuperEllipse>()) == (value));
	}

	SECTION("Triangle")
	{
		const Triangle value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 } };
		const JSON json = value;

		CheckObjectKeys(json, { U"p0", U"p1", U"p2" });
		CHECK((json.get<Triangle>()) == (value));
	}

	SECTION("Quad")
	{
		const Quad value{ Vec2{ -1.0, 2.0 }, Vec2{ 3.0, -4.0 }, Vec2{ 5.0, 6.0 }, Vec2{ -7.0, 8.0 } };
		const JSON json = value;

		CheckObjectKeys(json, { U"p0", U"p1", U"p2", U"p3" });
		CHECK((json.get<Quad>()) == (value));
	}

	SECTION("RoundRect")
	{
		const RoundRect value{ -1.25, 3.5, 6.75, 8.0, 2.5 };
		const JSON json = value;

		CheckObjectKeys(json, { U"x", U"y", U"w", U"h", U"r" });
		CHECK((json.get<RoundRect>()) == (value));
	}

	SECTION("Polygon")
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

	SECTION("MultiPolygon")
	{
		const Polygon first = RectF{ 0, 0, 10, 20 }.asPolygon();
		const Polygon second = Circle{ 30, 40, 5 }.asPolygon();
		const MultiPolygon value{ first, second };
		const JSON json = value;

		REQUIRE(json.isArray());
		REQUIRE((json.size()) == (value.size()));

		const MultiPolygon result = json.get<MultiPolygon>();
		REQUIRE((result.size()) == (value.size()));
		CheckPolygonGeometry(result[0], value[0]);
		CheckPolygonGeometry(result[1], value[1]);
		CHECK(RoundTrip(MultiPolygon{}).isEmpty());
	}
}
