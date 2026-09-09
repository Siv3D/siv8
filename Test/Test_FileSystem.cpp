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
# include <filesystem>
# include <ctime>

# if SIV3D_PLATFORM(WINDOWS)
	# include <Siv3D/Windows/Windows.hpp>
	# include <winioctl.h>
	# include <clocale>
	# include <Shlobj.h>
	# include "../Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/FileSystem/WindowsFileSystem.hpp"
# endif

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)
	# include <fcntl.h>
	# include <sys/stat.h>
	# include <unistd.h>
# endif

# if SIV3D_PLATFORM(MACOS)
	# include <sys/xattr.h>
# endif

# if SIV3D_PLATFORM(WINDOWS)
// SDK browser declarations must coexist with Siv3D's default using-directive.
static_assert(std::is_class_v<::WebBrowser>);
static_assert(std::is_enum_v<BrowserApp>);

namespace
{
	template <class F>
	void WithOEMFileAPIs(F&& check)
	{
		const char* locale = std::setlocale(LC_CTYPE, nullptr);
		REQUIRE(locale != nullptr);
		const std::string savedLocale = locale;
		const int savedMode = ::_configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
		REQUIRE(savedMode != -1);
		const ScopeExit restoreLocale{ [&savedLocale, savedMode]
			{
				std::setlocale(LC_CTYPE, savedLocale.c_str());
				::_configthreadlocale(savedMode);
			} };
		// A UTF-8 CRT locale takes precedence over the file API code page in MSVC.
		REQUIRE(std::setlocale(LC_CTYPE, "C") != nullptr);
		const BOOL wasANSI = ::AreFileApisANSI();
		const ScopeExit restoreFileAPIs{ [wasANSI]
			{
				if (wasANSI)
				{
					::SetFileApisToANSI();
				}
				else
				{
					::SetFileApisToOEM();
				}
			} };
		::SetFileApisToOEM();
		REQUIRE_FALSE(::AreFileApisANSI());
		if (::GetOEMCP() == CP_UTF8)
		{
			MESSAGE("The system OEM code page is UTF-8; a legacy code page is not exercised on this host.");
		}
		check();
	}
}
# endif

TEST_CASE("FileSystem::Path status")
{
	const FilePath root = Test::OutputPath(U"filesystem/status/");
	REQUIRE(FileSystem::CreateDirectories(root + U"directory/"));
	{
		BinaryFileWriter writer{ root + U"日本語.txt" };
		REQUIRE(writer.isOpen());
	}
	const Array<FilePath>& resources = EnumResourceFiles();
	REQUIRE_FALSE(resources.isEmpty());
	const struct
	{
		FilePath path;
		bool exists;
		bool isDirectory;
		bool isFile;
	} cases[] = {
		{ root, true, true, false },
		{ root + U"directory", true, true, false },
		{ root + U"directory/", true, true, false },
		{ root + U"日本語.txt", true, false, true },
		{ root + U"missing", false, false, false },
		{ root + U"missing/child", false, false, false },
		{ root + U"日本語.txt/child", false, false, false },
		{ U"", false, false, false },
		{ resources.front(), true, false, true },
		{ Resource(U"missing-resource/file.txt"), false, false, false },
	};
	for (const auto& test : cases)
	{
		CAPTURE(test.path);
		CHECK_EQ(FileSystem::Exists(test.path), test.exists);
		CHECK_EQ(FileSystem::IsDirectory(test.path), test.isDirectory);
		CHECK_EQ(FileSystem::IsFile(test.path), test.isFile);
	}
}

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

TEST_CASE("FileSystem::FileName and BaseName path views")
{
	const struct
	{
		FilePathView name;
		FilePathView baseName;
	} cases[] = {
		{ U"", U"" },
		{ U".", U"." },
		{ U"..", U".." },
		{ U"....", U"...." },
		{ U"file", U"file" },
		{ U"file.", U"file." },
		{ U"file..", U"file.." },
		{ U"file.txt.", U"file.txt." },
		{ U"archive.tar.gz", U"archive.tar" },
		{ U".gitignore", U".gitignore" },
		{ U".config.json", U".config" },
		{ U"日本語-😀.PNG", U"日本語-😀" },
	};
	const Array<FilePath> prefixes = {
		U"", U"/", U"\\", U"parent/", U"parent\\", U"mixed/parent\\nested/",
		String(4096, U'a') + U"/parent\\", Resource(U""), Resource(U"example/"),
	};
	for (const auto& prefix : prefixes)
	{
		CAPTURE(prefix);
		for (const auto& test : cases)
		{
			CAPTURE(test.name);
			const FilePath path = (prefix + test.name);
			// Neither end of the view coincides with an end of the backing string.
			const String storage = (U"ignored/" + path + U".ignored/suffix");
			const FilePathView view{ (storage.data() + 8), path.size() };
			CHECK_EQ(FileSystem::FileName(view), test.name);
			CHECK_EQ(FileSystem::BaseName(view), test.baseName);
			for (const char32 separator : { U'/', U'\\' })
			{
				CHECK(FileSystem::FileName(path + separator).isEmpty());
				CHECK(FileSystem::BaseName(path + separator).isEmpty());
			}
		}
	}
	CHECK(FileSystem::FileName(FilePathView{}).isEmpty());
	CHECK(FileSystem::BaseName(FilePathView{}).isEmpty());
}

