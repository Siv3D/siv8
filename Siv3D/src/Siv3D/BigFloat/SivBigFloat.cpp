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

# include <Siv3D/BigFloat.hpp>
# include <Siv3D/Unicode.hpp>
# include "BigFloatDetail.hpp"

namespace s3d
{
	namespace
	{
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

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	BigFloat::BigFloat()
		: pImpl{ std::make_unique<BigFloatDetail>() } {}

	BigFloat::BigFloat(const BigFloat& other)
		: pImpl{ std::make_unique<BigFloatDetail>(*other.pImpl) } {}

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
		pImpl->value.assign(i);
		return *this;
	}

	BigFloat& BigFloat::operator =(const uint64 i)
	{
		pImpl->value.assign(i);
		return *this;
	}

	BigFloat& BigFloat::operator =(const long double f)
	{
		pImpl->value.assign(f);
		return *this;
	}

	BigFloat& BigFloat::operator =(const BigInt& i)
	{
		pImpl->value.assign(i._detail().value);
		return *this;
	}

	BigFloat& BigFloat::operator =(const BigFloat& other)
	{
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
		pImpl->value.assign(number);
		return *this;
	}

	BigFloat& BigFloat::operator =(const StringView number)
	{
		pImpl->value.assign(Unicode::ToAscii(number));
		return *this;
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
		tmp.pImpl->value = (pImpl->value + BigFloatDetail::value_type{ i._detail().value });
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
		tmp.pImpl->value = (pImpl->value - BigFloatDetail::value_type{ i._detail().value });
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
		tmp.pImpl->value = (pImpl->value * BigFloatDetail::value_type{ i._detail().value });
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
		tmp.pImpl->value = (pImpl->value / BigFloatDetail::value_type{ i._detail().value });
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
		pImpl->value += BigFloatDetail::value_type{ i._detail().value };
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
		pImpl->value -= BigFloatDetail::value_type{ i._detail().value };
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
		pImpl->value *= BigFloatDetail::value_type{ i._detail().value };
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
		pImpl->value /= BigFloatDetail::value_type{ i._detail().value };
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
		pImpl->value.swap(other.pImpl->value);
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
		return pImpl->value.convert_to<float>();
	}

	double BigFloat::asDouble() const
	{
		return pImpl->value.convert_to<double>();
	}

	long double BigFloat::asLongDouble() const
	{
		return pImpl->value.convert_to<long double>();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator float, operator double
	//
	////////////////////////////////////////////////////////////////

	BigFloat::operator float() const noexcept
	{
		return asFloat();
	}

	BigFloat::operator double() const noexcept
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

	int32 BigFloat::compare(const int64 i) const noexcept
	{
		return pImpl->value.compare(i);
	}

	int32 BigFloat::compare(const uint64 i) const noexcept
	{
		return pImpl->value.compare(i);
	}

	int32 BigFloat::compare(const long double f) const noexcept
	{
		return pImpl->value.compare(f);
	}

	int32 BigFloat::compare(const BigInt& i) const
	{
		// コスト節約のため、先に符号で比較する
		const int32 a_sign = pImpl->value.sign();
		const int32 b_sign = i.sign();

		if (a_sign < b_sign)
		{
			return -1;
		}
		else if (a_sign > b_sign)
		{
			return 1;
		}

		return pImpl->value.compare(BigFloatDetail::value_type{ i._detail().value });
	}

	int32 BigFloat::compare(const BigFloat& f) const noexcept
	{
		return pImpl->value.compare(f.pImpl->value);
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
