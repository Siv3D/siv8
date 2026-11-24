//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------
# include "Siv3DTest.hpp"
# include <array>
# include <limits>
# include <ranges>
# include <sstream>
# include <stdexcept>
# include <type_traits>
# include <utility>

namespace
{
	constexpr std::array<Byte, 6> BlobBytes{ Byte{ 0 }, Byte{ 1 }, Byte{ 127 }, Byte{ 128 }, Byte{ 254 }, Byte{ 255 } };
	constexpr size_t MaxBlobSize = std::numeric_limits<size_t>::max();

	template <class T>
	concept HasTailSubspan = requires(T&& value) { std::forward<T>(value).subspan(0); };

	struct ReaderProbe
	{
		int moves = 0;
		int readCalls = 0;
		int64 requestedSize = 0;
		int64 finalPosition = 0;
		bool destroyed = false;
	};

	class ProbedBlobReader : public MemoryViewReader
	{
	public:

		using MemoryViewReader::read;

		int64 advertisedSize = 6;

		ProbedBlobReader(ReaderProbe& probe, const int64 readLimit, const bool throws = false)
			: MemoryViewReader{ BlobBytes.data(), BlobBytes.size() }
			, m_probe{ &probe }
			, m_readLimit{ readLimit }
			, m_throws{ throws } {}

		ProbedBlobReader(const ProbedBlobReader&) = delete;

		ProbedBlobReader(ProbedBlobReader&& other) noexcept
			: MemoryViewReader{ std::move(other) }
			, advertisedSize{ other.advertisedSize }
			, m_probe{ std::exchange(other.m_probe, nullptr) }
			, m_readLimit{ other.m_readLimit }
			, m_throws{ other.m_throws }
		{
			++m_probe->moves;
		}

		~ProbedBlobReader() override
		{
			if (m_probe)
			{
				m_probe->finalPosition = getPos();
				m_probe->destroyed = true;
			}
		}

		int64 size() const override
		{
			return advertisedSize;
		}

		int64 read(void* dst, const int64 size) override
		{
			++m_probe->readCalls;
			m_probe->requestedSize = size;
			if (m_throws)
			{
				throw std::runtime_error{ "Blob test reader failure" };
			}
			return MemoryViewReader::read(dst, Min(size, m_readLimit));
		}

	private:

		ReaderProbe* m_probe;
		int64 m_readLimit;
		bool m_throws;
	};

	class StackBlobReader : public ProbedBlobReader
	{
	public:

		using ProbedBlobReader::ProbedBlobReader;

		// A concrete Reader must not require an allocation for the Reader itself.
		static void* operator new(size_t) = delete;
	};
}

TEST_CASE("Blob.empty_and_sized_construction")
{
	for (const Blob& blob : { Blob{}, Blob{ size_t{ 0 } }, Blob{ nullptr, 0 }, Blob{ std::span<const Byte>{} } })
	{
		CHECK(blob.empty());
		CHECK(blob.isEmpty());
		CHECK_FALSE(static_cast<bool>(blob));
		CHECK(blob.size() == 0);
		CHECK(blob.ssize() == 0);
		CHECK(blob.size_bytes() == 0);
		CHECK(blob.begin() == blob.end());
		CHECK(blob.rbegin() == blob.rend());
		CHECK(blob.get_if(0) == nullptr);
		CHECK(blob.subspan(0, 0).empty());
	}

	const Blob sized{ size_t{ 16 } };
	CHECK(static_cast<bool>(sized));
	CHECK_FALSE(sized.isEmpty());
	CHECK(sized.size() == 16);
	CHECK(sized.ssize() == 16);
	CHECK(sized.size_bytes() == 16);
	CHECK(std::ranges::all_of(sized, [](const Byte b) { return (b == Byte{ 0 }); }));

	const Blob reserved{ Arg::reserve = size_t{ 32 } };
	CHECK(reserved.empty());
	CHECK(reserved.capacity() >= 32);
}

