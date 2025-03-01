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
		void ThrowParseIntError(std::string_view s, const ParseErrorReason reason);

		[[noreturn]]
		void ThrowParseIntError(StringView s, const ParseErrorReason reason);
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseInt
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral Int>
	Int ParseInt(const std::string_view s, const int32 radix)
	{
		if (const auto result = ParseIntWithReason<Int>(s, radix))
		{
			return *result;
		}
		else
		{
			detail::ThrowParseIntError(s, result.error());
		}
	}

	template <Concept::Integral Int>
	Int ParseInt(const StringView s, const int32 radix)
	{
		if (const auto result = ParseIntWithReason<Int>(s, radix))
		{
			return *result;
		}
		else
		{
			detail::ThrowParseIntError(s, result.error());
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseIntOpt
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral Int>
	Optional<Int> ParseIntOpt(const std::string_view s, const int32 radix) noexcept
	{
		if (const auto result = ParseIntWithReason<Int>(s, radix))
		{
			return *result;
		}
		else
		{
			return none;
		}
	}

	template <Concept::Integral Int>
	Optional<Int> ParseIntOpt(const StringView s, const int32 radix) noexcept
	{
		if (const auto result = ParseIntWithReason<Int>(s, radix))
		{
			return *result;
		}
		else
		{
			return none;
		}
	}
}
