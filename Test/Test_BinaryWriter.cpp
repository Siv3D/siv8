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

TEST_CASE("BinaryWriter.constructor")
{
	{
		const FilePath path{ U"../../Test/output/binarywriter/test.bin" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryWriter writer{ path };
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ U"../../Test/output/binarywriter/.test" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryWriter writer{ path };
			CHECK(writer.isOpen());
		}
		CHECK(FileSystem::Exists(path));
		FileSystem::Remove(path);
	}

	{
		const FilePath path{ U"../../Test/output/binarywriter/test/" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryWriter writer{ path };
			CHECK(not writer.isOpen());
		}
		CHECK(not FileSystem::Exists(path));
	}
}

TEST_CASE("BinaryWriter.open")
{
	{
		const FilePath path{ U"../../Test/output/binarywriter/test.bin" };
		CHECK(not FileSystem::Exists(path));
		{
			BinaryWriter writer;
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
			BinaryWriter writer;
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
			BinaryWriter writer;
			CHECK(not writer.open(path));
			CHECK(not writer.isOpen());
		}
		CHECK(not FileSystem::Exists(path));
	}
}