TEST_CASE("Blob.copy_construction_owns_bytes")
{
	Array<Byte> source(BlobBytes.begin(), BlobBytes.end());
	const Blob raw{ source.data(), source.size() };
	const Blob span{ std::span<const Byte>{ source.data(), source.size() } };
	const Blob array{ source };
	Blob copy{ array };
	CHECK(raw == span);
	CHECK(raw == array);
	CHECK(raw == copy);
	CHECK(raw.data() != source.data());
	CHECK(span.data() != source.data());
	CHECK(array.data() != source.data());
	CHECK(copy.data() != array.data());
	source[0] = Byte{ 42 };
	copy[1] = Byte{ 42 };
	CHECK(raw[0] == Byte{ 0 });
	CHECK(span[0] == Byte{ 0 });
	CHECK(array[0] == Byte{ 0 });
	CHECK(array[1] == Byte{ 1 });
	CHECK(copy != array);
	CHECK((Blob{ BlobBytes.data() + 2, 3 }.asArray() == Array<Byte>{ Byte{ 127 }, Byte{ 128 }, Byte{ 254 } }));
}

TEST_CASE("Blob.move_construction_and_array_extraction")
{
	Array<Byte> source(BlobBytes.begin(), BlobBytes.end());
	source.reserve(64);
	const auto storage = source.data();
	const auto capacity = source.capacity();
	Blob fromArray{ std::move(source) };
	CHECK(fromArray.data() == storage);
	CHECK(fromArray.capacity() == capacity);
	Blob moved{ std::move(fromArray) };
	CHECK(moved.data() == storage);
	CHECK(moved.capacity() == capacity);
	Array<Byte> extracted = std::move(moved).asArray();
	CHECK(extracted.data() == storage);
	CHECK(extracted.capacity() == capacity);
	CHECK(std::ranges::equal(extracted, BlobBytes));

	// Moved-from objects remain reusable without assuming their size or capacity.
	source.assign(BlobBytes.begin(), BlobBytes.end());
	fromArray.assign(source);
	moved.assign(fromArray);
	CHECK(std::ranges::equal(source, BlobBytes));
	CHECK(moved == fromArray);
}

TEST_CASE("Blob.copy_assignment_and_storage_reuse")
{
	const Array<Byte> array(BlobBytes.begin(), BlobBytes.end());
	const Blob source{ array };
	Blob dst{ Arg::reserve = size_t{ 64 } };
	const auto storage = dst.data();
	const auto capacity = dst.capacity();
	CHECK(&(dst = array) == &dst);
	CHECK(dst == source);
	dst[0] = Byte{ 42 };
	CHECK(array[0] == Byte{ 0 });
	CHECK(&(dst = source) == &dst);
	CHECK(dst == source);
	dst.assign(array);
	CHECK(dst == source);
	dst.assign(source);
	CHECK(dst == source);
	const Blob& self = dst;
	dst = self;
	dst.assign(self);
	CHECK(dst == source);
	dst.assign(BlobBytes.data() + 1, 3);
	CHECK((dst.asArray() == Array<Byte>{ Byte{ 1 }, Byte{ 127 }, Byte{ 128 } }));
	CHECK(dst.data() == storage);
	CHECK(dst.capacity() == capacity);
	dst.assign(nullptr, 0);
	CHECK(dst.empty());
	CHECK(dst.data() == storage);
	CHECK(dst.capacity() == capacity);
}

TEST_CASE("Blob.move_assignment_transfers_storage")
{
	static_assert(noexcept(std::declval<Blob&>().assign(std::declval<Blob&&>())));
	static_assert(noexcept(std::declval<Blob&>().assign(std::declval<Array<Byte>&&>())));

	for (const bool useAssign : { false, true })
	{
		CAPTURE(useAssign);
		Array<Byte> array(BlobBytes.begin(), BlobBytes.end());
		array.reserve(64);
		const auto storage = array.data();
		const auto capacity = array.capacity();
		Blob fromArray{ size_t{ 10 } };
		if (useAssign) { fromArray.assign(std::move(array)); }
		else { CHECK(&(fromArray = std::move(array)) == &fromArray); }
		CHECK(fromArray.data() == storage);
		CHECK(fromArray.capacity() == capacity);
		CHECK(std::ranges::equal(fromArray, BlobBytes));
		Blob fromBlob{ size_t{ 20 } };
		if (useAssign) { fromBlob.assign(std::move(fromArray)); }
		else { CHECK(&(fromBlob = std::move(fromArray)) == &fromBlob); }
		CHECK(fromBlob.data() == storage);
		CHECK(fromBlob.capacity() == capacity);
		CHECK(std::ranges::equal(fromBlob, BlobBytes));
		fromArray.assign(BlobBytes.data(), BlobBytes.size());
		CHECK(fromArray == fromBlob);
	}
}

