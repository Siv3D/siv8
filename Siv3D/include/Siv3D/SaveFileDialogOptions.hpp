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
	////////////////////////////////////////////////////////////////
	//
	//	SaveFileDialogOptions
	//
	////////////////////////////////////////////////////////////////

	/// @brief ファイル保存ダイアログのオプション
	struct SaveFileDialogOptions
	{
		/// @brief 保存可能なファイルのフィルタ一覧。空の場合はすべてのファイルを対象とします。
		Array<FileDialogFilter> filters = {};

		/// @brief ダイアログが開かれたときの初期ディレクトリ。空の場合は OS のデフォルトの場所になります。
		FilePath initialDirectory = {};

		/// @brief ファイル名の入力欄に初期表示される文字列。
		String defaultFileName = {};

		/// @brief ダイアログのタイトルバーに表示される文字列。空の場合は OS のデフォルトになります。
		String title = {};

		/// @brief ダイアログの「保存」に相当する決定ボタンのテキスト。空の場合は OS のデフォルトになります。
		String acceptButtonText = {};

		/// @brief ユーザーが拡張子を入力しなかった場合、選択中のフィルタに基づいて自動的に拡張子を追加するかどうか。
		bool addExtensionIfMissing = true;

		/// @brief 隠しファイルや隠しフォルダをダイアログに表示するかどうか。
		bool showHiddenFiles = false;
	};
}