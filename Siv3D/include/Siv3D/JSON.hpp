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

		//JSON(const json_base& j);

		//JSON(json_base&& j);

		JSON(std::reference_wrapper<json_base> j)
			: m_json(j) {}

		JSON(std::reference_wrapper<const json_base> j)
			: m_json(j) {}

		////////////////////////////////////////////////////////////////
		//
		//	operator = 
		//
		////////////////////////////////////////////////////////////////

		JSON& operator =(auto&& value)
		{
			get() = std::forward<decltype(value)>(value);
			return *this;
		}

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
		//	formatMinimum
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の文字列にフォーマットした結果を返します。
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		String formatMinimum() const;

		////////////////////////////////////////////////////////////////
		//
		//	formatUTF8Minimum
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の UTF-8 文字列にフォーマットした結果を返します。
		/// @return フォーマットされた JSON データ
		[[nodiscard]]
		std::string formatUTF8Minimum() const;

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
		//	saveMinimum
		//
		////////////////////////////////////////////////////////////////

		/// @brief JSON データを最小限の形式でファイルに保存します。
		/// @param path 保存するファイルのパス
		/// @return 保存に成功した場合 true, それ以外の場合は false
		bool saveMinimum(FilePathView path) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend bool operator ==(const JSON& lhs, const JSON& rhs) noexcept
		{
			return (lhs.get() == rhs.get());
		}

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

	private:

		std::variant<json_base, std::reference_wrapper<json_base>> m_json;

		[[nodiscard]]
		json_base& get()
		{
			if (std::holds_alternative<json_base>(m_json))
			{
				return std::get<json_base>(m_json);
			}
			else
			{
				return std::get<std::reference_wrapper<json_base>>(m_json).get();
			}
		}

		[[nodiscard]]
		const json_base& get() const
		{
			if (std::holds_alternative<json_base>(m_json))
			{
				return std::get<json_base>(m_json);
			}
			else
			{
				return std::get<std::reference_wrapper<json_base>>(m_json).get();
			}
		}

		//[[nodiscard]]
		//json_base clone() const
		//{
		//	return std::visit([](const auto& j) { return j; }, m_json);
		//}

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
