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

TEST_CASE("TextFileWriter.unicodeConverter")
{
	const String input{ U'A', U'\u00A2', U'あ', U'\U0001F600', char32{ 0xD800 }, char32{ 0x110000 }, U'B' };
	const String expected = U"A\u00A2あ\U0001F600\uFFFD\uFFFDB";

	for (const auto encoding : { TextEncoding::UTF8_NO_BOM, TextEncoding::UTF16LE, TextEncoding::UTF16BE })
	{
		CAPTURE(static_cast<int32>(encoding));
		const FilePath path = Test::OutputPath(U"unicode-converter-writer.txt");
		{
			TextFileWriter writer{ path, encoding };
			REQUIRE(writer.isOpen());
			writer.write(input);
		}

		std::string expectedBytes;
		if (encoding == TextEncoding::UTF8_NO_BOM)
		{
			expectedBytes = Unicode::ToUTF8(expected);
		}
		else
		{
			const bool bigEndian = (encoding == TextEncoding::UTF16BE);
			expectedBytes = (bigEndian ? "\xFE\xFF" : "\xFF\xFE");
			for (const char16 unit : Unicode::ToUTF16(expected))
			{
				const char low = static_cast<char>(unit & 0xFF);
				const char high = static_cast<char>(unit >> 8);
				expectedBytes.push_back(bigEndian ? high : low);
				expectedBytes.push_back(bigEndian ? low : high);
			}
		}

		BinaryFileReader reader{ path };
		REQUIRE(reader.isOpen());
		REQUIRE(reader.size() == static_cast<int64>(expectedBytes.size()));
		std::string actual(expectedBytes.size(), '\0');
		REQUIRE(reader.read(actual.data(), static_cast<int64>(actual.size())) == static_cast<int64>(actual.size()));
		CHECK(actual == expectedBytes);
	}
}
