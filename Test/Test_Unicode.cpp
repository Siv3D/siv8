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

TEST_CASE("Unicode.Wstring.RoundTrip")
{
	CHECK(Unicode::FromWstring(std::wstring_view{}).isEmpty());
	CHECK(Unicode::ToWstring(StringView{}).empty());

	// Include embedded nulls, scalar boundaries, and valid noncharacters.
	constexpr char32 text[] = U"\0A\x7F\x80\x7FF\x800\xD7FF\xE000\xFDD0\xFFFE\xFFFF\0\U00010000\U0010FFFF\0";
	constexpr wchar_t wideText[] = L"\0A\x7F\x80\x7FF\x800\xD7FF\xE000\xFDD0\xFFFE\xFFFF\0\U00010000\U0010FFFF\0";
	const String expected{ text, (std::size(text) - 1) };
	const std::wstring wide{ wideText, (std::size(wideText) - 1) };

	CHECK_EQ(Unicode::FromWstring(wide), expected);
	CHECK_EQ(Unicode::ToWstring(expected), wide);
	CHECK_EQ(Unicode::FromWstring(Unicode::ToWstring(expected)), expected);

	for (const size_t length : { 1, 4, 7, 8, 15, 16, 17, 31, 32, 33, 63, 64, 65, 127, 128, 129, 256, 4096 })
	{
		CAPTURE(length);
		const String s(length, U'あ');
		const std::wstring w(length, L'あ');
		CHECK_EQ(Unicode::FromWstring(w), s);
		CHECK_EQ(Unicode::ToWstring(s), w);
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
	CHECK_EQ(Unicode::FromWstring(wide), s);
}

TEST_CASE("Unicode.ValidateAscii")
{
	{
		const std::string s = "abcdef";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), true);
	}

	{
		const std::string s = "abcあdef";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 3u);
	}

	{
		std::string s = "\x80" "abcdef";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 0u);
	}

	{
		std::string s = "abc" "\x80" "def";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 3u);
	}

	{
		const std::string s = "abcdef\x80";
		CHECK_EQ(Unicode::ValidateAscii(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateAscii(s).error(), 6u);
	}

}

TEST_CASE("Unicode.ValidateUTF8")
{
	{
		const std::string s = "abcdef";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), true);
	}

	{
		const std::string s = "abcあdef";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), true);
	}

	{
		std::string s = "\x80" "abcdef";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateUTF8(s).error(), 0u);
	}

	{
		std::string s = "abc" "\x80" "def";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateUTF8(s).error(), 3u);
	}

	{
		const std::string s = "abcあdef\x80";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), false);
		CHECK_EQ(Unicode::ValidateUTF8(s).error(), 9u);
	}

	{
		const std::string s = "あいうえお";
		CHECK_EQ(Unicode::ValidateUTF8(s).has_value(), true);
	}
}
