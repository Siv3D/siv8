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

# include <Siv3D/JSON.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static constexpr nlohmann::json::value_t ToValueType(const JSONValueType valueType) noexcept
		{
			return static_cast<nlohmann::json::value_t>(valueType);
		}

		[[nodiscard]]
		static constexpr JSONValueType ToJSONValueType(const nlohmann::json::value_t valueType) noexcept
		{
			return static_cast<JSONValueType>(valueType);
		}
	}

	JSON::JSON(const std::nullptr_t)
		: m_json(nullptr) {}

	JSON::JSON(const JSONValueType valueType)
		: m_json(ToValueType(valueType)) {}

	JSON::JSON(const char* value)
		: m_json(value) {}

	JSON::JSON(const std::string_view value)
		: m_json(value) {}

	JSON::JSON(const char32* value)
		: m_json(String{ value }) {}

	JSON::JSON(const StringView value)
		: m_json(String{ value }) {}

	JSON::JSON(const String& value)
		: m_json(value) {}

	JSON::JSON(const bool value)
		: m_json(value) {}

	JSON::JSON(const std::initializer_list<std::pair<std::string, JSON>>& list)
		: m_json()
	{
		for (const auto& element : list)
		{
			m_json[element.first] = element.second.m_json;
		}
	}

	JSON::JSON(const std::initializer_list<std::pair<String, JSON>>& list)
		: m_json()
	{
		for (const auto& element : list)
		{
			m_json[Unicode::ToUTF8(element.first)] = element.second.m_json;
		}
	}

	JSON::JSON(const Array<JSON>& arr)
		: m_json(nlohmann::json::array())
	{
		for (const auto& v : arr)
		{
			m_json.push_back(v.m_json);
		}
	}






	////////////////////////////////////////////////////////////////
	//
	//	getType
	//
	////////////////////////////////////////////////////////////////

	JSONValueType JSON::getType() const noexcept
	{
		return ToJSONValueType(m_json.type());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNull
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isNull() const noexcept
	{
		return m_json.is_null();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBool
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isBool() const noexcept
	{
		return m_json.is_boolean();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNumber
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isNumber() const noexcept
	{
		return m_json.is_number();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInt
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isInt() const noexcept
	{
		return m_json.is_number_integer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isUint
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isUint() const noexcept
	{
		return m_json.is_number_unsigned();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isFloat
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isFloat() const noexcept
	{
		return m_json.is_number_float();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isString
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isString() const noexcept
	{
		return m_json.is_string();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isArray
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isArray() const noexcept
	{
		return m_json.is_array();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isObject
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isObject() const noexcept
	{
		return m_json.is_object();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBinary
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isBinary() const noexcept
	{
		return m_json.is_binary();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInvalid
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isInvalid() const noexcept
	{
		return m_json.is_discarded();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	JSON::operator bool() const noexcept
	{
		return ((not m_json.is_null())
			&&  (not m_json.is_discarded()));
	}



	JSON JSON::MakeArray()
	{
		return JSON(JSONValueType::Array);
	}

	JSON JSON::MakeObject()
	{
		return JSON(JSONValueType::Object);
	}

	JSON JSON::MakeBinary()
	{
		return JSON(JSONValueType::Binary);
	}
}

namespace nlohmann
{
	void adl_serializer<s3d::String>::to_json(json& j, const s3d::String& value)
	{
		j = s3d::Unicode::ToUTF8(value);
	}

	void adl_serializer<s3d::String>::from_json(const json& j, s3d::String& value)
	{
		if (j.is_string())
		{
			value = s3d::Unicode::FromUTF8(j.get<std::string>());
		}
		else
		{
			value.clear();
		}
	}
}
