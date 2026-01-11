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

TEST_CASE("BinaryFileWriter.constructor")
{
	{
		const FilePath path{ U"../../Test/output/binarywriter/test.bin" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ U"../../Test/output/binarywriter/.test" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ U"../../Test/output/binarywriter/test/" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer{ path };
			CHECK(not writer.isOpen());
		}
		CHECK(not FileSystem::Exists(path));
	}
}

TEST_CASE("BinaryFileWriter.open")
{
	{
		const FilePath path{ U"../../Test/output/binarywriter/test.bin" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer;
			CHECK(writer.open(path));
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ U"../../Test/output/binarywriter/.test" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer;
			CHECK(writer.open(path));
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ U"../../Test/output/binarywriter/test/" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryFileWriter writer;
			CHECK(not writer.open(path));
			CHECK(not writer.isOpen());
		}
		CHECK(not FileSystem::Exists(path));
	}
}
