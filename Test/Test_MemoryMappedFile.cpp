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
# include <limits>
# include <type_traits>

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)
# include <sys/stat.h>
# endif

TEST_CASE("MemoryMappedFile.flush_closed_file")
{
	MemoryMappedFile file;
	CHECK_FALSE(file.flush());
}

[[nodiscard]]
static std::string CreateTestData()
{
	std::string s;

	for (auto ch : std::string{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" })
	{
		s.append(98, ch);
		s.append("\r\n");
	}

	return s;
}

TEST_CASE("MemoryMappedFile")
{
	const std::string testData = CreateTestData();
	
	{
		TextFileWriter writer{ Test::OutputPath(U"mmf/text.txt"), TextEncoding::UTF8_NO_BOM };
		writer.writeUTF8(testData);
	}

	{
		MemoryMappedFile mmf;
		CHECK(not mmf.isOpen());
		CHECK(mmf.path().isEmpty());
	}

	{
		{
			MemoryMappedFile mmf{ Test::OutputPath(U"mmf/text.txt"), MemoryMappedFile::ExistingFilePolicy::JustOpen };
			CHECK(mmf.isOpen());
			CHECK(mmf.path() == FileSystem::FullPath(Test::OutputPath(U"mmf/text.txt")));
			CHECK(mmf.size() == static_cast<int64>(testData.size()));

			{
				auto mapped = mmf.mapAll();
				CHECK(mapped.operator bool());

				auto mapped2 = mmf.mapAll();
				CHECK(not mapped2);

				mmf.unmap();
			}

			{
				auto mapped = mmf.mapAll();
				CHECK(mapped.operator bool());
				CHECK(mapped.size == testData.size());
				CHECK(std::memcmp(mapped.data, testData.data(), testData.size()) == 0);
				mmf.unmap();
			}

			{
				auto mapped = mmf.map(100, 100);
				CHECK(mapped.operator bool());
				CHECK(mapped.size == 100);
				CHECK(std::memcmp(mapped.data, (testData.data() + 100), 100) == 0);
				mmf.unmap();
			}

			{
				auto mapped = mmf.map(100, 100);
				char* p = static_cast<char*>(mapped.data);

				for (int32 i = 0; i < 100; ++i)
				{
					p[i] = '-';
				}

				mmf.unmap();
			}

			{
				auto mapped = mmf.map(100, 100);
				CHECK(mapped.operator bool());
				CHECK(mapped.size == 100);

				const std::string expected = std::string(100, '-');
				CHECK(std::memcmp(mapped.data, expected.data(), 100) == 0);
				mmf.unmap();
			}
		}

		BinaryFileReader reader{ Test::OutputPath(U"mmf/text.txt") };
		std::string buffer(100, '\0');
		
		reader.read(buffer.data(), 100);
		CHECK(buffer == testData.substr(0, 100));
		
		reader.read(buffer.data(), 100);
		CHECK(buffer == std::string(100, '-'));
	}

	{
		const FilePath path = Test::OutputPath(U"mmf/create_1.txt");
		{
			MemoryMappedFile mmf{ path, MemoryMappedFile::ExistingFilePolicy::JustOpen };
			auto mapped = mmf.mapAll();
			CHECK(not mapped);
		}

		CHECK(FileSystem::Exists(path));
		CHECK(FileSystem::FileSize(path) == 0);
	}

	{
		const FilePath path = Test::OutputPath(U"mmf/create_2.txt");
		{
			MemoryMappedFile mmf{ path, MemoryMappedFile::ExistingFilePolicy::Fail, MemoryMappedFile::MissingFilePolicy::Fail };
			auto mapped = mmf.mapAll();
			CHECK(not mapped);
		}

		CHECK(not FileSystem::Exists(path));
		CHECK(FileSystem::FileSize(path) == 0);
	}

	{
		const FilePath path = Test::OutputPath(U"mmf/create_3.txt");
		{
			MemoryMappedFile mmf{ path, MemoryMappedFile::ExistingFilePolicy::JustOpen };
			auto mapped = mmf.map(0, (1024 * 1024));
			CHECK(mapped);

			char* p = static_cast<char*>(mapped.data);

			for (int32 i = 0; i < (1024 * 1024); ++i)
			{
				p[i] = 'a';
			}

			mmf.unmap();
			CHECK(mmf.size() == (1024 * 1024));
		}

		CHECK(FileSystem::Exists(path));
		CHECK(FileSystem::FileSize(path) == (1024 * 1024));

		Blob blob = Blob{ path };
		const std::string s(1024 * 1024, 'a');
		CHECK(blob == Blob{ s.data(), s.size() });
	}
}

namespace
{
	template <class File>
	bool OpenMapping(File& file, const FilePathView path)
	{
		if constexpr (std::same_as<File, MemoryMappedFile>)
		{
			return file.open(path, MemoryMappedFile::ExistingFilePolicy::JustOpen, MemoryMappedFile::MissingFilePolicy::Fail);
		}
		else
		{
			return file.open(path);
		}
	}

	Array<uint8> MappingBytes(const size_t size)
	{
		Array<uint8> bytes(size);
		for (size_t i = 0; i < size; ++i)
		{
			bytes[i] = static_cast<uint8>((i * 37 + i / 251) & 0xFF);
		}
		return bytes;
	}

	void WriteMappingBytes(const FilePathView path, const Array<uint8>& bytes)
	{
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write(bytes.data(), static_cast<int64>(bytes.size())) == static_cast<int64>(bytes.size()));
	}

	template <class File>
	FilePath MappingPath(const StringView leaf)
	{
		FilePath relative = std::same_as<File, MemoryMappedFile> ? U"mapped/file/" : U"mapped/view/";
		relative += leaf;
		return Test::OutputPath(relative);
	}

	template <class File>
	void CheckClosedMapping(File& file)
	{
		CHECK_FALSE(file.isOpen());
		CHECK_FALSE(static_cast<bool>(file));
		CHECK(file.size() == 0);
		CHECK(file.path().isEmpty());
		CHECK_FALSE(file.mapAll());
		CHECK_FALSE(file.map(0, 1));
		file.unmap();
		file.close();
		if constexpr (std::same_as<File, MemoryMappedFile>)
		{
			CHECK_FALSE(file.flush());
		}
	}
}

TEMPLATE_TEST_CASE("MemoryMappedFile.closed_empty_and_reopen", "", MemoryMappedFile, MemoryMappedFileView)
{
	ScopedLogSilencer silence;
	TestType file;
	CheckClosedMapping(file);
	const auto path = MappingPath<TestType>(U"empty.bin");
	WriteMappingBytes(path, {});
	REQUIRE(OpenMapping(file, path));
	CHECK(file.isOpen());
	CHECK(static_cast<bool>(file));
	CHECK(file.path() == FileSystem::FullPath(path));
	CHECK(file.size() == 0);
	CHECK_FALSE(file.mapAll());
	CHECK_FALSE(file.map(0, 0));
	if constexpr (std::same_as<TestType, MemoryMappedFile>)
	{
		CHECK(file.flush());
	}
	REQUIRE(OpenMapping(file, file.path()));
	CHECK_FALSE(OpenMapping(file, MappingPath<TestType>(U"does-not-exist.bin")));
	CheckClosedMapping(file);
	CHECK_FALSE(OpenMapping(file, U""));
	CheckClosedMapping(file);
	REQUIRE(OpenMapping(file, path));
	file.close();
	CheckClosedMapping(file);
}

TEMPLATE_TEST_CASE("MemoryMappedFile.ranges_and_mapping_lifetime", "", MemoryMappedFile, MemoryMappedFileView)
{
	ScopedLogSilencer silence;
	const auto bytes = MappingBytes(131109);
	const auto path = MappingPath<TestType>(U"ranges.bin");
	WriteMappingBytes(path, bytes);
	TestType file;
	REQUIRE(OpenMapping(file, path));
	for (const size_t offset : { size_t{ 0 }, size_t{ 1 }, size_t{ 4095 }, size_t{ 4096 }, size_t{ 16383 }, size_t{ 16384 }, size_t{ 65535 }, size_t{ 65536 } })
	{
		for (const size_t count : { size_t{ 1 }, size_t{ 257 }, size_t{ 65537 } })
		{
			CAPTURE(offset, count);
			const auto mapped = file.map(offset, count);
			REQUIRE(mapped);
			CHECK(mapped.size == count);
			CHECK(std::memcmp(mapped.data, bytes.data() + offset, count) == 0);
			CHECK_FALSE(file.mapAll());
			CHECK_FALSE(file.map(0, bytes.size() + 100));
			CHECK(file.size() == static_cast<int64>(bytes.size()));
			CHECK(std::memcmp(mapped.data, bytes.data() + offset, count) == 0);
			file.unmap();
			file.unmap();
		}
	}
	for (const size_t offset : { size_t{ 0 }, size_t{ 1 }, bytes.size() })
	{
		CHECK_FALSE(file.map(offset, 0));
	}
	for (const size_t offset : { bytes.size() + 1, std::numeric_limits<size_t>::max() })
	{
		const auto invalid = file.map(offset, 1);
		CHECK_FALSE(invalid);
		CHECK(invalid.data == nullptr);
		CHECK(invalid.size == 0);
	}
	const auto mapped = file.mapAll();
	REQUIRE(mapped);
	CHECK(mapped.size == bytes.size());
	CHECK(std::memcmp(mapped.data, bytes.data(), bytes.size()) == 0);
	// open(path()) must retain the path before invalidating the old mapping.
	REQUIRE(OpenMapping(file, file.path()));
	REQUIRE(file.mapAll());
	file.close();
	CheckClosedMapping(file);
}

TEMPLATE_TEST_CASE("MemoryMappedFile.rejects_invalid_paths_and_preserves_files", "", MemoryMappedFile, MemoryMappedFileView)
{
	ScopedLogSilencer silence;
	const auto bytes = MappingBytes(32);
	const auto path = MappingPath<TestType>(U"invalid-path.bin");
	WriteMappingBytes(path, bytes);
	TestType file;
	REQUIRE(OpenMapping(file, path));
	REQUIRE(file.mapAll());
	FilePath withNull = path;
	withNull.push_back(U'\0');
	withNull += U"ignored";
	CHECK_FALSE(OpenMapping(file, withNull));
	CheckClosedMapping(file);
	CHECK_FALSE(OpenMapping(file, FileSystem::ParentPath(path)));
	CheckClosedMapping(file);
	CHECK(Blob{ path } == Blob{ bytes.data(), bytes.size() });
}

TEMPLATE_TEST_CASE("MemoryMappedFile.move_transfers_live_mapping", "", MemoryMappedFile, MemoryMappedFileView)
{
	STATIC_REQUIRE(not std::is_copy_constructible_v<TestType>);
	STATIC_REQUIRE(not std::is_copy_assignable_v<TestType>);
	STATIC_REQUIRE(std::is_nothrow_move_constructible_v<TestType>);
	STATIC_REQUIRE(std::is_nothrow_move_assignable_v<TestType>);
	ScopedLogSilencer silence;
	const auto bytes = MappingBytes(100);
	const auto path = MappingPath<TestType>(U"move.bin");
	const auto otherPath = MappingPath<TestType>(U"move-target.bin");
	WriteMappingBytes(path, bytes);
	WriteMappingBytes(otherPath, MappingBytes(20));
	TestType source;
	REQUIRE(OpenMapping(source, path));
	const auto mapped = source.map(1, 99);
	REQUIRE(mapped);
	TestType moved{ std::move(source) };
	CheckClosedMapping(source);
	CHECK(moved.path() == FileSystem::FullPath(path));
	CHECK(moved.size() == 100);
	CHECK_FALSE(moved.mapAll());
	CHECK(std::memcmp(mapped.data, bytes.data() + 1, 99) == 0);

	TestType target;
	REQUIRE(OpenMapping(target, otherPath));
	REQUIRE(target.mapAll());
	CHECK(&(target = std::move(moved)) == &target);
	CheckClosedMapping(moved);
	CHECK(target.path() == FileSystem::FullPath(path));
	CHECK_FALSE(target.mapAll());
	CHECK(std::memcmp(mapped.data, bytes.data() + 1, 99) == 0);
	auto& alias = target;
	target = std::move(alias);
	CHECK(target.size() == 100);
	CHECK(std::memcmp(mapped.data, bytes.data() + 1, 99) == 0);

	REQUIRE(OpenMapping(source, path));
	REQUIRE(source.mapAll());
	target.unmap();
	REQUIRE(target.mapAll());
	TestType empty;
	target = std::move(empty);
	CheckClosedMapping(target);
	CheckClosedMapping(empty);
	TestType movedEmpty{ std::move(empty) };
	CheckClosedMapping(movedEmpty);
	CheckClosedMapping(empty);
}

TEST_CASE("MemoryMappedFile.existing_and_missing_policies")
{
	using Existing = MemoryMappedFile::ExistingFilePolicy;
	using Missing = MemoryMappedFile::MissingFilePolicy;
	ScopedLogSilencer silence;
	const auto bytes = MappingBytes(32);
	for (const bool exists : { false, true })
	{
		for (const auto existing : { Existing::Fail, Existing::JustOpen, Existing::Truncate })
		{
			for (const auto missing : { Missing::Fail, Missing::Create })
			{
				CAPTURE(exists, static_cast<int>(existing), static_cast<int>(missing));
				const auto path = Test::OutputPath(Format(U"mapped/policy-{}-{}-{}.bin", exists, static_cast<int>(existing), static_cast<int>(missing)));
				REQUIRE(FileSystem::CreateParentDirectories(path));
				if (exists)
				{
					WriteMappingBytes(path, bytes);
				}
				MemoryMappedFile file;
				const bool expected = (exists ? (existing != Existing::Fail) : (missing == Missing::Create));
				CHECK(file.open(path, existing, missing) == expected);
				CHECK(file.isOpen() == expected);
				const int64 expectedSize = (exists && (existing != Existing::Truncate)) ? static_cast<int64>(bytes.size()) : 0;
				CHECK(file.size() == (expected ? expectedSize : 0));
				file.close();
				CHECK(FileSystem::Exists(path) == (exists || expected));
				if (exists && ((not expected) || (existing != Existing::Truncate)))
				{
					CHECK(Blob{ path } == Blob{ bytes.data(), bytes.size() });
				}
				else if (expected)
				{
					CHECK(FileSystem::FileSize(path) == 0);
				}
			}
		}
	}
}

TEST_CASE("MemoryMappedFile.extension_flush_and_round_trip")
{
	ScopedLogSilencer silence;
	for (const bool flushAfterUnmap : { false, true })
	{
		CAPTURE(flushAfterUnmap);
		const auto path = Test::OutputPath(Format(U"mapped/round-trip-{}.bin", flushAfterUnmap));
		auto expected = MappingBytes(6);
		WriteMappingBytes(path, expected);
		{
			MemoryMappedFile file{ path, MemoryMappedFile::ExistingFilePolicy::JustOpen };
			REQUIRE(file.isOpen());
			const auto mapped = file.map(5, 8);
			REQUIRE(mapped);
			CHECK(mapped.size == 8);
			CHECK(file.size() == 13);
			std::memset(mapped.data, 0xA5, mapped.size);
			expected.resize(13);
			std::fill(expected.begin() + 5, expected.end(), uint8{ 0xA5 });
			if (flushAfterUnmap)
			{
				file.unmap();
			}
			REQUIRE(file.flush());
			file.unmap();
			const auto appended = file.map(13, 3);
			REQUIRE(appended);
			std::memset(appended.data, 0x5A, appended.size);
			expected.resize(16, uint8{ 0x5A });
			CHECK(file.size() == 16);
			REQUIRE(file.flush());
		} // Destruction also releases an active mapping.
		CHECK(FileSystem::FileSize(path) == static_cast<uint64>(expected.size()));
		CHECK(Blob{ path } == Blob{ expected.data(), expected.size() });
		MemoryMappedFileView reader{ path };
		const auto mapped = reader.mapAll();
		REQUIRE(mapped);
		CHECK(mapped.size == expected.size());
		CHECK(std::memcmp(mapped.data, expected.data(), expected.size()) == 0);
	}
}

TEST_CASE("MemoryMappedFile.rejects_unrepresentable_extent_without_extension")
{
	ScopedLogSilencer silence;
	const auto bytes = MappingBytes(32);
	const auto path = MappingPath<MemoryMappedFile>(U"overflow.bin");
	WriteMappingBytes(path, bytes);
	MemoryMappedFile file{ path, MemoryMappedFile::ExistingFilePolicy::JustOpen };
	for (const size_t offset : { size_t{ 0 }, size_t{ 1 }, bytes.size() })
	{
		CHECK_FALSE(file.map(offset, std::numeric_limits<size_t>::max()));
		if constexpr (sizeof(size_t) >= sizeof(int64))
		{
			CHECK_FALSE(file.map(offset, static_cast<size_t>(std::numeric_limits<int64>::max()) + 1));
		}
		CHECK(file.size() == static_cast<int64>(bytes.size()));
		CHECK(FileSystem::FileSize(path) == static_cast<uint64>(bytes.size()));
		REQUIRE(file.mapAll());
		file.unmap();
	}
	file.close();
	CHECK(Blob{ path } == Blob{ bytes.data(), bytes.size() });
}

TEST_CASE("MemoryMappedFile.rejects_writable_resource")
{
	ScopedLogSilencer silence;
	MemoryMappedFile file;
	CHECK_FALSE(file.open(Resource(U"engine/texture/box-shadow/64.png"), MemoryMappedFile::ExistingFilePolicy::JustOpen));
	CheckClosedMapping(file);
}

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)

TEST_CASE("MemoryMappedFile.rejects_fifo_without_waiting_for_peer")
{
	ScopedLogSilencer silence;
	const auto path = Test::OutputPath(U"mapped/unsupported-fifo");
	REQUIRE(FileSystem::CreateParentDirectories(path));
	REQUIRE(::mkfifo(path.toUTF8().c_str(), (S_IRUSR | S_IWUSR)) == 0);
	MemoryMappedFileView reader;
	CHECK_FALSE(reader.open(path));
	CheckClosedMapping(reader);
	MemoryMappedFile writer;
	CHECK_FALSE(OpenMapping(writer, path));
	CheckClosedMapping(writer);
}

# endif
