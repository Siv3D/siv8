//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/INISection.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/FormatLiteral.hpp>
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
	//	removeProperty
	//
	////////////////////////////////////////////////////////////////

	void INISection::removeProperty(const StringView key)
	{
		const auto it = items.find(key);

		if (it == items.end())
		{
			return;
		}

		const int32 index = it->second.index;

		items.erase(key);

		for (auto& [_, item] : items)
		{
			if (index < item.index)
			{
				--item.index;
			}
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator []
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
	//	begin, end
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
	//	cbegin, cend
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

	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String INISection::format() const
	{
		String result;

		if (name)
		{
			result += (U'[' + name + U"]\n");
		}

		Array<std::pair<int32, std::pair<String, String>>> sortedItems;

		for (const auto& [key, item] : items)
		{
			sortedItems.emplace_back(item.index, std::pair{ key, item.value });
		}

		sortedItems.sort();

		for (const auto& item : sortedItems)
		{
			result += (item.second.first + U" = " + item.second.second + U'\n');
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	_addProperty
	//
	////////////////////////////////////////////////////////////////

	void INISection::_addProperty(const StringView key, String&& value)
	{
		items.emplace(key, INIItem{ .value = std::move(value), .index = static_cast<int32>(items.size()) });
	}
}