TEST_CASE("FileSystem::ParentPath")
{
	const FilePath root = Test::OutputPath(U"filesystem/parentpath/");
	REQUIRE(FileSystem::CreateDirectories(root + U"a/b/"));
	const struct
	{
		FilePath path;
		size_t level;
		FilePath expected;
	} cases[] = {
		{ U"", 0, U"" },
		{ root + U"a/b/file.txt", 0, root + U"a/b/" },
		{ root + U"a/b/file.txt", 1, root + U"a/" },
		{ root + U"a/b/file.txt", 2, root },
		{ root + U"a/b", 0, root + U"a/" },
		{ root + U"a/b/", 0, root + U"a/" },
		{ root + U"a/b/", 1, root },
		{ root, std::numeric_limits<size_t>::max(), U"" },
	};
	for (const auto& test : cases)
	{
		CAPTURE(test.path);
		CAPTURE(test.level);
		const FilePath fullPath = FileSystem::FullPath(test.path);
		FilePath base = U"previous value";
		CHECK_EQ(FileSystem::ParentPath(test.path, test.level), test.expected);
		CHECK_EQ(FileSystem::ParentPath(test.path, test.level, base), test.expected);
		CHECK_EQ(base, fullPath);
		// The input may refer to the output string reused by the caller.
		base = test.path;
		CHECK_EQ(FileSystem::ParentPath(base, test.level, base), test.expected);
		CHECK_EQ(base, fullPath);
	}
}

TEST_CASE("FileSystem::File times missing paths")
{
	for (const FilePath& path : { FilePath{}, Test::OutputPath(U"filesystem/times/missing") })
	{
		CAPTURE(path);
		CHECK_FALSE(FileSystem::CreationTime(path));
		CHECK_FALSE(FileSystem::WriteTime(path));
		CHECK_FALSE(FileSystem::AccessTime(path));
	}
}

# if SIV3D_PLATFORM(WINDOWS)

TEST_CASE("FileSystem::Downloads folder")
{
	PWSTR native = nullptr;
	const HRESULT result = ::SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &native);
	const ScopeExit freePath{ [native] { ::CoTaskMemFree(native); } };
	const FilePath& actual = FileSystem::GetFolderPath(SpecialFolder::Downloads);
	if (FAILED(result))
	{
		CHECK(actual.isEmpty());
	}
	else
	{
		FilePath expected = Unicode::FromWstring(native).replaced(U'\\', U'/');
		if (not expected.ends_with(U'/'))
		{
			expected.push_back(U'/');
		}
		CHECK_EQ(actual, expected);
	}
}

TEST_CASE("FileSystem::File time conversion")
{
	const FilePath path = Test::OutputPath(U"filesystem/times/日本語-😀.bin");
	{
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
	}
	for (const WORD month : { WORD{ 1 }, WORD{ 7 } })
	{
		for (const WORD milliseconds : { WORD{ 0 }, WORD{ 123 }, WORD{ 999 } })
		{
			const SYSTEMTIME utc{ 2024, month, 0, 15, 12, 34, 56, milliseconds };
			FILETIME fileTime;
			SYSTEMTIME local;
			REQUIRE(::SystemTimeToFileTime(&utc, &fileTime));
			REQUIRE(::SystemTimeToTzSpecificLocalTimeEx(nullptr, &utc, &local));
			const DateTime expected{ local.wYear, local.wMonth, local.wDay,
				local.wHour, local.wMinute, local.wSecond, local.wMilliseconds };
			CHECK_EQ(detail::FileTimeToTime(fileTime), expected);
			{
				const HANDLE handle = ::CreateFileW(Unicode::ToWstring(path).c_str(), FILE_WRITE_ATTRIBUTES,
					(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE), nullptr, OPEN_EXISTING, 0, nullptr);
				REQUIRE(handle != INVALID_HANDLE_VALUE);
				const ScopeExit closeHandle{ [handle] { ::CloseHandle(handle); } };
				REQUIRE(::SetFileTime(handle, &fileTime, &fileTime, &fileTime));
			}
			CHECK_EQ(FileSystem::CreationTime(path), expected);
			CHECK_EQ(FileSystem::WriteTime(path), expected);
			CHECK_EQ(FileSystem::AccessTime(path), expected);
		}
	}
	for (const DWORD high : { DWORD{ 0xC0000000 }, DWORD{ 0xFFFFFFFF } })
	{
		CHECK_FALSE(detail::FileTimeToTime(FILETIME{ 0, high }));
	}
}

# endif

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)

TEST_CASE("FileSystem::File time conversion")
{
	const FilePath path = Test::OutputPath(U"filesystem/times/日本語-😀.bin");
	{
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
	}
	const std::string native = Unicode::ToUTF8(path);
	for (const time_t seconds : { time_t{ 0 }, time_t{ 1705322096 }, time_t{ 1721046896 } })
	{
		::tm local;
		REQUIRE(::localtime_r(&seconds, &local) != nullptr);
		for (const long nanoseconds : { 0L, 123456789L, 999999999L })
		{
			const ::timespec times[] = { { seconds, nanoseconds }, { seconds, nanoseconds } };
			REQUIRE(::utimensat(AT_FDCWD, native.c_str(), times, 0) == 0);
			const DateTime expected{ local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
				local.tm_hour, local.tm_min, local.tm_sec, static_cast<int32>(nanoseconds / 1'000'000) };
			CHECK_EQ(FileSystem::WriteTime(path), expected);
			CHECK_EQ(FileSystem::AccessTime(path), expected);
		}
	}
	struct stat status;
	REQUIRE(::stat(native.c_str(), &status) == 0);
	# if SIV3D_PLATFORM(MACOS)
		const ::timespec created = status.st_birthtimespec;
	# else
		const ::timespec created = status.st_ctim;
	# endif
	::tm local;
	REQUIRE(::localtime_r(&created.tv_sec, &local) != nullptr);
	const DateTime expected{ local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
		local.tm_hour, local.tm_min, local.tm_sec, static_cast<int32>(created.tv_nsec / 1'000'000) };
	CHECK_EQ(FileSystem::CreationTime(path), expected);
}

# endif

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

