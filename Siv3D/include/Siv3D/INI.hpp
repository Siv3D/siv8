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
# include "Result.hpp"
# include "IReader.hpp"
# include "HashMap.hpp"
# include "INIItem.hpp"
# include "INISection.hpp"
# include "INIParseErrorReason.hpp"
# include "INIReadOptions.hpp"
# include "INIWriteOptions.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	INI
	//
	////////////////////////////////////////////////////////////////

	/// @brief INI 形式のデータを保持し、読み込み・編集・書き出しを行うクラス | Class for storing, loading, editing, and writing INI data
	/// @remark INI の値は文字列として保持され、get() / getOpt() / getOr() で指定した型に変換して取得できます。 | INI values are stored as strings and can be converted to requested types by get(), getOpt(), and getOr().
	/// @remark 空の INI データは有効です。読み込みやパースに失敗した場合は Invalid() による無効な INI データになります。 | Empty INI data is valid. Failed loading or parsing produces invalid INI data created by Invalid().
	class INI
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief 有効な空の INI データを作成します。 | Creates valid empty INI data.
		[[nodiscard]]
		INI() = default;

		/// @brief INI 形式のデータをファイルからロードして作成します。 | Loads INI data from a file.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @remark ロードに失敗した場合、無効な INI データになります。詳細なエラー情報が必要な場合は LoadResult() を使います。 | If loading fails, the created INI data is invalid. Use LoadResult() to obtain detailed error information.
		[[nodiscard]]
		explicit INI(FilePathView path, const INIReadOptions& options = {});

		/// @brief Reader から INI 形式のデータをロードして作成します。 | Loads INI data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @remark ロードに失敗した場合、無効な INI データになります。詳細なエラー情報が必要な場合は LoadResult() を使います。 | If loading fails, the created INI data is invalid. Use LoadResult() to obtain detailed error information.
		[[nodiscard]]
		explicit INI(std::unique_ptr<IReader> reader, const INIReadOptions& options = {});

		/// @brief Reader から INI 形式のデータをロードして作成します。 | Loads INI data from a reader.
		/// @tparam Reader Reader 型 | Reader type
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @remark ロードに失敗した場合、無効な INI データになります。詳細なエラー情報が必要な場合は LoadResult() を使います。 | If loading fails, the created INI data is invalid. Use LoadResult() to obtain detailed error information.
		template <ReaderObject Reader>
		[[nodiscard]]
		explicit INI(Reader&& reader, const INIReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	isValid, isInvalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI データが有効であるかを返します。 | Returns whether the INI data is valid.
		/// @return 有効な INI データの場合 true, それ以外の場合は false | True if the INI data is valid, otherwise false
		/// @remark 空の INI データも有効です。 | Empty INI data is also valid.
		[[nodiscard]]
		bool isValid() const noexcept;

		/// @brief INI データが無効であるかを返します。 | Returns whether the INI data is invalid.
		/// @return 無効な INI データの場合 true, それ以外の場合は false | True if the INI data is invalid, otherwise false
		[[nodiscard]]
		bool isInvalid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI データが有効であるかを返します。 | Returns whether the INI data is valid.
		/// @return 有効な INI データの場合 true, それ以外の場合は false | True if the INI data is valid, otherwise false
		/// @remark isValid() と同じです。データが空であるかは isEmpty() で確認します。 | Same as isValid(). Use isEmpty() to check whether the data is empty.
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief データが空であるかを返します。 | Returns whether the data is empty.
		/// @return データが空である場合 true, それ以外の場合は false | True if the data is empty, otherwise false
		/// @remark 無効な INI データでも、保持しているセクションやプロパティが無ければ true を返します。 | Invalid INI data may also return true if it has no sections or properties.
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 保持しているデータを消去し、有効な空の INI データにします。 | Clears the data and makes it valid empty INI data.
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	sections
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションの一覧を返します。 | Returns sections.
		/// @return セクションの一覧 | Sections
		/// @remark 返される配列の順序は、セクションが追加された順序です。 | The returned array is ordered by section insertion order.
		[[nodiscard]]
		const Array<INISection>& sections() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションが存在するかを返します。 | Returns whether the specified section exists.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @return 指定したセクションが存在する場合 true, それ以外の場合は false | True if the specified section exists, otherwise false
		[[nodiscard]]
		bool hasSection(StringView section) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションへの参照を返します。 | Returns a reference to the section.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @return セクションへの参照 | Reference to the section
		/// @throw Error 指定したセクションが存在しない場合 | Throws Error if the section does not exist.
		[[nodiscard]]
		const INISection& getSection(StringView section) const;

		/// @brief セクションへの参照を返します。 | Returns a reference to the section.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @return セクションへの参照 | Reference to the section
		/// @throw Error 指定したセクションが存在しない場合 | Throws Error if the section does not exist.
		[[nodiscard]]
		INISection& getSection(StringView section);

		////////////////////////////////////////////////////////////////
		//
		//	hasGlobalSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションが存在するかを返します。 | Returns whether the global section exists.
		/// @return グローバル（無名）セクションが存在する場合 true, それ以外の場合は false | True if the global section exists, otherwise false
		[[nodiscard]]
		bool hasGlobalSection() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションへの参照を返します。 | Returns a reference to the global section.
		/// @return グローバル（無名）セクションへの参照 | Reference to the global section
		/// @throw Error グローバル（無名）セクションが存在しない場合 | Throws Error if the global section does not exist.
		[[nodiscard]]
		const INISection& getGlobalSection() const;

		/// @brief グローバル（無名）セクションへの参照を返します。 | Returns a reference to the global section.
		/// @return グローバル（無名）セクションへの参照 | Reference to the global section
		/// @throw Error グローバル（無名）セクションが存在しない場合 | Throws Error if the global section does not exist.
		[[nodiscard]]
		INISection& getGlobalSection();

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションへの参照を返します。 | Returns a reference to the section.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @return セクションへの参照 | Reference to the section
		/// @remark getSection() と同じです。存在しない場合は Error を投げます。 | Same as getSection(). Throws Error if the section does not exist.
		[[nodiscard]]
		const INISection& operator [](StringView section) const;

		/// @brief セクションへの参照を返します。 | Returns a reference to the section.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @return セクションへの参照 | Reference to the section
		/// @remark getSection() と同じです。存在しない場合は Error を投げます。 | Same as getSection(). Throws Error if the section does not exist.
		[[nodiscard]]
		INISection& operator [](StringView section);

		/// @brief グローバル（無名）セクションへの参照を返します。 | Returns a reference to the global section.
		/// @return グローバル（無名）セクションへの参照 | Reference to the global section
		/// @remark getGlobalSection() と同じです。存在しない場合は Error を投げます。 | Same as getGlobalSection(). Throws Error if the global section does not exist.
		[[nodiscard]]
		const INISection& operator []() const;

		/// @brief グローバル（無名）セクションへの参照を返します。 | Returns a reference to the global section.
		/// @return グローバル（無名）セクションへの参照 | Reference to the global section
		/// @remark getGlobalSection() と同じです。存在しない場合は Error を投げます。 | Same as getGlobalSection(). Throws Error if the global section does not exist.
		[[nodiscard]]
		INISection& operator []();

		////////////////////////////////////////////////////////////////
		//
		//	hasProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションにプロパティが存在するかを返します。 | Returns whether the specified section contains the property.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @param key プロパティのキー | Property key
		/// @return プロパティが存在する場合 true, それ以外の場合は false | True if the property exists, otherwise false
		[[nodiscard]]
		bool hasProperty(StringView section, StringView key) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasGlobalProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションにプロパティが存在するかを返します。 | Returns whether the global section contains the property.
		/// @param key プロパティのキー | Property key
		/// @return プロパティが存在する場合 true, それ以外の場合は false | True if the property exists, otherwise false
		[[nodiscard]]
		bool hasGlobalProperty(StringView key) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションのプロパティ値を指定した型に変換して返します。 | Returns the property value in the specified section converted to the specified type.
		/// @tparam Type 取得する型 | Type to get
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @param key プロパティのキー | Property key
		/// @return 変換された値 | Converted value
		/// @throw Error セクションまたはプロパティが存在しない場合、または値の変換に失敗した場合 | Throws Error if the section or property does not exist, or if conversion fails.
		template <class Type>
		[[nodiscard]]
		Type get(StringView section, StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションのプロパティ値を指定した型に変換して返します。失敗した場合はデフォルト値を返します。 | Returns the property value in the specified section converted to the specified type, or a default value on failure.
		/// @tparam Type 取得する型 | Type to get
		/// @tparam U デフォルト値の型 | Type of the default value
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @param key プロパティのキー | Property key
		/// @param defaultValue デフォルト値 | Default value
		/// @return 変換された値。セクションまたはプロパティが存在しない場合、または値の変換に失敗した場合は defaultValue | Converted value, or defaultValue if the section or property does not exist or conversion fails
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(StringView section, StringView key, U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションのプロパティ値を指定した型に変換して返します。 | Returns the property value in the specified section converted to the specified type.
		/// @tparam Type 取得する型 | Type to get
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @param key プロパティのキー | Property key
		/// @return 変換に成功した場合はその値、失敗した場合は none | Converted value if successful, otherwise none
		/// @remark セクションまたはプロパティが存在しない場合も none を返します。 | Returns none if the section or property does not exist.
		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt(StringView section, StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobal
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションのプロパティ値を指定した型に変換して返します。 | Returns the property value in the global section converted to the specified type.
		/// @tparam Type 取得する型 | Type to get
		/// @param key プロパティのキー | Property key
		/// @return 変換された値 | Converted value
		/// @throw Error グローバル（無名）セクションまたはプロパティが存在しない場合、または値の変換に失敗した場合 | Throws Error if the global section or property does not exist, or if conversion fails.
		template <class Type>
		[[nodiscard]]
		Type getGlobal(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションのプロパティ値を指定した型に変換して返します。失敗した場合はデフォルト値を返します。 | Returns the property value in the global section converted to the specified type, or a default value on failure.
		/// @tparam Type 取得する型 | Type to get
		/// @tparam U デフォルト値の型 | Type of the default value
		/// @param key プロパティのキー | Property key
		/// @param defaultValue デフォルト値 | Default value
		/// @return 変換された値。グローバル（無名）セクションまたはプロパティが存在しない場合、または値の変換に失敗した場合は defaultValue | Converted value, or defaultValue if the global section or property does not exist or conversion fails
		template <class Type, class U>
		[[nodiscard]]
		Type getGlobalOr(StringView key, U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getGlobalOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションのプロパティ値を指定した型に変換して返します。 | Returns the property value in the global section converted to the specified type.
		/// @tparam Type 取得する型 | Type to get
		/// @param key プロパティのキー | Property key
		/// @return 変換に成功した場合はその値、失敗した場合は none | Converted value if successful, otherwise none
		/// @remark グローバル（無名）セクションまたはプロパティが存在しない場合も none を返します。 | Returns none if the global section or property does not exist.
		template <class Type>
		[[nodiscard]]
		Optional<Type> getGlobalOpt(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	addSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションを追加します。 | Adds a section.
		/// @param section 追加するセクション名。空文字列の場合はグローバル（無名）セクション | Section name to add. Empty string means the global section.
		/// @remark 既に存在する場合は何もしません。 | Does nothing if it already exists.
		void addSection(StringView section);

		////////////////////////////////////////////////////////////////
		//
		//	removeSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief セクションを削除します。 | Removes a section.
		/// @param section 削除するセクション名。空文字列の場合はグローバル（無名）セクション | Section name to remove. Empty string means the global section.
		/// @remark 指定したセクションが存在しない場合は何もしません。 | Does nothing if the specified section does not exist.
		void removeSection(StringView section);

		////////////////////////////////////////////////////////////////
		//
		//	removeGlobalSection
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションを削除します。 | Removes the global section.
		/// @remark グローバル（無名）セクションが存在しない場合は何もしません。 | Does nothing if the global section does not exist.
		void removeGlobalSection();

		////////////////////////////////////////////////////////////////
		//
		//	addProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションにプロパティを追加または更新します。 | Adds or updates a property in the specified section.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @param key プロパティのキー | Property key
		/// @param value プロパティの値 | Property value
		/// @remark 指定したセクションが存在しない場合は作成されます。 | If the specified section does not exist, it is created.
		void addProperty(StringView section, StringView key, String value);

		////////////////////////////////////////////////////////////////
		//
		//	addGlobalProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションにプロパティを追加または更新します。 | Adds or updates a property in the global section.
		/// @param key プロパティのキー | Property key
		/// @param value プロパティの値 | Property value
		void addGlobalProperty(StringView key, String value);

		////////////////////////////////////////////////////////////////
		//
		//	removeProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したセクションからプロパティを削除します。 | Removes a property from the specified section.
		/// @param section セクション名。空文字列の場合はグローバル（無名）セクション | Section name. Empty string means the global section.
		/// @param key 削除するプロパティのキー | Property key to remove
		/// @remark 指定したセクションまたはプロパティが存在しない場合は何もしません。 | Does nothing if the specified section or property does not exist.
		void removeProperty(StringView section, StringView key);

		////////////////////////////////////////////////////////////////
		//
		//	removeGlobalProperty
		//
		////////////////////////////////////////////////////////////////

		/// @brief グローバル（無名）セクションからプロパティを削除します。 | Removes a property from the global section.
		/// @param key 削除するプロパティのキー | Property key to remove
		/// @remark グローバル（無名）セクションまたはプロパティが存在しない場合は何もしません。 | Does nothing if the global section or property does not exist.
		void removeGlobalProperty(StringView key);

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータを文字列に変換します。 | Formats the data as INI text.
		/// @param options 書き出しオプション | Write options
		/// @return INI 形式のデータ | INI text
		[[nodiscard]]
		String format(const INIWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータを UTF-8 文字列に変換します。 | Formats the data as UTF-8 INI text.
		/// @param options 書き出しオプション | Write options
		/// @return UTF-8 文字列 | UTF-8 string
		[[nodiscard]]
		std::string formatUTF8(const INIWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータをファイルに保存します。 | Saves the data as an INI file.
		/// @param path ファイルパス | File path
		/// @param options 書き出しオプション | Write options
		/// @return 保存に成功した場合 true, それ以外の場合は false | True if saving succeeded, otherwise false
		bool save(FilePathView path, const INIWriteOptions& options = {}) const;

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 の文字列から INI データをパースします。 | Parses INI data from a UTF-8 string.
		/// @param s INI 形式の UTF-8 文字列 | UTF-8 INI text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパスまたは名前 | Path or name used in error messages
		/// @return パースに成功した場合は INI データ、失敗した場合は無効な INI データ | Parsed INI data on success, otherwise invalid INI data
		/// @remark 詳細なエラー情報が必要な場合は ParseResult() を使います。 | Use ParseResult() to obtain detailed error information.
		[[nodiscard]]
		static INI Parse(std::string_view s, const INIReadOptions& options = {}, FilePathView pathHint = U"");

		/// @brief 文字列から INI データをパースします。 | Parses INI data from a string.
		/// @param s INI 形式の文字列 | INI text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパスまたは名前 | Path or name used in error messages
		/// @return パースに成功した場合は INI データ、失敗した場合は無効な INI データ | Parsed INI data on success, otherwise invalid INI data
		/// @remark 詳細なエラー情報が必要な場合は ParseResult() を使います。 | Use ParseResult() to obtain detailed error information.
		[[nodiscard]]
		static INI Parse(StringView s, const INIReadOptions& options = {}, FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	ParseResult
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 の文字列から INI データをパースし、結果またはエラー情報を返します。 | Parses INI data from a UTF-8 string and returns either the result or error information.
		/// @param s INI 形式の UTF-8 文字列 | UTF-8 INI text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパスまたは名前 | Path or name used in error messages
		/// @return 成功した場合は INI データ、失敗した場合はエラー情報の配列 | INI data on success, otherwise an array of error reasons
		[[nodiscard]]
		static Result<INI, Array<INIParseErrorReason>> ParseResult(std::string_view s, const INIReadOptions& options = {}, FilePathView pathHint = U"");

		/// @brief 文字列から INI データをパースし、結果またはエラー情報を返します。 | Parses INI data from a string and returns either the result or error information.
		/// @param s INI 形式の文字列 | INI text
		/// @param options 読み込みオプション | Read options
		/// @param pathHint エラー表示用のパスまたは名前 | Path or name used in error messages
		/// @return 成功した場合は INI データ、失敗した場合はエラー情報の配列 | INI data on success, otherwise an array of error reasons
		[[nodiscard]]
		static Result<INI, Array<INIParseErrorReason>> ParseResult(StringView s, const INIReadOptions& options = {}, FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータをファイルからロードします。 | Loads INI data from a file.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @return ロードに成功した場合は INI データ、失敗した場合は無効な INI データ | Loaded INI data on success, otherwise invalid INI data
		/// @remark 詳細なエラー情報が必要な場合は LoadResult() を使います。 | Use LoadResult() to obtain detailed error information.
		[[nodiscard]]
		static INI Load(FilePathView path, const INIReadOptions& options = {});

		/// @brief INI 形式のデータを Reader からロードします。 | Loads INI data from a reader.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return ロードに成功した場合は INI データ、失敗した場合は無効な INI データ | Loaded INI data on success, otherwise invalid INI data
		/// @remark 詳細なエラー情報が必要な場合は LoadResult() を使います。 | Use LoadResult() to obtain detailed error information.
		[[nodiscard]]
		static INI Load(std::unique_ptr<IReader> reader, const INIReadOptions& options = {});

		/// @brief INI 形式のデータを Reader からロードします。 | Loads INI data from a reader.
		/// @tparam Reader Reader 型 | Reader type
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return ロードに成功した場合は INI データ、失敗した場合は無効な INI データ | Loaded INI data on success, otherwise invalid INI data
		/// @remark 詳細なエラー情報が必要な場合は LoadResult() を使います。 | Use LoadResult() to obtain detailed error information.
		template <ReaderObject Reader>
		[[nodiscard]]
		static INI Load(Reader&& reader, const INIReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	LoadResult
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータをファイルからロードし、結果またはエラー情報を返します。 | Loads INI data from a file and returns either the result or error information.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @return 成功した場合は INI データ、失敗した場合はエラー情報の配列 | INI data on success, otherwise an array of error reasons
		[[nodiscard]]
		static Result<INI, Array<INIParseErrorReason>> LoadResult(FilePathView path, const INIReadOptions& options = {});

		/// @brief INI 形式のデータを Reader からロードし、結果またはエラー情報を返します。 | Loads INI data from a reader and returns either the result or error information.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 成功した場合は INI データ、失敗した場合はエラー情報の配列 | INI data on success, otherwise an array of error reasons
		[[nodiscard]]
		static Result<INI, Array<INIParseErrorReason>> LoadResult(std::unique_ptr<IReader> reader, const INIReadOptions& options = {});

		/// @brief INI 形式のデータを Reader からロードし、結果またはエラー情報を返します。 | Loads INI data from a reader and returns either the result or error information.
		/// @tparam Reader Reader 型 | Reader type
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return 成功した場合は INI データ、失敗した場合はエラー情報の配列 | INI data on success, otherwise an array of error reasons
		template <ReaderObject Reader>
		[[nodiscard]]
		static Result<INI, Array<INIParseErrorReason>> LoadResult(Reader&& reader, const INIReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	load
		//
		////////////////////////////////////////////////////////////////

		/// @brief INI 形式のデータをファイルからロードし、現在のデータを置き換えます。 | Loads INI data from a file and replaces the current data.
		/// @param path ファイルパス | File path
		/// @param options 読み込みオプション | Read options
		/// @return ロードに成功した場合 true, それ以外の場合は false | True if loading succeeded, otherwise false
		/// @remark 失敗した場合、現在のデータは消去され、無効な INI データになります。 | On failure, the current data is cleared and becomes invalid INI data.
		bool load(FilePathView path, const INIReadOptions& options = {});

		/// @brief INI 形式のデータを Reader からロードし、現在のデータを置き換えます。 | Loads INI data from a reader and replaces the current data.
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return ロードに成功した場合 true, それ以外の場合は false | True if loading succeeded, otherwise false
		/// @remark 失敗した場合、現在のデータは消去され、無効な INI データになります。 | On failure, the current data is cleared and becomes invalid INI data.
		bool load(std::unique_ptr<IReader> reader, const INIReadOptions& options = {});

		/// @brief INI 形式のデータを Reader からロードし、現在のデータを置き換えます。 | Loads INI data from a reader and replaces the current data.
		/// @tparam Reader Reader 型 | Reader type
		/// @param reader Reader
		/// @param options 読み込みオプション | Read options
		/// @return ロードに成功した場合 true, それ以外の場合は false | True if loading succeeded, otherwise false
		/// @remark 失敗した場合、現在のデータは消去され、無効な INI データになります。 | On failure, the current data is cleared and becomes invalid INI data.
		template <ReaderObject Reader>
		bool load(Reader&& reader, const INIReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief UTF-8 の文字列から INI データをパースし、現在のデータを置き換えます。 | Parses INI data from a UTF-8 string and replaces the current data.
		/// @param s INI 形式の UTF-8 文字列 | UTF-8 INI text
		/// @param options 読み込みオプション | Read options
		/// @return パースに成功した場合 true, それ以外の場合は false | True if parsing succeeded, otherwise false
		/// @remark 失敗した場合、現在のデータは消去され、無効な INI データになります。 | On failure, the current data is cleared and becomes invalid INI data.
		bool parse(std::string_view s, const INIReadOptions& options = {});

		/// @brief 文字列から INI データをパースし、現在のデータを置き換えます。 | Parses INI data from a string and replaces the current data.
		/// @param s INI 形式の文字列 | INI text
		/// @param options 読み込みオプション | Read options
		/// @return パースに成功した場合 true, それ以外の場合は false | True if parsing succeeded, otherwise false
		/// @remark 失敗した場合、現在のデータは消去され、無効な INI データになります。 | On failure, the current data is cleared and becomes invalid INI data.
		bool parse(StringView s, const INIReadOptions& options = {});

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無効な INI データを作成します。 | Creates invalid INI data.
		/// @return 無効な INI データ | Invalid INI data
		[[nodiscard]]
		static INI Invalid();

	private:

		static constexpr StringView GlobalSection{};

		Array<INISection> m_sections;

		HashMap<String, size_t> m_sectionIndex;

		bool m_isValid = true;

		const String* getPropertyValue(StringView section, StringView key) const;
	};
}

# include "detail/INI.ipp"
