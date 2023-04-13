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

# include <memory>
# include <Siv3D/IntFormatter.hpp>

namespace s3d
{
	namespace
	{
		struct FormatDecimalResult
		{
			char* begin;
			char* end;
		};

		[[nodiscard]]
		constexpr const char* GetDigits(const size_t value) noexcept
		{
			return &"0001020304050607080910111213141516171819"
				"2021222324252627282930313233343536373839"
				"4041424344454647484950515253545556575859"
				"6061626364656667686970717273747576777879"
				"8081828384858687888990919293949596979899"[value * 2];
		}

		constexpr void Copy(char* dst, const char* src) noexcept
		{
			if (not std::is_constant_evaluated())
			{
				std::memcpy(dst, src, 2);
				return;
			}

			*dst++ = *src++;
			*dst = *src;
		}

		[[nodiscard]]
		constexpr FormatDecimalResult FormatDecimal(char* out, uint64 value, const size_t size) noexcept
		{
			out += size;
			char* end = out;

			while (100 <= value)
			{
				out -= 2;
				Copy(out, GetDigits(static_cast<size_t>(value % 100)));
				value /= 100;
			}

			if (value < 10)
			{
				*--out = static_cast<char>('0' + value);
				return{ out, end };
			}

			out -= 2;
			Copy(out, GetDigits(static_cast<size_t>(value)));
			return{ out, end };
		}
	}

	IntFormatter::IntFormatter(const int32 value) noexcept
		: IntFormatter{ static_cast<int64>(value) } {}

	IntFormatter::IntFormatter(const int64 value) noexcept
		: m_str{ formatSigned(value) } {}

	IntFormatter::IntFormatter(const uint32 value) noexcept
		: IntFormatter{ static_cast<uint64>(value) } {}

	IntFormatter::IntFormatter(const uint64 value) noexcept
		: m_str{ formatUnsigned(value) } {}

	char* IntFormatter::formatUnsigned(const uint64 value) noexcept
	{
		return FormatDecimal(m_buffer, value, (BufferSize - 1)).begin;
	}

	const char* IntFormatter::formatSigned(const int64 value) noexcept
	{
		auto absValue = static_cast<uint64>(value);

		const bool negative = (value < 0);

		if (negative)
		{
			absValue = (0 - absValue);
		}

		auto begin = formatUnsigned(absValue);

		if (negative)
		{
			*--begin = '-';
		}

		return begin;
	}
}
