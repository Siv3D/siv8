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
# include <ThirdParty/nanobench/nanobench.h>
# include <sstream>
# include <stdexcept>

TEST_CASE("TextFileReader.encoding")
{
	REQUIRE(TextFileReader{}.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextFileReader{ U"../../Test/data/text/nonexist.txt" }.encoding() == TextEncoding::UTF8_NO_BOM);

	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8_empty.txt" }.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_empty.txt" }.encoding() == TextEncoding::UTF8_WITH_BOM);
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_empty.txt" }.encoding() == TextEncoding::UTF16LE);
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_empty.txt" }.encoding() == TextEncoding::UTF16BE);

	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8_a.txt" }.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_a.txt" }.encoding() == TextEncoding::UTF8_WITH_BOM);
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_a.txt" }.encoding() == TextEncoding::UTF16LE);
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_a.txt" }.encoding() == TextEncoding::UTF16BE);

	{
		TextFileReader reader;
		REQUIRE(reader.encoding() == TextEncoding::UTF8_NO_BOM);
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader.encoding() == TextEncoding::UTF8_WITH_BOM);
		reader.close();
		REQUIRE(reader.encoding() == TextEncoding::UTF8_NO_BOM);
	}
}

TEST_CASE("TextFileReader.path")
{
	REQUIRE(TextFileReader{}.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextFileReader{ U"../../Test/data/text/nonexist.txt" }.path().isEmpty());

	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8_empty.txt"));
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8bom_empty.txt"));
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16le_empty.txt"));
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16be_empty.txt"));

	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8_a.txt"));
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8bom_a.txt"));
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16le_a.txt"));
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16be_a.txt"));

	{
		TextFileReader reader;
		REQUIRE(reader.path().isEmpty());
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader.path() == FileSystem::FullPath(U"../../Test/data/text/utf8bom_a.txt"));
		reader.close();
		REQUIRE(reader.path().isEmpty());
	}
}

TEST_CASE("TextFileReader.isOpen")
{
	REQUIRE(TextFileReader{}.isOpen() == false);
	REQUIRE(TextFileReader{ U"../../Test/data/text/nonexist.txt" }.isOpen() == false);

	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8_empty.txt" }.isOpen());
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_empty.txt" }.isOpen());
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_empty.txt" }.isOpen());
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_empty.txt" }.isOpen());

	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8_a.txt" }.isOpen());
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_a.txt" }.isOpen());
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_a.txt" }.isOpen());
	REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_a.txt" }.isOpen());

	{
		TextFileReader reader;
		REQUIRE(not reader.isOpen());
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader.isOpen());
		reader.close();
		REQUIRE(not reader.isOpen());
	}
}

TEST_CASE("TextFileReader.operator bool")
{
	{
		TextFileReader reader;
		REQUIRE(not reader);
	}

	{
		TextFileReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(not reader);
	}

	{
		TextFileReader reader{ U"../../Test/data/text/utf8_empty.txt" };
		REQUIRE(reader);
	}

	{
		TextFileReader reader{ U"../../Test/data/text/utf16be_a.txt" };
		REQUIRE(reader);
	}
	
	{
		TextFileReader reader;
		REQUIRE(not reader);
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader);
		reader.close();
		REQUIRE(not reader);
	}
}

