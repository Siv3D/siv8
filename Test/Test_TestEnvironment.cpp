//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("Test::OutputPath stays anchored when the working directory changes")
{
	const FilePath originalDirectory = FileSystem::CurrentDirectory();
	const FilePath expected = FileSystem::FullPath(U"../../Test/output/environment/日本語.bin");
	CHECK_EQ(Test::OutputPath(U"environment/日本語.bin"), expected);
	const ScopeExit restoreDirectory{ [&] { FileSystem::ChangeCurrentDirectory(originalDirectory); } };
	REQUIRE(FileSystem::ChangeCurrentDirectory(U"example/"));
	CHECK_EQ(Test::OutputPath(U"environment/日本語.bin"), expected);
	{
		BinaryFileWriter writer{ Test::OutputPath(U"environment/日本語.bin") };
		REQUIRE(writer);
		REQUIRE_EQ(writer.write("test", 4), int64{ 4 });
	}
	CHECK_EQ(Blob{ expected }, Blob("test", 4));
}

TEST_CASE("Test::OutputPath accepts child files and directories")
{
	const FilePath root = FileSystem::FullPath(U"../../Test/output/");
	CHECK_EQ(Test::OutputPath(U".hidden"), (root + U".hidden"));
	CHECK_EQ(Test::OutputPath(U"nested/directory/"), (root + U"nested/directory/"));
	CHECK_EQ(Test::OutputPath(U"name..txt"), (root + U"name..txt"));
}

TEST_CASE("Test::OutputPath rejects invalid paths")
{
	for (const FilePathView path : { U"", U"/absolute", U"C:/absolute", U"a:b", U"\\\\server\\file",
		U"nested\\file", U".", U"..", U"../escape", U"nested/../escape", U"nested/..", U"./file", U"nested/./file" })
	{
		CHECK_THROWS_AS((void)Test::OutputPath(path), Error);
	}
	CHECK_THROWS_AS((void)Test::OutputPath(FilePathView{ U"bad\0path", 8 }), Error);
}
