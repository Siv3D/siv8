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
# include "Common.hpp"
# include "Array.hpp"
# include "String.hpp"
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

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////



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





		[[nodiscard]]
		friend bool operator ==(const JSON& lhs, const JSON& rhs) noexcept
		{
			return (lhs.m_json == rhs.m_json);
		}

		[[nodiscard]]
		static JSON MakeArray();

		[[nodiscard]]
		static JSON MakeObject();

		[[nodiscard]]
		static JSON MakeBinary();

	private:

		nlohmann::json m_json;
	};
}

namespace nlohmann
{
	template <>
	struct adl_serializer<s3d::String>
	{
		static void to_json(json& j, const s3d::String& value);

		static void from_json(const json& j, s3d::String& value);
	};
}

# include "detail/JSON.ipp"