TEST_CASE("TextFileReader.default")
{
	{
		TextFileReader reader;
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextFileReader reader;
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	{
		TextFileReader reader;
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextFileReader reader;
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}
}

TEST_CASE("TextFileReader.nonexist")
{
	{
		TextFileReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextFileReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	{
		TextFileReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextFileReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}
}

TEST_CASE("TextFileReader.reopen_nonexist")
{
	{
		TextFileReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.isOpen() == false);
		REQUIRE(not reader);
		REQUIRE(reader.encoding() == TextEncoding::UTF8_NO_BOM);
		REQUIRE(reader.path().isEmpty());
		REQUIRE(reader.readAll().isEmpty());
	}

	{
		TextFileReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextFileReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	{
		TextFileReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextFileReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}
}

TEST_CASE("TextFileReader.empty")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_empty.txt",
		U"../../Test/data/text/utf8bom_empty.txt",
		U"../../Test/data/text/utf16le_empty.txt",
		U"../../Test/data/text/utf16be_empty.txt",
	};

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextFileReader.a")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_a.txt",
		U"../../Test/data/text/utf8bom_a.txt",
		U"../../Test/data/text/utf16le_a.txt",
		U"../../Test/data/text/utf16be_a.txt",
	};

	const Array<String> targetTextLines = { U"a" };

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll() == U"a");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLines() == targetTextLines);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readChar() == U'a');
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLine() == U"a");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'a');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"a");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextFileReader.aCRLF")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_aCRLF.txt",
		U"../../Test/data/text/utf8bom_aCRLF.txt",
		U"../../Test/data/text/utf16le_aCRLF.txt",
		U"../../Test/data/text/utf16be_aCRLF.txt",
	};

	const Array<String> targetTextLines = { U"a", U"" };

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll() == U"a\n");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLines() == targetTextLines);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readChar() == U'a');
		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLine() == U"a");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'a');
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'\n');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"a");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextFileReader.aLF")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_aLF.txt",
		U"../../Test/data/text/utf8bom_aLF.txt",
		U"../../Test/data/text/utf16le_aLF.txt",
		U"../../Test/data/text/utf16be_aLF.txt",
	};

	const Array<String> targetTextLines = { U"a", U"" };

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll() == U"a\n");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLines() == targetTextLines);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readChar() == U'a');
		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLine() == U"a");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'a');
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'\n');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"a");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextFileReader.あ")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_あ.txt",
		U"../../Test/data/text/utf8bom_あ.txt",
		U"../../Test/data/text/utf16le_あ.txt",
		U"../../Test/data/text/utf16be_あ.txt",
	};

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll() == U"あ");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readChar() == U'あ');
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLine() == U"あ");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'あ');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"あ");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextFileReader.shortCRLF")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_shortCRLF.txt",
		U"../../Test/data/text/utf8bom_shortCRLF.txt",
		U"../../Test/data/text/utf16le_shortCRLF.txt",
		U"../../Test/data/text/utf16be_shortCRLF.txt",
	};

	const String short1 = U"\n\nあいうえお\n\nabcde\n\n𩸽_𩸽さかな𩸽𩹉_🐈\n\n𝔘𝔱𝔣\n\n";
	const Array<String> short1Lines = short1.split(U'\n');

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.isOpen());
		REQUIRE(reader.readAll() == short1);
		REQUIRE(reader.readAll() == String{});
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };

		for (const auto& c : short1)
		{
			REQUIRE(reader.readChar() == c);
		}

		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };

		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readChar() == U'あ');
		REQUIRE(reader.readChar() == U'い');
		REQUIRE(reader.readChar() == U'う');
		REQUIRE(reader.readChar() == U'え');
		REQUIRE(reader.readChar() == U'お');
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readLine() == U"abcde");
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readChar() == U'𩸽');
		REQUIRE(reader.readLine() == U"_𩸽さかな𩸽𩹉_🐈");
		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == U"𝔘𝔱𝔣");
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLines() == short1Lines);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll() == short1);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		std::ignore = reader.readChar();
		std::ignore = reader.readLine();
		std::ignore = reader.readChar();
		REQUIRE(reader.readAll() == short1.substr(3));
	}
}

