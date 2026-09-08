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

struct IntArray
{
	using value_type = int32;

	Array<int32> v = { 1, 2, 3 };

	constexpr const Array<int32>& asArray() const&
	{
		return v;
	}

	constexpr Array<int32>&& asArray()&&
	{
		return std::move(v);
	}
};

TEST_CASE("Array")
{
	const Array<int32> a123{ 1, 2, 3 };
	const Array<int32> a111{ 1, 1, 1 };
	const Array<int32> a000{ 0, 0, 0 };
	const std::vector<int32> v123{ 1, 2, 3 };

	{
		const Array<int32> v;
		CHECK_EQ(v, Array<int32>());
	}

	{
		const Array<int32> a(a123);
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a{ Array<int32>(a123) };
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a(v123);
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a{ std::vector<int32>(v123) };
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a(std::allocator<int32>{});
		CHECK(a.isEmpty());
		CHECK_EQ(a.get_allocator(), std::allocator<int32>{});
	}

	{
		const Array<int32> a(3, 1);
		CHECK_EQ(a, a111);
	}

	{
		const Array<int32> a(3);
		CHECK_EQ(a, a000);
	}

	{
		const Array<int32> a(a123.begin(), a123.end());
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a(a123, a123.get_allocator());
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a(Array<int32>(a123), a123.get_allocator());
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a(v123, v123.get_allocator());
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a(std::vector<int32>(v123), v123.get_allocator());
		CHECK_EQ(a, a123);
	}

	{
		const Array<int32> a{ 1, 2, 3 };
		CHECK_EQ(a, a123);
	}

	{
		static_assert(HasAsArray<IntArray>);
		static_assert(Array<int32>{ IntArray() } == Array<int32>{ 1, 2, 3 });
	}

	{
		static_assert(HasAsArray<IntArray>);
		const IntArray ia;
		const Array<int32> a{ ia };
		CHECK_EQ(a, a123);
	}

	{
		int32 i = 1;
		const Array<int32> v(3, Arg::generator = [&i]() { return i++; });
		CHECK_EQ(v, a123);
	}

	{
		const Array<int32> v(3, Arg::generator = [](size_t i) { return static_cast<int32>(i + 1); });
		CHECK_EQ(v, a123);
	}

	{
		Array<int32> v{ Arg::reserve = 100 };
		CHECK(v.isEmpty());
		CHECK(100 <= v.capacity());
	}

	{
		Array<int32> v;
		v = a123;
		CHECK_EQ(v, a123);
	}

	{
		Array<int32> v;
		v = Array<int32>(a123);
		CHECK_EQ(v, a123);
	}

	{
		Array<int32> v;
		v = v123;
		CHECK_EQ(v, a123);
	}

	{
		Array<int32> v;
		v = std::vector<int32>(v123);
		CHECK_EQ(v, a123);
	}

}

TEST_CASE("Array.all")
{
	{
		const Array<int32> v{ 1, 3, 5 };
		CHECK(v.all([](int32 x) { return (0 < x); }));
		CHECK(not v.all([](int32 x) { return (1 < x); }));
		CHECK(v.all(IsOdd));
		CHECK(not v.all(IsEven));
	}

	{
		const Array<String> v{ U"abc", U"def", U"ghi" };
		CHECK(v.all([](const String& s) { return (s.length() == 3); }));
		CHECK(not v.all([](const String& s) { return (s.length() == 2); }));
		CHECK(v.all(&String::operator bool));
		CHECK(not v.all(&String::isEmpty));
	}

	{
		static_assert(Array<int32>{}.all(IsEven) == true);
		static_assert(Array<int32>{ 1 }.all(IsEven) == false);
		static_assert(Array<int32>{ 1, 2, 3 }.all(IsEven) == false);
		static_assert(Array<int32>{ 1, 3, 5 }.all(IsEven) == false);
		static_assert(Array<int32>{ 2, 4, 6 }.all(IsEven) == true);

		static_assert(Array<int32>{}.all(IsOdd) == true);
		static_assert(Array<int32>{ 1 }.all(IsOdd) == true);
		static_assert(Array<int32>{ 1, 2, 3 }.all(IsOdd) == false);
		static_assert(Array<int32>{ 1, 3, 5 }.all(IsOdd) == true);
		static_assert(Array<int32>{ 2, 4, 6 }.all(IsOdd) == false);
	}
}

TEST_CASE("Array<bool>.sort_by")
{
	const Array<bool> source = { true, false, true, false };
	const Array<bool> ascending = { false, false, true, true };
	const Array<bool> descending = { true, true, false, false };

	Array<bool> values = source;
	CHECK_EQ(&(values.sort_by(std::greater<>{})), &values);
	CHECK_EQ(values, descending);

	CHECK_EQ(Array<bool>{ source }.sort_by(std::less<>{}), ascending);
	CHECK_EQ(source.sorted_by(std::greater<>{}), descending);
	CHECK_EQ(Array<bool>{ source }.sorted_by(std::less<>{}), ascending);
	CHECK_EQ(source, Array<bool>{ true, false, true, false });
	CHECK(Array<bool>{}.sort_by(std::less<>{}).isEmpty());
	CHECK_EQ(Array<bool>{ true }.sorted_by(std::greater<>{}), Array<bool>{ true });
}

