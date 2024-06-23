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
# include "Common.hpp"
# include "Concepts.hpp"
# include "SIMDMath.hpp"
# include "Hash.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	HalfFloat
	//
	////////////////////////////////////////////////////////////////

	/// @brief 半精度浮動小数点数
	class HalfFloat
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		HalfFloat() = default;

		/// @brief float 型の値から半精度浮動小数点数を作成します。
		/// @param value float 型の値
		[[nodiscard]]
		HalfFloat(float value) noexcept;

		/// @brief 与えられた数値から半精度浮動小数点数を作成します。
		/// @param value 数値
		[[nodiscard]]
		HalfFloat(Concept::Arithmetic auto value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator =
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値を代入します。
		/// @param value 代入する値
		/// @return *this
		HalfFloat& operator =(float value) noexcept;

		/// @brief 値を代入します。
		/// @param value 代入する値
		/// @return *this
		HalfFloat& operator =(Concept::Arithmetic auto value);

		////////////////////////////////////////////////////////////////
		//
		//	operator +
		//
		////////////////////////////////////////////////////////////////

		constexpr HalfFloat operator +() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator -
		//
		////////////////////////////////////////////////////////////////

		constexpr HalfFloat operator -() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	asFloat
		//
		////////////////////////////////////////////////////////////////

		/// @brief float 型に変換した値を返します。
		/// @return float 型に変換した値
		[[nodiscard]]
		float asFloat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator float
		//
		////////////////////////////////////////////////////////////////

		/// @brief float 型に変換した値を返します。
		/// @return float 型に変換した値
		[[nodiscard]]
		operator float() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getBits
		//
		////////////////////////////////////////////////////////////////

		/// @brief ビットパターンを返します。
		/// @return ビットパターン
		[[nodiscard]]
		constexpr uint16 getBits() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	setBits
		//
		////////////////////////////////////////////////////////////////

		/// @brief ビットパターンを設定します。
		/// @param bits ビットパターン
		constexpr void setBits(const uint16 bits) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isZero
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が 0 であるかを返します。
		/// @return 0 である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isZero() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isNaN
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が NaN であるかを返します。
		/// @return NaN である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isNaN() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isInfinity
		//
		////////////////////////////////////////////////////////////////

		/// @brief 値が無限大であるかを返します。
		/// @return 無限大である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool isInfinity() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	signBit
		//
		////////////////////////////////////////////////////////////////

		/// @brief 符号ビットを返します。
		/// @return 符号ビット。正の値の場合は false, 負の値の場合は true
		[[nodiscard]]
		constexpr bool signBit() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	abs
		//
		////////////////////////////////////////////////////////////////

		/// @brief 絶対値を返します。
		/// @return 絶対値
		[[nodiscard]]
		constexpr HalfFloat abs() const noexcept;

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
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの値が等しいかを返します。
		/// @param lhs 一方の値
		/// @param rhs もう一方の値
		/// @return 2 つの値が等しい場合 true, それ以外の場合は false
		[[nodiscard]]
		friend constexpr bool operator ==(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			if (IsZero(lhs.m_bits) && IsZero(rhs.m_bits))
			{
				return true;
			}

			if (IsNaN(lhs.m_bits) || IsNaN(rhs.m_bits))
			{
				return false;
			}

			return (lhs.m_bits == rhs.m_bits);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator >(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			if (IsZero(lhs.m_bits) && IsZero(rhs.m_bits))
			{
				return false;
			}

			if (IsNaN(lhs.m_bits) || IsNaN(rhs.m_bits))
			{
				return false;
			}

			if ((lhs.m_bits & HalfFloat::SignMask) == 0)
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == HalfFloat::SignMask)
				{
					return true;
				}
				else
				{
					return (lhs.m_bits > rhs.m_bits);
				}
			}
			else
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == 0)
				{
					return false;
				}
				else
				{
					return ((lhs.m_bits & 0x7FFF) < (rhs.m_bits & 0x7FFF));
				}
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >=
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator >=(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			if (IsZero(lhs.m_bits) && IsZero(rhs.m_bits))
			{
				return true;
			}

			if (IsNaN(lhs.m_bits) || IsNaN(rhs.m_bits))
			{
				return false;
			}
			
			if ((lhs.m_bits & HalfFloat::SignMask) == 0)
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == HalfFloat::SignMask)
				{
					return true;
				}
				else
				{
					return (lhs.m_bits >= rhs.m_bits);
				}
			}
			else
			{
				if ((rhs.m_bits & HalfFloat::SignMask) == 0)
				{
					return false;
				}
				else
				{
					return ((lhs.m_bits & 0x7FFF) <= (rhs.m_bits & 0x7FFF));
				}
			}
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator <(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			return (rhs > lhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <=
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator <=(const HalfFloat lhs, const HalfFloat rhs) noexcept
		{
			return (rhs >= lhs);
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <<
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_ostream<CharType>& operator <<(std::basic_ostream<CharType>& output, const HalfFloat& value)
		{
			return (output << value.asFloat());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator >>
		//
		////////////////////////////////////////////////////////////////

		template <class CharType>
		friend std::basic_istream<CharType>& operator >>(std::basic_istream<CharType>& input, HalfFloat& value)
		{
			float t = 0.0f;

			if (input >> t)
			{
				value = t;
			}

			return input;
		}

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const HalfFloat& value);

	private:

		static constexpr uint16 SignMask = 0x8000;

		uint16 m_bits = 0;

		[[nodiscard]]
		static constexpr bool IsZero(uint16 bits) noexcept;

		[[nodiscard]]
		static constexpr bool IsNaN(uint16 bits) noexcept;
	};
}

////////////////////////////////////////////////////////////////
//
//	std::hash
//
////////////////////////////////////////////////////////////////

template <>
struct std::hash<s3d::HalfFloat>
{
	[[nodiscard]]
	size_t operator ()(const s3d::HalfFloat& value) const noexcept
	{
		return value.hash();
	}
};

# include "detail/HalfFloat.ipp"
