//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"

TEST_CASE("MultiPolygon.array_like_contract")
{
	static_assert(std::same_as<decltype(MultiPolygon{}.append(MultiPolygon{})), MultiPolygon>);
	static_assert(std::same_as<decltype(MultiPolygon{}.fill(Polygon{})), MultiPolygon>);
	static_assert(std::same_as<decltype(MultiPolygon{}.moveBy(1, 2)), MultiPolygon>);
	static_assert(std::same_as<decltype(MultiPolygon{}.choice()), Polygon>);
	const Polygon small = RectF{ 0, 0, 2, 2 }.asPolygon();
	const Polygon large = RectF{ 0, 0, 4, 4 }.asPolygon();
	MultiPolygon polygons{ small, large, small };
	CHECK((polygons.get_if(1)) == (&polygons[1]));
	CHECK((std::as_const(polygons).get_if(1)) == (&polygons[1]));
	CHECK((polygons.get_if(3)) == (nullptr));
	CHECK((polygons.find_if([](const Polygon& p) { return p.area() > 10; })) == (&polygons[1]));
	CHECK((polygons.indexOf_if([](const Polygon& p) { return p.area() > 10; })) == (Optional<size_t>{ 1 }));
	CHECK((polygons.find_if([](const Polygon&) { return false; })) == (nullptr));
	CHECK((polygons.indexOf_if([](const Polygon&) { return false; })) == (none));
	CHECK((polygons.map(&Polygon::area)) == (Array<double>{ 4, 16, 4 }));
	CHECK((polygons.drop(1).map(&Polygon::area)) == (Array<double>{ 16, 4 }));
	CHECK((polygons.drop_while([](const Polygon& p) { return p.area() < 10; }).map(&Polygon::area)) == (Array<double>{ 16, 4 }));
	CHECK(polygons.drop(100).isEmpty());
	CHECK((polygons.slice(1).map(&Polygon::area)) == (Array<double>{ 16, 4 }));
	CHECK(polygons.slice(3).isEmpty());
	CHECK_THROWS_AS((void) polygons.slice(4), std::out_of_range);
	CHECK_THROWS_AS((void) MultiPolygon{}.choice(), std::out_of_range);

	polygons.reserve(32);
	const auto storage = polygons.data();
	const auto vertices = polygons[1].vertices().data();
	auto result = std::move(polygons).drop(1).filter([](const Polygon& p) { return p.area() > 10; });
	CHECK((result.size()) == (size_t{ 1 }));
	CHECK((result.data()) == (storage));
	CHECK((result[0].vertices().data()) == (vertices));
	result.append(std::move(result));
	CHECK((result.size()) == (size_t{ 1 }));
	CHECK((result.data()) == (storage));
	result.append(result);
	CHECK((result.size()) == (size_t{ 2 }));
	CHECK(std::ranges::equal(result[0].vertices(), result[1].vertices()));
	CHECK_THROWS_AS((void) result.filter([](const Polygon&) -> bool { throw std::runtime_error("predicate"); }), std::runtime_error);
}
