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
# include <variant>
# include "Common.hpp"
# include "Array.hpp"
# include "String.hpp"
# include "Blob.hpp"
# include "Optional.hpp"
# include "IntToString.hpp"
# include "JSONValueType.hpp"
# include "PredefinedYesNo.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(26819)
# include <ThirdParty/nlohmann/json.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	class JSON;
	class JSONIterator;
	class JSONConstIterator;
	class JSONPointer;

	template <class Type>
	concept JSONCompatibleType =
		std::is_constructible_v<JSON, Type>
		&& (not std::is_same_v<std::decay_t<Type>, JSON>)
		&& (not std::is_same_v<std::decay_t<Type>, JSONValueType>);

	class JSON
	{
	public:

		using json_base			= nlohmann::json;

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

		[[nodiscard]]
		JSON(const char* value);

		[[nodiscard]]
		JSON(std::string_view value);

		[[nodiscard]]
		JSON(const char32* value);

		[[nodiscard]]
		JSON(StringView value);

		[[nodiscard]]
		JSON(const String& value);

		[[nodiscard]]
		JSON(bool value);

		[[nodiscard]]
		JSON(Concept::SignedIntegral auto value);

		[[nodiscard]]
		JSON(Concept::UnsignedIntegral auto value);

		[[nodiscard]]
		JSON(Concept::FloatingPoint auto value);

		template <class Iterator>
		[[nodiscard]]
		JSON(Iterator first, Iterator last);

		[[nodiscard]]
		JSON(const std::initializer_list<std::pair<std::string, JSON>>& list);

		[[nodiscard]]
		JSON(const std::initializer_list<std::pair<String, JSON>>& list);

		[[nodiscard]]
		JSON(const Array<JSON>& array);

		template <JSONCompatibleType Type>
		[[nodiscard]]
		JSON(const Array<Type>& arr);

		template <JSONCompatibleType Type>
		[[nodiscard]]
		JSON(const std::initializer_list<Type>& list);
	
		[[nodiscard]]
		explicit JSON(std::reference_wrapper<json_base> json);
		
		[[nodiscard]]
		explicit JSON(std::reference_wrapper<const json_base> json);

		[[nodiscard]]
		explicit JSON(json_base&& json);

		JSON(const JSON& other) = default;

		JSON(JSON&& other) = default;

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

		/// @brief 値が Null または不正でないかを返します。
		/// @return 値が Null または不正でない場合 true, それ以外の場合は false
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
		/// @ `hasElement` と同じです。
		[[nodiscard]]
		bool contains(StringView key) const;

		////////////////////////////////////////////////////////////////
		//
		//	size
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の要素数またはオブジェクトのキーの数を返します。
		/// @return 配列の要素数またはオブジェクトのキーの数
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
		/// @param defaultValue 取得できなかった場合に返す値
		/// @return 値
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
		/// @return 要素
		[[nodiscard]]
		JSON operator [](std::string_view key);

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 要素
		[[nodiscard]]
		const JSON operator [](std::string_view key) const;

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 要素
		[[nodiscard]]
		JSON operator [](StringView key);

		/// @brief オブジェクトの指定したキーの要素を返します。
		/// @param key キー
		/// @return 要素
		[[nodiscard]]
		const JSON operator [](StringView key) const;

		/// @brief 配列の指定したインデックスの要素を返します。
		/// @param index インデックス
		/// @return 要素
		[[nodiscard]]
		JSON operator [](size_t index);

		/// @brief 配列の指定したインデックスの要素を返します。
		/// @param index インデックス
		/// @return 要素
		[[nodiscard]]
		const JSON operator [](size_t index) const;

		////////////////////////////////////////////////////////////////
		//
		//	push_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾に要素を追加します。
		/// @param value 追加する要素
		void push_back(const JSON& value);

		////////////////////////////////////////////////////////////////
		//
		//	pop_back
		//
		////////////////////////////////////////////////////////////////

		/// @brief 配列の末尾の要素を削除します。
		void pop_back();

		////////////////////////////////////////////////////////////////
		//
		//	clear
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値の型はそのままで、値を空にします。
		/// @remark 配列の場合は要素を全て削除します。数値型の場合は 0 に、bool 型の場合は false になります。
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
		/// @throe Error JSON の値が配列でない場合、またはインデックスが範囲外の場合
		void erase(size_t index);

		////////////////////////////////////////////////////////////////
		//
		//	begin, end
		//
		////////////////////////////////////////////////////////////////

		/// @brief オブジェクトまたは配列の要素の先頭を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の先頭を指すイテレータ
		[[nodiscard]]
		iterator begin() noexcept;

		/// @brief オブジェクトまたは配列の要素の終端位置を指すイテレータを返します。
		/// @return オブジェクトまたは配列の要素の終端位置を指すイテレータ
		[[nodiscard]]
		iterator end() noexcept;

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

		/// @brief JSON ポインタとプリミティブ型の値のペアをからなる JSON オブジェクトに変換した結果を返します。
		/// @return JSON オブジェクト
		[[nodiscard]]
		JSON flatten() const;

		////////////////////////////////////////////////////////////////
		//
		//	unflatten
		//
		////////////////////////////////////////////////////////////////

		/// @brief `flatten` で変換された JSON オブジェクトを、元の JSON オブジェクトに復元した結果を返します。
		/// @return JSON オブジェクト
		[[nodiscard]]
		JSON unflatten() const;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを文字列にフォーマットした結果を返します。
		/// @param indent インデントの文字
		/// @param spaceCount インデントの文字数
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
		/// @param indent インデントの文字
		/// @param spaceCount インデントの文字数
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
		/// @param indent インデントの文字
		/// @param spaceCount インデントの文字数
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
		//	toBSON
		//
		////////////////////////////////////////////////////////////////

		/// @brief BSON 形式にシリアライズした結果を返します。
		/// @return BSON データ
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

		[[nodiscard]]
		friend bool operator ==(const JSON& lhs, const JSON& rhs) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON ファイルをパースして JSON オブジェクトを返します。
		/// @param path ファイルパス
		/// @param allowExceptions パースに失敗した場合に例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON Load(FilePathView path, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON ファイルをパースして JSON オブジェクトを返します。
		/// @tparam Reader JSON ファイルを指す IReader の派生クラスの型
		/// @param reader JSON ファイルを指す IReader の派生クラスのインスタンス
		/// @param allowExceptions パースに失敗した場合に例外を発生させるか
		/// @return JSON オブジェクト
		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		static JSON Load(Reader&& reader, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON ファイルをパースして JSON オブジェクトを返します。
		/// @param reader JSON ファイルを指す IReader のインスタンス
		/// @param allowExceptions パースに失敗した場合に例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON Load(std::unique_ptr<IReader>&& reader, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON 文字列をパースして JSON オブジェクトを返します。
		/// @param [in] s 文字列
		/// @param [in] allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON Parse(std::string_view s, AllowExceptions allowExceptions = AllowExceptions::No);

		/// @brief JSON 文字列をパースして JSON オブジェクトを返します。
		/// @param [in] s 文字列
		/// @param [in] allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON Parse(StringView s, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	FromBSON
		//
		////////////////////////////////////////////////////////////////

		/// @brief BSON 形式のデータから JSON オブジェクトをデシリアライズします。
		/// @param [in] bson BSON データ
		/// @param [in] allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON FromBSON(const Blob& bson, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	FromCBOR
		//
		////////////////////////////////////////////////////////////////

		/// @brief CBOR 形式のデータから JSON オブジェクトをデシリアライズします。
		/// @param [in] cbor CBOR データ
		/// @param [in] allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON FromCBOR(const Blob& cbor, AllowExceptions allowExceptions = AllowExceptions::No);

		////////////////////////////////////////////////////////////////
		//
		//	FromMessagePack
		//
		////////////////////////////////////////////////////////////////

		/// @brief MessagePack 形式のデータから JSON オブジェクトをデシリアライズします。
		/// @param [in] msgpack MessagePack データ
		/// @param [in] allowExceptions 例外を発生させるか
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

		friend void Formatter(FormatData& formatData, const JSON& value);

	private:

		std::variant<json_base, std::reference_wrapper<json_base>, std::reference_wrapper<const json_base>> m_json;

		[[nodiscard]]
		json_base& getRef();

		[[nodiscard]]
		const json_base& getConstRef() const;
	};

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

namespace nlohmann
{
	template <>
	struct adl_serializer<s3d::String>
	{
		static void to_json(s3d::JSON::json_base& j, const s3d::String& value);

		static void from_json(const s3d::JSON::json_base& j, s3d::String& value);
	};
}

# include "detail/JSON.ipp"