TEST_CASE("Array<bool>.stable_sort_by")
{
	const Array<bool> source = { true, false, true, false };
	const Array<bool> ascending = { false, false, true, true };
	const Array<bool> descending = { true, true, false, false };

	Array<bool> values = source;
	CHECK_EQ(&(values.stable_sort_by(std::greater<>{})), &values);
	CHECK_EQ(values, descending);

	CHECK_EQ(Array<bool>{ source }.stable_sort_by(std::less<>{}), ascending);
	CHECK_EQ(source.stable_sorted_by(std::greater<>{}), descending);
	CHECK_EQ(Array<bool>{ source }.stable_sorted_by(std::less<>{}), ascending);

	const auto equivalent = [](const bool, const bool) { return false; };
	CHECK_EQ(source.stable_sorted_by(equivalent), source);
	CHECK(Array<bool>{}.stable_sort_by(std::less<>{}).isEmpty());
	CHECK_EQ(Array<bool>{ false }.stable_sorted_by(std::greater<>{}), Array<bool>{ false });
}

namespace
{
	struct ArrayTestRecord
	{
		int value;
		bool selected;
		int doubled() const { return (value * 2); }
	};

	struct ArrayLessOnly
	{
		int value;
		friend bool operator <(const ArrayLessOnly& a, const ArrayLessOnly& b) { return (a.value < b.value); }
	};

	struct ArrayConstructOnly
	{
		int value;
		ArrayConstructOnly() = delete;
		explicit ArrayConstructOnly(int v) : value(v) {}
		ArrayConstructOnly(ArrayConstructOnly&&) = default;
		ArrayConstructOnly(const ArrayConstructOnly&) = delete;
		ArrayConstructOnly& operator =(ArrayConstructOnly&&) = delete;
	};

	struct ArrayThrowingEquality
	{
		int value;
		friend bool operator ==(const ArrayThrowingEquality&, const ArrayThrowingEquality&) { throw std::runtime_error("comparison"); }
	};

	struct ArrayUnhashable
	{
		int value;
		friend bool operator ==(const ArrayUnhashable&, const ArrayUnhashable&) = default;
	};

	struct ArrayAdlHash
	{
		int value;
		friend bool operator ==(const ArrayAdlHash&, const ArrayAdlHash&) = default;
		friend size_t hash_value(const ArrayAdlHash& v) { return static_cast<size_t>(v.value); }
	};

	struct ArrayAllocationState
	{
		size_t allocations = 0;
		size_t deallocations = 0;
	};

	template <class T>
	struct ArrayTestAllocator
	{
		using value_type = T;
		ArrayAllocationState* state = nullptr;
		ArrayTestAllocator() = default;
		explicit ArrayTestAllocator(ArrayAllocationState* s) : state(s) {}
		template <class U>
		ArrayTestAllocator(const ArrayTestAllocator<U>& a) : state(a.state) {}
		T* allocate(size_t n)
		{
			if (state) { ++state->allocations; }
			return std::allocator<T>{}.allocate(n);
		}
		void deallocate(T* ptr, size_t n)
		{
			if (state) { ++state->deallocations; }
			std::allocator<T>{}.deallocate(ptr, n);
		}
		// Deliberately select a different allocator for ordinary copies.
		ArrayTestAllocator select_on_container_copy_construction() const { return {}; }
		template <class U>
		bool operator ==(const ArrayTestAllocator<U>& a) const { return (state == a.state); }
	};

	template <class A>
	concept ArrayCanBorrow = requires(A&& a)
	{
		std::forward<A>(a).data();
		std::forward<A>(a).begin();
		std::forward<A>(a).subspan(0, 0);
		std::forward<A>(a).head_span(0);
		std::forward<A>(a).tail_span(0);
		std::forward<A>(a).get_if(0);
		std::forward<A>(a).find_if([](auto) { return true; });
	};

	template <class A>
	concept ArrayCanPartition = requires(A&& a) { std::forward<A>(a).partition([](auto) { return true; }); };

	template <class A>
	concept ArrayCanStableUnique = requires(A& a) { a.stable_unique(); };

	template <class A>
	concept ArrayCanMapVoid = requires(const A& a) { a.map([](auto) {}); };

	template <class A>
	concept ArrayCanConstRvalueFront = requires(const A&& a) { std::move(a).front(); };

