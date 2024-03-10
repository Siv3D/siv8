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

# pragma once
# include <memory>
# include "Common.hpp"
# include "Concepts.hpp"
# include "String.hpp"
# include "FormatData.hpp"

namespace s3d
{
	class BigFloat;

	/// @brief 多倍長整数型
	class BigInt
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt();

		[[nodiscard]]
		BigInt(const BigInt& other);

		[[nodiscard]]
		BigInt(BigInt&& other) noexcept = default;

		[[nodiscard]]
		BigInt(Concept::SignedIntegral auto i);

		[[nodiscard]]
		BigInt(Concept::UnsignedIntegral auto i);

		[[nodiscard]]
		BigInt(int64 i);

		[[nodiscard]]
		BigInt(uint64 i);

		[[nodiscard]]
		explicit BigInt(std::string_view number);

		[[nodiscard]]
		explicit BigInt(StringView number);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~BigInt();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator =(int64 i);

		BigInt& operator =(uint64 i);

		BigInt& operator =(Concept::SignedIntegral auto i);

		BigInt& operator =(Concept::UnsignedIntegral auto i);

		BigInt& operator =(const BigInt& other);

		BigInt& operator =(BigInt&& other) noexcept;

		BigInt& operator =(std::string_view number);

		BigInt& operator =(StringView number);

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const BigInt& operator +() const;

		[[nodiscard]]
		BigInt operator +(int64 i) const;

		[[nodiscard]]
		BigInt operator +(uint64 i) const;

