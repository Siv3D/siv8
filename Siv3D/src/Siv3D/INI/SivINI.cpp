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
# include <Siv3D/TextReader.hpp>

namespace s3d
{
	namespace
	{
		[[noreturn]]
		static void ThrowGetSection(const StringView section)
		{
			throw Error{ U"INI::getSection(): Section `{}` not found"_fmt(section) };
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
			return (m_sections.front().name.isEmpty() && m_sections.front().items.isEmpty());
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

	////////////////////////////////////////////////////////////////
	//
	//	hasGlobalSection
	//
	////////////////////////////////////////////////////////////////

	bool INI::hasGlobalSection() const noexcept
	{
		return m_sectionIndex.contains(U"");
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlobalSection
	//
	////////////////////////////////////////////////////////////////

	const INISection& INI::getGlobalSection() const
	{
		return getSection(U"");
	}

	INISection& INI::getGlobalSection()
	{
		return getSection(U"");
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
			throw Error{ U"INI::Load(): Failed to open file" };
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

		INI ini;
		String currentSection;

		for (auto&& _line : std::views::split(s, U'\n'))
		{
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
					throw Error{ U"INI::Parse(): Unmatched '['" };
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
					throw Error{ U"INI::Parse(): '=' character not found in line" };
				}

				const String key = line.substr(0, (itEqual - line.begin())).trim();

				if (key.isEmpty())
				{
					throw Error{ U"INI::Parse(): key is empty" };
				}

				const String value = line.substr(itEqual - line.begin() + 1).trim();

				ini.addProperty(currentSection, key, value);
			}
		}

		return ini;
	}
}