	template <class A>
	void CheckArrayLifetimeTypes()
	{
		static_assert(ArrayCanBorrow<A&>);
		static_assert(ArrayCanBorrow<const A&>);
		static_assert(not ArrayCanBorrow<A>);
		static_assert(not ArrayCanBorrow<const A>);
		static_assert(ArrayCanPartition<A&>);
		static_assert(not ArrayCanPartition<A>);
		static_assert(not ArrayCanConstRvalueFront<A>);
		static_assert(not requires(A&& a) { std::move(a).data(); });
		static_assert(not requires(const A&& a) { std::move(a).data(); });
		static_assert(not requires(A&& a) { std::move(a).begin(); });
		static_assert(not requires(const A&& a) { std::move(a).begin(); });
		static_assert(not requires(A&& a) { std::move(a).end(); });
		static_assert(not requires(const A&& a) { std::move(a).end(); });
		static_assert(not requires(A&& a) { std::move(a).cbegin(); });
		static_assert(not requires(const A&& a) { std::move(a).cbegin(); });
		static_assert(not requires(A&& a) { std::move(a).cend(); });
		static_assert(not requires(const A&& a) { std::move(a).cend(); });
		static_assert(not requires(A&& a) { std::move(a).rbegin(); });
		static_assert(not requires(const A&& a) { std::move(a).rbegin(); });
		static_assert(not requires(A&& a) { std::move(a).rend(); });
		static_assert(not requires(const A&& a) { std::move(a).rend(); });
		static_assert(not requires(A&& a) { std::move(a).crbegin(); });
		static_assert(not requires(const A&& a) { std::move(a).crbegin(); });
		static_assert(not requires(A&& a) { std::move(a).crend(); });
		static_assert(not requires(const A&& a) { std::move(a).crend(); });
		static_assert(not requires(A&& a) { std::move(a).subspan(0, 0); });
		static_assert(not requires(const A&& a) { std::move(a).subspan(0, 0); });
		static_assert(not requires(A&& a) { std::move(a).head_span(0); });
		static_assert(not requires(const A&& a) { std::move(a).head_span(0); });
		static_assert(not requires(A&& a) { std::move(a).tail_span(0); });
		static_assert(not requires(const A&& a) { std::move(a).tail_span(0); });
		static_assert(not requires(A&& a) { std::move(a).get_if(0); });
		static_assert(not requires(const A&& a) { std::move(a).get_if(0); });
		static_assert(not requires(A&& a) { std::move(a).find_if([](auto) { return true; }); });
		static_assert(not requires(const A&& a) { std::move(a).find_if([](auto) { return true; }); });
		static_assert(not requires(A&& a) { std::move(a).partition([](auto) { return true; }); });
		static_assert(not requires(const A&& a) { std::move(a).partition([](auto) { return true; }); });
		static_assert(not requires(A&& a) { std::move(a).stable_partition([](auto) { return true; }); });
		static_assert(not requires(const A&& a) { std::move(a).stable_partition([](auto) { return true; }); });
		static_assert(not requires(const A&& a) { std::move(a).front(); });
		static_assert(not requires(const A&& a) { std::move(a).back(); });
		static_assert(not requires(const A&& a) { std::move(a).at(0); });
		static_assert(not requires(const A&& a) { std::move(a).operator [](0); });
		static_assert(not requires(const A&& a) { std::move(a).getContainer(); });
		static_assert(not requires(const A&& a) { std::move(a).head_view(0); });
		static_assert(not requires(const A&& a) { std::move(a).tail_view(0); });
		static_assert(not requires(const A&& a) { std::move(a).reverse_view(); });
		static_assert(not requires(const A&& a) { std::move(a).choice(); });
		static_assert(not ArrayCanMapVoid<A>);
		static_assert(std::same_as<decltype(std::declval<A&&>().front()), typename A::value_type>);
		static_assert(std::same_as<decltype(std::declval<A&&>().back()), typename A::value_type>);
		static_assert(std::same_as<decltype(std::declval<A&&>().choice()), typename A::value_type>);
		static_assert(std::same_as<decltype(std::declval<A&&>().fill({})), A>);
		static_assert(std::same_as<decltype(std::declval<A&&>().append(std::declval<A&&>())), A>);
		static_assert(std::same_as<decltype(std::declval<A&&>() << typename A::value_type{}), A>);
		static_assert(std::same_as<decltype(std::declval<A&>().partition([](auto) { return true; })), typename A::iterator>);
	}
}

TEST_CASE("Array.asArray_assignment")
{
	Array<int32> a;
	IntArray source;
	a = source;
	CHECK_EQ(a, source.v);
	CHECK_EQ(source.v, Array<int32>{ 1, 2, 3 });
	a = std::as_const(source);
	CHECK_EQ(a, source.v);
	a = IntArray{};
	CHECK_EQ(a, Array<int32>{ 1, 2, 3 });
	static_assert(not std::is_constructible_v<Array<String>, IntArray>);
	static_assert(not std::is_assignable_v<Array<String>&, IntArray>);

	struct BoolSource
	{
		Array<bool> asArray() const { return { true, false }; }
	};
	Array<bool> flags;
	flags = BoolSource{};
	CHECK_EQ(flags, Array<bool>{ true, false });
	CHECK_EQ(Array<bool>{ BoolSource{} }, flags);

	struct MoveSource
	{
		Array<std::unique_ptr<int>> values;
		Array<std::unique_ptr<int>> asArray() && { return std::move(values); }
	};
	MoveSource movable;
	movable.values.push_back(std::make_unique<int>(42));
	Array<std::unique_ptr<int>> owned;
	owned = std::move(movable);
	REQUIRE_EQ(owned.size(), size_t{ 1 });
	CHECK_EQ(*owned[0], 42);
}

TEST_CASE("Array.comparison_contracts")
{
	Array<ArrayLessOnly> a{ { 3 }, { 1 }, { 2 }, { 1 } };
	CHECK_FALSE(a.isSorted());
	CHECK_EQ(a.sorted().map(&ArrayLessOnly::value), Array<int>{ 1, 1, 2, 3 });
	CHECK_EQ(a.stable_sorted().map(&ArrayLessOnly::value), Array<int>{ 1, 1, 2, 3 });
	CHECK_EQ(a.rsorted().map(&ArrayLessOnly::value), Array<int>{ 3, 2, 1, 1 });
	a.sort();
	CHECK(a.isSorted());
	CHECK(Array<ArrayLessOnly>{}.isSorted());

	Array<ArrayTestRecord> records{ { 2, false }, { 1, false }, { 2, true } };
	records.stable_sort_by([](const auto& x, const auto& y) { return (x.value < y.value); });
	CHECK_EQ(records.map(&ArrayTestRecord::selected), Array<bool>{ false, false, true });
}

