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
# include <cstring>

# if SIV3D_PLATFORM(WINDOWS)
# include <Siv3D/Windows/Windows.hpp>
# include <winioctl.h>
# endif

TEST_CASE("BinaryFileReader")
{
	constexpr int64 FileSize = (32768 * sizeof(uint32));
	const FilePath path{ Test::OutputPath(U"binaryreader/test.bin") };
	{
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(writer.isOpen());

			for (uint32 i = 0; i < (FileSize / sizeof(uint32)); ++i)
			{
				writer.write(i);
			}
		}
	}

	{
		BinaryFileReader reader{ path };
		CHECK(reader.isOpen());
		CHECK(reader.size() == FileSize);

		CHECK(reader.getPos() == 0);
		CHECK(reader.setPos(0) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.setPos(FileSize) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.setPos(FileSize / 2) == (FileSize / 2));
		CHECK(reader.getPos() == (FileSize / 2));

		CHECK(reader.setPos(FileSize + 1) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.setPos(-1) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.skip(0) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.skip(1) == 1);
		CHECK(reader.getPos() == 1);

		CHECK(reader.skip(-1) == 0);
		CHECK(reader.getPos() == 0);

		CHECK(reader.skip(FileSize) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.skip(-1) == (FileSize - 1));
		CHECK(reader.getPos() == (FileSize - 1));

		CHECK(reader.skip(2) == FileSize);
		CHECK(reader.getPos() == FileSize);

		CHECK(reader.skip(-(FileSize + 1)) == 0);
		CHECK(reader.getPos() == 0);

		uint32 u32 = 123456;
		CHECK(reader.read(u32));
		CHECK(u32 == 0);
		CHECK(reader.getPos() == 4);

		CHECK(reader.read(u32));
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 8);

		CHECK(reader.read(&u32, 4) == 4);
		CHECK(u32 == 2);
		CHECK(reader.getPos() == 12);

		CHECK(reader.read(&u32, 4, 4) == 4);
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 8);

		CHECK(reader.read(&u32, (FileSize - 8), 4) == 4);
		CHECK(u32 == 32766);

		CHECK(reader.read(&u32, 4) == 4);
		CHECK(u32 == 32767);

		CHECK(reader.read(&u32, 4) == 0);
		CHECK(u32 == 32767);

		CHECK(not reader.read(u32));
		CHECK(u32 == 32767);

		CHECK(reader.read(&u32, 0, 4) == 4);
		CHECK(u32 == 0);

		CHECK(reader.lookahead(u32));
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 4);

		CHECK(reader.lookahead(u32));
		CHECK(u32 == 1);
		CHECK(reader.getPos() == 4);

		CHECK(reader.skip(4) == 8);
		CHECK(reader.getPos() == 8);

		CHECK(reader.lookahead(u32));
		CHECK(u32 == 2);
		CHECK(reader.getPos() == 8);

		CHECK(reader.setPos(FileSize - 4) == (FileSize - 4));
		CHECK(reader.getPos() == (FileSize - 4));

		uint32 u32s[4]{};
		CHECK(not reader.lookahead(u32s));
		CHECK(u32s[0] == 32767);

		CHECK(reader.lookahead(u32s, (FileSize - 16), 16) == 16);
		CHECK(u32s[0] == 32764);
		CHECK(u32s[1] == 32765);
		CHECK(u32s[2] == 32766);
		CHECK(u32s[3] == 32767);

		std::ranges::fill(u32s, 0);
		CHECK(reader.setPos(FileSize - 16) == (FileSize - 16));
		CHECK(reader.read(u32s));
		CHECK(u32s[0] == 32764);
		CHECK(u32s[1] == 32765);
		CHECK(u32s[2] == 32766);
		CHECK(u32s[3] == 32767);
		CHECK(reader.getPos() == FileSize);
	}
}

namespace
{
	Array<uint8> ReaderBytes(const size_t size, uint32 seed = 123456789)
	{
		Array<uint8> bytes(size);
		for (auto& byte : bytes)
		{
			seed ^= (seed << 13);
			seed ^= (seed >> 17);
			seed ^= (seed << 5);
			byte = static_cast<uint8>(seed);
		}
		return bytes;
	}

	void WriteReaderBytes(const FilePathView path, const Array<uint8>& bytes)
	{
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write(bytes.data(), bytes.size()) == static_cast<int64>(bytes.size()));
	}
}

