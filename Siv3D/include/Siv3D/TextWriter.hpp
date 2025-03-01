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
# include <memory>
# include "Common.hpp"
# include "String.hpp"
# include "TextEncoding.hpp"
# include "OpenMode.hpp"
# include "Format.hpp"

namespace s3d
{
	class TextWriter;

	namespace detail
	{
		/// @brief テキストファイル書き込みバッファ
		class TextWriterBuffer
		{
		public:

			std::unique_ptr<FormatData> formatData;

			TextWriterBuffer(TextWriter& writer);

			TextWriterBuffer(TextWriterBuffer&& other) noexcept;

			~TextWriterBuffer();

			/// @brief テキストファイル書き込みバッファにデータを書き込みます。
			/// @param value 書き込むデータ
			/// @return *this
			TextWriterBuffer& operator <<(const Concept::Formattable auto& value);

		private:

			TextWriter& m_writer;

			bool m_isLast = false;
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	TextWriter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 書き込み用テキストファイル
	class TextWriter
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		TextWriter();

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		TextWriter(FilePathView path, TextEncoding encoding);

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param openMode ファイルのオープンモード
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		explicit TextWriter(FilePathView path, OpenMode openMode = OpenMode::Trunc, TextEncoding encoding = TextEncoding::UTF8_WITH_BOM);

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
		detail::TextWriterBuffer operator <<(const Concept::Formattable auto& value);

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

		class TextWriterDetail;

		std::shared_ptr<TextWriterDetail> pImpl;
	};
}

# include "detail/TextWriter.ipp"