TEST_CASE("FileSystem::CurrentDirectory Unicode")
{
	const FilePath currentDirectory = FileSystem::CurrentDirectory();
	const ScopeExit restoreDirectory{ [&currentDirectory]
		{
			CHECK(FileSystem::ChangeCurrentDirectory(currentDirectory));
		} };
	const FilePath directory = Test::OutputPath(U"filesystem/currentdirectory/日本語-😀/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	REQUIRE(FileSystem::ChangeCurrentDirectory(directory));
	CHECK_EQ(FileSystem::CurrentDirectory(), directory);
	CHECK_EQ(FileSystem::RelativePath(directory), U"./");
}

# if SIV3D_PLATFORM(MACOS) || SIV3D_PLATFORM(LINUX)

TEST_CASE("FileSystem::CurrentDirectory removed directory")
{
	const FilePath currentDirectory = FileSystem::CurrentDirectory();
	const FilePath directory = Test::OutputPath(U"filesystem/currentdirectory/removed/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	{
		const ScopeExit restoreDirectory{ [&currentDirectory]
			{
				CHECK(FileSystem::ChangeCurrentDirectory(currentDirectory));
			} };
		REQUIRE(FileSystem::ChangeCurrentDirectory(directory));
		REQUIRE(::rmdir(Unicode::ToUTF8(directory).c_str()) == 0);
		std::error_code error;
		(void)std::filesystem::current_path(error);
		REQUIRE(error);

		FilePath result = U"unchanged";
		CHECK_NOTHROW(result = FileSystem::CurrentDirectory());
		CHECK(result.isEmpty());
		result = U"unchanged";
		CHECK_NOTHROW(result = FileSystem::RelativePath(currentDirectory));
		CHECK(result.isEmpty());
	}
	CHECK_EQ(FileSystem::CurrentDirectory(), currentDirectory);
}

# endif

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

TEST_CASE("FileSystem::RelativePath Unicode")
{
	const FilePath root = Test::OutputPath(U"filesystem/relativepath-unicode/日本語-😀/");
	const FilePath base = (root + U"基準-😀/");
	const FilePath other = (root + U"別-📂/");
	REQUIRE(FileSystem::CreateDirectories(base + U"資料/"));
	REQUIRE(FileSystem::CreateDirectories(other));
	for (const auto& file : { base + U"資料/画像-🐈.bin", other + U"文書.bin" })
	{
		BinaryFileWriter writer{ file };
		REQUIRE(writer.isOpen());
	}
	const struct
	{
		FilePath path;
		FilePath start;
		FilePath expected;
	} cases[] = {
		{ base + U"資料/画像-🐈.bin", base, U"資料/画像-🐈.bin" },
		{ base + U"資料", base, U"資料/" },
		{ other + U"文書.bin", base, U"../別-📂/文書.bin" },
		{ base + U"未作成/想定-🐈.bin", base, U"未作成/想定-🐈.bin" },
		{ base, base, U"./" },
		{ root, base, U"../" },
		{ other + U"文書.bin", base + U"存在しない/", other + U"文書.bin" },
		{ other + U"文書.bin", base + U"資料/画像-🐈.bin", other + U"文書.bin" },
	};
	const auto check = [&cases]
		{
			for (const auto& test : cases)
			{
				CAPTURE(test.path);
				CAPTURE(test.start);
				FilePath relative;
				CHECK_NOTHROW(relative = FileSystem::RelativePath(test.path, test.start));
				CHECK_EQ(relative, test.expected);
				if (FileSystem::IsDirectory(test.start))
				{
					CHECK_EQ(FileSystem::FullPath(test.start + relative), FileSystem::FullPath(test.path));
				}
			}
		};
	check();
# if SIV3D_PLATFORM(WINDOWS)
	WithOEMFileAPIs(check);
# endif
}

TEST_CASE("FileSystem::FullPath normal paths")
{
	const FilePath root = Test::OutputPath(U"filesystem/fullpath/normal/");
	REQUIRE(FileSystem::CreateDirectories(root + U"nested/"));
	{
		BinaryFileWriter writer{ root + U"file.txt" };
		REQUIRE(writer.isOpen());
	}
	const struct
	{
		FilePathView path;
		FilePathView expected;
	} cases[] = {
		{ U"", U"" },
		{ U"nested", U"nested/" },
		{ U"nested/../file.txt", U"file.txt" },
		{ U"missing/child.txt", U"missing/child.txt" },
	};
	for (const auto& test : cases)
	{
		const FilePath path = (root + test.path);
		const FilePath expected = (root + test.expected);
		CHECK_EQ(FileSystem::FullPath(path), expected);
		const NativeFilePath native = FileSystem::NativePath(path);
# if SIV3D_PLATFORM(WINDOWS)
		CHECK_EQ(FileSystem::FullPath(Unicode::FromWstring(native)), expected);
# else
		CHECK_EQ(FileSystem::FullPath(Unicode::FromUTF8(native)), expected);
# endif
	}
	CHECK(FileSystem::FullPath(U"").isEmpty());
	CHECK(FileSystem::NativePath(U"").empty());
	CHECK_EQ(FileSystem::RelativePath(root + U"file.txt", root + U"missing/"), root + U"file.txt");
	CHECK_EQ(FileSystem::RelativePath(root + U"nested", root + U"file.txt"), root + U"nested/");

	const FilePath currentDirectory = FileSystem::CurrentDirectory();
	const ScopeExit restoreDirectory{ [&currentDirectory]
		{
			FileSystem::ChangeCurrentDirectory(currentDirectory);
		} };
	REQUIRE(FileSystem::ChangeCurrentDirectory(root));
	CHECK_EQ(FileSystem::FullPath(U"nested/../file.txt"), root + U"file.txt");
}

# if SIV3D_PLATFORM(WINDOWS)

TEST_CASE("FileSystem::NativePath buffer boundaries")
{
	// NativePath resolves these paths lexically; no files or drive C: are required.
	const std::wstring prefixes[] = { L"C:\\", Unicode::ToWstring(U"C:\\設定\\😀\\") };
	for (const auto& prefix : prefixes)
	{
		// Lengths are UTF-16 code units, excluding the terminating NUL.
		for (const size_t length : { 318, 319, 320, 321, 322, 1023, 1024, 2048 })
		{
			CAPTURE(length);
			std::wstring expected = prefix;
			while ((expected.size() + 65) < length)
			{
				expected.append(64, L'a');
				expected.push_back(L'\\');
			}
			expected.append((length - expected.size()), L'b');
			const FilePath path = Unicode::FromWstring(expected);
			CHECK_EQ(FileSystem::NativePath(path), expected);
			CHECK_EQ(FileSystem::NativePath(path.replaced(U'\\', U'/')), expected);
		}
	}
}

TEST_CASE("FileSystem::Directory traversal sharing failure")
{
	for (const bool populated : { false, true })
	{
		CAPTURE(populated);
		const FilePath root = Test::OutputPath(populated
			? U"filesystem/traversal-error/populated/" : U"filesystem/traversal-error/empty/");
		const FilePath locked = (root + U"locked/");
		REQUIRE(FileSystem::CreateDirectories(locked));
		{
			BinaryFileWriter writer{ root + U"file.bin" };
			REQUIRE(writer.isOpen());
			REQUIRE(writer.write("data", 4) == 4);
		}
		if (populated)
		{
			BinaryFileWriter writer{ locked + U"inside.bin" };
			REQUIRE(writer.isOpen());
			REQUIRE(writer.write("data", 4) == 4);
		}

		std::wstring native = FileSystem::NativePath(locked);
		REQUIRE(native.ends_with(L'\\'));
		native.pop_back();
		{
			// An exclusive directory handle denies enumeration without changing ACLs.
			const HANDLE handle = ::CreateFileW(native.c_str(), GENERIC_READ, 0, nullptr,
				OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
			REQUIRE(handle != INVALID_HANDLE_VALUE);
			const ScopeExit closeHandle{ [handle] { ::CloseHandle(handle); } };
			WIN32_FIND_DATAW data;
			const std::wstring pattern = (native + L"\\*");
			const HANDLE probe = ::FindFirstFileW(pattern.c_str(), &data);
			const DWORD error = ::GetLastError();
			const ScopeExit closeProbe{ [probe]
				{
					if (probe != INVALID_HANDLE_VALUE)
					{
						::FindClose(probe);
					}
				} };
			CAPTURE(error);
			REQUIRE(probe == INVALID_HANDLE_VALUE);
			REQUIRE(error == ERROR_SHARING_VIOLATION);

			uint64 size = 123;
			CHECK_NOTHROW(size = FileSystem::Size(root));
			CHECK_EQ(size, 0);
			Array<FilePath> paths{ U"unchanged" };
			CHECK_NOTHROW(paths = FileSystem::DirectoryContents(root));
			CHECK(paths.isEmpty());
			CHECK_EQ(FileSystem::DirectoryContents(root, Recursive::No).sorted(),
				(Array<FilePath>{ root + U"file.bin", locked }.sorted()));
		}

		CHECK_EQ(FileSystem::Size(root), (populated ? 8 : 4));
		Array<FilePath> expected{ root + U"file.bin", locked };
		if (populated)
		{
			expected << (locked + U"inside.bin");
		}
		CHECK_EQ(FileSystem::DirectoryContents(root).sorted(), expected.sorted());
	}
}

TEST_CASE("FileSystem::Directory traversal junctions")
{
	const FilePath root = Test::OutputPath(U"filesystem/junctions/");
	const FilePath directory = (root + U"directory/");
	const FilePath outside = (root + U"outside/");
	const FilePath alias = (root + U"alias-日本語-😀");
	REQUIRE(FileSystem::CreateDirectories(directory + U"nested/empty/"));
	REQUIRE(FileSystem::CreateDirectories(outside));
	for (const auto& [path, contents] : Array<std::pair<FilePath, std::string>>{
		{ directory + U"file.bin", "abc" }, { directory + U"nested/file.bin", "12345" },
		{ outside + U"outside.bin", "0123456789" } })
	{
		BinaryFileWriter writer{ path };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write(contents.data(), contents.size()) == contents.size());
	}

	const FilePath links[] = {
		directory + U"external", directory + U"cycle", directory + U"nested/parent",
		directory + U"broken", alias, root + U"self",
	};
	const FilePath targets[] = { outside, directory, directory, outside + U"missing", directory, root + U"self" };
	Array<NativeFilePath> nativeLinks;
	for (const auto& link : links)
	{
		nativeLinks << FileSystem::NativePath(link);
	}
	// Remove the junctions themselves before the runner cleans up the fixture tree.
	const ScopeExit removeJunctions{ [&nativeLinks]
		{
			for (const auto& link : nativeLinks)
			{
				::RemoveDirectoryW(link.c_str());
			}
		} };

	for (size_t i = 0; i < std::size(links); ++i)
	{
		CAPTURE(i);
		std::wstring target = FileSystem::NativePath(targets[i]);
		if (target.ends_with(L'\\'))
		{
			target.pop_back();
		}
		// Junction targets use an absolute local NT path.
		REQUIRE(target.size() >= 3);
		REQUIRE(target[1] == L':');
		const std::wstring substitute = (L"\\??\\" + target);
		// MountPointReparseBuffer layout from REPARSE_DATA_BUFFER, without a WDK dependency.
		struct MountPointBuffer
		{
			DWORD tag;
			WORD dataLength;
			WORD reserved;
			WORD substituteOffset;
			WORD substituteLength;
			WORD printOffset;
			WORD printLength;
			wchar_t paths[8192 - 8];
		} buffer{};
		static_assert(offsetof(MountPointBuffer, paths) == 16);
		static_assert(sizeof(MountPointBuffer) == MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
		REQUIRE((substitute.size() + target.size() + 2) <= std::size(buffer.paths));
		buffer.tag = IO_REPARSE_TAG_MOUNT_POINT;
		buffer.substituteLength = static_cast<WORD>(substitute.size() * sizeof(wchar_t));
		buffer.printOffset = static_cast<WORD>(buffer.substituteLength + sizeof(wchar_t));
		buffer.printLength = static_cast<WORD>(target.size() * sizeof(wchar_t));
		buffer.dataLength = static_cast<WORD>(8 + buffer.printOffset + buffer.printLength + sizeof(wchar_t));
		std::copy(substitute.begin(), substitute.end(), buffer.paths);
		std::copy(target.begin(), target.end(), (buffer.paths + substitute.size() + 1));

		REQUIRE(::CreateDirectoryW(nativeLinks[i].c_str(), nullptr) != 0);
		const HANDLE handle = ::CreateFileW(nativeLinks[i].c_str(), GENERIC_WRITE,
			(FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE), nullptr, OPEN_EXISTING,
			(FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS), nullptr);
		REQUIRE(handle != INVALID_HANDLE_VALUE);
		const ScopeExit closeHandle{ [handle] { ::CloseHandle(handle); } };
		DWORD returned = 0;
		const BOOL created = ::DeviceIoControl(handle, FSCTL_SET_REPARSE_POINT,
			&buffer, (8 + buffer.dataLength), nullptr, 0, &returned, nullptr);
		const DWORD error = created ? ERROR_SUCCESS : ::GetLastError();
		CAPTURE(error);
		REQUIRE(created != 0);
	}

	for (const FilePathView suffix : { U"", U"/child" })
	{
		const FilePath path = (root + U"self" + suffix);
		CAPTURE(path);
		std::error_code error;
		const auto status = std::filesystem::status(Unicode::ToWstring(path), error);
		CAPTURE(error.value());
		REQUIRE(error);
		if (suffix.isEmpty())
		{
			// Keep a strict regression check for an attribute error on the link itself.
			REQUIRE(status.type() == std::filesystem::file_type::none);
		}
		else
		{
			// Windows may report a child of the cyclic link as missing instead.
			REQUIRE((status.type() == std::filesystem::file_type::none
				|| status.type() == std::filesystem::file_type::not_found));
		}
		for (const auto query : { FileSystem::Exists, FileSystem::IsDirectory, FileSystem::IsFile })
		{
			bool result = true;
			CHECK_NOTHROW(result = query(path));
			CHECK_FALSE(result);
		}
		uint64 size = 123;
		CHECK_NOTHROW(size = FileSystem::Size(path));
		CHECK_EQ(size, 0);
		Array<FilePath> paths{ U"unchanged" };
		CHECK_NOTHROW(paths = FileSystem::DirectoryContents(path));
		CHECK(paths.isEmpty());
		FilePath fullPath = U"unchanged";
		CHECK_NOTHROW(fullPath = FileSystem::FullPath(path));
		if (status.type() == std::filesystem::file_type::none)
		{
			CHECK(fullPath.isEmpty());
		}
		else
		{
			CHECK_EQ(fullPath, path);
		}
		CHECK_FALSE(FileSystem::NativePath(path).empty());
	}

	// Canonicalization must find the Unicode junction, including under a legacy code page.
	// A merely lexical round trip can hide a misdecoded path that was never resolved.
	const auto checkRelativePath = [&]
		{
			FilePath relative;
			CHECK_NOTHROW(relative = FileSystem::RelativePath(alias + U"/file.bin", root));
			CHECK_EQ(relative, U"directory/file.bin");
			CHECK_NOTHROW(relative = FileSystem::RelativePath(directory + U"file.bin", alias));
			CHECK_EQ(relative, U"file.bin");
		};
	checkRelativePath();
	WithOEMFileAPIs(checkRelativePath);

	for (const auto& base : { directory, (alias + U'/') })
	{
		CAPTURE(base);
		CHECK_EQ(FileSystem::Size(base), 8);
		for (const Recursive recursive : { Recursive::No, Recursive::Yes })
		{
			Array<FilePath> expected{
				base + U"file.bin", base + U"nested/", base + U"external/", base + U"cycle/", base + U"broken/",
			};
			if (recursive)
			{
				expected << (base + U"nested/file.bin") << (base + U"nested/empty/") << (base + U"nested/parent/");
			}
			CHECK_EQ(FileSystem::DirectoryContents(base, recursive).sorted(), expected.sorted());
		}
	}
	CHECK_EQ(FileSystem::Size(links[0]), 10);
	CHECK_EQ(FileSystem::DirectoryContents(links[0]), Array<FilePath>{ links[0] + U"/outside.bin" });
	CHECK_EQ(FileSystem::Size(outside), 10);
}

# endif

TEST_CASE("FileSystem::DirectoryContents normal paths")
{
	const FilePath root = Test::OutputPath(U"filesystem/directorycontents/normal/");
	REQUIRE(FileSystem::CreateDirectories(root + U"nested/empty/"));
	for (const FilePathView name : { U"file.txt", U".hidden", U"nested/inside.txt" })
	{
		BinaryFileWriter writer{ root + name };
		REQUIRE(writer.isOpen());
	}
	for (const Recursive recursive : { Recursive::No, Recursive::Yes })
	{
		Array<FilePath> expected{ root + U"file.txt", root + U".hidden", root + U"nested/" };
		if (recursive)
		{
			expected.push_back(root + U"nested/empty/");
			expected.push_back(root + U"nested/inside.txt");
		}
		Array<FilePath> actual = FileSystem::DirectoryContents(root, recursive);
		CHECK_EQ(actual.sort(), expected.sort());
		CHECK(FileSystem::DirectoryContents(root + U"nested/empty/", recursive).isEmpty());
		CHECK(FileSystem::DirectoryContents(root + U"file.txt", recursive).isEmpty());
		CHECK(FileSystem::DirectoryContents(root + U"missing", recursive).isEmpty());
		CHECK(FileSystem::DirectoryContents(U"", recursive).isEmpty());
	}
}

TEST_CASE("FileSystem::Size")
{
	const FilePath root = Test::OutputPath(U"filesystem/size/normal/");
	REQUIRE(FileSystem::CreateDirectories(root + U"nested/empty/"));
	const struct
	{
		FilePathView path;
		std::string_view contents;
	} files[] = {
		{ U"file.bin", "abcd" },
		{ U".hidden", "ef" },
		{ U"nested/日本語.bin", "1234567" },
		{ U"empty.bin", "" },
	};
	for (const auto& file : files)
	{
		{
			BinaryFileWriter writer{ root + file.path };
			REQUIRE(writer.isOpen());
			REQUIRE(writer.write(file.contents.data(), file.contents.size()) == file.contents.size());
		}
		CHECK_EQ(FileSystem::Size(root + file.path), file.contents.size());
	}
	CHECK_EQ(FileSystem::Size(root), 13);
	CHECK_EQ(FileSystem::Size(root + U"nested"), 7);
	CHECK_EQ(FileSystem::Size(root + U"nested/empty/"), 0);
	CHECK_EQ(FileSystem::Size(root + U"missing"), 0);
	CHECK_EQ(FileSystem::Size(U""), 0);
}

TEST_CASE("FileSystem::IsEmptyDirectory")
{
	const FilePath root = Test::OutputPath(U"filesystem/isemptydirectory/normal/");
	REQUIRE(FileSystem::CreateDirectories(root + U"empty/"));
	REQUIRE(FileSystem::CreateDirectories(root + U"directories/child/"));
	{
		BinaryFileWriter writer{ root + U"hidden/.hidden" };
		REQUIRE(writer.isOpen());
	}
	const struct
	{
		FilePath path;
		bool expected;
	} cases[] = {
		{ root + U"empty/", true },
		{ root + U"empty", true },
		{ root + U"directories/", false },
		{ root + U"hidden/", false },
		{ root + U"hidden/.hidden", false },
		{ root + U"missing/", false },
		{ U"", false },
		{ Resource(U"missing-resource/"), false },
	};
	for (const auto& test : cases)
	{
		CHECK_EQ(FileSystem::IsEmptyDirectory(test.path), test.expected);
	}
}

TEST_CASE("FileSystem::CreateParentDirectories")
{
	const FilePath root = Test::OutputPath(U"filesystem/createparent/normal/");
	const FilePath target = (root + U"nested/child/file.txt");
	CHECK(FileSystem::CreateParentDirectories(target));
	CHECK(FileSystem::IsDirectory(root + U"nested/child/"));
	CHECK_FALSE(FileSystem::Exists(target));
	CHECK(FileSystem::CreateParentDirectories(target));
	CHECK_FALSE(FileSystem::Exists(target));
	CHECK_FALSE(FileSystem::CreateParentDirectories(U""));
	CHECK_FALSE(FileSystem::CreateParentDirectories(Resource(U"missing-resource/file.txt")));

	const FilePath volume = FileSystem::VolumePath(root);
	REQUIRE(not volume.isEmpty());
	CHECK(FileSystem::CreateParentDirectories(volume));

	const FilePath currentDirectory = FileSystem::CurrentDirectory();
	const ScopeExit restoreDirectory{ [&currentDirectory]
		{
			FileSystem::ChangeCurrentDirectory(currentDirectory);
		} };
	REQUIRE(FileSystem::ChangeCurrentDirectory(root));
	CHECK(FileSystem::CreateParentDirectories(U"plain.txt"));
	CHECK_FALSE(FileSystem::Exists(root + U"plain.txt"));
}

TEST_CASE("FileSystem::CreateParentDirectories file collision")
{
	const FilePath root = Test::OutputPath(U"filesystem/createparent/collision/");
	const FilePath blocker = (root + U"file");
	{
		BinaryFileWriter writer{ blocker };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("keep", 4) == 4);
	}
	for (const FilePath& target : { blocker + U"/child.txt", blocker + U"/nested/child.txt" })
	{
		CHECK_FALSE(FileSystem::CreateParentDirectories(target));
	}
	BinaryFileReader reader{ blocker };
	char contents[4]{};
	REQUIRE(reader.read(contents, sizeof(contents)) == sizeof(contents));
	CHECK_EQ(std::string_view(contents, sizeof(contents)), "keep");
}

TEST_CASE("FileSystem::Copy parent directories")
{
	const FilePath root = Test::OutputPath(U"filesystem/copy/parents/");
	const FilePath source = (root + U"source.bin");
	const FilePath destination = (root + U"nested/child/copy.bin");
	{
		BinaryFileWriter writer{ source };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("data", 4) == 4);
	}
	CHECK(FileSystem::Copy(source, destination));
	{
		BinaryFileReader reader{ destination };
		char contents[4]{};
		REQUIRE(reader.read(contents, sizeof(contents)) == sizeof(contents));
		CHECK_EQ(std::string_view(contents, sizeof(contents)), "data");
	}
	CHECK_FALSE(FileSystem::Copy(source, source + U"/child.bin"));
	CHECK_EQ(FileSystem::FileSize(source), 4);
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

TEST_CASE("FileSystem::FullPath directory links")
{
	const FilePath root = Test::OutputPath(U"filesystem/fullpath/links/");
	const FilePath directory = (root + U"directory/");
	const FilePath alias = (root + U"alias");
	REQUIRE(FileSystem::CreateDirectories(directory + U"nested/"));
	REQUIRE(::symlink(Unicode::ToUTF8(directory).c_str(), Unicode::ToUTF8(alias).c_str()) == 0);
	CHECK_EQ(FileSystem::FullPath(alias), directory);
	CHECK_EQ(FileSystem::FullPath(alias + U"/missing.txt"), directory + U"missing.txt");
	CHECK_EQ(FileSystem::FullPath(Unicode::FromUTF8(FileSystem::NativePath(alias))), directory);
	for (const Recursive recursive : { Recursive::No, Recursive::Yes })
	{
		CHECK_EQ(FileSystem::DirectoryContents(alias, recursive), Array<FilePath>{ directory + U"nested/" });
	}
}

TEST_CASE("FileSystem::DirectoryContents resolution failure")
{
	const FilePath root = Test::OutputPath(U"filesystem/directorycontents/failure/");
	REQUIRE(FileSystem::CreateDirectories(root + U"nested/"));
	{
		BinaryFileWriter writer{ root + U"ordinary.txt" };
		REQUIRE(writer.isOpen());
	}
	REQUIRE(::symlink("loop", Unicode::ToUTF8(root + U"loop").c_str()) == 0);
	for (const Recursive recursive : { Recursive::No, Recursive::Yes })
	{
		Array<FilePath> paths{ U"unchanged" };
		CHECK_NOTHROW(paths = FileSystem::DirectoryContents(root, recursive));
		CHECK(paths.isEmpty());
		CHECK_NOTHROW(paths = FileSystem::DirectoryContents(root + U"loop", recursive));
		CHECK(paths.isEmpty());
	}
}

TEST_CASE("FileSystem::DirectoryContents permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	const FilePath root = Test::OutputPath(U"filesystem/directorycontents/permissions/");
	const FilePath directory = (root + U"locked/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	const std::string native = Unicode::ToUTF8(directory);
	const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
	REQUIRE(::chmod(native.c_str(), 0000) == 0);
	for (const Recursive recursive : { Recursive::No, Recursive::Yes })
	{
		Array<FilePath> paths{ U"unchanged" };
		CHECK_NOTHROW(paths = FileSystem::DirectoryContents(directory, recursive));
		CHECK(paths.isEmpty());
	}
	Array<FilePath> paths{ U"unchanged" };
	CHECK_NOTHROW(paths = FileSystem::DirectoryContents(root, Recursive::Yes));
	CHECK(paths.isEmpty());
	CHECK_NOTHROW(paths = FileSystem::DirectoryContents(root, Recursive::No));
	CHECK_EQ(paths, Array<FilePath>{ directory });
}

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

TEST_CASE("FileSystem::Size attribute failure")
{
	FilePathView scenario;
	SUBCASE("broken link") { scenario = U"broken"; }
	SUBCASE("cyclic link") { scenario = U"cycle"; }
	const FilePath root = Test::OutputPath(U"filesystem/size/attribute/" + scenario + U'/');
	REQUIRE(FileSystem::CreateDirectories(root));
	{
		BinaryFileWriter writer{ root + U"file.bin" };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("data", 4) == 4);
	}
	REQUIRE(::symlink((scenario == U"broken") ? "missing-target" : "link",
		Unicode::ToUTF8(root + U"link").c_str()) == 0);
	uint64 size = 123;
	CHECK_NOTHROW(size = FileSystem::Size(root));
	CHECK_EQ(size, 0);
	CHECK_EQ(FileSystem::Size(root + U"file.bin"), 4);
	CHECK_EQ(FileSystem::Size(root + U"link"), 0);
}

TEST_CASE("FileSystem::Size permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	const FilePath root = Test::OutputPath(U"filesystem/size/permissions/");
	const FilePath directory = (root + U"locked/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	for (const FilePath& file : { root + U"file.bin", directory + U"inside.bin" })
	{
		BinaryFileWriter writer{ file };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("data", 4) == 4);
	}
	const std::string native = Unicode::ToUTF8(directory);
	const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
	REQUIRE(::chmod(native.c_str(), 0000) == 0);
	for (const FilePath& path : { directory, root })
	{
		uint64 size = 123;
		CHECK_NOTHROW(size = FileSystem::Size(path));
		CHECK_EQ(size, 0);
	}
	REQUIRE(::chmod(native.c_str(), 0500) == 0);
	CHECK_EQ(FileSystem::Size(directory), 4);
	CHECK_EQ(FileSystem::Size(root), 8);
}

TEST_CASE("FileSystem::Size symbolic links")
{
	const FilePath root = Test::OutputPath(U"filesystem/size/links/");
	const FilePath directory = (root + U"directory/");
	const FilePath outside = (root + U"outside/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	{
		BinaryFileWriter writer{ directory + U"file.bin" };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("abc", 3) == 3);
	}
	{
		BinaryFileWriter writer{ outside + U"file.bin" };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("abcde", 5) == 5);
	}
	const FilePath alias = (root + U"alias");
	REQUIRE(::symlink(Unicode::ToUTF8(directory).c_str(), Unicode::ToUTF8(alias).c_str()) == 0);
	REQUIRE(::symlink(Unicode::ToUTF8(outside + U"file.bin").c_str(), Unicode::ToUTF8(directory + U"file-link").c_str()) == 0);
	REQUIRE(::symlink(Unicode::ToUTF8(outside).c_str(), Unicode::ToUTF8(directory + U"directory-link").c_str()) == 0);
	REQUIRE(::symlink(".", Unicode::ToUTF8(directory + U"cycle").c_str()) == 0);
	CHECK_EQ(FileSystem::Size(directory), 8);
	CHECK_EQ(FileSystem::Size(alias), 8);
	CHECK_EQ(FileSystem::Size(alias + U'/'), 8);
	CHECK_EQ(FileSystem::Size(directory + U"file-link"), 5);
}

