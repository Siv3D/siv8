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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INIWriteOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI 書き出し時のオプション | INI write options
	struct INIWriteOptions
	{
		/// @brief キーと値の間の `=` の前後に空白を入れます。 | Adds spaces around `=` between keys and values.
		bool spaceAroundEquals = true;

		/// @brief 最終行の末尾にも改行を書き込みます。 | Writes a newline after the last line.
		bool trailingNewline = true;

		/// @brief UTF-8 BOM を書き込みます。 | Writes a UTF-8 BOM.
		bool writeUTF8BOM = false;

		/// @brief 書き出しに使う改行文字列 | Newline sequence used for writing
		String newline = U"\n";
	};
}
