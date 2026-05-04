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
# include "DateTime.hpp"
# include "Result.hpp"
# include "IReader.hpp"
# include "TOMLValueType.hpp"
# include "TOMLParseErrorReason.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(5311)
# include <ThirdParty/toml11/toml.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	TOML
	//
	////////////////////////////////////////////////////////////////

	/// @brief TOML ドキュメントまたは TOML の値を参照するクラス
	class TOML
	{
	public:
		
		/// @brief 内部で使用する TOML 値の型
		using Value = toml::ordered_value;

		/// @brief TOML 配列のビュー
		class ArrayView;

		/// @brief TOML テーブルのビュー
		class TableView;

		/// @brief TOML テーブルのメンバ
		struct TableMember;

		/// @brief TOML の local time
		struct LocalTime
		{
			/// @brief 時
			uint8  hour = 0;

			/// @brief 分
			uint8  minute = 0;

			/// @brief 秒
			uint8  second = 0;

			/// @brief ミリ秒
			uint16 millisecond = 0;

			/// @brief マイクロ秒
			uint16 microsecond = 0;

			/// @brief ナノ秒
			uint16 nanosecond = 0;

			/// @brief DateTime に変換します。
			/// @return 変換された DateTime
			/// @remark DateTime が保持できないマイクロ秒およびナノ秒の情報は失われます。
			[[nodiscard]]
			DateTime toDateTime() const noexcept;

			/// @brief ミリ秒精度までに切り詰めた DateTime に変換します。
			/// @return 変換された DateTime
			[[nodiscard]]
			DateTime toDateTimeTruncatedToMilliseconds() const noexcept;

			/// @brief toml11 の local time から LocalTime を作成します。
			/// @param localTime toml11 の local time
			/// @return 作成した LocalTime
			[[nodiscard]]
			static LocalTime FromTOML(const toml::local_time& localTime) noexcept;
		};

		/// @brief TOML の local date-time
		struct LocalDateTime
		{
			/// @brief 日付
			Date date;

			/// @brief 時刻
			LocalTime time;

			/// @brief DateTime に変換します。
			/// @return 変換された DateTime
			/// @remark DateTime が保持できないマイクロ秒およびナノ秒の情報は失われます。
			[[nodiscard]]
			DateTime toDateTime() const noexcept;

			/// @brief ミリ秒精度までに切り詰めた DateTime に変換します。
			/// @return 変換された DateTime
			[[nodiscard]]
			DateTime toDateTimeTruncatedToMilliseconds() const noexcept;

			/// @brief toml11 の local date-time から LocalDateTime を作成します。
			/// @param localDateTime toml11 の local date-time
			/// @return 作成した LocalDateTime
			[[nodiscard]]
			static LocalDateTime FromTOML(const toml::local_datetime& localDateTime) noexcept;
		};

		/// @brief TOML の offset date-time
		struct OffsetDateTime
		{
			/// @brief ローカル日時
			LocalDateTime localDateTime;

			/// @brief UTC からのオフセット（分）
			int32 offsetMinutes = 0;

			/// @brief toml11 の offset date-time から OffsetDateTime を作成します。
			/// @param offsetDateTime toml11 の offset date-time
			/// @return 作成した OffsetDateTime
			[[nodiscard]]
			static OffsetDateTime FromTOML(const toml::offset_datetime& offsetDateTime) noexcept;
		};

		/// @brief デフォルトコンストラクタ。無効な TOML 値を作成します。
		[[nodiscard]]
		TOML() = default;

		/// @brief TOML 値が有効であるかを返します。
		/// @return TOML 値が有効である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isValid() const noexcept;
		
		/// @brief TOML 値が有効であるかを返します。
		/// @return TOML 値が有効である場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;
		
		/// @brief TOML 値のパスを返します。
		/// @return TOML 値のパス
		[[nodiscard]]
		const String& path() const noexcept;

		/// @brief TOML 値の型を返します。
		/// @return TOML 値の型
		[[nodiscard]]
		TOMLValueType type() const noexcept;

		/// @brief TOML 値の型を返します。
		/// @return TOML 値の型
		[[nodiscard]]
		TOMLValueType getType() const noexcept;
		
		/// @brief TOML 値が値を持っているかを返します。
		/// @return TOML 値が値を持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasValue() const noexcept;
		
		/// @brief TOML 値が無効であるかを返します。
		/// @return TOML 値が無効である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isInvalid() const noexcept;
		
		/// @brief TOML 値が空であるかを返します。
		/// @return TOML 値が空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		/// @brief TOML 値が bool 型であるかを返します。
		/// @return TOML 値が bool 型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isBool() const noexcept;
		
		/// @brief TOML 値が整数型であるかを返します。
		/// @return TOML 値が整数型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isInt() const noexcept;
		
		/// @brief TOML 値が浮動小数点数型であるかを返します。
		/// @return TOML 値が浮動小数点数型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isFloat() const noexcept;
		
		/// @brief TOML 値が文字列型であるかを返します。
		/// @return TOML 値が文字列型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isString() const noexcept;
		
		/// @brief TOML 値が local date 型であるかを返します。
		/// @return TOML 値が local date 型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isLocalDate() const noexcept;
		
		/// @brief TOML 値が local time 型であるかを返します。
		/// @return TOML 値が local time 型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isLocalTime() const noexcept;
		
		/// @brief TOML 値が local date-time 型であるかを返します。
		/// @return TOML 値が local date-time 型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isLocalDateTime() const noexcept;
		
		/// @brief TOML 値が offset date-time 型であるかを返します。
		/// @return TOML 値が offset date-time 型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isOffsetDateTime() const noexcept;
		
		/// @brief TOML 値が配列型であるかを返します。
		/// @return TOML 値が配列型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isArray() const noexcept;
		
		/// @brief TOML 値がテーブル型であるかを返します。
		/// @return TOML 値がテーブル型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isTable() const noexcept;
		
		/// @brief TOML 値が指定した型であるかを返します。
		/// @param valueType 調べる型
		/// @return TOML 値が指定した型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool is(const TOMLValueType valueType) const noexcept;

		/// @brief テーブルが指定したキーを持っているかを返します。
		/// @param key キー（UTF-8）
		/// @return テーブルが指定したキーを持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool contains(const std::string& key) const;

		/// @brief テーブルが指定したキーを持っているかを返します。
		/// @param key キー
		/// @return テーブルが指定したキーを持っている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool contains(const StringView key) const;

		/// @brief 配列またはテーブルの要素数を返します。
		/// @return 配列またはテーブルの要素数。配列でもテーブルでもない場合は 0
		[[nodiscard]]
		size_t size() const noexcept;

		/// @brief TOML 値が空の配列であるかを返します。
		/// @return TOML 値が空の配列である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmptyArray() const noexcept;

		/// @brief TOML 値が空のテーブルであるかを返します。
		/// @return TOML 値が空のテーブルである場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmptyTable() const noexcept;

		/// @brief テーブルから指定したキーの値を取得します。
		/// @param key キー（UTF-8）
		/// @return 指定したキーの TOML 値。存在しない場合は無効な TOML 値
		[[nodiscard]]
		TOML operator [](const std::string& key) const;

		/// @brief テーブルから指定したキーの値を取得します。
		/// @param key キー
		/// @return 指定したキーの TOML 値。存在しない場合は無効な TOML 値
		[[nodiscard]]
		TOML operator [](const StringView key) const;

		/// @brief 配列から指定したインデックスの値を取得します。
		/// @param index インデックス
		/// @return 指定したインデックスの TOML 値。範囲外の場合は無効な TOML 値
		[[nodiscard]]
		TOML operator [](const size_t index) const;

		/// @brief テーブルから指定したキーの値を取得します。
		/// @param key キー（UTF-8）
		/// @return 指定したキーの TOML 値
		/// @throw Error テーブルでない場合、または指定したキーが存在しない場合
		[[nodiscard]]
		TOML at(const std::string& key) const;

		/// @brief テーブルから指定したキーの値を取得します。
		/// @param key キー
		/// @return 指定したキーの TOML 値
		/// @throw Error テーブルでない場合、または指定したキーが存在しない場合
		[[nodiscard]]
		TOML at(const StringView key) const;

		/// @brief 配列から指定したインデックスの値を取得します。
		/// @param index インデックス
		/// @return 指定したインデックスの TOML 値
		/// @throw Error 配列でない場合、またはインデックスが範囲外の場合
		[[nodiscard]]
		TOML at(const size_t index) const;

		/// @brief TOML 値を String として取得します。
		/// @return TOML 値の文字列
		/// @throw Error TOML 値を String として取得できない場合
		[[nodiscard]]
		String getString() const;

		/// @brief TOML 値を UTF-8 文字列として取得します。
		/// @return TOML 値の UTF-8 文字列
		/// @throw Error TOML 値を UTF-8 文字列として取得できない場合
		[[nodiscard]]
		std::string getUTF8() const;

		/// @brief TOML 値を LocalTime として取得します。
		/// @return TOML 値の LocalTime
		/// @throw Error TOML 値を LocalTime として取得できない場合
		[[nodiscard]]
		LocalTime getLocalTime() const;

		/// @brief TOML 値を Date として取得します。
		/// @return TOML 値の Date
		/// @throw Error TOML 値を Date として取得できない場合
		[[nodiscard]]
		Date getLocalDate() const;

		/// @brief TOML 値を LocalDateTime として取得します。
		/// @return TOML 値の LocalDateTime
		/// @throw Error TOML 値を LocalDateTime として取得できない場合
		[[nodiscard]]
		LocalDateTime getLocalDateTime() const;

		/// @brief TOML 値を OffsetDateTime として取得します。
		/// @return TOML 値の OffsetDateTime
		/// @throw Error TOML 値を OffsetDateTime として取得できない場合
		[[nodiscard]]
		OffsetDateTime getOffsetDateTime() const;

		/// @brief TOML 値を指定した型として取得します。
		/// @tparam Type 取得する型
		/// @return 取得に成功した場合は値, それ以外の場合は none
		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt() const;

		/// @brief TOML 値を指定した型として取得します。
		/// @tparam Type 取得する型
		/// @return 取得した値
		/// @throw Error TOML 値を Type 型として取得できない場合
		template <class Type>
		[[nodiscard]]
		Type get() const;

		/// @brief TOML 値を指定した型として取得します。取得できない場合はデフォルト値を返します。
		/// @tparam Type 取得する型
		/// @tparam U デフォルト値の型
		/// @param defaultValue デフォルト値
		/// @return 取得に成功した場合は値, それ以外の場合はデフォルト値
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(U&& defaultValue) const;

		/// @brief TOML の文字列値を指定した型にパースして取得します。
		/// @tparam Type 取得する型
		/// @return パースに成功した場合は値, それ以外の場合は none
		template <class Type>
		[[nodiscard]]
		Optional<Type> getParsedOpt() const;

		/// @brief TOML の文字列値を指定した型にパースして取得します。
		/// @tparam Type 取得する型
		/// @return パースした値
		/// @throw Error TOML 値が文字列でない場合、またはパースに失敗した場合
		template <class Type>
		[[nodiscard]]
		Type getParsed() const;

		/// @brief TOML の文字列値を指定した型にパースして取得します。取得できない場合はデフォルト値を返します。
		/// @tparam Type 取得する型
		/// @tparam U デフォルト値の型
		/// @param defaultValue デフォルト値
		/// @return パースに成功した場合は値, それ以外の場合はデフォルト値
		template <class Type, class U>
		[[nodiscard]]
		Type getParsedOr(U&& defaultValue) const;

		/// @brief TOML 配列を指定した型の配列として取得します。
		/// @tparam Type 要素の型
		/// @return 取得に成功した場合は配列, それ以外の場合は none
		template <class Type>
		[[nodiscard]]
		Optional<Array<Type>> getArrayOpt() const;

		/// @brief TOML 配列を指定した型の配列として取得します。
		/// @tparam Type 要素の型
		/// @return 取得した配列
		/// @throw Error TOML 値が配列でない場合、または要素を Type 型として取得できない場合
		template <class Type>
		[[nodiscard]]
		Array<Type> getArray() const;

		/// @brief TOML 配列のビューを返します。
		/// @return TOML 配列のビュー
		/// @throw Error TOML 値が配列でない場合
		[[nodiscard]]
		ArrayView arrayView() const;

		/// @brief TOML テーブルのビューを返します。
		/// @return TOML テーブルのビュー
		/// @throw Error TOML 値がテーブルでない場合
		[[nodiscard]]
		TableView tableView() const;

		/// @brief TOML 値を TOML 形式の文字列に変換します。
		/// @return TOML 形式の文字列。無効な TOML 値の場合は空文字列
		[[nodiscard]]
		String format() const;

		/// @brief TOML 値を TOML 形式の UTF-8 文字列に変換します。
		/// @return TOML 形式の UTF-8 文字列。無効な TOML 値の場合は空文字列
		[[nodiscard]]
		std::string formatUTF8() const;

		/// @brief 2 つの TOML 値が等しいかを返します。
		/// @param lhs 一方の TOML 値
		/// @param rhs もう一方の TOML 値
		/// @return 2 つの TOML 値が等しい場合 true, それ以外の場合は false
		friend bool operator ==(const TOML& lhs, const TOML& rhs);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief TOML 文字列をパースします。
		/// @param content TOML 文字列（UTF-8）
		/// @param pathHint 入力名のヒント
		/// @return パースに成功した場合は TOML, それ以外の場合は無効な TOML 値
		[[nodiscard]]
		static TOML Parse(std::string content, const FilePathView pathHint = U"");

		/// @brief TOML 文字列をパースします。
		/// @param content TOML 文字列
		/// @param pathHint 入力名のヒント
		/// @return パースに成功した場合は TOML, それ以外の場合は無効な TOML 値
		[[nodiscard]]
		static TOML Parse(const StringView content, const FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	ParseResult
		//
		////////////////////////////////////////////////////////////////

		/// @brief TOML 文字列をパースし、結果またはエラー理由を返します。
		/// @param content TOML 文字列（UTF-8）
		/// @param pathHint 入力名のヒント
		/// @return パースに成功した場合は TOML, それ以外の場合はエラー理由の配列
		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> ParseResult(std::string content, const FilePathView pathHint = U"");

		/// @brief TOML 文字列をパースし、結果またはエラー理由を返します。
		/// @param content TOML 文字列
		/// @param pathHint 入力名のヒント
		/// @return パースに成功した場合は TOML, それ以外の場合はエラー理由の配列
		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> ParseResult(const StringView content, const FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		/// @brief TOML ファイルをロードします。
		/// @param path ファイルパス
		/// @return ロードとパースに成功した場合は TOML, それ以外の場合は無効な TOML 値
		[[nodiscard]]
		static TOML Load(const FilePathView path);

		/// @brief Reader から TOML をロードします。
		/// @param reader Reader
		/// @return ロードとパースに成功した場合は TOML, それ以外の場合は無効な TOML 値
		[[nodiscard]]
		static TOML Load(std::unique_ptr<IReader> reader);

		/// @brief Reader から TOML をロードします。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader
		/// @return ロードとパースに成功した場合は TOML, それ以外の場合は無効な TOML 値
		template <ReaderObject Reader>
		[[nodiscard]]
		static TOML Load(Reader&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	LoadResult
		//
		////////////////////////////////////////////////////////////////

		/// @brief TOML ファイルをロードし、結果またはエラー理由を返します。
		/// @param path ファイルパス
		/// @return ロードとパースに成功した場合は TOML, それ以外の場合はエラー理由の配列
		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> LoadResult(const FilePathView path);

		/// @brief Reader から TOML をロードし、結果またはエラー理由を返します。
		/// @param reader Reader
		/// @return ロードとパースに成功した場合は TOML, それ以外の場合はエラー理由の配列
		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> LoadResult(std::unique_ptr<IReader> reader);

		/// @brief Reader から TOML をロードし、結果またはエラー理由を返します。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader
		/// @return ロードとパースに成功した場合は TOML, それ以外の場合はエラー理由の配列
		template <ReaderObject Reader>
		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> LoadResult(Reader&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無効な TOML 値を返します。
		/// @return 無効な TOML 値
		[[nodiscard]]
		static TOML Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief TOML 値を文字列にフォーマットします。
		/// @param formatData 文字列バッファ
		/// @param value TOML 値
		friend void Formatter(FormatData& formatData, const TOML& value);

	private:
		
		template <class>
		static constexpr bool AlwaysFalse = false;

		std::shared_ptr<const Value> m_root;
		
		const Value* m_value = nullptr;
		
		String m_path = U"$";

		[[nodiscard]]
		TOML(std::shared_ptr<const Value> root, const Value* value, String path);

		[[nodiscard]]
		static TOML FromRoot(std::shared_ptr<const Value> root);

		[[nodiscard]]
		TOML InvalidWithPath(String path) const;

		[[nodiscard]]
		static Date ToDate(const toml::local_date& date) noexcept;

		[[nodiscard]]
		static String MakeIndexPath(const StringView parent, const size_t index);

		[[noreturn]]
		static void ThrowTOMLGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value);
	
		[[noreturn]]
		static void ThrowTOMLParsedGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value);
		
		[[noreturn]]
		static void ThrowNotArray(const char* type, const StringView path, const TOMLValueType actualType);

		[[noreturn]]
		static void ThrowArrayElementGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value);
	};

	struct TOML::TableMember
	{
		String name;
		
		TOML value;
	};

	class TOML::ArrayView
	{
	public:
		
		class Iterator
		{
		public:
			
			using difference_type = std::ptrdiff_t;
			using value_type = TOML;
			using iterator_category = std::forward_iterator_tag;

			[[nodiscard]]
			Iterator() = default;

			[[nodiscard]]
			Iterator(std::shared_ptr<const Value> root, const Value::array_type* array, const size_t index, String path);

			Iterator& operator ++();
			
			Iterator operator ++(int);

			[[nodiscard]]
			TOML operator *() const;

			friend bool operator ==(const Iterator& lhs, const Iterator& rhs) noexcept;

		private:
			
			std::shared_ptr<const Value> m_root;
			
			const Value::array_type* m_array = nullptr;
			
			size_t m_index = 0;
			
			String m_path;
		};

		[[nodiscard]]
		ArrayView() = default;

		[[nodiscard]]
		ArrayView(std::shared_ptr<const Value> root, const Value::array_type* array, String path);

		[[nodiscard]]
		Iterator begin() const;

		[[nodiscard]]
		Iterator end() const;

		[[nodiscard]]
		size_t size() const noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

	private:
		
		std::shared_ptr<const Value> m_root;
		
		const Value::array_type* m_array = nullptr;
		
		String m_path;
	};

	class TOML::TableView
	{
	public:

		class Iterator
		{
		public:
		
			using difference_type = std::ptrdiff_t;
			using value_type = TableMember;
			using iterator_category = std::forward_iterator_tag;
			using TableIterator = Value::table_type::const_iterator;

			[[nodiscard]]
			Iterator() = default;

			[[nodiscard]]
			Iterator(std::shared_ptr<const Value> root, TableIterator it, String path);

			Iterator& operator ++();

			Iterator operator ++(int);

			[[nodiscard]] TableMember operator *() const;

			friend bool operator ==(const Iterator& lhs, const Iterator& rhs) noexcept;

		private:
			
			std::shared_ptr<const Value> m_root;
			
			TableIterator m_it{};
			
			String m_path;
		};

		[[nodiscard]]
		TableView() = default;

		[[nodiscard]]
		TableView(std::shared_ptr<const Value> root, const Value::table_type* table, String path);

		[[nodiscard]]
		Iterator begin() const;
		
		[[nodiscard]]
		Iterator end() const;
		
		[[nodiscard]]
		size_t size() const noexcept;
		
		[[nodiscard]]
		bool isEmpty() const noexcept;

	private:
		
		std::shared_ptr<const Value> m_root;
		
		const Value::table_type* m_table = nullptr;
		
		String m_path;
	};
}

# include "detail/TOML.ipp"
