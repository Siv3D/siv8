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
# include <limits>
# include <bit>

namespace
{
	template <class View, class Destination>
	void CheckUnicodeDestination(void (*write)(View, Destination&), Destination (*make)(View),
		const View input, const Destination& expected)
	{
		Destination dst;
		dst.reserve(expected.size() + 256);
		dst.assign(128, static_cast<typename Destination::value_type>('X'));
		const auto* storage = dst.data();

		write(input, dst);
		CHECK((dst) == (expected));
		CHECK(dst.data() == storage);
		CHECK((make(input)) == (expected));

		write(View{}, dst);
		CHECK(dst.empty());
		CHECK(dst.data() == storage);

		write(input, dst);
		CHECK((dst) == (expected));
		CHECK(dst.data() == storage);
	}

	template <class View, class Destination>
	void CheckInvalidUnicodeDestination(void (*write)(View, Destination&), const View input)
	{
		Destination dst(256, static_cast<typename Destination::value_type>('X'));
		write(input, dst);
		CHECK(dst.empty());
	}
}

TEST_CASE("Unicode.Destination.NormalAndReuse")
{
	constexpr char text8[] = "A\0あ\U0001F600\0";
	constexpr char16 text16[] = u"A\0あ\U0001F600\0";
	constexpr char32 text32[] = U"A\0あ\U0001F600\0";
	constexpr wchar_t textWide[] = L"A\0あ\U0001F600\0";
	const std::string utf8{ text8, (std::size(text8) - 1) };
	const std::u16string utf16{ text16, (std::size(text16) - 1) };
	const std::u32string utf32{ text32, (std::size(text32) - 1) };
	const std::wstring wide{ textWide, (std::size(textWide) - 1) };
	const String text{ utf32 };
	std::u16string utf16BE = utf16;
	for (auto& ch : utf16BE)
	{
		ch = static_cast<char16>(std::byteswap(static_cast<uint16>(ch)));
	}

	CheckUnicodeDestination<std::string_view, String>(Unicode::FromUTF8, Unicode::FromUTF8, utf8, text);
	CheckUnicodeDestination<std::u16string_view, String>(Unicode::FromUTF16, Unicode::FromUTF16, utf16, text);
	CheckUnicodeDestination<std::u16string_view, String>(Unicode::FromUTF16BE, Unicode::FromUTF16BE, utf16BE, text);
	CheckUnicodeDestination<std::wstring_view, String>(Unicode::FromWstring, Unicode::FromWstring, wide, text);
	CheckUnicodeDestination<std::u32string_view, String>(Unicode::FromUTF32, Unicode::FromUTF32, utf32, text);
	CheckUnicodeDestination<StringView, std::string>(Unicode::ToUTF8, Unicode::ToUTF8, text, utf8);
	CheckUnicodeDestination<StringView, std::u16string>(Unicode::ToUTF16, Unicode::ToUTF16, text, utf16);
	CheckUnicodeDestination<StringView, std::wstring>(Unicode::ToWstring, Unicode::ToWstring, text, wide);
	CheckUnicodeDestination<std::string_view, std::wstring>(Unicode::ToWstring, Unicode::ToWstring, utf8, wide);
	CheckUnicodeDestination<StringView, std::u32string>(Unicode::ToUTF32, Unicode::ToUTF32, text, utf32);
	CheckUnicodeDestination<std::string_view, std::u16string>(Unicode::UTF8ToUTF16, Unicode::UTF8ToUTF16, utf8, utf16);
	CheckUnicodeDestination<std::string_view, std::u32string>(Unicode::UTF8ToUTF32, Unicode::UTF8ToUTF32, utf8, utf32);
	CheckUnicodeDestination<std::u16string_view, std::string>(Unicode::UTF16ToUTF8, Unicode::UTF16ToUTF8, utf16, utf8);
	CheckUnicodeDestination<std::u16string_view, std::u32string>(Unicode::UTF16ToUTF32, Unicode::UTF16ToUTF32, utf16, utf32);
	CheckUnicodeDestination<std::u32string_view, std::string>(Unicode::UTF32ToUTF8, Unicode::UTF32ToUTF8, utf32, utf8);
	CheckUnicodeDestination<std::u32string_view, std::u16string>(Unicode::UTF32ToUTF16, Unicode::UTF32ToUTF16, utf32, utf16);

	for (const size_t length : { 0, 1, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 4096 })
	{
		CAPTURE(length);
		const std::string ascii(length, '\x7F');
		const String ascii32(length, U'\x7F');
		const std::wstring asciiWide(length, L'\x7F');
		CheckUnicodeDestination<std::string_view, String>(Unicode::FromAscii, Unicode::FromAscii, ascii, ascii32);
		CheckUnicodeDestination<StringView, std::string>(Unicode::ToAscii, Unicode::ToAscii, ascii32, ascii);
		CheckUnicodeDestination<std::wstring_view, String>(Unicode::FromWstring, Unicode::FromWstring, asciiWide, ascii32);
		CheckUnicodeDestination<StringView, std::wstring>(Unicode::ToWstring, Unicode::ToWstring, ascii32, asciiWide);
	}
}

