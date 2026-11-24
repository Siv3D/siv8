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
# include <limits>
# include <new>

TEST_CASE("FmtExtension.nullptr")
{
	CHECK(fmt::to_string(nullptr) == "0x0");
	CHECK(fmt::format("{}", nullptr) == "0x0");
	CHECK(U"{}"_fmt(nullptr) == U"0x0");
}

TEST_CASE("FmtExtension.utf32_buffer_growth")
{
	fmt::basic_memory_buffer<char32, 4> buffer;
	fmt::format_to(std::back_inserter(buffer), U"{}", U"日本語");
	const size_t capacity = buffer.capacity();
	// The byte count would wrap to a small nonzero value on either 32- or 64-bit hosts.
	const size_t overflowingSize = (std::numeric_limits<size_t>::max() / sizeof(char32) + 2);
	CHECK_THROWS_AS(buffer.reserve(overflowingSize), std::bad_alloc);
	CHECK(buffer.capacity() == capacity);
	CHECK(std::u32string(buffer.data(), buffer.size()) == U"日本語");
	// A rejected allocation must leave the buffer usable, including normal growth.
	fmt::format_to(std::back_inserter(buffer), U"{}", U"とSiv3D");
	CHECK(std::u32string(buffer.data(), buffer.size()) == U"日本語とSiv3D");
}

TEST_CASE("FmtExtension.fmt::format")
{
	CHECK((fmt::format("{}", String{ U"あいう" })) == ("あいう"));
	CHECK((fmt::format("{:>12}", String{ U"あいう" })) == ("      あいう"));

	//CHECK((fmt::format("{}", StringView{ U"あいう" })) == ("あいう"));
	//CHECK((fmt::format("{:>12}", StringView{ U"abc" })) == ("         abc"));
}

TEST_CASE("FmtExtension._fmt")
{
	{
		CHECK((U"{}"_fmt(U"あいう")) == (U"あいう"));
		CHECK((U"{}"_fmt(StringView{ U"あいう" })) == (U"あいう"));
		CHECK((U"{}"_fmt(String{ U"あいう" })) == (U"あいう"));

		CHECK((U"{:>12}"_fmt(U"abc")) == (U"         abc"));
		CHECK((U"{:>12}"_fmt(StringView{ U"abc" })) == (U"         abc"));
		CHECK((U"{:>12}"_fmt(String{ U"abc" })) == (U"         abc"));

		CHECK((U"{:>12}"_fmt(U"あいう")) == (U"         あいう"));
		CHECK((U"{:>12}"_fmt(StringView{ U"あいう" })) == (U"         あいう"));
		CHECK((U"{:>12}"_fmt(String{ U"あいう" })) == (U"         あいう"));
	}

	{
		std::array<int32, 3> vi{ 1, 2, 3 };
		const std::array<double, 3> vd{ 1.111, 2.222, 3.333 };
		const std::array<String, 4> vs{ U"aaa", U"bbb", U"あああ", U"いいい" };

		CHECK((U"{}"_fmt(vi)) == (U"[1, 2, 3]"));
		CHECK((U"{}"_fmt(vd)) == (U"[1.111, 2.222, 3.333]"));
		CHECK((U"{}"_fmt(vs)) == (U"[aaa, bbb, あああ, いいい]"));
		CHECK((U"{::.1f}"_fmt(vd)) == (U"[1.1, 2.2, 3.3]"));
	}

	{
		const Array<int32> vi{ 1, 2, 3 };
		const Array<double> vd{ 1.111, 2.222, 3.333 };
		const Array<String> vs{ U"aaa", U"bbb", U"あああ", U"いいい" };

		CHECK((U"{}"_fmt(vi)) == (U"[1, 2, 3]"));
		CHECK((U"{}"_fmt(vd)) == (U"[1.111, 2.222, 3.333]"));
		CHECK((U"{}"_fmt(vs)) == (U"[aaa, bbb, あああ, いいい]"));
		CHECK((U"{::.1f}"_fmt(vd)) == (U"[1.1, 2.2, 3.3]"));
	}

	{
		const Array<int32> vi{ 1, 2, 3 };
		const Array<double> vd{ 1.111, 2.222, 3.333 };
		const Array<String> vs{ U"aaa", U"bbb", U"あああ", U"いいい" };

		CHECK((U"{}"_fmt(vi.head_span(3))) == (U"[1, 2, 3]"));
		CHECK((U"{}"_fmt(vd.head_span(3))) == (U"[1.111, 2.222, 3.333]"));
		CHECK((U"{}"_fmt(vs.head_span(4))) == (U"[aaa, bbb, あああ, いいい]"));
		CHECK((U"{::.1f}"_fmt(vd.head_span(3))) == (U"[1.1, 2.2, 3.3]"));
	}

	{
		const Array<int32> vi{ 1, 2, 3 };
		const Array<double> vd{ 1.111, 2.222, 3.333 };
		const Array<String> vs{ U"aaa", U"bbb", U"あああ", U"いいい" };

		CHECK((U"{}"_fmt(vi.head_view(3))) == (U"[1, 2, 3]"));
		CHECK((U"{}"_fmt(vd.head_view(3))) == (U"[1.111, 2.222, 3.333]"));
		CHECK((U"{}"_fmt(vs.head_view(4))) == (U"[aaa, bbb, あああ, いいい]"));
		CHECK((U"{::.1f}"_fmt(vd.head_view(3))) == (U"[1.1, 2.2, 3.3]"));
	}

	{
		CHECK((U"{}"_fmt(none)) == (U"none"));
	}
}
