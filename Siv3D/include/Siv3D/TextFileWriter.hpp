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
# include "String.hpp"
# include "TextEncoding.hpp"
# include "OpenMode.hpp"
# include "Format.hpp"

namespace s3d
{
	class TextFileWriter;

	namespace detail
	{
		/// @brief テキストファイル書き込みバッファ
		class TextFileWriterBuffer
		{
		public:

			std::unique_ptr<FormatData> formatData;

			TextFileWriterBuffer(TextFileWriter& writer);

			TextFileWriterBuffer(TextFileWriterBuffer&& other) noexcept;

			~TextFileWriterBuffer();

			/// @brief テキストファイル書き込みバッファにデータを書き込みます。
			/// @param value 書き込むデータ
			/// @return *this
			TextFileWriterBuffer& operator <<(const Concept::Formattable auto& value);

		private:

			TextFileWriter& m_writer;

			bool m_isLast = false;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	TextFileWriter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 書き込み用テキストファイル
	class TextFileWriter
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		TextFileWriter();

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		TextFileWriter(FilePathView path, TextEncoding encoding);

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param openMode ファイルのオープンモード
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		explicit TextFileWriter(FilePathView path, OpenMode openMode = OpenMode::Trunc, TextEncoding encoding = TextEncoding::UTF8_WITH_BOM);

		TextFileWriter(const TextFileWriter& other) = delete;

		/// @brief ムーブコンストラクタ
		/// @param other ムーブする TextFileWriter
		TextFileWriter(TextFileWriter&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		~TextFileWriter();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		TextFileWriter& operator =(const TextFileWriter& other) = delete;

		/// @brief ムーブ代入演算子
		/// @param other ムーブする TextFileReader
		/// @return *this
		TextFileWriter& operator =(TextFileWriter&& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	open
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param encoding テキストのエンコーディング形式
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		bool open(FilePathView path, TextEncoding encoding);

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param openMode ファイルのオープンモード
		/// @param encoding テキストのエンコーディング形式
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		bool open(FilePathView path, OpenMode openMode = OpenMode::Trunc, TextEncoding encoding = TextEncoding::UTF8_WITH_BOM);

		////////////////////////////////////////////////////////////////
		//
		//	close
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストファイルをクローズします。
		void close();

		////////////////////////////////////////////////////////////////
		//
		//	isOpen
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストファイルがオープンされているかを返します。
		/// @return ファイルがオープンされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストファイルがオープンされているかを返します。
		/// @remark `isOpen()` と同じです。
		/// @return ファイルがオープンされている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在開いているファイルの内容を消去し、書き込み位置を先頭に戻します。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルに文字を書き込みます。
		/// @param ch 書き込む文字
		void write(char ch) = delete;

		/// @brief ファイルに文字を書き込みます。
		/// @param ch 書き込む文字
		void write(char32 ch);

		/// @brief ファイルに文字列を書き込みます。
		/// @param s 書き込む文字列
		void write(const char32* s);

		/// @brief ファイルに文字列を書き込みます。
		/// @param s 書き込む文字列
		void write(StringView s);

		/// @brief ファイルに文字列を書き込みます。
		/// @param s 書き込む文字列
		void write(const String& s);

		/// @brief データを文字列に変換してファイルに書き込みます。
		/// @param args 書き込むデータ
		void write(const Concept::Formattable auto& ... args);

		////////////////////////////////////////////////////////////////
		//
		//	writeln
		//
		////////////////////////////////////////////////////////////////

		void writeln(char ch) = delete;

		/// @brief ファイルに文字を書き込み、改行します。
		/// @param ch 書き込む文字
		void writeln(char32 ch);

		/// @brief ファイルに文字列を書き込み、改行します。
		/// @param s 書き込む文字列
		void writeln(const char32* s);

		/// @brief ファイルに文字列を書き込み、改行します。
		/// @param s 書き込む文字列
		void writeln(StringView s);

		/// @brief ファイルに文字列を書き込み、改行します。
		/// @param s 書き込む文字列
		void writeln(const String& s);

		/// @brief データを文字列に変換してファイルに書き込み、改行します。
		/// @param args 書き込むデータ
		void writeln(const Concept::Formattable auto& ... args);

		////////////////////////////////////////////////////////////////
		//
		//	writeUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルに UTF-8 文字列を書き込みます。
		/// @remark ファイルのエンコーディング形式が `UTF8_NO_BOM` `UTF8_WITH_BOM` である場合、エンコーディング変換をスキップするためパフォーマンスが向上します。
		/// @param s UTF-8 文字列
		void writeUTF8(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	writelnUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief ファイルに UTF-8 文字列を書き込み、改行します。
		/// @remark ファイルのエンコーディング形式が `UTF8_NO_BOM` `UTF8_WITH_BOM` である場合、エンコーディング変換をスキップするためパフォーマンスが向上します。
		/// @param s UTF-8 文字列
		void writelnUTF8(std::string_view s);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		/// @brief データを文字列に変換してファイルに書き込み、一連の `<<` が終わったあとに改行します。
		/// @param value 書き込むデータ
		/// @return 続いて `<<` できるオブジェクト
		detail::TextFileWriterBuffer operator <<(const Concept::Formattable auto& value);

		////////////////////////////////////////////////////////////////
		//
		//	encoding
		//
		////////////////////////////////////////////////////////////////

		/// @brief テキストのエンコーディング形式を返します。
		/// @return テキストのエンコーディング形式
		[[nodiscard]]
		TextEncoding encoding() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	path
		//
		////////////////////////////////////////////////////////////////

		/// @brief オープンしているファイルのフルパスを返します。
		/// @return ファイルがオープンしている場合、ファイルのフルパス。それ以外の場合は空の文字列。
		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		class TextFileWriterDetail;

		std::unique_ptr<TextFileWriterDetail> pImpl;
	};
}

# include "detail/TextFileWriter.ipp"