TEST_CASE("Unicode.Destination.InvalidInput")
{
	for (const size_t padding : { 0, 15, 16, 31, 32, 63, 64 })
	{
		CAPTURE(padding);
		const std::string invalid8 = (std::string(padding, 'A') + "\xFF");
		const std::u16string invalid16 = (std::u16string(padding, u'A') + char16{ 0xD800 });
		std::u16string invalid16BE = invalid16;
		for (auto& ch : invalid16BE)
		{
			ch = static_cast<char16>(std::byteswap(static_cast<uint16>(ch)));
		}
		const std::u32string invalid32 = (std::u32string(padding, U'A') + char32{ 0x110000 });
		const String invalidText{ invalid32 };
		const std::wstring invalidWide = (std::wstring(padding, L'A') + wchar_t{ 0xD800 });

		CheckInvalidUnicodeDestination<std::string_view, String>(Unicode::FromAscii, invalid8);
		CheckInvalidUnicodeDestination<std::string_view, String>(Unicode::FromUTF8, invalid8);
		CheckInvalidUnicodeDestination<std::u16string_view, String>(Unicode::FromUTF16, invalid16);
		CheckInvalidUnicodeDestination<std::u16string_view, String>(Unicode::FromUTF16BE, invalid16BE);
		CheckInvalidUnicodeDestination<std::wstring_view, String>(Unicode::FromWstring, invalidWide);
		CheckInvalidUnicodeDestination<StringView, std::string>(Unicode::ToAscii, invalidText);
		CheckInvalidUnicodeDestination<StringView, std::string>(Unicode::ToUTF8, invalidText);
		CheckInvalidUnicodeDestination<StringView, std::u16string>(Unicode::ToUTF16, invalidText);
		CheckInvalidUnicodeDestination<StringView, std::wstring>(Unicode::ToWstring, invalidText);
		CheckInvalidUnicodeDestination<std::string_view, std::wstring>(Unicode::ToWstring, invalid8);
		CheckInvalidUnicodeDestination<std::string_view, std::u16string>(Unicode::UTF8ToUTF16, invalid8);
		CheckInvalidUnicodeDestination<std::string_view, std::u32string>(Unicode::UTF8ToUTF32, invalid8);
		CheckInvalidUnicodeDestination<std::u16string_view, std::string>(Unicode::UTF16ToUTF8, invalid16);
		CheckInvalidUnicodeDestination<std::u16string_view, std::u32string>(Unicode::UTF16ToUTF32, invalid16);
		CheckInvalidUnicodeDestination<std::u32string_view, std::string>(Unicode::UTF32ToUTF8, invalid32);
		CheckInvalidUnicodeDestination<std::u32string_view, std::u16string>(Unicode::UTF32ToUTF16, invalid32);

		CheckUnicodeDestination<std::u32string_view, String>(Unicode::FromUTF32, Unicode::FromUTF32, invalid32, invalidText);
		CheckUnicodeDestination<StringView, std::u32string>(Unicode::ToUTF32, Unicode::ToUTF32, invalidText, invalid32);
	}
}

