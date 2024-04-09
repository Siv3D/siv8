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
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/Error.hpp>

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

		[[noreturn]]
		static void ThrowNotString()
		{
			throw Error{ U"JSON::getString(): This JSON value is not a string" };
		}

		[[noreturn]]
		static void ThrowNotStringUTF8()
		{
			throw Error{ U"JSON::getUTF8(): This JSON value is not a string" };
		}

		[[noreturn]]
		static void ThrowNotBinary()
		{
			throw Error{ U"JSON::getBinary(): This JSON value is not a binary" };
		}

		[[noreturn]]
		static void ThrowNotArray()
		{
			throw Error{ U"JSON::operator [](size_t): This JSON value is not an array" };
		}

		[[noreturn]]
		static void ThrowIndexOutOfRange()
		{
			throw Error{ U"JSON::operator [](size_t): Index out of range" };
		}
	}

	namespace detail
	{
		void ThrowJSONGetError(const char* type, const std::string_view s)
		{
			throw Error{ fmt::format("JSON::get<{}>(\"{}\") failed", DemangleUTF8(type), s) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

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
		auto& json = std::get<json_base>(m_json);

		for (const auto& element : list)
		{
			json[element.first] = element.second.get();
		}
	}

	JSON::JSON(const std::initializer_list<std::pair<String, JSON>>& list)
		: m_json()
	{
		auto& json = std::get<json_base>(m_json);

		for (const auto& element : list)
		{
			json[Unicode::ToUTF8(element.first)] = element.second.get();
		}
	}

	JSON::JSON(const Array<JSON>& arr)
		: m_json(nlohmann::json::array())
	{
		auto& json = std::get<json_base>(m_json);

		for (const auto& v : arr)
		{
			json.push_back(v.get());
		}
	}

	//JSON::JSON(const json_base& j)
	//	: m_json(j) {}

	//JSON::JSON(json_base&& j)
	//	: m_json(std::move(j)) {}

	////////////////////////////////////////////////////////////////
	//
	//	getType
	//
	////////////////////////////////////////////////////////////////

	JSONValueType JSON::getType() const noexcept
	{
		return ToJSONValueType(get().type());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNull
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isNull() const noexcept
	{
		return get().is_null();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBool
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isBool() const noexcept
	{
		return get().is_boolean();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNumber
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isNumber() const noexcept
	{
		return get().is_number();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInt
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isInt() const noexcept
	{
		return get().is_number_integer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isUint
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isUint() const noexcept
	{
		return get().is_number_unsigned();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isFloat
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isFloat() const noexcept
	{
		return get().is_number_float();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isString
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isString() const noexcept
	{
		return get().is_string();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isArray
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isArray() const noexcept
	{
		return get().is_array();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isObject
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isObject() const noexcept
	{
		return get().is_object();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBinary
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isBinary() const noexcept
	{
		return get().is_binary();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInvalid
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isInvalid() const noexcept
	{
		return get().is_discarded();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	JSON::operator bool() const noexcept
	{
		const auto& j = get();
		return ((not j.is_null())
			&&  (not j.is_discarded()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasElement
	//
	////////////////////////////////////////////////////////////////

	bool JSON::hasElement(const std::string_view key) const
	{
		return get().contains(key);
	}

	bool JSON::hasElement(const StringView key) const
	{
		return get().contains(Unicode::ToUTF8(key));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getString
	//
	////////////////////////////////////////////////////////////////

	String JSON::getString() const
	{
		if (not isString())
		{
			ThrowNotString();
		}

		return Unicode::FromUTF8(get().get<std::string_view>());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string JSON::getUTF8() const
	{
		if (not isString())
		{
			ThrowNotStringUTF8();
		}

		return get().get<std::string>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBinary
	//
	////////////////////////////////////////////////////////////////

	Blob JSON::getBinary() const
	{
		if (not isBinary())
		{
			ThrowNotBinary();
		}

		const auto& binary = get().get<nlohmann::json::binary_t>();

		return Blob{ binary.data(), binary.size() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::operator [](const std::string_view key)
	{
		return JSON(std::ref(get()[key]));
	}

	const JSON JSON::operator [](const std::string_view key) const
	{
		return JSON(std::cref(get()[key]));
	}

	JSON JSON::operator [](const StringView key)
	{
		return operator[](Unicode::ToUTF8(key));
	}

	const JSON JSON::operator [](const StringView key) const
	{
		return operator[](Unicode::ToUTF8(key));
	}

	//JSON JSON::operator [](const size_t index)
	//{
	//	if (not isArray())
	//	{
	//		ThrowNotArray();
	//	}

	//	if (m_json.size() <= index)
	//	{
	//		ThrowIndexOutOfRange();
	//	}

	//	return JSON(m_json[index]);
	//}

	//const JSON JSON::operator [](const size_t index) const
	//{
	//	if (not isArray())
	//	{
	//		ThrowNotArray();
	//	}

	//	if (m_json.size() <= index)
	//	{
	//		ThrowIndexOutOfRange();
	//	}

	//	return JSON(m_json[index]);
	//}







	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String JSON::format(const char32 space, const size_t spaceCount) const
	{
		return Unicode::FromUTF8(formatUTF8(space, spaceCount));
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string JSON::formatUTF8(const char32 space, const size_t spaceCount) const
	{
		return get().dump(static_cast<int>(spaceCount), static_cast<char>(space));
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatMinimum
	//
	////////////////////////////////////////////////////////////////

	String JSON::formatMinimum() const
	{
		return Unicode::FromUTF8(formatUTF8Minimum());
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8Minimum
	//
	////////////////////////////////////////////////////////////////

	std::string JSON::formatUTF8Minimum() const
	{
		return get().dump();
	}

	////////////////////////////////////////////////////////////////
	//
	//	MakeArray
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::MakeArray()
	{
		return JSON(JSONValueType::Array);
	}

	////////////////////////////////////////////////////////////////
	//
	//	MakeObject
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::MakeObject()
	{
		return JSON(JSONValueType::Object);
	}

	////////////////////////////////////////////////////////////////
	//
	//	MakeBinary
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::MakeBinary()
	{
		return JSON(JSONValueType::Binary);
	}

}

namespace nlohmann
{
	void adl_serializer<s3d::String>::to_json(s3d::JSON::json_base& j, const s3d::String& value)
	{
		j = s3d::Unicode::ToUTF8(value);
	}

	void adl_serializer<s3d::String>::from_json(const s3d::JSON::json_base& j, s3d::String& value)
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
