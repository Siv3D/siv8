//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include "Common.hpp"
# include "String.hpp"
# include "PredefinedYesNo.hpp"

namespace s3d
{
	namespace FileSystem
	{
		[[nodiscard]]
		bool IsResourcePath(FilePathView path) noexcept;


		/// @brief 絶対パスを返します。
		/// @param path パス
		/// @remark 実際に存在するファイルやディレクトリのパスである必要はありません
		/// @remark 例: "C:/Users/Siv/Desktop/picture.png"
		/// @return 絶対パス。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath FullPath(FilePathView path);

		/// @brief ファイルのサイズを返します。
		/// @param path ファイルパス
		/// @remark `FileSystem::Size()` と異なり、ディレクトリのサイズは取得できません。
		/// @remark ファイルが存在しなかったり、空である場合は 0 を返します。
		/// @return ファイルのサイズ
		[[nodiscard]]
		int64 FileSize(FilePathView path);



		/// @brief 指定したファイルまたはディレクトリを削除します。
		/// @param path 削除するパス
		/// @param allowUndo 削除したファイルやディレクトリをゴミ箱に送る場合 `AllowUndo::Yes`, それ以外の場合は `AllowUndo::No`
		/// @return 削除に成功した場合 true, それ以外の場合は false
		bool Remove(FilePathView path, AllowUndo allowUndo = AllowUndo::No);

	}
}