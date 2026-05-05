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
			throw Error{ fmt::format("INISection::operator []: Section `{}` does not contain the key `{}`", section.toUTF8(), key.toUTF8()) };
		}
	}

	namespace detail
	{
		void ThrowINISectionGetError(const char* type, const StringView section, const StringView key)
		{
			throw Error{ fmt::format("INI[{}]::get<{}>({}) failed", DemangleUTF8(type), section.toUTF8(), key.toUTF8()) };
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
	//	propertyView
	//
	////////////////////////////////////////////////////////////////

	Array<INISection::Property> INISection::propertyView() const
	{
		Array<std::pair<int32, String>> sortedKeys;

		for (const auto& [key, item] : items)
		{
			sortedKeys.emplace_back(item.index, key);
		}

		sortedKeys.sort();

		Array<Property> result;
		result.reserve(sortedKeys.size());

		for (const auto& [_, key] : sortedKeys)
		{
			const auto it = items.find(key);

			if (it == items.end())
			{
				continue;
			}

			const INIItem& item = it->second;
			result.push_back(Property{ .key = key, .value = item.value, .index = item.index });
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	format
	//
	////////////////////////////////////////////////////////////////

	String INISection::format(const INIWriteOptions& options) const
	{
		String result;
		const String& newline = options.newline;

		if (name)
		{
			result.push_back(U'[');
			result.append(name);
			result.push_back(U']');
		}

		for (const auto& property : propertyView())
		{
			if (result)
			{
				result.append(newline);
			}

			result.append(property.key);

			if (options.spaceAroundEquals)
			{
				result.append(U" = ");
			}
			else
			{
				result.push_back(U'=');
			}

			result.append(property.value);
		}

		if (options.trailingNewline && result)
		{
			result.append(newline);
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
