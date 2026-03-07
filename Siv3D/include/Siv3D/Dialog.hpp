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
# include "FileDialogFilter.hpp"
# include "OpenFileDialogOptions.hpp"
# include "SaveFileDialogOptions.hpp"
# include "SelectFolderDialogOptions.hpp"

namespace s3d
{
	namespace Dialog
	{
		////////////////////////////////////////////////////////////////
		//
		//	OpenFile, OpenFiles
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイル選択ダイアログを表示します。
		/// @param options ダイアログのオプション
		/// @return ユーザーが選択したファイルのパス。選択されなかった場合は none
		[[nodiscard]]
		Optional<FilePath> OpenFile(const OpenFileDialogOptions& options = {});

		/// @brief 複数選択できるファイル選択ダイアログを表示します。
		/// @param options ダイアログのオプション
		/// @return ユーザーが選択したファイルのパスの配列。選択されなかった場合は空の配列
		[[nodiscard]]
		Array<FilePath> OpenFiles(const OpenFileDialogOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	SaveFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイル保存ダイアログを表示します。
		/// @param options ダイアログのオプション
		/// @return ユーザーが選択したファイルのパス。選択されなかった場合は none
		[[nodiscard]]
		Optional<FilePath> SaveFile(const SaveFileDialogOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	SelectFolder
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォルダ選択ダイアログを表示します。
		/// @param options ダイアログのオプション
		/// @return ユーザーが選択したフォルダのパス。選択されなかった場合は none
		[[nodiscard]]
		Optional<FilePath> SelectFolder(const SelectFolderDialogOptions& options = {});
	}
}
