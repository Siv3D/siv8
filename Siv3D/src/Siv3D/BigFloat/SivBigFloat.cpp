//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <algorithm>
# include <array>
# include <cerrno>
# include <charconv>
# include <cmath>
# include <cstdlib>
# include <limits>
# include <memory>
# include <new>
# include <type_traits>
# include <ThirdParty/fast_float/fast_float.h>
# include <Siv3D/BigFloat.hpp>
# include <Siv3D/Unicode.hpp>
# include "BigFloatDetail.hpp"

# if SIV3D_PLATFORM(MACOS)
	# include <xlocale.h>
# elif not SIV3D_PLATFORM(WINDOWS)
	# include <locale.h>
# endif

namespace s3d
{
	namespace
	{
		template <class Float>
		[[nodiscard]]
		static Float ToNativeFloat(const boost::multiprecision::cpp_dec_float_100& value)
		{
			const std::string text = value.str(0, std::ios_base::fmtflags{});
			Float result = 0;
			// The generated text is valid; range errors also set the result to signed infinity or zero.
			fast_float::from_chars(text.data(), (text.data() + text.size()), result);
			return result;
		}

		[[nodiscard]]
		static std::string RemoveTrailingZeros(std::string&& s) noexcept
		{
			if (s.find('.') == std::string::npos)
			{
				return s;
			}

			while (s.back() == '0')
			{
				s.pop_back();
			}

			if (s.back() == '.')
			{
				s.pop_back();
			}

			return s;
		}
	}

	void BigFloat::BigFloatDetail::AssignFromBigInt(value_type& destination, const BigInt& i)
	{
		using Integer = boost::multiprecision::cpp_int;
		using Limb = boost::multiprecision::limb_type;
		constexpr size_t LimbBits = std::numeric_limits<Limb>::digits;
		const auto& integer = i._detail().value;
		const auto& backend = integer.backend();

		// Native assignment is exact and needs no temporary storage for up to 64 magnitude bits.
		if (backend.size() <= (64 / LimbBits))
		{
			uint64 magnitude = static_cast<uint64>(backend.limbs()[0]);
			if constexpr (LimbBits < 64)
			{
				for (size_t index = 1; index < backend.size(); ++index)
				{
					magnitude |= (static_cast<uint64>(backend.limbs()[index]) << (index * LimbBits));
				}
			}

			destination.assign(magnitude);
			if (backend.sign())
			{
				destination.backend().negate();
			}
			return;
		}

		constexpr size_t BlockDigits = (LimbBits >= 64) ? 19 : 9;
		const Integer divisor = (LimbBits >= 64) ? 10000000000000000000ULL : 1000000000ULL;
		// The leading block can have only one digit, so retain one extra block.
		constexpr size_t RetainedBlocks = ((std::numeric_limits<value_type>::max_digits10 + BlockDigits - 1) / BlockDigits + 1);
		std::array<uint64, RetainedBlocks> blocks;
		Integer remaining = integer, quotient, remainder;
		remaining.backend().sign(false);
		size_t count = 0;

		// Keep only the leading decimal blocks and reuse all three integer buffers.
		do
		{
			boost::multiprecision::divide_qr(remaining, divisor, quotient, remainder);
			remaining.swap(quotient);
			blocks[count % RetainedBlocks] = remainder.convert_to<uint64>();
			++count;
		} while (remaining != 0);

		std::array<char, (RetainedBlocks * BlockDigits + std::numeric_limits<size_t>::digits10 + 4)> buffer;
		char* output = buffer.data();
		char* const end = (buffer.data() + buffer.size() - 1);
		if (backend.sign())
		{
			*output++ = '-';
		}

		const size_t kept = std::min(count, RetainedBlocks);
		for (size_t index = 0; index < kept; ++index)
		{
			const uint64 block = blocks[(count - 1 - index) % RetainedBlocks];
			char* const after = std::to_chars(output, end, block).ptr;
			if (index != 0)
			{
				const size_t length = static_cast<size_t>(after - output);
				std::move_backward(output, after, (output + BlockDigits));
				std::fill(output, (output + BlockDigits - length), '0');
				output += BlockDigits;
			}
			else
			{
				output = after;
			}
		}

		if (count > kept)
		{
			*output++ = 'e';
			output = std::to_chars(output, end, ((count - kept) * BlockDigits)).ptr;
		}
		*output = '\0';
		// Replace the floating-point classification and precision as well as the digits.
		destination = value_type{ buffer.data() };
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BigFloat::BigFloat()
		: pImpl{ std::make_unique<BigFloatDetail>() } {}

	BigFloat::BigFloat(const BigFloat& other)
		: pImpl{ std::make_unique<BigFloatDetail>(*other.pImpl) } {}

	BigFloat::BigFloat(BigFloat&& other) noexcept = default;

	BigFloat::BigFloat(const int64 i)
		: pImpl{ std::make_unique<BigFloatDetail>(i) } {}

	BigFloat::BigFloat(const uint64 i)
		: pImpl{ std::make_unique<BigFloatDetail>(i) } {}

	BigFloat::BigFloat(const long double f)
		: pImpl{ std::make_unique<BigFloatDetail>(f) } {}

	BigFloat::BigFloat(const BigInt& i)
		: pImpl{ std::make_unique<BigFloatDetail>(i) } {}

	BigFloat::BigFloat(const std::string_view number)
		: pImpl{ std::make_unique<BigFloatDetail>(number) } {}

	BigFloat::BigFloat(const StringView number)
		: BigFloat{ Unicode::ToAscii(number) } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	BigFloat::~BigFloat() = default;

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator =(const int64 i)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BigFloatDetail>(i);
			return *this;
		}

