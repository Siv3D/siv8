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

# pragma once

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		void ThrowINISectionGetError(const char* type, StringView section, StringView key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type INISection::get(const StringView key) const
	{
		if (const auto opt = getOpt<Type>(key))
		{
			return *opt;
		}
		else
		{
			detail::ThrowINISectionGetError(typeid(Type).name(), name, key);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class U>
	Type INISection::getOr(const StringView key, U&& defaultValue) const
	{
		return getOpt<Type>(key).value_or(std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> INISection::getOpt(const StringView key) const
	{
		const auto it = items.find(key);

		if (it != items.end())
		{
			if constexpr (std::is_same_v<Type, String>)
			{
				return it->second.value;
			}
			else
			{
				return ParseOpt<Type>(it->second.value);
			}
		}
		else
		{
			return none;
		}
	}
}
