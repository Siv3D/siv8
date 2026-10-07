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
	/// @remark 同時にマップできるのは 1 範囲です。返される範囲は所有権を持たず、アンマップ・クローズ・破棄で無効になります。
	/// @remark オープン中に、外部からファイルのサイズを変更しないでください。
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

		MemoryMappedFileView(const MemoryMappedFileView& other) = delete;

		/// @brief ファイルとマップの所有権を移します。取得済みの範囲はムーブ先がアンマップするまで存続します。
		/// @param other ムーブする MemoryMappedFileView。ムーブ元は閉じた状態となり、再オープンできます。
		MemoryMappedFileView(MemoryMappedFileView&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		~MemoryMappedFileView();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		MemoryMappedFileView& operator =(const MemoryMappedFileView& other) = delete;

		/// @brief 現在のファイルを閉じ、ファイルとマップの所有権を移します。
		/// @remark 自己ムーブ代入では状態を変更しません。
		/// @param other ムーブする MemoryMappedFileView。自己代入以外では、ムーブ元は閉じた状態となり、再オープンできます。
		/// @return *this
		MemoryMappedFileView& operator =(MemoryMappedFileView&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルをオープンします。
		/// @param path 通常ファイルまたは Resource() のパス。空のパスや NUL を含むパスは受け付けません。
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		/// @remark 現在のファイルとマップを置き換えます。false を返した場合は閉じた状態になります。
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
		/// @param offset マップする範囲の先頭位置（バイト）。size() 未満である必要があります。
		/// @param requestSize マップするバイト数。ファイルの終端までの残りバイト数に制限されます。
		/// @return マップされた範囲。未オープン、マップ済み、requestSize が 0、offset >= size()、または OS の処理が失敗した場合は { nullptr, 0 }。
		/// @remark 失敗した再マップは既存のマップを保持します。
		[[nodiscard]]
		MappedMemoryView map(size_t offset, size_t requestSize);

		////////////////////////////////////////////////////////////////
		//
		//	mapAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイル全体をマップします。
		/// @return map(0, size()) の結果。空のファイルは { nullptr, 0 }。
		[[nodiscard]]
		MappedMemoryView mapAll();

		////////////////////////////////////////////////////////////////
		//
		//	unmap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の範囲をアンマップします。ファイルは開いたままで、再びマップできます。
		/// @remark マップしていない場合は何もしません。
		void unmap();

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルのサイズを返します。
		/// @return オープン時のファイルサイズ（バイト）。閉じた状態では 0。
		[[nodiscard]]
		int64 size() const;

		////////////////////////////////////////////////////////////////
		//
		//	path
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリマップトファイルのパスを返します。
		/// @return 開いているファイルのフルパス。閉じた状態では空の文字列への参照。
		[[nodiscard]]
		const FilePath& path() const;

	private:

		class MemoryMappedFileViewDetail;

		std::unique_ptr<MemoryMappedFileViewDetail> pImpl;
	};
}
