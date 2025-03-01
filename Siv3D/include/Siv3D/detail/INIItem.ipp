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
		void ThrowINIItemParseError(const char* type, StringView value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	get
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	[[nodiscard]]
	Type INIItem::get() const
	{
		if (const auto opt = getOpt<Type>())
		{
			return *opt;
		}
		else
		{
			detail::ThrowINIItemParseError(typeid(Type).name(), value);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class U>
	Type INIItem::getOr(U&& defaultValue) const
	{
		return getOpt<Type>().value_or(std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	getOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> INIItem::getOpt() const
	{
		if constexpr (std::is_same_v<Type, String>)
		{
			return value;
		}
		else
		{
			return ParseOpt<Type>(value);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	INIItem& INIItem::operator =(const Concept::Formattable auto& newValue)
	{
		value = Format(newValue);

		return *this;
	}
}