TEST_CASE("TextFileReader.shortLF")
{
	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_shortLF.txt",
		U"../../Test/data/text/utf8bom_shortLF.txt",
		U"../../Test/data/text/utf16le_shortLF.txt",
		U"../../Test/data/text/utf16be_shortLF.txt",
	};

	const String targetText = U"\n\nあいうえお\n\nabcde\n\n𩸽_𩸽さかな𩸽𩹉_🐈\n\n𝔘𝔱𝔣\n\n";
	const Array<String> short1Lines = targetText.split(U'\n');

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.isOpen());
		REQUIRE(reader.readAll() == targetText);
		REQUIRE(reader.readAll() == String{});
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };

		for (const auto& c : targetText)
		{
			REQUIRE(reader.readChar() == c);
		}

		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };

		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readChar() == U'あ');
		REQUIRE(reader.readChar() == U'い');
		REQUIRE(reader.readChar() == U'う');
		REQUIRE(reader.readChar() == U'え');
		REQUIRE(reader.readChar() == U'お');
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readLine() == U"abcde");
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readChar() == U'𩸽');
		REQUIRE(reader.readLine() == U"_𩸽さかな𩸽𩹉_🐈");
		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == U"𝔘𝔱𝔣");
		REQUIRE(reader.readLine() == U"");
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readLines() == short1Lines);
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		REQUIRE(reader.readAll() == targetText);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextFileReader reader{ path };
		std::ignore = reader.readChar();
		std::ignore = reader.readLine();
		std::ignore = reader.readChar();
		REQUIRE(reader.readAll() == targetText.substr(3));
	}
}

TEST_CASE("TextFileReader.longCRLF")
{
	const String targetText = TextFileReader{ U"../../Test/data/text/utf8_longCRLF.txt" }.readAll();
	const Array<String> targetTextLines = targetText.split(U'\n');
	const Array<String> targetTextLines_(targetTextLines.begin(), (targetTextLines.end() - 1));
	const std::string targetTextUTF8 = targetText.toUTF8();
	const Array<std::string> targetTextLinesUTF8 = targetTextLines.map([](const String& line) { return Unicode::ToUTF8(line); });
	const Array<std::string> targetTextLinesUTF8_(targetTextLinesUTF8.begin(), (targetTextLinesUTF8.end() - 1));

	REQUIRE(targetText.starts_with(U"{{転送"));
	REQUIRE(targetText.ends_with(U"*]]\n"));
	REQUIRE(targetTextLines.size() == 4473);

	constexpr std::array TestTextFiles
	{
		U"../../Test/data/text/utf8_longCRLF.txt",
		U"../../Test/data/text/utf8bom_longCRLF.txt",
		U"../../Test/data/text/utf16le_longCRLF.txt",
		U"../../Test/data/text/utf16be_longCRLF.txt",
	};

	for (const auto& path : TestTextFiles)
	{
		{
			REQUIRE(TextFileReader{ path }.readAll() == targetText);
		}

		{
			String s;
			REQUIRE(TextFileReader{ path }.readAll(s));
			REQUIRE(s == targetText);
		}

		{
			std::string s;
			REQUIRE(TextFileReader{ path }.readAll(s));
			REQUIRE(s == targetTextUTF8);
		}

		{
			REQUIRE(TextFileReader{ path }.readLines() == targetTextLines);
		}

		{
			Array<String> lines;
			REQUIRE(TextFileReader{ path }.readLines(lines));
			REQUIRE(lines == targetTextLines);
		}

		{
			Array<std::string> lines;
			REQUIRE(TextFileReader{ path }.readLines(lines));
			const bool eq = (lines == targetTextLinesUTF8);
			REQUIRE(eq);
		}

		{
			TextFileReader reader{ path };
			Array<String> lines;
			while (auto line = reader.readLine())
			{
				lines.push_back(*line);
			}
			REQUIRE(lines == targetTextLines_);
		}

		{
			TextFileReader reader{ path };
			Array<String> lines;
			String line;
			while (reader.readLine(line))
			{
				lines.push_back(line);
			}
			REQUIRE(lines == targetTextLines_);
		}

		{
			TextFileReader reader{ path };
			Array<std::string> lines;
			std::string line;
			while (reader.readLine(line))
			{
				lines.push_back(line);
			}
			const bool eq = (lines == targetTextLinesUTF8_);
			REQUIRE(eq);
		}

		{
			TextFileReader reader{ path };
			String s;
			while (auto ch = reader.readChar())
			{
				s.push_back(*ch);
			}
			REQUIRE(s == targetText);
		}

		{
			TextFileReader reader{ path };
			String s;
			char32 ch;
			while (reader.readChar(ch))
			{
				s.push_back(ch);
			}
			REQUIRE(s == targetText);
		}
	}
}