TEST_CASE("Blob.assign_range")
{
	Blob blob{ size_t{ 20 } };
	blob.assign_range(BlobBytes);
	CHECK(std::ranges::equal(blob, BlobBytes));
	std::istringstream input{ "0 1 127 128 254 255" };
	auto bytes = (std::views::istream<unsigned int>(input)
		| std::views::transform([](const unsigned int value) { return static_cast<Byte>(value); }));
	static_assert(std::ranges::input_range<decltype(bytes)>);
	static_assert(not std::ranges::forward_range<decltype(bytes)>);
	blob.assign_range(bytes);
	CHECK(std::ranges::equal(blob, BlobBytes));
	blob.assign_range(std::span<const Byte>{});
	CHECK(blob.empty());
}

TEST_CASE("Blob.resize_clear_release_and_swap")
{
	Blob blob{ BlobBytes.data(), BlobBytes.size() };
	blob.reserve(64);
	const auto storage = blob.data();
	const auto capacity = blob.capacity();
	blob.reserve(1);
	blob.resize(8);
	CHECK(blob.data() == storage);
	CHECK(std::ranges::equal(blob.subspan(0, 6), BlobBytes));
	CHECK(blob[6] == Byte{ 0 });
	CHECK(blob[7] == Byte{ 0 });
	blob.resize(2);
	blob.resize(6);
	CHECK((blob.asArray() == Array<Byte>{ Byte{ 0 }, Byte{ 1 }, Byte{ 0 }, Byte{ 0 }, Byte{ 0 }, Byte{ 0 } }));
	blob.clear();
	CHECK(blob.empty());
	CHECK(blob.capacity() == capacity);
	blob.push_back(Byte{ 255 });
	CHECK(blob.data() == storage);
	blob.pop_back();
	CHECK(blob.empty());
	blob.assign(BlobBytes.data(), BlobBytes.size());

	Blob other{ Array<Byte>{ Byte{ 42 } } };
	const auto otherStorage = other.data();
	blob.swap(other);
	CHECK(blob.data() == otherStorage);
	CHECK(other.data() == storage);
	CHECK((blob.asArray() == Array<Byte>{ Byte{ 42 } }));
	CHECK(std::ranges::equal(other, BlobBytes));
	swap(blob, other);
	CHECK(blob.data() == storage);
	CHECK(other.data() == otherStorage);
	blob.shrink_to_fit();
	CHECK(std::ranges::equal(blob, BlobBytes));
	CHECK(blob.capacity() >= blob.size()); // A reduction in capacity is non-binding.
	blob.release();
	CHECK(blob.empty());
	CHECK(blob.capacity() == 0);
	blob.release();
	blob.push_back(Byte{ 42 });
	CHECK(blob[0] == Byte{ 42 });
}

TEST_CASE("Blob.iterators_and_subspan_alias_storage")
{
	static_assert(HasTailSubspan<Blob&>);
	static_assert(HasTailSubspan<const Blob&>);
	static_assert(not HasTailSubspan<Blob>);
	static_assert(not HasTailSubspan<const Blob>);
	static_assert(std::same_as<decltype(std::declval<Blob&>().subspan(0)), std::span<Byte>>);
	static_assert(std::same_as<decltype(std::declval<const Blob&>().subspan(0)), std::span<const Byte>>);
	static_assert(noexcept(std::declval<Blob&>().subspan(0)));
	static_assert(noexcept(std::declval<const Blob&>().subspan(0)));

	Blob blob{ BlobBytes.data(), BlobBytes.size() };
	const Blob& view = blob;
	CHECK(std::ranges::equal(view, BlobBytes));
	CHECK(std::equal(view.cbegin(), view.cend(), BlobBytes.begin()));
	CHECK(std::equal(view.rbegin(), view.rend(), BlobBytes.rbegin()));
	CHECK(std::equal(view.crbegin(), view.crend(), BlobBytes.rbegin()));
	*blob.begin() = Byte{ 10 };
	*blob.rbegin() = Byte{ 20 };
	auto middle = blob.subspan(1, 3);
	CHECK(middle.data() == (blob.data() + 1));
	middle[1] = Byte{ 30 };
	CHECK(view[0] == Byte{ 10 });
	CHECK(view[2] == Byte{ 30 });
	CHECK(view[5] == Byte{ 20 });
	CHECK(view.subspan(2, std::dynamic_extent).size() == 4);
	CHECK(view.subspan(6, 0).empty());
	CHECK(blob.subspan(6, std::dynamic_extent).empty());
	CHECK(view.get_if(MaxBlobSize) == nullptr);
	CHECK(blob.get_if(MaxBlobSize) == nullptr);
	CHECK(view.asArray().data() == blob.data());
	for (size_t pos = 0; pos <= blob.size(); ++pos)
	{
		CAPTURE(pos);
		CHECK(blob.subspan(pos).size() == (blob.size() - pos));
		CHECK(blob.subspan(pos).data() == (blob.data() + pos));
		CHECK(std::ranges::equal(view.subspan(pos), view.subspan(pos, view.size() - pos)));
	}
	blob.subspan(3)[0] = Byte{ 40 };
	CHECK(view[3] == Byte{ 40 });
	const Blob empty;
	CHECK(empty.subspan(0).empty());
	blob.clear();
	CHECK(blob.subspan(0).empty());
}

