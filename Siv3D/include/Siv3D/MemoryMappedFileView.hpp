﻿//-----------------------------------------------
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
# include <memory>
# include "Common.hpp"
# include "Byte.hpp"
# include "String.hpp"
# include "MappedMemoryView.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MemoryMappedFileView
	//
	////////////////////////////////////////////////////////////////

	/// @brief 読み込み専用メモリマップトファイル
	class MemoryMappedFileView
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		MemoryMappedFileView();

		/// @brief メモリマップトファイルをオープンします。
		/// @param path ファイルパス
		[[nodiscard]]
		explicit MemoryMappedFileView(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルをオープンします。
		/// @param path ファイルパス
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		bool open(FilePathView path);

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
		MappedMemoryView map(size_t offset, size_t requestSize);

		////////////////////////////////////////////////////////////////
		//
		//	mapAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイル全体をマップします。
		/// @return マップされたメモリの情報
		/// @remark マップするサイズが 0 の場合、マップは失敗します。
		[[nodiscard]]
		MappedMemoryView mapAll();

		////////////////////////////////////////////////////////////////
		//
		//	unmap
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルをアンマップします。
		void unmap();

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

		class MemoryMappedFileViewDetail;

		std::shared_ptr<MemoryMappedFileViewDetail> pImpl;
	};
}