		pImpl->value.assign(i);
		return *this;
	}

	BigFloat& BigFloat::operator =(const uint64 i)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BigFloatDetail>(i);
			return *this;
		}

		pImpl->value.assign(i);
		return *this;
	}

	BigFloat& BigFloat::operator =(const long double f)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BigFloatDetail>(f);
			return *this;
		}

		pImpl->value.assign(f);
		return *this;
	}

	BigFloat& BigFloat::operator =(const BigInt& i)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BigFloatDetail>(i);
			return *this;
		}

		BigFloatDetail::AssignFromBigInt(pImpl->value, i);
		return *this;
	}

	BigFloat& BigFloat::operator =(const BigFloat& other)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BigFloatDetail>(*other.pImpl);
			return *this;
		}

		pImpl->value = other.pImpl->value;
		return *this;
	}

	BigFloat& BigFloat::operator =(BigFloat&& other) noexcept
	{
		pImpl = std::move(other.pImpl);
		return *this;
	}

	BigFloat& BigFloat::operator =(const std::string_view number)
	{
		if (not pImpl)
		{
			pImpl = std::make_unique<BigFloatDetail>(number);
			return *this;
		}

		pImpl->value = BigFloatDetail::value_type{ number };
		return *this;
	}

	BigFloat& BigFloat::operator =(const StringView number)
	{
		return (*this = Unicode::ToAscii(number));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	const BigFloat& BigFloat::operator +() const& noexcept
	{
		return *this;
	}

	BigFloat BigFloat::operator +() && noexcept
	{
		return std::move(*this);
	}

	BigFloat BigFloat::operator +(const int64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value + i);
		return tmp;
	}

	BigFloat BigFloat::operator +(const uint64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value + i);
		return tmp;
	}

	BigFloat BigFloat::operator +(const long double f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value + f);
		return tmp;
	}

	BigFloat BigFloat::operator +(const BigInt& i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value + BigFloatDetail::FromBigInt(i));
		return tmp;
	}

	BigFloat BigFloat::operator +(const BigFloat& f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value + f.pImpl->value);
		return tmp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator -() const&
	{
		BigFloat tmp;
		tmp.pImpl->value = -(pImpl->value);
		return tmp;
	}

	BigFloat BigFloat::operator -() && noexcept
	{
		pImpl->value.backend().negate();
		return std::move(*this);
	}

	BigFloat BigFloat::operator -(const int64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value - i);
		return tmp;
	}

	BigFloat BigFloat::operator -(const uint64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value - i);
		return tmp;
	}

	BigFloat BigFloat::operator -(const long double f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value - f);
		return tmp;
	}

	BigFloat BigFloat::operator -(const BigInt& i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value - BigFloatDetail::FromBigInt(i));
		return tmp;
	}

	BigFloat BigFloat::operator -(const BigFloat& f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value - f.pImpl->value);
		return tmp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator *(const int64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value * i);
		return tmp;
	}

	BigFloat BigFloat::operator *(const uint64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value * i);
		return tmp;
	}

	BigFloat BigFloat::operator *(const long double f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value * f);
		return tmp;
	}

	BigFloat BigFloat::operator *(const BigInt& i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value * BigFloatDetail::FromBigInt(i));
		return tmp;
	}

	BigFloat BigFloat::operator *(const BigFloat& f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value * f.pImpl->value);
		return tmp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator /(const int64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value / i);
		return tmp;
	}

	BigFloat BigFloat::operator /(const uint64 i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value / i);
		return tmp;
	}

	BigFloat BigFloat::operator /(const long double f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value / f);
		return tmp;
	}

	BigFloat BigFloat::operator /(const BigInt& i) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value / BigFloatDetail::FromBigInt(i));
		return tmp;
	}

	BigFloat BigFloat::operator /(const BigFloat& f) const
	{
		BigFloat tmp;
		tmp.pImpl->value = (pImpl->value / f.pImpl->value);
		return tmp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator +=(const int64 i)
	{
		pImpl->value += i;
		return *this;
	}

	BigFloat& BigFloat::operator +=(const uint64 i)
	{
		pImpl->value += i;
		return *this;
	}

	BigFloat& BigFloat::operator +=(const long double f)
	{
		pImpl->value += f;
		return *this;
	}

	BigFloat& BigFloat::operator +=(const BigInt& i)
	{
		pImpl->value += BigFloatDetail::FromBigInt(i);
		return *this;
	}

	BigFloat& BigFloat::operator +=(const BigFloat& f)
	{
		pImpl->value += f.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator -=(const int64 i)
	{
		pImpl->value -= i;
		return *this;
	}

	BigFloat& BigFloat::operator -=(const uint64 i)
	{
		pImpl->value -= i;
		return *this;
	}

	BigFloat& BigFloat::operator -=(const long double f)
	{
		pImpl->value -= f;
		return *this;
	}

	BigFloat& BigFloat::operator -=(const BigInt& i)
	{
		pImpl->value -= BigFloatDetail::FromBigInt(i);
		return *this;
	}

	BigFloat& BigFloat::operator -=(const BigFloat& f)
	{
		pImpl->value -= f.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator *=(const int64 i)
	{
		pImpl->value *= i;
		return *this;
	}

	BigFloat& BigFloat::operator *=(const uint64 i)
	{
		pImpl->value *= i;
		return *this;
	}

	BigFloat& BigFloat::operator *=(const long double f)
	{
		pImpl->value *= f;
		return *this;
	}

	BigFloat& BigFloat::operator *=(const BigInt& i)
	{
		pImpl->value *= BigFloatDetail::FromBigInt(i);
		return *this;
	}

	BigFloat& BigFloat::operator *=(const BigFloat& f)
	{
		pImpl->value *= f.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator /=(const int64 i)
	{
		pImpl->value /= i;
		return *this;
	}

	BigFloat& BigFloat::operator /=(const uint64 i)
	{
		pImpl->value /= i;
		return *this;
	}

	BigFloat& BigFloat::operator /=(const long double f)
	{
		pImpl->value /= f;
		return *this;
	}

	BigFloat& BigFloat::operator /=(const BigInt& i)
	{
		pImpl->value /= BigFloatDetail::FromBigInt(i);
		return *this;
	}

	BigFloat& BigFloat::operator /=(const BigFloat& f)
	{
		pImpl->value /= f.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ++
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator ++()
	{
		++pImpl->value;
		return *this;
	}

	BigFloat BigFloat::operator ++(int)
	{
		BigFloat tmp{ *this };
		++pImpl->value;
		return tmp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator --
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator --()
	{
		--pImpl->value;
		return *this;
	}

	BigFloat BigFloat::operator --(int)
	{
		BigFloat tmp{ *this };
		--pImpl->value;
		return tmp;
	}


	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	BigFloat::operator bool() const noexcept
	{
		return (not pImpl->value.backend().iszero());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////

	bool BigFloat::isZero() const noexcept
	{
		return pImpl->value.backend().iszero();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isNaN
	//
	////////////////////////////////////////////////////////////////

	bool BigFloat::isNaN() const noexcept
	{
		return pImpl->value.backend().isnan();
	}

	////////////////////////////////////////////////////////////////
	//
	//	isInf
	//
	////////////////////////////////////////////////////////////////

	bool BigFloat::isInf() const noexcept
	{
		return pImpl->value.backend().isinf();
	}

	////////////////////////////////////////////////////////////////
	//
	//	sign
	//
	////////////////////////////////////////////////////////////////

	int32 BigFloat::sign() const noexcept
	{
		return pImpl->value.sign();
	}

	////////////////////////////////////////////////////////////////
	//
	//	abs
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::abs() const
	{
		BigFloat tmp;
		tmp.pImpl->value = boost::multiprecision::abs(pImpl->value);
		return tmp;
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void BigFloat::swap(BigFloat& other) noexcept
	{
		pImpl.swap(other.pImpl);
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	uint64 BigFloat::hash() const noexcept
	{
		return std::hash<decltype(pImpl->value)>{}(pImpl->value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	asFloat, asDouble, asLongDouble
	//
	////////////////////////////////////////////////////////////////

	float BigFloat::asFloat() const
	{
		return ToNativeFloat<float>(pImpl->value);
	}

	double BigFloat::asDouble() const
	{
		return ToNativeFloat<double>(pImpl->value);
	}

	long double BigFloat::asLongDouble() const
	{
	# if not SIV3D_PLATFORM(WINDOWS)

		if constexpr (std::numeric_limits<long double>::digits != std::numeric_limits<double>::digits
			|| std::numeric_limits<long double>::max_exponent != std::numeric_limits<double>::max_exponent)
		{
			static const auto locale = []
			{
				std::unique_ptr<std::remove_pointer_t<locale_t>, decltype(&freelocale)> result{
					newlocale(LC_NUMERIC_MASK, "C", nullptr), &freelocale };
				if (not result)
				{
					throw std::bad_alloc{};
				}
				return result;
			}();
			const std::string text = pImpl->value.str(0, std::ios_base::fmtflags{});
			const int savedErrno = errno;
			const long double result = strtold_l(text.c_str(), nullptr, locale.get());
			errno = savedErrno;
			return result;
		}

	# endif

		return asDouble();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator float, operator double
	//
	////////////////////////////////////////////////////////////////

	BigFloat::operator float() const
	{
		return asFloat();
	}

	BigFloat::operator double() const
	{
		return asDouble();
	}

	////////////////////////////////////////////////////////////////
	//
	//	to_string
	//
	////////////////////////////////////////////////////////////////

	std::string BigFloat::to_string(const int32 digits, const std::ios_base::fmtflags fmtFlags) const
	{
		std::string result = pImpl->value.str(digits, fmtFlags);

		if (fmtFlags == std::ios_base::fixed)
		{
			return RemoveTrailingZeros(std::move(result));
		}
		else
		{
			return result;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	str
	//
	////////////////////////////////////////////////////////////////

	String BigFloat::str(const int32 digits, const std::ios_base::fmtflags fmtFlags) const
	{
		const std::string t = to_string(digits, fmtFlags);

		return String(t.begin(), t.end());
	}

	////////////////////////////////////////////////////////////////
	//
	//	compare
	//
	////////////////////////////////////////////////////////////////

	std::partial_ordering BigFloat::compare(const int64 i) const noexcept
	{
		if (isNaN())
		{
			return std::partial_ordering::unordered;
		}

		return (pImpl->value.compare(i) <=> 0);
	}

	std::partial_ordering BigFloat::compare(const uint64 i) const noexcept
	{
		if (isNaN())
		{
			return std::partial_ordering::unordered;
		}

		return (pImpl->value.compare(i) <=> 0);
	}

	std::partial_ordering BigFloat::compare(const long double f) const noexcept
	{
		if (isNaN() || std::isnan(f))
		{
			return std::partial_ordering::unordered;
		}

		return (pImpl->value.compare(f) <=> 0);
	}

	std::partial_ordering BigFloat::compare(const BigInt& i) const
	{
		if (isNaN())
		{
			return std::partial_ordering::unordered;
		}

		// コスト節約のため、先に符号で比較する
		const int32 a_sign = pImpl->value.sign();
		const int32 b_sign = i.sign();

		if (a_sign < b_sign)
		{
			return std::partial_ordering::less;
		}
		else if (a_sign > b_sign)
		{
			return std::partial_ordering::greater;
		}

		return (pImpl->value.compare(BigFloatDetail::FromBigInt(i)) <=> 0);
	}

	std::partial_ordering BigFloat::compare(const BigFloat& f) const noexcept
	{
		if (isNaN() || f.isNaN())
		{
			return std::partial_ordering::unordered;
		}

		return (pImpl->value.compare(f.pImpl->value) <=> 0);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const BigFloat& value)
	{
		formatData.string.append(value.str());
	}

	inline namespace Literals
	{
		inline namespace BigNumLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_bigF
			//
			////////////////////////////////////////////////////////////////

			BigFloat operator ""_bigF(const char* s)
			{
				return BigFloat{ std::string_view{ s } };
			}
		}
	}
}
