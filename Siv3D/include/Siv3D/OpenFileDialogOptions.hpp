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
	/// @brief ファイル選択ダイアログのオプション
	struct OpenFileDialogOptions
	{
		/// @brief 選択可能なファイルのフィルタ一覧。空の場合はすべてのファイルを対象とします。
		Array<FileDialogFilter> filters = {};

		/// @brief ダイアログが開かれたときの初期ディレクトリ。空の場合は OS のデフォルトの場所になります。
		FilePath initialDirectory = {};

		/// @brief ファイル名の入力欄に初期表示される文字列。（プラットフォームによっては無視される場合があります）
		String defaultFileName = {};

		/// @brief ダイアログのタイトルバーに表示される文字列。空の場合は OS のデフォルトになります。
		String title = {};

		/// @brief ダイアログの「開く」に相当する決定ボタンのテキスト。空の場合は OS のデフォルトになります。
		String acceptButtonText = {};

		/// @brief 隠しファイルや隠しフォルダをダイアログに表示するかどうか。
		bool showHiddenFiles = false;
	};
}
