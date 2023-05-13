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
		void ThrowParseIntError(std::string_view s, const ParseErrorReason reason);

		[[noreturn]]
		void ThrowParseIntError(StringView s, const ParseErrorReason reason);
	}

	template <Concept::Integral Int>
	Int ParseInt(const std::string_view s, const int32 radix)
	{
		if (const auto checked = ParseIntChecked<Int>(s, radix))
		{
			return checked.value();
		}
		else
		{
			detail::ThrowParseIntError(s, checked.error());
		}
	}

	template <Concept::Integral Int>
	Int ParseInt(const StringView s, const int32 radix)
	{
		if (const auto checked = ParseIntChecked<Int>(s, radix))
		{
			return checked.value();
		}
		else
		{
			detail::ThrowParseIntError(s, checked.error());
		}
	}

	template <Concept::Integral Int>
	Optional<Int> ParseIntOpt(const std::string_view s, const int32 radix) noexcept
	{
		if (const auto checked = ParseIntChecked<Int>(s, radix))
		{
			return checked.value();
		}
		else
		{
			return none;
		}
	}

	template <Concept::Integral Int>
	Optional<Int> ParseIntOpt(const StringView s, const int32 radix) noexcept
	{
		if (const auto checked = ParseIntChecked<Int>(s, radix))
		{
			return checked.value();
		}
		else
		{
			return none;
		}
	}
}
