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

# include <Siv3D/CSV.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace
	{
		////////////////////////////////////////////////////////////////
		//
		//	CSVParserState
		//
		////////////////////////////////////////////////////////////////

		enum class CSVParserState : uint8
		{
			StartField,
			InUnquotedField,
			InQuotedField,
			AfterQuote,
		};

		////////////////////////////////////////////////////////////////
		//
		//	Helpers
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		static bool HasUTF8BOM(const std::string_view content) noexcept
		{
			return ((3 <= content.size())
				&& (static_cast<uint8>(content[0]) == 0xEF)
				&& (static_cast<uint8>(content[1]) == 0xBB)
				&& (static_cast<uint8>(content[2]) == 0xBF));
		}

		[[nodiscard]]
		static bool HasUTF16LEBOM(const std::string_view content) noexcept
		{
			return ((2 <= content.size())
				&& (static_cast<uint8>(content[0]) == 0xFF)
				&& (static_cast<uint8>(content[1]) == 0xFE));
		}

		[[nodiscard]]
		static bool HasUTF16BEBOM(const std::string_view content) noexcept
		{
			return ((2 <= content.size())
				&& (static_cast<uint8>(content[0]) == 0xFE)
				&& (static_cast<uint8>(content[1]) == 0xFF));
		}

		[[nodiscard]]
		static String MakeLocation(const FilePathView pathHint, const size_t line, const size_t character)
		{
			if (pathHint)
			{
				return Format(pathHint, U":", line, U":", character);
			}

			return Format(U"line ", line, U", character ", character);
		}

		[[nodiscard]]
		static CSVParseErrorReason MakeError(
			const CSVParseErrorCode code,
			String title,
			const FilePathView pathHint,
			const size_t line,
			const size_t character,
			const size_t row,
			const size_t column,
			String hint = U"")
		{
			CSVParseErrorReason reason;
			reason.code = code;
			reason.title = std::move(title);
			reason.locations.push_back(MakeLocation(pathHint, line, character));
			reason.hint = std::move(hint);
			reason.line = line;
			reason.character = character;
			reason.row = row;
			reason.column = column;
			return reason;
		}

		[[nodiscard]]
		static Array<CSVParseErrorReason> MakeSingleError(
			const CSVParseErrorCode code,
			String title,
			const FilePathView pathHint,
			const size_t line,
			const size_t character,
			const size_t row,
			const size_t column,
			String hint = U"")
		{
			return Array<CSVParseErrorReason>{ MakeError(code, std::move(title), pathHint, line, character, row, column, std::move(hint)) };
		}

		[[nodiscard]]
		static Array<CSVParseErrorReason> MakeInvalidReaderError(const FilePathView pathHint)
		{
			return MakeSingleError(
				CSVParseErrorCode::InvalidReader,
				U"Invalid CSV reader",
				pathHint,
				1,
				1,
				0,
				0,
				U"Check that the file exists and the reader is open.");
		}

		[[nodiscard]]
		static Array<CSVParseErrorReason> MakeInvalidEncodingError(const FilePathView pathHint)
		{
			return MakeSingleError(
				CSVParseErrorCode::InvalidEncoding,
				U"Invalid CSV text encoding",
				pathHint,
				1,
				1,
				0,
				0,
				U"CSV::Load() expects UTF-8, UTF-8 with BOM, UTF-16LE with BOM, or UTF-16BE with BOM.");
		}

		[[nodiscard]]
		static String DecodeCSVText(std::string content, const CSVReadOptions& options, const FilePathView pathHint, Array<CSVParseErrorReason>& errors)
		{
			if (content.empty())
			{
				return String{};
			}

			if (HasUTF16LEBOM(content) || HasUTF16BEBOM(content))
			{
				const bool isBigEndian = HasUTF16BEBOM(content);
				const size_t offset = 2;

				if (((content.size() - offset) % 2) != 0)
				{
					errors = MakeInvalidEncodingError(pathHint);
					return String{};
				}

				std::u16string utf16;
				utf16.resize((content.size() - offset) / 2);

				for (size_t i = 0; i < utf16.size(); ++i)
				{
					const uint8 b0 = static_cast<uint8>(content[offset + i * 2]);
					const uint8 b1 = static_cast<uint8>(content[offset + i * 2 + 1]);

					if (isBigEndian)
					{
						utf16[i] = static_cast<char16>((static_cast<uint16>(b0) << 8) | static_cast<uint16>(b1));
					}
					else
					{
						utf16[i] = static_cast<char16>(static_cast<uint16>(b0) | (static_cast<uint16>(b1) << 8));
					}
				}

				const String result = Unicode::FromUTF16(utf16);

				if ((not utf16.empty()) && result.isEmpty())
				{
					errors = MakeInvalidEncodingError(pathHint);
				}

				return result;
			}

			std::string_view view = content;

			if (options.skipUTF8BOM && HasUTF8BOM(view))
			{
				view.remove_prefix(3);
			}

			const String result = Unicode::FromUTF8(view);

			if ((not view.empty()) && result.isEmpty())
			{
				errors = MakeInvalidEncodingError(pathHint);
			}

			return result;
		}

		[[nodiscard]]
		static Result<CSV::Table, Array<CSVParseErrorReason>> ParseCSVTable(StringView content, const CSVReadOptions& options, const FilePathView pathHint)
		{
			if (options.skipUTF8BOM && (not content.empty()) && (content.front() == U'\uFEFF'))
			{
				content.remove_prefix(1);
			}

			CSV::Table table;
			CSV::Row currentRow;
			String currentField;
			Array<size_t> recordStartLines;

			CSVParserState state = CSVParserState::StartField;
			bool currentRecordHasData = false;

			size_t line = 1;
			size_t character = 1;
			size_t recordStartLine = 1;

			const auto currentRowIndex = [&]() noexcept -> size_t
			{
				return table.size();
			};

			const auto currentColumnIndex = [&]() noexcept -> size_t
			{
				return currentRow.size();
			};

			const auto appendField = [&]()
			{
				currentRow.push_back(std::move(currentField));
				currentField.clear();
			};

			const auto finishRecord = [&]()
			{
				appendField();
				table.push_back(std::move(currentRow));
				currentRow.clear();
				recordStartLines.push_back(recordStartLine);
				currentRecordHasData = false;
			};

			const auto advanceNormal = [&]()
			{
				++character;
			};

			const auto advanceNewLine = [&]()
			{
				++line;
				character = 1;
			};

			size_t i = 0;

			while (i < content.size())
			{
				const char32 ch = content[i];

				if (state == CSVParserState::InQuotedField)
				{
					if (ch == U'"')
					{
						state = CSVParserState::AfterQuote;
						++i;
						advanceNormal();
						continue;
					}
					else if (ch == U'\r')
					{
						currentField.push_back(ch);

						if (((i + 1) < content.size()) && (content[i + 1] == U'\n'))
						{
							currentField.push_back(U'\n');
							i += 2;
							advanceNewLine();
							continue;
						}

						if (not options.allowLoneCR)
						{
							return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
								CSVParseErrorCode::BareCR,
								U"Bare CR is not allowed in CSV input",
								pathHint,
								line,
								character,
								currentRowIndex(),
								currentColumnIndex(),
								U"Use CRLF as a record separator, or enable CSVReadOptions::allowLoneCR.") };
						}

						++i;
						advanceNewLine();
						continue;
					}
					else if (ch == U'\n')
					{
						if (not options.allowLoneLF)
						{
							return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
								CSVParseErrorCode::BareLF,
								U"Bare LF is not allowed in CSV input",
								pathHint,
								line,
								character,
								currentRowIndex(),
								currentColumnIndex(),
								U"Use CRLF as a record separator, or enable CSVReadOptions::allowLoneLF.") };
						}

						currentField.push_back(ch);
						++i;
						advanceNewLine();
						continue;
					}
					else
					{
						currentField.push_back(ch);
						++i;
						advanceNormal();
						continue;
					}
				}

				if ((ch == U'\r') || (ch == U'\n'))
				{
					if (ch == U'\r')
					{
						if (((i + 1) < content.size()) && (content[i + 1] == U'\n'))
						{
							finishRecord();
							state = CSVParserState::StartField;
							i += 2;
							advanceNewLine();
							recordStartLine = line;
							continue;
						}

						if (not options.allowLoneCR)
						{
							return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
								CSVParseErrorCode::BareCR,
								U"Bare CR is not allowed in CSV input",
								pathHint,
								line,
								character,
								currentRowIndex(),
								currentColumnIndex(),
								U"Use CRLF as a record separator, or enable CSVReadOptions::allowLoneCR.") };
						}

						finishRecord();
						state = CSVParserState::StartField;
						++i;
						advanceNewLine();
						recordStartLine = line;
						continue;
					}

					if (not options.allowLoneLF)
					{
						return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
							CSVParseErrorCode::BareLF,
							U"Bare LF is not allowed in CSV input",
							pathHint,
							line,
							character,
							currentRowIndex(),
							currentColumnIndex(),
							U"Use CRLF as a record separator, or enable CSVReadOptions::allowLoneLF.") };
					}

					finishRecord();
					state = CSVParserState::StartField;
					++i;
					advanceNewLine();
					recordStartLine = line;
					continue;
				}

				switch (state)
				{
				case CSVParserState::StartField:
					if (ch == U',')
					{
						currentRow.push_back(String{});
						currentRecordHasData = true;
					}
					else if (ch == U'"')
					{
						state = CSVParserState::InQuotedField;
						currentRecordHasData = true;
					}
					else
					{
						currentField.push_back(ch);
						state = CSVParserState::InUnquotedField;
						currentRecordHasData = true;
					}
					break;
				case CSVParserState::InUnquotedField:
					if (ch == U',')
					{
						appendField();
						state = CSVParserState::StartField;
						currentRecordHasData = true;
					}
					else if (ch == U'"')
					{
						return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
							CSVParseErrorCode::UnexpectedQuoteInUnquotedField,
							U"Unexpected double quote in unquoted CSV field",
							pathHint,
							line,
							character,
							currentRowIndex(),
							currentColumnIndex(),
							U"If a field contains a double quote, enclose the entire field in double quotes and escape the quote as two double quotes.") };
					}
					else
					{
						currentField.push_back(ch);
					}
					break;
				case CSVParserState::AfterQuote:
					if (ch == U'"')
					{
						currentField.push_back(U'"');
						state = CSVParserState::InQuotedField;
					}
					else if (ch == U',')
					{
						appendField();
						state = CSVParserState::StartField;
						currentRecordHasData = true;
					}
					else
					{
						return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
							CSVParseErrorCode::UnexpectedCharacterAfterClosingQuote,
							U"Unexpected character after closing double quote in CSV field",
							pathHint,
							line,
							character,
							currentRowIndex(),
							currentColumnIndex(),
							U"A closing double quote must be followed by a comma, CRLF, LF if allowed, or end of file.") };
					}
					break;
				case CSVParserState::InQuotedField:
					break;
				}

				++i;
				advanceNormal();
			}

			switch (state)
			{
			case CSVParserState::StartField:
				if (currentRecordHasData)
				{
					finishRecord();
				}
				break;
			case CSVParserState::InUnquotedField:
			case CSVParserState::AfterQuote:
				finishRecord();
				break;
			case CSVParserState::InQuotedField:
				return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, MakeSingleError(
					CSVParseErrorCode::UnterminatedQuotedField,
					U"Unterminated quoted CSV field",
					pathHint,
					line,
					character,
					currentRowIndex(),
					currentColumnIndex(),
					U"Close the quoted field with a double quote.") };
			}

			if (options.requireConsistentColumns && (not table.isEmpty()))
			{
				Array<CSVParseErrorReason> errors;
				const size_t expectedColumns = table.front().size();

				for (size_t rowIndex = 1; rowIndex < table.size(); ++rowIndex)
				{
					const size_t actualColumns = table[rowIndex].size();

					if (actualColumns != expectedColumns)
					{
						const size_t rowLine = (rowIndex < recordStartLines.size()) ? recordStartLines[rowIndex] : 1;

						errors.push_back(MakeError(
							CSVParseErrorCode::InconsistentColumns,
							Format(U"Inconsistent number of CSV fields: expected ", expectedColumns, U", but got ", actualColumns),
							pathHint,
							rowLine,
							1,
							rowIndex,
							0,
							U"Disable CSVReadOptions::requireConsistentColumns to allow ragged CSV records."));
					}
				}

				if (errors)
				{
					return Result<CSV::Table, Array<CSVParseErrorReason>>{ unexpect, std::move(errors) };
				}
			}

			return table;
		}

		[[nodiscard]]
		static bool RequiresQuote(const StringView field) noexcept
		{
			for (const char32 ch : field)
			{
				if ((ch == U',') || (ch == U'"') || (ch == U'\r') || (ch == U'\n'))
				{
					return true;
				}
			}

			return false;
		}

		static void AppendField(String& result, const StringView field, const CSVWriteOptions& options)
		{
			const bool quote = (options.alwaysQuote || RequiresQuote(field));

			if (not quote)
			{
				result.append(field);
				return;
			}

			result.push_back(U'"');

			for (const char32 ch : field)
			{
				if (ch == U'"')
				{
					result.append(U"\"\"");
				}
				else
				{
					result.push_back(ch);
				}
			}

			result.push_back(U'"');
		}

		[[nodiscard]]
		static Result<std::string, Array<CSVParseErrorReason>> ReadAll(IReader& reader, const FilePathView pathHint)
		{
			if (not reader)
			{
				return Result<std::string, Array<CSVParseErrorReason>>{ unexpect, MakeInvalidReaderError(pathHint) };
			}

			const int64 size = reader.size();
			const int64 pos = reader.getPos();

			if ((size < 0) || (pos < 0) || (size < pos))
			{
				return Result<std::string, Array<CSVParseErrorReason>>{ unexpect, MakeInvalidReaderError(pathHint) };
			}

			const int64 remaining = (size - pos);
			std::string content;
			content.resize(static_cast<size_t>(remaining));

			if (remaining == 0)
			{
				return content;
			}

			const int64 readSize = reader.read(content.data(), remaining);

			if (readSize != remaining)
			{
				return Result<std::string, Array<CSVParseErrorReason>>{ unexpect, MakeInvalidReaderError(pathHint) };
			}

			return content;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	CSV::CSV(const FilePathView path, const CSVReadOptions& options)
	{
		*this = Load(path, options);
	}

	CSV::CSV(std::unique_ptr<IReader> reader, const CSVReadOptions& options)
	{
		*this = Load(std::move(reader), options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isValid, isInvalid
	//
	////////////////////////////////////////////////////////////////

	bool CSV::isValid() const noexcept
	{
		return m_isValid;
	}

	bool CSV::isInvalid() const noexcept
	{
		return (not m_isValid);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	CSV::operator bool() const noexcept
	{
		return isValid();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool CSV::isEmpty() const noexcept
	{
		return m_data.isEmpty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	rows
	//
	////////////////////////////////////////////////////////////////

	size_t CSV::rows() const noexcept
	{
		return m_data.size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	columns
	//
	////////////////////////////////////////////////////////////////

	size_t CSV::columns(const size_t row) const noexcept
	{
		if (m_data.size() <= row)
		{
			return 0;
		}

		return m_data[row].size();
	}

	////////////////////////////////////////////////////////////////
	//
	//	inBounds
	//
	////////////////////////////////////////////////////////////////

	bool CSV::inBounds(const size_t row, const size_t column) const noexcept
	{
		return ((row < m_data.size()) && (column < m_data[row].size()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	data, getData
	//
	////////////////////////////////////////////////////////////////

	const CSV::Table& CSV::data() const noexcept
	{
		return m_data;
	}

	CSV::Table& CSV::data() noexcept
	{
		return m_data;
	}

	const CSV::Table& CSV::getData() const noexcept
	{
		return m_data;
	}

	CSV::Table& CSV::getData() noexcept
	{
		return m_data;
	}

	////////////////////////////////////////////////////////////////
	//
	//	row, getRow, operator []
	//
	////////////////////////////////////////////////////////////////

	const CSV::Row& CSV::row(const size_t row) const
	{
		return m_data[row];
	}

	CSV::Row& CSV::row(const size_t row)
	{
		return m_data[row];
	}

	const CSV::Row& CSV::getRow(const size_t row) const
	{
		return m_data[row];
	}

	CSV::Row& CSV::getRow(const size_t row)
	{
		return m_data[row];
	}

	const CSV::Row& CSV::operator [](const size_t row) const
	{
		return m_data[row];
	}

	CSV::Row& CSV::operator [](const size_t row)
	{
		return m_data[row];
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void CSV::clear() noexcept
	{
		m_data.clear();
		m_isValid = true;
		m_isAtRowHead = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	addRow
	//
	////////////////////////////////////////////////////////////////

	void CSV::addRow(Row row)
	{
		m_data.push_back(std::move(row));
		m_isValid = true;
		m_isAtRowHead = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	write
	//
	////////////////////////////////////////////////////////////////

	void CSV::write(const String& field)
	{
		if (m_isAtRowHead)
		{
			m_data.emplace_back();
			m_isAtRowHead = false;
		}

		m_data.back().push_back(field);
		m_isValid = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	newLine
	//
	////////////////////////////////////////////////////////////////

	void CSV::newLine()
	{
		if (m_isAtRowHead)
		{
			m_data.emplace_back();
		}

		m_isAtRowHead = true;
		m_isValid = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String CSV::format(const CSVWriteOptions& options) const
	{
		if (not isValid())
		{
			return String{};
		}

		String result;

		if (options.writeUTF8BOM)
		{
			result.push_back(U'\uFEFF');
		}

		for (size_t rowIndex = 0; rowIndex < m_data.size(); ++rowIndex)
		{
			const Row& row = m_data[rowIndex];

			for (size_t columnIndex = 0; columnIndex < row.size(); ++columnIndex)
			{
				if (0 < columnIndex)
				{
					result.push_back(U',');
				}

				AppendField(result, row[columnIndex], options);
			}

			if (options.trailingNewline || ((rowIndex + 1) < m_data.size()))
			{
				result.append(U"\r\n");
			}
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string CSV::formatUTF8(const CSVWriteOptions& options) const
	{
		return Unicode::ToUTF8(format(options));
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool CSV::save(const FilePathView path, const CSVWriteOptions& options) const
	{
		if (not isValid())
		{
			return false;
		}

		BinaryFileWriter writer{ path };

		if (not writer)
		{
			return false;
		}

		const std::string content = formatUTF8(options);

		if (content.empty())
		{
			return true;
		}

		return (writer.write(content.data(), static_cast<int64>(content.size())) == static_cast<int64>(content.size()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	CSV CSV::Parse(std::string content, const CSVReadOptions& options, const FilePathView pathHint)
	{
		if (auto result = ParseResult(std::move(content), options, pathHint))
		{
			return std::move(result.value());
		}

		return Invalid();
	}

	CSV CSV::Parse(const StringView content, const CSVReadOptions& options, const FilePathView pathHint)
	{
		if (auto result = ParseResult(content, options, pathHint))
		{
			return std::move(result.value());
		}

		return Invalid();
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseResult
	//
	////////////////////////////////////////////////////////////////

	Result<CSV, Array<CSVParseErrorReason>> CSV::ParseResult(std::string content, const CSVReadOptions& options, const FilePathView pathHint)
	{
		Array<CSVParseErrorReason> errors;
		const String text = DecodeCSVText(std::move(content), options, pathHint, errors);

		if (errors)
		{
			return Result<CSV, Array<CSVParseErrorReason>>{ unexpect, std::move(errors) };
		}

		return ParseResult(StringView{ text }, options, pathHint);
	}

	Result<CSV, Array<CSVParseErrorReason>> CSV::ParseResult(const StringView content, const CSVReadOptions& options, const FilePathView pathHint)
	{
		if (auto result = ParseCSVTable(content, options, pathHint))
		{
			return CSV{ std::move(result.value()) };
		}
		else
		{
			return Result<CSV, Array<CSVParseErrorReason>>{ unexpect, std::move(result.error()) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	CSV CSV::Load(const FilePathView path, const CSVReadOptions& options)
	{
		if (auto result = LoadResult(path, options))
		{
			return std::move(result.value());
		}

		return Invalid();
	}

	CSV CSV::Load(std::unique_ptr<IReader> reader, const CSVReadOptions& options)
	{
		if (auto result = LoadResult(std::move(reader), options))
		{
			return std::move(result.value());
		}

		return Invalid();
	}

	////////////////////////////////////////////////////////////////
	//
	//	LoadResult
	//
	////////////////////////////////////////////////////////////////

	Result<CSV, Array<CSVParseErrorReason>> CSV::LoadResult(const FilePathView path, const CSVReadOptions& options)
	{
		BinaryFileReader reader{ path };

		if (auto content = ReadAll(reader, path))
		{
			return ParseResult(std::move(content.value()), options, path);
		}
		else
		{
			return Result<CSV, Array<CSVParseErrorReason>>{ unexpect, std::move(content.error()) };
		}
	}

	Result<CSV, Array<CSVParseErrorReason>> CSV::LoadResult(std::unique_ptr<IReader> reader, const CSVReadOptions& options)
	{
		if (not reader)
		{
			return Result<CSV, Array<CSVParseErrorReason>>{ unexpect, MakeInvalidReaderError(U"") };
		}

		if (auto content = ReadAll(*reader, U""))
		{
			return ParseResult(std::move(content.value()), options, U"");
		}
		else
		{
			return Result<CSV, Array<CSVParseErrorReason>>{ unexpect, std::move(content.error()) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	bool CSV::load(const FilePathView path, const CSVReadOptions& options)
	{
		if (auto result = LoadResult(path, options))
		{
			*this = std::move(result.value());
			return true;
		}

		*this = Invalid();
		return false;
	}

	bool CSV::load(std::unique_ptr<IReader> reader, const CSVReadOptions& options)
	{
		if (auto result = LoadResult(std::move(reader), options))
		{
			*this = std::move(result.value());
			return true;
		}

		*this = Invalid();
		return false;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Invalid
	//
	////////////////////////////////////////////////////////////////

	CSV CSV::Invalid()
	{
		CSV csv;
		csv.m_data.clear();
		csv.m_isValid = false;
		csv.m_isAtRowHead = true;
		return csv;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	CSV::CSV(Table data)
		: m_data{ std::move(data) } {}

	Optional<String> CSV::getItem(const size_t row, const size_t column) const
	{
		if (not inBounds(row, column))
		{
			return none;
		}

		return m_data[row][column];
	}
}
