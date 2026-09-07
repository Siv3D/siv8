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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	LetterCase
	//
	////////////////////////////////////////////////////////////////

	/// @brief アルファベットの大文字・小文字 | Letter case
	enum class LetterCase : uint8
	{
		/// @brief 小文字 | Lowercase
		Lower,

		/// @brief 大文字 | Uppercase
		Upper,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief アルファベットの大文字・小文字の指定を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value アルファベットの大文字・小文字の指定
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, LetterCase value);
}
