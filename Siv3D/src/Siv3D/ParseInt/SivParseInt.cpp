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

# include <Siv3D/ParseInt.hpp>
# include <Siv3D/FormatLiteral.hpp>
# include <Siv3D/Error/ParseError.hpp>
# include <Siv3D/Char.hpp>

namespace s3d
{
	namespace detail
	{
		[[noreturn]]
		void ThrowParseIntError(const std::string_view s, const ParseErrorReason reason)
		{
			if (reason == ParseErrorReason::EmptyInput)
			{
				throw ParseError{ "ParseInt(): Empty input" };
			}
			else
			{
				throw ParseError{ fmt::format("ParseInt(): Failed to parse `{}`", s) };
			}
		}

		[[noreturn]]
		void ThrowParseIntError(const StringView s, const ParseErrorReason reason)
		{
			ThrowParseIntError(Unicode::ToUTF8(s), reason);
		}

		[[nodiscard]]
		static constexpr bool IsValidRadix(const int32 radix) noexcept
		{
			return (radix == 0) || ((2 <= radix) && (radix <= 36));
		}

		template <class SV>
		[[nodiscard]]
		ParseErrorReason CheckParseInt(const SV sv) noexcept
		{
			auto* start = sv.data();
			auto* end = (start + sv.size());

			while ((start < end) && IsSpace(start[0]))
			{
				++start;
			}

			while ((start < end) && IsSpace(end[-1]))
			{
				--end;
			}

			if (end <= start)
			{
				return ParseErrorReason::EmptyInput;
			}

			return ParseErrorReason::InvalidFormat;
		}

		template <class SV>
		[[nodiscard]]
		ParseErrorReason CheckParseUint(SV sv) noexcept
		{
			auto* start = sv.data();
			auto* end = (start + sv.size());

			while ((start < end) && IsSpace(start[0]))
			{
				++start;
			}

			while ((start < end) && IsSpace(end[-1]))
			{
				--end;
			}

			if (end <= start)
			{
				return ParseErrorReason::EmptyInput;
			}

			if (*start == '-')
			{
				return ParseErrorReason::InvalidValueForType;
			}

			return ParseErrorReason::InvalidFormat;
		}


		// Copyright 2017 The Abseil Authors.
		//
		// Licensed under the Apache License, Version 2.0 (the "License");
		// you may not use this file except in compliance with the License.
		// You may obtain a copy of the License at
		//
		//      http://www.apache.org/licenses/LICENSE-2.0
		//
		// Unless required by applicable law or agreed to in writing, software
		// distributed under the License is distributed on an "AS IS" BASIS,
		// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
		// See the License for the specific language governing permissions and
		// limitations under the License.

		// Lookup tables per IntType:
		// vmax/base and vmin/base are precomputed because division costs at least 8ns.
		// TODO(junyer): Doing this per base instead (i.e. an array of structs, not a
		// struct of arrays) would probably be better in terms of d-cache for the most
		// commonly used bases.
		template <typename IntType>
		struct LookupTables {
			static const IntType kVmaxOverBase[];
			static const IntType kVminOverBase[];
		};

		// An array initializer macro for X/base where base in [0, 36].
		// However, note that lookups for base in [0, 1] should never happen because
		// base has been validated to be in [2, 36] by safe_parse_sign_and_base().
#define X_OVER_BASE_INITIALIZER(X)                                        \
		  {                                                                       \
			0, 0, X / 2, X / 3, X / 4, X / 5, X / 6, X / 7, X / 8, X / 9, X / 10, \
				X / 11, X / 12, X / 13, X / 14, X / 15, X / 16, X / 17, X / 18,   \
				X / 19, X / 20, X / 21, X / 22, X / 23, X / 24, X / 25, X / 26,   \
				X / 27, X / 28, X / 29, X / 30, X / 31, X / 32, X / 33, X / 34,   \
				X / 35, X / 36,                                                   \
		  }

		template <typename IntType>
		const IntType LookupTables<IntType>::kVmaxOverBase[] =
			X_OVER_BASE_INITIALIZER(std::numeric_limits<IntType>::max());

