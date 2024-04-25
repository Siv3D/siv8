//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <memory>
# include "Common.hpp"
# include "Byte.hpp"
# include "String.hpp"
# include "MappedMemory.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MemoryMappedFile
	//
	////////////////////////////////////////////////////////////////

	/// @brief メモリマップトファイル
	class MemoryMappedFile
	{
	public:

		/// @brief 存在するファイルをオープンする際のオプション
		enum class OpenMode_if_Exists : uint8
		{
			/// @brief ファイルが存在する場合、失敗にします。
			Fail,

			/// @brief ファイルが存在する場合、そのファイルをオープンします。
			JustOpen,

			/// @brief ファイルが存在する場合、ファイルをオープンしてサイズを 0 にします。
			Truncate,
		};

		/// @brief 存在しないファイルをオープンする際のオプション
		enum class OpenMode_if_NotFound : uint8
		{
			/// @brief ファイルが存在しない場合、失敗にします。
			Fail,

			/// @brief ファイルが存在しない場合、新規作成します。
			Create,
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		MemoryMappedFile();

		/// @brief メモリマップトファイルをオープンします。
		/// @param path ファイルパス
		/// @param ifExists ファイルが存在する場合のオプション
		/// @param ifNotFound ファイルが存在しない場合のオプション
		[[nodiscard]]
		MemoryMappedFile(FilePathView path, OpenMode_if_Exists ifExists, OpenMode_if_NotFound ifNotFound = OpenMode_if_NotFound::Create);

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルをオープンします。
		/// @param path ファイルパス
		/// @param ifExists ファイルが存在する場合のオプション
		/// @param ifNotFound ファイルが存在しない場合のオプション
		/// @return メモリマップトファイルがオープンに成功した場合 true, それ以外の場合は false
		bool open(FilePathView path, OpenMode_if_Exists ifExists, OpenMode_if_NotFound ifNotFound = OpenMode_if_NotFound::Create);

		////////////////////////////////////////////////////////////////
		//
		//	close
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルをクローズします。
		/// @remark マップ中であれば自動的にアンマップします。
		void close();

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルがオープンしているかを返します。
		/// @return メモリマップトファイルがオープンしている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルがオープンしているかを返します。
		/// @return メモリマップトファイルがオープンしている場合 true, それ以外の場合は false
		/// @remark `isOpen()` と同じです。
		[[nodiscard]]
		explicit operator bool() const;

		////////////////////////////////////////////////////////////////
		//
		//	map
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルの指定した範囲をマップします。
		/// @param offset マップする範囲の先頭位置（バイト）
		/// @param requestSize マップする範囲のサイズ（バイト）
		/// @return マップされたメモリの情報
		/// @remark マップするサイズが 0 の場合、マップは失敗します。
		[[nodiscard]]
		MappedMemory map(size_t offset, size_t requestSize);

		////////////////////////////////////////////////////////////////
		//
		//	mapAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイル全体をマップします。
		/// @return マップされたメモリの情報
		/// @remark マップするサイズが 0 の場合、マップは失敗します。
		[[nodiscard]]
		MappedMemory mapAll();

		////////////////////////////////////////////////////////////////
		//
		//	unmap
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルをアンマップします。
		void unmap();

		////////////////////////////////////////////////////////////////
		//
		//	flush
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルの内容をディスクにフラッシュします。
		/// @return フラッシュに成功した場合 true, それ以外の場合は false
		bool flush();

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルのサイズを返します。
		/// @return メモリマップトファイルのサイズ（バイト）
		[[nodiscard]]
		int64 size() const;

		////////////////////////////////////////////////////////////////
		//
		//	path
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルのパスを返します。
		/// @return メモリマップトファイルのパス
		[[nodiscard]]
		const FilePath& path() const;

	private:

		class MemoryMappedFileDetail;

		std::shared_ptr<MemoryMappedFileDetail> pImpl;
	};
}
