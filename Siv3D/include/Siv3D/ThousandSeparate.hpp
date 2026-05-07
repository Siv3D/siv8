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

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "Int128.hpp"
# include "IntToString.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	ThousandSeparate
	//
	////////////////////////////////////////////////////////////////

	/// @brief 桁区切りを行いながら整数を文字列に変換します。 | Converts an integer to a string while performing digit separation.
	/// @param value 変換する値 | Value
	/// @param separator 桁区切りの文字 | Digit separator
	/// @return 桁区切りされた文字列 | Digit-separated string
	[[nodiscard]]
	String ThousandSeparate(Concept::Integral auto value, char32 separator = U',');

	/// @brief 桁区切りを行いながら浮動小数点数を文字列に変換します。 | Converts a floating point number to a string while performing digit separation.
	/// @param value 変換する値 | Value
	/// @param decimalPlace 小数点以下の桁数 | Number of digits below the decimal point
	/// @param fixDecimalPlaces 小数点以下の桁数を固定するには `FixDecimalPlaces::Yes`, それ以外は `FixDecimalPlaces::No` | If you want to fix the number of digits below the decimal point, `FixDecimalPlaces::Yes`, otherwise `FixDecimalPlaces::No`
	/// @param separator 桁区切りの文字 | Digit separator
	/// @return 桁区切りされた文字列 | Digit-separated string
	[[nodiscard]]
	String ThousandSeparate(float value, int32 decimalPlace, FixDecimalPlaces fixDecimalPlaces = FixDecimalPlaces::No, char32 separator = U',');

	/// @brief 桁区切りを行いながら浮動小数点数を文字列に変換します。 | Converts a floating point number to a string while performing digit separation.
	/// @param value 変換する値 | Value
	/// @param decimalPlace 小数点以下の桁数 | Number of digits below the decimal point
	/// @param fixDecimalPlaces 小数点以下の桁数を固定するには `FixDecimalPlaces::Yes`, それ以外は `FixDecimalPlaces::No` | If you want to fix the number of digits below the decimal point, `FixDecimalPlaces::Yes`, otherwise `FixDecimalPlaces::No`
	/// @param separator 桁区切りの文字 | Digit separator
	/// @return 桁区切りされた文字列 | Digit-separated string
	[[nodiscard]]
	String ThousandSeparate(double value, int32 decimalPlace, FixDecimalPlaces fixDecimalPlaces = FixDecimalPlaces::No, char32 separator = U',');

	/// @brief 桁区切りを行いながら整数を文字列に変換します。 | Converts an integer to a string while performing digit separation.
	/// @param value 変換する値 | Value
	/// @param separator 桁区切りの文字 | Digit separator
	/// @return 桁区切りされた文字列 | Digit-separated string
	[[nodiscard]]
	String ThousandSeparate(const int128& value, char32 separator = U',');

	/// @brief 桁区切りを行いながら整数を文字列に変換します。 | Converts an integer to a string while performing digit separation.
	/// @param value 変換する値 | Value
	/// @param separator 桁区切りの文字 | Digit separator
	/// @return 桁区切りされた文字列 | Digit-separated string
	[[nodiscard]]
	String ThousandSeparate(const uint128& value, char32 separator = U',');
}

# include "detail/ThousandSeparate.ipp"
