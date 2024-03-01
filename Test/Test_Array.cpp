//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
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
		const Array<int32> v(3, Arg::indexedGenerator = [](size_t i) { return static_cast<int32>(i + 1); });
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
