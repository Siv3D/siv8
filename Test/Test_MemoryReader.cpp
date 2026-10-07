//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"
# include <array>
# include <limits>
# include <type_traits>

namespace
{
	constexpr std::array<uint8, 6> MemoryReaderBytes{ 0, 1, 127, 128, 254, 255 };
	constexpr int64 MemoryReaderSize = 6;
	constexpr int64 MinPosition = std::numeric_limits<int64>::min();
	constexpr int64 MaxPosition = std::numeric_limits<int64>::max();
}

TEMPLATE_TEST_CASE("MemoryReader.empty", "", MemoryReader, MemoryViewReader)
{
	TestType reader;
	CHECK_FALSE(reader.isOpen());
	CHECK_FALSE(static_cast<bool>(reader));
	CHECK(reader.supportsLookahead());
	CHECK(reader.size() == 0);
	CHECK(reader.getPos() == 0);
	CHECK(reader.setPos(MaxPosition) == 0);
	CHECK(reader.setPos(MinPosition) == 0);
	CHECK(reader.skip(MaxPosition) == 0);
	CHECK(reader.skip(MinPosition) == 0);
	uint8 value = 42;
	CHECK(reader.read(&value, 1) == 0);
	CHECK(reader.read(&value, MaxPosition, 1) == 0);
	CHECK(reader.lookahead(&value, 1) == 0);
	CHECK(reader.lookahead(&value, MaxPosition, 1) == 0);
	CHECK_FALSE(reader.read(value));
	CHECK_FALSE(reader.lookahead(value));
	CHECK(value == 42);
	CHECK(reader.getPos() == 0);
	CHECK_THROWS_AS(reader.read(nullptr, 1), Error);
	CHECK_THROWS_AS(reader.lookahead(nullptr, 1), Error);

	const TestType nullEmpty{ nullptr, 0 };
	CHECK_FALSE(nullEmpty.isOpen());
	CHECK(nullEmpty.size() == 0);
	const TestType nonNullEmpty{ MemoryReaderBytes.data(), 0 };
	CHECK(nonNullEmpty.isOpen() == (std::is_same_v<TestType, MemoryViewReader>));
	CHECK(static_cast<bool>(nonNullEmpty) == nonNullEmpty.isOpen());
	CHECK(nonNullEmpty.size() == 0);
}

