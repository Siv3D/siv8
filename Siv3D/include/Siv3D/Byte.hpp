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
# include <cstddef>
# include <array>
# include "Common.hpp"
# include "LetterCase.hpp"

namespace s3d
{
	class String;
	struct FormatData;

	/// @brief ビット列としてのバイトデータを表す型 | Type representing byte data as a bit sequence
	using Byte = std::byte;

	/// @brief バイトデータを文字列に変換します。 | Converts byte data to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	/// @remark `ToHex(value, LetterCase::Upper)` と同じです。 | It is the same as `ToHex(value, LetterCase::Upper)`.
	String ToString(Byte value);

	/// @brief バイトデータを指定した基数の文字列に変換します。 | Converts byte data to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(Byte value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief バイトデータを 2 進数の文字列に変換します。 | Converts byte data to a binary string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBinary(Byte value);

	/// @brief バイトデータを 8 進数の文字列に変換します。 | Converts byte data to an octal string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToOctal(Byte value);

	/// @brief バイトデータを 16 進数の文字列に変換します。 | Converts byte data to a hexadecimal string.
	/// @param value 変換する値 | The value to convert
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToHex(Byte value, LetterCase letterCase = LetterCase::Upper);

	/// @brief バイトデータを 16 進数の文字列（小文字）に変換します。 | Converts byte data to a hexadecimal string (lowercase).
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	std::array<char, 2> ToHexCharsLowerCase(Byte value);

	/// @brief バイトデータを 16 進数の文字列（大文字）に変換します。 | Converts byte data to a hexadecimal string (uppercase).
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	std::array<char, 2> ToHexCharsUpperCase(Byte value);

	void Formatter(FormatData& formatData, const Byte& value);
}