TEST_CASE("BinaryFileReader boundaries and lookahead")
{
	const auto bytes = ReaderBytes(65536 + 37);
	const auto path = Test::OutputPath(U"binaryreader/境界.bin");
	WriteReaderBytes(path, bytes);
	BinaryFileReader reader{ path };
	REQUIRE(reader.isOpen());
	Array<uint8> actual(bytes.size() + 16, 0);

	for (const int64 pos : { 0, 1, 4095, 4096, 16383, 16384, 16385, 32767, 65535 })
	{
		for (const int64 length : { 1, 4, 17, 16383, 16384, 16385, 65536 })
		{
			CAPTURE(pos);
			CAPTURE(length);
			REQUIRE(reader.setPos(pos) == pos);
			const int64 count = Min<int64>(length, (bytes.size() - pos));
			REQUIRE(reader.lookahead(actual.data(), length) == count);
			CHECK(reader.getPos() == pos);
			CHECK(std::memcmp(actual.data(), (bytes.data() + pos), static_cast<size_t>(count)) == 0);
			REQUIRE(reader.read(actual.data(), length) == count);
			CHECK(reader.getPos() == (pos + count));
			CHECK(std::memcmp(actual.data(), (bytes.data() + pos), static_cast<size_t>(count)) == 0);
		}
	}

	REQUIRE(reader.setPos(0) == 0);
	REQUIRE(reader.read(actual.data(), 4) == 4);
	REQUIRE(reader.lookahead(actual.data(), 50000, 128) == 128);
	CHECK(reader.getPos() == 4);
	CHECK(std::memcmp(actual.data(), (bytes.data() + 50000), 128) == 0);
	REQUIRE(reader.read(actual.data(), 4) == 4);
	CHECK(std::memcmp(actual.data(), (bytes.data() + 4), 4) == 0);
	REQUIRE(reader.readExact(actual.data(), 50000));
	CHECK(reader.getPos() == 50008);
	CHECK(std::memcmp(actual.data(), (bytes.data() + 8), 50000) == 0);

	// The final prefetch may be short while many subsequent caller reads succeed.
	REQUIRE(reader.setPos(65536) == 65536);
	for (size_t i = 65536; i < bytes.size(); ++i)
	{
		uint8 value = 0;
		REQUIRE(reader.read(value));
		CHECK(value == bytes[i]);
	}
	CHECK(reader.isEOF());
	CHECK(reader.read(actual.data(), 1) == 0);
	CHECK(reader.setPos(3) == 3);
	CHECK_FALSE(reader.readExact(actual.data(), static_cast<int64>(bytes.size() - 2), 4));
	CHECK(reader.getPos() == 3);
	CHECK(reader.lookahead(actual.data(), static_cast<int64>(bytes.size() - 2), 4) == 2);
	CHECK(reader.getPos() == 3);
	CHECK(reader.read(nullptr, -1, 0) == 0);
	CHECK(reader.readExact(nullptr, -1, 0));
	CHECK(reader.getPos() == 3);
	CHECK_THROWS_AS(reader.read(nullptr, 1), Error);
	CHECK_THROWS_AS(reader.read(actual.data(), -1, 1), Error);
	CHECK_THROWS_AS(reader.lookahead(actual.data(), (reader.size() + 1), 1), Error);
	CHECK(reader.skip(INT64_MAX) == reader.size());
	CHECK(reader.skip(INT64_MIN) == 0);
}

TEST_CASE("BinaryFileReader mixed operation model")
{
	const auto bytes = ReaderBytes(65536 + 37);
	const auto path = Test::OutputPath(U"binaryreader/model.bin");
	WriteReaderBytes(path, bytes);
	BinaryFileReader reader{ path };
	REQUIRE(reader.isOpen());
	Array<uint8> actual(32769);
	uint32 state = 2463534242;
	const auto next = [&state]() { state ^= (state << 13); state ^= (state >> 17); state ^= (state << 5); return state; };
	int64 pos = 0;
	const int64 end = static_cast<int64>(bytes.size());
	for (int step = 0; step < 3000; ++step)
	{
		const uint32 operation = (next() % 6);
		const int64 at = (next() % (bytes.size() + 1));
		const int64 size = (next() % actual.size());
		CAPTURE(step);
		CAPTURE(operation);
		CAPTURE(at);
		CAPTURE(size);
		if (operation == 0)
		{
			REQUIRE(reader.setPos(at) == at);
			pos = at;
		}
		else
		{
			const bool positioned = ((operation == 2) || (operation == 4));
			const bool peek = ((operation == 3) || (operation == 4));
			const int64 start = (positioned ? at : pos);
			const int64 count = Min(size, (end - start));
			if (operation == 5)
			{
				const bool complete = reader.readExact(actual.data(), size);
				REQUIRE(complete == (count == size));
				if (complete && size)
				{
					CHECK(std::memcmp(actual.data(), (bytes.data() + start), static_cast<size_t>(count)) == 0);
					pos += count;
				}
			}
			else
			{
				const int64 read = peek
					? (positioned ? reader.lookahead(actual.data(), at, size) : reader.lookahead(actual.data(), size))
					: (positioned ? reader.read(actual.data(), at, size) : reader.read(actual.data(), size));
				REQUIRE(read == count);
				CHECK(std::memcmp(actual.data(), (bytes.data() + start), static_cast<size_t>(count)) == 0);
				if ((not peek) && size) pos = (start + count);
			}
		}
		CHECK(reader.getPos() == pos);
		CHECK(reader.remaining() == (end - pos));
		CHECK(reader.isEOF() == (pos == end));
	}
}

