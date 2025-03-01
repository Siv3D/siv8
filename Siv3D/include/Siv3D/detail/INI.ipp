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

# pragma once

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		void ThrowINIGetError(const char* type, StringView section, StringView key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	INI::INI(Reader&& reader)
	{
		load(std::make_unique<Reader>(std::move(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	bool INI::load(Reader&& reader)
	{
		return load(std::make_unique<Reader>(std::move(reader)));
	}

	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type INI::get(const StringView section, const StringView key) const
	{
		if (const auto opt = getOpt<Type>())
		{
			return *opt;
		}
		else
		{
			detail::ThrowINIGetError(typeid(Type).name(), section, key);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class U>
	Type INI::getOr(const StringView section, const StringView key, U&& defaultValue) const
	{
		return getOpt<Type>(section, key).value_or(std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> INI::getOpt(const StringView section, const StringView key) const
	{
		if (const String* value = getPropertyValue(section, key))
		{
			if constexpr (std::is_same_v<Type, String>)
			{
				return *value;
			}
			else
			{
				return ParseOpt<Type>(*value);
			}
		}
		else
		{
			return none;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlobal
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type INI::getGlobal(const StringView key) const
	{
		return get<Type>(GlobalSection, key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlobalOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class U>
	Type INI::getGlobalOr(const StringView key, U&& defaultValue) const
	{
		return getOr<Type>(GlobalSection, key, std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getGlobalOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> INI::getGlobalOpt(const StringView key) const
	{
		return getOpt<Type>(GlobalSection, key);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Load
	//
	////////////////////////////////////////////////////////////////

	template <class Reader>
		requires (std::is_base_of_v<IReader, Reader> && (not std::is_lvalue_reference_v<Reader>))
	INI INI::Load(Reader&& reader)
	{
		return Load(std::make_unique<Reader>(std::move(reader)));
	}
}