		template <typename IntType>
		const IntType LookupTables<IntType>::kVminOverBase[] =
			X_OVER_BASE_INITIALIZER(std::numeric_limits<IntType>::min());

#undef X_OVER_BASE_INITIALIZER

		// Represents integer values of digits.
		// Uses 36 to indicate an invalid character since we support
		// bases up to 36.
		constexpr int8_t kAsciiToInt[256] = {
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,  // 16 36s.
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 0,  1,  2,  3,  4,  5,
			6,  7,  8,  9,  36, 36, 36, 36, 36, 36, 36, 10, 11, 12, 13, 14, 15, 16, 17,
			18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
			36, 36, 36, 36, 36, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
			24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
			36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36 };

		// Parse the sign and optional hex or oct prefix in text.
		template <class SV>
		inline bool safe_parse_sign_and_base(SV* text /*inout*/, int* base_ptr /*inout*/, bool* negative_ptr /*output*/)
		{
			if (text->data() == nullptr) {
				return false;
			}

			const auto* start = text->data();
			const auto* end = start + text->size();
			int base = *base_ptr;

			// Consume whitespace.
			while (start < end && IsSpace(start[0])) {
				++start;
			}
			while (start < end && IsSpace(end[-1])) {
				--end;
			}
			if (start >= end) {
				return false;
			}

			// Consume sign.
			*negative_ptr = (start[0] == '-');
			if (*negative_ptr || start[0] == '+') {
				++start;
				if (start >= end) {
					return false;
				}
			}

			// Consume base-dependent prefix.
			//  base 0: "0x" -> base 16, "0" -> base 8, default -> base 10
			//  base 16: "0x" -> base 16
			// Also validate the base.
			if (base == 0) {
				if (end - start >= 2 && start[0] == '0' &&
					(start[1] == 'x' || start[1] == 'X')) {
					base = 16;
					start += 2;
					if (start >= end) {
						// "0x" with no digits after is invalid.
						return false;
					}
				}
				else if (end - start >= 1 && start[0] == '0') {
					base = 8;
					start += 1;
				}
				else {
					base = 10;
				}
			}
			else if (base == 16) {
				if (end - start >= 2 && start[0] == '0' &&
					(start[1] == 'x' || start[1] == 'X')) {
					start += 2;
					if (start >= end) {
						// "0x" with no digits after is invalid.
						return false;
					}
				}
			}
			else if (base >= 2 && base <= 36) {
				// okay
			}
			else {
				return false;
			}
			*text = SV(start, end - start);
			*base_ptr = base;
			return true;
		}

		template <class SV, typename IntType>
		inline bool safe_parse_positive_int(SV text, int base, IntType* value_p)
		{
			IntType value = 0;
			const IntType vmax = std::numeric_limits<IntType>::max();
			assert(vmax > 0);
			assert(base >= 0);
			assert(vmax >= static_cast<IntType>(base));
			const IntType vmax_over_base = LookupTables<IntType>::kVmaxOverBase[base];
			assert(base < 2 ||
				std::numeric_limits<IntType>::max() / base == vmax_over_base);
			const auto* start = text.data();
			const auto* end = start + text.size();
			// loop over digits
			for (; start < end; ++start) {
				unsigned char c = static_cast<unsigned char>(start[0]);
				int digit = kAsciiToInt[c];
				if (digit >= base) {
					*value_p = value;
					return false;
				}
				if (value > vmax_over_base) {
					*value_p = vmax;
					return false;
				}
				value *= base;
				if (value > vmax - digit) {
					*value_p = vmax;
					return false;
				}
				value += digit;
			}
			*value_p = value;
			return true;
		}

