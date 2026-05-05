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
	//	CSVWriteOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief CSV 書き出し時のオプション | CSV write options
	struct CSVWriteOptions
	{
		/// @brief すべてのフィールドを quote します。 | Quotes all fields.
		bool alwaysQuote = false;

		/// @brief UTF-8 BOM を書き込みます。 | Writes a UTF-8 BOM.
		bool writeUTF8BOM = false;

		/// @brief 最終レコードの末尾にも CRLF を書き込みます。 | Writes CRLF after the last record.
		bool trailingNewline = true;
	};
}
