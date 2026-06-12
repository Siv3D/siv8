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
	//	CSVParseErrorCode
	//
	////////////////////////////////////////////////////////////////

	/// @brief CSV のパースエラーの種類 | CSV parse error code
	enum class CSVParseErrorCode : uint8
	{
		/// @brief Reader が無効です。 | The reader is invalid.
		InvalidReader,

		/// @brief 入力の文字エンコーディングが不正です。 | The input text encoding is invalid.
		InvalidEncoding,

		/// @brief quote されていないフィールド内に `"` が出現しました。 | A double quote appeared in an unquoted field.
		UnexpectedQuoteInUnquotedField,

		/// @brief 閉じ quote の後に、comma または改行以外の文字が出現しました。 | An unexpected character appeared after a closing quote.
		UnexpectedCharacterAfterClosingQuote,

		/// @brief quote されたフィールドが閉じられていません。 | A quoted field is not closed.
		UnterminatedQuotedField,

		/// @brief CR 単独の改行が出現しました。 | A bare CR appeared.
		BareCR,

		/// @brief LF 単独の改行が出現しました。 | A bare LF appeared.
		BareLF,

		/// @brief 行ごとの列数が一致しません。 | The number of columns is inconsistent.
		InconsistentColumns,
	};
}