		template <class SV, typename IntType>
		inline bool safe_parse_negative_int(SV text, int base, IntType* value_p)
		{
			IntType value = 0;
			const IntType vmin = std::numeric_limits<IntType>::min();
			assert(vmin < 0);
			assert(vmin <= 0 - base);
			IntType vmin_over_base = LookupTables<IntType>::kVminOverBase[base];
			assert(base < 2 ||
				std::numeric_limits<IntType>::min() / base == vmin_over_base);
			// 2003 c++ standard [expr.mul]
			// "... the sign of the remainder is implementation-defined."
			// Although (vmin/base)*base + vmin%base is always vmin.
			// 2011 c++ standard tightens the spec but we cannot rely on it.
			// TODO(junyer): Handle this in the lookup table generation.
			if (vmin % base > 0) {
				vmin_over_base += 1;
			}
			const auto* start = text.data();
			const auto* end = start + text.size();
			// loop over digits
			for (; start < end; ++start) {
				unsigned char c = static_cast<unsigned char>(start[0]);
				int digit = kAsciiToInt[c];
				if (digit >= base) {
					*value_p = value;
					return false;
				}
				if (value < vmin_over_base) {
					*value_p = vmin;
					return false;
				}
				value *= base;
				if (value < vmin + digit) {
					*value_p = vmin;
					return false;
				}
				value -= digit;
			}
			*value_p = value;
			return true;
		}

		// Input format based on POSIX.1-2008 strtol
		// http://pubs.opengroup.org/onlinepubs/9699919799/functions/strtol.html
		template <class SV, typename IntType>
		inline bool safe_int_internal(SV text, IntType* value_p, int base)
		{
			*value_p = 0;
			bool negative;
			if (!safe_parse_sign_and_base(&text, &base, &negative)) {
				return false;
			}
			if (!negative) {
				return safe_parse_positive_int(text, base, value_p);
			}
			else {
				return safe_parse_negative_int(text, base, value_p);
			}
		}

		template <class SV, typename IntType>
		inline bool safe_uint_internal(SV text, IntType* value_p, int base)
		{
			*value_p = 0;
			bool negative;
			if (!safe_parse_sign_and_base(&text, &base, &negative) || negative) {
				return false;
			}
			return safe_parse_positive_int(text, base, value_p);
		}

		template <class SV>
		Result<int8, ParseErrorReason> ParseInt8WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			int32 result = 0;

			if (not safe_int_internal(s, &result, radix)) [[unlikely]]
			{
				if ((result == std::numeric_limits<int32>::min())
					|| (result == std::numeric_limits<int32>::max()))
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseInt(s) };
			}

			if (not InRange<int32>(result, INT8_MIN, INT8_MAX)) [[unlikely]]
			{
				return Err{ ParseErrorReason::NumericOverflow };
			}