TEST_CASE("Unicode.Destination.UTF32SelfReference")
{
	for (const size_t length : { 0, 1, 4, 31, 32, 128, 4096 })
	{
		String original(length, U'A');
		for (size_t i = 0; i < length; ++i)
		{
			original[i] = static_cast<char32>(i);
		}
		for (const auto [offset, count] : {
			std::pair<size_t, size_t>{ 0, length },
			std::pair<size_t, size_t>{ length / 2, length / 2 },
			std::pair<size_t, size_t>{ length, 0 } })
		{
			CAPTURE(length);
			CAPTURE(offset);
			String dst = original;
			const String expected = original.substr(offset, count);
			Unicode::FromUTF32(std::u32string_view{ dst.data() + offset, count }, dst);
			CHECK((dst) == (expected));

			std::u32string dst32(original.begin(), original.end());
			Unicode::ToUTF32(StringView{ dst32.data() + offset, count }, dst32);
			CHECK((dst32) == (std::u32string(expected.begin(), expected.end())));
		}
	}
}

TEST_CASE("Unicode.FromAscii.RoundTrip")
{
	CHECK(Unicode::FromAscii(std::string_view{}).isEmpty());

	for (const size_t length : { 0, 1, 15, 31, 32, 33, 63, 64, 65, 127, 128, 129, 256, 4096 })
	{
		for (const size_t offset : { 1, 3 })
		{
			CAPTURE(length);
			CAPTURE(offset);
			std::string storage((offset + length + 1), '\xFF');
			String expected;
			for (size_t i = 0; i < length; ++i)
			{
				const uint32 cp = ((i * 17) % 128);
				storage[offset + i] = static_cast<char>(cp);
				expected.push_back(static_cast<char32>(cp));
			}

			const std::string_view input{ (storage.data() + offset), length };
			const auto result = Unicode::FromAscii(input);
			CHECK((result) == (expected));
			CHECK((Unicode::ToAscii(result)) == (input));
		}
	}
}

TEST_CASE("Unicode.FromAscii.InvalidInput")
{
	for (uint32 invalid = 0x80; invalid <= 0xFF; ++invalid)
	{
		for (const size_t length : { 1, 31, 32, 33, 63, 64, 65, 129 })
		{
			for (const size_t position : { size_t{ 0 }, (length / 2), (length - 1) })
			{
				CAPTURE(invalid);
				CAPTURE(length);
				CAPTURE(position);
				std::string storage((length + 2), 'A');
				storage[position + 1] = static_cast<char>(invalid);
				CHECK(Unicode::FromAscii(std::string_view{ (storage.data() + 1), length }).isEmpty());
			}
		}
	}

	for (const size_t padding : { 0, 29, 32, 64 })
	{
		CHECK(Unicode::FromAscii(std::string(padding, 'A') + "あ").isEmpty());
	}
}

TEST_CASE("Unicode.Wstring.RoundTrip")
{
	CHECK(Unicode::FromWstring(std::wstring_view{}).isEmpty());
	CHECK(Unicode::ToWstring(StringView{}).empty());

	// Include embedded nulls, scalar boundaries, and valid noncharacters.
	constexpr char32 text[] = U"\0A\x7F\x80\x7FF\x800\xD7FF\xE000\xFDD0\xFFFE\xFFFF\0\U00010000\U0010FFFF\0";
	constexpr wchar_t wideText[] = L"\0A\x7F\x80\x7FF\x800\xD7FF\xE000\xFDD0\xFFFE\xFFFF\0\U00010000\U0010FFFF\0";
	const String expected{ text, (std::size(text) - 1) };
	const std::wstring wide{ wideText, (std::size(wideText) - 1) };

	CHECK((Unicode::FromWstring(wide)) == (expected));
	CHECK((Unicode::ToWstring(expected)) == (wide));
	CHECK((Unicode::FromWstring(Unicode::ToWstring(expected))) == (expected));

	for (const size_t length : { 1, 4, 7, 8, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 256, 4096 })
	{
		CAPTURE(length);
		const String s(length, U'あ');
		const std::wstring w(length, L'あ');
		CHECK((Unicode::FromWstring(w)) == (s));
		CHECK((Unicode::ToWstring(s)) == (w));
	}
}

TEST_CASE("Unicode.ToWstring.InvalidUTF32")
{
	for (const char32 invalid : { char32{ 0xD800 }, char32{ 0xDBFF }, char32{ 0xDC00 }, char32{ 0xDFFF }, char32{ 0x110000 }, char32{ 0xFFFFFFFF } })
	{
		for (const size_t length : { 1, 7, 8, 15, 16, 17, 31, 32, 33, 63, 64, 65, 129, 4096 })
		{
			for (const size_t position : { size_t{ 0 }, (length / 2), (length - 1) })
			{
				CAPTURE(static_cast<uint32>(invalid));
				CAPTURE(length);
				CAPTURE(position);
				String s(length, U'A');
				s[position] = invalid;
				CHECK(Unicode::ToWstring(s).empty());
			}
		}
	}
}