TEST_CASE("BinaryFileReader lifetime and independent readers")
{
	const auto bytes = ReaderBytes(32769);
	const auto other = ReaderBytes(32769, 987654321);
	const auto first = Test::OutputPath(U"binaryreader/lifetime-first.bin");
	const auto second = Test::OutputPath(U"binaryreader/lifetime-second.bin");
	const auto empty = Test::OutputPath(U"binaryreader/lifetime-empty.bin");
	WriteReaderBytes(first, bytes);
	WriteReaderBytes(second, other);
	WriteReaderBytes(empty, {});
	BinaryFileReader reader{ first }, independent{ first };
	uint8 value = 0;
	REQUIRE(reader.read(value));
	REQUIRE(reader.setPos(16380) == 16380);
	BinaryFileReader moved{ std::move(reader) };
	CHECK_FALSE(reader.isOpen());
	REQUIRE(moved.read(value));
	CHECK(value == bytes[16380]);
	CHECK(independent.getPos() == 0);
	REQUIRE(independent.read(value));
	CHECK(value == bytes[0]);
	REQUIRE(reader.open(second));
	REQUIRE(reader.read(value));
	CHECK(value == other[0]);
	reader = std::move(moved);
	REQUIRE(reader.read(value));
	CHECK(value == bytes[16381]);
	reader.close();
	reader.close();
	CHECK_FALSE(reader.isOpen());
	CHECK(reader.path().isEmpty());
	CHECK(reader.size() == 0);
	CHECK(reader.getPos() == 0);
	CHECK_FALSE(reader.isEOF());
	CHECK_FALSE(reader.open(Test::OutputPath(U"binaryreader/absent.bin")));
	REQUIRE(reader.open(second));
	REQUIRE(reader.read(value));
	CHECK(value == other[0]);
	REQUIRE(reader.open(empty));
	CHECK(reader.size() == 0);
	CHECK(reader.isEOF());
	CHECK(reader.read(&value, 1) == 0);
}

# if SIV3D_PLATFORM(WINDOWS)

