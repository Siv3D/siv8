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
# include "JSONValueType.hpp"
# include "PredefinedYesNo.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(26819)
# include <ThirdParty/nlohmann/json.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	class JSON;

	template <class Type>
	concept JSONCompatibleType =
		std::is_constructible_v<JSON, Type>
		&& (not std::is_same_v<std::decay_t<Type>, JSON>)
		&& (not std::is_same_v<std::decay_t<Type>, JSONValueType>);

	class JSON
	{
	public:

		using json_base = nlohmann::json;

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

		////////////////////////////////////////////////////////////////
		//
		//	operator = 
		//
		////////////////////////////////////////////////////////////////

		JSON& operator =(auto&& value);

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
		[[nodiscard]]
		bool isNull() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isBool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値の型が bool であるかを返します。 
		[[nodiscard]]
		bool isBool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNumber
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が数値型（整数型、符号なし整数型、浮動小数点数型）であるかを返します。 
		[[nodiscard]]
		bool isNumber() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInt
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が整数型（整数型、符号なし整数型）であるかを返します。 
		[[nodiscard]]
		bool isInt() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isUint
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が符号なし整数型であるかを返します。 
		[[nodiscard]]
		bool isUint() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が浮動小数点数型であるかを返します。 
		[[nodiscard]]
		bool isFloat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isString
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が文字列型であるかを返します。 
		[[nodiscard]]
		bool isString() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isArray
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が配列型であるかを返します。 
		[[nodiscard]]
		bool isArray() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isObject
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値がオブジェクト型であるかを返します。 
		[[nodiscard]]
		bool isObject() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isBinary
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値がバイナリデータ型であるかを返します。 
		[[nodiscard]]
		bool isBinary() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInvalid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が不正であるかを返します。
		[[nodiscard]]
		bool isInvalid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が Null または不正でないかを返します。
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hasElement
		//
		////////////////////////////////////////////////////////////////

		/// @brief 指定したキーの要素が存在するかを返します。
		/// @param key キー
		[[nodiscard]]
		bool hasElement(std::string_view key) const;

		/// @brief 指定したキーの要素が存在するかを返します。
		/// @param key キー
		[[nodiscard]]
		bool hasElement(StringView key) const;

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

		[[nodiscard]]
		JSON operator [](std::string_view key);

		[[nodiscard]]
		const JSON operator [](std::string_view key) const;

		[[nodiscard]]
		JSON operator [](StringView key);

		[[nodiscard]]
		const JSON operator [](StringView key) const;

		[[nodiscard]]
		JSON operator [](size_t index);

		[[nodiscard]]
		const JSON operator [](size_t index) const;










		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを文字列にフォーマットした結果を返します。
		/// @param space インデントの文字
		/// @param spaceCount インデントの文字数
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		String format(char32 space = U' ', size_t spaceCount = 2) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを UTF-8 文字列にフォーマットした結果を返します。
		/// @param space インデントの文字
		/// @param spaceCount インデントの文字数
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		std::string formatUTF8(char32 space = U' ', size_t spaceCount = 2) const;

		////////////////////////////////////////////////////////////////
		//
		//	formatMinified
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の文字列にフォーマットした結果を返します。
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		String formatMinified() const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8Minified
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の UTF-8 文字列にフォーマットした結果を返します。
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		std::string formatUTF8Minified() const;

		////////////////////////////////////////////////////////////////
		//
		//	save
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データをファイルに保存します。
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool save(FilePathView path) const;

		////////////////////////////////////////////////////////////////
		//
		//	saveMinified
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の形式でファイルに保存します。
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool saveMinified(FilePathView path) const;

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
		/// @param [in] path ファイルパス
		/// @param [in] allowExceptions 例外を発生させるか
		/// @return JSON オブジェクト
		[[nodiscard]]
		static JSON Load(FilePathView path, AllowExceptions allowExceptions = AllowExceptions::No);

		template <class Reader>
			requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
		[[nodiscard]]
		static JSON Load(Reader&& reader, AllowExceptions allowExceptions = AllowExceptions::No);

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
		//	MakeArray
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static JSON MakeArray();

		////////////////////////////////////////////////////////////////
		//
		//	MakeObject
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static JSON MakeObject();

		////////////////////////////////////////////////////////////////
		//
		//	MakeBinary
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static JSON MakeBinary();

		////////////////////////////////////////////////////////////////
		//
		//	MakeInvalid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static JSON MakeInvalid();

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
