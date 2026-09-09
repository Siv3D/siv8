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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BigInt::BigInt(const Concept::SignedIntegral auto i)
		: BigInt{ static_cast<int64>(i) } {}

	BigInt::BigInt(const Concept::UnsignedIntegral auto i)
		: BigInt{ static_cast<uint64>(i) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator =(const Concept::SignedIntegral auto i)
	{
		*this = static_cast<int64>(i);
		return *this;
	}

	BigInt& BigInt::operator =(const Concept::UnsignedIntegral auto i)
	{
		*this = static_cast<uint64>(i);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator +(const Concept::SignedIntegral auto i) const
	{
		return (*this + static_cast<int64>(i));
	}

	BigInt BigInt::operator +(const Concept::UnsignedIntegral auto i) const
	{
		return (*this + static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator -(const Concept::SignedIntegral auto i) const
	{
		return (*this - static_cast<int64>(i));
	}

	BigInt BigInt::operator -(const Concept::UnsignedIntegral auto i) const
	{
		return (*this - static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator *(const Concept::SignedIntegral auto i) const
	{
		return (*this * static_cast<int64>(i));
	}

	BigInt BigInt::operator *(const Concept::UnsignedIntegral auto i) const
	{
		return (*this * static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator /(const Concept::SignedIntegral auto i) const
	{
		return (*this / static_cast<int64>(i));
	}

	BigInt BigInt::operator /(const Concept::UnsignedIntegral auto i) const
	{
		return (*this / static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator %
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator %(const Concept::SignedIntegral auto i) const
	{
		return (*this % static_cast<int64>(i));
	}

	BigInt BigInt::operator %(const Concept::UnsignedIntegral auto i) const
	{
		return (*this % static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator +=(const Concept::SignedIntegral auto i)
	{
		return (*this += static_cast<int64>(i));
	}

	BigInt& BigInt::operator +=(const Concept::UnsignedIntegral auto i)
	{
		return (*this += static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator -=(const Concept::SignedIntegral auto i)
	{
		return (*this -= static_cast<int64>(i));
	}

	BigInt& BigInt::operator -=(const Concept::UnsignedIntegral auto i)
	{
		return (*this -= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator *=(const Concept::SignedIntegral auto i)
	{
		return (*this *= static_cast<int64>(i));
	}

	BigInt& BigInt::operator *=(const Concept::UnsignedIntegral auto i)
	{
		return (*this *= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator /=(const Concept::SignedIntegral auto i)
	{
		return (*this /= static_cast<int64>(i));
	}

	BigInt& BigInt::operator /=(const Concept::UnsignedIntegral auto i)
	{
		return (*this /= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator %=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator %=(const Concept::SignedIntegral auto i)
	{
		return (*this %= static_cast<int64>(i));
	}

	BigInt& BigInt::operator %=(const Concept::UnsignedIntegral auto i)
	{
		return (*this %= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator &
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator &(const Concept::SignedIntegral auto i) const
	{
		return (*this & static_cast<int64>(i));
	}

	BigInt BigInt::operator &(const Concept::UnsignedIntegral auto i) const
	{
		return (*this & static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator |
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator |(const Concept::SignedIntegral auto i) const
	{
		return (*this | static_cast<int64>(i));
	}

	BigInt BigInt::operator |(const Concept::UnsignedIntegral auto i) const
	{
		return (*this | static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ^
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator ^(const Concept::SignedIntegral auto i) const
	{
		return (*this ^ static_cast<int64>(i));
	}

	BigInt BigInt::operator ^(const Concept::UnsignedIntegral auto i) const
	{
		return (*this ^ static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator &=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator &=(const Concept::SignedIntegral auto i)
	{
		return (*this &= static_cast<int64>(i));
	}

	BigInt& BigInt::operator &=(const Concept::UnsignedIntegral auto i)
	{
		return (*this &= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator |=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator |=(const Concept::SignedIntegral auto i)
	{
		return (*this |= static_cast<int64>(i));
	}

	BigInt& BigInt::operator |=(const Concept::UnsignedIntegral auto i)
	{
		return (*this |= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator ^=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator ^=(const Concept::SignedIntegral auto i)
	{
		return (*this ^= static_cast<int64>(i));
	}

	BigInt& BigInt::operator ^=(const Concept::UnsignedIntegral auto i)
	{
		return (*this ^= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator <<(const Concept::SignedIntegral auto i) const
	{
		return (*this << static_cast<int64>(i));
	}

	BigInt BigInt::operator <<(const Concept::UnsignedIntegral auto i) const
	{
		return (*this << static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>
	//
	////////////////////////////////////////////////////////////////

	BigInt BigInt::operator >>(const Concept::SignedIntegral auto i) const
	{
		return (*this >> static_cast<int64>(i));
	}

	BigInt BigInt::operator >>(const Concept::UnsignedIntegral auto i) const
	{
		return (*this >> static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator <<=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator <<=(const Concept::SignedIntegral auto i)
	{
		return (*this <<= static_cast<int64>(i));
	}

	BigInt& BigInt::operator <<=(const Concept::UnsignedIntegral auto i)
	{
		return (*this <<= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator >>=
	//
	////////////////////////////////////////////////////////////////

	BigInt& BigInt::operator >>=(const Concept::SignedIntegral auto i)
	{
		return (*this >>= static_cast<int64>(i));
	}

	BigInt& BigInt::operator >>=(const Concept::UnsignedIntegral auto i)
	{
		return (*this >>= static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	convertTo
	//
	////////////////////////////////////////////////////////////////

	template <Concept::Integral Int>
	Optional<Int> BigInt::convertTo() const noexcept
		requires ((not std::same_as<std::remove_cv_t<Int>, bool>) && (sizeof(Int) <= sizeof(uint64)))
	{
		if constexpr (Concept::SignedIntegral<Int>)
		{
			return _convertToInt64(static_cast<int64>(std::numeric_limits<Int>::min()),
				static_cast<int64>(std::numeric_limits<Int>::max()));
		}
		else
		{
			return _convertToUint64(static_cast<uint64>(std::numeric_limits<Int>::max()));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	inline void BigInt::swap(BigInt& other) noexcept
	{
		pImpl.swap(other.pImpl);
	}

	////////////////////////////////////////////////////////////////
	//
	//	compare
	//
	////////////////////////////////////////////////////////////////
	
	int32 BigInt::compare(const Concept::SignedIntegral auto i) const noexcept
	{
		return compare(static_cast<int64>(i));
	}

	int32 BigInt::compare(const Concept::UnsignedIntegral auto i) const noexcept
	{
		return compare(static_cast<uint64>(i));
	}

	////////////////////////////////////////////////////////////////
	//
	//	_detail (internal)
	//
	////////////////////////////////////////////////////////////////

	inline BigInt::BigIntDetail& BigInt::_detail() noexcept
	{
		return *pImpl;
	}

	inline const BigInt::BigIntDetail& BigInt::_detail() const noexcept
	{
		return *pImpl;
	}
}
