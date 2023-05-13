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
	/// @param value 変換する値 | Value
	/// @param padding 文字数と埋め文字のペア | Number of characters and padding character
	/// @return パディングされた文字列 | Padded string
	[[nodiscard]]
	String Pad(const auto& value, const std::pair<int32, char32>& padding);

	/// @brief データサイズを文字列に変換します。 | Converts the data size to a string.
	/// @param bytes データサイズ | Data size
	/// @return 変換した結果 | Conversion result
	[[nodiscard]]
	String FormatDataSize(int64 bytes);
}

# include "detail/FormatUtility.ipp"
