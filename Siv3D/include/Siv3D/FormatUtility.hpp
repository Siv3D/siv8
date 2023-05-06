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
# include "Common.hpp"
# include "String.hpp"
# include "Format.hpp"

namespace s3d
{
	/// @brief 値を文字列に変換し、指定した文字数に満たない場合は値の左を埋め文字で埋めます。 | Converts a value to a string, and if the string is shorter than the specified length, the left of the string is filled with the padding character.
	/// @tparam Type 変換する値の型 | Value type
	/// @param value 変換する値 | Value
	/// @param padding 文字数と埋め文字のペア | Number of characters and padding character
	/// @return 結果の文字列 | String of the result
	template <class Type>
	[[nodiscard]]
	String Pad(const Type& value, std::pair<int32, char32> padding);
}

# include "detail/FormatUtility.ipp"