TEST_CASE("Blob.insert_and_external_append")
{
	Blob blob;
	auto inserted = blob.insert(blob.cbegin(), Byte{ 1 });
	REQUIRE(inserted == blob.begin());
	CHECK(*inserted == Byte{ 1 });
	const std::array<Byte, 2> extra{ Byte{ 2 }, Byte{ 3 } };
	inserted = blob.insert(blob.cend(), extra.begin(), extra.end());
	CHECK(inserted == (blob.begin() + 1));
	inserted = blob.insert(blob.cbegin() + 1, Byte{ 4 });
	CHECK(*inserted == Byte{ 4 });
	CHECK((blob.asArray() == Array<Byte>{ Byte{ 1 }, Byte{ 4 }, Byte{ 2 }, Byte{ 3 } }));
	CHECK(blob.insert(blob.cend(), extra.end(), extra.end()) == blob.end());
	blob.append(extra.data(), extra.size());
	const Blob tail{ Array<Byte>{ Byte{ 0 }, Byte{ 255 } } };
	blob.append(tail);
	blob.append(Blob{});
	blob.append(nullptr, 0);
	blob.append(blob.data() + blob.size(), 0);
	CHECK((blob.asArray() == Array<Byte>{ Byte{ 1 }, Byte{ 4 }, Byte{ 2 }, Byte{ 3 }, Byte{ 2 }, Byte{ 3 }, Byte{ 0 }, Byte{ 255 } }));
	CHECK((tail.asArray() == Array<Byte>{ Byte{ 0 }, Byte{ 255 } }));
}

TEST_CASE("Blob.array_like_contract")
{
	static_assert(std::same_as<decltype(Blob{}[0]), Byte>);
	static_assert(std::same_as<decltype(Blob{}.asArray()), Array<Byte>>);
	const Array<Byte> data{ Byte{ 1 }, Byte{ 2 }, Byte{ 3 } };
	Blob blob{ data };
	CHECK((blob.get_if(1)) == (&blob[1]));
	CHECK((std::as_const(blob).get_if(1)) == (&blob[1]));
	CHECK((blob.get_if(3)) == (nullptr));
	CHECK((blob.take(2).asArray()) == (data.take(2)));
	CHECK((blob.drop(1).asArray()) == (data.drop(1)));
	CHECK(blob.drop(100).isEmpty());
	CHECK((blob.slice(1).asArray()) == (data.drop(1)));
	CHECK((blob.slice(1, 1).asArray()) == (data.slice(1, 1)));
	CHECK(blob.slice(3).isEmpty());
	CHECK_THROWS_AS((void) blob.slice(4), std::out_of_range);
	CHECK_THROWS_AS((void) blob.slice(2, 2), std::out_of_range);
	blob.reserve(32);
	const auto storage = blob.data();
	auto result = std::move(blob).drop(1).take(1);
	CHECK((result.data()) == (storage));
	CHECK((result.asArray()) == (Array<Byte>{ Byte{ 2 } }));
	result.append(result);
	CHECK((result.asArray()) == (Array<Byte>{ Byte{ 2 }, Byte{ 2 } }));
	result.append(nullptr, 0);
	CHECK((result.size()) == (size_t{ 2 }));
	result.release();
	CHECK((result.capacity()) == (size_t{ 0 }));
	CHECK(result.isEmpty());
	CHECK((result.get_if(0)) == (nullptr));
}

