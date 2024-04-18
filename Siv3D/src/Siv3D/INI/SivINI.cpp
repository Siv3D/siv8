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

# include <Siv3D/INI.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/FmtExtension.hpp>
# include <Siv3D/Error.hpp>
# include <Siv3D/Demangle.hpp>
# include <Siv3D/TextReader.hpp>
# include <Siv3D/TextWriter.hpp>
# include <Siv3D/EngineLog.hpp>

namespace s3d
{
	namespace
	{
		[[noreturn]]
		static void ThrowGetSection(const StringView section)
		{
			throw Error{ fmt::format("INI::getSection(): Section `{}` not found", section) };
		}
	}

	namespace detail
	{
		void ThrowINIGetError(const char* type, const StringView section, const StringView key)
		{
			throw Error{ fmt::format("INI::get<{}>({}, {}) failed", DemangleUTF8(type), section, key) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	INI::INI(const FilePathView path)
	{
		load(path);
	}

	INI::INI(std::unique_ptr<IReader>&& reader)
	{
		load(std::move(reader));
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	bool INI::load(const FilePathView path)
	{
		try
		{
			*this = Load(path);
		}
		catch (const Error&)
		{
			clear();
			return false;
		}

		return true;
	}

	bool INI::load(std::unique_ptr<IReader>&& reader)
	{
		try
		{
			*this = Load(std::move(reader));
		}
		catch (const Error&)
		{
			clear();
			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////
	//
	//	parse
	//
	////////////////////////////////////////////////////////////////

	bool INI::parse(const std::string_view s)
	{
		try
		{
			*this = Parse(s);
		}
		catch (const Error&)
		{
			clear();
			return false;
		}

		return true;
	}

	bool INI::parse(const StringView s)
	{
		try
		{
			*this = Parse(s);
		}
		catch (const Error&)
		{
			clear();
			return false;
		}

		return true;
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
			// セクションが一つもない場合、INI ファイルは空
			return true;
		}
		else if (sectionCount == 1)
		{
			// 唯一のセクションが無名セクションかつ空である場合、INI ファイルは空
			return (m_sections.front().name.isEmpty() && m_sections.front().items.empty());
		}
		else
		{
			// 2 つ以上のセクションがある場合、INI ファイルは空ではない
			return false;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	INI::operator bool() const noexcept
	{
		return (not isEmpty());
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

# ifdef __cpp_multidimensional_subscript

	const INISection& INI::operator []() const
	{
		return getSection(GlobalSection);
	}

	INISection& INI::operator []()
	{
		return getSection(GlobalSection);
	}

# endif

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
		// すでに同名のセクションが存在する場合は何もしない
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

	String INI::format() const
	{
		String result;

		// グローバル（無名）セクション
		if (const auto itGlobal = m_sectionIndex.find(GlobalSection);
			itGlobal != m_sectionIndex.end())
		{
			result += m_sections[itGlobal->second].format();
		}

		// その他のセクション
		for (const auto& section : m_sections)
		{
			if (section.name == GlobalSection)
			{
				continue;
			}

			if (result)
			{
				result.push_back(U'\n');
			}

			result += section.format();
		}
	
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	formatUTF8
	//
	////////////////////////////////////////////////////////////////

	std::string INI::formatUTF8() const
	{
		return Unicode::ToUTF8(format());
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool INI::save(const FilePathView path) const
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
	//	Load
	//
	////////////////////////////////////////////////////////////////

	INI INI::Load(const FilePathView path)
	{
		TextReader textReader{ path };

		if (not textReader)
		{
			throw Error{ fmt::format("INI::Load(): Failed to open file `{}`", path) };
		}

		return Parse(textReader.readAll());
	}

	INI INI::Load(std::unique_ptr<IReader>&& reader)
	{
		TextReader textReader{ std::move(reader) };

		if (not textReader)
		{
			throw Error{ "INI::Load(): Failed to open file" };
		}

		return Parse(textReader.readAll());
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	INI INI::Parse(const std::string_view s)
	{
		return Parse(Unicode::FromUTF8(s));
	}

	INI INI::Parse(const StringView s)
	{
		constexpr char32 Semicolon = U';';
		constexpr char32 Hash = U'#';
		constexpr char32 LBracket = U'[';
		constexpr char32 RBracket = U']';

		size_t lineIndex = 0;
		INI ini;
		String currentSection;

		for (auto&& _line : std::views::split(s, U'\n'))
		{
			++lineIndex;

			String line{ _line.begin(), _line.end() };

			line.trim();

			// 空白行
			if (line.isEmpty())
			{
				continue;
			}

			const char32 firstChar = line[0];

			// コメント
			if ((firstChar == Semicolon)
				|| (firstChar == Hash))
			{
				continue;
			}

			// セクション
			if (firstChar == LBracket)
			{
				const auto itRBracket = std::find((line.begin() + 1), line.end(), RBracket);

				if (itRBracket == line.end())
				{
					throw Error{ fmt::format("INI::Parse(): ({}) Unmatched '['", lineIndex) };
				}

				currentSection = line.substr(1, itRBracket - (line.begin() + 1)).trim();

				ini.addSection(currentSection);

				continue;
			}

			// プロパティ
			{
				const auto itEqual = std::find(line.begin(), line.end(), U'=');

				if (itEqual == line.end())
				{
					// error: '=' character not found in line
					throw Error{ fmt::format("INI::Parse(): ({}) '=' character not found in line", lineIndex) };
				}

				const String key = line.substr(0, (itEqual - line.begin())).trim();

				if (key.isEmpty())
				{
					throw Error{ fmt::format("INI::Parse(): ({}) key is empty", lineIndex) };
				}

				const String value = line.substr(itEqual - line.begin() + 1).trim();

				ini.addProperty(currentSection, key, value);
			}
		}

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
