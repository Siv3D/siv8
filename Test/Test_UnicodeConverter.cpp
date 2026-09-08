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

# include <Siv3D/UnicodeDecodeStatus.hpp>
# include "Siv3DTest.hpp"

namespace
{
	template <class Converter, class View>
	std::u32string Decode(const View input)
	{
		Converter converter;
		std::u32string output;

		for (const auto code : input)
		{
			auto result = converter.put(code);

			if (result.status != UnicodeDecodeStatus::NeedMore)
			{
				output.push_back(converter.get());
			}

			if (not result.consumed)
			{
				REQUIRE(result.status == UnicodeDecodeStatus::Invalid);
				result = converter.put(code);
				REQUIRE(result.consumed);

				if (result.status != UnicodeDecodeStatus::NeedMore)
				{
					output.push_back(converter.get());
				}
			}
		}

		if (converter.finish())
		{
			output.push_back(converter.get());
		}

		CHECK_FALSE(converter.finish());
		return output;
	}
}

TEST_CASE("UnicodeConverter.encodingBoundaries")
{
	struct Example
	{
		char32 code;
		std::string_view utf8;
		std::u16string_view utf16;
	};

	const Example examples[] =
	{
		{ 0, std::string_view{ "\0", 1 }, std::u16string_view{ u"\0", 1 } },
		{ 0x7F, "\x7F", u"\x7F" },
		{ 0x80, "\xC2\x80", u"\x80" },
		{ 0x7FF, "\xDF\xBF", u"\x7FF" },
		{ 0x800, "\xE0\xA0\x80", u"\x800" },
		{ 0xD7FF, "\xED\x9F\xBF", u"\xD7FF" },
		{ 0xE000, "\xEE\x80\x80", u"\xE000" },
		{ 0xFFFD, "\xEF\xBF\xBD", u"\xFFFD" },
		{ 0xFFFF, "\xEF\xBF\xBF", u"\xFFFF" },
		{ 0x10000, "\xF0\x90\x80\x80", u"\xD800\xDC00" },
		{ 0x10FFFF, "\xF4\x8F\xBF\xBF", u"\xDBFF\xDFFF" },
	};

	UTF32toUTF8_Converter utf8;
	UTF32toUTF16_Converter utf16;
	CHECK(utf8.get() == std::array<char8, 4>{});
	CHECK(utf16.get() == std::array<char16, 2>{});
	CHECK(utf8.data() == &*utf8.begin());
	CHECK(utf16.data() == &*utf16.begin());
	const auto* const data8 = utf8.data();
	const auto* const data16 = utf16.data();

	for (const auto& example : examples)
	{
		CAPTURE(static_cast<uint32>(example.code));
		const size_t length8 = utf8.put(example.code);
		const size_t length16 = utf16.put(example.code);
		CHECK(std::string_view(utf8.data(), length8) == example.utf8);
		CHECK(std::u16string_view(utf16.data(), length16) == example.utf16);
		CHECK(utf8.get()[0] == example.utf8[0]);
		CHECK(utf16.get()[0] == example.utf16[0]);
		CHECK(Decode<UTF8toUTF32_Converter>(example.utf8) == std::u32string(1, example.code));
		CHECK(Decode<UTF16toUTF32_Converter>(example.utf16) == std::u32string(1, example.code));
	}

	// Reuse after a longer output; only the returned prefix is meaningful.
	CHECK(utf8.put(U'A') == 1);
	CHECK(utf16.put(U'A') == 1);
	CHECK(utf8.data() == data8);
	CHECK(utf16.data() == data16);
	CHECK(utf8.data()[0] == 'A');
	CHECK(utf16.data()[0] == u'A');
}