TEST_CASE("Array.invocable_contracts")
{
	const Array<ArrayTestRecord> a{ { 1, true }, { 2, false }, { 3, true } };
	CHECK_EQ(a.map(&ArrayTestRecord::value), Array<int>{ 1, 2, 3 });
	CHECK_EQ(a.map(&ArrayTestRecord::doubled), Array<int>{ 2, 4, 6 });
	CHECK_EQ(a.filter(&ArrayTestRecord::selected).map(&ArrayTestRecord::value), Array<int>{ 1, 3 });
	CHECK_EQ(a.without_if(&ArrayTestRecord::selected).map(&ArrayTestRecord::value), Array<int>{ 2 });
	CHECK_EQ(a.replaced_if(&ArrayTestRecord::selected, ArrayTestRecord{ 9, false }).map(&ArrayTestRecord::value), Array<int>{ 9, 2, 9 });
	CHECK_EQ(a.parallel_map(&ArrayTestRecord::doubled), Array<int>{ 2, 4, 6 });
}

TEST_CASE("Array.lifetime_and_partition")
{
	CheckArrayLifetimeTypes<Array<int>>();
	CheckArrayLifetimeTypes<Array<bool>>();
	const auto& value = Array{ 1, 2 }.fill(7).front();
	CHECK_EQ(value, 7);
	const auto& flag = Array<bool>{ true, false }.fill(true).back();
	CHECK(flag);
	CHECK_EQ(Array{ 1 }.append({ 2, 3 }), Array{ 1, 2, 3 });
	CHECK_EQ((Array{ 1 } << 2 << 3), Array{ 1, 2, 3 });

	Array<int> a{ 1, 2, 3, 4, 5 };
	const auto boundary = a.stable_partition(IsEven);
	CHECK_EQ(boundary - a.begin(), 2);
	CHECK_EQ(a, Array{ 2, 4, 1, 3, 5 });
	CHECK_EQ(a.partition([](int) { return true; }), a.end());
	CHECK_EQ(a.partition([](int) { return false; }), a.begin());
	Array<int> empty;
	CHECK_EQ(empty.stable_partition(IsEven), empty.end());

	Array<bool> flags{ true, false, true, false };
	CHECK_EQ(flags.stable_partition(Identity) - flags.begin(), 2);
	CHECK_EQ(flags, Array<bool>{ true, true, false, false });

	a.head_span(1)[0] = 10;
	CHECK_EQ(a[0], 10);
	const auto view = Array{ 1, 2, 3 }.head_view(2);
	CHECK_EQ(ToArray(view), Array{ 1, 2 });
	CHECK_EQ(ToArray(Array{ 1, 2, 3 }.tail_view(2)), Array{ 2, 3 });
	CHECK_EQ(ToArray(Array{ 1, 2, 3 }.reverse_view()), Array{ 3, 2, 1 });
}

TEST_CASE("Array.search")
{
	Array<int> a{ 10, 20, 30, 20 };
	REQUIRE(a.get_if(1));
	*a.get_if(1) = 21;
	CHECK_EQ(a[1], 21);
	CHECK_EQ(a.get_if(a.size()), nullptr);
	CHECK_EQ(a.get_if(std::numeric_limits<size_t>::max()), nullptr);
	CHECK_EQ(Array<int>{}.indexOf_if(IsEven), none);
	CHECK_EQ(a.find_if([](int x) { return (x == 20); }), &a[3]);
	CHECK_EQ(std::as_const(a).find_if([](int x) { return (x > 100); }), nullptr);
	CHECK_EQ(a.indexOf_if([](int x) { return (x >= 20); }), Optional<size_t>{ 1 });
	CHECK_EQ(Array{ 1, 2, 3 }.indexOf_if(IsEven), Optional<size_t>{ 1 });
	const Array<ArrayTestRecord> records{ { 1, false }, { 2, true } };
	CHECK_EQ(records.find_if(&ArrayTestRecord::selected), &records[1]);
	CHECK_EQ(records.indexOf_if(&ArrayTestRecord::selected), Optional<size_t>{ 1 });
	Array<bool> flags{ false, true, true };
	CHECK_EQ(flags.find_if(Identity), &flags[1]);
	CHECK_EQ(flags.indexOf_if(Identity), Optional<size_t>{ 1 });
	CHECK_EQ(flags.get_if(3), nullptr);
}

