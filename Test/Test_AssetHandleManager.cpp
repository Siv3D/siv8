//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include <Siv3D/AssetMonitor/CAssetMonitor.hpp>

namespace s3d::Test
{
	// Exercise the real manager's wrap path without exhausting uint32 IDs.
	template <uint8 Limit>
	struct ManagerTestID
	{
		using value_type = uint8;
		static constexpr value_type InvalidID = Limit;
		value_type key = 0;

		constexpr ManagerTestID(const value_type id = 0) : key{ id } {}
		constexpr value_type value() const noexcept { return key; }
		constexpr bool isNull() const noexcept { return (key == 0); }
		static constexpr ManagerTestID Null() noexcept { return {}; }
		friend bool operator ==(ManagerTestID, ManagerTestID) = default;
	};
}

namespace std
{
	template <s3d::uint8 Limit>
	struct hash<s3d::Test::ManagerTestID<Limit>>
	{
		size_t operator ()(const s3d::Test::ManagerTestID<Limit> id) const noexcept { return id.value(); }
	};
}

namespace
{
	using SmallID = s3d::Test::ManagerTestID<7>;
	using ByteID = s3d::Test::ManagerTestID<255>;

	struct Asset
	{
		int value = 0;
		std::shared_ptr<int> lifetime;
	};

	struct MonitorState
	{
		s3d::CAssetMonitor& monitor = *static_cast<s3d::CAssetMonitor*>(SIV3D_ENGINE(AssetMonitor));
		const s3d::CAssetMonitor saved = monitor;
		~MonitorState() { monitor = saved; }
	};
}

TEMPLATE_TEST_CASE("AssetHandleManager.exhaustion_and_last_free_id", "", SmallID, ByteID)
{
	const MonitorState restore;
	for (const bool withNull : { false, true })
	{
		CAPTURE(withNull, TestType::InvalidID);
		AssetHandleManager<TestType, Asset> manager{ "TestAsset" };
		if (withNull) { manager.setNullData(std::make_unique<Asset>(Asset{ -1, {} })); }
		Array<Asset*> originals;
		for (uint16 i = 1; i < TestType::InvalidID; ++i)
		{
			const auto id = manager.add(std::make_unique<Asset>(Asset{ i, {} }));
			REQUIRE(id.value() == i);
			originals.push_back(manager[id]);
		}
		for (int attempt = 0; attempt < 3; ++attempt)
		{
			auto rejected = std::make_unique<Asset>();
			rejected->lifetime = std::make_shared<int>(42);
			const std::weak_ptr<int> lifetime = rejected->lifetime;
			CHECK(manager.add(std::move(rejected)).isNull());
			CHECK(lifetime.expired());
		}
		REQUIRE(manager.size() == (static_cast<size_t>(TestType::InvalidID) - 1 + withNull));
		for (uint16 i = 1; i < TestType::InvalidID; ++i)
		{
			const TestType id{ static_cast<uint8>(i) };
			CHECK(manager[id] == originals[i - 1]);
			CHECK(manager[id]->value == i);
		}
		// A single free slot is usable even when the null resource is present.
		for (const uint8 freeID : { uint8{ 1 }, uint8{ 3 }, static_cast<uint8>(TestType::InvalidID - 1) })
		{
			manager.erase(TestType{ freeID });
			const auto id = manager.add(std::make_unique<Asset>(Asset{ 1000 + freeID, {} }));
			REQUIRE(id.value() == freeID);
			CHECK(manager[id]->value == (1000 + freeID));
			CHECK(manager.add(std::make_unique<Asset>()).isNull());
		}
		if (withNull) { CHECK(manager[TestType::Null()]->value == -1); }
		for (const auto& [id, data] : manager)
		{
			CHECK(id.value() < TestType::InvalidID);
			CHECK(data != nullptr);
		}
	}
}

TEST_CASE("AssetHandleManager.reuse_wraps_to_free_ids_before_cursor")
{
	const MonitorState restore;
	AssetHandleManager<SmallID, int> manager{ "TestAsset" };
	manager.setNullData(std::make_unique<int>(-1));
	for (uint8 i = 1; i < SmallID::InvalidID; ++i)
	{
		REQUIRE(manager.add(std::make_unique<int>(i)).value() == i);
	}
	manager.erase(SmallID{ 1 });
	manager.erase(SmallID{ 6 });
	CHECK(manager.add(std::make_unique<int>(10)).value() == 1);
	CHECK(manager.add(std::make_unique<int>(60)).value() == 6);
	// The only free slot repeatedly moves both ahead of and behind the cursor.
	for (int cycle = 0; cycle < 24; ++cycle)
	{
		const uint8 freeID = static_cast<uint8>(1 + ((cycle * 5) % 6));
		manager.erase(SmallID{ freeID });
		REQUIRE(manager.add(std::make_unique<int>(cycle)).value() == freeID);
		CHECK(*manager[SmallID{ freeID }] == cycle);
	}
	CHECK(*manager[SmallID::Null()] == -1);
	CHECK(manager.size() == 7);
}

TEST_CASE("AssetHandleManager.uses_monotonic_ids_until_first_wrap")
{
	const MonitorState restore;
	AssetHandleManager<SmallID, int> manager{ "TestAsset" };
	REQUIRE(manager.add(std::make_unique<int>(1)).value() == 1);
	manager.erase(SmallID{ 1 });
	for (uint8 i = 2; i < SmallID::InvalidID; ++i)
	{
		REQUIRE(manager.add(std::make_unique<int>(i)).value() == i);
	}
	CHECK(manager.add(std::make_unique<int>(11)).value() == 1);
	CHECK(*manager[SmallID{ 1 }] == 11);
}
