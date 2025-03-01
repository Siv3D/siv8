//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "Siv3DTest.hpp"

TEST_CASE("FileSystem::Extension")
{
	CHECK_EQ(FileSystem::Extension(U"aaa.png"), U"png");
	CHECK_EQ(FileSystem::Extension(U"./aaa.png"), U"png");
	CHECK_EQ(FileSystem::Extension(U"../aaa.png"), U"png");
	CHECK_EQ(FileSystem::Extension(U"aaa"), U"");
	CHECK_EQ(FileSystem::Extension(U"aaa.bbb/ccc"), U"");
	CHECK_EQ(FileSystem::Extension(U"aaa.bbb/ccc.d"), U"d");
	CHECK_EQ(FileSystem::Extension(U"aaa.bbb/ccc.d.e"), U"e");
	CHECK_EQ(FileSystem::Extension(U""), U"");
	CHECK_EQ(FileSystem::Extension(U"./"), U"");
	CHECK_EQ(FileSystem::Extension(U"../"), U"");
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
	CHECK_EQ(FileSystem::Extension(Resource(U"example/windmill.png")), U"png");
	CHECK_EQ(FileSystem::Extension(Resource(U"example/windmill.PNG")), U"png");
	CHECK_EQ(FileSystem::Extension(Resource(U"example/windmill")), U"");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/windmill")), U"");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/windmill.p")), U"p");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/a.b.c/windmill.p")), U"p");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/a.b.c/windmill.p.q")), U"q");
}

TEST_CASE("FileSystem::Extension(PreserveCase::Yes)")
{
	CHECK_EQ(FileSystem::Extension(U"aaa.png", PreserveCase::Yes), U"png");
	CHECK_EQ(FileSystem::Extension(U"./aaa.png", PreserveCase::Yes), U"png");
	CHECK_EQ(FileSystem::Extension(U"../aaa.png", PreserveCase::Yes), U"png");
	CHECK_EQ(FileSystem::Extension(U"aaa", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"aaa.bbb/ccc", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"aaa.bbb/ccc.d", PreserveCase::Yes), U"d");
	CHECK_EQ(FileSystem::Extension(U"aaa.bbb/ccc.d.e", PreserveCase::Yes), U"e");
	CHECK_EQ(FileSystem::Extension(U"", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"./", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"../", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"document.txt", PreserveCase::Yes), U"txt");
	CHECK_EQ(FileSystem::Extension(U"filename", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"./filename", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U".gitignore", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U".test.txt", PreserveCase::Yes), U"txt");
	CHECK_EQ(FileSystem::Extension(U"document.TXT", PreserveCase::Yes), U"TXT");
	CHECK_EQ(FileSystem::Extension(U"FILENAME", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"./FILENAME", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U".GITIGNORE", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U".test.TXT", PreserveCase::Yes), U"TXT");
	CHECK_EQ(FileSystem::Extension(U"path/to/file.pdf", PreserveCase::Yes), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"./path/./to/file.pdf", PreserveCase::Yes), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"./path/../to/file.pdf", PreserveCase::Yes), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"../path/to/file.pdf", PreserveCase::Yes), U"pdf");
	CHECK_EQ(FileSystem::Extension(U"archive.tar.gz", PreserveCase::Yes), U"gz");
	CHECK_EQ(FileSystem::Extension(U"archive.tar.GZ", PreserveCase::Yes), U"GZ");
	CHECK_EQ(FileSystem::Extension(U"file.", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"file..", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"file...", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"file.txt.", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"file.txt..", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"file.txt...", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"my document.docx", PreserveCase::Yes), U"docx");
	CHECK_EQ(FileSystem::Extension(U".hidden.tar.gz", PreserveCase::Yes), U"gz");
	CHECK_EQ(FileSystem::Extension(U"path/to/.config.file.dat", PreserveCase::Yes), U"dat");
	CHECK_EQ(FileSystem::Extension(U"....", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"file.middle.", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"path.to.folder/file", PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(U"C:\\path\\to\\file.docx", PreserveCase::Yes), U"docx");
	CHECK_EQ(FileSystem::Extension(U"file.extension-with-special_chars!@#", PreserveCase::Yes), U"extension-with-special_chars!@#");
	CHECK_EQ(FileSystem::Extension(U"メモ.テキスト", PreserveCase::Yes), U"テキスト");
	CHECK_EQ(FileSystem::Extension(U"my file. ", PreserveCase::Yes), U" ");
	CHECK_EQ(FileSystem::Extension(U"file..txt", PreserveCase::Yes), U"txt");
	CHECK_EQ(FileSystem::Extension(U"a.a", PreserveCase::Yes), U"a");
	CHECK_EQ(FileSystem::Extension(U"a.A", PreserveCase::Yes), U"A");
	CHECK_EQ(FileSystem::Extension(Resource(U"example/windmill.png"), PreserveCase::Yes), U"png");
	CHECK_EQ(FileSystem::Extension(Resource(U"example/windmill.PNG"), PreserveCase::Yes), U"PNG");
	CHECK_EQ(FileSystem::Extension(Resource(U"example/windmill"), PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/windmill"), PreserveCase::Yes), U"");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/windmill.p"), PreserveCase::Yes), U"p");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/a.b.c/windmill.p"), PreserveCase::Yes), U"p");
	CHECK_EQ(FileSystem::Extension(Resource(U"example.test/a.b.c/windmill.p.q"), PreserveCase::Yes), U"q");
}