TEST_CASE("TextFileReader.longLF")
{
	const String targetText = TextFileReader{ U"../../Test/data/text/utf8_longLF.txt" }.readAll();

	{
		REQUIRE(targetText.starts_with(U"{{転送"));
		REQUIRE(targetText.ends_with(U"*]]\n"));
	}

	{
		REQUIRE(TextFileReader{ U"../../Test/data/text/utf8bom_longLF.txt" }.readAll() == targetText);
		REQUIRE(TextFileReader{ U"../../Test/data/text/utf16le_longLF.txt" }.readAll() == targetText);
		REQUIRE(TextFileReader{ U"../../Test/data/text/utf16be_longLF.txt" }.readAll() == targetText);
	}
}

TEST_CASE("TextFileReader.UTF8.invalidCodePoints")
{
	struct Example
	{
		std::string_view input;
		StringView expected;
	};

	const Example examples[] =
	{
		{ "\x80" "ABC", U"\uFFFDABC" },
		{ "\xE2" "A", U"\uFFFDA" },
		{ "\xE2\x82" "A", U"\uFFFDA" },
		{ "\xE2\xC2\xA2", U"\uFFFD\u00A2" },
		{ "\xED\xA0\x80", U"\uFFFD\uFFFD\uFFFD" },
		{ "\xF4\x90\x80\x80", U"\uFFFD\uFFFD\uFFFD\uFFFD" },
		{ "\xC2", U"\uFFFD" },
		{ "\xE2\x82", U"\uFFFD" },
		{ "\xF0\x90\x80", U"\uFFFD" },
	};

	for (size_t i = 0; i < std::size(examples); ++i)
	{
		CAPTURE(i);
		const auto& example = examples[i];
		TextFileReader reader{ MemoryReader{ example.input.data(), example.input.size() }, TextEncoding::UTF8_NO_BOM };
		String output;
		char32 ch;
		while (reader.readChar(ch))
		{
			output.push_back(ch);
		}
		CHECK(output == example.expected);
		CHECK_FALSE(reader.readChar(ch));
	}
}

TEST_CASE("TextFileReader.UTF8.retryAcrossReadMethods")
{
	const std::string_view input = "\xE2" "ABC\nDEF";
	TextFileReader reader{ MemoryReader{ input.data(), input.size() }, TextEncoding::UTF8_NO_BOM };
	char32 ch;
	REQUIRE(reader.readChar(ch));
	CHECK(ch == 0xFFFD);
	std::string line;
	REQUIRE(reader.readLine(line));
	CHECK(line == "ABC");
	CHECK(reader.readAll() == U"DEF");

	const std::string_view newline = "\xE2\nZ";
	TextFileReader lines{ MemoryReader{ newline.data(), newline.size() }, TextEncoding::UTF8_NO_BOM };
	String text;
	REQUIRE(lines.readLine(text));
	CHECK(text == U"\uFFFD");
	REQUIRE(lines.readLine(text));
	CHECK(text == U"Z");
	CHECK_FALSE(lines.readLine(text));
}


namespace
{
	class TextReaderFailure : public std::runtime_error
	{
	public:
		TextReaderFailure() : std::runtime_error{ "Injected reader failure" } {}
	};

	class LimitedTextReader : public MemoryViewReader
	{
	public:
		LimitedTextReader(const std::string_view input, const int64 limit, const bool throws = false)
			: MemoryViewReader{ input.data(), input.size() }
			, m_limit{ limit }
			, m_throws{ throws } {}

		int64 read(void* dst, const int64 size) override
		{
			const int64 count = MemoryViewReader::read(dst, Min(size, m_limit));
			if (m_throws)
			{
				throw TextReaderFailure{};
			}
			return count;
		}

	private:
		int64 m_limit;
		bool m_throws;
	};
}

