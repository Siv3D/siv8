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

TEST_CASE("TextReader.encoding")
{
	REQUIRE(TextReader{}.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextReader{ U"../../Test/data/text/nonexist.txt" }.encoding() == TextEncoding::UTF8_NO_BOM);

	REQUIRE(TextReader{ U"../../Test/data/text/utf8_empty.txt" }.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_empty.txt" }.encoding() == TextEncoding::UTF8_WITH_BOM);
	REQUIRE(TextReader{ U"../../Test/data/text/utf16le_empty.txt" }.encoding() == TextEncoding::UTF16LE);
	REQUIRE(TextReader{ U"../../Test/data/text/utf16be_empty.txt" }.encoding() == TextEncoding::UTF16BE);

	REQUIRE(TextReader{ U"../../Test/data/text/utf8_a.txt" }.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_a.txt" }.encoding() == TextEncoding::UTF8_WITH_BOM);
	REQUIRE(TextReader{ U"../../Test/data/text/utf16le_a.txt" }.encoding() == TextEncoding::UTF16LE);
	REQUIRE(TextReader{ U"../../Test/data/text/utf16be_a.txt" }.encoding() == TextEncoding::UTF16BE);

	{
		TextReader reader;
		REQUIRE(reader.encoding() == TextEncoding::UTF8_NO_BOM);
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader.encoding() == TextEncoding::UTF8_WITH_BOM);
		reader.close();
		REQUIRE(reader.encoding() == TextEncoding::UTF8_NO_BOM);
	}
}
/*
TEST_CASE("TextReader.path")
{
	REQUIRE(TextReader{}.encoding() == TextEncoding::UTF8_NO_BOM);
	REQUIRE(TextReader{ U"../../Test/data/text/nonexist.txt" }.path().isEmpty());

	REQUIRE(TextReader{ U"../../Test/data/text/utf8_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8_empty.txt"));
	REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8bom_empty.txt"));
	REQUIRE(TextReader{ U"../../Test/data/text/utf16le_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16le_empty.txt"));
	REQUIRE(TextReader{ U"../../Test/data/text/utf16be_empty.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16be_empty.txt"));

	REQUIRE(TextReader{ U"../../Test/data/text/utf8_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8_a.txt"));
	REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf8bom_a.txt"));
	REQUIRE(TextReader{ U"../../Test/data/text/utf16le_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16le_a.txt"));
	REQUIRE(TextReader{ U"../../Test/data/text/utf16be_a.txt" }.path() == FileSystem::FullPath(U"../../Test/data/text/utf16be_a.txt"));

	{
		TextReader reader;
		REQUIRE(reader.path().isEmpty());
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader.path() == FileSystem::FullPath(U"../../Test/data/text/utf8bom_a.txt"));
		reader.close();
		REQUIRE(reader.path().isEmpty());
	}
}
*/
TEST_CASE("TextReader.isOpen")
{
	REQUIRE(TextReader{}.isOpen() == false);
	REQUIRE(TextReader{ U"../../Test/data/text/nonexist.txt" }.isOpen() == false);

	REQUIRE(TextReader{ U"../../Test/data/text/utf8_empty.txt" }.isOpen());
	REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_empty.txt" }.isOpen());
	REQUIRE(TextReader{ U"../../Test/data/text/utf16le_empty.txt" }.isOpen());
	REQUIRE(TextReader{ U"../../Test/data/text/utf16be_empty.txt" }.isOpen());

	REQUIRE(TextReader{ U"../../Test/data/text/utf8_a.txt" }.isOpen());
	REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_a.txt" }.isOpen());
	REQUIRE(TextReader{ U"../../Test/data/text/utf16le_a.txt" }.isOpen());
	REQUIRE(TextReader{ U"../../Test/data/text/utf16be_a.txt" }.isOpen());

	{
		TextReader reader;
		REQUIRE(not reader.isOpen());
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader.isOpen());
		reader.close();
		REQUIRE(not reader.isOpen());
	}
}

TEST_CASE("TextReader.operator bool")
{
	{
		TextReader reader;
		REQUIRE(not reader);
	}

	{
		TextReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(not reader);
	}

	{
		TextReader reader{ U"../../Test/data/text/utf8_empty.txt" };
		REQUIRE(reader);
	}

	{
		TextReader reader{ U"../../Test/data/text/utf16be_a.txt" };
		REQUIRE(reader);
	}
	
	{
		TextReader reader;
		REQUIRE(not reader);
		REQUIRE(reader.open(U"../../Test/data/text/utf8bom_a.txt"));
		REQUIRE(reader);
		reader.close();
		REQUIRE(not reader);
	}
}

