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
# include "Array.hpp"
# include "String.hpp"
# include "TextEncoding.hpp"
# include "Optional.hpp"

namespace s3d
{
	/// @brief 読み込み用テキストファイル
	class TextReader
	{
	public:

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		TextReader();

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		explicit TextReader(FilePathView path, const Optional<TextEncoding>& encoding = unspecified);

		/// @brief テキストファイルを IReader 経由でオープンします。
		/// @tparam Reader IReader オブジェクトの型
		/// @param reader IReader オブジェクト
		/// @param encoding テキストのエンコーディング形式
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		explicit TextReader(Reader&& reader, const Optional<TextEncoding>& encoding = unspecified);

		/// @brief テキストファイルを IReader 経由でオープンします。
		/// @param reader IReader オブジェクト
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		explicit TextReader(std::unique_ptr<IReader>&& reader, const Optional<TextEncoding>& encoding = unspecified);

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param encoding テキストのエンコーディング形式
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		bool open(FilePathView path, const Optional<TextEncoding>& encoding = unspecified);

		/// @brief テキストファイルを IReader 経由でオープンします。
		/// @tparam Reader IReader オブジェクトの型
		/// @param reader IReader オブジェクト
		/// @param encoding テキストのエンコーディング形式
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		bool open(Reader&& reader, const Optional<TextEncoding>& encoding = unspecified);

		/// @brief テキストファイルを IReader 経由でオープンします。
		/// @param reader IReader オブジェクト
		/// @param encoding テキストのエンコーディング形式
		/// @return ファイルのオープンに成功した場合 true, それ以外の場合は false
		bool open(std::unique_ptr<IReader>&& reader, const Optional<TextEncoding>& encoding = unspecified);

		/// @brief テキストファイルをクローズします。
		void close();

		/// @brief テキストファイルがオープンされているかを返します。
		/// @return ファイルがオープンされている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOpen() const noexcept;

		/// @brief テキストファイルがオープンされているかを返します。
		/// @remark `isOpen()` と同じです。
		/// @return ファイルがオープンされている場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		/// @brief テキストファイルから 1 文字読み込みます。
		/// @return 読み込みに成功した場合はその文字の Optional, ファイルの終端や失敗の場合は none
		[[nodiscard]]
		Optional<char32> readChar();

		/// @brief テキストファイルから 1 行読み込みます。
		/// @return 読み込みに成功した場合はその文字列の Optional, ファイルの終端や失敗の場合は none
		[[nodiscard]]
		Optional<String> readLine();

		/// @brief テキストファイルのすべての行を読み込みます。
		/// @return 読み込んだすべての行
		[[nodiscard]]
		Array<String> readLines();

		/// @brief テキストファイルの内容をすべて読み込みます。
		/// @return 読み込んだ文字列
		[[nodiscard]]
		String readAll();

		/// @brief テキストファイルから 1 文字読み込みます。
		/// @param ch 読み込んだ文字の格納先
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readChar(char32& ch);

		/// @brief テキストファイルから 1 行読み込みます。
		/// @param line 読み込んだ文字列の格納先
		/// @remark line が十分なキャパシティを持っている場合、メモリの再確保が発生しないため、引数の無い `readLine()` より効率的です。
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readLine(std::string& line);

		/// @brief テキストファイルから 1 行読み込みます。
		/// @param line 読み込んだ文字列の格納先
		/// @remark line が十分なキャパシティを持っている場合、メモリの再確保が発生しないため、引数の無い `readLine()` より効率的です。
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readLine(String& line);

		/// @brief テキストファイルのすべての行を読み込みます。
		/// @param lines 読み込んだ文字列の格納先
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readLines(Array<std::string>& lines);

		/// @brief テキストファイルのすべての行を読み込みます。
		/// @param lines 読み込んだ文字列の格納先
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readLines(Array<String>& lines);

		/// @brief テキストファイルの内容をすべて読み込みます。
		/// @param s 読み込んだ文字列の格納先
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readAll(std::string& s);

		/// @brief テキストファイルの内容をすべて読み込みます。
		/// @param s 読み込んだ文字列の格納先
		/// @return 読み込みに成功した場合 true, ファイルの終端や失敗の場合は false
		bool readAll(String& s);

		/// @brief テキストのエンコーディング形式を返します。
		/// @return テキストのエンコーディング形式
		[[nodiscard]]
		TextEncoding encoding() const noexcept;

		/// @brief オープンしているファイルのフルパスを返します。
		/// @return ファイルがオープンしている場合、ファイルのフルパス。それ以外の場合は空の文字列。
		[[nodiscard]]
		const FilePath& path() const noexcept;

	private:

		class TextReaderDetail;

		std::shared_ptr<TextReaderDetail> pImpl;
	};
}

//# include "detail/TextReader.ipp"
