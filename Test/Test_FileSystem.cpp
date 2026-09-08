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

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)
	# include <filesystem>
	# include <fcntl.h>
	# include <sys/stat.h>
	# include <unistd.h>
# endif

# if SIV3D_PLATFORM(MACOS)
	# include <sys/xattr.h>
# endif

TEST_CASE("FileSystem::IsResourcePath")
{
	CHECK_FALSE(FileSystem::IsResourcePath(U""));
	CHECK(FileSystem::IsResourcePath(Resource(U"")));
	CHECK(FileSystem::IsResourcePath(Resource(U"missing-resource/file.txt")));
	CHECK_FALSE(FileSystem::IsResourcePath(Test::OutputPath(U"filesystem/ordinary/file.txt")));
}

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

TEST_CASE("FileSystem::Extension dot files")
{
	const struct
	{
		FilePathView name;
		StringView extension;
		StringView preservedExtension;
	} cases[] = {
		{ U".gitignore", U"", U"" },
		{ U"..gitignore", U"", U"" },
		{ U".test.TXT", U"txt", U"TXT" },
		{ U"...test.TXT", U"txt", U"TXT" },
		{ U".gitignore.", U"", U"" },
		{ U".", U"", U"" },
		{ U"..", U"", U"" },
		{ U"...", U"", U"" },
		{ U"", U"", U"" },
		{ U"plain", U"", U"" },
		{ U"file..TXT", U"txt", U"TXT" },
		{ U".設定.テキスト", U"テキスト", U"テキスト" },
		{ U".hidden/", U"", U"" },
		{ U".hidden\\", U"", U"" },
	};
	const FilePathView prefixes[] = {
		U"", U"./", U"../", U"parent/", U"parent.with.dots/", U"C:\\parent\\", U"parent\\nested/",
	};

	for (const auto& test : cases)
	{
		for (const FilePathView prefix : prefixes)
		{
			const FilePath path = (prefix + test.name);
			CHECK_EQ(FileSystem::Extension(path), test.extension);
			CHECK_EQ(FileSystem::Extension(path, PreserveCase::No), test.extension);
			CHECK_EQ(FileSystem::Extension(path, PreserveCase::Yes), test.preservedExtension);
		}

		const FilePath resourcePath = Resource(test.name);
		CHECK_EQ(FileSystem::Extension(resourcePath), test.extension);
		CHECK_EQ(FileSystem::Extension(resourcePath, PreserveCase::No), test.extension);
		CHECK_EQ(FileSystem::Extension(resourcePath, PreserveCase::Yes), test.preservedExtension);
	}
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
	const ScopeExit restoreDirectory{ [&currentDirectory]
		{
			FileSystem::ChangeCurrentDirectory(currentDirectory);
		} };

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(U"./"), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(currentDirectory), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);

	REQUIRE(FileSystem::ChangeCurrentDirectory(U"example/"));
	CHECK_EQ(FileSystem::CurrentDirectory(), (currentDirectory + U"example/"));

	CHECK_EQ(FileSystem::ChangeCurrentDirectory(U"../"), true);
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);
}

TEST_CASE("FileSystem::RelativePath")
{
	const FilePath root = Test::OutputPath(U"filesystem/relativepath/");
	const FilePath base = (root + U"base/");
	const FilePath other = (root + U"other/");
	REQUIRE(FileSystem::CreateDirectories(base + U"directory/"));
	REQUIRE(FileSystem::CreateDirectories(other + U"file.txt/"));
	REQUIRE(FileSystem::CreateDirectories(other + U"missing/"));
	{
		BinaryFileWriter writer{ base + U"file.txt" };
		REQUIRE(writer.isOpen());
	}

	const FilePath currentDirectory = FileSystem::CurrentDirectory();
	const ScopeExit restoreDirectory{ [&currentDirectory]
		{
			FileSystem::ChangeCurrentDirectory(currentDirectory);
		} };

	for (const FilePath& workingDirectory : { base, other })
	{
		REQUIRE(FileSystem::ChangeCurrentDirectory(workingDirectory));
		CHECK_EQ(FileSystem::RelativePath(base + U"directory", base), U"directory/");
		CHECK_EQ(FileSystem::RelativePath(base + U"file.txt", base), U"file.txt");
		CHECK_EQ(FileSystem::RelativePath(base + U"missing", base), U"missing");
		CHECK_EQ(FileSystem::RelativePath(base, base), U"./");
		CHECK_EQ(FileSystem::FullPath(base + FileSystem::RelativePath(base + U"file.txt", base)),
			FileSystem::FullPath(base + U"file.txt"));
	}

	CHECK_EQ(FileSystem::RelativePath(U"", base), U"");
	CHECK_EQ(FileSystem::RelativePath(base, U""), U"");
}