TEST_CASE("Blob.overlapping_append")
{
	for (const bool spareCapacity : { false, true })
	{
		for (const auto [offset, count] : { std::pair<size_t, size_t>{ 0, 6 }, { 1, 3 }, { 5, 1 }, { 6, 0 } })
		{
			CAPTURE(spareCapacity, offset, count);
			Blob blob{ BlobBytes.data(), BlobBytes.size() };
			if (spareCapacity) { blob.reserve(32); }
			Array<Byte> expected(BlobBytes.begin(), BlobBytes.end());
			expected.insert(expected.end(), BlobBytes.begin() + offset, BlobBytes.begin() + offset + count);
			blob.append(blob.data() + offset, count);
			CHECK(blob.asArray() == expected);
		}
		Blob self{ BlobBytes.data(), BlobBytes.size() };
		if (spareCapacity) { self.reserve(32); }
		self.append(self);
		CHECK(self.size() == (BlobBytes.size() * 2));
		CHECK(std::ranges::equal(self.subspan(0, 6), BlobBytes));
		CHECK(std::ranges::equal(self.subspan(6, 6), BlobBytes));
	}
	Blob empty;
	empty.append(empty);
	empty.append(nullptr, 0);
	CHECK(empty.empty());
}

TEST_CASE("Blob.take_and_drop_boundaries")
{
	const Blob original{ BlobBytes.data(), BlobBytes.size() };
	for (const size_t count : { size_t{ 0 }, size_t{ 1 }, size_t{ 6 }, size_t{ 7 }, MaxBlobSize })
	{
		CAPTURE(count);
		const auto clamped = Min(count, BlobBytes.size());
		const Blob prefix{ BlobBytes.data(), clamped };
		const Blob suffix{ BlobBytes.data() + clamped, BlobBytes.size() - clamped };
		CHECK(original.take(count) == prefix);
		CHECK(original.drop(count) == suffix);
		for (const bool take : { false, true })
		{
			Blob source{ original };
			source.reserve(64);
			const auto storage = source.data();
			const auto capacity = source.capacity();
			const Blob result = (take ? std::move(source).take(count) : std::move(source).drop(count));
			CHECK(result == (take ? prefix : suffix));
			CHECK(result.data() == storage);
			CHECK(result.capacity() == capacity);
		}
		CHECK(Blob{}.take(count).empty());
		CHECK(Blob{}.drop(count).empty());
	}
	CHECK(std::ranges::equal(original, BlobBytes));
}

TEST_CASE("Blob.slice_boundaries")
{
	const Blob original{ BlobBytes.data(), BlobBytes.size() };
	for (size_t offset = 0; offset <= BlobBytes.size(); ++offset)
	{
		CAPTURE(offset);
		const Blob suffix{ BlobBytes.data() + offset, BlobBytes.size() - offset };
		CHECK(original.slice(offset) == suffix);
		CHECK(Blob{ original }.slice(offset) == suffix);
		for (size_t count = 0; count <= (BlobBytes.size() - offset); ++count)
		{
			CAPTURE(count);
			const Blob expected{ BlobBytes.data() + offset, count };
			CHECK(original.slice(offset, count) == expected);
			CHECK(Blob{ original }.slice(offset, count) == expected);
		}
	}
	for (const auto [offset, count] : { std::pair<size_t, size_t>{ 7, 0 }, { 6, 1 }, { 5, 2 }, { 1, MaxBlobSize }, { MaxBlobSize, 0 } })
	{
		CAPTURE(offset, count);
		CHECK_THROWS_AS((void) original.slice(offset, count), std::out_of_range);
		Blob source{ original };
		CHECK_THROWS_AS((void) std::move(source).slice(offset, count), std::out_of_range);
		CHECK(source == original);
	}
	CHECK_THROWS_AS((void) original.slice(MaxBlobSize), std::out_of_range);
	CHECK_THROWS_AS((void) Blob{ original }.slice(7), std::out_of_range);
	CHECK(Blob{}.slice(0).empty());
	CHECK(Blob{}.slice(0, 0).empty());
	CHECK(std::ranges::equal(original, BlobBytes));
}

