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
# include <Siv3D/FmtExtension.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/TextWriter.hpp>
# include <Siv3D/TextReader.hpp>

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
			json[element.first] = element.second.getConstRef();
		}
	}

	JSON::JSON(const std::initializer_list<std::pair<String, JSON>>& list)
		: m_json()
	{
		auto& json = std::get<json_base>(m_json);

		for (const auto& element : list)
		{
			json[Unicode::ToUTF8(element.first)] = element.second.getConstRef();
		}
	}

	JSON::JSON(const Array<JSON>& arr)
		: m_json(nlohmann::json::array())
	{
		auto& json = std::get<json_base>(m_json);

		for (const auto& v : arr)
		{
			json.push_back(v.getConstRef());
		}
	}

	JSON::JSON(std::reference_wrapper<json_base> json)
		: m_json(json) {}

	JSON::JSON(std::reference_wrapper<const json_base> json)
		: m_json(json) {}

	JSON::JSON(json_base&& json)
		: m_json(std::move(json)) {}

	////////////////////////////////////////////////////////////////
	//
	//	getType
	//
	////////////////////////////////////////////////////////////////

	JSONValueType JSON::getType() const noexcept
	{
		return ToJSONValueType(getConstRef().type());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNull
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isNull() const noexcept
	{
		return getConstRef().is_null();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBool
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isBool() const noexcept
	{
		return getConstRef().is_boolean();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNumber
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isNumber() const noexcept
	{
		return getConstRef().is_number();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInt
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isInt() const noexcept
	{
		return getConstRef().is_number_integer();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isUint
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isUint() const noexcept
	{
		return getConstRef().is_number_unsigned();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isFloat
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isFloat() const noexcept
	{
		return getConstRef().is_number_float();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isString
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isString() const noexcept
	{
		return getConstRef().is_string();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isArray
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isArray() const noexcept
	{
		return getConstRef().is_array();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isObject
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isObject() const noexcept
	{
		return getConstRef().is_object();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBinary
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isBinary() const noexcept
	{
		return getConstRef().is_binary();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInvalid
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isInvalid() const noexcept
	{
		return getConstRef().is_discarded();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	JSON::operator bool() const noexcept
	{
		const auto& j = getConstRef();
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
		return getConstRef().contains(key);
	}

	bool JSON::hasElement(const StringView key) const
	{
		return getConstRef().contains(Unicode::ToUTF8(key));
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

		return Unicode::FromUTF8(getConstRef().get<std::string_view>());
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

		return getConstRef().get<std::string>();
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

		const auto& binary = getConstRef().get<nlohmann::json::binary_t>();

		return Blob{ binary.data(), binary.size() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::operator [](const std::string_view key)
	{
		return JSON(std::ref(getRef()[key]));
	}

	const JSON JSON::operator [](const std::string_view key) const
	{
		return JSON(std::cref(getConstRef()[key]));
	}

	JSON JSON::operator [](const StringView key)
	{
		return operator[](Unicode::ToUTF8(key));
	}

	const JSON JSON::operator [](const StringView key) const
	{
		return operator[](Unicode::ToUTF8(key));
	}

	JSON JSON::operator [](const size_t index)
	{
		auto& j = getRef();

		if (not j.is_array())
		{
			ThrowNotArray();
		}

		if (j.size() <= index)
		{
			ThrowIndexOutOfRange();
		}

		return JSON(std::ref(j[index]));
	}

	const JSON JSON::operator [](const size_t index) const
	{
		auto& j = getConstRef();

		if (not j.is_array())
		{
			ThrowNotArray();
		}

		if (j.size() <= index)
		{
			ThrowIndexOutOfRange();
		}

		return JSON(std::ref(j[index]));
	}







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
		return getConstRef().dump(static_cast<int>(spaceCount), static_cast<char>(space));
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatMinified
	//
	////////////////////////////////////////////////////////////////

	String JSON::formatMinified() const
	{
		return Unicode::FromUTF8(formatUTF8Minified());
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8Minified
	//
	////////////////////////////////////////////////////////////////

	std::string JSON::formatUTF8Minified() const
	{
		return getConstRef().dump();
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool JSON::save(const FilePathView path) const
	{
		TextWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		writer.writeUTF8(formatUTF8());

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	saveMinified
	//
	////////////////////////////////////////////////////////////////

	bool JSON::saveMinified(const FilePathView path) const
	{
		TextWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		writer.writeUTF8(formatUTF8Minified());

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	toBSON
	//
	////////////////////////////////////////////////////////////////

	Blob JSON::toBSON() const
	{
		std::vector<uint8> result;
		nlohmann::json::to_bson(getConstRef(), result);
		return Blob{ result.data(), result.size() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toCBOR
	//
	////////////////////////////////////////////////////////////////

	Blob JSON::toCBOR() const
	{
		std::vector<uint8> result;
		nlohmann::json::to_cbor(getConstRef(), result);
		return Blob{ result.data(), result.size() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	toMessagePack
	//
	////////////////////////////////////////////////////////////////

	Blob JSON::toMessagePack() const
	{
		std::vector<uint8> result;
		nlohmann::json::to_msgpack(getConstRef(), result);
		return Blob{ result.data(), result.size() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void JSON::swap(JSON& other) noexcept
	{
		m_json.swap(other.m_json);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ==
	//
	////////////////////////////////////////////////////////////////

	bool operator ==(const JSON& lhs, const JSON& rhs) noexcept
	{
		return (lhs.getConstRef() == rhs.getConstRef());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::Load(const FilePathView path, const AllowExceptions allowExceptions)
	{
		TextReader reader{ path };

		if (not reader)
		{
			if (allowExceptions)
			{
				throw Error{ fmt::format("JSON::load(): failed to open `{}`", path) };
			}

			return JSON::MakeInvalid();
		}

		return Parse(reader.readAllUTF8(), allowExceptions);
	}

	JSON JSON::Load(std::unique_ptr<IReader>&& reader, const AllowExceptions allowExceptions)
	{
		TextReader textReader{ std::move(reader) };

		if (not textReader)
		{
			if (allowExceptions)
			{
				throw Error{ "JSON::load(): failed to open from IReader" };
			}

			return JSON::MakeInvalid();
		}

		return Parse(textReader.readAllUTF8(), allowExceptions);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::Parse(const std::string_view s, const AllowExceptions allowExceptions)
	{
		JSON json;

		try
		{
			json.m_json = nlohmann::json::parse(s, nullptr, allowExceptions.getBool(), true);
		}
		catch (const nlohmann::json::parse_error& e)
		{
			if (allowExceptions)
			{
				throw Error{ fmt::format("JSON::Parse(): {}", e.what()) };
			}

			return JSON::MakeInvalid();
		}

		return json;
	}

	JSON JSON::Parse(const StringView s, const AllowExceptions allowExceptions)
	{
		return Parse(Unicode::ToUTF8(s), allowExceptions);
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

	////////////////////////////////////////////////////////////////
	//
	//	MakeInvalid
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::MakeInvalid()
	{
		return JSON(JSONValueType::Invalid);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const JSON& value)
	{
		formatData.string.append(value.format());
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	JSON::json_base& JSON::getRef()
	{
		if (std::holds_alternative<json_base>(m_json))
		{
			return std::get<json_base>(m_json);
		}
		else if (std::holds_alternative<std::reference_wrapper<json_base>>(m_json))
		{
			return std::get<std::reference_wrapper<json_base>>(m_json).get();
		}
		else
		{
			m_json = json_base(std::get<std::reference_wrapper<const json_base>>(m_json).get());
			return std::get<json_base>(m_json);
		}
	}

	const JSON::json_base& JSON::getConstRef() const
	{
		if (std::holds_alternative<json_base>(m_json))
		{
			return std::get<json_base>(m_json);
		}
		else if (std::holds_alternative<std::reference_wrapper<json_base>>(m_json))
		{
			return std::get<std::reference_wrapper<json_base>>(m_json).get();
		}
		else
		{
			return std::get<std::reference_wrapper<const json_base>>(m_json).get();
		}
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