		[[nodiscard]]
		BigInt operator +(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator +(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator +(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator +(const Concept::Integral auto a, const BigInt& b)
		{
			return (b + a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator -() const&;

		[[nodiscard]]
		BigInt operator -()&&;

		[[nodiscard]]
		BigInt operator -(int64 i) const;

		[[nodiscard]]
		BigInt operator -(uint64 i) const;

		[[nodiscard]]
		BigInt operator -(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator -(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator -(const BigInt& i) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator *(int64 i) const;

		[[nodiscard]]
		BigInt operator *(uint64 i) const;

		[[nodiscard]]
		BigInt operator *(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator *(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator *(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator *(Concept::Integral auto a, const BigInt& b)
		{
			return (b * a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator /(int64 i) const;

		[[nodiscard]]
		BigInt operator /(uint64 i) const;

		[[nodiscard]]
		BigInt operator /(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator /(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator /(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator /(Concept::SignedIntegral auto a, const BigInt& b)
		{
			return (static_cast<int64>(a) / b);
		}

		[[nodiscard]]
		friend BigInt operator /(const Concept::UnsignedIntegral auto a, const BigInt& b)
		{
			return (static_cast<uint64>(a) / b);
		}

		[[nodiscard]]
		friend BigInt operator /(const int64 a, const BigInt& b)
		{
			return b._divI(a);
		}

		[[nodiscard]]
		friend BigInt operator /(const uint64 a, const BigInt& b)
		{
			return b._divI(a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator %
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator %(int64 i) const;

		[[nodiscard]]
		BigInt operator %(uint64 i) const;

		[[nodiscard]]
		BigInt operator %(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator %(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator %(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator %(Concept::SignedIntegral auto a, const BigInt& b)
		{
			return (static_cast<int64>(a) % b);
		}

		[[nodiscard]]
		friend BigInt operator %(const Concept::UnsignedIntegral auto a, const BigInt& b)
		{
			return (static_cast<uint64>(a) % b);
		}

		[[nodiscard]]
		friend BigInt operator %(const int64 a, const BigInt& b)
		{
			return b._modI(a);
		}

		[[nodiscard]]
		friend BigInt operator %(const uint64 a, const BigInt& b)
		{
			return b._modI(a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator +=(int64 i);

		BigInt& operator +=(uint64 i);

		BigInt& operator +=(Concept::SignedIntegral auto i);

		BigInt& operator +=(Concept::UnsignedIntegral auto i);

		BigInt& operator +=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator -=(int64 i);

		BigInt& operator -=(uint64 i);

		BigInt& operator -=(Concept::SignedIntegral auto i);

		BigInt& operator -=(Concept::UnsignedIntegral auto i);

		BigInt& operator -=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator *=(int64 i);

		BigInt& operator *=(uint64 i);

		BigInt& operator *=(Concept::SignedIntegral auto i);

		BigInt& operator *=(Concept::UnsignedIntegral auto i);

		BigInt& operator *=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator /=(int64 i);

		BigInt& operator /=(uint64 i);

		BigInt& operator /=(Concept::SignedIntegral auto i);

		BigInt& operator /=(Concept::UnsignedIntegral auto i);

		BigInt& operator /=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator %=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator %=(int64 i);

		BigInt& operator %=(uint64 i);

		BigInt& operator %=(Concept::SignedIntegral auto i);

		BigInt& operator %=(Concept::UnsignedIntegral auto i);

		BigInt& operator %=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator ++
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator ++()&;

		BigInt operator ++()&&;

		BigInt operator ++(int);

		////////////////////////////////////////////////////////////////
		//
		//	operator --
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator --()&;

		BigInt operator --()&&;

		BigInt operator --(int);

		////////////////////////////////////////////////////////////////
		//
		//	operator &
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator &(int64 i) const;

		[[nodiscard]]
		BigInt operator &(uint64 i) const;

		[[nodiscard]]
		BigInt operator &(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator &(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator &(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator &(Concept::Integral auto a, const BigInt& b)
		{
			return (b & a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator |
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator |(int64 i) const;

		[[nodiscard]]
		BigInt operator |(uint64 i) const;

		[[nodiscard]]
		BigInt operator |(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator |(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator |(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator |(Concept::Integral auto a, const BigInt& b)
		{
			return (b | a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator ^
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator ^(int64 i) const;

		[[nodiscard]]
		BigInt operator ^(uint64 i) const;

		[[nodiscard]]
		BigInt operator ^(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator ^(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator ^(const BigInt& i) const;

		[[nodiscard]]
		friend BigInt operator ^(Concept::Integral auto a, const BigInt& b)
		{
			return (b ^ a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator &=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator &=(int64 i);

		BigInt& operator &=(uint64 i);

		BigInt& operator &=(Concept::SignedIntegral auto i);

		BigInt& operator &=(Concept::UnsignedIntegral auto i);

		BigInt& operator &=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator |=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator |=(int64 i);

		BigInt& operator |=(uint64 i);

		BigInt& operator |=(Concept::SignedIntegral auto i);

		BigInt& operator |=(Concept::UnsignedIntegral auto i);

		BigInt& operator |=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator ^=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator ^=(int64 i);

		BigInt& operator ^=(uint64 i);

		BigInt& operator ^=(Concept::SignedIntegral auto i);

		BigInt& operator ^=(Concept::UnsignedIntegral auto i);

		BigInt& operator ^=(const BigInt& i);

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator <<(int64 i) const;

		[[nodiscard]]
		BigInt operator <<(uint64 i) const;

		[[nodiscard]]
		BigInt operator <<(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator <<(Concept::UnsignedIntegral auto i) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt operator >>(int64 i) const;

		[[nodiscard]]
		BigInt operator >>(uint64 i) const;

		[[nodiscard]]
		BigInt operator >>(Concept::SignedIntegral auto i) const;

		[[nodiscard]]
		BigInt operator >>(Concept::UnsignedIntegral auto i) const;

		////////////////////////////////////////////////////////////////
		//
		//	operator <<=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator <<=(int64 i);

		BigInt& operator <<=(uint64 i);

		BigInt& operator <<=(Concept::SignedIntegral auto i);

		BigInt& operator <<=(Concept::UnsignedIntegral auto i);

		////////////////////////////////////////////////////////////////
		//
		//	operator >>=
		//
		////////////////////////////////////////////////////////////////

		BigInt& operator >>=(int64 i);

		BigInt& operator >>=(uint64 i);

		BigInt& operator >>=(Concept::SignedIntegral auto i);

		BigInt& operator >>=(Concept::UnsignedIntegral auto i);

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend bool operator ==(const BigInt& a, const BigInt& b) noexcept
		{
			return (a.compare(b) == 0);
		}

		[[nodiscard]]
		friend bool operator ==(const BigInt& a, const Concept::Arithmetic auto b) noexcept
		{
			return (a.compare(b) == 0);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend auto operator <=>(const BigInt& a, const BigInt& b) noexcept
		{
			return (a.compare(b) <=> 0);
		}

		[[nodiscard]]
		friend auto operator <=>(const BigInt& a, const Concept::Arithmetic auto b) noexcept
		{
			return (a.compare(b) <=> 0);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isZero
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sign
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 sign() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	abs
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt abs() const;

		////////////////////////////////////////////////////////////////
		//
		//	pow
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt pow(uint32 x) const;

		////////////////////////////////////////////////////////////////
		//
		//	divmod
		//
		////////////////////////////////////////////////////////////////

		void divmod(const BigInt& x, BigInt& q, BigInt& r) const;

		////////////////////////////////////////////////////////////////
		//
		//	lsb
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 lsb() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	msb
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 msb() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	bitTest
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool bitTest(uint32 index) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	bitSet
		//
		////////////////////////////////////////////////////////////////

		BigInt& bitSet(uint32 index, bool value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	bitFlip
		//
		////////////////////////////////////////////////////////////////

		BigInt& bitFlip(uint32 index) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	gcd
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt gcd(const BigInt& x) const;

		////////////////////////////////////////////////////////////////
		//
		//	lcm
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt lcm(const BigInt& x) const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		void swap(BigInt& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asInt32, asUint32
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 asInt32() const noexcept;

		[[nodiscard]]
		uint32 asUint32() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asInt64, asUint64
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int64 asInt64() const noexcept;

		[[nodiscard]]
		uint64 asUint64() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asFloat, asDouble
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		float asFloat() const noexcept;

		[[nodiscard]]
		double asDouble() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asBigFloat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigFloat asBigFloat() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator size_t
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator size_t() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator float, operator double
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator float() const noexcept;

		[[nodiscard]]
		explicit operator double() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator BigFloat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		explicit operator BigFloat() const;

		////////////////////////////////////////////////////////////////
		//
		//	to_string
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		std::string to_string() const;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String str() const;

		////////////////////////////////////////////////////////////////
		//
		//	compare
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 compare(int64 i) const noexcept;

		[[nodiscard]]
		int32 compare(uint64 i) const noexcept;

		[[nodiscard]]
		int32 compare(float f) const noexcept;

		[[nodiscard]]
		int32 compare(double f) const noexcept;

		[[nodiscard]]
		int32 compare(long double f) const noexcept;

		[[nodiscard]]
		int32 compare(Concept::SignedIntegral auto i) const noexcept;

		[[nodiscard]]
		int32 compare(Concept::UnsignedIntegral auto i) const noexcept;

		[[nodiscard]]
		int32 compare(const BigInt& i) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(BigInt& lhs, BigInt& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const BigInt& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, BigInt& value)
		{
			if (String s; input >> s)
			{
				value = s;
			}

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const BigInt& value);

	private:

		struct BigIntDetail;

		std::unique_ptr<BigIntDetail> pImpl;

		BigInt _divI(int64 a) const;
		BigInt _divI(uint64 a) const;
		BigInt _modI(int64 a) const;
		BigInt _modI(uint64 a) const;
	};

	inline namespace Literals
	{
		inline namespace BigNumLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_big
			//
			////////////////////////////////////////////////////////////////

			[[nodiscard]]
			BigInt operator ""_big(const char* s);

			/// @remark BigFloat 用のサフィックスは _bigF です。
			[[nodiscard]]
			BigInt operator ""_big(long double) = delete;
		}
	}

	namespace Math
	{
		////////////////////////////////////////////////////////////////
		//
		//	GCD
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt GCD(const BigInt& a, const BigInt& b);

		////////////////////////////////////////////////////////////////
		//
		//	LCM
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigInt LCM(const BigInt& a, const BigInt& b);
	}
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::BigInt>
{
	[[nodiscard]]
	size_t operator ()(const s3d::BigInt& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/BigInt.ipp"