TEST_CASE("Blob.reader_construction")
{
	const Blob expected{ BlobBytes.data(), BlobBytes.size() };
	CHECK(Blob{ std::unique_ptr<IReader>{} }.empty());
	CHECK(Blob{ MemoryViewReader{} }.empty());
	CHECK(Blob{ MemoryReader{} }.empty());
	CHECK((Blob{ MemoryViewReader{ BlobBytes.data(), BlobBytes.size() } } == expected));
	CHECK((Blob{ MemoryReader{ BlobBytes.data(), BlobBytes.size() } } == expected));
	CHECK((Blob{ std::make_unique<MemoryViewReader>(BlobBytes.data(), BlobBytes.size()) } == expected));

	ReaderProbe probe;
	const Blob owned{ std::make_unique<ProbedBlobReader>(probe, 6) };
	CHECK(owned == expected);
	CHECK(probe.destroyed);
	CHECK(probe.finalPosition == 6);
}

TEST_CASE("Blob.concrete_reader_move_and_lifetime")
{
	static_assert(not std::is_copy_constructible_v<StackBlobReader>);
	for (const int64 position : { int64{ 0 }, int64{ 1 }, int64{ 6 } })
	{
		for (const int64 limit : { int64{ 0 }, int64{ 2 }, int64{ 6 } })
		{
			CAPTURE(position, limit);
			ReaderProbe probe;
			StackBlobReader reader{ probe, limit };
			REQUIRE(reader.setPos(position) == position);
			const Blob blob{ std::move(reader) };
			CHECK(probe.moves == 1);
			CHECK(probe.destroyed); // The moved-to Reader is already gone while the source is alive.
			const bool success = ((position == 6) || (limit > 0));
			CHECK(probe.finalPosition == (success ? 6 : position));
			if (position == 6) { CHECK(probe.readCalls == 0); }
			if (success)
			{
				CHECK((blob == Blob{ BlobBytes.data() + position, BlobBytes.size() - position }));
			}
			else
			{
				CHECK(blob.empty());
			}
		}
	}

	ReaderProbe probe;
	StackBlobReader reader{ probe, 6, true };
	CHECK_THROWS_AS((void) Blob{ std::move(reader) }, std::runtime_error);
	CHECK(probe.moves == 1);
	CHECK(probe.destroyed);
	CHECK(probe.readCalls == 1);
	CHECK(probe.finalPosition == 0);
}

TEST_CASE("Blob.reader_short_read_and_exception")
{
	for (const int64 limit : { int64{ 0 }, int64{ 2 } })
	{
		CAPTURE(limit);
		ReaderProbe probe;
		const Blob blob{ std::make_unique<ProbedBlobReader>(probe, limit) };
		if (limit == 0)
		{
			CHECK(blob.empty());
			CHECK(probe.readCalls == 1);
			CHECK(probe.finalPosition == 0);
		}
		else
		{
			CHECK(std::ranges::equal(blob, BlobBytes));
			CHECK(probe.readCalls == 3);
			CHECK(probe.requestedSize == 2);
			CHECK(probe.finalPosition == 6);
		}
		CHECK(probe.destroyed);
	}
	ReaderProbe probe;
	CHECK_THROWS_AS((void) Blob{ std::make_unique<ProbedBlobReader>(probe, 6, true) }, std::runtime_error);
	CHECK(probe.destroyed);
	CHECK(probe.readCalls == 1);
}

TEST_CASE("Blob.reader_at_nonzero_position")
{
	for (const int64 position : { int64{ 1 }, int64{ 6 } })
	{
		CAPTURE(position);
		ReaderProbe probe;
		auto reader = std::make_unique<ProbedBlobReader>(probe, 6);
		REQUIRE(reader->setPos(position) == position);
		const Blob blob{ std::move(reader) };
		CHECK((blob == Blob{ BlobBytes.data() + position, BlobBytes.size() - position }));
		CHECK(probe.requestedSize == (6 - position));
		CHECK(probe.finalPosition == 6);
		CHECK(probe.destroyed);
	}
}