TEST_CASE("FileSystem::FileName")
{
	CHECK_EQ(FileSystem::FileName(U"aaa.png"), U"aaa.png");
	CHECK_EQ(FileSystem::FileName(U"./aaa.png"), U"aaa.png");
	CHECK_EQ(FileSystem::FileName(U"../aaa.png"), U"aaa.png");
	CHECK_EQ(FileSystem::FileName(U"aaa/"), U"");
	CHECK_EQ(FileSystem::FileName(U"aaa/bbb/"), U"");
	CHECK_EQ(FileSystem::FileName(U"aaa/bbb.aa/ccc"), U"ccc");
	CHECK_EQ(FileSystem::FileName(U"aaa/bbb.aa/ccc/"), U"");
	CHECK_EQ(FileSystem::FileName(U"aaa"), U"aaa");
	CHECK_EQ(FileSystem::FileName(U"aaa.bbb/ccc"), U"ccc");
	CHECK_EQ(FileSystem::FileName(U"aaa.bbb/ccc.d"), U"ccc.d");
	CHECK_EQ(FileSystem::FileName(U"aaa.bbb/ccc.d.e"), U"ccc.d.e");
	CHECK_EQ(FileSystem::FileName(U""), U"");
	CHECK_EQ(FileSystem::FileName(U"./"), U"");
	CHECK_EQ(FileSystem::FileName(U"../"), U"");
	CHECK_EQ(FileSystem::FileName(U"document.txt"), U"document.txt");
	CHECK_EQ(FileSystem::FileName(U"filename"), U"filename");
	CHECK_EQ(FileSystem::FileName(U"./filename"), U"filename");
	CHECK_EQ(FileSystem::FileName(U".gitignore"), U".gitignore");
	CHECK_EQ(FileSystem::FileName(U".test.txt"), U".test.txt");
	CHECK_EQ(FileSystem::FileName(U"document.TXT"), U"document.TXT");
	CHECK_EQ(FileSystem::FileName(U"FILENAME"), U"FILENAME");
	CHECK_EQ(FileSystem::FileName(U"./FILENAME"), U"FILENAME");
	CHECK_EQ(FileSystem::FileName(U".GITIGNORE"), U".GITIGNORE");
	CHECK_EQ(FileSystem::FileName(U".test.TXT"), U".test.TXT");
	CHECK_EQ(FileSystem::FileName(U"path/to/file.pdf"), U"file.pdf");
	CHECK_EQ(FileSystem::FileName(U"./path/./to/file.pdf"), U"file.pdf");
	CHECK_EQ(FileSystem::FileName(U"./path/../to/file.pdf"), U"file.pdf");
	CHECK_EQ(FileSystem::FileName(U"../path/to/file.pdf"), U"file.pdf");
	CHECK_EQ(FileSystem::FileName(U"archive.tar.gz"), U"archive.tar.gz");
	CHECK_EQ(FileSystem::FileName(U"archive.tar.GZ"), U"archive.tar.GZ");
	CHECK_EQ(FileSystem::FileName(U"file."), U"file.");
	CHECK_EQ(FileSystem::FileName(U"file.."), U"file..");
	CHECK_EQ(FileSystem::FileName(U"file..."), U"file...");
	CHECK_EQ(FileSystem::FileName(U"file.txt."), U"file.txt.");
	CHECK_EQ(FileSystem::FileName(U"file.txt.."), U"file.txt..");
	CHECK_EQ(FileSystem::FileName(U"file.txt..."), U"file.txt...");
	CHECK_EQ(FileSystem::FileName(U"my document.docx"), U"my document.docx");
	CHECK_EQ(FileSystem::FileName(U".hidden.tar.gz"), U".hidden.tar.gz");
	CHECK_EQ(FileSystem::FileName(U"path/to/.config.file.dat"), U".config.file.dat");
	CHECK_EQ(FileSystem::FileName(U"...."), U"....");
	CHECK_EQ(FileSystem::FileName(U"file.middle."), U"file.middle.");
	CHECK_EQ(FileSystem::FileName(U"path.to.folder/file"), U"file");
	CHECK_EQ(FileSystem::FileName(U"C:\\path\\to\\file.docx"), U"file.docx");
	CHECK_EQ(FileSystem::FileName(U"file.extension-with-special_chars!@#"), U"file.extension-with-special_chars!@#");
	CHECK_EQ(FileSystem::FileName(U"メモ.テキスト"), U"メモ.テキスト");
	CHECK_EQ(FileSystem::FileName(U"my file. "), U"my file. ");
	CHECK_EQ(FileSystem::FileName(U"file..txt"), U"file..txt");
	CHECK_EQ(FileSystem::FileName(U"a.a"), U"a.a");
	CHECK_EQ(FileSystem::FileName(U"a.A"), U"a.A");
	CHECK_EQ(FileSystem::FileName(Resource(U"example/windmill.png")), U"windmill.png");
	CHECK_EQ(FileSystem::FileName(Resource(U"example/windmill.PNG")), U"windmill.PNG");
	CHECK_EQ(FileSystem::FileName(Resource(U"example/windmill")), U"windmill");
	CHECK_EQ(FileSystem::FileName(Resource(U"example.test/windmill")), U"windmill");
	CHECK_EQ(FileSystem::FileName(Resource(U"example.test/windmill/")), U"");
	CHECK_EQ(FileSystem::FileName(Resource(U"")), U"");
	CHECK_EQ(FileSystem::FileName(Resource(U"example.test/windmill.p")), U"windmill.p");
	CHECK_EQ(FileSystem::FileName(Resource(U"example.test/a.b.c/windmill.p")), U"windmill.p");
	CHECK_EQ(FileSystem::FileName(Resource(U"example.test/a.b.c/windmill.p.q")), U"windmill.p.q");
}