TEST_CASE("FileSystem::IsEmptyDirectory permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	for (const bool populated : { false, true })
	{
		const FilePath directory = Test::OutputPath(populated
			? U"filesystem/isemptydirectory/locked-populated/"
			: U"filesystem/isemptydirectory/locked-empty/");
		REQUIRE(FileSystem::CreateDirectories(directory));
		if (populated)
		{
			BinaryFileWriter writer{ directory + U"file.txt" };
			REQUIRE(writer.isOpen());
		}
		const std::string native = Unicode::ToUTF8(directory);
		const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
		REQUIRE(::chmod(native.c_str(), 0000) == 0);
		bool result = true;
		CHECK_NOTHROW(result = FileSystem::IsEmptyDirectory(directory));
		CHECK_FALSE(result);
		CHECK_FALSE(FileSystem::IsEmptyDirectory(directory + U"missing"));
		REQUIRE(::chmod(native.c_str(), 0500) == 0);
		CHECK_EQ(FileSystem::IsEmptyDirectory(directory), (not populated));
	}
}

TEST_CASE("FileSystem::IsEmptyDirectory symbolic links")
{
	const FilePath root = Test::OutputPath(U"filesystem/isemptydirectory/links/");
	const FilePath alias = (root + U"alias");
	REQUIRE(FileSystem::CreateDirectories(root + U"empty/"));
	REQUIRE(::symlink("empty", Unicode::ToUTF8(alias).c_str()) == 0);
	REQUIRE(::symlink("missing-target", Unicode::ToUTF8(root + U"broken").c_str()) == 0);
	REQUIRE(::symlink("loop", Unicode::ToUTF8(root + U"loop").c_str()) == 0);
	CHECK(FileSystem::IsEmptyDirectory(alias));
	CHECK_FALSE(FileSystem::IsEmptyDirectory(root + U"broken"));
	CHECK_FALSE(FileSystem::IsEmptyDirectory(root + U"loop"));
	{
		BinaryFileWriter writer{ root + U"empty/file.txt" };
		REQUIRE(writer.isOpen());
	}
	CHECK_FALSE(FileSystem::IsEmptyDirectory(alias));
	CHECK(std::filesystem::is_symlink(Unicode::ToUTF8(alias)));
}

