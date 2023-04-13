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

# include <Siv3D/IntToString.hpp>
# include <Siv3D/IntFormatter.hpp>

namespace s3d
{
	namespace
	{
		template <Concept::UnsignedIntegral UnsignedInt>
		[[nodiscard]]
		inline String ItoW(const UnsignedInt value, const unsigned radix, const bool isNegative, const LetterCase letterCase)
		{
			if (radix < 2 || 36 < radix)
			{
				return{};
			}

			char32 buffer[std::numeric_limits<UnsignedInt>::digits];
			char32* p = buffer;
			size_t length = 0;
			UnsignedInt remaining = value;

			if (isNegative)
			{
				*p++ = '-';
				++length;
				remaining = static_cast<UnsignedInt>(-static_cast<std::make_signed_t<UnsignedInt>>(remaining));
			}

			char32* first_digit = p;
			const char32 a = (letterCase == LetterCase::Upper) ? U'A' : U'a';

			do
			{
				const UnsignedInt digit = static_cast<UnsignedInt>(remaining % static_cast<UnsignedInt>(radix));
				remaining /= static_cast<UnsignedInt>(radix);
				*p++ = static_cast<char32>(digit < 10 ? (U'0' + digit) : (a + digit - 10));
				++length;
			} while (remaining > 0);

			--p;

			do
			{
				std::swap(*p, *first_digit);
				--p;
				++first_digit;
			} while (first_digit < p);

			return String(buffer, length);
		}
	}

	String ToString(const int8 value)
	{
		return ToString(static_cast<int32>(value));
	}

	String ToString(const uint8 value)
	{
		return ToString(static_cast<uint32>(value));
	}

	String ToString(const int16 value)
	{
		return ToString(static_cast<int32>(value));
	}

	String ToString(const uint16 value)
	{
		return ToString(static_cast<uint32>(value));
	}

	String ToString(const int32 value)
	{
		const IntFormatter formatter{ value };
		return String(formatter.data(), (formatter.data() + formatter.size()));
	}

	String ToString(const uint32 value)
	{
		const IntFormatter formatter{ value };
		return String(formatter.data(), (formatter.data() + formatter.size()));
	}

	String ToString(const long value)
	{
		if constexpr (sizeof(long) == sizeof(int32))
		{
			return ToString(static_cast<int32>(value));
		}
		else
		{
			return ToString(static_cast<long long>(value));
		}
	}

	String ToString(const unsigned long value)
	{
		if constexpr (sizeof(unsigned long) == sizeof(uint32))
		{
			return ToString(static_cast<uint32>(value));
		}
		else
		{
			return ToString(static_cast<unsigned long long>(value));
		}
	}

	String ToString(const long long value)
	{
		const IntFormatter formatter{ static_cast<int64>(value) };
		return String(formatter.data(), (formatter.data() + formatter.size()));
	}

	String ToString(const unsigned long long value)
	{
		const IntFormatter formatter{ static_cast<uint64>(value) };
		return String(formatter.data(), (formatter.data() + formatter.size()));
	}

	String ToBaseN(const int8 value, const int32 radix, const LetterCase letterCase)
	{
		return ToBaseN(static_cast<int32>(value), radix, letterCase);
	}

	String ToBaseN(const uint8 value, const int32 radix, const LetterCase letterCase)
	{
		return ToBaseN(static_cast<uint32>(value), radix, letterCase);
	}

	String ToBaseN(const int16 value, const int32 radix, const LetterCase letterCase)
	{
		return ToBaseN(static_cast<int32>(value), radix, letterCase);
	}

	String ToBaseN(const uint16 value, const int32 radix, const LetterCase letterCase)
	{
		return ToBaseN(static_cast<uint32>(value), radix, letterCase);
	}

	String ToBaseN(const int32 value, const int32 radix, const LetterCase letterCase)
	{
		const bool isNegative = ((radix == 10) && (value < 0));
		return ItoW(static_cast<uint32>(value), radix, isNegative, letterCase);
	}

	String ToBaseN(const uint32 value, const int32 radix, const LetterCase letterCase)
	{
		constexpr bool isNegative = false;
		return ItoW(value, radix, isNegative, letterCase);
	}

	String ToBaseN(const long value, const int32 radix, const LetterCase letterCase)
	{
		if constexpr (sizeof(long) == sizeof(int32))
		{
			return ToBaseN(static_cast<int32>(value), radix, letterCase);
		}
		else
		{
			return ToBaseN(static_cast<long long>(value), radix, letterCase);
		}
	}

	String ToBaseN(const unsigned long value, const int32 radix, const LetterCase letterCase)
	{
		if constexpr (sizeof(unsigned long) == sizeof(uint32))
		{
			return ToBaseN(static_cast<uint32>(value), radix, letterCase);
		}
		else
		{
			return ToBaseN(static_cast<unsigned long long>(value), radix, letterCase);
		}
	}

	String ToBaseN(const long long value, const int32 radix, const LetterCase letterCase)
	{
		const bool isNegative = ((radix == 10) && (value < 0));
		return ItoW(static_cast<uint64>(value), radix, isNegative, letterCase);
	}

	String ToBaseN(const unsigned long long value, const int32 radix, const LetterCase letterCase)
	{
		constexpr bool isNegative = false;
		return ItoW(value, radix, isNegative, letterCase);
	}
}