			return static_cast<int8>(result);
		}

		template <class SV>
		Result<uint8, ParseErrorReason> ParseUint8WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			uint32 result = 0;

			if (not safe_uint_internal(s, &result, radix)) [[unlikely]]
			{
				if (result == std::numeric_limits<uint32>::max())
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseUint(s) };
			}
		
			if (UINT8_MAX < result) [[unlikely]]
			{
				return Err{ ParseErrorReason::NumericOverflow };
			}

			return static_cast<uint8>(result);
		}

		template <class SV>
		Result<int16, ParseErrorReason> ParseInt16WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			int32 result = 0;

			if (not safe_int_internal(s, &result, radix)) [[unlikely]]
			{
				if ((result == std::numeric_limits<int32>::min())
					|| (result == std::numeric_limits<int32>::max()))
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseInt(s) };
			}
		
			if (not InRange<int32>(result, INT16_MIN, INT16_MAX)) [[unlikely]]
			{
				return Err{ ParseErrorReason::NumericOverflow };
			}

			return static_cast<int16>(result);
		}

		template <class SV>
		Result<uint16, ParseErrorReason> ParseUint16WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			uint32 result = 0;

			if (not safe_uint_internal(s, &result, radix)) [[unlikely]]
			{
				if (result == std::numeric_limits<uint32>::max())
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseUint(s) };
			}
		
			if (UINT16_MAX < result) [[unlikely]]
			{
				return Err{ ParseErrorReason::NumericOverflow };
			}

			return static_cast<uint16>(result);
		}

		template <class SV>
		Result<int32, ParseErrorReason> ParseInt32WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			int32 result = 0;

			if (not safe_int_internal(s, &result, radix)) [[unlikely]]
			{
				if ((result == std::numeric_limits<int32>::min())
					|| (result == std::numeric_limits<int32>::max()))
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseInt(s) };
			}

			return result;
		}

		template <class SV>
		Result<uint32, ParseErrorReason> ParseUint32WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			uint32 result = 0;

			if (not safe_uint_internal(s, &result, radix)) [[unlikely]]
			{
				if (result == std::numeric_limits<uint32>::max())
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseUint(s) };
			}

			return result;
		}

		template <class SV>
		Result<int64, ParseErrorReason> ParseInt64WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			int64 result = 0;

			if (not safe_int_internal(s, &result, radix)) [[unlikely]]
			{
				if ((result == std::numeric_limits<int64>::min())
					|| (result == std::numeric_limits<int64>::max()))
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseInt(s) };
			}

			return result;
		}

		template <class SV>
		Result<uint64, ParseErrorReason> ParseUint64WithReason_impl(const SV s, const int32 radix) noexcept
		{
			if (not IsValidRadix(radix)) [[unlikely]]
			{
				return Err{ ParseErrorReason::InvalidRadix };
			}

			uint64 result = 0;

			if (not safe_uint_internal(s, &result, radix)) [[unlikely]]
			{
				if (result == std::numeric_limits<uint64>::max())
				{
					return Err{ ParseErrorReason::NumericOverflow };
				}

				return Err{ CheckParseUint(s) };
			}

			return result;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	ParseIntWithReason
	//
	////////////////////////////////////////////////////////////////

	template <>
	Result<int8, ParseErrorReason> ParseIntWithReason<int8>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseInt8WithReason_impl(s, radix);
	}

	template <>
	Result<int8, ParseErrorReason> ParseIntWithReason<int8>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseInt8WithReason_impl(s, radix);
	}

	template <>
	Result<uint8, ParseErrorReason> ParseIntWithReason<uint8>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseUint8WithReason_impl(s, radix);
	}

	template <>
	Result<uint8, ParseErrorReason> ParseIntWithReason<uint8>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseUint8WithReason_impl(s, radix);
	}

	template <>
	Result<int16, ParseErrorReason> ParseIntWithReason<int16>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseInt16WithReason_impl(s, radix);
	}

	template <>
	Result<int16, ParseErrorReason> ParseIntWithReason<int16>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseInt16WithReason_impl(s, radix);
	}

	template <>
	Result<uint16, ParseErrorReason> ParseIntWithReason<uint16>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseUint16WithReason_impl(s, radix);
	}

	template <>
	Result<uint16, ParseErrorReason> ParseIntWithReason<uint16>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseUint16WithReason_impl(s, radix);
	}

	template <>
	Result<int32, ParseErrorReason> ParseIntWithReason<int32>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseInt32WithReason_impl(s, radix);
	}

	template <>
	Result<int32, ParseErrorReason> ParseIntWithReason<int32>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseInt32WithReason_impl(s, radix);
	}

	template <>
	Result<uint32, ParseErrorReason> ParseIntWithReason<uint32>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseUint32WithReason_impl(s, radix);
	}

	template <>
	Result<uint32, ParseErrorReason> ParseIntWithReason<uint32>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseUint32WithReason_impl(s, radix);
	}

	template <>
	Result<int64, ParseErrorReason> ParseIntWithReason<int64>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseInt64WithReason_impl(s, radix);
	}

	template <>
	Result<int64, ParseErrorReason> ParseIntWithReason<int64>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseInt64WithReason_impl(s, radix);
	}

	template <>
	Result<uint64, ParseErrorReason> ParseIntWithReason<uint64>(const std::string_view s, const int32 radix) noexcept
	{
		return detail::ParseUint64WithReason_impl(s, radix);
	}

	template <>
	Result<uint64, ParseErrorReason> ParseIntWithReason<uint64>(const StringView s, const int32 radix) noexcept
	{
		return detail::ParseUint64WithReason_impl(s, radix);
	}
}
