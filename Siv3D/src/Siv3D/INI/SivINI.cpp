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

# include <algorithm>
# include <Siv3D/INI.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include <Siv3D/BinaryFileWriter.hpp>
# include <Siv3D/Format.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/Unicode.hpp>

namespace s3d
{
	namespace
	{
		struct INILine
		{
			String text;
			size_t line = 1;
		};

		[[noreturn]]
		static void ThrowGetSection(const StringView section)
		{
			throw Error{ fmt::format("INI::getSection(): Section `{}` not found", section.toUTF8()) };
		}

		[[nodiscard]]
		static bool HasUTF8BOM(const std::string_view content) noexcept
		{
			return ((3 <= content.size())
				&& (static_cast<uint8>(content[0]) == 0xEF)
				&& (static_cast<uint8>(content[1]) == 0xBB)
				&& (static_cast<uint8>(content[2]) == 0xBF));
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
		static INIParseErrorReason MakeError(
			const INIParseErrorCode code,
			String title,
			const FilePathView pathHint,
			const size_t line,
			const size_t character,
			String section = U"",
			String key = U"",
			String hint = U"")
		{
			INIParseErrorReason reason;
			reason.code = code;
			reason.title = std::move(title);
			reason.locations.push_back(MakeLocation(pathHint, line, character));
			reason.hint = std::move(hint);
			reason.line = line;
			reason.character = character;
			reason.section = std::move(section);
			reason.key = std::move(key);
			return reason;
		}

		[[nodiscard]]
		static Array<INIParseErrorReason> MakeSingleError(
			const INIParseErrorCode code,
			String title,
			const FilePathView pathHint,
			const size_t line,
			const size_t character,
			String section = U"",
			String key = U"",
			String hint = U"")
		{
			return Array<INIParseErrorReason>{ MakeError(code, std::move(title), pathHint, line, character, std::move(section), std::move(key), std::move(hint)) };
		}

		[[nodiscard]]
		static Array<INIParseErrorReason> MakeInvalidReaderError(const FilePathView pathHint)
		{
			return MakeSingleError(
				INIParseErrorCode::InvalidReader,
				U"Invalid INI reader",
				pathHint,
				1,
				1,
				U"",
				U"",
				U"Check that the file exists and the reader is open.");
		}

		[[nodiscard]]
		static Array<INIParseErrorReason> MakeInvalidEncodingError(const FilePathView pathHint)
		{
			return MakeSingleError(
				INIParseErrorCode::InvalidEncoding,
				U"Invalid INI text encoding",
				pathHint,
				1,
				1,
				U"",
				U"",
				U"INI::Load() and INI::Parse(std::string_view) expect UTF-8 text.");
		}

		[[nodiscard]]
		static Result<std::string, Array<INIParseErrorReason>> ReadAll(IReader& reader, const FilePathView pathHint)
		{
			if (not reader)
			{
				return Result<std::string, Array<INIParseErrorReason>>{ unexpect, MakeInvalidReaderError(pathHint) };
			}

			const int64 size = reader.size();
			const int64 pos = reader.getPos();

			if ((size < 0) || (pos < 0) || (size < pos))
			{
				return Result<std::string, Array<INIParseErrorReason>>{ unexpect, MakeInvalidReaderError(pathHint) };
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
				return Result<std::string, Array<INIParseErrorReason>>{ unexpect, MakeInvalidReaderError(pathHint) };
			}

			return content;
		}

		[[nodiscard]]
		static Result<Array<INILine>, Array<INIParseErrorReason>> SplitLines(const StringView content, const INIReadOptions& options, const FilePathView pathHint)
		{
			Array<INILine> lines;
			String currentLine;
			size_t line = 1;
			size_t character = 1;

			for (size_t i = 0; i < content.size(); ++i)
			{
				const char32 ch = content[i];

				if (ch == U'\r')
				{
					if (((i + 1) < content.size()) && (content[i + 1] == U'\n'))
					{
						lines.push_back(INILine{ .text = currentLine, .line = line });
						currentLine.clear();
						++i;
						++line;
						character = 1;
					}
					else if (options.allowLoneCR)
					{
						lines.push_back(INILine{ .text = currentLine, .line = line });
						currentLine.clear();
						++line;
						character = 1;
					}
					else
					{
						return Result<Array<INILine>, Array<INIParseErrorReason>>{ unexpect, MakeSingleError(
							INIParseErrorCode::BareCR,
							U"Bare CR is not allowed in INI text",
							pathHint,
							line,
							character,
							U"",
							U"",
							U"Use LF or CRLF as a line separator, or set INIReadOptions::allowLoneCR to true.") };
					}
				}
				else if (ch == U'\n')
				{
					lines.push_back(INILine{ .text = currentLine, .line = line });
					currentLine.clear();
					++line;
					character = 1;
				}
				else
				{
					currentLine.push_back(ch);
					++character;
				}
			}

			if (currentLine || content.empty())
			{
				if (not content.empty())
				{
					lines.push_back(INILine{ .text = currentLine, .line = line });
				}
			}

			return lines;
		}

		[[nodiscard]]
		static bool IsCommentLine(const StringView line, const INIReadOptions& options) noexcept
		{
			if (line.empty())
			{
				return false;
			}

			const char32 firstChar = line[0];

			return ((options.allowSemicolonComment && (firstChar == U';'))
				|| (options.allowHashComment && (firstChar == U'#')));
		}

		[[nodiscard]]
		static Result<INI, Array<INIParseErrorReason>> ParseINI(const StringView content, const INIReadOptions& options, const FilePathView pathHint)
		{
			StringView view = content;

			if (options.skipUTF8BOM && (not view.empty()) && (view.front() == U'\uFEFF'))
			{
				view.remove_prefix(1);
			}

			auto splitResult = SplitLines(view, options, pathHint);

			if (not splitResult)
			{
				return Result<INI, Array<INIParseErrorReason>>{ unexpect, std::move(splitResult.error()) };
			}

			INI ini;
			String currentSection;
			Array<INIParseErrorReason> errors;

			for (const auto& sourceLine : splitResult.value())
			{
				String line = sourceLine.text;
				line.trim();

				if (line.isEmpty() || IsCommentLine(line, options))
				{
					continue;
				}

				const char32 firstChar = line[0];

				if (firstChar == U'[')
				{
					const auto itRBracket = std::find((line.begin() + 1), line.end(), U']');

					if (itRBracket == line.end())
					{
						return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeSingleError(
							INIParseErrorCode::MissingClosingBracket,
							U"Missing closing bracket in INI section",
							pathHint,
							sourceLine.line,
							1,
							U"",
							U"",
							U"Add `]` to close the section declaration.") };
					}

					const size_t rBracketIndex = static_cast<size_t>(itRBracket - line.begin());
					String sectionName = line.substr(1, (rBracketIndex - 1));

					if (options.trimSectionName)
					{
						sectionName.trim();
					}

					if (sectionName.isEmpty())
					{
						return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeSingleError(
							INIParseErrorCode::EmptySectionName,
							U"INI section name is empty",
							pathHint,
							sourceLine.line,
							1,
							U"",
							U"",
							U"Use properties before any section header for the global section. `[]` is not a valid section name.") };
					}

					String tail = line.substr(rBracketIndex + 1);
					tail.trim();

					if (tail)
					{
						return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeSingleError(
							INIParseErrorCode::UnexpectedCharacterAfterSection,
							U"Unexpected characters after INI section declaration",
							pathHint,
							sourceLine.line,
							(rBracketIndex + 2),
							sectionName,
							U"",
							U"Remove characters after `]`. Inline comments after a section header are not supported.") };
					}

					currentSection = sectionName;

					if (ini.hasSection(sectionName))
					{
						if (options.duplicateSectionPolicy == INIDuplicateSectionPolicy::Error)
						{
							errors.push_back(MakeError(
								INIParseErrorCode::DuplicateSection,
								U"Duplicate INI section",
								pathHint,
								sourceLine.line,
								1,
								sectionName,
								U"",
								U"Use a unique section name, or set INIReadOptions::duplicateSectionPolicy to Merge."));
						}

						continue;
					}

					ini.addSection(sectionName);
					continue;
				}

