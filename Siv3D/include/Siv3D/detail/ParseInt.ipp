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
	# if SIV3D_BUILD(DEBUG)
	
		[[noreturn]]
		void ThrowParseIntError(std::string_view s, const ParseErrorReason reason, const std::source_location& location);

		[[noreturn]]
		void ThrowParseIntError(StringView s, const ParseErrorReason reason, const std::source_location& location);
		
	# else

		[[noreturn]]
		void ThrowParseIntError(std::string_view s, const ParseErrorReason reason);

		[[noreturn]]
		void ThrowParseIntError(StringView s, const ParseErrorReason reason);

	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseInt
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral Int>
	Int ParseInt(const std::string_view s, const int32 radix, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto result = ParseIntWithReason<Int>(s, radix))
		{
			return *result;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			detail::ThrowParseIntError(s, result.error(), location);
		# else
			detail::ThrowParseIntError(s, result.error());
		# endif
		}
	}

	template <Concept::Integral Int>
	Int ParseInt(const StringView s, const int32 radix, [[maybe_unused]] const std::source_location& location)
	{
		if (const auto result = ParseIntWithReason<Int>(s, radix))
		{
			return *result;
		}
		else
		{
		# if SIV3D_BUILD(DEBUG)
			detail::ThrowParseIntError(s, result.error(), location);
		# else
			detail::ThrowParseIntError(s, result.error());
		# endif
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