TEST_CASE("UnicodeConverter.allScalars")
{
	UTF32toUTF8_Converter encode8;
	UTF32toUTF16_Converter encode16;
	UTF8toUTF32_Converter decode8;
	UTF16toUTF32_Converter decode16;
	String scalars;
	std::string bytes;
	std::u16string units;
	scalars.reserve(0x110000 - 0x800);
	bytes.reserve(0x110000 * 4);
	units.reserve(0x110000 * 2);

	for (uint32 value = 0; value <= 0x10FFFF; ++value)
	{
		if ((0xD800 <= value) && (value <= 0xDFFF))
		{
			continue;
		}

		const char32 code = static_cast<char32>(value);
		const size_t length8 = encode8.put(code);
		const size_t length16 = encode16.put(code);
		REQUIRE(((1 <= length8) && (length8 <= 4)));
		REQUIRE(((1 <= length16) && (length16 <= 2)));
		scalars.push_back(code);
		bytes.append(encode8.data(), length8);
		units.append(encode16.data(), length16);

		for (size_t i = 0; i < length8; ++i)
		{
			const UTF8toUTF32_Converter::DecodeResult result = decode8.put(encode8.data()[i]);
			const auto expected = (((i + 1) == length8) ? UnicodeDecodeStatus::Ready : UnicodeDecodeStatus::NeedMore);
			if ((result.status != expected) || (not result.consumed))
			{
				CAPTURE(value);
				CAPTURE(i);
				FAIL("UTF-8 failed to decode a scalar");
			}
		}

		for (size_t i = 0; i < length16; ++i)
		{
			const UTF16toUTF32_Converter::DecodeResult result = decode16.put(encode16.data()[i]);
			const auto expected = (((i + 1) == length16) ? UnicodeDecodeStatus::Ready : UnicodeDecodeStatus::NeedMore);
			if ((result.status != expected) || (not result.consumed))
			{
				CAPTURE(value);
				CAPTURE(i);
				FAIL("UTF-16 failed to decode a scalar");
			}
		}

		if ((decode8.get() != code) || (decode16.get() != code))
		{
			CAPTURE(value);
			FAIL("Scalar round trip failed");
		}
	}

	CHECK_FALSE(decode8.finish());
	CHECK_FALSE(decode16.finish());
	// Independent bulk implementations also verify the encoded bytes and code units.
	CHECK(bytes == Unicode::ToUTF8(scalars));
	CHECK(units == Unicode::ToUTF16(scalars));
}

TEST_CASE("UnicodeConverter.UTF8.maximalSubparts")
{
	struct Example
	{
		std::string_view input;
		std::u32string_view expected;
	};

	const Example examples[] =
	{
		{ "", U"" },
		{ "\x80" "ABC", U"\uFFFDABC" },
		{ "\xC0\xAF", U"\uFFFD\uFFFD" },
		{ "\xC1\xBF", U"\uFFFD\uFFFD" },
		{ "\xC2" "A", U"\uFFFDA" },
		{ "\xE2" "A", U"\uFFFDA" },
		{ "\xE2\x82" "A", U"\uFFFDA" },
		{ "\xF0\x90\x80" "A", U"\uFFFDA" },
		{ "\xE0\x80\x80", U"\uFFFD\uFFFD\uFFFD" },
		{ "\xED\xA0\x80", U"\uFFFD\uFFFD\uFFFD" },
		{ "\xED\xBF\xBF", U"\uFFFD\uFFFD\uFFFD" },
		{ "\xF0\x80\x80\x80", U"\uFFFD\uFFFD\uFFFD\uFFFD" },
		{ "\xF4\x90\x80\x80", U"\uFFFD\uFFFD\uFFFD\uFFFD" },
		{ "\xF5\x80\x80\x80", U"\uFFFD\uFFFD\uFFFD\uFFFD" },
		{ "\xF8\xFF", U"\uFFFD\uFFFD" },
		{ "\xE2\xC2\xA2", U"\uFFFD\u00A2" },
		{ "\xE2\x82\xF0\x9F\x98\x80", U"\uFFFD\U0001F600" },
		{ "\xC2", U"\uFFFD" },
		{ "\xE2", U"\uFFFD" },
		{ "\xE2\x82", U"\uFFFD" },
		{ "\xF0", U"\uFFFD" },
		{ "\xF0\x90", U"\uFFFD" },
		{ "\xF0\x90\x80", U"\uFFFD" },
		{ std::string_view{ "\xE2\0A", 3 }, std::u32string_view{ U"\uFFFD\0A", 3 } },
	};

	for (size_t i = 0; i < std::size(examples); ++i)
	{
		CAPTURE(i);
		CHECK(Decode<UTF8toUTF32_Converter>(examples[i].input) == examples[i].expected);
	}

	for (uint32 byte = 0; byte <= 0xFF; ++byte)
	{
		CAPTURE(byte);
		UTF8toUTF32_Converter converter;
		const auto result = converter.put(static_cast<char8>(byte));
		REQUIRE(result.consumed);
		if (byte < 0x80)
		{
			CHECK(result.status == UnicodeDecodeStatus::Ready);
			CHECK(converter.get() == byte);
			CHECK_FALSE(converter.finish());
		}
		else if ((0xC2 <= byte) && (byte <= 0xF4))
		{
			CHECK(result.status == UnicodeDecodeStatus::NeedMore);
			CHECK(converter.finish());
			CHECK(converter.get() == 0xFFFD);
		}
		else
		{
			CHECK(result.status == UnicodeDecodeStatus::Invalid);
			CHECK(converter.get() == 0xFFFD);
			CHECK_FALSE(converter.finish());
		}
	}
}

