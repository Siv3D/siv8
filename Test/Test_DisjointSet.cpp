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
# include <stdexcept>

TEMPLATE_TEST_CASE("DisjointSet.empty_and_singletons", "", int8, uint8, int16, uint16, int32, uint32, int64, uint64)
{
	DisjointSet<TestType> empty;
	CHECK(empty.isEmpty());
	CHECK_FALSE(static_cast<bool>(empty));
	CHECK(empty.size() == 0);
	CHECK(empty.ssize() == 0);
	CHECK(empty.countSets() == 0);
	CHECK(empty.groups().isEmpty());
	empty.resize(0);
	empty.reset();
	CHECK(empty.countSets() == 0);
	CHECK(empty.groups().isEmpty());
	CHECK(DisjointSet<TestType>{ 0 }.groups().isEmpty());

	DisjointSet<TestType> singletons(4);
	CHECK_FALSE(singletons.isEmpty());
	CHECK(static_cast<bool>(singletons));
	CHECK(singletons.size() == 4);
	CHECK(singletons.ssize() == 4);
	CHECK(singletons.countSets() == 4);
	CHECK((singletons.groups() == Array<Array<TestType>>{ { 0 }, { 1 }, { 2 }, { 3 } }));

	for (TestType i = 0; i < 4; ++i)
	{
		CHECK(singletons.root(i) == i);
		CHECK(singletons.isRoot(i));
		CHECK(singletons.size(i) == 1);
		CHECK(singletons.connected(i, i));
		CHECK_FALSE(singletons.merge(i, i));
	}
	CHECK(singletons.countSets() == 4);
}

TEMPLATE_TEST_CASE("DisjointSet.merge_and_sizes", "", int8, uint8, int16, uint16, int32, uint32, int64, uint64)
{
	DisjointSet<TestType> sets(8);
	CHECK_FALSE(sets.connected(1, 4));
	REQUIRE(sets.merge(4, 5));
	REQUIRE(sets.merge(1, 2));
	REQUIRE(sets.merge(1, 3));
	const TestType largerRoot = sets.root(1);
	REQUIRE(sets.merge(4, 1));
	CHECK(sets.root(4) == largerRoot);
	CHECK(sets.size(5) == 5);
	CHECK(sets.countSets() == 4);
	CHECK_FALSE(sets.isRoot(5));
	CHECK_FALSE(sets.merge(2, 5));
	CHECK_FALSE(sets.merge(1, 1));
	CHECK(sets.countSets() == 4);

	REQUIRE(sets.merge(0, 6));
	REQUIRE(sets.merge(0, 1));
	CHECK(sets.countSets() == 2);
	CHECK(sets.size() == 8);
	for (TestType i = 0; i < 7; ++i)
	{
		CHECK(sets.root(i) == largerRoot);
		CHECK(sets.size(i) == 7);
		CHECK(sets.connected(i, 5));
		CHECK_FALSE(sets.connected(i, 7));
	}
	CHECK(sets.isRoot(largerRoot));
	CHECK(sets.isRoot(7));
	CHECK(sets.size(7) == 1);
}

TEMPLATE_TEST_CASE("DisjointSet.groups_order", "", int8, uint8, int16, uint16, int32, uint32, int64, uint64)
{
	DisjointSet<TestType> sets(10);
	REQUIRE(sets.merge(7, 1));
	REQUIRE(sets.merge(7, 4));
	REQUIRE(sets.merge(5, 0));
	REQUIRE(sets.merge(8, 3));
	const Array<Array<TestType>> expected{ { 2 }, { 0, 5 }, { 6 }, { 1, 4, 7 }, { 3, 8 }, { 9 } };
	CHECK(sets.countSets() == expected.size());
	CHECK(sets.groups() == expected);
	CHECK(sets.groups() == expected);

	REQUIRE(sets.merge(2, 7));
	CHECK((sets.groups() == Array<Array<TestType>>{ { 0, 5 }, { 6 }, { 1, 2, 4, 7 }, { 3, 8 }, { 9 } }));
	CHECK(sets.countSets() == 5);
	CHECK(sets.root(2) == 7);
	CHECK(sets.size(2) == 4);
}

TEMPLATE_TEST_CASE("DisjointSet.balanced_tree_queries", "", int8, uint8, int16, uint16, int32, uint32, int64, uint64)
{
	constexpr size_t N = 128;
	DisjointSet<TestType> sets(N);
	for (size_t width = 1; width < N; width *= 2)
	{
		for (size_t i = 0; i < N; i += (width * 2))
		{
			REQUIRE(sets.merge(static_cast<TestType>(i), static_cast<TestType>(i + width)));
		}
	}

	for (int pass = 0; pass < 2; ++pass)
	{
		for (size_t i = N; i > 0; --i)
		{
			const auto index = static_cast<TestType>(i - 1);
			CHECK(sets.root(index) == 0);
			CHECK(sets.size(index) == N);
			CHECK(sets.connected(index, 0));
		}
	}
	CHECK(sets.countSets() == 1);
	const auto groups = sets.groups();
	REQUIRE(groups.size() == 1);
	REQUIRE(groups[0].size() == N);
	for (size_t i = 0; i < N; ++i)
	{
		CHECK(groups[0][i] == static_cast<TestType>(i));
	}
}

