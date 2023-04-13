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
# include "LetterCase.hpp"

namespace s3d
{
	class String;

	/// @brief bool 型を文字列に変換します。 | Converts a bool type to a string.
	/// @param value 変換する値 | The value to convert
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(bool value);

	/// @brief bool 型を文字列に変換します。 | Converts a bool type to a string.
	/// @param value 変換する値 | The value to convert
	/// @param letterCase letterCase 大文字か小文字か | Whether to use uppercase or lowercase letters
	/// @return 文字列 | String
	[[nodiscard]]
	String ToString(bool value, LetterCase letterCase);
}