TEST_CASE("FileSystem::CreateParentDirectories resolution failure")
{
	const FilePath root = Test::OutputPath(U"filesystem/createparent/resolution/");
	REQUIRE(FileSystem::CreateDirectories(root));
	const FilePath loop = (root + U"loop");
	REQUIRE(::symlink("loop", Unicode::ToUTF8(loop).c_str()) == 0);
	const FilePath source = (root + U"source.bin");
	{
		BinaryFileWriter writer{ source };
		REQUIRE(writer.isOpen());
		REQUIRE(writer.write("data", 4) == 4);
	}
	for (const FilePath& path : { loop, loop + U"/child.txt" })
	{
		CHECK_FALSE(FileSystem::CreateParentDirectories(path));
		CHECK_FALSE(FileSystem::Copy(source, path));
	}
	CHECK_EQ(FileSystem::FileSize(source), 4);
}

TEST_CASE("FileSystem::CreateParentDirectories permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	const FilePath directory = Test::OutputPath(U"filesystem/createparent/locked/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	const std::string native = Unicode::ToUTF8(directory);
	const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
	REQUIRE(::chmod(native.c_str(), 0000) == 0);
	CHECK_FALSE(FileSystem::CreateParentDirectories(directory + U"nested/file.txt"));
	REQUIRE(::chmod(native.c_str(), 0500) == 0);
	CHECK_FALSE(FileSystem::CreateParentDirectories(directory + U"nested/file.txt"));
	CHECK(FileSystem::CreateParentDirectories(directory + U"file.txt"));
}