TEST_CASE("Unicode.FromWstring.InvalidInput")
{
	for (const wchar_t invalid : { wchar_t{ 0xD800 }, wchar_t{ 0xDBFF }, wchar_t{ 0xDC00 }, wchar_t{ 0xDFFF } })
	{
		for (const size_t length : { 1, 7, 8, 15, 16, 17, 31, 32, 33, 63, 64, 65, 129, 4096 })
		{
			for (const size_t position : { size_t{ 0 }, (length / 2), (length - 1) })
			{
				CAPTURE(static_cast<uint32>(invalid));
				CAPTURE(length);
				CAPTURE(position);
				std::wstring s(length, L'A');
				s[position] = invalid;
				CHECK(Unicode::FromWstring(s).isEmpty());
			}
		}
	}

# if not SIV3D_PLATFORM(WINDOWS)

	for (const wchar_t invalid : { wchar_t{ 0x110000 }, std::numeric_limits<wchar_t>::max(), static_cast<wchar_t>(-1), static_cast<wchar_t>(0x80000000u) })
	{
		for (const size_t position : { size_t{ 0 }, size_t{ 32 }, size_t{ 64 } })
		{
			CAPTURE(static_cast<uint32>(invalid));
			CAPTURE(position);
			std::wstring s(65, L'A');
			s[position] = invalid;
			CHECK(Unicode::FromWstring(s).isEmpty());
		}
	}

# endif
}

TEST_CASE("Unicode.Wstring.AllScalars")
{
	String s;
	s.reserve(0x110000 - 0x800);
	for (uint32 cp = 0; cp <= 0x10FFFF; ++cp)
	{
		if ((cp < 0xD800) || (0xDFFF < cp))
		{
			s.push_back(static_cast<char32>(cp));
		}
	}

	const std::wstring wide = Unicode::ToWstring(s);
	CHECK((Unicode::FromWstring(wide)) == (s));
}

TEST_CASE("Unicode.ValidateAscii")
{
	{
		const std::string s = "abcdef";
		CHECK((Unicode::ValidateAscii(s).has_value()) == (true));
	}

	{
		const std::string s = "abcあdef";
		CHECK((Unicode::ValidateAscii(s).has_value()) == (false));
		CHECK((Unicode::ValidateAscii(s).error()) == (3u));
	}

	{
		std::string s = "\x80" "abcdef";
		CHECK((Unicode::ValidateAscii(s).has_value()) == (false));
		CHECK((Unicode::ValidateAscii(s).error()) == (0u));
	}

	{
		std::string s = "abc" "\x80" "def";
		CHECK((Unicode::ValidateAscii(s).has_value()) == (false));
		CHECK((Unicode::ValidateAscii(s).error()) == (3u));
	}

	{
		const std::string s = "abcdef\x80";
		CHECK((Unicode::ValidateAscii(s).has_value()) == (false));
		CHECK((Unicode::ValidateAscii(s).error()) == (6u));
	}

}

TEST_CASE("Unicode.ValidateUTF8")
{
	{
		const std::string s = "abcdef";
		CHECK((Unicode::ValidateUTF8(s).has_value()) == (true));
	}

	{
		const std::string s = "abcあdef";
		CHECK((Unicode::ValidateUTF8(s).has_value()) == (true));
	}

	{
		std::string s = "\x80" "abcdef";
		CHECK((Unicode::ValidateUTF8(s).has_value()) == (false));
		CHECK((Unicode::ValidateUTF8(s).error()) == (0u));
	}

	{
		std::string s = "abc" "\x80" "def";
		CHECK((Unicode::ValidateUTF8(s).has_value()) == (false));
		CHECK((Unicode::ValidateUTF8(s).error()) == (3u));
	}

	{
		const std::string s = "abcあdef\x80";
		CHECK((Unicode::ValidateUTF8(s).has_value()) == (false));
		CHECK((Unicode::ValidateUTF8(s).error()) == (9u));
	}

	{
		const std::string s = "あいうえお";
		CHECK((Unicode::ValidateUTF8(s).has_value()) == (true));
	}
}