TEST_CASE("TextReader.default")
{
	{
		TextReader reader;
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextReader reader;
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	{
		TextReader reader;
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextReader reader;
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}
}

TEST_CASE("TextReader.nonexist")
{
	{
		TextReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	{
		TextReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextReader reader{ U"../../Test/data/text/nonexist.txt" };
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}
}

TEST_CASE("TextReader.reopen_nonexist")
{
	{
		TextReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.isOpen() == false);
		REQUIRE(not reader);
		REQUIRE(reader.encoding() == TextEncoding::UTF8_NO_BOM);
		REQUIRE(reader.path().isEmpty());
		REQUIRE(reader.readAll().isEmpty());
	}

	{
		TextReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	{
		TextReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	{
		TextReader reader;
		REQUIRE(not reader.open(U"../../Test/data/text/nonexist.txt"));
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}
}

TEST_CASE("TextReader.empty")
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
		TextReader reader{ path };
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readLines().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readAll().isEmpty());
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextReader.a")
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
		TextReader reader{ path };
		REQUIRE(reader.readAll() == U"a");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLines() == targetTextLines);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readChar() == U'a');
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLine() == U"a");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'a');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"a");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextReader.aCRLF")
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
		TextReader reader{ path };
		REQUIRE(reader.readAll() == U"a\n");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLines() == targetTextLines);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readChar() == U'a');
		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLine() == U"a");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
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
		TextReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"a");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextReader.aLF")
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
		TextReader reader{ path };
		REQUIRE(reader.readAll() == U"a\n");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLines() == targetTextLines);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readChar() == U'a');
		REQUIRE(reader.readChar() == U'\n');
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLine() == U"a");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
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
		TextReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"a");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextReader.あ")
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
		TextReader reader{ path };
		REQUIRE(reader.readAll() == U"あ");
		REQUIRE(reader.readAll() == String{});
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readChar() == U'あ');
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readLine() == U"あ");
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		char32 ch = U'?';
		REQUIRE(reader.readChar(ch) == true);
		REQUIRE(ch == U'あ');
		REQUIRE(reader.readChar(ch) == false);
		REQUIRE(ch == U'\0');
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		String line = U"???";
		REQUIRE(reader.readLine(line) == true);
		REQUIRE(line == U"あ");
		REQUIRE(reader.readLine(line) == false);
		REQUIRE(line.isEmpty());
	}
}

TEST_CASE("TextReader.shortCRLF")
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
		TextReader reader{ path };
		REQUIRE(reader.isOpen());
		REQUIRE(reader.readAll() == short1);
		REQUIRE(reader.readAll() == String{});
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };

		for (const auto& c : short1)
		{
			REQUIRE(reader.readChar() == c);
		}

		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };

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
		TextReader reader{ path };
		REQUIRE(reader.readLines() == short1Lines);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readAll() == short1);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		std::ignore = reader.readChar();
		std::ignore = reader.readLine();
		std::ignore = reader.readChar();
		REQUIRE(reader.readAll() == short1.substr(3));
	}
}

TEST_CASE("TextReader.shortLF")
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
		TextReader reader{ path };
		REQUIRE(reader.isOpen());
		REQUIRE(reader.readAll() == targetText);
		REQUIRE(reader.readAll() == String{});
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };

		for (const auto& c : targetText)
		{
			REQUIRE(reader.readChar() == c);
		}

		REQUIRE(reader.readChar() == none);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };

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
		TextReader reader{ path };
		REQUIRE(reader.readLines() == short1Lines);
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		REQUIRE(reader.readAll() == targetText);
		REQUIRE(reader.readChar() == none);
		REQUIRE(reader.readLine() == none);
		REQUIRE(reader.readLines().isEmpty());
	}

	for (const auto& path : TestTextFiles)
	{
		TextReader reader{ path };
		std::ignore = reader.readChar();
		std::ignore = reader.readLine();
		std::ignore = reader.readChar();
		REQUIRE(reader.readAll() == targetText.substr(3));
	}
}