TEST_CASE("Array.take_drop_slice_filter")
{
	const Array<int> a{ 1, 2, 3, 4 };
	for (const size_t n : { size_t{ 0 }, size_t{ 1 }, size_t{ 4 }, size_t{ 5 }, std::numeric_limits<size_t>::max() })
	{
		CHECK_EQ(a.take(n).append(a.drop(n)), a);
		CHECK_EQ(Array{ a }.take(n), a.take(n));
		CHECK_EQ(Array{ a }.drop(n), a.drop(n));
	}
	CHECK_EQ(a.drop_while([](int x) { return (x < 3); }), Array{ 3, 4 });
	CHECK_EQ(Array{ a }.drop_while([](int x) { return (x < 3); }), Array{ 3, 4 });
	CHECK(a.drop_while([](int) { return true; }).empty());
	CHECK_EQ(a.drop_while([](int) { return false; }), a);
	CHECK_EQ(a.slice(0), a);
	CHECK_EQ(a.slice(2), Array{ 3, 4 });
	CHECK(a.slice(4).empty());
	CHECK(Array{ a }.slice(4).empty());
	CHECK_THROWS_AS((void)a.slice(5), std::out_of_range);
	CHECK_THROWS_AS((void)Array{ a }.slice(5), std::out_of_range);
	CHECK_THROWS_AS((void)a.slice(3, 2), std::out_of_range);
	CHECK_EQ(a.filter(IsEven), Array{ 2, 4 });
	CHECK_EQ(Array{ a }.filter(IsEven), Array{ 2, 4 });
	CHECK(Array<int>{}.drop(4).empty());
	CHECK(Array<int>{}.filter(IsEven).empty());
	CHECK_EQ(a, Array{ 1, 2, 3, 4 });

	const Array<bool> flags{ false, true, false, true };
	CHECK_EQ(flags.take(2).append(flags.drop(2)), flags);
	CHECK_EQ(Array{ flags }.filter(Identity), Array<bool>{ true, true });
	CHECK_EQ(Array{ flags }.drop_while([](bool v) { return not v; }), Array<bool>{ true, false, true });
	CHECK_EQ(flags.slice(1), Array<bool>{ true, false, true });
	CHECK_THROWS_AS((void)Array{ flags }.slice(5), std::out_of_range);
}

TEST_CASE("Array.move_only_and_storage_reuse")
{
	Array<std::unique_ptr<int>> a;
	a.reserve(16);
	a.push_back(std::make_unique<int>(1));
	a.push_back(std::make_unique<int>(2));
	a.push_back(std::make_unique<int>(3));
	const auto storage = a.data();
	const auto capacity = a.capacity();
	a = std::move(a).filter([](const auto& p) { return (*p != 2); });
	CHECK_EQ(a.data(), storage);
	CHECK_EQ(a.capacity(), capacity);
	CHECK_EQ(a.size(), size_t{ 2 });
	CHECK_EQ(*a.back(), 3);
	a = std::move(a).take(1);
	CHECK_EQ(a.data(), storage);
	CHECK_EQ(*a.front(), 1);
	Array<std::unique_ptr<int>> b;
	b.push_back(std::make_unique<int>(4));
	a.append(std::move(b));
	CHECK_EQ(*a.back(), 4);
	a.append(std::move(a));
	CHECK_EQ(a.size(), size_t{ 2 });
	CHECK_EQ(*a.front(), 1);
	a = std::move(a).drop(1);
	CHECK_EQ(a.data(), storage);
	CHECK_EQ(*a.front(), 4);

	struct NoDefault
	{
		int value;
		explicit NoDefault(int v) : value(v) {}
	};
	Array<NoDefault> values;
	values.emplace_back(7);
	CHECK_EQ(std::move(values).take(1).front().value, 7);

	Array<int> self{ 1, 2 };
	self.append(self);
	CHECK_EQ(self, Array{ 1, 2, 1, 2 });
	self.append(std::move(self));
	CHECK_EQ(self, Array{ 1, 2, 1, 2 });
}