TEST_CASE("TextFileReader.UTF8.bulk.normalAndReuse")
{
	struct Example
	{
		std::string_view input;
		std::string_view expected8;
		StringView expected32;
	};
	const Example examples[] =
	{
		{ "", "", U"" },
		{ "\r\r", "", U"" },
		{ "\n", "\n", U"\n" },
		{ "A\r\nB\rC\n", "A\nBC\n", U"A\nBC\n" },
		{ "日本語\U0001F600\r\nlast", "日本語\U0001F600\nlast", U"日本語\U0001F600\nlast" },
		{ { "A\0B\r\nC", 6 }, { "A\0B\nC", 5 }, { U"A\0B\nC", 5 } },
		{ "A\xFF" "B", "A\xFF" "B", U"" },
	};

	std::string output8;
	String output32;
	output8.reserve(256);
	output32.reserve(256);
	const auto* storage8 = output8.data();
	const auto* storage32 = output32.data();
	for (const bool bom : { false, true })
	{
		for (size_t i = 0; i < std::size(examples); ++i)
		{
			CAPTURE(bom);
			CAPTURE(i);
			const auto& example = examples[i];
			const std::string input = (std::string(bom ? "\xEF\xBB\xBF" : "") + std::string(example.input));
			const auto makeReader = [&] { return TextFileReader{ MemoryViewReader{ input.data(), input.size() } }; };
			const bool hasBytes = (not example.input.empty());
			output8.assign(64, 'X');
			output32.assign(64, U'X');
			auto reader8 = makeReader();
			REQUIRE(reader8.isOpen());
			CHECK(reader8.readAll(output8) == hasBytes);
			CHECK(output8 == example.expected8);
			CHECK(output8.data() == storage8);
			CHECK(output8.c_str()[output8.size()] == '\0');
			CHECK_FALSE(reader8.readAll(output8));
			CHECK(output8.empty());
			CHECK(makeReader().readAllUTF8() == example.expected8);

			auto reader32 = makeReader();
			CHECK(reader32.readAll(output32) == hasBytes);
			CHECK(output32 == example.expected32);
			CHECK(output32.data() == storage32);
			CHECK_FALSE(reader32.readAll(output32));
			CHECK(output32.empty());
			CHECK(makeReader().readAll() == example.expected32);
		}
	}
}

TEST_CASE("TextFileReader.UTF8.bulk.sizesAndMixedReads")
{
	for (const size_t length : { 0, 1, 15, 16, 17, 31, 32, 33, 4095, 4096, 4097, 16383, 16384, 16385, 65536 })
	{
		CAPTURE(length);
		const std::string expected(length, 'x');
		TextFileReader reader{ MemoryViewReader{ expected.data(), expected.size() } };
		std::string output = "old content";
		CHECK(reader.readAll(output) == (length != 0));
		CHECK(output == expected);
		CHECK(output.c_str()[output.size()] == '\0');
	}

	const std::string input = "\xEF\xBB\xBF" "first\r\n第二\U0001F600\nlast\r";
	TextFileReader reader{ MemoryViewReader{ input.data(), input.size() } };
	char32 ch;
	REQUIRE(reader.readChar(ch));
	CHECK(ch == U'f');
	std::string line;
	REQUIRE(reader.readLine(line));
	CHECK(line == "irst");
	CHECK(reader.readAllUTF8() == "第二\U0001F600\nlast");
	CHECK(reader.readAllUTF8().empty());
}

TEST_CASE("TextFileReader.UTF8.bulk.shortReadAndException")
{
	const std::string_view input = "ab\rcdef";
	TextFileReader reader{ LimitedTextReader{ input, 3 }, TextEncoding::UTF8_NO_BOM };
	std::string output(128, 'X');
	REQUIRE(reader.readAll(output));
	CHECK(output == "ab");
	REQUIRE(reader.readAll(output));
	CHECK(output == "cde");
	REQUIRE(reader.readAll(output));
	CHECK(output == "f");
	CHECK_FALSE(reader.readAll(output));
	CHECK(output.empty());

	TextFileReader stopped{ LimitedTextReader{ input, 0 }, TextEncoding::UTF8_NO_BOM };
	output = "old content";
	CHECK_FALSE(stopped.readAll(output));
	CHECK(output.empty());

	// The reader may write some bytes before throwing. The exception must reach
	// the caller, and the output must remain a valid, reusable string.
	TextFileReader throwing{ LimitedTextReader{ input, 1, true }, TextEncoding::UTF8_NO_BOM };
	CHECK_THROWS_AS(throwing.readAll(output), TextReaderFailure);
	output = "reusable";
	CHECK(output == "reusable");
	REQUIRE(throwing.open(MemoryViewReader{ input.data(), input.size() }));
	REQUIRE(throwing.readAll(output));
	CHECK(output == "abcdef");
}