TEST_CASE("TextReader.longCRLF")
{
	const String targetText = TextReader{ U"../../Test/data/text/utf8_longCRLF.txt" }.readAll();
	const Array<String> targetTextLines = targetText.split(U'\n');
	const Array<String> targetTextLines_(targetTextLines.begin(), (targetTextLines.end() - 1));
	const std::string targetTextUTF8 = targetText.toUTF8();
	const Array<std::string> targetTextLinesUTF8 = targetTextLines.map(Unicode::ToUTF8);
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
			REQUIRE(TextReader{ path }.readAll() == targetText);
		}

		{
			String s;
			REQUIRE(TextReader{ path }.readAll(s));
			REQUIRE(s == targetText);
		}

		{
			std::string s;
			REQUIRE(TextReader{ path }.readAll(s));
			REQUIRE(s == targetTextUTF8);
		}

		{
			REQUIRE(TextReader{ path }.readLines() == targetTextLines);
		}

		{
			Array<String> lines;
			REQUIRE(TextReader{ path }.readLines(lines));
			REQUIRE(lines == targetTextLines);
		}

		{
			Array<std::string> lines;
			REQUIRE(TextReader{ path }.readLines(lines));
			const bool eq = (lines == targetTextLinesUTF8);
			REQUIRE(eq);
		}

		{
			TextReader reader{ path };
			Array<String> lines;
			while (auto line = reader.readLine())
			{
				lines.push_back(*line);
			}
			REQUIRE(lines == targetTextLines_);
		}

		{
			TextReader reader{ path };
			Array<String> lines;
			String line;
			while (reader.readLine(line))
			{
				lines.push_back(line);
			}
			REQUIRE(lines == targetTextLines_);
		}

		{
			TextReader reader{ path };
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
			TextReader reader{ path };
			String s;
			while (auto ch = reader.readChar())
			{
				s.push_back(*ch);
			}
			REQUIRE(s == targetText);
		}

		{
			TextReader reader{ path };
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

TEST_CASE("TextReader.longLF")
{
	const String targetText = TextReader{ U"../../Test/data/text/utf8_longLF.txt" }.readAll();

	{
		REQUIRE(targetText.starts_with(U"{{転送"));
		REQUIRE(targetText.ends_with(U"*]]\n"));
	}

	{
		REQUIRE(TextReader{ U"../../Test/data/text/utf8bom_longLF.txt" }.readAll() == targetText);
		REQUIRE(TextReader{ U"../../Test/data/text/utf16le_longLF.txt" }.readAll() == targetText);
		REQUIRE(TextReader{ U"../../Test/data/text/utf16be_longLF.txt" }.readAll() == targetText);
	}
}

TEST_CASE("TextReader.benchmark")
{
	const auto oldOutputLevel = Logger.getOutputLevel();
	Logger.setOutputLevel(LogType::Error);

	{
		{
			Bench{}.title("TextReader utf8").run("readAll(std::string&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };
					std::string s;
					reader.readAll(s);
					doNotOptimizeAway(s);
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readAll(String&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };
					String s;
					reader.readAll(s);
					doNotOptimizeAway(s);
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readAll()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };
					auto s = reader.readAll();
					doNotOptimizeAway(s);
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readLines(Array<std::string>&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };
					Array<std::string> lines;
					reader.readLines(lines);
					doNotOptimizeAway(lines);
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readLines(Array<String>&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };
					Array<String> lines;
					reader.readLines(lines);
					doNotOptimizeAway(lines);
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readLines()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };
					auto lines = reader.readLines();
					doNotOptimizeAway(lines);
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readLine(std::string&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };

					std::string line;
					while (reader.readLine(line))
					{
						doNotOptimizeAway(line);
					}
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readLine(String&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };

					String line;
					while (reader.readLine(line))
					{
						doNotOptimizeAway(line);
					}
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readLine()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };

					String line;
					while (reader.readLine(line))
					{
						doNotOptimizeAway(line);
					}
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readChar(char32&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };

					char32 ch;
					while (reader.readChar(ch))
					{
						doNotOptimizeAway(ch);
					}
				});
		}

		{
			Bench{}.title("TextReader utf8").run("readChar()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf8_longCRLF.txt" };

					while (auto ch = reader.readChar())
					{
						doNotOptimizeAway(ch);
					}
				});
		}
	}

	{
		{
			Bench{}.title("TextReader utf16le").run("readAll(std::string&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };
					std::string s;
					reader.readAll(s);
					doNotOptimizeAway(s);
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readAll(String&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };
					String s;
					reader.readAll(s);
					doNotOptimizeAway(s);
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readAll()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };
					auto s = reader.readAll();
					doNotOptimizeAway(s);
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readLines(Array<std::string>&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };
					Array<std::string> lines;
					reader.readLines(lines);
					doNotOptimizeAway(lines);
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readLines(Array<String>&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };
					Array<String> lines;
					reader.readLines(lines);
					doNotOptimizeAway(lines);
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readLines()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };
					auto lines = reader.readLines();
					doNotOptimizeAway(lines);
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readLine(std::string&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };

					std::string line;
					while (reader.readLine(line))
					{
						doNotOptimizeAway(line);
					}
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readLine(String&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };

					String line;
					while (reader.readLine(line))
					{
						doNotOptimizeAway(line);
					}
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readLine()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };

					String line;
					while (reader.readLine(line))
					{
						doNotOptimizeAway(line);
					}
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readChar(char32&)", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };

					char32 ch;
					while (reader.readChar(ch))
					{
						doNotOptimizeAway(ch);
					}
				});
		}

		{
			Bench{}.title("TextReader utf16le").run("readChar()", [&]()
				{
					TextReader reader{ U"../../Test/data/text/utf16le_longCRLF.txt" };

					while (auto ch = reader.readChar())
					{
						doNotOptimizeAway(ch);
					}
				});
		}
	}

	Logger.setOutputLevel(oldOutputLevel);
}

