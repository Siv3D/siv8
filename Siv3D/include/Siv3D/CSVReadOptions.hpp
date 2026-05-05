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
	//	CSVReadOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief CSV 読み込み時のオプション | CSV read options
	struct CSVReadOptions
	{
		/// @brief レコード区切りとして LF 単独を許可します。 | Allows a lone LF as a record separator.
		/// @remark デフォルトでは `true` です。RFC 4180 strict mode では `false` にしてください。
		bool allowLoneLF = true;

		/// @brief レコード区切りとして CR 単独を許可します。 | Allows a lone CR as a record separator.
		/// @remark デフォルトでは `false` です。
		bool allowLoneCR = false;

		/// @brief 各レコードの列数が 1 レコード目と一致することを要求します。 | Requires all records to have the same number of fields as the first record.
		bool requireConsistentColumns = false;

		/// @brief UTF-8 BOM を読み飛ばします。 | Skips a UTF-8 BOM.
		bool skipUTF8BOM = true;
	};
}