TEMPLATE_TEST_CASE("DisjointSet.resize_and_reset", "", int8, uint8, int16, uint16, int32, uint32, int64, uint64)
{
	DisjointSet<TestType> sets;
	sets.resize(4);
	REQUIRE(sets.merge(3, 1));
	const Array<Array<TestType>> initial{ { 0 }, { 2 }, { 1, 3 } };
	sets.resize(4);
	CHECK(sets.countSets() == 3);
	CHECK(sets.groups() == initial);

	sets.resize(7);
	CHECK(sets.size() == 7);
	CHECK(sets.ssize() == 7);
	CHECK(sets.countSets() == 6);
	CHECK((sets.groups() == Array<Array<TestType>>{ { 0 }, { 2 }, { 1, 3 }, { 4 }, { 5 }, { 6 } }));
	CHECK(sets.root(1) == 3);
	for (TestType i = 4; i < 7; ++i)
	{
		CHECK(sets.isRoot(i));
		CHECK(sets.size(i) == 1);
	}
	REQUIRE(sets.merge(4, 3));
	CHECK(sets.size(1) == 3);
	CHECK(sets.countSets() == 5);

	for (int pass = 0; pass < 2; ++pass)
	{
		sets.reset();
		CHECK(sets.size() == 7);
		CHECK(sets.countSets() == 7);
		CHECK((sets.groups() == Array<Array<TestType>>{ { 0 }, { 1 }, { 2 }, { 3 }, { 4 }, { 5 }, { 6 } }));
		CHECK_FALSE(sets.connected(1, 3));
	}
	REQUIRE(sets.merge(6, 0));
	CHECK(sets.size(0) == 2);
	CHECK(sets.countSets() == 6);
}

TEMPLATE_TEST_CASE("DisjointSet.eight_bit_max_size", "", int8, uint8)
{
	using Set = DisjointSet<TestType>;
	STATIC_REQUIRE(Set::MaxSize == 128);
	Set sets(Set::MaxSize - 1);
	sets.resize(Set::MaxSize);
	CHECK(sets.countSets() == Set::MaxSize);
	const auto singletons = sets.groups();
	REQUIRE(singletons.size() == Set::MaxSize);
	for (size_t i = 0; i < Set::MaxSize; ++i)
	{
		REQUIRE(singletons[i].size() == 1);
		CHECK(singletons[i][0] == static_cast<TestType>(i));
	}

	// Keep the largest valid parent index, and reach the minimum signed size.
	const auto last = static_cast<TestType>(Set::MaxSize - 1);
	for (size_t i = 0; i < (Set::MaxSize - 1); ++i)
	{
		REQUIRE(sets.merge(last, static_cast<TestType>(i)));
	}
	CHECK(sets.root(0) == last);
	CHECK(sets.countSets() == 1);
	CHECK(sets.size(0) == Set::MaxSize);
	CHECK(sets.size(last) == Set::MaxSize);
	CHECK_FALSE(sets.merge(0, last));
	const auto groups = sets.groups();
	REQUIRE(groups.size() == 1);
	REQUIRE(groups[0].size() == Set::MaxSize);
	for (size_t i = 0; i < Set::MaxSize; ++i)
	{
		CHECK(groups[0][i] == static_cast<TestType>(i));
	}
	sets.reset();
	CHECK(sets.groups() == singletons);
}

TEST_CASE("DisjointSet.resize_exception_preserves_state")
{
	DisjointSet<int64> sets(4);
	REQUIRE(sets.merge(3, 1));
	const auto originalGroups = sets.groups();
	const size_t originalCount = sets.countSets();

	// Exceed the backing array's limit without attempting a large allocation.
	const size_t oversized = (Array<int64>{}.max_size() + 1);
	REQUIRE(oversized <= DisjointSet<int64>::MaxSize);
	REQUIRE_THROWS_AS(sets.resize(oversized), std::length_error);
	CHECK(sets.size() == 4);
	CHECK(sets.countSets() == originalCount);
	CHECK(sets.groups() == originalGroups);
	CHECK(sets.connected(1, 3));
	CHECK(sets.size(1) == 2);

	sets.resize(5);
	CHECK(sets.countSets() == (originalCount + 1));
	CHECK(sets.isRoot(4));
	CHECK(sets.size(4) == 1);
	CHECK(sets.connected(1, 3));
}
