//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <string>
# include "Common.hpp"

//////////////////////////////////////////////////
//
//	文字コードの変換 | Character Code Conversion
//
//	String WidenAscii(std::string_view asciiText);
//	String FromWstring(std::wstring_view s);
//	String FromUTF8(std::string_view s);
//	String FromUTF16(std::u16string_view s);
//	String FromUTF32(std::u32string_view s);
//	std::string NarrowAscii(StringView asciiText);
//	std::wstring ToWstring(std::string_view s);
//	std::wstring ToWstring(StringView s);
//	std::string ToUTF8(StringView s);
//	std::u16string ToUTF16(StringView s);
//	std::u32string ToUTF32(StringView s);
//	std::u16string UTF8ToUTF16(std::string_view s);
//	std::u32string UTF8ToUTF32(std::string_view s);
//	std::string UTF16ToUTF8(std::u16string_view s);
//	std::u32string UTF16ToUTF32(std::u16string_view s);
//	std::string UTF32ToUTF8(std::u32string_view s);
//	std::u16string UTF32ToUTF16(std::u32string_view s);
//	bool IsHighSurrogate(const char16 ch);
//	bool IsLowSurrogate(const char16 ch);
// 
//////////////////////////////////////////////////

namespace s3d
{
	class StringView;
	class String;

	/// @brief 文字コードの変換 | Character Code Conversion
	namespace Unicode
	{
		/// @brief ASCII 文字列を String に変換します。 | Converts an ASCII string to String.
		/// @param asciiText ASCII 文字で構成された文字列 | A string composed of ASCII characters
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String WidenAscii(std::string_view asciiText);

		[[deprecated("Use StringView::FromUTF8() instead")]]
		String Widen(std::string_view s);

		/// @brief ワイド文字列を String に変換します。 | Converts a wide string to String.
		/// @param s 変換するワイド文字列 | The wide string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromWstring(std::wstring_view s);

		/// @brief UTF-8 文字列を String に変換します。 | Converts a UTF-8 string to String.
		/// @param s 変換する UTF-8 文字列 | The UTF-8 string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromUTF8(std::string_view s);

		/// @brief UTF-16 文字列を String に変換します。 | Converts a UTF-16 string to String.
		/// @param s 変換する UTF-16 文字列 | The UTF-16 string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromUTF16(std::u16string_view s);

		/// @brief UTF-32 文字列を String に変換します。 | Converts a UTF-32 string to String.
		/// @param s 変換する UTF-32 文字列 | The UTF-32 string to convert
		/// @return 変換された文字列 | The converted string
		[[nodiscard]]
		String FromUTF32(std::u32string_view s);

		/// @brief ASCII 文字列を std::string に変換します。 | Converts an ASCII string to std::string.
		/// @param asciiText ASCII 文字で構成された文字列 | A string composed of ASCII characters
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::string NarrowAscii(StringView asciiText);

		[[deprecated("Use StringView::ToUTF8() instead")]]
		std::string Narrow(StringView s);

		/// @brief std::string を std::wstring に変換します。 | Converts std::string to std::wstring.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::wstring ToWstring(std::string_view s);

		/// @brief String を std::wstring に変換します。 | Converts String to std::wstring.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::wstring ToWstring(StringView s);

		/// @brief String を UTF-8 文字列に変換します。 | Converts String to a UTF-8 string.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::string ToUTF8(StringView s);

		/// @brief String を UTF-16 文字列に変換します。 | Converts String to a UTF-16 string.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::u16string ToUTF16(StringView s);

		/// @brief String を UTF-32 文字列に変換します。 | Converts String to a UTF-32 string.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列 | The converted string
		[[nodiscard]]
		std::u32string ToUTF32(StringView s);

		/// @brief UTF-8 文字列を UTF-16 文字列に変換します。 | Converts a UTF-8 string to a UTF-16 string.
		/// @param s UTF-8 文字列 | A UTF-8 string
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::u16string UTF8ToUTF16(std::string_view s);

		/// @brief UTF-8 文字列を UTF-32 文字列に変換します。 | Converts a UTF-8 string to a UTF-32 string.
		/// @param s UTF-8 文字列 | A UTF-8 string
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::u32string UTF8ToUTF32(std::string_view s);

		/// @brief UTF-16 文字列を UTF-8 文字列に変換します。 | Converts a UTF-16 string to a UTF-8 string.
		/// @param s UTF-16 文字列 | A UTF-16 string
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::string UTF16ToUTF8(std::u16string_view s);

		/// @brief UTF-16 文字列を UTF-32 文字列に変換します。 | Converts a UTF-16 string to a UTF-32 string.
		/// @param s UTF-16 文字列 | A UTF-16 string
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::u32string UTF16ToUTF32(std::u16string_view s);

		/// @brief UTF-32 文字列を UTF-8 文字列に変換します。 | Converts a UTF-32 string to a UTF-8 string.
		/// @param s UTF-32 文字列 | A UTF-32 string
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::string UTF32ToUTF8(std::u32string_view s);

		/// @brief UTF-32 文字列を UTF-16 文字列に変換します。 | Converts a UTF-32 string to a UTF-16 string.
		/// @param s UTF-32 文字列 | A UTF-32 string
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::u16string UTF32ToUTF16(std::u32string_view s);

		/// @brief 上位サロゲートであるかを返します。 | Returns whether the character is a high surrogate.
		/// @param ch UTF-16 の文字要素 | A character element of UTF-16
		/// @return 上位サロゲートである場合 true, それ以外の場合は false | True if the character is a high surrogate, false otherwise
		[[nodiscard]]
		constexpr bool IsHighSurrogate(const char16 ch) noexcept;

		/// @brief 下位サロゲートであるかを返します。 | Returns whether the character is a low surrogate.
		/// @param ch UTF-16 の文字要素 | A character element of UTF-16
		/// @return 下位サロゲートである場合 true, それ以外の場合は false | True if the character is a low surrogate, false otherwise
		[[nodiscard]]
		constexpr bool IsLowSurrogate(const char16 ch) noexcept;
	}
}

# include "detail/Unicode.ipp"
