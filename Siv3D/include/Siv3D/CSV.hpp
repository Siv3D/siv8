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
# include "Optional.hpp"
# include "Parse.hpp"
# include "Result.hpp"
# include "IReader.hpp"
# include "CSVParseErrorReason.hpp"
# include "CSVReadOptions.hpp"
# include "CSVWriteOptions.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	CSV
	//
	////////////////////////////////////////////////////////////////

	/// @brief RFC 4180 CSV 形式のデータの読み書き | Reading and writing RFC 4180 CSV data
	class CSV
	{
	public:

		using Row = Array<String>;

		using Table = Array<Row>;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の CSV データを作成します。 | Creates an empty CSV data.
		[[nodiscard]]
		CSV() = default;

		/// @brief CSV ファイルからデータを読み込みます。 | Loads CSV data from a file.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		[[nodiscard]]
		explicit CSV(FilePathView path, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		template <ReaderObject Reader>
		[[nodiscard]]
		explicit CSV(Reader&& reader, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		[[nodiscard]]
		explicit CSV(std::unique_ptr<IReader> reader, const CSVReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	isValid, isInvalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データが有効であるかを返します。 | Returns whether the CSV data is valid.
		/// @return CSV データが有効である場合 true, それ以外の場合は false | True if the CSV data is valid, otherwise false
		[[nodiscard]]
		bool isValid() const noexcept;

		/// @brief CSV データが無効であるかを返します。 | Returns whether the CSV data is invalid.
		/// @return CSV データが無効である場合 true, それ以外の場合は false | True if the CSV data is invalid, otherwise false
		[[nodiscard]]
		bool isInvalid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データが有効であるかを返します。 | Returns whether the CSV data is valid.
		/// @return CSV データが有効である場合 true, それ以外の場合は false | True if the CSV data is valid, otherwise false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データが空であるかを返します。 | Returns whether the CSV data is empty.
		/// @return CSV データが空である場合 true, それ以外の場合は false | True if the CSV data is empty, otherwise false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rows
		//
		////////////////////////////////////////////////////////////////

		/// @brief レコード数を返します。 | Returns the number of records.
		/// @return レコード数 | Number of records
		[[nodiscard]]
		size_t rows() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	columns
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したレコードのフィールド数を返します。 | Returns the number of fields in the specified record.
		/// @param row レコードのインデックス | Record index
		/// @return 指定したレコードのフィールド数 | Number of fields in the specified record
		[[nodiscard]]
		size_t columns(size_t row) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	inBounds
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置が範囲内であるかを返します。 | Returns whether the specified position is in bounds.
		/// @param row レコードのインデックス | Record index
		/// @param column フィールドのインデックス | Field index
		/// @return 範囲内の場合 true, それ以外の場合は false | True if the specified position is in bounds, otherwise false
		[[nodiscard]]
		bool inBounds(size_t row, size_t column) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の値を読み取ります。 | Reads a value at the specified position.
		/// @tparam Type 読み取る値の型 | Type of value to read
		/// @param row レコードのインデックス | Record index
		/// @param column フィールドのインデックス | Field index
		/// @return 読み取った値。失敗した場合はデフォルト値 | The read value, or the default value on failure
		template <class Type = String>
		[[nodiscard]]
		Type get(size_t row, size_t column) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の値を読み取ります。失敗した場合は defaultValue を返します。 | Reads a value at the specified position, or returns defaultValue on failure.
		/// @tparam Type 読み取る値の型 | Type of value to read
		/// @tparam U デフォルトの値の型 | Type of the default value
		/// @param row レコードのインデックス | Record index
		/// @param column フィールドのインデックス | Field index
		/// @param defaultValue デフォルトの値 | Default value
		/// @return 読み取った値。失敗した場合はデフォルトの値 | The read value, or the default value on failure
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(size_t row, size_t column, U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定した位置の値を読み取ります。失敗した場合は none を返します。 | Reads a value at the specified position, or returns none on failure.
		/// @tparam Type 読み取る値の型 | Type of value to read
		/// @param row レコードのインデックス | Record index
		/// @param column フィールドのインデックス | Field index
		/// @return 読み取った値。失敗した場合は none | The read value, or none on failure
		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt(size_t row, size_t column) const;

		////////////////////////////////////////////////////////////////
		//
		//	data
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データを返します。 | Returns the CSV table.
		/// @return CSV データ | CSV table
		[[nodiscard]]
		const Table& data() const noexcept;

		/// @brief CSV データを返します。 | Returns the CSV table.
		/// @return CSV データ | CSV table
		[[nodiscard]]
		Table& data() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	row, operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したレコードを返します。 | Returns the specified record.
		/// @param row レコードのインデックス | Record index
		/// @return 指定したレコード | The specified record
		[[nodiscard]]
		const Row& row(size_t row) const;

		/// @brief 指定したレコードを返します。 | Returns the specified record.
		/// @param row レコードのインデックス | Record index
		/// @return 指定したレコード | The specified record
		[[nodiscard]]
		Row& row(size_t row);

		/// @brief 指定したレコードを返します。 | Returns the specified record.
		/// @param row レコードのインデックス | Record index
		/// @return 指定したレコード | The specified record
		[[nodiscard]]
		const Row& operator [](size_t row) const;

		/// @brief 指定したレコードを返します。 | Returns the specified record.
		/// @param row レコードのインデックス | Record index
		/// @return 指定したレコード | The specified record
		[[nodiscard]]
		Row& operator [](size_t row);

		////////////////////////////////////////////////////////////////
		//
		//	claer
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データを消去し、有効な空 CSV にします。 | Clears the CSV data and makes it a valid empty CSV.
		void clear() noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	addRow
		//
		////////////////////////////////////////////////////////////////

		/// @brief レコードを追加します。 | Adds a record.
		/// @param row 追加するレコード | Record to add
		void addRow(Row row);

		/// @brief レコードを追加します。 | Adds a record.
		/// @param records 追加する値 | Values to add
		template <class ... Args>
		void addRow(const Args& ... records);

		////////////////////////////////////////////////////////////////
		//
		//	write
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のレコードにフィールドを追加します。 | Adds a field to the current record.
		/// @param field 追加するフィールド | Field to add
		void write(const String& field);

		/// @brief 現在のレコードにフィールドを追加します。 | Adds a field to the current record.
		/// @param value 追加する値 | Value to add
		template <class Type>
		void write(const Type& value);

		/// @brief 現在のレコードにフィールドを追加します。 | Adds fields to the current record.
		/// @param records 追加する値 | Values to add
		template <class ... Args>
		void write(const Args& ... records);

		////////////////////////////////////////////////////////////////
		//
		//	writeRow
		//
		////////////////////////////////////////////////////////////////

		/// @brief レコードを追加します。 | Adds a record.
		/// @param records 追加する値 | Values to add
		template <class ... Args>
		void writeRow(const Args& ... records);

		////////////////////////////////////////////////////////////////
		//
		//	newLine
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のレコードを終了します。 | Ends the current record.
		void newLine();

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データを文字列に変換します。 | Formats the CSV data as a string.
		/// @param options 書き込みオプション | Write options
		/// @return 変換された文字列 | Formatted string
		[[nodiscard]]
		String format(const CSVWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データを UTF-8 文字列に変換します。 | Formats the CSV data as a UTF-8 string.
		/// @param options 書き込みオプション | Write options
		/// @return 変換された UTF-8 文字列 | Formatted UTF-8 string
		[[nodiscard]]
		std::string formatUTF8(const CSVWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV データをファイルに保存します。 | Saves the CSV data to a file.
		/// @param path ファイルパス | File path
		/// @param options 書き込みオプション | Write options
		/// @return 保存に成功した場合 true, それ以外の場合は false | True if saving succeeded, otherwise false
		bool save(FilePathView path, const CSVWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV テキストをパースします。 | Parses CSV text.
		/// @param content CSV テキスト | CSV text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパス | Path hint for error messages
		/// @return パースされた CSV。失敗時は無効な CSV | Parsed CSV, or an invalid CSV on failure
		[[nodiscard]]
		static CSV Parse(std::string content, const CSVReadOptions& options = {}, FilePathView pathHint = U"");

		/// @brief CSV テキストをパースします。 | Parses CSV text.
		/// @param content CSV テキスト | CSV text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパス | Path hint for error messages
		/// @return パースされた CSV。失敗時は無効な CSV | Parsed CSV, or an invalid CSV on failure
		[[nodiscard]]
		static CSV Parse(StringView content, const CSVReadOptions& options = {}, FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	ParseResult
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV テキストをパースします。 | Parses CSV text.
		/// @param content CSV テキスト | CSV text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパス | Path hint for error messages
		/// @return パース結果 | Parse result
		[[nodiscard]]
		static Result<CSV, Array<CSVParseErrorReason>> ParseResult(std::string content, const CSVReadOptions& options = {}, FilePathView pathHint = U"");

		/// @brief CSV テキストをパースします。 | Parses CSV text.
		/// @param content CSV テキスト | CSV text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパス | Path hint for error messages
		/// @return パース結果 | Parse result
		[[nodiscard]]
		static Result<CSV, Array<CSVParseErrorReason>> ParseResult(StringView content, const CSVReadOptions& options = {}, FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV ファイルからデータを読み込みます。 | Loads CSV data from a file.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @return 読み込んだ CSV。失敗時は無効な CSV | Loaded CSV, or an invalid CSV on failure
		[[nodiscard]]
		static CSV Load(FilePathView path, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 読み込んだ CSV。失敗時は無効な CSV | Loaded CSV, or an invalid CSV on failure
		[[nodiscard]]
		static CSV Load(std::unique_ptr<IReader> reader, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 読み込んだ CSV。失敗時は無効な CSV | Loaded CSV, or an invalid CSV on failure
		template <ReaderObject Reader>
		[[nodiscard]]
		static CSV Load(Reader&& reader, const CSVReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	LoadResult
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV ファイルからデータを読み込みます。 | Loads CSV data from a file.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @return 読み込み結果 | Load result
		[[nodiscard]]
		static Result<CSV, Array<CSVParseErrorReason>> LoadResult(FilePathView path, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 読み込み結果 | Load result
		[[nodiscard]]
		static Result<CSV, Array<CSVParseErrorReason>> LoadResult(std::unique_ptr<IReader> reader, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 読み込み結果 | Load result
		template <ReaderObject Reader>
		[[nodiscard]]
		static Result<CSV, Array<CSVParseErrorReason>> LoadResult(Reader&& reader, const CSVReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	load
		//
		////////////////////////////////////////////////////////////////

		/// @brief CSV ファイルからデータを読み込みます。 | Loads CSV data from a file.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @return 読み込みに成功した場合 true, それ以外の場合は false | True if loading succeeded, otherwise false
		bool load(FilePathView path, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 読み込みに成功した場合 true, それ以外の場合は false | True if loading succeeded, otherwise false
		template <ReaderObject Reader>
		bool load(Reader&& reader, const CSVReadOptions& options = {});

		/// @brief Reader から CSV データを読み込みます。 | Loads CSV data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 読み込みに成功した場合 true, それ以外の場合は false | True if loading succeeded, otherwise false
		bool load(std::unique_ptr<IReader> reader, const CSVReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無効な CSV データを作成します。 | Creates invalid CSV data.
		/// @return 無効な CSV データ | Invalid CSV data
		[[nodiscard]]
		static CSV Invalid();

	private:

		Table m_data;

		bool m_isValid = true;

		bool m_isAtRowHead = true;

		[[nodiscard]]
		explicit CSV(Table data);

		[[nodiscard]]
		Optional<String> getItem(size_t row, size_t column) const;

		void _write();

		template <class Type, class ... Args>
		void _write(const Type& record, const Args& ... records);
	};
}

# include "detail/CSV.ipp"
