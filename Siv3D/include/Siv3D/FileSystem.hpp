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
# include "Array.hpp"
# include "PredefinedYesNo.hpp"
# include "FileCopyOption.hpp"
# include "Optional.hpp"
# include "DateTime.hpp"

namespace s3d
{
	enum class SpecialFolder : uint8;

# if SIV3D_PLATFORM(WINDOWS)

	/// @brief ネイティブ形式のファイルパス
	using NativeFilePath = std::wstring;

# else

	/// @brief ネイティブ形式のファイルパス
	using NativeFilePath = std::string;

# endif

	namespace Platform
	{
		/// @brief ファイルを実行ファイルに埋め込み可能であるか
		inline constexpr bool HasEmbeddedResource = SIV3D_PLATFORM(WINDOWS);
	}

	namespace FileSystem
	{
		////////////////////////////////////////////////////////////////
		//
		//	IsResourcePath
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルパスがリソースのパスであるかを返します。
		/// @param path ファイルパス
		/// @remark 実際に存在するリソースのパスである必要はありません。
		/// @return リソースのパスである場合 true, 該当しない場合や判定できない場合は false
		[[nodiscard]]
		bool IsResourcePath(FilePathView path) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Exists
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定されたパスのファイルまたはディレクトリが存在するかを返します。
		/// @param path パス
		/// @return 存在する場合 true, 存在しない場合や判定できない場合は false
		[[nodiscard]]
		bool Exists(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	IsDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したパスのディレクトリが存在するかを返します。
		/// @param path ディレクトリのパス
		/// @return ディレクトリが存在する場合 true, 該当しない場合や判定できない場合は false
		[[nodiscard]]
		bool IsDirectory(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	IsFile
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したパスのファイルが存在するかを返します。
		/// @param path ファイルパス
		/// @return ファイルが存在する場合 true, 該当しない場合や判定できない場合は false
		[[nodiscard]]
		bool IsFile(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	IsResource
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したパスのリソースが存在するかを返します。
		/// @param path リソースパス
		/// @return 指定したパスのリソースが存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool IsResource(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	FullPath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 絶対パスを返します。
		/// @param path パス
		/// @remark 実際に存在するファイルやディレクトリのパスである必要はありません。
		/// @return 絶対パス。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath FullPath(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	NativePath
		//
		////////////////////////////////////////////////////////////////

		/// @brief OS ネイティブの形式で表記された絶対パスを返します。
		/// @param path パス
		/// @return OS ネイティブの形式で表記された絶対パス。失敗した場合は空の文字列
		[[nodiscard]]
		NativeFilePath NativePath(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	Extension
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの拡張子（.を含まない）を小文字にして返します。
		/// @param path ファイルパス
		/// @remark 例: `dir/picture.PNG` → `png`。
		/// @remark ファイル名の先頭に連続する `.` は拡張子の区切りとして扱いません。例: `.gitignore` は空、`dir/.config.txt` は `txt`。
		/// @return 小文字の拡張子。拡張子がない場合は空の文字列
		[[nodiscard]]
		String Extension(FilePathView path);

		/// @brief ファイルの拡張子（.を含まない）を返します。
		/// @param path ファイルパス
		/// @param preserveCase 大文字小文字を保持するか
		/// @remark 例: `dir/picture.PNG` は `PreserveCase::Yes` で `PNG`、`PreserveCase::No` で `png`。
		/// @remark ファイル名の先頭に連続する `.` は拡張子の区切りとして扱いません。例: `.gitignore` は空、`dir/.config.txt` は `txt`。
		/// @return 拡張子。拡張子がない場合は空の文字列
		[[nodiscard]]
		String Extension(FilePathView path, PreserveCase preserveCase);

		////////////////////////////////////////////////////////////////
		//
		//	FileName
		//
		////////////////////////////////////////////////////////////////

		/// @brief 親ディレクトリを含まずに、ファイル名を返します。
		/// @param path ファイルパス
		/// @remark 例: `dir/picture.png` → `picture.png`、`dir/archive.tar.gz` → `archive.tar.gz`。
		/// @return ファイル名。パスが空または区切り文字で終わる場合は空の文字列
		[[nodiscard]]
		String FileName(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	BaseName
		//
		////////////////////////////////////////////////////////////////

		/// @brief 親ディレクトリを含まずに、拡張子を除いたファイル名を返します。
		/// @param path ファイルパス
		/// @remark 例: `dir/picture.png` → `picture`、`dir/archive.tar.gz` → `archive.tar`、`.gitignore` → `.gitignore`。
		/// @return 拡張子を除いたファイル名。パスが空または区切り文字で終わる場合は空の文字列
		[[nodiscard]]
		String BaseName(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	ParentPath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したパスの親ディレクトリを返します。
		/// @param path パス
		/// @param level 親のレベル。0 は親ディレクトリ、1 はその親ディレクトリ
		/// @return 親ディレクトリ。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath ParentPath(FilePathView path, size_t level = 0);

		/// @brief 指定したパスの親ディレクトリを返します。合わせて、渡したパスのフルパスを取得します。
		/// @param path パス
		/// @param level 親のレベル。0 は親ディレクトリ、1 はその親ディレクトリ
		/// @param baseFullPath 渡したパスのフルパスを格納する変数への参照
		/// @return 親ディレクトリ。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath ParentPath(FilePathView path, size_t level, FilePath& baseFullPath);

		////////////////////////////////////////////////////////////////
		//
		//	VolumePath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したファイルパスのボリューム名を返します。
		/// @param path パス
		/// @return ボリューム名。失敗した場合は空の文字列
		[[nodiscard]]
		FilePath VolumePath(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	PathAppend
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つのパスを結合して返します。
		/// @param lhs 結合する左側のパス
		/// @param rhs 結合する右側のパス
		/// @return 結合したパス
		[[nodiscard]]
		FilePath PathAppend(FilePathView lhs, FilePathView rhs);

		////////////////////////////////////////////////////////////////
		//
		//	IsEmptyDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したパスが空のディレクトリであるかを返します。
		/// @param path パス
		/// @return 空のディレクトリである場合 true, 該当しない場合や中身を列挙できない場合は false
		[[nodiscard]]
		bool IsEmptyDirectory(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	Size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したファイルやディレクトリのサイズを返します。
		/// @param path パス
		/// @remark OS エラーで取得に失敗した場合は、部分合計を返さず 0 を返します。
		/// @remark macOS / Linux では、集計中に見つかったディレクトリへのシンボリックリンクには再帰しません。ファイルへのシンボリックリンクはリンク先のサイズを加算します。
		/// @remark Windows では、集計中に見つかったディレクトリのジャンクションやシンボリックリンクなど、別の場所を指す再解析ポイントには再帰しません。
		/// @remark path に直接指定したディレクトリへのリンクはたどって集計します。
		/// @return ファイルやディレクトリのサイズ（バイト）
		[[nodiscard]]
		uint64 Size(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	FileSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルのサイズを返します。
		/// @param path ファイルパス
		/// @return ファイルのサイズ（バイト）。ファイルが存在しない場合、ディレクトリの場合、取得に失敗した場合は 0
		[[nodiscard]]
		uint64 FileSize(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	CreationTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの作成日時を取得します。
		/// @param path ファイルパス
		/// @return ファイルの作成日時。取得に失敗した場合 none
		[[nodiscard]]
		Optional<DateTime> CreationTime(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	WriteTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの最終更新日時を取得します。
		/// @param path ファイルパス
		/// @return ファイルの最終更新日時。取得に失敗した場合 none
		[[nodiscard]]
		Optional<DateTime> WriteTime(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	AccessTime
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルの最終アクセス日時を取得します。
		/// @param path ファイルパス
		/// @return ファイルの最終アクセス日時。取得に失敗した場合 none
		[[nodiscard]]
		Optional<DateTime> AccessTime(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	DirectoryContents
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したディレクトリの中身（パス）を取得します。
		/// @param path ディレクトリのパス
		/// @param recursive ディレクトリの中身にあるディレクトリの中身も取得する場合は `Recursive::Yes`, それ以外の場合は `Recursive::No`
		/// @remark OS エラーで取得に失敗した場合は、部分結果を返さず空の一覧を返します。
		/// @remark Windows では、列挙中に見つかったディレクトリのジャンクションやシンボリックリンクなど、別の場所を指す再解析ポイントは一覧に含めますが、その中には再帰しません。path に直接指定したディレクトリへのリンクはたどって列挙します。
		/// @return 指定したディレクトリの中身（パス）の一覧
		[[nodiscard]]
		Array<FilePath> DirectoryContents(FilePathView path, Recursive recursive = Recursive::Yes);

		////////////////////////////////////////////////////////////////
		//
		//	GetLaunchDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 実行ファイルを起動したディレクトリを返します。
		/// @return 実行ファイルを起動したディレクトリ
		[[nodiscard]]
		const FilePath& GetLaunchDirectory() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetExecutablePath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 実行ファイルのフルパスを返します。
		/// @return 実行ファイルのフルパス
		[[nodiscard]]
		const FilePath& GetExecutablePath() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GetExecutableDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 実行ファイルが存在するディレクトリを返します。
		/// @return 実行ファイルが存在するディレクトリ
		[[nodiscard]]
		const FilePath& GetExecutableDirectory() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	CurrentDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のカレントディレクトリを返します。
		/// @return 現在のカレントディレクトリ
		[[nodiscard]]
		FilePath CurrentDirectory();

		////////////////////////////////////////////////////////////////
		//
		//	ChangeCurrentDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief カレントディレクトリを変更します。
		/// @param path 新しいカレントディレクトリ
		/// @return カレントディレクトリの変更に成功した場合 true, それ以外の場合は false
		bool ChangeCurrentDirectory(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	GetFolderPath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 特殊フォルダのパスを返します。
		/// @param folder 特殊フォルダの種類
		/// @return 特殊フォルダのパス。存在しない場合は空の文字列
		[[nodiscard]]
		const FilePath& GetFolderPath(SpecialFolder folder);

		////////////////////////////////////////////////////////////////
		//
		//	TemporaryDirectoryPath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 一時ファイルの保存に使えるフォルダのパスを返します。
		/// @return 一時ファイルの保存に使えるフォルダのパス
		[[nodiscard]]
		FilePath TemporaryDirectoryPath();

		////////////////////////////////////////////////////////////////
		//
		//	UniqueFilePath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 一時ファイル用に使えるファイルパスを返します。
		/// @param directory 一時ファイルの保存に使うフォルダのパス
		/// @return 一時ファイル用に使えるファイルパス
		[[nodiscard]]
		FilePath UniqueFilePath(FilePathView directory = TemporaryDirectoryPath());

		////////////////////////////////////////////////////////////////
		//
		//	RelativePath
		//
		////////////////////////////////////////////////////////////////

		/// @brief 相対パスを作成して返します。
		/// @param path パス
		/// @param start 相対パスの基準となるパス
		/// @remark 相対パスの末尾の `/` は、対象の path がディレクトリであるかに基づいて付加されます。
		/// @remark path または start の絶対パス取得に失敗した場合は空の文字列を返します。
		/// @remark start の絶対パスを取得できてもディレクトリが存在しない場合は、path の絶対パスを返します。
		/// @return start からみた path の相対パス
		[[nodiscard]]
		FilePath RelativePath(FilePathView path, FilePathView start = CurrentDirectory());

		////////////////////////////////////////////////////////////////
		//
		//	CreateDirectories
		//
		////////////////////////////////////////////////////////////////

		/// @brief ディレクトリを作成します。
		/// @param path 作成するディレクトリのパス
		/// @remark 間のディレクトリが存在しない場合は自動的に作成します。
		/// @return 作成に成功したか、すでに同名のディレクトリが存在する場合 true, それ以外の場合は false
		bool CreateDirectories(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	CreateParentDirectories
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したパスの親ディレクトリを作成します。
		/// @param path パス
		/// @remark 間のディレクトリが存在しない場合は自動的に作成します。
		/// @remark 親ディレクトリがすでに存在する場合、書き込み可能かどうかは確認しません。
		/// @return 作成に成功したか、すでに同名のディレクトリが存在する場合 true, それ以外の場合は false
		bool CreateParentDirectories(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	Copy
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したファイルまたはディレクトリをコピーします。
		/// @param from コピー元のパス
		/// @param to コピー先のパス
		/// @param copyOption オプション
		/// @return コピーに成功した場合 true, それ以外の場合は false
		bool Copy(FilePathView from, FilePathView to, FileCopyOption copyOption = FileCopyOption::Default);

		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したファイルまたはディレクトリを削除します。
		/// @param path 削除するパス
		/// @param moveToTrash 削除したファイルやディレクトリをゴミ箱に送る場合 `MoveToTrash::Yes`, それ以外の場合は `MoveToTrash::No`
		/// @return 削除に成功した場合 true, それ以外の場合は false
		bool Remove(FilePathView path, MoveToTrash moveToTrash = MoveToTrash::No);

		////////////////////////////////////////////////////////////////
		//
		//	RemoveContents
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したディレクトリの中身を削除します。
		/// @param path 中身を削除するディレクトリのパス
		/// @param moveToTrash 削除したファイルやディレクトリをゴミ箱に送る場合 `MoveToTrash::Yes`, それ以外の場合は `MoveToTrash::No`
		/// @remark ディレクトリ本体は削除・再作成しません。
		/// @remark macOS / Linux では、path がディレクトリへのシンボリックリンクの場合、リンクとリンク先のディレクトリ本体を残して中身を削除します。中にあるシンボリックリンクはリンク自体を削除します。
		/// @remark 途中で失敗した場合、すでに削除した項目は元に戻しません。
		/// @return 削除に成功した場合 true, それ以外の場合は false
		bool RemoveContents(FilePathView path, MoveToTrash moveToTrash = MoveToTrash::No);

		////////////////////////////////////////////////////////////////
		//
		//	Rename
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したファイルまたはディレクトリをリネームします。
		/// @param from リネームするパス
		/// @param to リネーム後のパス
		/// @return リネームに成功した場合 true, それ以外の場合は false
		bool Rename(FilePathView from, FilePathView to);
	}
}
