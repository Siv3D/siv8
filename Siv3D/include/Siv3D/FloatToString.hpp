//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "LetterCase.hpp"

namespace s3d
{
	class String;

	////////////////////////////////////////////////////////////////
	//
	//	ToSting
	//
	////////////////////////////////////////////////////////////////

	/// @brief 浮動小数点数を文字列に変換します。 | Converts a floating-point number to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(float value);

	/// @brief 浮動小数点数を文字列に変換します。 | Converts a floating-point number to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(double value);

	/// @brief 浮動小数点数を文字列に変換します。 | Converts a floating-point number to a string.
	/// @param value 変換する値 | The value to convert
	/// @param decimalPlace 小数点以下の桁数 | The number of digits after the decimal point
	/// @return 文字列 | String
	/// @remark 小数点以下に続く 0 は削除されます。 | The trailing 0s after the decimal point are removed.
	[[nodiscard]]
	String ToString(float value, int32 decimalPlace);

	/// @brief 浮動小数点数を文字列に変換します。 | Converts a floating-point number to a string.
	/// @param value 変換する値 | The value to convert
	/// @param decimalPlace 小数点以下の桁数 | The number of digits after the decimal point
	/// @return 文字列 | String
	/// @remark 小数点以下に続く 0 は削除されます。 | The trailing 0s after the decimal point are removed.
	[[nodiscard]]
	String ToString(double value, int32 decimalPlace);

	////////////////////////////////////////////////////////////////
	//
	//	ToFixed
	//
	////////////////////////////////////////////////////////////////

	/// @brief 浮動小数点数を文字列に変換します。 | Converts a floating-point number to a string.
	/// @param value 変換する値 | The value to convert
	/// @param decimalPlace 小数点以下の桁数 | The number of digits after the decimal point
	/// @return 文字列 | String
	/// @remark 小数点以下に続く 0 は削除されません。 | The trailing 0s after the decimal point are not removed.
	[[nodiscard]]
	String ToFixed(float value, int32 decimalPlace);

	/// @brief 浮動小数点数を文字列に変換します。 | Converts a floating-point number to a string.
	/// @param value 変換する値 | The value to convert
	/// @param decimalPlace 小数点以下の桁数 | The number of digits after the decimal point
	/// @return 文字列 | String
	/// @remark 小数点以下に続く 0 は削除されません。 | The trailing 0s after the decimal point are not removed.
	[[nodiscard]]
	String ToFixed(double value, int32 decimalPlace);

	////////////////////////////////////////////////////////////////
	//
	//	ToBinary
	//
	////////////////////////////////////////////////////////////////

	/// @brief 浮動小数点数のビット列を 2 進数文字列に変換します。 | Converts the bit sequence of a floating-point number to a binary string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBinary(float value);

	/// @brief 浮動小数点数のビット列を 2 進数文字列に変換します。 | Converts the bit sequence of a floating-point number to a binary string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToBinary(double value);

	////////////////////////////////////////////////////////////////
	//
	//	ToOctal
	//
	////////////////////////////////////////////////////////////////

	/// @brief 浮動小数点数のビット列を 8 進数文字列に変換します。 | Converts the bit sequence of a floating-point number to an octal string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToOctal(float value);

	/// @brief 浮動小数点数のビット列を 8 進数文字列に変換します。 | Converts the bit sequence of a floating-point number to an octal string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToOctal(double value);

	////////////////////////////////////////////////////////////////
	//
	//	ToHex
	//
	////////////////////////////////////////////////////////////////

	/// @brief 浮動小数点数のビット列を 16 進数文字列に変換します。 | Converts the bit sequence of a floating-point number to a hexadecimal string.
	/// @param value 変換する値 | The value to convert
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToHex(float value, LetterCase letterCase = LetterCase::Upper);

	/// @brief 浮動小数点数のビット列を 16 進数文字列に変換します。 | Converts the bit sequence of a floating-point number to a hexadecimal string.
	/// @param value 変換する値 | The value to convert
	/// @param letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToHex(double value, LetterCase letterCase = LetterCase::Upper);
}