TEMPLATE_TEST_CASE("MemoryReader.sequential_read_and_lookahead", "", MemoryReader, MemoryViewReader)
{
	TestType concrete{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	IReader& reader = concrete;
	CHECK(reader.isOpen());
	CHECK(static_cast<bool>(reader));
	CHECK(reader.supportsLookahead());
	CHECK(reader.size() == MemoryReaderSize);
	CHECK(reader.getPos() == 0);
	for (int64 pos = 0; pos <= MemoryReaderSize; ++pos)
	{
		for (const int64 count : { MinPosition, int64{ -1 }, int64{ 0 }, int64{ 1 }, int64{ 4 }, MaxPosition })
		{
			CAPTURE(pos, count);
			const int64 expectedCount = ((count <= 0) ? 0 : Min(count, (MemoryReaderSize - pos)));
			std::array<uint8, 8> expected;
			expected.fill(42);
			for (int64 i = 0; i < expectedCount; ++i)
			{
				expected[static_cast<size_t>(i + 1)] = MemoryReaderBytes[static_cast<size_t>(pos + i)];
			}
			std::array<uint8, 8> actual;
			actual.fill(42);
			REQUIRE(reader.setPos(pos) == pos);
			CHECK(std::as_const(reader).lookahead(actual.data() + 1, count) == expectedCount);
			CHECK(actual == expected);
			CHECK(reader.getPos() == pos);
			actual.fill(42);
			CHECK(reader.read(actual.data() + 1, count) == expectedCount);
			CHECK(actual == expected);
			CHECK(reader.getPos() == (pos + expectedCount));
		}
	}
	CHECK(reader.isOpen()); // Reaching EOF does not close the reader.
}

TEMPLATE_TEST_CASE("MemoryReader.positioned_read_and_lookahead", "", MemoryReader, MemoryViewReader)
{
	TestType reader{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	for (const int64 pos : { int64{ 0 }, int64{ 3 }, MemoryReaderSize, (MemoryReaderSize + 1), MaxPosition })
	{
		for (const int64 count : { int64{ -1 }, int64{ 0 }, int64{ 1 }, int64{ 4 }, MaxPosition })
		{
			CAPTURE(pos, count);
			const int64 expectedCount = (((count <= 0) || (pos >= MemoryReaderSize)) ? 0 : Min(count, (MemoryReaderSize - pos)));
			std::array<uint8, 8> expected;
			expected.fill(42);
			for (int64 i = 0; i < expectedCount; ++i)
			{
				expected[static_cast<size_t>(i + 1)] = MemoryReaderBytes[static_cast<size_t>(pos + i)];
			}
			std::array<uint8, 8> actual;
			actual.fill(42);
			REQUIRE(reader.setPos(2) == 2);
			CHECK(std::as_const(reader).lookahead(actual.data() + 1, pos, count) == expectedCount);
			CHECK(actual == expected);
			CHECK(reader.getPos() == 2);
			actual.fill(42);
			CHECK(reader.read(actual.data() + 1, pos, count) == expectedCount);
			CHECK(actual == expected);
			CHECK(reader.getPos() == ((expectedCount == 0) ? 2 : (pos + expectedCount)));
		}
	}
}

TEMPLATE_TEST_CASE("MemoryReader.argument_errors_and_no_ops", "", MemoryReader, MemoryViewReader)
{
	TestType reader{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	for (const int64 current : { int64{ 2 }, MemoryReaderSize })
	{
		CAPTURE(current);
		REQUIRE(reader.setPos(current) == current);
		uint8 value = 42;
		for (const int64 count : { MinPosition, int64{ -1 }, int64{ 0 } })
		{
			CHECK(reader.read(nullptr, count) == 0);
			CHECK(reader.lookahead(nullptr, count) == 0);
			CHECK(reader.read(nullptr, MinPosition, count) == 0);
			CHECK(reader.lookahead(nullptr, MinPosition, count) == 0);
			CHECK(reader.read(&value, MinPosition, count) == 0);
			CHECK(reader.lookahead(&value, MinPosition, count) == 0);
			CHECK(value == 42);
			CHECK(reader.getPos() == current);
		}
		CHECK_THROWS_AS(reader.read(nullptr, 1), Error);
		CHECK_THROWS_AS(reader.lookahead(nullptr, 1), Error);
		for (const int64 pos : { MinPosition, int64{ -1 }, int64{ 0 }, MaxPosition })
		{
			CHECK_THROWS_AS(reader.read(nullptr, pos, 1), Error);
			CHECK_THROWS_AS(reader.lookahead(nullptr, pos, 1), Error);
		}
		for (const int64 pos : { MinPosition, int64{ -1 } })
		{
			CHECK_THROWS_AS(reader.read(&value, pos, 1), Error);
			CHECK_THROWS_AS(reader.lookahead(&value, pos, 1), Error);
		}
		CHECK(value == 42);
		CHECK(reader.getPos() == current);
	}
}

TEMPLATE_TEST_CASE("MemoryReader.seek_and_skip", "", MemoryReader, MemoryViewReader)
{
	TestType reader{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	CHECK(reader.setPos(-1) == 0);
	CHECK(reader.setPos(MinPosition) == 0);
	CHECK(reader.setPos(MaxPosition) == MemoryReaderSize);
	CHECK(reader.setPos(MemoryReaderSize + 1) == MemoryReaderSize);
	CHECK(reader.setPos(3) == 3);
	CHECK(reader.skip(0) == 3);
	CHECK(reader.skip(-2) == 1);
	CHECK(reader.skip(3) == 4);
	CHECK(reader.skip(3) == MemoryReaderSize);
	CHECK(reader.skip(-10) == 0);
	for (const int64 pos : { int64{ 0 }, int64{ 1 }, MemoryReaderSize })
	{
		CAPTURE(pos);
		REQUIRE(reader.setPos(pos) == pos);
		CHECK(reader.skip(MaxPosition) == MemoryReaderSize);
		CHECK(reader.getPos() == MemoryReaderSize);
		REQUIRE(reader.setPos(pos) == pos);
		CHECK(reader.skip(MinPosition) == 0);
		CHECK(reader.getPos() == 0);
	}
}

TEMPLATE_TEST_CASE("MemoryReader.typed_reads", "", MemoryReader, MemoryViewReader)
{
	const uint32 source = 0xFEDCBA98u;
	TestType reader{ &source, sizeof(source) };
	uint32 value = 0;
	CHECK(std::as_const(reader).lookahead(value));
	CHECK(value == source);
	CHECK(reader.getPos() == 0);
	value = 0;
	CHECK(reader.read(value));
	CHECK(value == source);
	CHECK(reader.getPos() == static_cast<int64>(sizeof(source)));
	CHECK_FALSE(reader.read(value));
	CHECK_FALSE(reader.lookahead(value));
	CHECK(value == source);

	// Both the concrete and interface templates retain bytes from a short read.
	TestType shortReader{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	auto checkPartial = [&](auto& input)
	{
		REQUIRE(input.setPos(4) == 4);
		std::array<uint8, 4> bytes{ 42, 42, 42, 42 };
		const std::array<uint8, 4> expected{ 254, 255, 42, 42 };
		CHECK_FALSE(std::as_const(input).lookahead(bytes));
		CHECK(bytes == expected);
		CHECK(input.getPos() == 4);
		bytes.fill(42);
		CHECK_FALSE(input.read(bytes));
		CHECK(bytes == expected);
		CHECK(input.getPos() == MemoryReaderSize);
	};
	checkPartial(shortReader);
	checkPartial(static_cast<IReader&>(shortReader));
}

TEMPLATE_TEST_CASE("MemoryReader.copy_preserves_data_and_independent_positions", "", MemoryReader, MemoryViewReader)
{
	TestType original{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	REQUIRE(original.setPos(2) == 2);
	TestType copy{ original };
	TestType assigned;
	CHECK(&(assigned = original) == &assigned);
	for (auto* reader : { &copy, &assigned })
	{
		CHECK(reader->size() == MemoryReaderSize);
		CHECK(reader->getPos() == 2);
		uint8 value = 0;
		CHECK(reader->read(value));
		CHECK(value == 127);
		CHECK(reader->getPos() == 3);
		CHECK(original.getPos() == 2);
	}
	original = TestType{};
	uint8 value = 0;
	CHECK(copy.read(value));
	CHECK(value == 128);
	CHECK(assigned.getPos() == 3);
	const auto& alias = copy;
	copy = alias;
	CHECK(copy.getPos() == 4);
	CHECK(copy.read(value));
	CHECK(value == 254);
}

TEST_CASE("MemoryViewReader.borrows_data_and_copies_views")
{
	auto bytes = MemoryReaderBytes;
	MemoryViewReader reader{ bytes.data(), bytes.size() };
	REQUIRE(reader.setPos(1) == 1);
	MemoryViewReader copy{ reader };
	MemoryViewReader moved{ std::move(reader) };
	MemoryViewReader assigned;
	assigned = std::move(copy);
	bytes[1] = 99;
	for (auto* view : { &reader, &copy, &moved, &assigned })
	{
		CHECK(view->isOpen());
		CHECK(view->getPos() == 1);
		uint8 value = 0;
		CHECK(view->read(value));
		CHECK(value == 99);
		CHECK(view->getPos() == 2);
	}
}

TEST_CASE("MemoryReader.owns_copied_or_moved_data")
{
	MemoryReader fromPointer;
	MemoryReader fromBlob;
	MemoryReader fromMovedBlob;
	{
		auto bytes = MemoryReaderBytes;
		Blob blob{ bytes.data(), bytes.size() };
		fromPointer = MemoryReader{ bytes.data(), bytes.size() };
		fromBlob = MemoryReader{ blob };
		fromMovedBlob = MemoryReader{ std::move(blob) };
		bytes.fill(42);
		blob = Blob{ 12 };
	}
	for (auto* reader : { &fromPointer, &fromBlob, &fromMovedBlob })
	{
		std::array<uint8, 6> actual{};
		CHECK(reader->isOpen());
		CHECK(reader->getPos() == 0);
		CHECK(reader->read(actual));
		CHECK(actual == MemoryReaderBytes);
	}
	const Blob empty;
	CHECK_FALSE(MemoryReader{ empty }.isOpen());
	CHECK_FALSE(MemoryReader{ Blob{} }.isOpen());
	CHECK_FALSE((std::is_nothrow_constructible_v<MemoryReader, const Blob&>));
	STATIC_REQUIRE(std::is_nothrow_constructible_v<MemoryReader, Blob&&>);
}

TEST_CASE("MemoryReader.move_preserves_position_and_resets_source")
{
	STATIC_REQUIRE(std::is_nothrow_move_constructible_v<MemoryReader>);
	STATIC_REQUIRE(std::is_nothrow_move_assignable_v<MemoryReader>);
	for (const int64 pos : { int64{ 0 }, int64{ 2 }, MemoryReaderSize })
	{
		for (const bool assignment : { false, true })
		{
			CAPTURE(pos, assignment);
			MemoryReader source{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
			REQUIRE(source.setPos(pos) == pos);
			MemoryReader destination = [&]()
			{
				if (assignment)
				{
					MemoryReader target{ MemoryReaderBytes.data(), 2 };
					target.setPos(1);
					CHECK(&(target = std::move(source)) == &target);
					return target;
				}
				return MemoryReader{ std::move(source) };
			}();
			CHECK(destination.size() == MemoryReaderSize);
			CHECK(destination.getPos() == pos);
			uint8 value = 42;
			CHECK(destination.read(value) == (pos < MemoryReaderSize));
			CHECK(value == ((pos < MemoryReaderSize) ? MemoryReaderBytes[static_cast<size_t>(pos)] : 42));
			CHECK_FALSE(source.isOpen());
			CHECK(source.size() == 0);
			REQUIRE(source.getPos() == 0); // Stop before read if the cursor invariant is broken.
			value = 42;
			CHECK_FALSE(source.read(value));
			CHECK_FALSE(source.lookahead(value));
			CHECK(value == 42);
			source = MemoryReader{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
			CHECK(source.getPos() == 0);
			CHECK(source.read(value));
			CHECK(value == 0);
		}
	}

	MemoryReader reader{ MemoryReaderBytes.data(), MemoryReaderBytes.size() };
	REQUIRE(reader.setPos(2) == 2);
	auto& alias = reader;
	reader = std::move(alias);
	REQUIRE(reader.size() == MemoryReaderSize);
	CHECK(reader.getPos() == 2);
	uint8 value = 0;
	CHECK(reader.read(value));
	CHECK(value == 127);

	MemoryReader empty;
	MemoryReader movedEmpty{ std::move(empty) };
	CHECK_FALSE(movedEmpty.isOpen());
	CHECK(movedEmpty.getPos() == 0);
	reader = std::move(movedEmpty);
	CHECK_FALSE(reader.isOpen());
	CHECK(reader.size() == 0);
	CHECK(reader.getPos() == 0);
	CHECK(movedEmpty.getPos() == 0);
}
