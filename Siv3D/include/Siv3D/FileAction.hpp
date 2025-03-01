//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
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
	//	FileAction
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイルの操作 | File action
	enum class FileAction : uint8
	{
		/// @brief 不明な操作 | Unknown operation
		Unknown,

		/// @brief ファイルが新規追加された | A file was added
		Added,

		/// @brief ファイルが削除された | A file was removed
		Removed,

		/// @brief ファイルの内容が更新された | A file was modified
		Modified,
	};

	/// @brief ファイルの操作を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value ファイルの操作
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, FileAction value);
}