TEST_CASE("Array.allocator_and_nested_arrays")
{
	ArrayAllocationState state;
	using A = Array<int, ArrayTestAllocator<int>>;
	const A a({ 3, 1, 2, 1 }, ArrayTestAllocator<int>{ &state });
	const auto verify = [&state](const A& result) { CHECK_EQ(result.get_allocator().state, &state); };
	verify(a.take(2));
	verify(a.drop(1));
	verify(a.take_while([](int x) { return (x > 1); }));
	verify(a.drop_while([](int x) { return (x > 1); }));
	verify(a.slice(1));
	verify(a.filter(IsOdd));
	verify(a.sorted());
	verify(a.rsorted());
	verify(a.stable_sorted());
	verify(a.reversed());
	verify(a.without(1));
	verify(a.without_at(0));
	verify(a.without_if(IsEven));
	verify(a.replaced(1, 5));
	verify(a.replaced_if(IsEven, 5));
	verify(a.values_at({ 3, 0 }));
	verify(a.stable_uniqued());
	verify(a.uniqued_consecutive());
	verify(a.choice(2));
	verify(a.shuffled());
	verify(a.slice(4));
	verify(a.take(0));

	const auto chunks = a.chunk(3);
	static_assert(std::same_as<std::remove_cvref_t<decltype(chunks)>, Array<A>>);
	REQUIRE_EQ(chunks.size(), size_t{ 2 });
	verify(chunks[0]);
	verify(chunks[1]);
	CHECK_EQ(chunks.flatten(), Array{ 3, 1, 2, 1 });
	CHECK_EQ(a.in_groups(3).map([](const A& v) { return v.size(); }), Array<size_t>{ 2, 1, 1 });
	CHECK_EQ(a.in_groups(20).size(), size_t{ 4 });
	CHECK(a.in_groups(0).empty());
	CHECK(a.chunk(0).empty());
	CHECK_EQ(a.chunk(std::numeric_limits<size_t>::max()).size(), size_t{ 1 });
	CHECK(A{}.chunk(2).empty());

	using Aligned = Array<int, AlignedAllocator<int, 32>>;
	CHECK_EQ(Aligned{ 1, 2, 3 }.chunk(2).flatten(), Array{ 1, 2, 3 });
	Array<Array<std::unique_ptr<int>>> nested;
	Array<std::unique_ptr<int>> inner;
	inner.push_back(std::make_unique<int>(8));
	nested.push_back(std::move(inner));
	CHECK_EQ(*std::move(nested).flatten().front(), 8);

	// Ordinary copy construction still uses select_on_container_copy_construction.
	const A copied = a;
	CHECK_EQ(copied.get_allocator().state, nullptr);
	A allocated(ArrayTestAllocator<int>{ &state });
	allocated.reserve(128);
	// Debug STL implementations may also allocate and free iterator bookkeeping in release().
	const auto outstanding = (state.allocations - state.deallocations);
	allocated.release();
	CHECK(allocated.empty());
	CHECK_EQ(allocated.capacity(), size_t{ 0 });
	CHECK_EQ((state.allocations - state.deallocations) + 1, outstanding);
	verify(allocated);

	using B = Array<bool, ArrayTestAllocator<bool>>;
	const B flags({ false, true, false }, ArrayTestAllocator<bool>{ &state });
	CHECK_EQ(flags.filter(Identity).get_allocator().state, &state);
	CHECK_EQ(flags.chunk(2)[0].get_allocator().state, &state);
	CHECK_EQ(flags.chunk(2).flatten(), Array<bool>{ false, true, false });
	CHECK_EQ(flags.rsorted().get_allocator().state, &state);
	CHECK_EQ(flags.without(false).get_allocator().state, &state);
	CHECK_EQ(flags.replaced(false, false).get_allocator().state, &state);
	CHECK_EQ(flags.replaced(false, true).get_allocator().state, &state);
	CHECK_EQ(flags.stable_uniqued().get_allocator().state, &state);
	CHECK_EQ(flags.sorted_and_uniqued().get_allocator().state, &state);
	const B allTrue(3, true, ArrayTestAllocator<bool>{ &state });
	const B allFalse(3, false, ArrayTestAllocator<bool>{ &state });
	const B emptyFlags(ArrayTestAllocator<bool>{ &state });
	for (const B* input : { &allTrue, &allFalse, &emptyFlags })
	{
		CHECK_EQ(input->stable_uniqued().get_allocator().state, &state);
		CHECK_EQ(input->sorted_and_uniqued().get_allocator().state, &state);
	}

}

TEST_CASE("Array.unique_contracts")
{
	CHECK_EQ(Array{ 3, 1, 3, 2, 1 }.stable_unique(), Array{ 3, 1, 2 });
	CHECK_EQ(Array{ 3, 1, 3, 2, 1 }.sort_and_unique(), Array{ 1, 2, 3 });
	CHECK_EQ(Array{ 1, 1, 2, 1, 1 }.uniqued_consecutive(), Array{ 1, 2, 1 });
	CHECK(Array<int>{}.stable_unique().empty());
	CHECK_EQ(Array{ 4 }.uniqued_consecutive(), Array{ 4 });
	CHECK_EQ(Array<bool>{ true, true, false, true }.uniqued_consecutive(), Array<bool>{ true, false, true });
	CHECK_EQ(Array<ArrayAdlHash>{ { 2 }, { 1 }, { 2 } }.stable_unique().map(&ArrayAdlHash::value), Array{ 2, 1 });
	static_assert(not ArrayCanStableUnique<Array<ArrayUnhashable>>);
	static_assert(not noexcept(std::declval<Array<ArrayThrowingEquality>&&>().uniqued_consecutive()));
	CHECK_THROWS_AS((void)(Array<ArrayThrowingEquality>{ { 1 }, { 1 } }.uniqued_consecutive()), std::runtime_error);
}

TEST_CASE("Array.random_selection")
{
	DefaultRNG rng{ uint64{ 12345 } };
	Array<int> a{ 1, 2, 3, 4, 5 };
	int& selected = a.choice(rng);
	CHECK((&selected >= a.data() && &selected < (a.data() + a.size())));
	CHECK_THROWS_AS((void)Array<int>{}.choice(rng), std::out_of_range);
	const auto& owned = Array{ 42 }.choice(rng);
	CHECK_EQ(owned, 42);
	CHECK_EQ(Array<bool>{ true }.choice(rng), true);
	CHECK_THROWS_AS((void)Array<bool>{}.choice(rng), std::out_of_range);
	CHECK(a.choice(0, rng).empty());
	CHECK_EQ(a.choice(20, rng), a);
	const auto sample = a.choice(3, rng);
	CHECK_EQ(sample.size(), size_t{ 3 });
	CHECK(sample.isSorted());
	CHECK_EQ(sample.stable_uniqued().size(), sample.size());
}

