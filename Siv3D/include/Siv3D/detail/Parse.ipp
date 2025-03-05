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
		void ThrowParseError(const char* type, std::string_view s);

		[[noreturn]]
		void ThrowParseError(const char* type, std::string_view s, const std::source_location& location);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Parse
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Type Parse(const std::string_view s, [[maybe_unused]] const std::source_location& location)
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			return ParseBool(s, location);
		}
		else if constexpr (Concept::Integral<Type>)
		{
			return ParseInt<Type>(s, 10, location);
		}
		else if constexpr (Concept::FloatingPoint<Type>)
		{
			return ParseFloat<Type>(s, location);
		}
		else
		{
			Type to;

			if (not(std::istringstream{ std::string{ s } } >> to))
			{
			# if SIV3D_BUILD(DEBUG)
				detail::ThrowParseError(typeid(Type).name(), s, location);
			# else
				detail::ThrowParseError(typeid(Type).name(), s);
			# endif
			}

			return to;
		}
	}

	template <class Type>
	Type Parse(const StringView s, [[maybe_unused]] const std::source_location& location)
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			return ParseBool(s, location);
		}
		else if constexpr (Concept::Integral<Type>)
		{
			return ParseInt<Type>(s, 10, location);
		}
		else if constexpr (Concept::FloatingPoint<Type>)
		{
			return ParseFloat<Type>(s, location);
		}
		else
		{
			const std::string utf8 = Unicode::ToUTF8(s);

			Type to;

			if (not(std::istringstream{ utf8 } >> to))
			{
			# if SIV3D_BUILD(DEBUG)
				detail::ThrowParseError(typeid(Type).name(), utf8, location);
			# else
				detail::ThrowParseError(typeid(Type).name(), utf8);
			# endif
			}

			return to;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseOr
	//
	////////////////////////////////////////////////////////////////

	template <class Type, class U> requires std::is_convertible_v<U, Type>
	Type ParseOr(const std::string_view s, U&& defaultValue)
	{
		return ParseWithReason<Type>(s).value_or(std::forward<U>(defaultValue));
	}

	template <class Type, class U> requires std::is_convertible_v<U, Type>
	Type ParseOr(const StringView s, U&& defaultValue)
	{
		return ParseWithReason<Type>(s).value_or(std::forward<U>(defaultValue));
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseOpt
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Optional<Type> ParseOpt(const std::string_view s) noexcept
	{
		if (const auto reason = ParseWithReason<Type>(s))
		{
			return *reason;
		}
		else
		{
			return none;
		}
	}

	template <class Type>
	Optional<Type> ParseOpt(const StringView s) noexcept
	{
		if (const auto reason = ParseWithReason<Type>(s))
		{
			return *reason;
		}
		else
		{
			return none;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseWithReason
	//
	////////////////////////////////////////////////////////////////

	template <class Type>
	Result<Type, ParseErrorReason> ParseWithReason(const std::string_view s) noexcept
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			return ParseBoolWithReason(s);
		}
		else if constexpr (Concept::Integral<Type>)
		{
			return ParseIntWithReason<Type>(s);
		}
		else if constexpr (Concept::FloatingPoint<Type>)
		{
			return ParseFloatWithReason<Type>(s);
		}
		else
		{
			Type to;

			if (not(std::istringstream{ std::string{ s } } >> to))
			{
				return Err{ ParseErrorReason::InvalidFormat };
			}

			return to;
		}
	}

	template <class Type>
	Result<Type, ParseErrorReason> ParseWithReason(const StringView s) noexcept
	{
		if constexpr (std::is_same_v<Type, bool>)
		{
			return ParseBoolWithReason(s);
		}
		else if constexpr (Concept::Integral<Type>)
		{
			return ParseIntWithReason<Type>(s);
		}
		else if constexpr (Concept::FloatingPoint<Type>)
		{
			return ParseFloatWithReason<Type>(s);
		}
		else
		{
			const std::string utf8 = Unicode::ToUTF8(s);

			Type to;

			if (not(std::istringstream{ utf8 } >> to))
			{
				return Err{ ParseErrorReason::InvalidFormat };
			}

			return to;
		}
	}
}