TEST_CASE("TextFileReader.UTF8.lines.viewsAndBoundaries")
{
	const String longLine(65536, U'x');
	const Array<String> expected = { longLine, U"", U"日本語\U0001F600", String{ U"A\0B", 3 }, U"" };
	const std::string input = ("\xEF\xBB\xBF" + longLine.toUTF8() + "\r\n\n日本語\U0001F600\r\n"
		+ std::string{ "A\0B\n", 4 });
	Array<String> lines = { U"old content" };
	TextFileReader reader{ MemoryViewReader{ input.data(), input.size() } };
	REQUIRE(reader.readLines(lines));
	CHECK(lines == expected);
	CHECK_FALSE(reader.readLines(lines));
	CHECK(lines.empty());

	for (const auto& example : std::initializer_list<std::pair<std::string_view, Array<String>>>{
		{ "", {} }, { "\r", {} }, { "\n", { U"", U"" } }, { "\n\n", { U"", U"", U"" } },
		{ "tail", { U"tail" } }, { "ok\nA\xFF" "B\nlast", { U"ok", U"", U"last" } } })
	{
		TextFileReader text{ MemoryViewReader{ example.first.data(), example.first.size() } };
		CHECK(text.readLines() == example.second);
	}
}

namespace
{
	struct UTF8Workload
	{
		std::string name;
		std::string input;
		std::string normalized;
		Array<String> lines;
	};

	Array<UTF8Workload> MakeUTF8Workloads()
	{
		Array<UTF8Workload> workloads;
		const auto addLine = [](UTF8Workload& workload, String line, const bool crlf = false)
		{
			const std::string utf8 = line.toUTF8();
			workload.input += (utf8 + (crlf ? "\r\n" : "\n"));
			workload.normalized += (utf8 + "\n");
			workload.lines.push_back(std::move(line));
		};

		UTF8Workload config{ .name = "config-json" };
		addLine(config, U"{\"items\": [");
		for (size_t i = 0; i < 64; ++i)
		{
			addLine(config, U"  {{\"id\": {}, \"name\": \"アイテム{}\", \"enabled\": true}}{}"_fmt(i, i, (i == 63) ? U"" : U","));
		}
		addLine(config, U"]}");
		workloads.push_back(std::move(config));

		UTF8Workload shader{ .name = "shader-ascii-lf" };
		for (size_t i = 0; i < 1024; ++i)
		{
			addLine(shader, U"float4 shade{}(float4 c) {{ return float4(c.rgb * 0.5, c.a); }}"_fmt(i));
		}
		workloads.push_back(std::move(shader));

		UTF8Workload dialogue{ .name = "dialogue-utf8-bom-crlf", .input = "\xEF\xBB\xBF" };
		for (size_t i = 0; i < 1024; ++i)
		{
			addLine(dialogue, U"{}\t勇者\tこんにちは、世界！ Welcome to the next stage. \U0001F600"_fmt(i), true);
		}
		workloads.push_back(std::move(dialogue));

		UTF8Workload longLines{ .name = "long-lines-1mib" };
		for (size_t i = 0; i < 32; ++i)
		{
			addLine(longLines, String(32768, U'x') + U"日本語\U0001F600", true);
		}
		workloads.push_back(std::move(longLines));
		return workloads;
	}
}

