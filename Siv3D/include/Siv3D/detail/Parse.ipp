//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
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
		void ThrowParseError(const char* type, std::string_view s);
	}

	template <class Type>
	Type Parse(const StringView s)
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			return ParseBool(s);
		}
		else if constexpr (Concept::Integral<Type>)
		{
			return ParseInt<Type>(s);
		}
		else if constexpr (Concept::FloatingPoint<Type>)
		{
			return ParseFloat<Type>(s);
		}
		else
		{
			const std::string utf8 = s.toUTF8();

			Type to;

			if (not(std::istringstream{ utf8 } >> to))
			{
				detail::ThrowParseError(typeid(Type).name(), utf8);
			}

			return to;
		}
	}

	template <class Type, class U>
	Type ParseOr(const StringView s, U&& defaultValue)
	{
		return ParseChecked<Type>(s).value_or(std::forward<U>(defaultValue));
	}

	template <class Type, class Fty>
	[[nodiscard]]
	Type ParseOrElse(StringView s, Fty&& f)
	{
		return ParseChecked<Type>(s).or_else(std::forward<Fty>(f));
	}

	template <class Type>
	Optional<Type> ParseOpt(const StringView s) noexcept
	{
		if (const auto checked = ParseChecked<Type>(s))
		{
			return *checked;
		}
		else
		{
			return none;
		}
	}

	template <class Type>
	Expected<Type, ParseErrorReason> ParseChecked(const StringView s) noexcept
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			return ParseBoolChecked(s);
		}
		else if constexpr (Concept::Integral<Type>)
		{
			return ParseIntChecked<Type>(s);
		}
		else if constexpr (Concept::FloatingPoint<Type>)
		{
			return ParseFloatChecked<Type>(s);
		}
		else
		{
			const std::string utf8 = s.toUTF8();
			
			Type to;
			
			if (not(std::istringstream{ utf8 } >> to))
			{
				return Unexpected{ ParseErrorReason::InvalidFormat };
			}
			
			return to;
		}
	}
}
