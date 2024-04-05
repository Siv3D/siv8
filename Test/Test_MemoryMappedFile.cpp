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
		TextWriter writer{ U"../../Test/output/mmf/text.txt", TextEncoding::UTF8_NO_BOM };
		writer.writeUTF8(testData);
	}

	{
		MemoryMappedFile mmf;
		CHECK(not mmf.isOpen());
		CHECK(mmf.path().isEmpty());
	}

	{
		{
			MemoryMappedFile mmf{ U"../../Test/output/mmf/text.txt", MemoryMappedFile::OpenMode_if_Exists::JustOpen };
			CHECK(mmf.isOpen());
			CHECK(mmf.path() == FileSystem::FullPath(U"../../Test/output/mmf/text.txt"));
			CHECK(mmf.size() == testData.size());

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

		BinaryReader reader{ U"../../Test/output/mmf/text.txt" };
		std::string buffer(100, '\0');
		
		reader.read(buffer.data(), 100);
		CHECK(buffer == testData.substr(0, 100));
		
		reader.read(buffer.data(), 100);
		CHECK(buffer == std::string(100, '-'));
	}

	{
		const FilePath path = U"../../Test/output/mmf/create_1.txt";
		{
			MemoryMappedFile mmf{ path, MemoryMappedFile::OpenMode_if_Exists::JustOpen };
			auto mapped = mmf.mapAll();
			CHECK(not mapped);
		}

		CHECK(FileSystem::Exists(path));
		CHECK(FileSystem::FileSize(path) == 0);
	}

	{
		const FilePath path = U"../../Test/output/mmf/create_2.txt";
		{
			MemoryMappedFile mmf{ path, MemoryMappedFile::OpenMode_if_Exists::Fail, MemoryMappedFile::OpenMode_if_NotFound::Fail };
			auto mapped = mmf.mapAll();
			CHECK(not mapped);
		}

		CHECK(not FileSystem::Exists(path));
		CHECK(FileSystem::FileSize(path) == 0);
	}

	{
		const FilePath path = U"../../Test/output/mmf/create_3.txt";
		{
			MemoryMappedFile mmf{ path, MemoryMappedFile::OpenMode_if_Exists::JustOpen };
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
