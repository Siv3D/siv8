//-----------------------------------------------
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
# include <string>
# include "Types.hpp"
# include "Result.hpp"

//////////////////////////////////////////////////
//
//	文字コードの変換 | Character Code Conversion
//
//	String FromAscii(std::string_view asciiText);
//	String FromUTF8(std::string_view s);
//	String FromUTF16(std::u16string_view s);
//	String FromUTF16BE(std::u16string_view s);
//	String FromWstring(std::wstring_view s);
//	String FromUTF32(std::u32string_view s);
//	std::string ToAscii(StringView asciiText);
//	std::string ToUTF8(StringView s);
//	std::u16string ToUTF16(StringView s);
//	std::wstring ToWstring(std::string_view s);
//	std::wstring ToWstring(StringView s);
//	std::u32string ToUTF32(StringView s);
//	std::u16string UTF8ToUTF16(std::string_view s);
//	std::u32string UTF8ToUTF32(std::string_view s);
//	std::string UTF16ToUTF8(std::u16string_view s);
//	std::u32string UTF16ToUTF32(std::u16string_view s);
//	std::string UTF32ToUTF8(std::u32string_view s);
//	std::u16string UTF32ToUTF16(std::u32string_view s);
//	Result<void, size_t> ValidateAscii(std::string_view s);
//	Result<void, size_t> ValidateUTF8(std::string_view s);
//	Result<void, size_t> ValidateUTF16(std::u16string_view s);
//	Result<void, size_t> ValidateUTF32(std::u32string_view s);
//	bool IsHighSurrogate(const char16 ch);
//	bool IsLowSurrogate(const char16 ch);
// 
//////////////////////////////////////////////////

namespace s3d
{
	class String;
	class StringView;

	/// @brief 文字コードの変換 | Character Code Conversion
	namespace Unicode
	{
		////////////////////////////////////////////////////////////////
		//
		//	FromAscii
		//
		////////////////////////////////////////////////////////////////

		/// @brief ASCII 文字列を String に変換します。 | Converts an ASCII string to String.
		/// @param asciiText ASCII 文字で構成された文字列 | A string composed of ASCII characters
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromAscii(std::string_view asciiText);

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 文字列を String に変換します。 | Converts a UTF-8 string to String.
		/// @param s 変換する UTF-8 文字列 | The UTF-8 string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromUTF8(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF16
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-16LE 文字列を String に変換します。 | Converts a UTF-16LE string to String.
		/// @param s 変換する UTF-16LE 文字列 | The UTF-16LE string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromUTF16(std::u16string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF16BE
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-16BE 文字列を String に変換します。 | Converts a UTF-16BE string to String.
		/// @param s 変換する UTF-16BE 文字列 | The UTF-16BE string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromUTF16BE(std::u16string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	FromWstring
		//
		////////////////////////////////////////////////////////////////

		/// @brief ワイド文字列を String に変換します。 | Converts a wide string to String.
		/// @param s 変換するワイド文字列 | The wide string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		String FromWstring(std::wstring_view s);

		////////////////////////////////////////////////////////////////
		//
		//	FromUTF32
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-32 文字列を String に変換します。 | Converts a UTF-32 string to String.
		/// @param s 変換する UTF-32 文字列 | The UTF-32 string to convert
		/// @return 変換された文字列 | The converted string
		[[nodiscard]]
		String FromUTF32(std::u32string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	ToAscii
		//
		////////////////////////////////////////////////////////////////

		/// @brief ASCII 文字列を std::string に変換します。 | Converts an ASCII string to std::string.
		/// @param asciiText ASCII 文字で構成された文字列 | A string composed of ASCII characters
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::string ToAscii(StringView asciiText);

		////////////////////////////////////////////////////////////////
		//
		//	ToUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief String を UTF-8 文字列に変換します。 | Converts String to a UTF-8 string.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::string ToUTF8(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	ToUTF16
		//
		////////////////////////////////////////////////////////////////

		/// @brief String を UTF-16 文字列に変換します。 | Converts String to a UTF-16 string.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列。入力が不正な場合は空の文字列 | The converted string. An empty string if the input is invalid
		[[nodiscard]]
		std::u16string ToUTF16(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	ToWstring
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	ToUTF32
		//
		////////////////////////////////////////////////////////////////

		/// @brief String を UTF-32 文字列に変換します。 | Converts String to a UTF-32 string.
		/// @param s 変換する文字列 | The string to convert
		/// @return 変換された文字列 | The converted string
		[[nodiscard]]
		std::u32string ToUTF32(StringView s);

		////////////////////////////////////////////////////////////////
		//
		//	UTF8ToUTF16, UTF8ToUTF32
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	UTF16ToUTF8, UTF16ToUTF32
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	UTF32ToUTF8, UTF32ToUTF16
		//
		////////////////////////////////////////////////////////////////

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

		////////////////////////////////////////////////////////////////
		//
		//	ValidateAscii
		//
		////////////////////////////////////////////////////////////////

		/// @brief ASCII 文字列が正しいかを検証します。 | Validates an ASCII string.
		/// @param s 文字列 | A string
		/// @return 検証結果。エラー時はエラーの位置 | The validation result, or the error position on failure
		[[nodiscard]]
		Result<void, size_t> ValidateAscii(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	ValidateUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 文字列を検証します。 | Validates a UTF-8 string.
		/// @param s 文字列 | A string
		/// @return 検証結果。エラー時はエラーの位置 | The validation result, or the error position on failure
		[[nodiscard]]
		Result<void, size_t> ValidateUTF8(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	ValidateUTF16
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-16LE 文字列を検証します。 | Validates a UTF-16LE string.
		/// @param s 文字列 | A string
		/// @return 検証結果。エラー時はエラーの位置 | The validation result, or the error position on failure
		[[nodiscard]]
		Result<void, size_t> ValidateUTF16(std::u16string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	ValidateUTF16BE
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-16BE 文字列を検証します。 | Validates a UTF-16BE string.
		/// @param s 文字列 | A string
		/// @return 検証結果。エラー時はエラーの位置 | The validation result, or the error position on failure
		[[nodiscard]]
		Result<void, size_t> ValidateUTF16BE(std::u16string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	ValidateUTF32
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-32 文字列を検証します。 | Validates a UTF-32 string.
		/// @param s 文字列 | A string
		/// @return 検証結果。エラー時はエラーの位置 | The validation result, or the error position on failure
		[[nodiscard]]
		Result<void, size_t> ValidateUTF32(std::u32string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	IsHighSurrogate, IsLowSurrogate
		//
		////////////////////////////////////////////////////////////////

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
