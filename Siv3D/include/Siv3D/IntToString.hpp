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
# include "String.hpp"
# include "LetterCase.hpp"
# include "Concepts.hpp"

namespace s3d
{
	//////////////////////////////////////////////////
	//
	//	ToString
	//
	//////////////////////////////////////////////////

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(int8 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(uint8 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(int16 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(uint16 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(int32 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(uint32 value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(long value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(unsigned long value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(long long value);

	/// @brief 整数を文字列に変換します。 | Converts an integer to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(unsigned long long value);

	//////////////////////////////////////////////////
	//
	//	ToBaseN
	//
	//////////////////////////////////////////////////

	[[nodiscard]]
	String ToBaseN(bool value, int32 radix, LetterCase letterCase = LetterCase::Upper) = delete;

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(int8 value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(uint8 value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(int16 value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(uint16 value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(int32 value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(uint32 value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(long value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(unsigned long value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(long long value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	/// @brief 整数を指定した基数の文字列に変換します。 | Converts an integer to a string in the specified radix.
	/// @param value 変換する値 | The value to convert
	/// @param radix 基数 | Radix
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBaseN(unsigned long long value, int32 radix, LetterCase letterCase = LetterCase::Upper);

	//////////////////////////////////////////////////
	//
	//	ToBinary
	//
	//////////////////////////////////////////////////

	/// @brief 整数を 2 進数の文字列に変換します。 | Converts an integer to a binary string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBinary(Concept::Integral auto value);

	//////////////////////////////////////////////////
	//
	//	ToOctal
	//
	//////////////////////////////////////////////////

	/// @brief 整数を 8 進数の文字列に変換します。 | Converts an integer to an octal string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToOctal(Concept::Integral auto value);

	//////////////////////////////////////////////////
	//
	//	ToHex
	//
	//////////////////////////////////////////////////

	/// @brief 整数を 16 進数の文字列に変換します。 | Converts an integer to a hexadecimal string.
	/// @param value 変換する値 | The value to convert
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToHex(Concept::Integral auto value, LetterCase letterCase = LetterCase::Upper);
}

# include "detail/IntToString.ipp"
