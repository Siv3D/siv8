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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BigFloat::BigFloat(const Concept::SignedIntegral auto i)
		: BigFloat{ static_cast<int64>(i) } {}

	BigFloat::BigFloat(const Concept::UnsignedIntegral auto i)
		: BigFloat{ static_cast<uint64>(i) } {}

	BigFloat::BigFloat(const Concept::FloatingPoint auto f)
		: BigFloat{ static_cast<long double>(f) } {}

	////////////////////////////////////////////////////////////////
	//
	//	operator =
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator =(const Concept::SignedIntegral auto i)
	{
		*this = static_cast<int64>(i);
		return *this;
	}

	BigFloat& BigFloat::operator =(const Concept::UnsignedIntegral auto i)
	{
		*this = static_cast<uint64>(i);
		return *this;
	}

	BigFloat& BigFloat::operator =(Concept::FloatingPoint auto f)
	{
		*this = static_cast<long double>(f);
		return *this;
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator +(const Concept::SignedIntegral auto i) const
	{
		return (*this + static_cast<int64>(i));
	}

	BigFloat BigFloat::operator +(const Concept::UnsignedIntegral auto i) const
	{
		return (*this + static_cast<uint64>(i));
	}

	BigFloat BigFloat::operator +(const Concept::FloatingPoint auto f) const
	{
		return (*this + static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator -(const Concept::SignedIntegral auto i) const
	{
		return (*this - static_cast<int64>(i));
	}

	BigFloat BigFloat::operator -(const Concept::UnsignedIntegral auto i) const
	{
		return (*this - static_cast<uint64>(i));
	}

	BigFloat BigFloat::operator -(const Concept::FloatingPoint auto f) const
	{
		return (*this - static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator *(const Concept::SignedIntegral auto i) const
	{
		return (*this * static_cast<int64>(i));
	}

	BigFloat BigFloat::operator *(const Concept::UnsignedIntegral auto i) const
	{
		return (*this * static_cast<uint64>(i));
	}

	BigFloat BigFloat::operator *(const Concept::FloatingPoint auto f) const
	{
		return (*this * static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /
	//
	////////////////////////////////////////////////////////////////

	BigFloat BigFloat::operator /(const Concept::SignedIntegral auto i) const
	{
		return (*this / static_cast<int64>(i));
	}

	BigFloat BigFloat::operator /(const Concept::UnsignedIntegral auto i) const
	{
		return (*this / static_cast<uint64>(i));
	}

	BigFloat BigFloat::operator /(const Concept::FloatingPoint auto f) const
	{
		return (*this / static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator +=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator +=(const Concept::SignedIntegral auto i)
	{
		return (*this += static_cast<int64>(i));
	}

	BigFloat& BigFloat::operator +=(const Concept::UnsignedIntegral auto i)
	{
		return (*this += static_cast<uint64>(i));
	}

	BigFloat& BigFloat::operator +=(const Concept::FloatingPoint auto f)
	{
		return (*this += static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator -=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator -=(const Concept::SignedIntegral auto i)
	{
		return (*this -= static_cast<int64>(i));
	}

	BigFloat& BigFloat::operator -=(const Concept::UnsignedIntegral auto i)
	{
		return (*this -= static_cast<uint64>(i));
	}

	BigFloat& BigFloat::operator -=(const Concept::FloatingPoint auto f)
	{
		return (*this -= static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator *=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator *=(const Concept::SignedIntegral auto i)
	{
		return (*this *= static_cast<int64>(i));
	}

	BigFloat& BigFloat::operator *=(const Concept::UnsignedIntegral auto i)
	{
		return (*this *= static_cast<uint64>(i));
	}

	BigFloat& BigFloat::operator *=(const Concept::FloatingPoint auto f)
	{
		return (*this *= static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator /=
	//
	////////////////////////////////////////////////////////////////

	BigFloat& BigFloat::operator /=(const Concept::SignedIntegral auto i)
	{
		return (*this /= static_cast<int64>(i));
	}

	BigFloat& BigFloat::operator /=(const Concept::UnsignedIntegral auto i)
	{
		return (*this /= static_cast<uint64>(i));
	}

	BigFloat& BigFloat::operator /=(const Concept::FloatingPoint auto f)
	{
		return (*this /= static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	compare
	//
	////////////////////////////////////////////////////////////////

	int32 BigFloat::compare(const Concept::SignedIntegral auto i) const noexcept
	{
		return compare(static_cast<int64>(i));
	}

	int32 BigFloat::compare(const Concept::UnsignedIntegral auto i) const noexcept
	{
		return compare(static_cast<uint64>(i));
	}

	int32 BigFloat::compare(const Concept::FloatingPoint auto f) const noexcept
	{
		return compare(static_cast<long double>(f));
	}

	////////////////////////////////////////////////////////////////
	//
	//	_detail (internal)
	//
	////////////////////////////////////////////////////////////////

	inline BigFloat::BigFloatDetail& BigFloat::_detail() noexcept
	{
		return *pImpl;
	}

	inline const BigFloat::BigFloatDetail& BigFloat::_detail() const noexcept
	{
		return *pImpl;
	}
}