				const auto itEqual = std::find(line.begin(), line.end(), U'=');

				if (itEqual == line.end())
				{
					return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeSingleError(
						INIParseErrorCode::MissingAssignment,
						U"Missing assignment character in INI property",
						pathHint,
						sourceLine.line,
						1,
						currentSection,
						U"",
						U"Add `=` between the key and the value.") };
				}

				const size_t equalIndex = static_cast<size_t>(itEqual - line.begin());
				String key = line.substr(0, equalIndex);

				if (options.trimKey)
				{
					key.trim();
				}

				if (key.isEmpty())
				{
					return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeSingleError(
						INIParseErrorCode::EmptyKey,
						U"INI key is empty",
						pathHint,
						sourceLine.line,
						1,
						currentSection,
						U"",
						U"Specify a non-empty key before `=`.") };
				}

				String value = line.substr(equalIndex + 1);

				if (options.trimValue)
				{
					value.trim();
				}

				if (ini.hasProperty(currentSection, key))
				{
					if (options.duplicateKeyPolicy == INIDuplicateKeyPolicy::Error)
					{
						errors.push_back(MakeError(
							INIParseErrorCode::DuplicateKey,
							U"Duplicate INI key",
							pathHint,
							sourceLine.line,
							1,
							currentSection,
							key,
							U"Use a unique key, or set INIReadOptions::duplicateKeyPolicy to Overwrite or KeepFirst."));
						continue;
					}
					else if (options.duplicateKeyPolicy == INIDuplicateKeyPolicy::KeepFirst)
					{
						continue;
					}
				}

				ini.addProperty(currentSection, key, value);
			}

			if (errors)
			{
				return Result<INI, Array<INIParseErrorReason>>{ unexpect, std::move(errors) };
			}

			return ini;
		}
	}

	namespace detail
	{
		void ThrowINIGetError(const char* type, const StringView section, const StringView key)
		{
			throw Error{ fmt::format("INI::get<{}>({}, {}) failed", DemangleUTF8(type), section.toUTF8(), key.toUTF8()) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	INI::INI(const FilePathView path, const INIReadOptions& options)
	{
		*this = Load(path, options);
	}

	INI::INI(std::unique_ptr<IReader> reader, const INIReadOptions& options)
	{
		*this = Load(std::move(reader), options);
	}

	////////////////////////////////////////////////////////////////
	//
	//	isValid, isInvalid
	//
	////////////////////////////////////////////////////////////////

	bool INI::isValid() const noexcept
	{
		return m_isValid;
	}

	bool INI::isInvalid() const noexcept
	{
		return (not m_isValid);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	INI::operator bool() const noexcept
	{
		return isValid();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	bool INI::isEmpty() const noexcept
	{
		const size_t sectionCount = m_sections.size();

		if (sectionCount == 0)
		{
			return true;
		}
		else if (sectionCount == 1)
		{
			return (m_sections.front().name.isEmpty() && m_sections.front().items.empty());
		}
		else
		{
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	clear
	//
	////////////////////////////////////////////////////////////////

	void INI::clear()
	{
		m_sections.clear();
		m_sectionIndex.clear();
		m_isValid = true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	sections
	//
	////////////////////////////////////////////////////////////////

	const Array<INISection>& INI::sections() const noexcept
	{
		return m_sections;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasSection
	//
	////////////////////////////////////////////////////////////////

	bool INI::hasSection(const StringView section) const noexcept
	{
		return m_sectionIndex.contains(section);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getSection
	//
	////////////////////////////////////////////////////////////////

	const INISection& INI::getSection(const StringView section) const
	{
		const auto it = m_sectionIndex.find(section);

		if (it == m_sectionIndex.end())
		{
			ThrowGetSection(section);
		}

		return m_sections[it->second];
	}

	INISection& INI::getSection(const StringView section)
	{
		const auto it = m_sectionIndex.find(section);

		if (it == m_sectionIndex.end())
		{
			ThrowGetSection(section);
		}

		return m_sections[it->second];
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlobalSection
	//
	////////////////////////////////////////////////////////////////

	bool INI::hasGlobalSection() const noexcept
	{
		return m_sectionIndex.contains(GlobalSection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlobalSection
	//
	////////////////////////////////////////////////////////////////

	const INISection& INI::getGlobalSection() const
	{
		return getSection(GlobalSection);
	}

	INISection& INI::getGlobalSection()
	{
		return getSection(GlobalSection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
	//
	////////////////////////////////////////////////////////////////

	const INISection& INI::operator [](const StringView section) const
	{
		return getSection(section);
	}

	INISection& INI::operator [](const StringView section)
	{
		return getSection(section);
	}

	const INISection& INI::operator []() const
	{
		return getSection(GlobalSection);
	}

	INISection& INI::operator []()
	{
		return getSection(GlobalSection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	bool INI::hasProperty(const StringView section, const StringView key) const noexcept
	{
		const auto it = m_sectionIndex.find(section);

		if (it == m_sectionIndex.end())
		{
			return false;
		}

		return m_sections[it->second].hasProperty(key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasGlobalProperty
	//
	////////////////////////////////////////////////////////////////

	bool INI::hasGlobalProperty(const StringView key) const noexcept
	{
		return hasProperty(GlobalSection, key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addSection
	//
	////////////////////////////////////////////////////////////////

	void INI::addSection(const StringView section)
	{
		if (m_sectionIndex.contains(section))
		{
			return;
		}

		m_sections.push_back(INISection{ .name = String{ section }, .items = {} });
		m_sectionIndex.emplace(section, (m_sections.size() - 1));
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeSection
	//
	////////////////////////////////////////////////////////////////

	void INI::removeSection(const StringView section)
	{
		const auto it = m_sectionIndex.find(section);
		
		if (it == m_sectionIndex.end())
		{
			return;
		}

		const size_t sectionIndex = it->second;

		m_sections.erase(m_sections.begin() + sectionIndex);

		m_sectionIndex.erase(it);

		for (auto& [key, index] : m_sectionIndex)
		{
			if (sectionIndex < index)
			{
				--index;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeGlobalSection
	//
	////////////////////////////////////////////////////////////////

	void INI::removeGlobalSection()
	{
		removeSection(GlobalSection);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addProperty
	//
	////////////////////////////////////////////////////////////////

	void INI::addProperty(const StringView section, const StringView key, const String value)
	{
		const auto it = m_sectionIndex.find(section);

		if (it == m_sectionIndex.end())
		{
			addSection(section);
		}

		m_sections[m_sectionIndex[section]].addProperty(key, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	addGlobalProperty
	//
	////////////////////////////////////////////////////////////////

	void INI::addGlobalProperty(const StringView key, const String value)
	{
		addProperty(GlobalSection, key, value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeProperty
	//
	////////////////////////////////////////////////////////////////

	void INI::removeProperty(const StringView section, const StringView key)
	{
		const auto it = m_sectionIndex.find(section);

		if (it == m_sectionIndex.end())
		{
			return;
		}

		m_sections[m_sectionIndex[section]].removeProperty(key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	removeGlobalProperty
	//
	////////////////////////////////////////////////////////////////

	void INI::removeGlobalProperty(const StringView key)
	{
		removeProperty(GlobalSection, key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String INI::format(const INIWriteOptions& options) const
	{
		if (not isValid())
		{
			return String{};
		}

		String result;
		INIWriteOptions sectionOptions = options;
		sectionOptions.trailingNewline = false;

		const auto appendSection = [&](const INISection& section)
		{
			const String sectionText = section.format(sectionOptions);

			if (sectionText.isEmpty())
			{
				return;
			}

			if (result)
			{
				result.append(options.newline);
				result.append(options.newline);
			}

			result.append(sectionText);
		};

		if (const auto itGlobal = m_sectionIndex.find(GlobalSection);
			itGlobal != m_sectionIndex.end())
		{
			appendSection(m_sections[itGlobal->second]);
		}

		for (const auto& section : m_sections)
		{
			if (section.name == GlobalSection)
			{
				continue;
			}

			appendSection(section);
		}

		if (options.trailingNewline && result)
		{
			result.append(options.newline);
		}
	
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string INI::formatUTF8(const INIWriteOptions& options) const
	{
		std::string result = Unicode::ToUTF8(format(options));

		if (options.writeUTF8BOM)
		{
			result.insert(0, std::string{ "\xEF\xBB\xBF", 3 });
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool INI::save(const FilePathView path, const INIWriteOptions& options) const
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

	INI INI::Parse(const std::string_view s, const INIReadOptions& options, const FilePathView pathHint)
	{
		if (auto result = ParseResult(s, options, pathHint))
		{
			return std::move(result.value());
		}

		return Invalid();
	}

	INI INI::Parse(const StringView s, const INIReadOptions& options, const FilePathView pathHint)
	{
		if (auto result = ParseResult(s, options, pathHint))
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

	Result<INI, Array<INIParseErrorReason>> INI::ParseResult(std::string_view s, const INIReadOptions& options, const FilePathView pathHint)
	{
		if (options.skipUTF8BOM && HasUTF8BOM(s))
		{
			s.remove_prefix(3);
		}

		const String content = Unicode::FromUTF8(s);

		if ((not s.empty()) && content.isEmpty())
		{
			return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeInvalidEncodingError(pathHint) };
		}

		return ParseResult(StringView{ content }, options, pathHint);
	}

	Result<INI, Array<INIParseErrorReason>> INI::ParseResult(const StringView s, const INIReadOptions& options, const FilePathView pathHint)
	{
		return ParseINI(s, options, pathHint);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	INI INI::Load(const FilePathView path, const INIReadOptions& options)
	{
		if (auto result = LoadResult(path, options))
		{
			return std::move(result.value());
		}

		return Invalid();
	}

	INI INI::Load(std::unique_ptr<IReader> reader, const INIReadOptions& options)
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

	Result<INI, Array<INIParseErrorReason>> INI::LoadResult(const FilePathView path, const INIReadOptions& options)
	{
		BinaryFileReader reader{ path };

		if (auto content = ReadAll(reader, path))
		{
			return ParseResult(std::string_view{ content.value() }, options, path);
		}
		else
		{
			return Result<INI, Array<INIParseErrorReason>>{ unexpect, std::move(content.error()) };
		}
	}

	Result<INI, Array<INIParseErrorReason>> INI::LoadResult(std::unique_ptr<IReader> reader, const INIReadOptions& options)
	{
		if (not reader)
		{
			return Result<INI, Array<INIParseErrorReason>>{ unexpect, MakeInvalidReaderError(U"") };
		}

		if (auto content = ReadAll(*reader, U""))
		{
			return ParseResult(std::string_view{ content.value() }, options, U"");
		}
		else
		{
			return Result<INI, Array<INIParseErrorReason>>{ unexpect, std::move(content.error()) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	bool INI::load(const FilePathView path, const INIReadOptions& options)
	{
		if (auto result = LoadResult(path, options))
		{
			*this = std::move(result.value());
			return true;
		}

		*this = Invalid();
		return false;
	}

	bool INI::load(std::unique_ptr<IReader> reader, const INIReadOptions& options)
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
	//	parse
	//
	////////////////////////////////////////////////////////////////

	bool INI::parse(const std::string_view s, const INIReadOptions& options)
	{
		if (auto result = ParseResult(s, options))
		{
			*this = std::move(result.value());
			return true;
		}

		*this = Invalid();
		return false;
	}

	bool INI::parse(const StringView s, const INIReadOptions& options)
	{
		if (auto result = ParseResult(s, options))
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

	INI INI::Invalid()
	{
		INI ini;
		ini.m_sections.clear();
		ini.m_sectionIndex.clear();
		ini.m_isValid = false;
		return ini;
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	const String* INI::getPropertyValue(const StringView section, const StringView key) const
	{
		const auto it = m_sectionIndex.find(section);

		if (it == m_sectionIndex.end())
		{
			return nullptr;
		}

		const INISection& s = m_sections[it->second];

		const auto itItem = s.items.find(key);

		if (itItem == s.items.end())
		{
			return nullptr;
		}

		return &(itItem->second.value);
	}
}
