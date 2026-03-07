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
# include "FileDialogFilter.hpp"
# include "FileSystem.hpp"

namespace s3d
{
	/// @brief フォルダ選択ダイアログのオプション
	struct SelectFolderDialogOptions
	{
		/// @brief ダイアログが開かれたときの初期ディレクトリ。空の場合は OS のデフォルトの場所になります。
		FilePath initialDirectory = {};

		/// @brief ダイアログのタイトルバーに表示される文字列。空の場合は OS のデフォルトになります。
		String title = {};

		/// @brief ダイアログの決定ボタンのテキスト。空の場合は OS のデフォルトになります。
		String acceptButtonText = {};

		/// @brief 隠しファイルや隠しフォルダをダイアログに表示するかどうか。
		bool showHiddenFiles = false;
	};
}
