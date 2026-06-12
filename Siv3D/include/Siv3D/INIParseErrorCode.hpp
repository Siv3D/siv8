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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INIParseErrorCode
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI のパースエラーの種類 | INI parse error code
	enum class INIParseErrorCode : uint8
	{
		/// @brief Reader が無効です。 | The reader is invalid.
		InvalidReader,

		/// @brief 入力の文字エンコーディングが不正です。 | The input text encoding is invalid.
		InvalidEncoding,

		/// @brief CR 単独の改行が出現しました。 | A bare CR appeared.
		BareCR,

		/// @brief セクションの閉じ `]` がありません。 | The closing `]` of a section is missing.
		MissingClosingBracket,

		/// @brief セクション名が空です。 | The section name is empty.
		EmptySectionName,

		/// @brief セクション宣言の後に予期しない文字があります。 | Unexpected characters appeared after a section declaration.
		UnexpectedCharacterAfterSection,

		/// @brief キーと値を区切る `=` がありません。 | The assignment character `=` is missing.
		MissingAssignment,

		/// @brief キーが空です。 | The key is empty.
		EmptyKey,

		/// @brief セクションが重複しています。 | The section is duplicated.
		DuplicateSection,

		/// @brief キーが重複しています。 | The key is duplicated.
		DuplicateKey,
	};
}
