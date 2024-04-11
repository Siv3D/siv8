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

# include <Siv3D/INISection.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/FmtExtension.hpp>
# include <Siv3D/Demangle.hpp>

namespace s3d
{
	namespace
	{
		[[noreturn]]
		static void ThrowGetProperty(const StringView section, const StringView key)
		{
			throw Error{ fmt::format("INISection::operator []: Section `{}` does not contain the key `{}`", section, key) };
		}
	}

	namespace detail
	{
		void ThrowINISectionGetError(const char* type, const StringView section, const StringView key)
		{
			throw Error{ fmt::format("INI[{}]::get<{}>({}) failed", DemangleUTF8(type), section, key) };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	bool INISection::hasProperty(const StringView key) const noexcept
	{
		return items.contains(key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	void INISection::addProperty(const StringView key, const StringView value)
	{
		const auto it = items.find(key);

		if (it != items.end())
		{
			it->second.value = value;
		}
		else
		{
			items.emplace(key, INIItem{ String{ value }, static_cast<int32>(items.size()) });
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	void INISection::removeProperty(const StringView key)
	{
		items.erase(key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	const INIItem& INISection::operator [](const StringView key) const
	{
		const auto it = items.find(key);

		if (it == items.end())
		{
			ThrowGetProperty(name, key);
		}

		return it->second;
	}

	INIItem& INISection::operator [](const StringView key)
	{
		const auto it = items.find(key);

		if (it == items.end())
		{
			ThrowGetProperty(name, key);
		}

		return it->second;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	INISection::iterator INISection::begin() noexcept
	{
		return items.begin();
	}

	INISection::iterator INISection::end() noexcept
	{
		return items.end();
	}

	INISection::const_iterator INISection::begin() const noexcept
	{
		return items.begin();
	}

	INISection::const_iterator INISection::end() const noexcept
	{
		return items.end();
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasProperty
	//
	////////////////////////////////////////////////////////////////

	INISection::const_iterator INISection::cbegin() const noexcept
	{
		return items.cbegin();
	}

	INISection::const_iterator INISection::cend() const noexcept
	{
		return items.cend();
	}
}