TEST_CASE("Array.parallel_map_contracts")
{
	const auto source = Array<int>::IndexedGenerate(97, [](size_t i) { return static_cast<int>(i); });
	CHECK_EQ(source.parallel_map([](int x) { return (x * 3); }), source.map([](int x) { return (x * 3); }));
	CHECK_EQ(source.parallel_map([](int x) { return (x % 2 == 0); }), source.map([](int x) { return (x % 2 == 0); }));
	const auto constructed = source.parallel_map([](int x) { return ArrayConstructOnly{ x }; });
	CHECK_EQ(constructed.map(&ArrayConstructOnly::value), source);
	CHECK(Array<int>{}.parallel_map([](int x) { return ArrayConstructOnly{ x }; }).empty());
	CHECK_EQ(Array{ 4 }.parallel_map([](int x) { return ArrayConstructOnly{ x }; }).front().value, 4);
	std::atomic<int> calls{ 0 };
	const auto result = source.parallel_map([counter = std::make_unique<int>(0), &calls](int x) { ++calls; return (x + *counter); });
	CHECK_EQ(result, source);
	CHECK_EQ(static_cast<size_t>(calls.load()), source.size());

	for (const int failAt : { 0, 96 })
	{
		std::atomic<int> active{ 0 };
		CHECK_THROWS_AS((void)source.parallel_map([&](int x)
		{
			struct Guard
			{
				std::atomic<int>& count;
				explicit Guard(std::atomic<int>& c) : count(c) { ++count; }
				~Guard() { --count; }
			} guard(active);
			if (x == failAt) { throw std::runtime_error("map"); }
			return ArrayConstructOnly{ x };
		}), std::runtime_error);
		CHECK_EQ(active.load(), 0);
	}
}

TEST_CASE("Array.sequence_operations")
{
	Array<int> a;
	a.reserve(32);
	const auto capacity = a.capacity();
	a.push_back(2);
	a.push_front(1);
	CHECK_EQ(a.emplace_back(4), 4);
	CHECK_EQ(a.emplace_front(0), 0);
	const auto inserted = a.insert(a.begin() + 3, 3);
	CHECK_EQ(*inserted, 3);
	CHECK_EQ(a, Array{ 0, 1, 2, 3, 4 });
	CHECK_EQ(a.at(4), 4);
	CHECK_THROWS_AS((void)a.at(5), std::out_of_range);
	CHECK_EQ(a.fetch(5, 99), 99);
	CHECK(a.indexInBounds(4));
	CHECK_FALSE(a.indexInBounds(5));
	CHECK_EQ(a.indexOf(3), Optional<size_t>{ 3 });
	CHECK_EQ(a.indexOf(9), none);
	CHECK_EQ(a.count_if(IsEven), 3);
	CHECK_EQ(a.fold_left(0, std::plus<>{}), 10);
	CHECK_EQ(a.sum(), 10);
	CHECK_EQ((Array<double>{ 0.5, 1.5, 2.0 }.sumF()), 4.0);

	CHECK_EQ(a.erase(a.begin() + 1), a.begin() + 1);
	CHECK_EQ(a, Array{ 0, 2, 3, 4 });
	a.erase_at(0);
	CHECK_EQ(a, Array{ 2, 3, 4 });
	CHECK_THROWS_AS((void)a.erase_at(a.size()), std::out_of_range);
	CHECK_EQ(a, Array{ 2, 3, 4 });
	a.pop_front();
	a.pop_back();
	CHECK_EQ(a, Array{ 3 });
	a.clear();
	CHECK(a.empty());
	CHECK_EQ(a.capacity(), capacity);
	a.append({ 1, 2, 3, 4 });
	a.pop_front_N(2);
	CHECK_EQ(a, Array{ 3, 4 });
	a.pop_back_N(100);
	CHECK(a.empty());
	CHECK_EQ(a.capacity(), capacity);
}

TEST_CASE("Array.unstable_erase_and_rearrangement")
{
	Array<int> a{ 0, 1, 2, 3, 4 };
	a.erase_at_unstable(1);
	CHECK_EQ(a, Array{ 0, 4, 2, 3 });
	a.erase_at_unstable(a.size() - 1);
	CHECK_EQ(a, Array{ 0, 4, 2 });
	CHECK_THROWS_AS((void)a.erase_at_unstable(3), std::out_of_range);
	CHECK(a.erase_first_unstable(4));
	CHECK_EQ(a, Array{ 0, 2 });
	CHECK_FALSE(a.erase_first_unstable(9));

	const Array<int> source{ 1, 2, 3, 4 };
	CHECK_EQ(source.rotated(2), Array{ 3, 4, 1, 2 });
	CHECK_EQ(Array{ source }.rotate(4), source);
	CHECK_EQ(Array{ source }.rotate(0), source);
	CHECK_THROWS_AS((void)source.rotated(5), std::out_of_range);
	CHECK_EQ(source.reversed().reversed(), source);
	CHECK_EQ(source.tail(2), Array{ 3, 4 });
	CHECK_EQ(source.tail(100), source);
	CHECK(source.tail(0).empty());
	CHECK_EQ(source.values_at({ 3, 0, 3 }), Array{ 4, 1, 4 });
	CHECK_THROWS_AS((void)source.values_at({ 4 }), std::out_of_range);
}