TEST_CASE("FileSystem::FullPath resolution failures")
{
	const FilePath root = Test::OutputPath(U"filesystem/fullpath/failures/");
	REQUIRE(FileSystem::CreateDirectories(root));
	const FilePath loop = (root + U"loop");
	REQUIRE(::symlink("loop", Unicode::ToUTF8(loop).c_str()) == 0);
	for (const FilePath& path : { loop, loop + U"/child" })
	{
		FilePath result = U"unchanged";
		CHECK_NOTHROW(result = FileSystem::FullPath(path));
		CHECK(result.isEmpty());
		CHECK_NOTHROW(result = FileSystem::RelativePath(path, root));
		CHECK(result.isEmpty());
		CHECK_NOTHROW(result = FileSystem::RelativePath(root, path));
		CHECK(result.isEmpty());
		FilePath base = U"unchanged";
		CHECK_NOTHROW(result = FileSystem::ParentPath(path, 0, base));
		CHECK(result.isEmpty());
		CHECK(base.isEmpty());
# if SIV3D_PLATFORM(MACOS)
		NativeFilePath native = "unchanged";
		CHECK_NOTHROW(native = FileSystem::NativePath(path));
		CHECK(native.empty());
		CHECK_FALSE(System::OpenInBrowser(path + U"/file.html"));
		CHECK_FALSE(System::ShowInFileManager(path));
		CHECK_FALSE(System::LaunchFile(path));
		CHECK_FALSE(System::LaunchFileWithTextEditor(path));
# endif
	}
}