TEST_CASE("BinaryFileReader Windows range lock and recovery")
{
	const ScopedLogSilencer silence;
	const auto bytes = ReaderBytes(65536);
	const auto path = Test::OutputPath(U"binaryreader/locked.bin");
	WriteReaderBytes(path, bytes);
	const HANDLE handle = ::CreateFileW(path.toWstr().c_str(), (GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE), nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	REQUIRE(handle != INVALID_HANDLE_VALUE);
	const ScopeExit close{ [handle] { ::CloseHandle(handle); } };
	OVERLAPPED lock{};
	lock.Offset = 8192;
	REQUIRE(::LockFileEx(handle, (LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY), 0, 4096, 0, &lock));
	BinaryFileReader reader{ path };
	REQUIRE(reader.isOpen());
	uint32 value = 0;
	// The requested bytes are readable even though speculative read-ahead is locked.
	REQUIRE(reader.read(value));
	CHECK(std::memcmp(&value, bytes.data(), sizeof(value)) == 0);
	REQUIRE(reader.setPos(8192) == 8192);
	CHECK(reader.read(&value, 4) == 0);
	CHECK(reader.getPos() == 8192);
	REQUIRE(::UnlockFileEx(handle, 0, 4096, 0, &lock));
	REQUIRE(reader.read(value));
	CHECK(std::memcmp(&value, (bytes.data() + 8192), sizeof(value)) == 0);

	// Prime the first window, then prevent reads of the following window.
	REQUIRE(reader.open(path));
	REQUIRE(reader.read(value));
	lock.Offset = 16384;
	REQUIRE(::LockFileEx(handle, (LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY), 0, 16384, 0, &lock));
	Array<uint8> actual(32);
	CHECK(reader.lookahead(actual.data(), 16380, 32) == 4);
	CHECK(reader.getPos() == 4);
	CHECK(std::memcmp(actual.data(), (bytes.data() + 16380), 4) == 0);
	REQUIRE(reader.open(path));
	REQUIRE(reader.read(value));
	CHECK_FALSE(reader.readExact(actual.data(), 16380, 32));
	CHECK(reader.getPos() == 4);
	REQUIRE(::UnlockFileEx(handle, 0, 16384, 0, &lock));
	REQUIRE(reader.readExact(actual.data(), 16380, 32));
	CHECK(reader.getPos() == 16412);
	CHECK(std::memcmp(actual.data(), (bytes.data() + 16380), 32) == 0);
}

TEST_CASE("BinaryFileReader Windows resource file transitions")
{
	const auto resource = Resource(U"engine/texture/box-shadow/64.png");
	BinaryFileReader reader{ resource };
	REQUIRE(reader.isOpen());
	const Blob expected = reader.readToEnd();
	REQUIRE(expected.size() > 32);
	const auto path = Test::OutputPath(U"binaryreader/resource-copy.bin");
	{
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write(expected.data(), expected.size()) == static_cast<int64>(expected.size()));
	}
	uint8 actual[32]{};
	for (const auto& source : { path, resource, path, resource })
	{
		REQUIRE(reader.open(source));
		CHECK(reader.getPos() == 0);
		CHECK(reader.size() == static_cast<int64>(expected.size()));
		REQUIRE(reader.readExact(actual, 8, sizeof(actual)));
		CHECK(std::memcmp(actual, (expected.data() + 8), sizeof(actual)) == 0);
		CHECK(reader.getPos() == 40);
		REQUIRE(reader.lookahead(actual, 0, sizeof(actual)) == sizeof(actual));
		CHECK(reader.getPos() == 40);
		CHECK(std::memcmp(actual, expected.data(), sizeof(actual)) == 0);
	}
}

TEST_CASE("BinaryFileReader Windows sparse offsets")
{
	const auto path = Test::OutputPath(U"binaryreader/sparse.bin");
	WriteReaderBytes(path, {});
	const HANDLE handle = ::CreateFileW(path.toWstr().c_str(), GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	REQUIRE(handle != INVALID_HANDLE_VALUE);
	constexpr int64 Boundary = (int64{ 1 } << 32);
	{
		const ScopeExit close{ [handle] { ::CloseHandle(handle); } };
		DWORD count = 0;
		if (not ::DeviceIoControl(handle, FSCTL_SET_SPARSE, nullptr, 0, nullptr, 0, &count, nullptr))
		{
			WARN("Sparse-file check unavailable on this filesystem");
			return;
		}
		LARGE_INTEGER pos;
		pos.QuadPart = (Boundary + 65539);
		REQUIRE(::SetFilePointerEx(handle, pos, nullptr, FILE_BEGIN));
		REQUIRE(::SetEndOfFile(handle));
		pos.QuadPart = (Boundary - 16);
		REQUIRE(::SetFilePointerEx(handle, pos, nullptr, FILE_BEGIN));
		const auto bytes = ReaderBytes(64);
		REQUIRE(::WriteFile(handle, bytes.data(), static_cast<DWORD>(bytes.size()), &count, nullptr));
		REQUIRE(count == bytes.size());
	}
	BinaryFileReader reader{ path };
	REQUIRE(reader.size() == (Boundary + 65539));
	Array<uint8> actual(64);
	REQUIRE(reader.readExact(actual.data(), (Boundary - 16), 64));
	CHECK(actual == ReaderBytes(64));
	CHECK(reader.getPos() == (Boundary + 48));
	REQUIRE(reader.lookahead(actual.data(), (Boundary - 16), 64) == 64);
	CHECK(actual == ReaderBytes(64));
	CHECK(reader.getPos() == (Boundary + 48));
	CHECK(reader.setPos(reader.size() - 3) == (reader.size() - 3));
	CHECK_FALSE(reader.readExact(actual.data(), 4));
	CHECK(reader.getPos() == (reader.size() - 3));
	CHECK(reader.read(actual.data(), 4) == 3);
	CHECK(reader.isEOF());
}

# endif
