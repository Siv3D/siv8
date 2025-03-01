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
# include "Common.hpp"
# include "FileChange.hpp"
# include "Optional.hpp"
# include "Array.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	DirectoryWatcher
	//
	////////////////////////////////////////////////////////////////

	/// @brief ディレクトリの監視
	class DirectoryWatcher
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		DirectoryWatcher();

		/// @brief ディレクトリを監視を開始します。
		/// @param directory 監視するディレクトリ
		/// @param applicableExtensions 監視するファイルの拡張子。空の場合はすべてのファイルを監視します。
		[[nodiscard]]
		explicit DirectoryWatcher(FilePathView directory, const Array<String>& applicableExtensions = {});

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~DirectoryWatcher();

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォルダの監視が有効かを返します。
		/// @return フォルダの監視が有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォルダの監視が有効かを返します。
		/// @return フォルダの監視が有効な場合 true, それ以外の場合は false
		/// @remark isOpen() と同じです。
		[[nodiscard]]
		explicit operator bool() const;

		////////////////////////////////////////////////////////////////
		//
		//	retrieveChanges
		//
		////////////////////////////////////////////////////////////////

		/// @brief 変更のあったファイルパスの一覧を取得します。
		/// @return 変更のあったファイルパスの一覧
		[[nodiscard]]
		Array<FileChange> retrieveChanges() const;

		/// @brief 変更のあったファイルパスの一覧を取得します。
		/// @param fileChanges 変更のあったファイルパスの一覧の格納先
		/// @return 変更のあったファイルパスが 1 つ以上ある場合 true, それ以外の場合は false
		bool retrieveChanges(Array<FileChange>& fileChanges) const;

		////////////////////////////////////////////////////////////////
		//
		//	clearChanges
		//
		////////////////////////////////////////////////////////////////

		/// @brief 変更のあったファイルパスの一覧をクリアします。
		void clearChanges();

		////////////////////////////////////////////////////////////////
		//
		//	directory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 監視しているディレクトリのパスを返します。
		/// @return 監視しているディレクトリのパス
		[[nodiscard]]
		const FilePath& directory() const;

		////////////////////////////////////////////////////////////////
		//
		//	applicableExtensions
		//
		////////////////////////////////////////////////////////////////

		/// @brief 監視するファイルの拡張子の一覧を返します。
		/// @return 監視するファイルの拡張子の一覧。すべてのファイルを監視している場合は空の配列
		[[nodiscard]]
		const Array<String>& applicableExtensions() const;

	private:

		class DirectoryWatcherDetail;

		std::shared_ptr<DirectoryWatcherDetail> pImpl;
	};
}
