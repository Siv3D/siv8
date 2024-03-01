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

TEST_CASE("FileSystem::Extension")
{
	CHECK_EQ(FileSystem::Extension(U"document.txt"), U"txt");
	CHECK_EQ(FileSystem::Extension(U"filename"), U"");
	CHECK_EQ(FileSystem::Extension(U"./filename"), U"");
	CHECK_EQ(FileSystem::Extension(U".gitignore"), U"");
	CHECK_EQ(FileSystem::Extension(U".test.txt"), U"txt");
	CHECK_EQ(FileSystem::Extension(U"document.TXT"), U"txt");
	CHECK_EQ(FileSystem::Extension(U"FILENAME"), U"");
	CHECK_EQ(FileSystem::Extension(U"./FILENAME"), U"");
	CHECK_EQ(FileSystem::Extension(U".GITIGNORE"), U"");
	CHECK_EQ(FileSystem::Extension(U".test.TXT"), U"txt");
	CHECK_EQ(FileSystem::Extension(U"path/to/file.pdf"), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"./path/./to/file.pdf"), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"./path/../to/file.pdf"), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"../path/to/file.pdf"), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"archive.tar.gz"), U"gz");
	CHECK_EQ(FileSystem::Extension(U"archive.tar.GZ"), U"gz");
	CHECK_EQ(FileSystem::Extension(U"file."), U"");
	CHECK_EQ(FileSystem::Extension(U"file.."), U"");
	CHECK_EQ(FileSystem::Extension(U"file..."), U"");
	CHECK_EQ(FileSystem::Extension(U"file.txt."), U"");
	CHECK_EQ(FileSystem::Extension(U"file.txt.."), U"");
	CHECK_EQ(FileSystem::Extension(U"file.txt..."), U"");
	CHECK_EQ(FileSystem::Extension(U"my document.docx"), U"docx");
	CHECK_EQ(FileSystem::Extension(U".hidden.tar.gz"), U"gz");
	CHECK_EQ(FileSystem::Extension(U"path/to/.config.file.dat"), U"dat");
	CHECK_EQ(FileSystem::Extension(U"...."), U"");
	CHECK_EQ(FileSystem::Extension(U"file.middle."), U"");
	CHECK_EQ(FileSystem::Extension(U"path.to.folder/file"), U"");
	CHECK_EQ(FileSystem::Extension(U"C:\\path\\to\\file.docx"), U"docx");
	CHECK_EQ(FileSystem::Extension(U"file.extension-with-special_chars!@#"), U"extension-with-special_chars!@#");
	CHECK_EQ(FileSystem::Extension(U"メモ.テキスト"), U"テキスト");
	CHECK_EQ(FileSystem::Extension(U"my file. "), U" ");
	CHECK_EQ(FileSystem::Extension(U"file..txt"), U"txt");
	CHECK_EQ(FileSystem::Extension(U"a.a"), U"a");
	CHECK_EQ(FileSystem::Extension(U"a.A"), U"a");
}

TEST_CASE("FileSystem::ChangeCurrentDirectory")
{
	const FilePath currentDirectory = FileSystem::CurrentDirectory();

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(U"./"), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(currentDirectory), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(U"example/"), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), (currentDirectory + U"example/"));

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(U"../"), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);
}

TEST_CASE("FileSystem Misc")
{
	Console << U"FileSystem";
	Console << U"| CreationTime(ModulePath()):\t" << FileSystem::CreationTime(FileSystem::ModulePath());
	Console << U"| WriteTime(ModulePath()):\t" << FileSystem::WriteTime(FileSystem::ModulePath());
	Console << U"| AccessTime(ModulePath()):\t" << FileSystem::AccessTime(FileSystem::ModulePath());
	Console << U"| InitialDirectory:\t" << FileSystem::InitialDirectory();
	Console << U"| ModulePath:\t" << FileSystem::ModulePath();
	Console << U"| CurrentDirectory:\t" << FileSystem::CurrentDirectory();
}