TEST_CASE("Blob.createFromReader_reuses_capacity_and_borrows_reader")
{
	Blob blob{ Arg::reserve = size_t{ 64 } };
	const auto storage = blob.data();
	const auto capacity = blob.capacity();
	ReaderProbe probe;
	ProbedBlobReader reader{ probe, 2 };
	REQUIRE(reader.setPos(1) == 1);
	REQUIRE(blob.createFromReader(reader));
	CHECK((blob == Blob{ BlobBytes.data() + 1, 5 }));
	CHECK(reader.getPos() == 6);
	CHECK(probe.readCalls == 3);
	CHECK(probe.moves == 0);
	CHECK_FALSE(probe.destroyed);
	CHECK(blob.data() == storage);
	CHECK(blob.capacity() == capacity);
	REQUIRE(blob.createFromReader(reader)); // EOF is a successful empty result.
	CHECK(blob.empty());
	CHECK(probe.readCalls == 3);
	MemoryViewReader empty{ BlobBytes.data(), 0 };
	REQUIRE(empty.isOpen());
	REQUIRE(blob.createFromReader(empty));
	CHECK(blob.empty());
}

TEST_CASE("Blob.createFromReader_failures_clear_and_recover")
{
	Blob blob{ Arg::reserve = size_t{ 64 } };
	const auto storage = blob.data();
	const auto capacity = blob.capacity();
	for (const int64 advertisedSize : { int64{ -1 }, int64{ 9 } })
	{
		CAPTURE(advertisedSize);
		ReaderProbe probe;
		ProbedBlobReader reader{ probe, 2 };
		reader.advertisedSize = advertisedSize;
		blob.assign(BlobBytes.data(), BlobBytes.size());
		CHECK_FALSE(blob.createFromReader(reader));
		CHECK(blob.empty());
		CHECK(blob.data() == storage);
		CHECK(blob.capacity() == capacity);
		CHECK(reader.getPos() == ((advertisedSize < 0) ? 0 : 6));
		CHECK(probe.readCalls == ((advertisedSize < 0) ? 0 : 4));
		CHECK_FALSE(probe.destroyed);
	}
	ReaderProbe probe;
	ProbedBlobReader reader{ probe, 6 };
	reader.advertisedSize = 2;
	REQUIRE(reader.setPos(3) == 3);
	CHECK_FALSE(blob.createFromReader(reader));
	CHECK(probe.readCalls == 0);
	CHECK(reader.getPos() == 3);
	MemoryViewReader closed;
	blob.assign(BlobBytes.data(), BlobBytes.size());
	CHECK_FALSE(blob.createFromReader(closed));
	CHECK(blob.empty());
	CHECK(blob.capacity() == capacity);
	reader.advertisedSize = 6;
	REQUIRE(reader.setPos(0) == 0);
	REQUIRE(blob.createFromReader(reader));
	CHECK(std::ranges::equal(blob, BlobBytes));
	CHECK(blob.data() == storage);
}

TEST_CASE("Blob.createFromReader_uses_initial_end_and_propagates_exceptions")
{
	class GrowingReader : public ProbedBlobReader
	{
	public:
		using ProbedBlobReader::ProbedBlobReader;
		int64 size() const override { return ((getPos() == 0) ? 4 : 6); }
	};
	ReaderProbe growth;
	GrowingReader reader{ growth, 2 };
	Blob blob;
	REQUIRE(blob.createFromReader(reader));
	CHECK((blob == Blob{ BlobBytes.data(), 4 }));
	CHECK(reader.getPos() == 4);
	CHECK(growth.readCalls == 2);
	ReaderProbe failure;
	ProbedBlobReader throwing{ failure, 6, true };
	CHECK_THROWS_AS(blob.createFromReader(throwing), std::runtime_error);
	CHECK_FALSE(failure.destroyed);
	MemoryViewReader recovery{ BlobBytes.data(), BlobBytes.size() };
	REQUIRE(blob.createFromReader(recovery));
	CHECK(std::ranges::equal(blob, BlobBytes));
}

