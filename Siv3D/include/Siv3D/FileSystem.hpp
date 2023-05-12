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
	namespace Platform
	{
		/// @brief ファイルを実行ファイルに埋め込み可能であるか
		inline constexpr bool HasEmbeddedResource = SIV3D_PLATFORM(WINDOWS);
	}

	namespace FileSystem
	{
		/// @brief ファイルパスがリソースのパスであるかを返します。
		/// @param path ファイルパス
		/// @remark 実際に存在するリソースのパスである必要はありません
		/// @return リソースのパスである場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsResourcePath(FilePathView path) noexcept;

		/// @brief 指定されたパスのファイルまたはディレクトリが存在するかを返します。
		/// @param path パス
		/// @return 指定されたパスのファイルまたはディレクトリが存在場合 true, それ以外の場合は false
		[[nodiscard]]
		bool Exists(FilePathView path);


		/// @brief 絶対パスを返します。
		/// @param path パス
		/// @remark 実際に存在するファイルやディレクトリのパスである必要はありません
		/// @remark 例: "C:/Users/Siv/Desktop/picture.png"
		/// @return 絶対パス。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath FullPath(FilePathView path);

		/// @brief 指定したパスの親ディレクトリを返します。合わせて、渡したパスのフルパスを取得します。
		/// @param path パス
		/// @param level 親のレベル。大きいほど上位の親ディレクトリ
		/// @param baseFullPath 渡したパスのフルパスを格納する変数への参照
		/// @return 親ディレクトリ。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath ParentPath(FilePathView path, size_t level, FilePath& baseFullPath);

		/// @brief ファイルのサイズを返します。
		/// @param path ファイルパス
		/// @remark `FileSystem::Size()` と異なり、ディレクトリのサイズは取得できません。
		/// @remark ファイルが存在しなかったり、空である場合は 0 を返します。
		/// @return ファイルのサイズ
		[[nodiscard]]
		int64 FileSize(FilePathView path);

		/// @brief 実行ファイルを起動したディレクトリを返します。
		/// @return 実行ファイルを起動したディレクトリ
		[[nodiscard]]
		const FilePath& InitialDirectory() noexcept;

		/// @brief 実行ファイルのフルパスを返します。
		/// @return 実行ファイルのフルパス
		[[nodiscard]]
		const FilePath& ModulePath() noexcept;
	
		/// @brief 現在のカレントディレクトリを返します。
		/// @return 現在のカレントディレクトリ
		[[nodiscard]]
		FilePath CurrentDirectory();

		/// @brief ディレクトリを作成します。
		/// @param path 作成するディレクトリのパス
		/// @remark 間のディレクトリが存在しない場合は自動的に作成します。
		/// @return 作成に成功したか、すでに同名のディレクトリが存在する場合 true, それ以外の場合は false
		bool CreateDirectories(FilePathView path);

		/// @brief 指定したファイルまたはディレクトリを削除します。
		/// @param path 削除するパス
		/// @param allowUndo 削除したファイルやディレクトリをゴミ箱に送る場合 `AllowUndo::Yes`, それ以外の場合は `AllowUndo::No`
		/// @return 削除に成功した場合 true, それ以外の場合は false
		bool Remove(FilePathView path, AllowUndo allowUndo = AllowUndo::No);

	}
}