TEST_CASE("FileSystem::FullPath permission failure")
{
	if (::geteuid() == 0)
	{
		MESSAGE("Permission denial requires a non-root user.");
		return;
	}
	const FilePath root = Test::OutputPath(U"filesystem/fullpath/permissions/");
	const FilePath directory = (root + U"locked/");
	REQUIRE(FileSystem::CreateDirectories(directory));
	const std::string native = Unicode::ToUTF8(directory);
	const ScopeExit restorePermissions{ [&native] { ::chmod(native.c_str(), 0700); } };
	REQUIRE(::chmod(native.c_str(), 0000) == 0);
	const FilePath path = (directory + U"child");
	FilePath result = U"unchanged";
	CHECK_NOTHROW(result = FileSystem::FullPath(path));
	CHECK(result.isEmpty());
	CHECK_NOTHROW(result = FileSystem::RelativePath(path, root));
	CHECK(result.isEmpty());
	CHECK_NOTHROW(result = FileSystem::RelativePath(root, path));
	CHECK(result.isEmpty());
# if SIV3D_PLATFORM(MACOS)
	NativeFilePath nativeResult = "unchanged";
	CHECK_NOTHROW(nativeResult = FileSystem::NativePath(path));
	CHECK(nativeResult.empty());
	CHECK_FALSE(System::OpenInBrowser(path + U"/file.html"));
# endif
}

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
