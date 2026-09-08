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

TEST_CASE("StringView.view")
{
	static_assert(std::ranges::enable_view<StringView>);
	static_assert(std::ranges::borrowed_range<StringView>);
}

TEST_CASE("StringView.owner_lifetime_contract")
{
	const String owner = U"alpha,beta";
	// A temporary view is safe when its backing owner remains alive.
	const auto data = StringView{ owner }.data();
	CHECK_EQ(data, owner.data());
	const auto parts = StringView{ owner }.splitView(U',');
	CHECK_EQ(parts, Array<StringView>{ U"alpha", U"beta" });
	CHECK_EQ(parts[0].data(), owner.data());
	CHECK_EQ(parts[1].data(), owner.data() + 6);
	const auto length = [](StringView v) { return v.size(); };
	CHECK_EQ(length(String{ U"temporary" }), size_t{ 9 });
	CHECK_EQ(StringView{ U"abc" }.map([](char32 ch) { return static_cast<int32>(ch); }), Array<int32>{ 97, 98, 99 });
	CHECK_THROWS_AS((void) StringView{ U"abc" }.filter([](char32) -> bool { throw std::runtime_error("predicate"); }), std::runtime_error);
}

namespace
{
	template <class T>
	constexpr bool CanMapVoidView = requires(const T& view) { view.map([](char32) {}); };
	static_assert(not CanMapVoidView<StringView>);
}