TEST_CASE("Blob.file_round_trip_and_capacity_reuse")
{
	const auto path = Test::OutputPath(U"blob/round-trip.bin");
	Blob loaded{ Arg::reserve = size_t{ 131072 } };
	const auto storage = loaded.data();
	const auto capacity = loaded.capacity();
	for (const size_t size : { 0, 1, 255, 256, 4095, 4096, 16385, 65537 })
	{
		CAPTURE(size);
		Array<Byte> bytes(size);
		for (size_t i = 0; i < size; ++i) { bytes[i] = static_cast<Byte>(i % 256); }
		const Blob blob{ bytes };
		REQUIRE(blob.save(path));
		{
			// Read the saved bytes independently of Blob's loading path.
			BinaryFileReader reader{ path };
			REQUIRE(reader.isOpen());
			CHECK(reader.size() == static_cast<int64>(size));
			Array<Byte> actual(size);
			REQUIRE(reader.read(actual.data(), size) == static_cast<int64>(size));
			CHECK(actual == bytes);
		}
		REQUIRE(loaded.createFromFile(path));
		CHECK(loaded == blob);
		CHECK(loaded.data() == storage);
		CHECK(loaded.capacity() == capacity);
		CHECK(Blob{ path } == blob);
	}
	// Replacing a larger file must truncate it, including when the new Blob is empty.
	const Blob small{ BlobBytes.data(), BlobBytes.size() };
	REQUIRE(small.save(path));
	CHECK(FileSystem::FileSize(path) == BlobBytes.size());
	CHECK(Blob{ path } == small);
	REQUIRE(Blob{}.save(path));
	CHECK(FileSystem::FileSize(path) == 0);
	REQUIRE(loaded.createFromFile(path));
	CHECK(loaded.empty());
	CHECK(loaded.capacity() == capacity);
}

TEST_CASE("Blob.file_open_failures_clear_and_preserve_capacity")
{
	const auto missing = Test::OutputPath(U"blob/does-not-exist.bin");
	const auto directory = Test::OutputPath(U"blob/directory/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	REQUIRE_FALSE(FileSystem::Exists(missing));
	const Blob expected{ BlobBytes.data(), BlobBytes.size() };
	Blob blob{ expected };
	blob.reserve(64);
	const auto storage = blob.data();
	const auto capacity = blob.capacity();
	for (const FilePath& path : { missing, directory, FilePath{} })
	{
		CAPTURE(path);
		blob = expected;
		REQUIRE_FALSE(blob.createFromFile(path));
		CHECK(blob.empty());
		CHECK(blob.data() == storage);
		CHECK(blob.capacity() == capacity);
		CHECK(Blob{ path }.empty());
	}
	blob = expected;
	CHECK_FALSE(blob.save(directory));
	CHECK_FALSE(blob.save(U""));
	CHECK(blob == expected);
	const auto blockedParent = Test::OutputPath(U"blob/regular-file.bin");
	REQUIRE(blob.save(blockedParent));
	CHECK_FALSE(blob.save(blockedParent + U"/child.bin"));
	CHECK(Blob{ blockedParent } == expected);
}

TEST_CASE("Blob.resource_file")
{
	const auto path = Resource(U"engine/texture/box-shadow/64.png");
	const Blob blob{ path };
	REQUIRE(blob.size() > 8);
	const std::array<Byte, 8> pngSignature{ Byte{ 137 }, Byte{ 80 }, Byte{ 78 }, Byte{ 71 }, Byte{ 13 }, Byte{ 10 }, Byte{ 26 }, Byte{ 10 } };
	CHECK(std::ranges::equal(blob.subspan(0, 8), pngSignature));
	Blob loaded;
	REQUIRE(loaded.createFromFile(path));
	CHECK(loaded == blob);
	CHECK(Blob{ BinaryFileReader{ path } } == blob);
}

TEST_CASE("Blob.md5_and_base64")
{
	for (const auto& [input, digest] : {
		std::pair<std::string_view, std::string_view>{ "", "d41d8cd98f00b204e9800998ecf8427e" },
		{ "a", "0cc175b9c0f1b6a831c399e269772661" },
		{ "abc", "900150983cd24fb0d6963f7d28e17f72" },
		{ "message digest", "f96b697d7cb7938d525a2f31aaf161d0" } })
	{
		CAPTURE(input);
		const auto expected = MD5Value::Parse(digest);
		REQUIRE(expected);
		CHECK((Blob{ input.data(), input.size() }.md5() == *expected));
	}
	const Blob binary{ BlobBytes.data(), BlobBytes.size() };
	CHECK(binary.base64().getBase64() == "AAF/gP7/");
	Base64Value encoded{ "old contents" };
	binary.base64(encoded);
	CHECK(encoded.getBase64() == "AAF/gP7/");
	CHECK(encoded.decodeToBlob() == binary);
	Blob{}.base64(encoded);
	CHECK(encoded.isEmpty());
	CHECK(Blob{}.base64().isEmpty());
	CHECK(std::ranges::equal(binary, BlobBytes));
}
