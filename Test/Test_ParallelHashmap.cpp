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
# include <memory>

TEST_CASE("ParallelHashmap.rehash_zero")
{
	for (const int32 count : { 15, 31, 63, 127, 255 })
	{
		CAPTURE(count);
		HashMap<int32, int32> map;
		HashSet<int32> set;
		for (int32 key = 0; key < count; ++key)
		{
			map.emplace(key, key * 3);
			set.insert(key);
		}

		map.rehash(0);
		set.rehash(0);
		REQUIRE(map.size() == static_cast<size_t>(count));
		REQUIRE(set.size() == static_cast<size_t>(count));
		// A regressed full table can loop forever on an absent-key lookup.
		REQUIRE(map.capacity() > map.size());
		REQUIRE(set.capacity() > set.size());
		CHECK_FALSE(map.contains(-1));
		CHECK_FALSE(set.contains(-1));
		for (int32 key = 0; key < count; ++key)
		{
			CHECK(map.at(key) == key * 3);
			CHECK(set.contains(key));
		}
		CHECK(map.emplace(count, count * 3).second);
		CHECK(set.insert(count).second);
		CHECK(map.at(count) == count * 3);
		CHECK(set.contains(count));
	}
}

TEST_CASE("ParallelHashmap.reserve_existing_size")
{
	for (const int32 count : { 7, 14, 15, 28, 31, 56, 63, 112, 127, 224, 255 })
	{
		CAPTURE(count);
		HashMap<int32, int32> map;
		HashSet<int32> set;
		for (int32 key = 0; key < count; ++key)
		{
			map.emplace(key, key);
			set.insert(key);
		}
		const size_t mapCapacity = map.capacity();
		const size_t setCapacity = set.capacity();
		// reserve(0) may shrink flat containers; only positive requests are no-ops.
		for (const size_t request : { size_t{ 1 }, map.size() / 2, map.size() })
		{
			CAPTURE(request);
			map.reserve(request);
			set.reserve(request);
			CHECK(map.capacity() == mapCapacity);
			CHECK(set.capacity() == setCapacity);
			CHECK(map.size() == static_cast<size_t>(count));
			CHECK(set.size() == static_cast<size_t>(count));
		}
	}
}

TEST_CASE("ParallelHashmap.string_lookup")
{
	const String backing = U"--Siv3D--";
	const StringView slice{ backing.data() + 2, 5 };
	const String embeddedNull{ U"A\0B", 3 };
	HashMap<String, int32> map{ { U"Siv3D", 1 }, { embeddedNull, 2 } };
	HashSet<String> set{ U"Siv3D", embeddedNull };
	CHECK(map.at(String{ U"Siv3D" }) == 1);
	CHECK(map.at(U"Siv3D") == 1);
	CHECK(map.at(slice) == 1);
	CHECK(map.at(StringView{ embeddedNull }) == 2);
	CHECK(set.contains(String{ U"Siv3D" }));
	CHECK(set.contains(U"Siv3D"));
	CHECK(set.contains(slice));
	CHECK(set.contains(StringView{ embeddedNull }));
	CHECK_FALSE(map.contains(U"A"));
	CHECK_FALSE(set.contains(U"A"));
	CHECK(map.erase(slice) == 1);
	CHECK(set.erase(slice) == 1);
	CHECK_FALSE(map.contains(slice));
	CHECK_FALSE(set.contains(slice));
	CHECK(map.at(StringView{ embeddedNull }) == 2);
	CHECK(set.contains(StringView{ embeddedNull }));
}

TEST_CASE("ParallelHashmap.move_only_mapped_values")
{
	HashMap<int32, std::unique_ptr<int32>> map;
	Array<int32*> addresses;
	for (int32 key = 0; key < 63; ++key)
	{
		auto value = std::make_unique<int32>(key * 3);
		addresses.push_back(value.get());
		REQUIRE(map.try_emplace(key, std::move(value)).second);
		CHECK(value == nullptr);
	}
	map.reserve(256);
	auto duplicate = std::make_unique<int32>(-1);
	CHECK_FALSE(map.try_emplace(0, std::move(duplicate)).second);
	REQUIRE(duplicate != nullptr);
	CHECK(*duplicate == -1);

	auto moved = std::move(map);
	REQUIRE(moved.size() == addresses.size());
	for (int32 key = 0; key < 63; ++key)
	{
		REQUIRE(moved.at(key).get() == addresses[key]);
		CHECK(*moved.at(key) == key * 3);
	}
	CHECK(moved.erase(0) == 1);
	CHECK(moved.try_emplace(0, std::move(duplicate)).second);
	REQUIRE(moved.at(0) != nullptr);
	CHECK(*moved.at(0) == -1);
}

TEST_CASE("ParallelHashmap.btree_order_and_erase")
{
	BTreeMap<int32, int32> map;
	BTreeSet<int32> set;
	for (int32 key = 127; 0 <= key; --key)
	{
		map.emplace(key, key * 3);
		set.insert(key);
	}
	for (int32 key = 0; key < 128; key += 2)
	{
		CHECK(map.erase(key) == 1);
		CHECK(set.erase(key) == 1);
	}
	REQUIRE(map.lower_bound(32) != map.end());
	REQUIRE(set.lower_bound(32) != set.end());
	CHECK(map.lower_bound(32)->first == 33);
	CHECK(*set.lower_bound(32) == 33);
	map.erase(map.lower_bound(32), map.upper_bound(63));
	set.erase(set.lower_bound(32), set.upper_bound(63));
	REQUIRE(map.size() == 48);
	REQUIRE(set.size() == 48);

	auto mapIt = map.begin();
	auto setIt = set.begin();
	for (int32 key = 1; key < 128; key += 2)
	{
		if ((32 <= key) && (key <= 63))
		{
			continue;
		}
		REQUIRE(mapIt != map.end());
		REQUIRE(setIt != set.end());
		CHECK(mapIt->first == key);
		CHECK(mapIt->second == key * 3);
		CHECK(*setIt == key);
		++mapIt;
		++setIt;
	}
	CHECK(mapIt == map.end());
	CHECK(setIt == set.end());
}
