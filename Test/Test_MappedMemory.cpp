//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"
# include <type_traits>

TEMPLATE_TEST_CASE("MappedMemory.non_owning_range", "", MappedMemory, MappedMemoryView)
{
	STATIC_REQUIRE(std::is_aggregate_v<TestType>);
	STATIC_REQUIRE(std::is_trivially_copyable_v<TestType>);
	STATIC_REQUIRE(std::is_trivially_destructible_v<TestType>);
	STATIC_REQUIRE(std::same_as<decltype(MappedMemory{}.data), void*>);
	STATIC_REQUIRE(std::same_as<decltype(MappedMemoryView{}.data), const void*>);
	constexpr TestType empty;
	STATIC_REQUIRE(empty.data == nullptr);
	STATIC_REQUIRE(empty.size == 0);
	STATIC_REQUIRE(not static_cast<bool>(empty));
	STATIC_REQUIRE(noexcept(static_cast<bool>(empty)));
	uint8 bytes[]{ 1, 2, 3 };
	const TestType mapped{ .data = bytes, .size = sizeof(bytes) };
	CHECK(static_cast<bool>(mapped));
	CHECK(mapped.data == bytes);
	CHECK(mapped.size == sizeof(bytes));
	const TestType copy = mapped;
	CHECK(copy.data == mapped.data);
	CHECK(copy.size == mapped.size);
	bytes[1] = 42;
	CHECK(static_cast<const uint8*>(copy.data)[1] == 42);
	if constexpr (std::same_as<TestType, MappedMemory>)
	{
		static_cast<uint8*>(copy.data)[2] = 99;
		CHECK(bytes[2] == 99);
	}
	// The predicate only checks the pointer, not size or the owner's lifetime.
	CHECK(static_cast<bool>(TestType{ .data = bytes, .size = 0 }));
	CHECK_FALSE(static_cast<bool>(TestType{ .data = nullptr, .size = 1 }));
}
