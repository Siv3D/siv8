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
# include "INIDuplicatePolicy.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INIReadOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI 読み込み時のオプション | INI read options
	struct INIReadOptions
	{
		/// @brief UTF-8 BOM を読み飛ばします。 | Skips a UTF-8 BOM.
		bool skipUTF8BOM = true;

		/// @brief レコード区切りとして CR 単独を許可します。 | Allows a lone CR as a line separator.
		/// @remark デフォルトでは `false` です。 | The default is `false`.
		bool allowLoneCR = false;

		/// @brief 行頭の `#` をコメントとして扱います。 | Treats a leading `#` as a comment.
		bool allowHashComment = true;

		/// @brief 行頭の `;` をコメントとして扱います。 | Treats a leading `;` as a comment.
		bool allowSemicolonComment = true;

		/// @brief セクション名の前後の空白を除去します。 | Trims whitespace around section names.
		bool trimSectionName = true;

		/// @brief キーの前後の空白を除去します。 | Trims whitespace around keys.
		bool trimKey = true;

		/// @brief 値の前後の空白を除去します。 | Trims whitespace around values.
		bool trimValue = true;

		/// @brief 重複するセクションの扱い | Policy for duplicate sections
		INIDuplicateSectionPolicy duplicateSectionPolicy = INIDuplicateSectionPolicy::Merge;

		/// @brief 重複するキーの扱い | Policy for duplicate keys
		INIDuplicateKeyPolicy duplicateKeyPolicy = INIDuplicateKeyPolicy::Overwrite;
	};
}