TEST_CASE("Array.range_input_and_empty_algorithms")
{
	const auto range = std::views::iota(0, 5);
	Array<int> a(std::from_range, range);
	CHECK_EQ(a, Array{ 0, 1, 2, 3, 4 });
	a.assign_range(std::views::iota(5, 8));
	CHECK_EQ(a, Array{ 5, 6, 7 });
	a.append_range(std::views::iota(8, 10));
	CHECK_EQ(a, Array{ 5, 6, 7, 8, 9 });
	CHECK_EQ(ToArray(range | std::views::filter(IsEven)), Array{ 0, 2, 4 });

	const Array<int> empty;
	CHECK(empty.all(IsEven));
	CHECK(empty.none(IsEven));
	CHECK_FALSE(empty.any(IsEven));
	CHECK_EQ(empty.count(0), 0);
	CHECK_EQ(empty.count_if(IsEven), 0);
	CHECK_EQ(empty.sum(), 0);
	CHECK_EQ(empty.fold_left(42, std::plus<>{}), 42);
	CHECK(empty.chunk(1).empty());
	CHECK(empty.in_groups(2).empty());
	CHECK(empty.slice(0, 0).empty());
	CHECK(empty.take_while(IsEven).empty());
	CHECK(empty.drop_while(IsEven).empty());
}

TEST_CASE("Array.const_filter_exception_preserves_input")
{
	const Array<int> source{ 1, 2, 3, 4 };
	CHECK_THROWS_AS((void)source.filter([](int x)
	{
		if (x == 3) { throw std::runtime_error("predicate"); }
		return IsOdd(x);
	}), std::runtime_error);
	CHECK_EQ(source, Array{ 1, 2, 3, 4 });

	ArrayAllocationState state;
	using A = Array<int, ArrayTestAllocator<int>>;
	{
		const A allocated({ 1, 2, 3, 4 }, ArrayTestAllocator<int>{ &state });
		CHECK_THROWS_AS((void)allocated.filter([](int x)
		{
			if (x == 3) { throw std::runtime_error("predicate"); }
			return true;
		}), std::runtime_error);
	}
	CHECK_EQ(state.allocations, state.deallocations);
}

TEST_CASE("Array.bool_storage_and_allocator")
{
	ArrayAllocationState state;
	using B = Array<bool, ArrayTestAllocator<bool>>;
	{
		B flags(256, true, ArrayTestAllocator<bool>{ &state });
		flags[1] = false;
		const auto* address = flags.data();
		auto taken = std::move(flags).take(128);
		CHECK_EQ(taken.data(), address);
		CHECK_EQ(taken.get_allocator().state, &state);
		auto filtered = std::move(taken).filter(Identity);
		CHECK_EQ(filtered.data(), address);
		CHECK_EQ(filtered.size(), size_t{ 127 });
		CHECK(filtered.all());
		const auto* allocator = filtered.get_allocator().state;
		// Count outstanding allocations, including any debug iterator bookkeeping.
		const auto outstanding = (state.allocations - state.deallocations);
		filtered.release();
		CHECK(filtered.empty());
		CHECK_EQ(filtered.get_allocator().state, allocator);
		CHECK_EQ((state.allocations - state.deallocations) + 1, outstanding);
	}
	CHECK_EQ(state.allocations, state.deallocations);
	Array<bool> flags{ true, false };
	flags.append(flags);
	CHECK_EQ(flags, Array<bool>{ true, false, true, false });
	flags.append(std::move(flags));
	CHECK_EQ(flags, Array<bool>{ true, false, true, false });
	CHECK_EQ((Array<bool>{ false } << true), Array<bool>{ false, true });
}

TEST_CASE("Array.constexpr_basic_operations")
{
	static_assert([]
	{
		Array<int> a{ 1, 2, 3 };
		return (*a.get_if(1) == 2) && (a.get_if(3) == nullptr)
			&& (a.find_if(IsEven) == &a[1]) && (a.indexOf_if(IsEven) == Optional<size_t>{ 1 });
	}());
	static_assert(Array{ 1, 2, 3 }.take(2) == Array{ 1, 2 });
	static_assert(Array{ 1, 2, 3 }.drop(2) == Array{ 3 });
	static_assert(Array{ 1, 2, 3 }.slice(1) == Array{ 2, 3 });
	static_assert(Array{ 1, 2, 3 }.filter(IsOdd) == Array{ 1, 3 });
}

namespace
{
	template <class T>
	void CheckArrayAppendOverloads()
	{
		using A = Array<T>;
		const T low = static_cast<T>(0);
		const T high = static_cast<T>(1);
		const A source{ low, high };
		CHECK_EQ(A{ low }.append(source), A{ low, low, high });
		CHECK_EQ(A{ low }.append(A{ low, high }), A{ low, low, high });
		CHECK_EQ(A{ low }.append(source.begin(), source.end()), A{ low, low, high });
		CHECK_EQ(A{ low }.append({ low, high }), A{ low, low, high });
		CHECK_EQ(A{ low }.append(2, high), A{ low, high, high });
		A result{ low };
		CHECK_EQ(&result.append(2, high), &result);
		CHECK_EQ(result, A{ low, high, high });
		result.append(std::move(result));
		CHECK_EQ(result, A{ low, high, high });
	}
}

TEST_CASE("Array.append_overloads")
{
	CheckArrayAppendOverloads<int>();
	CheckArrayAppendOverloads<bool>();
}