TEST_CASE("FileSystem::RemoveContents")
{
	const FilePath root = Test::OutputPath(U"filesystem/removecontents/basic/");
	const FilePath directory = (root + U"directory/");
	REQUIRE(FileSystem::CreateDirectories(directory + U"nested/empty/"));
	for (const FilePathView name : { U"file.txt", U".hidden", U"nested/日本語.txt" })
	{
		BinaryFileWriter writer{ directory + name };
		REQUIRE(writer.isOpen());
	}
	{
		BinaryFileWriter writer{ root + U"sibling.txt" };
		REQUIRE(writer.isOpen());
	}

	CHECK(FileSystem::RemoveContents(directory));
	CHECK(FileSystem::IsDirectory(directory));
	CHECK(FileSystem::IsEmptyDirectory(directory));
	CHECK(FileSystem::IsFile(root + U"sibling.txt"));
	CHECK_FALSE(FileSystem::RemoveContents(U""));
	CHECK_FALSE(FileSystem::RemoveContents(root + U"missing"));
	CHECK_FALSE(FileSystem::RemoveContents(root + U"sibling.txt"));
	CHECK(FileSystem::IsFile(root + U"sibling.txt"));
}

# if SIV3D_PLATFORM(MACOS)

TEST_CASE("FileSystem::IsResourcePath directory links")
{
	const FilePath root = Test::OutputPath(U"filesystem/isresourcepath/links/");
	REQUIRE(FileSystem::CreateDirectories(root));
	const FilePath resourceDirectory = Resource(U"");
	REQUIRE(FileSystem::IsDirectory(resourceDirectory));
	const FilePath alias = (root + U"resource-alias");
	REQUIRE(::symlink(Unicode::ToUTF8(resourceDirectory).c_str(), Unicode::ToUTF8(alias).c_str()) == 0);
	CHECK(FileSystem::IsResourcePath(alias));
	CHECK(FileSystem::IsResourcePath(alias + U"/missing-resource/file.txt"));
	CHECK(FileSystem::IsResourcePath(resourceDirectory + U"../Resources/missing-resource/file.txt"));
	CHECK_FALSE(FileSystem::IsResourcePath(FileSystem::GetExecutablePath() + U"/Contents/Resources-other/file.txt"));

	const FilePath ordinaryAlias = (root + U"ordinary-alias");
	REQUIRE(::symlink(".", Unicode::ToUTF8(ordinaryAlias).c_str()) == 0);
	CHECK_FALSE(FileSystem::IsResourcePath(ordinaryAlias));
	const FilePath brokenAlias = (root + U"broken-alias");
	REQUIRE(::symlink("missing-target", Unicode::ToUTF8(brokenAlias).c_str()) == 0);
	CHECK_FALSE(FileSystem::IsResourcePath(brokenAlias));
}

TEST_CASE("FileSystem::IsResourcePath resolution failure")
{
	const FilePath root = Test::OutputPath(U"filesystem/isresourcepath/failure/");
	REQUIRE(FileSystem::CreateDirectories(root));
	const FilePath loop = (root + U"loop");
	REQUIRE(::symlink("loop", Unicode::ToUTF8(loop).c_str()) == 0);

	for (const FilePath& path : { loop, loop + U"/child" })
	{
		CHECK_FALSE(FileSystem::IsResourcePath(path));
	}
}

TEST_CASE("FileSystem::IsResourcePath permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	const FilePath directory = Test::OutputPath(U"filesystem/isresourcepath/locked/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	const std::string native = Unicode::ToUTF8(directory);
	const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
	REQUIRE(::chmod(native.c_str(), 0000) == 0);
	CHECK_FALSE(FileSystem::IsResourcePath(directory + U"child"));
}

# endif

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)

TEST_CASE("FileSystem::RemoveContents preserves directory")
{
	const FilePath directory = Test::OutputPath(U"filesystem/removecontents/metadata/");
	const std::string native = Unicode::ToUTF8(directory);
	REQUIRE(FileSystem::CreateDirectories(directory));
	REQUIRE(::chmod(native.c_str(), 0751) == 0);

	// Keep the original inode alive so deletion and recreation cannot reuse it.
	const int fd = ::open(native.c_str(), O_RDONLY | O_DIRECTORY);
	REQUIRE(fd >= 0);
	const ScopeExit closeDirectory{ [fd] { ::close(fd); } };
	struct stat before{};
	REQUIRE(::fstat(fd, &before) == 0);

# if SIV3D_PLATFORM(MACOS)
	constexpr char attribute[] = "org.siv3d.test.removecontents";
	constexpr char value[] = "metadata";
	REQUIRE(::setxattr(native.c_str(), attribute, value, sizeof(value), 0, 0) == 0);
# endif

	SUBCASE("empty directory") {}
	SUBCASE("populated directory")
	{
		BinaryFileWriter writer{ directory + U"file.txt" };
		REQUIRE(writer.isOpen());
	}

	CHECK(FileSystem::RemoveContents(directory));
	CHECK(FileSystem::IsEmptyDirectory(directory));
	struct stat after{};
	REQUIRE(::stat(native.c_str(), &after) == 0);
	CHECK_EQ(after.st_dev, before.st_dev);
	CHECK_EQ(after.st_ino, before.st_ino);
	CHECK_EQ((after.st_mode & 07777), (before.st_mode & 07777));
# if SIV3D_PLATFORM(MACOS)
	char actual[sizeof(value)]{};
	CHECK_EQ(::getxattr(native.c_str(), attribute, actual, sizeof(actual), 0, 0), sizeof(value));
	CHECK_EQ(std::string_view(actual, sizeof(actual)), std::string_view(value, sizeof(value)));
# endif
}

