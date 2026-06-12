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

# include <Siv3D/TOML.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/TextEncoding.hpp>
# include <Siv3D/BinaryFileReader.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String MakeShortValue(String s)
		{
			constexpr size_t MaxLength = 64;
			const bool truncated = (MaxLength < s.size());
			String result;

			for (const auto ch : s.substr(0, MaxLength))
			{
				switch (ch)
				{
				case U'\n': result.append(U"\\n"); break;
				case U'\r': result.append(U"\\r"); break;
				case U'\t': result.append(U"\\t"); break;
				default: result.push_back(ch); break;
				}
			}

			if (truncated)
			{
				result.append(U" ...");
			}
			
			return result;
		}

		[[nodiscard]]
		static int32 ToOffsetMinutes(const toml::time_offset& offset) noexcept
		{
			return ((static_cast<int32>(offset.hour) * 60) + static_cast<int32>(offset.minute));
		}

		[[nodiscard]]
		static String MakeKeyPath(const StringView parent, const StringView key)
		{
			return U"{}[\"{}\"]"_fmt(parent, key);
		}

		[[nodiscard]]
		static std::string MakeParseName(const FilePathView pathHint)
		{
			return pathHint ? Unicode::ToUTF8(pathHint) : std::string{ "internal string" };
		}

		[[nodiscard]]
		static Array<TOMLParseErrorReason> ToParseErrorReasons(const std::vector<toml::error_info>& internalErrors)
		{
			Array<TOMLParseErrorReason> errors;
			errors.reserve(internalErrors.size());

			for (const auto& internalError : internalErrors)
			{
				TOMLParseErrorReason error;
				error.title = Unicode::FromUTF8(internalError.title());
				error.hint = Unicode::FromUTF8(internalError.suffix());

				for (const auto& location : internalError.locations())
				{
					error.locations.push_back(Unicode::FromUTF8(toml::format_location(location.first, location.second)));
				}

				errors.push_back(std::move(error));
			}

			return errors;
		}

		[[nodiscard]]
		static bool ReadTextFile(IReader& reader, std::string& content)
		{
			content.clear();

			if (not reader.isOpen())
			{
				return false;
			}

			const TextEncoding encoding = Unicode::GetTextEncoding(reader);
			if ((encoding != TextEncoding::UTF8_NO_BOM) && (encoding != TextEncoding::UTF8_WITH_BOM))
			{
				return false;
			}

			const int64 start = Unicode::GetBOMSize(encoding);
			const int64 size = (reader.size() - start);
			if (size < 0)
			{
				return false;
			}

			content.resize(static_cast<size_t>(size));
			if (size == 0)
			{
				return true;
			}

			if (size != reader.read(content.data(), start, size))
			{
				content.clear();
				return false;
			}

			return true;
		}

		[[noreturn]]
		static void ThrowAtNotTable(const StringView key, const FilePathView path, const TOMLValueType type)
		{
			throw Error{ U"TOML::at(\"{}\"): value at {} is not a table. Actual type: {}"_fmt(key, path, type) };
		}

		[[noreturn]]
		static void ThrowAtKeyNotFound(const StringView key, const FilePathView path)
		{
			throw Error{ U"TOML::at(\"{}\"): key was not found at {}"_fmt(key, path) };
		}

		[[noreturn]]
		static void ThrowAtNotArray(const size_t index, const FilePathView path, const TOMLValueType type)
		{
			throw Error{ U"TOML::at({}): value at {} is not an array. Actual type: {}"_fmt(index, path, type) };
		}

		[[noreturn]]
		static void ThrowAtIndexOutOfRange(const size_t index, const FilePathView path, const size_t size)
		{
			throw Error{ U"TOML::at({}): index out of range at {}. Size: {}"_fmt(index, path, size) };
		}

		[[noreturn]]
		static void ThrowNotArrayView(const StringView path, const TOMLValueType actualType)
		{
			throw Error{ U"TOML::arrayView(): value at {} is not an array. Actual type: {}"_fmt(path, actualType) };
		}

		[[noreturn]]
		static void ThrowNotTableView(const StringView path, const TOMLValueType actualType)
		{
			throw Error{ U"TOML::tableView(): value at {} is not a table. Actual type: {}"_fmt(path, actualType) };
		}
	}

	DateTime TOML::LocalTime::toDateTime() const noexcept
	{
		return DateTime{ 0, 0, 0, hour, minute, second, millisecond };
	}

	DateTime TOML::LocalTime::toDateTimeTruncatedToMilliseconds() const noexcept
	{
		return toDateTime();
	}

	TOML::LocalTime TOML::LocalTime::FromTOML(const toml::local_time& localTime) noexcept
	{
		return LocalTime{
			static_cast<uint8>(localTime.hour),
			static_cast<uint8>(localTime.minute),
			static_cast<uint8>(localTime.second),
			static_cast<uint16>(localTime.millisecond),
			static_cast<uint16>(localTime.microsecond),
			static_cast<uint16>(localTime.nanosecond)
		};
	}

	DateTime TOML::LocalDateTime::toDateTime() const noexcept
	{
		return DateTime{ date, time.hour, time.minute, time.second, time.millisecond };
	}

	DateTime TOML::LocalDateTime::toDateTimeTruncatedToMilliseconds() const noexcept
	{
		return toDateTime();
	}

	TOML::LocalDateTime TOML::LocalDateTime::FromTOML(const toml::local_datetime& localDateTime) noexcept
	{
		return LocalDateTime{ ToDate(localDateTime.date), LocalTime::FromTOML(localDateTime.time) };
	}

	TOML::OffsetDateTime TOML::OffsetDateTime::FromTOML(const toml::offset_datetime& offsetDateTime) noexcept
	{
		return OffsetDateTime{
			LocalDateTime{ ToDate(offsetDateTime.date), LocalTime::FromTOML(offsetDateTime.time) },
			ToOffsetMinutes(offsetDateTime.offset)
		};
	}

	bool TOML::isValid() const noexcept
	{
		return (m_value != nullptr);
	}

	TOML::operator bool() const noexcept
	{
		return isValid();
	}

	const String& TOML::path() const noexcept
	{
		return m_path;
	}

	TOMLValueType TOML::type() const noexcept
	{
		if (not m_value)
		{
			return TOMLValueType::Invalid;
		}

		switch (m_value->type())
		{
		case toml::value_t::boolean:         return TOMLValueType::Bool;
		case toml::value_t::integer:         return TOMLValueType::Int;
		case toml::value_t::floating:        return TOMLValueType::Float;
		case toml::value_t::string:          return TOMLValueType::String;
		case toml::value_t::offset_datetime: return TOMLValueType::OffsetDateTime;
		case toml::value_t::local_datetime:  return TOMLValueType::LocalDateTime;
		case toml::value_t::local_date:      return TOMLValueType::LocalDate;
		case toml::value_t::local_time:      return TOMLValueType::LocalTime;
		case toml::value_t::array:           return TOMLValueType::Array;
		case toml::value_t::table:           return TOMLValueType::Table;
		default:                             return TOMLValueType::Invalid;
		}
	}

	TOMLValueType TOML::getType() const noexcept
	{
		return type();
	}
	
	bool TOML::hasValue() const noexcept
	{
		return (m_value && (m_value->type() != toml::value_t::empty));
	}
	
	bool TOML::isInvalid() const noexcept
	{
		return (not isValid());
	}
	
	bool TOML::isEmpty() const noexcept
	{
		return (m_value && (m_value->type() == toml::value_t::empty));
	}

	bool TOML::isBool() const noexcept
	{
		return (m_value && m_value->is_boolean());
	}
	
	bool TOML::isInt() const noexcept
	{
		return (m_value && m_value->is_integer());
	}
	
	bool TOML::isFloat() const noexcept
	{
		return (m_value && m_value->is_floating());
	}
	
	bool TOML::isString() const noexcept
	{
		return (m_value && m_value->is_string());
	}
	
	bool TOML::isLocalDate() const noexcept
	{
		return (m_value && m_value->is_local_date());
	}
	
	bool TOML::isLocalTime() const noexcept
	{
		return (m_value && m_value->is_local_time());
	}
	
	bool TOML::isLocalDateTime() const noexcept
	{
		return (m_value && m_value->is_local_datetime());
	}
	
	bool TOML::isOffsetDateTime() const noexcept
	{
		return (m_value && m_value->is_offset_datetime());
	}
	
	bool TOML::isArray() const noexcept
	{
		return (m_value && m_value->is_array());
	}
	
	bool TOML::isTable() const noexcept
	{
		return (m_value && m_value->is_table());
	}
	
	bool TOML::is(const TOMLValueType valueType) const noexcept
	{
		return (type() == valueType);
	}

	bool TOML::contains(const std::string& key) const
	{
		return (isTable() && m_value->contains(key));
	}

	bool TOML::contains(const StringView key) const
	{
		return contains(Unicode::ToUTF8(key));
	}

	size_t TOML::size() const noexcept
	{
		if (isArray())
		{
			return m_value->as_array().size();
		}
		
		if (isTable())
		{
			return m_value->as_table().size();
		}

		return 0;
	}

	bool TOML::isEmptyArray() const noexcept
	{
		return (isArray() && m_value->as_array().empty());
	}
	
	bool TOML::isEmptyTable() const noexcept
	{
		return (isTable() && m_value->as_table().empty());
	}

	TOML TOML::operator [](const std::string& key) const
	{
		const String key32 = Unicode::FromUTF8(key);
		
		if (not isTable())
		{
			return InvalidWithPath(MakeKeyPath(m_path, key32));
		}
		
		if (not m_value->contains(key))
		{
			return InvalidWithPath(MakeKeyPath(m_path, key32));
		}
		
		return TOML{ m_root, &(m_value->at(key)), MakeKeyPath(m_path, key32) };
	}

	TOML TOML::operator [](const StringView key) const
	{
		return operator [](Unicode::ToUTF8(key));
	}

	TOML TOML::operator [](const size_t index) const
	{
		if (not isArray())
		{
			return InvalidWithPath(MakeIndexPath(m_path, index));
		}
		
		const auto& array = m_value->as_array();
		
		if (array.size() <= index)
		{
			return InvalidWithPath(MakeIndexPath(m_path, index));
		}
		
		return TOML{ m_root, &(array[index]), MakeIndexPath(m_path, index) };
	}

	TOML TOML::at(const std::string& key) const
	{
		const String key32 = Unicode::FromUTF8(key);
		
		if (not isTable())
		{
			ThrowAtNotTable(key32, m_path, type());
		}
		
		if (not m_value->contains(key))
		{
			ThrowAtKeyNotFound(key32, m_path);
		}
		
		return TOML{ m_root, &(m_value->at(key)), MakeKeyPath(m_path, key32) };
	}

	TOML TOML::at(const StringView key) const
	{
		return at(Unicode::ToUTF8(key));
	}

	TOML TOML::at(const size_t index) const
	{
		if (not isArray())
		{
			ThrowAtNotArray(index, m_path, type());
		}
		
		const auto& array = m_value->as_array();
		
		if (array.size() <= index)
		{
			ThrowAtIndexOutOfRange(index, m_path, size());
		}
		
		return TOML{ m_root, &(array[index]), MakeIndexPath(m_path, index) };
	}

	String TOML::getString() const
	{
		return get<String>();
	}
	
	std::string TOML::getUTF8() const
	{
		return get<std::string>();
	}
	
	TOML::LocalTime TOML::getLocalTime() const
	{
		return get<LocalTime>();
	}
	
	Date TOML::getLocalDate() const
	{
		return get<Date>();
	}
	
	TOML::LocalDateTime TOML::getLocalDateTime() const
	{
		return get<LocalDateTime>();
	}
	
	TOML::OffsetDateTime TOML::getOffsetDateTime() const
	{
		return get<OffsetDateTime>();
	}

	TOML::ArrayView TOML::arrayView() const
	{
		if (not isArray())
		{
			ThrowNotArrayView(path(), type());
		}
		
		return ArrayView{ m_root, &(m_value->as_array()), m_path };
	}

	TOML::TableView TOML::tableView() const
	{
		if (not isTable())
		{
			ThrowNotTableView(path(), type());
		}
		
		return TableView{ m_root, &(m_value->as_table()), m_path };
	}

	String TOML::format() const
	{
		if (not m_value)
		{
			return U"";
		}
		
		return Unicode::FromUTF8(toml::format(*m_value, toml::spec::default_version()));
	}

	std::string TOML::formatUTF8() const
	{
		if (not m_value)
		{
			return{};
		}
		
		return toml::format(*m_value, toml::spec::default_version());
	}

	bool operator ==(const TOML& lhs, const TOML& rhs)
	{
		if (not lhs && not rhs)
		{
			return true;
		}
		
		if ((not lhs) || (not rhs))
		{
			return false;
		}
		
		return (*lhs.m_value == *rhs.m_value);
	}

	TOML TOML::Parse(std::string content, const FilePathView pathHint)
	{
		std::istringstream iss{ std::move(content) };
		
		if (auto parseResult = toml::try_parse<toml::ordered_type_config>(iss, MakeParseName(pathHint), toml::spec::default_version()))
		{
			return FromRoot(std::make_shared<Value>(std::move(parseResult.unwrap())));
		}
		
		return{};
	}

	TOML TOML::Parse(const StringView content, const FilePathView pathHint)
	{
		return Parse(Unicode::ToUTF8(content), pathHint);
	}

	Result<TOML, Array<TOMLParseErrorReason>> TOML::ParseResult(std::string content, const FilePathView pathHint)
	{
		std::istringstream iss{ std::move(content) };
		
		if (auto parseResult = toml::try_parse<toml::ordered_type_config>(iss, MakeParseName(pathHint), toml::spec::default_version()))
		{
			return FromRoot(std::make_shared<Value>(std::move(parseResult.unwrap())));
		}
		else
		{
			return Err{ ToParseErrorReasons(parseResult.as_err()) };
		}
	}

	Result<TOML, Array<TOMLParseErrorReason>> TOML::ParseResult(const StringView content, const FilePathView pathHint)
	{
		return ParseResult(Unicode::ToUTF8(content), pathHint);
	}

	TOML TOML::Load(const FilePathView path)
	{
		std::string content;
		{
			BinaryFileReader reader{ path };
			
			if (not ReadTextFile(reader, content))
			{
				return{};
			}
		}

		return Parse(std::move(content), path);
	}

	TOML TOML::Load(std::unique_ptr<IReader> reader)
	{
		if (not reader)
		{
			return{};
		}
		
		std::string content;
		{
			if (not ReadTextFile(*reader, content))
			{
				return{};
			}
			
			reader.reset();
		}
		
		return Parse(std::move(content));
	}

	Result<TOML, Array<TOMLParseErrorReason>> TOML::LoadResult(const FilePathView path)
	{
		std::string content;
		{
			BinaryFileReader reader{ path };
			
			if (not ReadTextFile(reader, content))
			{
				return Err{ Array<TOMLParseErrorReason>{ TOMLParseErrorReason{ U"Failed to read the file.", { String{ path } }, U"Check if the file exists and is a valid UTF-8 text file." } } };
			}
		}
		
		return ParseResult(std::move(content), path);
	}

	Result<TOML, Array<TOMLParseErrorReason>> TOML::LoadResult(std::unique_ptr<IReader> reader)
	{
		if (not reader)
		{
			return Err{ Array<TOMLParseErrorReason>{ TOMLParseErrorReason{ U"Reader is null.", {}, U"Provide a valid reader." } } };
		}

		std::string content;
		{
			if (not ReadTextFile(*reader, content))
			{
				return Err{ Array<TOMLParseErrorReason>{ TOMLParseErrorReason{ U"Failed to read the file.", {}, U"Check if the file is a valid UTF-8 text file." } } };
			}
			
			reader.reset();
		}
		return ParseResult(std::move(content));
	}

	TOML TOML::Invalid()
	{
		return{};
	}

	void Formatter(FormatData& formatData, const TOML& value)
	{
		formatData.string.append(value.format());
	}

	TOML::TOML(std::shared_ptr<const Value> root, const Value* value, String path)
		: m_root{ std::move(root) }
		, m_value{ value }
		, m_path{ std::move(path) } {}

	TOML TOML::FromRoot(std::shared_ptr<const Value> root)
	{
		const Value* value = root.get();
		return TOML{ std::move(root), value, U"$" };
	}

	TOML TOML::InvalidWithPath(String path) const
	{
		return TOML{ m_root, nullptr, std::move(path) };
	}

	Date TOML::ToDate(const toml::local_date& date) noexcept
	{
		return Date{ static_cast<int32>(date.year), (static_cast<int32>(date.month) + 1), static_cast<int32>(date.day) };
	}

	void TOML::ThrowTOMLGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value)
	{
		throw Error{ U"TOML::get<{}>() failed at {}. Actual type: {}, value: \"{}\""_fmt(Demangle(type), path, actualType, MakeShortValue(value)) };
	}

	void TOML::ThrowTOMLParsedGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value)
	{
		throw Error{ U"TOML::getParsed<{}>() failed at {}. Actual type: {}, value: \"{}\""_fmt(Demangle(type), path, actualType, MakeShortValue(value)) };
	}

	void TOML::ThrowNotArray(const char* type, const StringView path, const TOMLValueType actualType)
	{
		throw Error{ U"TOML::getArray<{}>(): value at {} is not an array. Actual type: {}"_fmt(Demangle(type), path, actualType) };
	}

	void TOML::ThrowArrayElementGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value)
	{
		throw Error{ U"TOML::getArray<{}>(): element conversion failed at {}. Actual type: {}, value: \"{}\""_fmt(Demangle(type), path, actualType, MakeShortValue(value)) };
	}

	String TOML::MakeIndexPath(const StringView parent, const size_t index)
	{
		return U"{}[{}]"_fmt(parent, index);
	}

	TOML::ArrayView::Iterator::Iterator(std::shared_ptr<const Value> root, const Value::array_type* array, const size_t index, String path)
		: m_root{ std::move(root) }
		, m_array{ array }
		, m_index{ index }
		, m_path{ std::move(path) } {}

	TOML::ArrayView::Iterator& TOML::ArrayView::Iterator::operator ++()
	{
		++m_index;
		return *this;
	}

	TOML::ArrayView::Iterator TOML::ArrayView::Iterator::operator ++(int)
	{
		Iterator old = *this;
		++(*this);
		return old;
	}

	[[nodiscard]]
	TOML TOML::ArrayView::Iterator::operator *() const
	{
		return TOML{ m_root, &((*m_array)[m_index]), MakeIndexPath(m_path, m_index) };
	}

	bool operator ==(const TOML::ArrayView::Iterator& lhs, const TOML::ArrayView::Iterator& rhs) noexcept
	{
		return ((lhs.m_array == rhs.m_array) && (lhs.m_index == rhs.m_index));
	}

	TOML::ArrayView::ArrayView(std::shared_ptr<const Value> root, const Value::array_type* array, String path)
		: m_root{ std::move(root) }
		, m_array{ array }
		, m_path{ std::move(path) } {}

	TOML::ArrayView::Iterator TOML::ArrayView::begin() const
	{
		return Iterator{ m_root, m_array, 0, m_path };
	}
	
	TOML::ArrayView::Iterator TOML::ArrayView::end() const
	{
		return Iterator{ m_root, m_array, (m_array ? m_array->size() : 0), m_path };
	}
	
	size_t TOML::ArrayView::size() const noexcept
	{
		return (m_array ? m_array->size() : 0);
	}
	
	bool TOML::ArrayView::isEmpty() const noexcept
	{
		return (size() == 0);
	}

	TOML::TableView::Iterator::Iterator(std::shared_ptr<const Value> root, TableIterator it, String path)
		: m_root{ std::move(root) }
		, m_it{ it }
		, m_path{ std::move(path) } {}

	TOML::TableView::Iterator& TOML::TableView::Iterator::operator ++()
	{
		++m_it; return *this;
	}

	TOML::TableView::Iterator TOML::TableView::Iterator::operator ++(int)
	{
		Iterator old = *this;
		++(*this);
		return old;
	}

	TOML::TableMember TOML::TableView::Iterator::operator *() const
	{
		const String key = Unicode::FromUTF8(m_it->first);
		return TableMember{ key, TOML{ m_root, &(m_it->second), MakeKeyPath(m_path, key) } };
	}

	bool operator ==(const TOML::TableView::Iterator& lhs, const TOML::TableView::Iterator& rhs) noexcept
	{
		return (lhs.m_it == rhs.m_it);
	}

	TOML::TableView::TableView(std::shared_ptr<const Value> root, const Value::table_type* table, String path)
		: m_root{ std::move(root) }
		, m_table{ table }
		, m_path{ std::move(path) } {}

	TOML::TableView::Iterator TOML::TableView::begin() const
	{
		return Iterator{ m_root, m_table->begin(), m_path };
	}
	
	TOML::TableView::Iterator TOML::TableView::end() const
	{
		return Iterator{ m_root, m_table->end(), m_path };
	}
	
	size_t TOML::TableView::size() const noexcept
	{
		return (m_table ? m_table->size() : 0);
	}
	
	bool TOML::TableView::isEmpty() const noexcept
	{
		return (size() == 0);
	}
}
