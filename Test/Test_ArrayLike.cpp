//-----------------------------------------------
//
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

namespace
{
	template <class T>
	constexpr void CheckOwningBorrows()
	{
		static_assert(requires(T& v) { v.data(); v.begin(); v.end(); });
		static_assert(requires(const T& v) { v.data(); v.begin(); v.end(); });
		static_assert(not requires(T&& v) { std::move(v).data(); });
		static_assert(not requires(const T&& v) { std::move(v).data(); });
		static_assert(not requires(T&& v) { std::move(v).begin(); });
		static_assert(not requires(const T&& v) { std::move(v).begin(); });
		static_assert(not requires(T&& v) { std::move(v).end(); });
		static_assert(not requires(const T&& v) { std::move(v).end(); });
		static_assert(not requires(T&& v) { std::move(v).cbegin(); });
		static_assert(not requires(const T&& v) { std::move(v).cbegin(); });
		static_assert(not requires(T&& v) { std::move(v).cend(); });
		static_assert(not requires(const T&& v) { std::move(v).cend(); });
		static_assert(not requires(T&& v) { std::move(v).rbegin(); });
		static_assert(not requires(const T&& v) { std::move(v).rbegin(); });
		static_assert(not requires(T&& v) { std::move(v).rend(); });
		static_assert(not requires(const T&& v) { std::move(v).rend(); });
		static_assert(not requires(T&& v) { std::move(v).crbegin(); });
		static_assert(not requires(const T&& v) { std::move(v).crbegin(); });
		static_assert(not requires(T&& v) { std::move(v).crend(); });
		static_assert(not requires(const T&& v) { std::move(v).crend(); });
		static_assert(not requires(const T&& v) { std::move(v)[0]; });
		static_assert(not requires(const T&& v) { std::move(v).front(); });
		static_assert(not requires(const T&& v) { std::move(v).back(); });
		static_assert(not requires(const T&& v) { std::move(v).at(0); });
		static_assert(not requires(T&& v) { std::move(v).subspan(0, 0); });
		static_assert(not requires(const T&& v) { std::move(v).subspan(0, 0); });
		static_assert(not requires(T&& v) { std::move(v).get_if(0); });
		static_assert(not requires(const T&& v) { std::move(v).get_if(0); });
		static_assert(not requires(T&& v) { std::move(v).get_if(Point{}); });
		static_assert(not requires(const T&& v) { std::move(v).get_if(Point{}); });
		static_assert(not requires(T&& v) { std::move(v).get_if(0, 0); });
		static_assert(not requires(const T&& v) { std::move(v).get_if(0, 0); });
		static_assert(not requires(T&& v) { std::move(v).row(0); });
		static_assert(not requires(const T&& v) { std::move(v).row(0); });
		static_assert(not requires(T&& v) { std::move(v).column(0); });
		static_assert(not requires(const T&& v) { std::move(v).column(0); });
		static_assert(not requires(T&& v) { std::move(v).head_span(0); });
		static_assert(not requires(const T&& v) { std::move(v).head_span(0); });
		static_assert(not requires(T&& v) { std::move(v).tail_span(0); });
		static_assert(not requires(const T&& v) { std::move(v).tail_span(0); });
		static_assert(not requires(const T&& v) { std::move(v).head_view(0); });
		static_assert(not requires(const T&& v) { std::move(v).tail_view(0); });
		static_assert(not requires(const T&& v) { std::move(v).reverse_view(); });
		static_assert(not requires(const T&& v) { std::move(v).asArray(); });
		static_assert(not requires(const T&& v) { std::move(v).getContainer(); });
		static_assert(not requires(T&& v) { std::move(v).find_if([](const auto&) { return true; }); });
		static_assert(not requires(const T&& v) { std::move(v).find_if([](const auto&) { return true; }); });
		static_assert(not requires(T&& v) { std::move(v).dataAsUint8(); });
		static_assert(not requires(const T&& v) { std::move(v).dataAsUint8(); });
		static_assert(not requires(const T& v) { v.map([](const auto&) {}); });
		static_assert(not requires(const T& v) { v.map_indexed([](Point, const auto&) {}); });
		if constexpr (std::same_as<T, LineString> || std::same_as<T, MultiPolygon>)
		{
			static_assert(not requires(const T&& v) { std::move(v).choice(); });
			static_assert(not requires(const T&& v, DefaultRNG& rng) { std::move(v).choice(rng); });
			static_assert(not requires(T&& v) { std::move(v).emplace_back(); });
			static_assert(not requires(T&& v) { std::move(v).emplace_front(); });
		}
		if constexpr (requires(T& v) { v.insert(v.begin(), *v.begin()); })
		{
			static_assert(not requires(T&& v) { std::move(v).insert(v.begin(), *v.begin()); });
		}
		if constexpr (requires(T& v) { v.erase(v.begin()); })
		{
			static_assert(not requires(T&& v) { std::move(v).erase(v.begin()); });
		}
	}
}

TEST_CASE("ArrayLike.owning_borrow_contract")
{
	CheckOwningBorrows<LineString>();
	CheckOwningBorrows<MultiPolygon>();
	CheckOwningBorrows<Grid<int32>>();
	CheckOwningBorrows<Grid<bool>>();
	CheckOwningBorrows<Image>();
	CheckOwningBorrows<String>();
	CheckOwningBorrows<Blob>();
	CheckOwningBorrows<Wave>();
}
