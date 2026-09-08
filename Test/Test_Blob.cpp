//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"

TEST_CASE("Blob.array_like_contract")
{
	static_assert(std::same_as<decltype(Blob{}[0]), Byte>);
	static_assert(std::same_as<decltype(Blob{}.asArray()), Array<Byte>>);
	const Array<Byte> data{ Byte{ 1 }, Byte{ 2 }, Byte{ 3 } };
	Blob blob{ data };
	CHECK_EQ(blob.get_if(1), &blob[1]);
	CHECK_EQ(std::as_const(blob).get_if(1), &blob[1]);
	CHECK_EQ(blob.get_if(3), nullptr);
	CHECK_EQ(blob.take(2).asArray(), data.take(2));
	CHECK_EQ(blob.drop(1).asArray(), data.drop(1));
	CHECK(blob.drop(100).isEmpty());
	CHECK_EQ(blob.slice(1).asArray(), data.drop(1));
	CHECK_EQ(blob.slice(1, 1).asArray(), data.slice(1, 1));
	CHECK(blob.slice(3).isEmpty());
	CHECK_THROWS_AS((void) blob.slice(4), std::out_of_range);
	CHECK_THROWS_AS((void) blob.slice(2, 2), std::out_of_range);
	blob.reserve(32);
	const auto storage = blob.data();
	auto result = std::move(blob).drop(1).take(1);
	CHECK_EQ(result.data(), storage);
	CHECK_EQ(result.asArray(), Array<Byte>{ Byte{ 2 } });
	result.append(result);
	CHECK_EQ(result.asArray(), Array<Byte>{ Byte{ 2 }, Byte{ 2 } });
	result.append(nullptr, 0);
	CHECK_EQ(result.size(), size_t{ 2 });
	result.release();
	CHECK_EQ(result.capacity(), size_t{ 0 });
	CHECK(result.isEmpty());
	CHECK_EQ(result.get_if(0), nullptr);
}

TEST_CASE("Blob.overlapping_append")
{
	for (const bool spareCapacity : { false, true })
	{
		Blob blob{ Array<Byte>{ Byte{ 1 }, Byte{ 2 }, Byte{ 3 }, Byte{ 4 } } };
		if (spareCapacity) { blob.reserve(32); }
		else { blob.shrink_to_fit(); }
		blob.append(blob.data() + 1, 2);
		CHECK_EQ(blob.asArray(), Array<Byte>{ Byte{ 1 }, Byte{ 2 }, Byte{ 3 }, Byte{ 4 }, Byte{ 2 }, Byte{ 3 } });
	}
}
