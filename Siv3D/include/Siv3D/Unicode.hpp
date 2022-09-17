//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
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
//	String Widen(std::string_view s);
//	String FromWstring(std::wstring_view s);
//	String FromUTF8(std::string_view s);
//	String FromUTF16(std::u16string_view s);
//	String FromUTF32(std::u32string_view s);
//	std::string NarrowAscii(StringView asciiText);
//	std::string Narrow(StringView s);
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
	using StringView = std::u32string_view;
	using String = std::u32string;

	/// @brief 文字コードの変換
	namespace Unicode
	{
		/// @brief ASCII 文字列を String に変換します。
		/// @param asciiText ASCII 文字で構成された文字列
		/// @remark Widen() 関数より高速に動作します。
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		String WidenAscii(std::string_view asciiText);

		/// @brief 文字列を String に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		String Widen(std::string_view s);

		/// @brief ワイド文字列を String に変換します。
		/// @param s 変換するワイド文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		String FromWstring(std::wstring_view s);

		/// @brief UTF-8 文字列を String に変換します。
		/// @param s 変換する UTF-8 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		String FromUTF8(std::string_view s);

		/// @brief UTF-16 文字列を String に変換します。
		/// @param s 変換する UTF-16 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		String FromUTF16(std::u16string_view s);

		/// @brief UTF-32 文字列を String に変換します。
		/// @param s 変換する UTF-32 文字列
		/// @return 変換された文字列
		[[nodiscard]]
		String FromUTF32(std::u32string_view s);

		/// @brief ASCII 文字列を std::string に変換します。
		/// @param asciiText ASCII 文字で構成された文字列
		/// @remark Narrow() 関数より高速に動作します。
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::string NarrowAscii(StringView asciiText);

		/// @brief String を std::string に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::string Narrow(StringView s);

		/// @brief String を std::wstring に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::wstring ToWstring(StringView s);

		/// @brief String を UTF-8 文字列に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::string ToUTF8(StringView s);

		/// @brief String を UTF-16 文字列に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::u16string ToUTF16(StringView s);

		/// @brief String を UTF-32 文字列に変換します。
		/// @param s 変換する文字列
		/// @return 変換された文字列
		[[nodiscard]]
		std::u32string ToUTF32(StringView s);

		/// @brief UTF-8 文字列を UTF-16 文字列に変換します。
		/// @param s UTF-8 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::u16string UTF8ToUTF16(std::string_view s);

		/// @brief UTF-8 文字列を UTF-32 文字列に変換します。
		/// @param s UTF-8 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::u32string UTF8ToUTF32(std::string_view s);

		/// @brief UTF-16 文字列を UTF-8 文字列に変換します。
		/// @param s UTF-16 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::string UTF16ToUTF8(std::u16string_view s);

		/// @brief UTF-16 文字列を UTF-32 文字列に変換します。
		/// @param s UTF-16 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::u32string UTF16ToUTF32(std::u16string_view s);

		/// @brief UTF-32 文字列を UTF-8 文字列に変換します。
		/// @param s UTF-32 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::string UTF32ToUTF8(std::u32string_view s);

		/// @brief UTF-32 文字列を UTF-16 文字列に変換します。
		/// @param s UTF-32 文字列
		/// @return 変換された文字列。入力が不正な場合は空の文字列
		[[nodiscard]]
		std::u16string UTF32ToUTF16(std::u32string_view s);

		/// @brief 上位サロゲートであるかを返します
		/// @param ch UTF-16 の文字要素
		/// @return 上位サロゲートである場合 true, それ以外の場合は false
		[[nodiscard]]
		inline constexpr bool IsHighSurrogate(const char16 ch) noexcept;

		/// @brief 下位サロゲートであるかを返します
		/// @param ch UTF-16 の文字要素
		/// @return 下位サロゲートである場合 true, それ以外の場合は false
		[[nodiscard]]
		inline constexpr bool IsLowSurrogate(const char16 ch) noexcept;
	}
}

# include "detail/Unicode.ipp"
