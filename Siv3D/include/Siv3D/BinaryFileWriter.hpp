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
# include "IWriter.hpp"
# include "StringView.hpp"
# include "FileWriteMode.hpp"

namespace s3d
{
	class String;
	using FilePath = String;

	////////////////////////////////////////////////////////////////
	//
	//	BinaryFileWriter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 書き込み用バイナリファイル
	class BinaryFileWriter : public IWriter
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		BinaryFileWriter();

		/// @brief ファイルを開きます。
		/// @param path ファイルパス
		/// @param writeMode 書き込みモード
		[[nodiscard]]
		explicit BinaryFileWriter(FilePathView path, FileWriteMode writeMode = FileWriteMode::Trunc);

		BinaryFileWriter(const BinaryFileWriter& other) = delete;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする BinaryFileWriter
		BinaryFileWriter(BinaryFileWriter&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		~BinaryFileWriter() override;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		BinaryFileWriter& operator =(const BinaryFileWriter& other) = delete;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする BinaryFileWriter
		/// @return *this
		BinaryFileWriter& operator =(BinaryFileWriter&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを開きます。
		/// @param path ファイルパス
		/// @param writeMode 書き込みモード
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		/// @remark 開いているファイルは閉じられます。前のファイルの終了結果が必要な場合は、先に close() の結果を確認してください。
		/// @remark 成功すると、以前の書き込みエラーを解除します。
		bool open(FilePathView path, FileWriteMode writeMode = FileWriteMode::Trunc);

		////////////////////////////////////////////////////////////////
		//
		//	close
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルを閉じます。
		/// @return ファイルへの出力・クローズに関するエラーを検出しなかった場合 true, それ以外の場合は false
		/// @remark エラーがあってもファイルを閉じます。エラーは次の open() が成功するまで保持され、繰り返し close() を呼んでも同じ結果を返します。
		/// @remark 未オープンでエラーもない場合は何もせず true を返します。
		bool close();

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルが開いているかを返します。
		/// @return ファイルが開いている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルが開いているかを返します。
		/// @return ファイルが開いている場合 true, それ以外の場合は false	
		[[nodiscard]]
		explicit operator bool() const noexcept override;

		////////////////////////////////////////////////////////////////
		//
		//	flush
		//
		////////////////////////////////////////////////////////////////

		/// @brief 書き込みバッファの内容を OS に引き渡します。
		/// @return ファイルへの出力・クローズに関するエラーを検出していない場合 true, それ以外の場合は false
		/// @remark エラーは次の open() が成功するまで保持されます。未オープンでエラーもない場合は何もせず true を返します。
		bool flush();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開いているファイルの内容をすべて消去し、サイズが 0 のファイルにします。
		/// @remark 以前の出力エラーがある場合は何もしません。消去中の出力エラーも flush() / close() の結果に反映されます。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開いているファイルの現在のサイズ（バイト）を返します。
		/// @return 開いているファイルの現在のサイズ（バイト）
		[[nodiscard]]
		int64 size() const override;

		////////////////////////////////////////////////////////////////
		//
		//	getPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の書き込み位置を返します。
		/// @return 現在の書き込み位置
		[[nodiscard]]
		int64 getPos() const override;

		////////////////////////////////////////////////////////////////
		//
		//	setPos
		//
		////////////////////////////////////////////////////////////////

		/// @brief 書き込み位置を変更します。
		/// @param pos 新しい書き込み位置（バイト）
		/// @return 書き込み位置の変更に成功した場合 true, それ以外の場合は false
		bool setPos(int64 pos) override;

		////////////////////////////////////////////////////////////////
		//
		//	seekToEnd
		//
		////////////////////////////////////////////////////////////////

		/// @brief 書き込み位置をファイルの終端に移動させます。
		/// @return 新しい書き込み位置（バイト）
		int64 seekToEnd();

		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在の書き込み位置にデータを書き込みます。
		/// @param src 書き込むデータ
		/// @param writeSize 書き込むサイズ（バイト）
		/// @remark 書き込み位置がファイルの終端の場合、書き込んだ分だけファイルのサイズが拡張されます。
		/// @return バッファに受け入れた、または OS に渡したバイト数。閉じている場合や以前の書き込みエラーがある場合は 0。
		/// @remark バッファへの受け入れ後に失敗する場合もあるため、保存結果は明示的な close() で確認してください。エラー後の書き込みは次の open() が成功するまで停止します。
		int64 write(const void* src, int64 writeSize) override;

		/// @brief 現在の書き込み位置にデータを書き込みます。
		/// @param src 書き込むデータ
		/// @remark 書き込み位置がファイルの終端の場合、書き込んだ分だけファイルのサイズが拡張されます。
		/// @return 書き込みに成功した場合 true, それ以外の場合は false
		bool write(const Concept::TriviallyCopyable auto& src);

		////////////////////////////////////////////////////////////////
		//
		//	path
		//
		////////////////////////////////////////////////////////////////

		/// @brief 開いているファイルのパスを返します。
		/// @return 開いているファイルのパス。ファイルが開いていない場合は空の文字列
		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		class BinaryFileWriterDetail;

		std::unique_ptr<BinaryFileWriterDetail> pImpl;
	};
}

# include "detail/BinaryFileWriter.ipp"
