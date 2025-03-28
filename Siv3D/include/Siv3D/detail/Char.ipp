﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	namespace detail
	{
		inline constexpr char32 HalfWidthSpace{ U' ' };

		inline constexpr char32 FullWidthSpace{ U'　' };

		[[nodiscard]]
		constexpr int32 Compare(char32 a, char32 b) noexcept
		{
			return ((a < b) ? -1 : (a > b));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsAscii
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsAscii(const char32 ch) noexcept
	{
		return (ch <= 0x7F);
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsDigit
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsDigit(const char32 ch) noexcept
	{
		return ((ch - U'0') <= (U'9' - U'0'));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsLower
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsLower(const char32 ch) noexcept
	{
		return ((ch - U'a') <= (U'z' - U'a'));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsUpper
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsUpper(const char32 ch) noexcept
	{
		return ((ch - U'A') <= (U'Z' - U'A'));
	}

	////////////////////////////////////////////////////////////////
	//
	//	ToLower
	//
	////////////////////////////////////////////////////////////////

	constexpr char32 ToLower(const char32 ch) noexcept
	{
		if (IsUpper(ch))
		{
			return (ch + ('a' - 'A'));
		}
		else
		{
			return ch;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ToUpper
	//
	////////////////////////////////////////////////////////////////

	constexpr char32 ToUpper(const char32 ch) noexcept
	{
		if (IsLower(ch))
		{
			return (ch - ('a' - 'A'));
		}
		else
		{
			return ch;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsAlpha
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsAlpha(const char32 ch) noexcept
	{
		return (IsLower(ch) || IsUpper(ch));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsAlnum
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsAlnum(const char32 ch) noexcept
	{
		return (IsDigit(ch) || IsAlpha(ch));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsXdigit
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsXdigit(const char32 ch) noexcept
	{
		return (IsDigit(ch)
			|| ((ch - U'A') <= (U'F' - U'A'))
			|| ((ch - U'a') <= (U'f' - U'a')));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsControl
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsControl(const char32 ch) noexcept
	{
		return ((ch <= 0x1F) || ((ch - 0x7F) <= (0x9F - 0x7F)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsBlank
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsBlank(const char32 ch) noexcept
	{
		return ((ch == detail::HalfWidthSpace)
			|| (ch == U'\t')
			|| (ch == detail::FullWidthSpace));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsSpace
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsSpace(const char32 ch) noexcept
	{
		return ((ch == detail::HalfWidthSpace)
			|| ((ch - U'\t') <= (U'\r' - U'\t'))
			|| (ch == detail::FullWidthSpace));
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsPrint
	//
	////////////////////////////////////////////////////////////////

	inline bool IsPrint(const char32 ch) noexcept
	{
		if (ch < 0xFFFF)
		{
			return static_cast<bool>(::iswprint(static_cast<wchar_t>(ch)));
		}
		else
		{
			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsHiragana
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsHiragana(const char32 ch) noexcept
	{
		return (((0x3041 <= ch) && (ch <= 0x3096))		// Hiragana
			 || ((0x3099 <= ch) && (ch <= 0x309F)));	// Hiragana
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsKatakana
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsKatakana(const char32 ch) noexcept
	{
		return (((0x30A0 <= ch) && (ch <= 0x30FF))		// Katakana
			 || ((0x31F0 <= ch) && (ch <= 0x31FF))		// Katakana Phonetic Extensions
			 || ((0xFF65 <= ch) && (ch <= 0xFF9F)));	// Halfwidth Katakana
	}

	////////////////////////////////////////////////////////////////
	//
	//	IsCJKUnifiedIdeograph
	//
	////////////////////////////////////////////////////////////////

	constexpr bool IsCJKUnifiedIdeograph(const char32 ch) noexcept
	{
		return (((0x4E00 <= ch) && (ch <= 0x9FFF))		// CJK Unified Ideographs
			 || ((0x3400 <= ch) && (ch <= 0x4DBF))		// CJK Unified Ideographs Extension A
			 || ((0x20000 <= ch) && (ch <= 0x2A6DF))	// CJK Unified Ideographs Extension B
			 || ((0x2A700 <= ch) && (ch <= 0x2B739))	// CJK Unified Ideographs Extension C
			 || ((0x2B740 <= ch) && (ch <= 0x2B81D))	// CJK Unified Ideographs Extension D
			 || ((0x2B820 <= ch) && (ch <= 0x2CEA1)));	// CJK Unified Ideographs Extension E
	}

	////////////////////////////////////////////////////////////////
	//
	//	CaseInsensitiveCompare
	//
	////////////////////////////////////////////////////////////////

	constexpr int32 CaseInsensitiveCompare(const char32 a, const char32 b) noexcept
	{
		if (IsAlpha(a) && IsAlpha(b))
		{
			return detail::Compare(ToLower(a), ToLower(b));
		}
		else
		{
			return detail::Compare(a, b);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	CaseInsensitiveEquals
	//
	////////////////////////////////////////////////////////////////

	constexpr bool CaseInsensitiveEquals(const char32 a, const char32 b) noexcept
	{
		return (ToLower(a) == ToLower(b));
	}
}