TEST_CASE("UnicodeConverter.UTF16.maximalSubparts")
{
	struct Example
	{
		std::u16string_view input;
		std::u32string_view expected;
	};

	const Example examples[] =
	{
		{ u"", U"" },
		{ u"\xD800", U"\uFFFD" },
		{ u"\xDBFF", U"\uFFFD" },
		{ u"\xDC00", U"\uFFFD" },
		{ u"\xDFFF", U"\uFFFD" },
		{ u"\xD800" u"A", U"\uFFFDA" },
		{ u"\xD800\xD800\xDC00", U"\uFFFD\U00010000" },
		{ u"\xD800\xDBFF", U"\uFFFD\uFFFD" },
		{ u"\xDC00\xDFFF" u"A", U"\uFFFD\uFFFDA" },
		{ std::u16string_view{ u"\xD800\0A", 3 }, std::u32string_view{ U"\uFFFD\0A", 3 } },
	};

	for (size_t i = 0; i < std::size(examples); ++i)
	{
		CAPTURE(i);
		CHECK(Decode<UTF16toUTF32_Converter>(examples[i].input) == examples[i].expected);
	}
}

TEST_CASE("UnicodeConverter.stateAndRetry")
{
	UTF8toUTF32_Converter utf8;
	UTF16toUTF32_Converter utf16;

	const auto check = [](auto& converter, const auto prefix, const auto suffix, const char32 expected)
	{
		CHECK(converter.get() == U'\0');
		CHECK_FALSE(converter.finish());
		CHECK(converter.put('A').status == UnicodeDecodeStatus::Ready);
		CHECK(converter.put(prefix).status == UnicodeDecodeStatus::NeedMore);
		CHECK(converter.get() == U'A');
		CHECK(converter.put(suffix).status == UnicodeDecodeStatus::Ready);
		CHECK(converter.get() == expected);
		CHECK_FALSE(converter.finish());
		CHECK(converter.get() == expected);

		CHECK(converter.put(prefix).status == UnicodeDecodeStatus::NeedMore);
		const auto invalid = converter.put('B');
		CHECK(invalid.status == UnicodeDecodeStatus::Invalid);
		CHECK_FALSE(invalid.consumed);
		CHECK(converter.get() == 0xFFFD);
		CHECK_FALSE(converter.finish());
		const auto retry = converter.put('B');
		CHECK(retry.status == UnicodeDecodeStatus::Ready);
		CHECK(retry.consumed);
		CHECK(converter.get() == U'B');

		CHECK(converter.put(prefix).status == UnicodeDecodeStatus::NeedMore);
		CHECK(converter.finish());
		CHECK(converter.get() == 0xFFFD);
		CHECK_FALSE(converter.finish());
		CHECK(converter.get() == 0xFFFD);
		CHECK(converter.put('C').status == UnicodeDecodeStatus::Ready);
		CHECK(converter.get() == U'C');

		CHECK(converter.put(prefix).status == UnicodeDecodeStatus::NeedMore);
		converter.reset();
		CHECK(converter.get() == U'\0');
		CHECK_FALSE(converter.finish());
		CHECK(converter.put(suffix).status == UnicodeDecodeStatus::Invalid);
		converter.reset();
		converter.reset();
		CHECK(converter.put(prefix).status == UnicodeDecodeStatus::NeedMore);
		CHECK(converter.put(suffix).status == UnicodeDecodeStatus::Ready);
		CHECK(converter.get() == expected);
	};

	check(utf8, static_cast<char8>(0xC2), static_cast<char8>(0xA2), U'\u00A2');
	check(utf16, char16{ 0xD800 }, char16{ 0xDC00 }, U'\U00010000');

	// Reset after a restricted UTF-8 lead must not constrain the next sequence.
	CHECK(utf8.put(static_cast<char8>(0xED)).status == UnicodeDecodeStatus::NeedMore);
	utf8.reset();
	CHECK(utf8.put(static_cast<char8>(0xE0)).status == UnicodeDecodeStatus::NeedMore);
	CHECK(utf8.put(static_cast<char8>(0xA0)).status == UnicodeDecodeStatus::NeedMore);
	CHECK(utf8.put(static_cast<char8>(0x80)).status == UnicodeDecodeStatus::Ready);
	CHECK(utf8.get() == 0x800);
}

TEST_CASE("UnicodeConverter.invalidUTF32")
{
	UTF32toUTF8_Converter utf8;
	UTF32toUTF16_Converter utf16;
	const auto check = [&](const char32 code)
	{
		CAPTURE(static_cast<uint32>(code));
		CHECK(utf8.put(code) == 3);
		CHECK(std::string_view(utf8.data(), 3) == "\xEF\xBF\xBD");
		CHECK(utf16.put(code) == 1);
		CHECK(utf16.data()[0] == 0xFFFD);
	};

	for (char32 code = 0xD800; code <= 0xDFFF; ++code)
	{
		check(code);
	}

	check(0x110000);
	check(0x7FFFFFFF);
	check(0xFFFFFFFF);
}
