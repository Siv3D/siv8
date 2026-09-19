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
# include <variant>
# include "Common.hpp"
# include "Array.hpp"
# include "String.hpp"
# include "Blob.hpp"
# include "Error.hpp"
# include "Optional.hpp"
# include "IntToString.hpp"
# include "JSONValueType.hpp"
# include "PredefinedYesNo.hpp"
// Keep the underlying library's literals out of the global namespace.
# ifndef JSON_USE_GLOBAL_UDLS
# define JSON_USE_GLOBAL_UDLS 0
# endif
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(26819)
# include <ThirdParty/nlohmann/json.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	class JSON;
	class JSONIterator;
	class JSONConstIterator;
	class JSONPointer;

	////////////////////////////////////////////////////////////////
	//
	//	JSON
	//
	////////////////////////////////////////////////////////////////

	/// @brief 挿入順を保持する JSON データ。
	/// @remark `operator []` とイテレータから得る子要素は親の値への参照です。親の破棄、代入、要素の追加・削除で無効になることがあります。
	/// @remark 参照をコピーしても所有権は得られません。独立した値が必要な場合は `clone()` を使用してください。
	/// @remark const の親から得た参照を非 const の変数にコピーすると、非 const の要素アクセスや変更時に参照先を複製して独立した値になります。
	class JSON
	{
	public:

		using json_base			= nlohmann::ordered_json;

		using iterator			= JSONIterator;

		using const_iterator	= JSONConstIterator;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		JSON(std::nullptr_t = nullptr);

		[[nodiscard]]
		explicit JSON(JSONValueType valueType);

		/// @brief 値から JSON を作成します。UTF-32 の文字列・文字列ビュー・文字列ポインタは JSON 文字列になります。
		/// @param value 値
		/// @pre 文字列は型に対応した有効な UTF-8 または UTF-32 である必要があります。
		template <class Type>
			requires std::constructible_from<JSON::json_base, Type>
		[[nodiscard]]
		JSON(Type&& value);
		
		/// @brief 初期化子リストから JSON を作成します。`JSON{ 42 }` は `[42]`、`JSON(42)` は `42` です。
		/// @param init 要素
		/// @param typeDeduction キーと値のペアのリストをオブジェクトとして推定するか
		/// @param manualType typeDeduction が false の場合の型（Array または Object）
		[[nodiscard]]
		JSON(JSON::json_base::initializer_list_t init, bool typeDeduction = true, JSONValueType manualType = JSONValueType::Array);

		/// @brief イテレータ範囲の値から JSON 配列を作成します。
		/// @param first 範囲の先頭
		/// @param last 範囲の終端
		/// @pre 各要素は json_base に変換できる必要があります。
		template <std::input_iterator Iterator>
		[[nodiscard]]
		JSON(Iterator first, Iterator last);

		[[nodiscard]]
		explicit JSON(std::reference_wrapper<json_base> json);

		[[nodiscard]]
		explicit JSON(std::reference_wrapper<const json_base> json);
		
		[[nodiscard]]
		JSON(const JSON& other) = default;

		[[nodiscard]]
		JSON(JSON&& other) = default;

		////////////////////////////////////////////////////////////////
		//
		//	load
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON ファイルを読み込みます。
		/// @param path ファイルパス
		/// @param allowExceptions ロードに失敗した場合に例外を発生させるか
		/// @return ロードに成功した場合 true, それ以外の場合は false
		/// @remark 失敗時は値が Invalid になります。
		/// @throw Error allowExceptions が Yes で入力を読み込めない、またはパースできない場合
		bool load(FilePathView path, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON ファイルを読み込みます。
		/// @param reader Reader オブジェクト
		/// @param allowExceptions ロードに失敗した場合に例外を発生させるか
		/// @return ロードに成功した場合 true, それ以外の場合は false
		/// @remark 失敗時は値が Invalid になります。
		/// @throw Error allowExceptions が Yes で入力を読み込めない、またはパースできない場合
		bool load(std::unique_ptr<IReader> reader, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON ファイルを読み込みます。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader オブジェクト
		/// @param allowExceptions ロードに失敗した場合に例外を発生させるか
		/// @return ロードに成功した場合 true, それ以外の場合は false
		/// @remark 失敗時は値が Invalid になります。
		/// @throw Error allowExceptions が Yes で入力を読み込めない、またはパースできない場合
		template <ReaderObject Reader>
		bool load(Reader&& reader, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON 文字列をパースします。
		/// @param s 文字列
		/// @param allowExceptions 例外を発生させるか
		/// @return パースに成功した場合 true, それ以外の場合は false
		/// @remark 失敗時は値が Invalid になります。受理する構文は `Parse()` と同じです。
		/// @throw Error allowExceptions が Yes でパースできない場合
		bool parse(std::string_view s, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON 文字列をパースします。
		/// @param s 文字列
		/// @param allowExceptions 例外を発生させるか
		/// @return パースに成功した場合 true, それ以外の場合は false
		/// @remark 失敗時は値が Invalid になります。受理する構文は `Parse()` と同じです。
		/// @throw Error allowExceptions が Yes でパースできない場合
		bool parse(StringView s, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	operator = 
		//
		////////////////////////////////////////////////////////////////

		JSON& operator =(const char32* value);

		JSON& operator =(std::u32string_view value);

		JSON& operator =(StringView value);

		JSON& operator =(const JSON& value);

		JSON& operator =(JSON&& value);

		JSON& operator =(JSON& value);

		JSON& operator =(auto&& value);

		////////////////////////////////////////////////////////////////
		//
		//	clone
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON オブジェクトを複製します。
		/// @return 複製された JSON オブジェクト
		/// @remark 関数内で作成した JSON から子オブジェクトを返す場合、`clone()` を使用してください。
		[[nodiscard]]
		JSON clone() const;

		////////////////////////////////////////////////////////////////
		//
		//	getType
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値の型を取得します。
		/// @return 値の型
		[[nodiscard]]
		JSONValueType getType() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNull
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値の型が null であるかを返します。 
		/// @return 値の型が null である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isNull() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isBool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値の型が bool であるかを返します。 
		/// @return 値の型が bool である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isBool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNumber
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が数値型（整数型、符号なし整数型、浮動小数点数型）であるかを返します。 
		/// @return 値が数値型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isNumber() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が整数型（整数型、符号なし整数型）であるかを返します。 
		/// @return 値が整数型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isInt() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isUint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が符号なし整数型であるかを返します。 
		/// @return 値が符号なし整数型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isUint() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が浮動小数点数型であるかを返します。 
		/// @return 値が浮動小数点数型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isFloat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が文字列型であるかを返します。 
		/// @return 値が文字列型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isString() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isPrimitive
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値がプリミティブ型（null, bool, 数値型, 文字列型、バイナリデータ型）であるかを返します。 
		/// @return 値がプリミティブ型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isPrimitive() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が配列型であるかを返します。 
		/// @return 値が配列型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isArray() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isObject
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値がオブジェクト型であるかを返します。
		/// @return 値がオブジェクト型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isObject() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isBinary
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値がバイナリデータ型であるかを返します。 
		/// @return 値がバイナリデータ型である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isBinary() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInvalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が不正であるかを返します。
		/// @return 値が不正である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isInvalid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が null でも Invalid でもないかを返します。
		/// @return 値が null でも Invalid でもない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasElement
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したキーの要素が存在するかを返します。
		/// @param key キー
		/// @return 要素が存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasElement(std::string_view key) const;

		/// @brief 指定したキーの要素が存在するかを返します。
		/// @param key キー
		/// @return 要素が存在する場合 true, それ以外の場合は false
		[[nodiscard]]
		bool hasElement(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	contains
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したキーの要素が存在するかを返します。
		/// @param key キー
		/// @return 要素が存在する場合 true, それ以外の場合は false
		/// @remark `hasElement` と同じです。
		[[nodiscard]]
		bool contains(std::string_view key) const;

		/// @brief 指定したキーの要素が存在するかを返します。
		/// @param key キー
		/// @return 要素が存在する場合 true, それ以外の場合は false
		/// @remark `hasElement` と同じです。
		[[nodiscard]]
		bool contains(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数またはオブジェクトのキーの数を返します。
		/// @return 配列の要素数またはオブジェクトのキーの数。null は 0、その他の値は 1
		[[nodiscard]]
		size_t size() const noexcept;
		
		////////////////////////////////////////////////////////////////
		//
		//	getString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が文字列の場合、String として返します。
		/// @return 値
		/// @throw Error JSON の値が文字列でない場合
		[[nodiscard]]
		String getString() const;

		////////////////////////////////////////////////////////////////
		//
		//	getUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が文字列の場合、UTF-8 文字列として返します。
		/// @return 値
		/// @throw Error JSON の値が文字列でない場合
		[[nodiscard]]
		std::string getUTF8() const;

		////////////////////////////////////////////////////////////////
		//
		//	getBinary
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値がバイナリデータの場合、そのバイナリデータを返します。
		/// @return バイナリデータ
		/// @throw Error JSON の値がバイナリデータでない場合
		[[nodiscard]]
		Blob getBinary() const;

		////////////////////////////////////////////////////////////////
		//
		//	get
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。
		/// @tparam Type 値の型
		/// @return 値
		/// @throw Error JSON の値を Type 型に変換できない場合
		/// @remark 数値の範囲は検査しません。整数への変換は小数部分を切り捨てます。
		/// @pre 浮動小数点数から整数への変換では、切り捨て後の値が変換先の範囲内である必要があります。
		template <class Type>
		[[nodiscard]]
		Type get() const;

		////////////////////////////////////////////////////////////////
		//
		//	getOr
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。取得できなかった場合は defaultValue を返します。
		/// @tparam Type 値の型
		/// @tparam U 取得できなかった場合に返す値の型
		/// @param defaultValue 取得できなかった場合に返す値（変数または一時値）
		/// @return 値
		/// @remark 数値変換の規則と事前条件は `get()` と同じです。範囲検査には使用できません。
		template <class Type, class U>
		[[nodiscard]]
		Type getOr(U&& defaultValue) const;

		////////////////////////////////////////////////////////////////
		//
		//	getOpt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を Type 型として取得します。
		/// @tparam Type 値の型
		/// @return 値。取得できなかった場合は none
		/// @remark 数値変換の規則と事前条件は `get()` と同じです。範囲検査には使用できません。
		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator []
		//
		////////////////////////////////////////////////////////////////

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 親を参照する要素
		/// @remark キーがなければ null の要素を追加します。値が null の場合はオブジェクトになります。
		[[nodiscard]]
		JSON operator [](std::string_view key);

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 親を参照する要素
		/// @throw Error オブジェクトでない場合、またはキーがない場合
		[[nodiscard]]
		const JSON operator [](std::string_view key) const;

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 親を参照する要素
		/// @remark キーがなければ null の要素を追加します。値が null の場合はオブジェクトになります。
		[[nodiscard]]
		JSON operator [](StringView key);

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 親を参照する要素
		/// @throw Error オブジェクトでない場合、またはキーがない場合
		[[nodiscard]]
		const JSON operator [](StringView key) const;

		/// @brief 配列の指定したインデックスの要素を返します。
		/// @param index インデックス
		/// @return 親を参照する要素
		/// @throw Error 配列でない場合、またはインデックスが範囲外の場合
		[[nodiscard]]
		JSON operator [](size_t index);

		/// @brief 配列の指定したインデックスの要素を返します。
		/// @param index インデックス
		/// @return 親を参照する要素
		/// @throw Error 配列でない場合、またはインデックスが範囲外の場合
		[[nodiscard]]
		const JSON operator [](size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する要素
		/// @remark 値が null の場合は配列になります。
		void push_back(const JSON& value);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾の要素を削除します。
		/// @throw Error 配列でない場合、または配列が空の場合
		void pop_back();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値の型はそのままで、値を空にします。
		/// @remark 配列の場合は要素をすべて削除します。数値型の場合は 0 に、bool 型の場合は false になります。
		void clear();

		////////////////////////////////////////////////////////////////
		//
		//	erase
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したキーの要素を削除します。
		/// @param key キー
		void erase(std::string_view key);

		/// @brief 指定したキーの要素を削除します。
		/// @param key キー
		void erase(StringView key);

		/// @brief 指定したインデックスの要素を削除します。
		/// @param index インデックス
		/// @throw Error JSON の値が配列でない場合、またはインデックスが範囲外の場合
		void erase(size_t index);

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief オブジェクトまたは配列の要素の先頭を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の先頭を指すイテレータ
		[[nodiscard]]
		iterator begin();

		/// @brief オブジェクトまたは配列の要素の終端位置を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の終端位置を指すイテレータ
		[[nodiscard]]
		iterator end();

		/// @brief オブジェクトまたは配列の要素の先頭を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の先頭を指すイテレータ
		[[nodiscard]]
		const_iterator begin() const noexcept;

		/// @brief オブジェクトまたは配列の要素の終端位置を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の終端位置を指すイテレータ
		[[nodiscard]]
		const_iterator end() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	cbegin, cend
		//
		////////////////////////////////////////////////////////////////

		/// @brief オブジェクトまたは配列の要素の先頭を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の先頭を指すイテレータ
		[[nodiscard]]
		const_iterator cbegin() const noexcept;

		/// @brief オブジェクトまたは配列の要素の終端位置を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の終端位置を指すイテレータ
		[[nodiscard]]
		const_iterator cend() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	flatten
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON ポインタとプリミティブ型の値のペアからなる JSON オブジェクトに変換した結果を返します。
		/// @return JSON オブジェクト
		/// @remark 空の配列とオブジェクトは null になり、`unflatten()` では復元できません。
		[[nodiscard]]
		JSON flatten() const;

		////////////////////////////////////////////////////////////////
		//
		//	unflatten
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON ポインタと値のオブジェクトから階層を復元します。
		/// @remark 空のコンテナは null になり、数字のキーは配列のインデックスと解釈されることがあります。
		/// @return JSON オブジェクト
		[[nodiscard]]
		JSON unflatten() const;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを文字列にフォーマットした結果を返します。
		/// @param indent インデントの文字（空白またはタブ）
		/// @param spaceCount インデントの文字数（INT_MAX 以下）
		/// @param ensureAscii ASCII 文字のみにエンコードするか
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		String format(char32 indent = U' ', size_t spaceCount = 2, EnsureAscii ensureAscii = EnsureAscii::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを UTF-8 文字列にフォーマットした結果を返します。
		/// @param indent インデントの文字（空白またはタブ）
		/// @param spaceCount インデントの文字数（INT_MAX 以下）
		/// @param ensureAscii ASCII 文字のみにエンコードするか
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		std::string formatUTF8(char32 indent = U' ', size_t spaceCount = 2, EnsureAscii ensureAscii = EnsureAscii::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatMinified
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の文字列にフォーマットした結果を返します。
		/// @param ensureAscii ASCII 文字のみにエンコードするか
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		String formatMinified(EnsureAscii ensureAscii = EnsureAscii::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8Minified
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の UTF-8 文字列にフォーマットした結果を返します。
		/// @param ensureAscii ASCII 文字のみにエンコードするか
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		std::string formatUTF8Minified(EnsureAscii ensureAscii = EnsureAscii::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データをファイルに保存します。
		/// @param indent インデントの文字（空白またはタブ）
		/// @param spaceCount インデントの文字数（INT_MAX 以下）
		/// @param ensureAscii ASCII 文字のみにエンコードするか
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(FilePathView path, char32 indent = U' ', size_t spaceCount = 2, EnsureAscii ensureAscii = EnsureAscii::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	saveMinified
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の形式でファイルに保存します。
		/// @param path 保存するファイルのパス
		/// @param ensureAscii ASCII 文字のみにエンコードするか
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool saveMinified(FilePathView path, EnsureAscii ensureAscii = EnsureAscii::No) const;

		////////////////////////////////////////////////////////////////
		//
		//	base
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const json_base& base() const;

		////////////////////////////////////////////////////////////////
		//
		//	toBSON
		//
		////////////////////////////////////////////////////////////////

		/// @brief BSON 形式にシリアライズした結果を返します。
		/// @return BSON データ
		/// @pre ルートはオブジェクトである必要があります。
		/// @remark INT64_MAX を超える符号なし整数は BSON Timestamp（型コード 0x11）として表現されます。
		[[nodiscard]]
		Blob toBSON() const;

		////////////////////////////////////////////////////////////////
		//
		//	toCBOR
		//
		////////////////////////////////////////////////////////////////

		/// @brief CBOR 形式にシリアライズした結果を返します。
		/// @return CBOR データ
		[[nodiscard]]
		Blob toCBOR() const;

		////////////////////////////////////////////////////////////////
		//
		//	toMessagePack
		//
		////////////////////////////////////////////////////////////////

		/// @brief MessagePack 形式にシリアライズした結果を返します。
		/// @return MessagePack データ
		[[nodiscard]]
		Blob toMessagePack() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON オブジェクトを交換します。
		/// @param other 交換する JSON オブジェクト
		/// @remark 所有値・参照の状態を交換します。子要素を参照する 2 つの変数を交換しても、親の要素は書き換えません。
		void swap(JSON& other) noexcept;

		/// @brief 2 つの JSON オブジェクトを交換します。
		/// @param lhs JSON オブジェクト
		/// @param rhs JSON オブジェクト
		friend void swap(JSON& lhs, JSON& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を比較します。オブジェクトではキーの挿入順も比較に含まれます。
		friend bool operator ==(const JSON& lhs, const JSON& rhs) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON ファイルをロードして JSON オブジェクトを返します。
		/// @param path ファイルパス
		/// @param allowExceptions ロードに失敗した場合に例外を発生させるか
		/// @return 読み込んだ値。入力を読み込めない、またはパースできない場合は Invalid
		/// @remark UTF-8（BOM は任意）と BOM 付き UTF-16LE / UTF-16BE を読み込みます。
		/// @throw Error allowExceptions が Yes で入力を読み込めない、またはパースできない場合
		[[nodiscard]]
		static JSON Load(FilePathView path, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON ファイルをロードして JSON オブジェクトを返します。
		/// @param reader Reader オブジェクト
		/// @param allowExceptions ロードに失敗した場合に例外を発生させるか
		/// @return 読み込んだ値。入力を読み込めない、またはパースできない場合は Invalid
		/// @remark UTF-8（BOM は任意）と BOM 付き UTF-16LE / UTF-16BE を読み込みます。
		/// @throw Error allowExceptions が Yes で入力を読み込めない、またはパースできない場合
		[[nodiscard]]
		static JSON Load(std::unique_ptr<IReader> reader, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON ファイルをロードして JSON オブジェクトを返します。
		/// @tparam Reader Reader オブジェクトの型
		/// @param reader Reader オブジェクト
		/// @param allowExceptions ロードに失敗した場合に例外を発生させるか
		/// @return 読み込んだ値。入力を読み込めない、またはパースできない場合は Invalid
		/// @remark UTF-8（BOM は任意）と BOM 付き UTF-16LE / UTF-16BE を読み込みます。
		/// @throw Error allowExceptions が Yes で入力を読み込めない、またはパースできない場合
		template <ReaderObject Reader>
		[[nodiscard]]
		static JSON Load(Reader&& reader, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON 文字列をパースして JSON オブジェクトを返します。
		/// @param s 文字列
		/// @param allowExceptions 例外を発生させるか
		/// @return パースした値。構文・文字コード・数値の表現範囲が不正な場合は Invalid
		/// @remark C / C++ 形式のコメントを許可します。重複キーは最後の値を採用します。
		/// @remark 整数は int64 / uint64、その他の数値は double で保持します。整数の範囲を超える値は double になり精度を失うことがあります。
		/// @throw Error allowExceptions が Yes でパースできない場合
		[[nodiscard]]
		static JSON Parse(std::string_view s, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON 文字列をパースして JSON オブジェクトを返します。
		/// @param s 文字列
		/// @param allowExceptions 例外を発生させるか
		/// @return パースした値。構文・文字コード・数値の表現範囲が不正な場合は Invalid
		/// @remark C / C++ 形式のコメントを許可します。重複キーは最後の値を採用します。
		/// @remark 整数は int64 / uint64、その他の数値は double で保持します。整数の範囲を超える値は double になり精度を失うことがあります。
		/// @throw Error allowExceptions が Yes でパースできない場合
		[[nodiscard]]
		static JSON Parse(StringView s, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	FromBSON
		//
		////////////////////////////////////////////////////////////////

		/// @brief BSON 形式のデータから JSON オブジェクトをデシリアライズします。
		/// @param bson BSON データ
		/// @param allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON FromBSON(const Blob& bson, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	FromCBOR
		//
		////////////////////////////////////////////////////////////////

		/// @brief CBOR 形式のデータから JSON オブジェクトをデシリアライズします。
		/// @param cbor CBOR データ
		/// @param allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON FromCBOR(const Blob& cbor, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	FromMessagePack
		//
		////////////////////////////////////////////////////////////////

		/// @brief MessagePack 形式のデータから JSON オブジェクトをデシリアライズします。
		/// @param msgpack MessagePack データ
		/// @param allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON FromMessagePack(const Blob& msgpack, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	EmptyArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空の配列を持つ JSON オブジェクトを作成します。
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON EmptyArray();

		////////////////////////////////////////////////////////////////
		//
		//	EmptyObject
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のオブジェクトを持つ JSON オブジェクトを作成します。
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON EmptyObject();

		////////////////////////////////////////////////////////////////
		//
		//	EmptyBinary
		//
		////////////////////////////////////////////////////////////////

		/// @brief 空のバイナリデータを持つ JSON オブジェクトを作成します。
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON EmptyBinary();

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 不正な JSON オブジェクトを作成します。
		/// @return 不正な JSON オブジェクト
		[[nodiscard]]
		static JSON Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON オブジェクトを文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value JSON オブジェクト
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const JSON& value);

	private:

		std::variant<json_base, std::reference_wrapper<json_base>, std::reference_wrapper<const json_base>> m_json;

		[[nodiscard]]
		json_base& getRef();

		[[nodiscard]]
		const json_base& getConstRef() const;
	};

	////////////////////////////////////////////////////////////////
	//
	//	ToJSON
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
		requires std::constructible_from<JSON::json_base, Type>
	[[nodiscard]]
	JSON ToJSON(Type&& value);

	////////////////////////////////////////////////////////////////
	//
	//	FromJSON
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	[[nodiscard]]
	Type FromJSON(const JSON& json);

	////////////////////////////////////////////////////////////////
	//
	//	FromJSONOr
	//
	////////////////////////////////////////////////////////////////

	/// @brief JSON から値を取得します。変換できない場合は defaultValue を返します。
	/// @param json JSON 値
	/// @param defaultValue 変換できない場合の既定値
	/// @return 取得した値または既定値
	/// @remark 数値変換の規則と事前条件は JSON::get() と同じです。
	template <class Type>
		requires (not std::is_reference_v<Type>)
	[[nodiscard]]
	Type FromJSONOr(const JSON& json, Type&& defaultValue);

	/// @copydoc FromJSONOr(const JSON&, Type&&)
	template <class Type>
	[[nodiscard]]
	Type FromJSONOr(const JSON& json, const Type& defaultValue);

	////////////////////////////////////////////////////////////////
	//
	//	FromJSONOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	[[nodiscard]]
	Optional<Type> FromJSONOpt(const JSON& json);

	inline namespace Literals
	{
		inline namespace JSONLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_json
			//
			////////////////////////////////////////////////////////////////

			/// @brief JSON 文字列リテラルをパースして JSON オブジェクトを作成します。
			/// @param str JSON 文字列リテラル
			/// @param length JSON 文字列リテラルの長さ
			/// @return JSON オブジェクト
			[[nodiscard]]
			JSON operator ""_json(const char* str, size_t length);

			/// @brief JSON 文字列リテラルをパースして JSON オブジェクトを作成します。
			/// @param str JSON 文字列リテラル
			/// @param length JSON 文字列リテラルの長さ
			/// @return JSON オブジェクト
			[[nodiscard]]
			JSON operator ""_json(const char32_t* str, size_t length);
		}
	}
}

////////////////////////////////////////////////////////////////
//
//	JSONSerializer
//
////////////////////////////////////////////////////////////////

# define JSONSerializer nlohmann::adl_serializer 

template <size_t N>
struct JSONSerializer<s3d::char32[N]>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::char32(&value)[N])
	{
		j = s3d::JSON::json_base(s3d::Unicode::ToUTF8(value));
	}
};

template <>
struct JSONSerializer<s3d::StringView>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::StringView& value);
};

template <>
struct JSONSerializer<s3d::String>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::String& value);

	static void from_json(const s3d::JSON::json_base& j, s3d::String& value);
};

template <>
struct JSONSerializer<std::u32string_view>
{
	static void to_json(s3d::JSON::json_base& j, std::u32string_view value);
};

template <>
struct JSONSerializer<std::u32string>
{
	static void to_json(s3d::JSON::json_base& j, const std::u32string& value);
	static void from_json(const s3d::JSON::json_base& j, std::u32string& value);
};

template <>
struct JSONSerializer<const s3d::char32*>
{
	static void to_json(s3d::JSON::json_base& j, const s3d::char32* value);
};

template <>
struct JSONSerializer<s3d::char32*> : JSONSerializer<const s3d::char32*> {};

# include "detail/JSON.ipp"