TEST_CASE("FileSystem::RemoveContents symbolic links")
{
	FilePathView scenario;
	SUBCASE("directory path") { scenario = U"directory"; }
	SUBCASE("directory link") { scenario = U"link"; }
	SUBCASE("directory link with trailing slash") { scenario = U"link-with-slash"; }

	const FilePath root = Test::OutputPath(U"filesystem/removecontents/links/" + scenario + U'/');
	const FilePath directory = (root + U"directory/");
	const FilePath outside = (root + U"outside/");
	REQUIRE(FileSystem::CreateDirectories(directory + U"nested/"));
	REQUIRE(FileSystem::CreateDirectories(outside));
	{
		BinaryFileWriter writer{ outside + U"keep.txt" };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("keep", 4) == 4);
	}
	for (const FilePathView parent : { U"", U"nested/" })
	{
		const std::string prefix = Unicode::ToUTF8(directory + parent);
		REQUIRE(::symlink(Unicode::ToUTF8(outside).c_str(), (prefix + "directory-link").c_str()) == 0);
		REQUIRE(::symlink(Unicode::ToUTF8(outside + U"keep.txt").c_str(), (prefix + "file-link").c_str()) == 0);
		REQUIRE(::symlink("missing-target", (prefix + "broken-link").c_str()) == 0);
		REQUIRE(::symlink(".", (prefix + "cycle").c_str()) == 0);
	}

	FilePath argument = directory;
	if (scenario != U"directory")
	{
		argument = (root + U"alias");
		REQUIRE(::symlink(Unicode::ToUTF8(directory).c_str(), Unicode::ToUTF8(argument).c_str()) == 0);
		if (scenario == U"link-with-slash")
		{
			argument.push_back(U'/');
		}
	}

	CHECK(FileSystem::RemoveContents(argument));
	CHECK(FileSystem::IsEmptyDirectory(directory));
	CHECK(FileSystem::IsFile(outside + U"keep.txt"));
	CHECK_EQ(FileSystem::FileSize(outside + U"keep.txt"), 4);
	if (argument != directory)
	{
		if (argument.ends_with(U'/'))
		{
			argument.pop_back();
		}
		CHECK(std::filesystem::is_symlink(Unicode::ToUTF8(argument)));
	}
}

TEST_CASE("FileSystem::RemoveContents permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	mode_t permissions = 0500;
	SUBCASE("deletion denied") {}
	SUBCASE("enumeration denied") { permissions = 0000; }

	const FilePath directory = Test::OutputPath(U"filesystem/removecontents/locked/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	{
		BinaryFileWriter writer{ directory + U"keep.txt" };
		REQUIRE(writer.isOpen());
	}
	const std::string native = Unicode::ToUTF8(directory);
	const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
	REQUIRE(::chmod(native.c_str(), permissions) == 0);
	CHECK_FALSE(FileSystem::RemoveContents(directory));
	REQUIRE(::chmod(native.c_str(), 0700) == 0);
	CHECK(FileSystem::IsDirectory(directory));
	CHECK(FileSystem::IsFile(directory + U"keep.txt"));
}

TEST_CASE("FileSystem::RemoveContents empty directory with trash")
{
	const FilePath directory = Test::OutputPath(U"filesystem/removecontents/empty-trash/");
	const std::string native = Unicode::ToUTF8(directory);
	REQUIRE(FileSystem::CreateDirectories(directory));
	struct stat before{};
	REQUIRE(::stat(native.c_str(), &before) == 0);
	CHECK(FileSystem::RemoveContents(directory, MoveToTrash::Yes));
	CHECK(FileSystem::IsEmptyDirectory(directory));
	struct stat after{};
	REQUIRE(::stat(native.c_str(), &after) == 0);
	CHECK_EQ(after.st_dev, before.st_dev);
	CHECK_EQ(after.st_ino, before.st_ino);
}

# endif

TEST_CASE("FileSystem Misc")
{
	Console << U"FileSystem";
	Console << U"| CreationTime(GetExecutablePath()):\t" << FileSystem::CreationTime(FileSystem::GetExecutablePath());
	Console << U"| WriteTime(GetExecutablePath()):\t" << FileSystem::WriteTime(FileSystem::GetExecutablePath());
	Console << U"| AccessTime(GetExecutablePath()):\t" << FileSystem::AccessTime(FileSystem::GetExecutablePath());
	Console << U"| GetLaunchDirectory():\t" << FileSystem::GetLaunchDirectory();
	Console << U"| ExecutablePath():\t" << FileSystem::GetExecutablePath();
	Console << U"| ExecutableDirectory():\t" << FileSystem::GetExecutableDirectory();
	Console << U"| CurrentDirectory():\t" << FileSystem::CurrentDirectory();
}
