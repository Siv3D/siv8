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
# include <Siv3D/JSONIterator.hpp>
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
		static void ThrowOpSubscriptNotArray()
		{
			throw Error{ U"JSON::operator [](size_t): This JSON value is not an array" };
		}

		[[noreturn]]
		static void ThrowOpSubscriptIndexOutOfRange()
		{
			throw Error{ U"JSON::operator [](size_t): Index out of range" };
		}

		[[noreturn]]
		static void ThrowEraseNotArray()
		{
			throw Error{ U"JSON::erase(size_t): This JSON value is not an array" };
		}

		[[noreturn]]
		static void ThrowEraseIndexOutOfRange()
		{
			throw Error{ U"JSON::erase(size_t): Index out of range" };
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
	//	operator = 
	//
	////////////////////////////////////////////////////////////////

	JSON& JSON::operator =(const char32* value)
	{
		return operator =(Unicode::ToUTF8(value));
	}

	JSON& JSON::operator =(const std::u32string_view value)
	{
		return operator =(Unicode::ToUTF8(value));
	}

	JSON& JSON::operator =(const StringView value)
	{
		return operator =(Unicode::ToUTF8(value));
	}

	JSON& JSON::operator =(const JSON& value)
	{
		return operator =(value.getConstRef());
	}

	JSON& JSON::operator =(JSON&& value)
	{
		return operator =(value.getConstRef());
	}

	JSON& JSON::operator =(JSON& value)
	{
		return operator =(value.getConstRef());
	}

	////////////////////////////////////////////////////////////////
	//
	//	clone
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::clone() const
	{
		return JSON(json_base(getConstRef()));
	}

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
	//	isPrimitive
	//
	////////////////////////////////////////////////////////////////

	bool JSON::isPrimitive() const noexcept
	{
		return getConstRef().is_primitive();
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
	//	contains
	//
	////////////////////////////////////////////////////////////////

	bool JSON::contains(const std::string_view key) const
	{
		return getConstRef().contains(key);
	}

	bool JSON::contains(const StringView key) const
	{
		return getConstRef().contains(Unicode::ToUTF8(key));
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	size_t JSON::size() const noexcept
	{
		return getConstRef().size();
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
			ThrowOpSubscriptNotArray();
		}

		if (j.size() <= index)
		{
			ThrowOpSubscriptIndexOutOfRange();
		}

		return JSON(std::ref(j[index]));
	}

	const JSON JSON::operator [](const size_t index) const
	{
		auto& j = getConstRef();

		if (not j.is_array())
		{
			ThrowOpSubscriptNotArray();
		}

		if (j.size() <= index)
		{
			ThrowOpSubscriptIndexOutOfRange();
		}

		return JSON(std::ref(j[index]));
	}

	////////////////////////////////////////////////////////////////
	//
	//	push_back
	//
	////////////////////////////////////////////////////////////////

	void JSON::push_back(const JSON& value)
	{
		getRef().push_back(value.getConstRef());
	}

	////////////////////////////////////////////////////////////////
	//
	//	pop_back
	//
	////////////////////////////////////////////////////////////////

	void JSON::pop_back()
	{
		getRef().erase(getRef().end() - 1);
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void JSON::clear()
	{
		getRef().clear();
	}

	////////////////////////////////////////////////////////////////
	//
	//	erase
	//
	////////////////////////////////////////////////////////////////

	void JSON::erase(const std::string_view key)
	{
		getRef().erase(key);
	}

	void JSON::erase(const StringView key)
	{
		getRef().erase(Unicode::ToUTF8(key));
	}

	void JSON::erase(const size_t index)
	{
		auto& j = getRef();

		if (not j.is_array())
		{
			ThrowEraseNotArray();
		}

		if (j.size() <= index)
		{
			ThrowEraseIndexOutOfRange();
		}

		j.erase(j.begin() + index);
	}

	////////////////////////////////////////////////////////////////
	//
	//	begin, end
	//
	////////////////////////////////////////////////////////////////

	JSON::iterator JSON::begin() noexcept
	{
		auto& j = getRef();

		if (j.is_array())
		{
			return iterator{ j.begin(), 0 };
		}
		else
		{
			return iterator{ j.begin() };
		}
	}

	JSON::iterator JSON::end() noexcept
	{
		auto& j = getRef();

		if (j.is_array())
		{
			return iterator{ j.end(), j.size() };
		}
		else
		{
			return iterator{ j.end() };
		}
	}

	JSON::const_iterator JSON::begin() const noexcept
	{
		return cbegin();
	}

	JSON::const_iterator JSON::end() const noexcept
	{
		return cend();
	}

	////////////////////////////////////////////////////////////////
	//
	//	cbegin, cend
	//
	////////////////////////////////////////////////////////////////

	JSON::const_iterator JSON::cbegin() const noexcept
	{
		const auto& j = getConstRef();

		if (j.is_array())
		{
			return const_iterator{ j.cbegin(), 0 };
		}
		else
		{
			return const_iterator{ j.cbegin() };
		}
	}

	JSON::const_iterator JSON::cend() const noexcept
	{
		const auto& j = getConstRef();

		if (j.is_array())
		{
			return const_iterator{ j.cend(), j.size() };
		}
		else
		{
			return const_iterator{ j.cend() };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flatten
	//
	////////////////////////////////////////////////////////////////

	[[nodiscard]]
	JSON JSON::flatten() const
	{
		return JSON(getConstRef().flatten());
	}

	////////////////////////////////////////////////////////////////
	//
	//	unflatten
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::unflatten() const
	{
		return JSON(getConstRef().unflatten());
	}

	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String JSON::format(const char32 indent, const size_t spaceCount, const EnsureAscii ensureAscii) const
	{
		return Unicode::FromUTF8(formatUTF8(indent, spaceCount, ensureAscii));
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string JSON::formatUTF8(const char32 indent, const size_t spaceCount, const EnsureAscii ensureAscii) const
	{
		return getConstRef().dump(static_cast<int>(spaceCount), static_cast<char>(indent), ensureAscii.getBool());
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatMinified
	//
	////////////////////////////////////////////////////////////////

	String JSON::formatMinified(const EnsureAscii ensureAscii) const
	{
		return Unicode::FromUTF8(formatUTF8Minified(ensureAscii));
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8Minified
	//
	////////////////////////////////////////////////////////////////

	std::string JSON::formatUTF8Minified(const EnsureAscii ensureAscii) const
	{
		return getConstRef().dump(-1, ' ', ensureAscii.getBool());
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool JSON::save(const FilePathView path, const char32 indent, const size_t spaceCount, const EnsureAscii ensureAscii) const
	{
		TextWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		writer.writeUTF8(formatUTF8(indent, spaceCount, ensureAscii));

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	saveMinified
	//
	////////////////////////////////////////////////////////////////

	bool JSON::saveMinified(const FilePathView path, const EnsureAscii ensureAscii) const
	{
		TextWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		writer.writeUTF8(formatUTF8Minified(ensureAscii));

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

			return JSON::Invalid();
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

			return JSON::Invalid();
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
			throw Error{ fmt::format("JSON::Parse(): {}", e.what()) };
		}

		return json;
	}

	JSON JSON::Parse(const StringView s, const AllowExceptions allowExceptions)
	{
		return Parse(Unicode::ToUTF8(s), allowExceptions);
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromBSON
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::FromBSON(const Blob& bson, const AllowExceptions allowExceptions)
	{
		JSON json;

		try
		{
			json.m_json = nlohmann::json::from_bson(bson.begin(), bson.end(), true, allowExceptions.getBool());
		}
		catch (const std::exception& e)
		{
			throw Error{ fmt::format("JSON::FromBSON(): {}", e.what()) };
		}

		return json;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromCBOR
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::FromCBOR(const Blob& cbor, const AllowExceptions allowExceptions)
	{
		JSON json;

		try
		{
			json.m_json = nlohmann::json::from_cbor(cbor.begin(), cbor.end(), true, allowExceptions.getBool());
		}
		catch (const std::exception& e)
		{
			throw Error{ fmt::format("JSON::FromCBOR(): {}", e.what()) };
		}

		return json;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromMessagePack
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::FromMessagePack(const Blob& msgpack, const AllowExceptions allowExceptions)
	{
		JSON json;

		try
		{
			json.m_json = nlohmann::json::from_msgpack(msgpack.begin(), msgpack.end(), true, allowExceptions.getBool());
		}
		catch (const std::exception& e)
		{
			throw Error{ fmt::format("JSON::FromMessagePack(): {}", e.what()) };
		}

		return json;
	}

	////////////////////////////////////////////////////////////////
	//
	//	EmptyArray
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::EmptyArray()
	{
		return JSON(JSONValueType::Array);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EmptyObject
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::EmptyObject()
	{
		return JSON(JSONValueType::Object);
	}

	////////////////////////////////////////////////////////////////
	//
	//	EmptyBinary
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::EmptyBinary()
	{
		return JSON(JSONValueType::Binary);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Invalid
	//
	////////////////////////////////////////////////////////////////

	JSON JSON::Invalid()
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

	inline namespace Literals
	{
		inline namespace JSONLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_json
			//
			////////////////////////////////////////////////////////////////

			[[nodiscard]]
			JSON operator ""_json(const char* str, size_t length)
			{
				return JSON::Parse(std::string_view{ str, length });
			}

			[[nodiscard]]
			JSON operator ""_json(const char32_t* str, size_t length)
			{
				return JSON::Parse(StringView{ str, length });
			}
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