// Opt in with --no-skip --test-case=TextFileReader.benchmark.UTF8.
// Fixture creation and complete content checks are outside the timed regions.
TEST_CASE("TextFileReader.benchmark.UTF8" * doctest::skip())
{
	using ankerl::nanobench::Bench;
	using ankerl::nanobench::doNotOptimizeAway;
	const ScopedLogSilencer logSilencer;
	const auto workloads = MakeUTF8Workloads();
	for (const auto& workload : workloads)
	{
		const FilePath path = Test::OutputPath(U"text-reader-benchmark/" + Unicode::FromUTF8(workload.name) + U".txt");
		{
			BinaryFileWriter writer{ path };
			REQUIRE(writer.isOpen());
			REQUIRE(writer.write(workload.input.data(), workload.input.size()) == static_cast<int64>(workload.input.size()));
		}
		const String expected32 = Unicode::FromUTF8(workload.normalized);
		Array<String> expectedLines = workload.lines;
		expectedLines.push_back(U""); // Bulk readLines retains the trailing empty line.
		for (const bool file : { false, true })
		{
			CAPTURE(workload.name);
			CAPTURE(file);
			const auto makeReader = [&]() -> TextFileReader
			{
				if (file)
				{
					return TextFileReader{ path };
				}
				return TextFileReader{ MemoryViewReader{ workload.input.data(), workload.input.size() } };
			};
			std::string output8;
			String output32;
			Array<String> lines;
			REQUIRE(makeReader().readAllUTF8() == workload.normalized);
			REQUIRE(makeReader().readAll(output8));
			REQUIRE(output8 == workload.normalized);
			REQUIRE(makeReader().readAll(output32));
			REQUIRE(output32 == expected32);
			REQUIRE(makeReader().readLines(lines));
			REQUIRE(lines == expectedLines);
			{
				auto reader = makeReader();
				for (const auto& expected : workload.lines)
				{
					REQUIRE(reader.readLine(output32));
					REQUIRE(output32 == expected);
				}
				REQUIRE_FALSE(reader.readLine(output32));
			}
			{
				auto reader = makeReader();
				for (const auto& expected : workload.lines)
				{
					const auto line = reader.readLine();
					REQUIRE(line);
					REQUIRE(*line == expected);
				}
				REQUIRE_FALSE(reader.readLine());
			}

			{
				auto reader = makeReader();
				for (const auto& expected : workload.lines)
				{
					REQUIRE(reader.readLine(output8));
					REQUIRE(output8 == expected.toUTF8());
				}
				REQUIRE_FALSE(reader.readLine(output8));
			}

			std::ostringstream report;
			Bench bench;
			bench.title(workload.name + (file ? "/file" : "/memory"))
				.unit("byte").batch(workload.input.size()).epochs(11)
				.minEpochTime(std::chrono::milliseconds(2)).warmup(1)
				.performanceCounters(false).output(&report);
			bench.run("readAllUTF8/fresh", [&]
			{
				auto reader = makeReader();
				const auto output = reader.readAllUTF8();
				doNotOptimizeAway(output);
			});
			bench.run("readAll/utf8-reuse", [&]
			{
				auto reader = makeReader();
				doNotOptimizeAway(reader.readAll(output8));
				doNotOptimizeAway(output8);
			});
			bench.run("readAll/utf32-reuse", [&]
			{
				auto reader = makeReader();
				doNotOptimizeAway(reader.readAll(output32));
				doNotOptimizeAway(output32);
			});
			bench.run("readLines/utf32", [&]
			{
				auto reader = makeReader();
				doNotOptimizeAway(reader.readLines(lines));
				doNotOptimizeAway(lines);
			});
			bench.run("readLine/utf8-reuse", [&]
			{
				auto reader = makeReader();
				while (reader.readLine(output8))
				{
					doNotOptimizeAway(output8);
				}
			});
			bench.run("readLine/utf32-reuse", [&]
			{
				auto reader = makeReader();
				while (reader.readLine(output32))
				{
					doNotOptimizeAway(output32);
				}
			});
			bench.run("readLine/utf32-value", [&]
			{
				auto reader = makeReader();
				while (const auto line = reader.readLine())
				{
					doNotOptimizeAway(*line);
				}
			});
			if (workload.name == "config-json")
			{
				const auto load = [&]
				{
					return file ? JSON::Load(path) : JSON::Load(MemoryViewReader{ workload.input.data(), workload.input.size() });
				};
				REQUIRE(load()[U"items"].size() == 64);
				bench.run("JSON.Load", [&]
				{
					const auto json = load();
					doNotOptimizeAway(json);
				});
			}
			// CSV includes seconds per complete workload, not rounded ns/byte.
			ankerl::nanobench::render(ankerl::nanobench::templates::csv(), bench, report);
			MESSAGE(report.str());
		}
	}
}
