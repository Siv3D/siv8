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
	class BigInt;

	////////////////////////////////////////////////////////////////
	//
	//	BigFloat
	//
	////////////////////////////////////////////////////////////////

	/// @brief 多倍長浮動小数点数（有効桁数 100 桁）
	class BigFloat
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigFloat();

		[[nodiscard]]
		BigFloat(const BigFloat& other);

		[[nodiscard]]
		BigFloat(BigFloat&& other) noexcept = default;

		[[nodiscard]]
		BigFloat(Concept::SignedIntegral auto i);

		[[nodiscard]]
		BigFloat(Concept::UnsignedIntegral auto i);

		[[nodiscard]]
		BigFloat(int64 i);

		[[nodiscard]]
		BigFloat(uint64 i);
	
		[[nodiscard]]
		BigFloat(long double f);

		[[nodiscard]]
		BigFloat(Concept::FloatingPoint auto f);
		
		[[nodiscard]]
		BigFloat(const BigInt& i);
		
		[[nodiscard]]
		explicit BigFloat(std::string_view number);
		
		[[nodiscard]]
		explicit BigFloat(StringView number);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		~BigFloat();

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator =(int64 i);

		BigFloat& operator =(uint64 i);

		BigFloat& operator =(Concept::SignedIntegral auto i);

		BigFloat& operator =(Concept::UnsignedIntegral auto i);

		BigFloat& operator =(long double f);

		BigFloat& operator =(Concept::FloatingPoint auto f);

		BigFloat& operator =(const BigInt& i);

		BigFloat& operator =(const BigFloat& other);

		BigFloat& operator =(BigFloat&& other) noexcept;

		BigFloat& operator =(std::string_view number);

		BigFloat& operator =(StringView number);

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		const BigFloat& operator +() const& noexcept;

		[[nodiscard]]
		BigFloat operator +()&& noexcept;

		[[nodiscard]]
		BigFloat operator +(int64 i) const;

		[[nodiscard]]
		BigFloat operator +(uint64 i) const;

		[[nodiscard]]
		BigFloat operator +(Concept::SignedIntegral auto  i) const;

		[[nodiscard]]
		BigFloat operator +(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigFloat operator +(long double f) const;

		[[nodiscard]]
		BigFloat operator +(Concept::FloatingPoint auto f) const;

		[[nodiscard]]
		BigFloat operator +(const BigInt& i) const;
		
		[[nodiscard]]
		BigFloat operator +(const BigFloat& f) const;

		[[nodiscard]]
		friend BigFloat operator +(const Concept::Arithmetic auto a, const BigFloat& b)
		{
			return (b + a);
		}

		[[nodiscard]]
		friend BigFloat operator +(const BigInt& a, const BigFloat& b)
		{
			return (b + a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigFloat operator -() const&;

		[[nodiscard]]
		BigFloat operator -()&& noexcept;

		[[nodiscard]]
		BigFloat operator -(int64 i) const;

		[[nodiscard]]
		BigFloat operator -(uint64 i) const;

		[[nodiscard]]
		BigFloat operator -(Concept::SignedIntegral auto  i) const;

		[[nodiscard]]
		BigFloat operator -(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigFloat operator -(long double f) const;

		[[nodiscard]]
		BigFloat operator -(Concept::FloatingPoint auto f) const;

		[[nodiscard]]
		BigFloat operator -(const BigInt& i) const;

		[[nodiscard]]
		BigFloat operator -(const BigFloat& f) const;

		[[nodiscard]]
		friend BigFloat operator -(const Concept::Arithmetic auto a, const BigFloat& b)
		{
			return (b - a);
		}

		[[nodiscard]]
		friend BigFloat operator -(const BigInt& a, const BigFloat& b)
		{
			return (-b + a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator *
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigFloat operator *(int64 i) const;

		[[nodiscard]]
		BigFloat operator *(uint64 i) const;

		[[nodiscard]]
		BigFloat operator *(Concept::SignedIntegral auto  i) const;

		[[nodiscard]]
		BigFloat operator *(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigFloat operator *(long double f) const;

		[[nodiscard]]
		BigFloat operator *(Concept::FloatingPoint auto f) const;

		[[nodiscard]]
		BigFloat operator *(const BigInt& i) const;

		[[nodiscard]]
		BigFloat operator *(const BigFloat& f) const;

		[[nodiscard]]
		friend BigFloat operator *(const Concept::Arithmetic auto a, const BigFloat& b)
		{
			return (b * a);
		}

		[[nodiscard]]
		friend BigFloat operator *(const BigInt& a, const BigFloat& b)
		{
			return (b * a);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator /
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigFloat operator /(int64 i) const;

		[[nodiscard]]
		BigFloat operator /(uint64 i) const;

		[[nodiscard]]
		BigFloat operator /(Concept::SignedIntegral auto  i) const;

		[[nodiscard]]
		BigFloat operator /(Concept::UnsignedIntegral auto i) const;

		[[nodiscard]]
		BigFloat operator /(long double f) const;

		[[nodiscard]]
		BigFloat operator /(Concept::FloatingPoint auto f) const;

		[[nodiscard]]
		BigFloat operator /(const BigInt& i) const;

		[[nodiscard]]
		BigFloat operator /(const BigFloat& f) const;

		[[nodiscard]]
		friend BigFloat operator /(const Concept::Arithmetic auto a, const BigFloat& b)
		{
			return (BigFloat{ a } / b);
		}

		[[nodiscard]]
		friend BigFloat operator /(const BigInt& a, const BigFloat& b)
		{
			return (BigFloat{ a } / b);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator +=
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator +=(int64 i);

		BigFloat& operator +=(uint64 i);

		BigFloat& operator +=(Concept::SignedIntegral auto i);

		BigFloat& operator +=(Concept::UnsignedIntegral auto i);

		BigFloat& operator +=(long double f);

		BigFloat& operator +=(Concept::FloatingPoint auto f);

		BigFloat& operator +=(const BigInt& i);

		BigFloat& operator +=(const BigFloat& f);

		////////////////////////////////////////////////////////////////
		//
		//	operator -=
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator -=(int64 i);

		BigFloat& operator -=(uint64 i);

		BigFloat& operator -=(Concept::SignedIntegral auto i);

		BigFloat& operator -=(Concept::UnsignedIntegral auto i);

		BigFloat& operator -=(long double f);

		BigFloat& operator -=(Concept::FloatingPoint auto f);

		BigFloat& operator -=(const BigInt& i);

		BigFloat& operator -=(const BigFloat& f);

		////////////////////////////////////////////////////////////////
		//
		//	operator *=
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator *=(int64 i);

		BigFloat& operator *=(uint64 i);

		BigFloat& operator *=(Concept::SignedIntegral auto i);

		BigFloat& operator *=(Concept::UnsignedIntegral auto i);

		BigFloat& operator *=(long double f);

		BigFloat& operator *=(Concept::FloatingPoint auto f);

		BigFloat& operator *=(const BigInt& i);

		BigFloat& operator *=(const BigFloat& f);

		////////////////////////////////////////////////////////////////
		//
		//	operator /=
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator /=(int64 i);

		BigFloat& operator /=(uint64 i);

		BigFloat& operator /=(Concept::SignedIntegral auto i);

		BigFloat& operator /=(Concept::UnsignedIntegral auto i);

		BigFloat& operator /=(long double f);

		BigFloat& operator /=(Concept::FloatingPoint auto f);

		BigFloat& operator /=(const BigInt& i);

		BigFloat& operator /=(const BigFloat& f);

		////////////////////////////////////////////////////////////////
		//
		//	operator ++
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator ++();

		BigFloat operator ++(int);

		////////////////////////////////////////////////////////////////
		//
		//	operator --
		//
		////////////////////////////////////////////////////////////////

		BigFloat& operator --();

		BigFloat operator --(int);

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend bool operator ==(const BigFloat& a, const BigFloat& b) noexcept
		{
			return (a.compare(b) == 0);
		}

		[[nodiscard]]
		friend bool operator ==(const BigFloat& a, const BigInt& b) noexcept
		{
			return (a.compare(b) == 0);
		}

		[[nodiscard]]
		friend bool operator ==(const BigFloat& a, const Concept::Arithmetic auto b) noexcept
		{
			return (a.compare(b) == 0);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend auto operator <=>(const BigFloat& a, const BigFloat& b) noexcept
		{
			return (a.compare(b) <=> 0);
		}

		[[nodiscard]]
		friend auto operator <=>(const BigFloat& a, const BigInt& b) noexcept
		{
			return (a.compare(b) <=> 0);
		}

		[[nodiscard]]
		friend auto operator <=>(const BigFloat& a, const Concept::Arithmetic auto b) noexcept
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
		//	isNaN
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInf
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isInf() const noexcept;

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
		BigFloat abs() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		void swap(BigFloat& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asFloat, asDouble, asLongDouble
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		float asFloat() const;

		[[nodiscard]]
		double asDouble() const;

		[[nodiscard]]
		long double asLongDouble() const;

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
		//	to_string
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		std::string to_string(int32 digits = 100, std::ios_base::fmtflags fmtFlags = std::ios_base::fixed) const;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		String str(int32 digits = 100, std::ios_base::fmtflags fmtFlags = std::ios_base::fixed) const;

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
		int32 compare(Concept::SignedIntegral auto i) const noexcept;

		[[nodiscard]]
		int32 compare(Concept::UnsignedIntegral auto i) const noexcept;

		[[nodiscard]]
		int32 compare(long double f) const noexcept;

		[[nodiscard]]
		int32 compare(Concept::FloatingPoint auto f) const noexcept;

		[[nodiscard]]
		int32 compare(const BigInt& i) const;
		
		[[nodiscard]]
		int32 compare(const BigFloat& f) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		friend void swap(BigFloat& lhs, BigFloat& rhs) noexcept
		{
			lhs.swap(rhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const BigFloat& value)
		{
			return (output << value.str());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, BigFloat& value)
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

		friend void Formatter(FormatData& formatData, const BigFloat& value);

	private:

		struct BigFloatDetail;

		std::unique_ptr<BigFloatDetail> pImpl;

	public:

		////////////////////////////////////////////////////////////////
		//
		//	_detail (internal)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		BigFloatDetail& _detail() noexcept;

		[[nodiscard]]
		const BigFloatDetail& _detail() const noexcept;
	};

	inline namespace Literals
	{
		inline namespace BigNumLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_bigF
			//
			////////////////////////////////////////////////////////////////

			[[nodiscard]]
			BigFloat operator ""_bigF(const char* s);
		}
	}
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::BigFloat>
{
	[[nodiscard]]
	size_t operator ()(const s3d::BigFloat& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/BigFloat.ipp"
