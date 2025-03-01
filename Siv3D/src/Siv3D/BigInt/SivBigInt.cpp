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

# include <Siv3D/BigInt.hpp>
# include <Siv3D/BigFloat.hpp>
# include <Siv3D/Unicode.hpp>
# include "BigIntDetail.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BigInt::BigInt()
		: pImpl{ std::make_unique<BigIntDetail>() } {}

	BigInt::BigInt(const BigInt& other)
		: pImpl{ std::make_unique<BigIntDetail>(*other.pImpl) } {}

	BigInt::BigInt(const int64 i)
		: pImpl{ std::make_unique<BigIntDetail>(i) } {}

	BigInt::BigInt(const uint64 i)
		: pImpl{ std::make_unique<BigIntDetail>(i) } {}

	BigInt::BigInt(const std::string_view number)
		: pImpl{ std::make_unique<BigIntDetail>(number) } {}

	BigInt::BigInt(const StringView number)
		: BigInt{ Unicode::ToAscii(number) } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	BigInt::~BigInt() = default;

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator =(const int64 i)
	{
		pImpl->value.assign(i);
		return *this;
	}

	BigInt& BigInt::operator =(const uint64 i)
	{
		pImpl->value.assign(i);
		return *this;
	}

	BigInt& BigInt::operator =(const BigInt& other)
	{
		pImpl->value = other.pImpl->value;
		return *this;
	}

	BigInt& BigInt::operator =(BigInt&& other) noexcept
	{
		pImpl = std::move(other.pImpl);
		return *this;
	}

	BigInt& BigInt::operator =(const std::string_view number)
	{
		pImpl->value.assign(number);
		return *this;
	}

	BigInt& BigInt::operator =(const StringView number)
	{
		pImpl->value.assign(Unicode::ToAscii(number));
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	const BigInt& BigInt::operator +() const& noexcept
	{
		return *this;
	}

	BigInt BigInt::operator +()&& noexcept
	{
		return std::move(*this);
	}

	BigInt BigInt::operator +(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value + i);
		return result;
	}

	BigInt BigInt::operator +(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value + i);
		return result;
	}

	BigInt BigInt::operator +(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value + i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator -() const&
	{
		BigInt result;
		result.pImpl->value = -pImpl->value;
		return result;
	}

	BigInt BigInt::operator -()&&
	{
		pImpl->value = -pImpl->value;
		return std::move(*this);
	}

	BigInt BigInt::operator -(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value - i);
		return result;
	}

	BigInt BigInt::operator -(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value - i);
		return result;
	}

	BigInt BigInt::operator -(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value - i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator *(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value * i);
		return result;
	}

	BigInt BigInt::operator *(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value * i);
		return result;
	}

	BigInt BigInt::operator *(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value * i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator /(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value / i);
		return result;
	}

	BigInt BigInt::operator /(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value / i);
		return result;
	}

	BigInt BigInt::operator /(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value / i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator %
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator %(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value % i);
		return result;
	}

	BigInt BigInt::operator %(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value % i);
		return result;
	}

	BigInt BigInt::operator %(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value % i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator +=(const int64 i)
	{
		pImpl->value += i;
		return *this;
	}

	BigInt& BigInt::operator +=(const uint64 i)
	{
		pImpl->value += i;
		return *this;
	}

	BigInt& BigInt::operator +=(const BigInt& i)
	{
		pImpl->value += i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator -=(const int64 i)
	{
		pImpl->value -= i;
		return *this;
	}

	BigInt& BigInt::operator -=(const uint64 i)
	{
		pImpl->value -= i;
		return *this;
	}

	BigInt& BigInt::operator -=(const BigInt& i)
	{
		pImpl->value -= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator *=(const int64 i)
	{
		pImpl->value *= i;
		return *this;
	}

	BigInt& BigInt::operator *=(const uint64 i)
	{
		pImpl->value *= i;
		return *this;
	}

	BigInt& BigInt::operator *=(const BigInt& i)
	{
		pImpl->value *= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator /=(const int64 i)
	{
		pImpl->value /= i;
		return *this;
	}

	BigInt& BigInt::operator /=(const uint64 i)
	{
		pImpl->value /= i;
		return *this;
	}

	BigInt& BigInt::operator /=(const BigInt& i)
	{
		pImpl->value /= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator %=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator %=(const int64 i)
	{
		pImpl->value %= i;
		return *this;
	}

	BigInt& BigInt::operator %=(const uint64 i)
	{
		pImpl->value %= i;
		return *this;
	}

	BigInt& BigInt::operator %=(const BigInt& i)
	{
		pImpl->value %= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ++
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator ++()&
	{
		++pImpl->value;
		return *this;
	}

	BigInt BigInt::operator ++()&&
	{
		++pImpl->value;
		return std::move(*this);
	}

	BigInt BigInt::operator ++(int)
	{
		BigInt result{ *this };
		++pImpl->value;
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator --
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator --()&
	{
		--pImpl->value;
		return *this;
	}

	BigInt BigInt::operator --()&&
	{
		--pImpl->value;
		return std::move(*this);
	}

	BigInt BigInt::operator --(int)
	{
		BigInt result{ *this };
		--pImpl->value;
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator &
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator &(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value & i);
		return result;
	}

	BigInt BigInt::operator &(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value & i);
		return result;
	}

	BigInt BigInt::operator &(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value & i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator |(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value | i);
		return result;
	}

	BigInt BigInt::operator |(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value | i);
		return result;
	}

	BigInt BigInt::operator |(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value | i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ^
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator ^(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value ^ i);
		return result;
	}

	BigInt BigInt::operator ^(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value ^ i);
		return result;
	}

	BigInt BigInt::operator ^(const BigInt& i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value ^ i.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator &=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator &=(const int64 i)
	{
		pImpl->value &= i;
		return *this;
	}

	BigInt& BigInt::operator &=(const uint64 i)
	{
		pImpl->value &= i;
		return *this;
	}

	BigInt& BigInt::operator &=(const BigInt& i)
	{
		pImpl->value &= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator |=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator |=(const int64 i)
	{
		pImpl->value |= i;
		return *this;
	}

	BigInt& BigInt::operator |=(const uint64 i)
	{
		pImpl->value |= i;
		return *this;
	}

	BigInt& BigInt::operator |=(const BigInt& i)
	{
		pImpl->value |= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ^=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator ^=(const int64 i)
	{
		pImpl->value ^= i;
		return *this;
	}

	BigInt& BigInt::operator ^=(const uint64 i)
	{
		pImpl->value ^= i;
		return *this;
	}

	BigInt& BigInt::operator ^=(const BigInt& i)
	{
		pImpl->value ^= i.pImpl->value;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator <<(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value << i);
		return result;
	}

	BigInt BigInt::operator <<(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value << i);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator >>(const int64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value >> i);
		return result;
	}

	BigInt BigInt::operator >>(const uint64 i) const
	{
		BigInt result;
		result.pImpl->value = (pImpl->value >> i);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator <<=(const int64 i)
	{
		pImpl->value <<= i;
		return *this;
	}

	BigInt& BigInt::operator <<=(const uint64 i)
	{
		pImpl->value <<= i;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator >>=(const int64 i)
	{
		pImpl->value >>= i;
		return *this;
	}

	BigInt& BigInt::operator >>=(const uint64 i)
	{
		pImpl->value >>= i;
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	BigInt::operator bool() const noexcept
	{
		return (not pImpl->value.is_zero());
	}

	////////////////////////////////////////////////////////////////
	//
	//	isZero
	//
	////////////////////////////////////////////////////////////////

	bool BigInt::isZero() const noexcept
	{
		return pImpl->value.is_zero();
	}

	////////////////////////////////////////////////////////////////
	//
	//	sign
	//
	////////////////////////////////////////////////////////////////

	int32 BigInt::sign() const noexcept
	{
		return pImpl->value.sign();
	}

	////////////////////////////////////////////////////////////////
	//
	//	abs
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::abs() const
	{
		BigInt result;
		result.pImpl->value = boost::multiprecision::abs(pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pow
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::pow(const uint32 x) const
	{
		BigInt result;
		result.pImpl->value = boost::multiprecision::pow(pImpl->value, x);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	divmod
	//
	////////////////////////////////////////////////////////////////

	void BigInt::divmod(const BigInt& x, BigInt& q, BigInt& r) const
	{
		boost::multiprecision::divide_qr(pImpl->value, x.pImpl->value, q.pImpl->value, r.pImpl->value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	lsb
	//
	////////////////////////////////////////////////////////////////

	Optional<uint64> BigInt::lsb() const noexcept
	{
		if (pImpl->value.sign() <= 0)
		{
			return none;
		}
		
		return boost::multiprecision::lsb(pImpl->value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	msb
	//
	////////////////////////////////////////////////////////////////

	Optional<uint64> BigInt::msb() const noexcept
	{
		if (pImpl->value.sign() <= 0)
		{
			return none;
		}
		
		return boost::multiprecision::msb(pImpl->value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	bitTest
	//
	////////////////////////////////////////////////////////////////

	bool BigInt::bitTest(const uint32 index) const noexcept
	{
		return boost::multiprecision::bit_test(pImpl->value, index);
	}

	////////////////////////////////////////////////////////////////
	//
	//	bitSet
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::bitSet(const uint32 index, const bool value) noexcept
	{
		if (value)
		{
			boost::multiprecision::bit_set(pImpl->value, index);
		}
		else
		{
			boost::multiprecision::bit_unset(pImpl->value, index);
		}

		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	bitFlip
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::bitFlip(const uint32 index) noexcept
	{
		boost::multiprecision::bit_flip(pImpl->value, index);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	gcd
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::gcd(const BigInt& x) const
	{
		BigInt result;
		result.pImpl->value = boost::multiprecision::gcd(pImpl->value, x.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	lcm
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::lcm(const BigInt& x) const
	{
		BigInt result;
		result.pImpl->value = boost::multiprecision::lcm(pImpl->value, x.pImpl->value);
		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	hash
	//
	////////////////////////////////////////////////////////////////

	uint64 BigInt::hash() const noexcept
	{
		return std::hash<boost::multiprecision::cpp_int>{}(pImpl->value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	asInt32, asUint32
	//
	////////////////////////////////////////////////////////////////

	int32 BigInt::asInt32() const noexcept
	{
		return pImpl->value.convert_to<int32>();
	}

	uint32 BigInt::asUint32() const noexcept
	{
		return pImpl->value.convert_to<uint32>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asInt64, asUint64
	//
	////////////////////////////////////////////////////////////////

	int64 BigInt::asInt64() const noexcept
	{
		return pImpl->value.convert_to<int64>();
	}

	uint64 BigInt::asUint64() const noexcept
	{
		return pImpl->value.convert_to<uint64>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asFloat, asDouble
	//
	////////////////////////////////////////////////////////////////

	float BigInt::asFloat() const noexcept
	{
		return pImpl->value.convert_to<float>();
	}

	double BigInt::asDouble() const noexcept
	{
		return pImpl->value.convert_to<double>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	asBigFloat
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigInt::asBigFloat() const
	{
		return BigFloat{ *this };
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator size_t
	//
	////////////////////////////////////////////////////////////////

	BigInt::operator size_t() const noexcept
	{
		return pImpl->value.convert_to<size_t>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator float, operator double
	//
	////////////////////////////////////////////////////////////////

	BigInt::operator float() const noexcept
	{
		return asFloat();
	}

	BigInt::operator double() const noexcept
	{
		return asDouble();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator BigFloat
	//
	////////////////////////////////////////////////////////////////

	BigInt::operator BigFloat() const
	{
		return asBigFloat();
	}

	////////////////////////////////////////////////////////////////
	//
	//	to_string
	//
	////////////////////////////////////////////////////////////////

	std::string BigInt::to_string() const
	{
		return pImpl->value.str();
	}

	////////////////////////////////////////////////////////////////
	//
	//	str
	//
	////////////////////////////////////////////////////////////////

	String BigInt::str() const
	{
		return Unicode::FromAscii(to_string());
	}

	//////////////////////////////////////////////////
	//
	//	compare
	//
	//////////////////////////////////////////////////

	int32 BigInt::compare(const int64 i) const noexcept
	{
		return this->pImpl->value.compare(i);
	}

	int32 BigInt::compare(const uint64 i) const noexcept
	{
		return this->pImpl->value.compare(i);
	}

	int32 BigInt::compare(const float i) const noexcept
	{
		return this->pImpl->value.compare(i);
	}

	int32 BigInt::compare(const double i) const noexcept
	{
		return this->pImpl->value.compare(i);
	}

	int32 BigInt::compare(const long double i) const noexcept
	{
		return this->pImpl->value.compare(i);
	}

	int32 BigInt::compare(const BigInt& i) const noexcept
	{
		return this->pImpl->value.compare(i.pImpl->value);
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const BigInt& value)
	{
		formatData.string.append(value.str());
	}

	////////////////////////////////////////////////////////////////
	//
	//	(private function)
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::_divI(const int64 a) const
	{
		BigInt result;
		result.pImpl->value = (a / pImpl->value);
		return result;
	}

	BigInt BigInt::_divI(const uint64 a) const
	{
		BigInt result;
		result.pImpl->value = (a / pImpl->value);
		return result;
	}

	BigInt BigInt::_modI(const int64 a) const
	{
		BigInt result;
		result.pImpl->value = (a % pImpl->value);
		return result;
	}

	BigInt BigInt::_modI(const uint64 a) const
	{
		BigInt result;
		result.pImpl->value = (a % pImpl->value);
		return result;
	}

	inline namespace Literals
	{
		inline namespace BigNumLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_big
			//
			////////////////////////////////////////////////////////////////

			BigInt operator ""_big(const char* s)
			{
				return BigInt{ std::string_view{ s } };
			}
		}
	}
}
