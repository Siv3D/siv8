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
# include <memory>
# include <ios>
# include "Common.hpp"
# include "Concepts.hpp"
# include "String.hpp"

namespace s3d
{
	class BigInt;
	struct FormatData;

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

		/// @brief 0 で初期化します。
		[[nodiscard]]
		BigFloat();

		/// @brief 別の BigFloat から初期化します。
		/// @param other 初期化に使う BigFloat
		[[nodiscard]]
		BigFloat(const BigFloat& other);

		/// @brief 別の BigFloat から初期化します。
		/// @param other 初期化に使う BigFloat
		[[nodiscard]]
		BigFloat(BigFloat&& other) noexcept = default;

		/// @brief 整数から初期化します。
		/// @param i 初期化に使う整数
		[[nodiscard]]
		BigFloat(Concept::SignedIntegral auto i);

		/// @brief 整数から初期化します。
		/// @param i 初期化に使う整数
		[[nodiscard]]
		BigFloat(Concept::UnsignedIntegral auto i);

		/// @brief 整数から初期化します。
		/// @param i 初期化に使う整数
		[[nodiscard]]
		BigFloat(int64 i);

		/// @brief 整数から初期化します。
		/// @param i 初期化に使う整数
		[[nodiscard]]
		BigFloat(uint64 i);
	
		/// @brief 浮動小数点数から初期化します。
		/// @param f 初期化に使う浮動小数点数
		[[nodiscard]]
		BigFloat(long double f);

		/// @brief 浮動小数点数から初期化します。
		/// @param f 初期化に使う浮動小数点数
		[[nodiscard]]
		BigFloat(Concept::FloatingPoint auto f);
		
		/// @brief 多倍長整数から初期化します。
		/// @param i 初期化に使う多倍長整数
		[[nodiscard]]
		BigFloat(const BigInt& i);
		
		/// @brief 文字列から初期化します。
		/// @param number 初期化に使う文字列
		[[nodiscard]]
		explicit BigFloat(std::string_view number);
		
		/// @brief 文字列から初期化します。
		/// @param number 初期化に使う文字列
		[[nodiscard]]
		explicit BigFloat(StringView number);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
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

		/// @brief 0 でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief 0 であるかを返します。
		/// @return 0 である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief NaN であるかを返します。
		/// @return NaN である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInf
		//
		////////////////////////////////////////////////////////////////

		/// @brief 無限大であるかを返します。
		/// @return 無限大である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isInf() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	sign
		//
		////////////////////////////////////////////////////////////////

		/// @brief 符号を返します。
		/// @return 符号が正の場合 1, 負の場合 -1, それ以外の場合は 0
		[[nodiscard]]
		int32 sign() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	abs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 絶対値を返します。
		/// @return 絶対値
		[[nodiscard]]
		BigFloat abs() const;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの BigFloat の値を交換します。
		/// @param other もう一方の BigFloat
		void swap(BigFloat& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	hash
		//
		////////////////////////////////////////////////////////////////

		/// @brief ハッシュ値を返します。
		/// @return ハッシュ値
		[[nodiscard]]
		uint64 hash() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asFloat, asDouble, asLongDouble
		//
		////////////////////////////////////////////////////////////////

		/// @brief float に変換します。
		/// @return float に変換した値
		[[nodiscard]]
		float asFloat() const;

		/// @brief double に変換します。
		/// @return double に変換した値
		[[nodiscard]]
		double asDouble() const;

		/// @brief long double に変換します。
		/// @return long double に変換した値
		[[nodiscard]]
		long double asLongDouble() const;

		////////////////////////////////////////////////////////////////
		//
		//	operator float, operator double
		//
		////////////////////////////////////////////////////////////////

		/// @brief float に変換します。
		/// @return float に変換した値
		[[nodiscard]]
		explicit operator float() const noexcept;

		/// @brief double に変換します。
		/// @return double に変換した値
		[[nodiscard]]
		explicit operator double() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	to_string
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列に変換します。
		/// @param digits 桁数
		/// @param fmtFlags 書式指定
		/// @return 変換した文字列
		[[nodiscard]]
		std::string to_string(int32 digits = 100, std::ios_base::fmtflags fmtFlags = std::ios_base::fixed) const;

		////////////////////////////////////////////////////////////////
		//
		//	str
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列に変換します。
		/// @param digits 桁数
		/// @param fmtFlags 書式指定
		/// @return 変換した文字列
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
		
		/// @brief 多倍長浮動小数点数を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value 多倍長浮動小数点数
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
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
