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
# include "Common.hpp"
# include "String.hpp"
# include "Array.hpp"
# include "DateTime.hpp"
# include "Result.hpp"
# include "IReader.hpp"
# include "TOMLValueType.hpp"
# include "TOMLParseErrorReason.hpp"
SIV3D_DISABLE_MSVC_WARNINGS_PUSH(5311)
# include <ThirdParty/toml11/toml.hpp>
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	TOML
	//
	////////////////////////////////////////////////////////////////

	class TOML
	{
	public:
		
		using Value = toml::ordered_value;

		class ArrayView;
		class TableView;
		struct TableMember;

		struct LocalTime
		{
			uint8  hour = 0;
			uint8  minute = 0;
			uint8  second = 0;
			uint16 millisecond = 0;
			uint16 microsecond = 0;
			uint16 nanosecond = 0;

			[[nodiscard]]
			DateTime toDateTime() const noexcept;

			[[nodiscard]]
			DateTime toDateTimeTruncatedToMilliseconds() const noexcept;

			[[nodiscard]]
			static LocalTime FromTOML(const toml::local_time& localTime) noexcept;
		};

		struct LocalDateTime
		{
			Date date;
			LocalTime time;

			[[nodiscard]]
			DateTime toDateTime() const noexcept;

			[[nodiscard]]
			DateTime toDateTimeTruncatedToMilliseconds() const noexcept;

			[[nodiscard]]
			static LocalDateTime FromTOML(const toml::local_datetime& localDateTime) noexcept;
		};

		struct OffsetDateTime
		{
			LocalDateTime localDateTime;
			int32 offsetMinutes = 0;

			[[nodiscard]]
			static OffsetDateTime FromTOML(const toml::offset_datetime& offsetDateTime) noexcept;
		};

		[[nodiscard]]
		TOML() = default;

		[[nodiscard]]
		bool isValid() const noexcept;
		
		[[nodiscard]]
		explicit operator bool() const noexcept;
		
		[[nodiscard]]
		const String& path() const noexcept;

		[[nodiscard]]
		TOMLValueType type() const noexcept;

		[[nodiscard]]
		TOMLValueType getType() const noexcept;
		
		[[nodiscard]]
		bool hasValue() const noexcept;
		
		[[nodiscard]]
		bool isInvalid() const noexcept;
		
		[[nodiscard]]
		bool isEmpty() const noexcept;

		[[nodiscard]]
		bool isBool() const noexcept;
		
		[[nodiscard]]
		bool isInt() const noexcept;
		
		[[nodiscard]]
		bool isFloat() const noexcept;
		
		[[nodiscard]]
		bool isString() const noexcept;
		
		[[nodiscard]]
		bool isLocalDate() const noexcept;
		
		[[nodiscard]]
		bool isLocalTime() const noexcept;
		
		[[nodiscard]]
		bool isLocalDateTime() const noexcept;
		
		[[nodiscard]]
		bool isOffsetDateTime() const noexcept;
		
		[[nodiscard]]
		bool isArray() const noexcept;
		
		[[nodiscard]]
		bool isTable() const noexcept;
		
		[[nodiscard]]
		bool is(const TOMLValueType valueType) const noexcept;

		[[nodiscard]]
		bool contains(const std::string& key) const;

		[[nodiscard]]
		bool contains(const StringView key) const;

		[[nodiscard]]
		size_t size() const noexcept;

		[[nodiscard]]
		bool isEmptyArray() const noexcept;

		[[nodiscard]]
		bool isEmptyTable() const noexcept;

		[[nodiscard]]
		TOML operator [](const std::string& key) const;

		[[nodiscard]]
		TOML operator [](const StringView key) const;

		[[nodiscard]]
		TOML operator [](const size_t index) const;

		[[nodiscard]]
		TOML at(const std::string& key) const;

		[[nodiscard]]
		TOML at(const StringView key) const;

		[[nodiscard]]
		TOML at(const size_t index) const;

		[[nodiscard]]
		String getString() const;

		[[nodiscard]]
		std::string getUTF8() const;

		[[nodiscard]]
		LocalTime getLocalTime() const;

		[[nodiscard]]
		Date getLocalDate() const;

		[[nodiscard]]
		LocalDateTime getLocalDateTime() const;

		[[nodiscard]]
		OffsetDateTime getOffsetDateTime() const;

		template <class Type>
		[[nodiscard]]
		Optional<Type> getOpt() const;

		template <class Type>
		[[nodiscard]]
		Type get() const;

		template <class Type, class U>
		[[nodiscard]]
		Type getOr(U&& defaultValue) const;

		template <class Type>
		[[nodiscard]]
		Optional<Type> getParsedOpt() const;

		template <class Type>
		[[nodiscard]]
		Type getParsed() const;

		template <class Type, class U>
		[[nodiscard]]
		Type getParsedOr(U&& defaultValue) const;

		template <class Type>
		[[nodiscard]]
		Optional<Array<Type>> getArrayOpt() const;

		template <class Type>
		[[nodiscard]]
		Array<Type> getArray() const;

		[[nodiscard]]
		ArrayView arrayView() const;

		[[nodiscard]]
		TableView tableView() const;

		[[nodiscard]]
		String format() const;

		[[nodiscard]]
		std::string formatUTF8() const;

		[[nodiscard]]
		friend bool operator ==(const TOML& lhs, const TOML& rhs);

		////////////////////////////////////////////////////////////////
		//
		//	Parse
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static TOML Parse(std::string content, const FilePathView pathHint = U"");

		[[nodiscard]]
		static TOML Parse(const StringView content, const FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	ParseResult
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> ParseResult(std::string content, const FilePathView pathHint = U"");

		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> ParseResult(const StringView content, const FilePathView pathHint = U"");

		////////////////////////////////////////////////////////////////
		//
		//	Load
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static TOML Load(const FilePathView path);

		[[nodiscard]]
		static TOML Load(std::unique_ptr<IReader> reader);

		template <ReaderObject Reader>
		[[nodiscard]]
		static TOML Load(Reader&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	LoadResult
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> LoadResult(const FilePathView path);

		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> LoadResult(std::unique_ptr<IReader> reader);

		template <ReaderObject Reader>
		[[nodiscard]]
		static Result<TOML, Array<TOMLParseErrorReason>> LoadResult(Reader&& reader);

		////////////////////////////////////////////////////////////////
		//
		//	Invalid
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static TOML Invalid();

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const TOML& value);

	private:
		
		template <class>
		static constexpr bool AlwaysFalse = false;

		std::shared_ptr<const Value> m_root;
		
		const Value* m_value = nullptr;
		
		String m_path = U"$";

		[[nodiscard]]
		TOML(std::shared_ptr<const Value> root, const Value* value, String path);

		[[nodiscard]]
		static TOML FromRoot(std::shared_ptr<const Value> root);

		[[nodiscard]]
		TOML InvalidWithPath(String path) const;

		[[nodiscard]]
		static Date ToDate(const toml::local_date& date) noexcept;

		[[nodiscard]]
		static String MakeIndexPath(const StringView parent, const size_t index);

		[[noreturn]]
		static void ThrowTOMLGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value);
	
		[[noreturn]]
		static void ThrowTOMLParsedGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value);
		
		[[noreturn]]
		static void ThrowNotArray(const char* type, const StringView path, const TOMLValueType actualType);

		[[noreturn]]
		static void ThrowArrayElementGetError(const char* type, const StringView path, const TOMLValueType actualType, const String& value);
	};

	struct TOML::TableMember
	{
		String name;
		
		TOML value;
	};

	class TOML::ArrayView
	{
	public:
		
		class Iterator
		{
		public:
			
			using difference_type = std::ptrdiff_t;
			using value_type = TOML;
			using iterator_category = std::forward_iterator_tag;

			[[nodiscard]]
			Iterator() = default;

			[[nodiscard]]
			Iterator(std::shared_ptr<const Value> root, const Value::array_type* array, const size_t index, String path);

			Iterator& operator ++();
			
			Iterator operator ++(int);

			[[nodiscard]]
			TOML operator *() const;

			[[nodiscard]]
			friend bool operator ==(const Iterator& lhs, const Iterator& rhs) noexcept;

		private:
			
			std::shared_ptr<const Value> m_root;
			
			const Value::array_type* m_array = nullptr;
			
			size_t m_index = 0;
			
			String m_path;
		};

		[[nodiscard]]
		ArrayView() = default;

		[[nodiscard]]
		ArrayView(std::shared_ptr<const Value> root, const Value::array_type* array, String path);

		[[nodiscard]]
		Iterator begin() const;

		[[nodiscard]]
		Iterator end() const;

		[[nodiscard]]
		size_t size() const noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

	private:
		
		std::shared_ptr<const Value> m_root;
		
		const Value::array_type* m_array = nullptr;
		
		String m_path;
	};

	class TOML::TableView
	{
	public:

		class Iterator
		{
		public:
		
			using difference_type = std::ptrdiff_t;
			using value_type = TableMember;
			using iterator_category = std::forward_iterator_tag;
			using TableIterator = Value::table_type::const_iterator;

			[[nodiscard]]
			Iterator() = default;

			[[nodiscard]]
			Iterator(std::shared_ptr<const Value> root, TableIterator it, String path);

			Iterator& operator ++();

			Iterator operator ++(int);

			[[nodiscard]] TableMember operator *() const;

			[[nodiscard]]
			friend bool operator ==(const Iterator& lhs, const Iterator& rhs) noexcept;

		private:
			
			std::shared_ptr<const Value> m_root;
			
			TableIterator m_it{};
			
			String m_path;
		};

		[[nodiscard]]
		TableView() = default;

		[[nodiscard]]
		TableView(std::shared_ptr<const Value> root, const Value::table_type* table, String path);

		[[nodiscard]]
		Iterator begin() const;
		
		[[nodiscard]]
		Iterator end() const;
		
		[[nodiscard]]
		size_t size() const noexcept;
		
		[[nodiscard]]
		bool isEmpty() const noexcept;

	private:
		
		std::shared_ptr<const Value> m_root;
		
		const Value::table_type* m_table = nullptr;
		
		String m_path;
	};
}

# include "detail/TOML.ipp"