TEST_CASE("FileSystem::BaseName")
{
	CHECK_EQ(FileSystem::BaseName(U"aaa.png"), U"aaa");
	CHECK_EQ(FileSystem::BaseName(U"./aaa.png"), U"aaa");
	CHECK_EQ(FileSystem::BaseName(U"../aaa.png"), U"aaa");
	CHECK_EQ(FileSystem::BaseName(U"aaa/"), U"");
	CHECK_EQ(FileSystem::BaseName(U"aaa/bbb/"), U"");
	CHECK_EQ(FileSystem::BaseName(U"aaa/bbb.aa/ccc"), U"ccc");
	CHECK_EQ(FileSystem::BaseName(U"aaa/bbb.aa/ccc/"), U"");
	CHECK_EQ(FileSystem::BaseName(U"aaa"), U"aaa");
	CHECK_EQ(FileSystem::BaseName(U"aaa.bbb/ccc"), U"ccc");
	CHECK_EQ(FileSystem::BaseName(U"aaa.bbb/ccc.d"), U"ccc");
	CHECK_EQ(FileSystem::BaseName(U"aaa.bbb/ccc.d.e"), U"ccc.d");
	CHECK_EQ(FileSystem::BaseName(U""), U"");
	CHECK_EQ(FileSystem::BaseName(U"./"), U"");
	CHECK_EQ(FileSystem::BaseName(U"../"), U"");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example/windmill.png")), U"windmill");
	CHECK_EQ(FileSystem::BaseName(U"document.txt"), U"document");
	CHECK_EQ(FileSystem::BaseName(U"filename"), U"filename");
	CHECK_EQ(FileSystem::BaseName(U"./filename"), U"filename");
	CHECK_EQ(FileSystem::BaseName(U".gitignore"), U".gitignore");
	CHECK_EQ(FileSystem::BaseName(U".test.txt"), U".test");
	CHECK_EQ(FileSystem::BaseName(U"document.TXT"), U"document");
	CHECK_EQ(FileSystem::BaseName(U"FILENAME"), U"FILENAME");
	CHECK_EQ(FileSystem::BaseName(U"./FILENAME"), U"FILENAME");
	CHECK_EQ(FileSystem::BaseName(U".GITIGNORE"), U".GITIGNORE");
	CHECK_EQ(FileSystem::BaseName(U".test.TXT"), U".test");
	CHECK_EQ(FileSystem::BaseName(U"path/to/file.pdf"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"./path/./to/file.pdf"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"./path/../to/file.pdf"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"../path/to/file.pdf"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"archive.tar.gz"), U"archive.tar");
	CHECK_EQ(FileSystem::BaseName(U"archive.tar.GZ"), U"archive.tar");
	CHECK_EQ(FileSystem::BaseName(U"file."), U"file.");
	CHECK_EQ(FileSystem::BaseName(U"file.."), U"file..");
	CHECK_EQ(FileSystem::BaseName(U"file..."), U"file...");
	CHECK_EQ(FileSystem::BaseName(U"file.txt."), U"file.txt.");
	CHECK_EQ(FileSystem::BaseName(U"file.txt.."), U"file.txt..");
	CHECK_EQ(FileSystem::BaseName(U"file.txt..."), U"file.txt...");
	CHECK_EQ(FileSystem::BaseName(U"my document.docx"), U"my document");
	CHECK_EQ(FileSystem::BaseName(U".hidden.tar.gz"), U".hidden.tar");
	CHECK_EQ(FileSystem::BaseName(U"path/to/.config.file.dat"), U".config.file");
	CHECK_EQ(FileSystem::BaseName(U"...."), U"....");
	CHECK_EQ(FileSystem::BaseName(U"file.middle."), U"file.middle.");
	CHECK_EQ(FileSystem::BaseName(U"path.to.folder/file"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"C:\\path\\to\\file.docx"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"file.extension-with-special_chars!@#"), U"file");
	CHECK_EQ(FileSystem::BaseName(U"メモ.テキスト"), U"メモ");
	CHECK_EQ(FileSystem::BaseName(U"my file. "), U"my file");
	CHECK_EQ(FileSystem::BaseName(U"file..txt"), U"file.");
	CHECK_EQ(FileSystem::BaseName(U"a.a"), U"a");
	CHECK_EQ(FileSystem::BaseName(U"a.A"), U"a");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example/windmill.png")), U"windmill");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example/windmill")), U"windmill");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example.test/windmill")), U"windmill");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example.test/windmill/")), U"");
	CHECK_EQ(FileSystem::BaseName(Resource(U"")), U"");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example.test/windmill.p")), U"windmill");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example.test/a.b.c/windmill.p")), U"windmill");
	CHECK_EQ(FileSystem::BaseName(Resource(U"example.test/a.b.c/windmill.p.q")), U"windmill.p");
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
